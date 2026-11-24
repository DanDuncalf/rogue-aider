#include "monster.h"
#include "game_state.h"
#include "map.h"
#include "items.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_MONSTERS 30
#define AGGRO_RANGE 10

static const struct {
    char glyph;
    const char *name;
    int hp;
    int damage;
    int speed;
    int slow;
} monster_types[MON_TYPE_COUNT] = {
    {'r', "rat",    4,  2,  2, 0},
    {'g', "goblin", 8,  3,  1, 0},
    {'o', "orc",    15, 5,  1, 0},
    {'T', "troll",  26, 8,  1, 1},
    {'D', "dragon", 45, 12, 1, 0},
};

void init_monster(MonsterState *monster, MonsterType type, int x, int y, int depth) {
    monster->x = x;
    monster->y = y;
    monster->type = type;
    monster->glyph = monster_types[type].glyph;
    monster->name = monster_types[type].name;
    monster->max_hp = monster_types[type].hp + depth;
    monster->hp = monster->max_hp;
    monster->damage = monster_types[type].damage + depth / 3;
    monster->speed = monster_types[type].speed;
    monster->slow = monster_types[type].slow;
    monster->alive = 1;
}

void free_monster(MonsterState *monster) {
    // No dynamic memory to free for a single MonsterState
    (void)monster;
}

int monster_at(struct GameState *state, int x, int y) {
    for (int i = 0; i < state->monster_count; i++) {
        MonsterState *m = &state->monsters[i];
        if (m->alive && m->x == x && m->y == y) return i;
    }
    return -1;
}

void player_attack_monster(struct GameState *state, int index) {
    MonsterState *m = &state->monsters[index];
    int dmg = player_attack_damage(state) + rand() % 3;
    m->hp -= dmg;
    if (m->hp <= 0) {
        m->alive = 0;
        int reward = 2 + (int)m->type * 3 + rand() % (state->depth + 3);
        state->player.gold += reward;
        set_message(state, "You kill the %s! (+%d gold)", m->name, reward);
    } else {
        set_message(state, "You hit the %s for %d damage.", m->name, dmg);
    }
}

static int max_type_for_depth(int depth) {
    if (depth <= 2) return MON_GOBLIN;
    if (depth <= 4) return MON_ORC;
    if (depth <= 7) return MON_TROLL;
    return MON_DRAGON;
}

void spawn_monsters(struct GameState *state) {
    free(state->monsters);
    state->monsters = NULL;

    int count = 4 + state->depth;
    if (count > MAX_MONSTERS) count = MAX_MONSTERS;

    state->monsters = malloc((size_t)count * sizeof(MonsterState));
    if (!state->monsters) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    state->monster_count = count;

    int max_type = max_type_for_depth(state->depth);
    for (int i = 0; i < count; i++) {
        MonsterType type = (MonsterType)(rand() % (max_type + 1));
        int x = 0, y = 0;
        int placed = 0;
        for (int tries = 0; tries < 200 && !placed; tries++) {
            if (!map_random_floor(&state->map, &x, &y)) break;
            int dx = x - state->player.x;
            int dy = y - state->player.y;
            if (dx * dx + dy * dy <= 36) continue;  /* not on top of the player */
            if (monster_at(state, x, y) >= 0) continue;
            placed = 1;
        }
        if (!placed) {
            /* Give up on this slot: park it dead far from view. */
            init_monster(&state->monsters[i], type, 0, 0, state->depth);
            state->monsters[i].alive = 0;
            continue;
        }
        init_monster(&state->monsters[i], type, x, y, state->depth);
    }
}

/* Bresenham line of sight over transparent tiles. */
static int has_los(struct GameState *state, int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int x = x1, y = y1;

    while (x != x2 || y != y2) {
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
        if (x == x2 && y == y2) break;
        if (!map_transparent(&state->map, x, y)) return 0;
    }
    return 1;
}

static void monster_attack_player(struct GameState *state, MonsterState *m) {
    int dmg = m->damage + rand() % 3;
    state->player.hp -= dmg;
    if (state->player.hp <= 0) {
        state->player.hp = 0;
        state->game_over = 1;
        set_message(state, "The %s hits you for %d damage. You die...", m->name, dmg);
    } else {
        set_message(state, "The %s hits you for %d damage.", m->name, dmg);
    }
}

static void monster_step(struct GameState *state, MonsterState *m) {
    int px = state->player.x;
    int py = state->player.y;
    int dx = px - m->x;
    int dy = py - m->y;

    /* Adjacent: attack. */
    if (abs(dx) <= 1 && abs(dy) <= 1) {
        monster_attack_player(state, m);
        return;
    }

    int chase = (dx * dx + dy * dy <= AGGRO_RANGE * AGGRO_RANGE)
                && has_los(state, m->x, m->y, px, py);

    int step_x = 0, step_y = 0;
    if (chase) {
        step_x = (dx > 0) - (dx < 0);
        step_y = (dy > 0) - (dy < 0);
    } else if (rand() % 4 == 0) {
        /* Wander. */
        step_x = rand() % 3 - 1;
        step_y = rand() % 3 - 1;
    } else {
        return;  /* idle */
    }

    /* Try the direct step, then fall back to axis-aligned moves. */
    static const int fallbacks[3][2] = {{0, 0}, {0, 1}, {1, 0}};
    for (int i = 0; i < 3; i++) {
        int mx = fallbacks[i][0] ? 0 : step_x;
        int my = fallbacks[i][1] ? 0 : step_y;
        if (i > 0 && !chase) break;  /* wanderers get one try */
        if (mx == 0 && my == 0) continue;
        int nx = m->x + mx;
        int ny = m->y + my;
        if (nx == px && ny == py) continue;  /* handled as attack above */
        if (!map_is_walkable(&state->map, nx, ny)) continue;
        if (monster_at(state, nx, ny) >= 0) continue;
        m->x = nx;
        m->y = ny;
        return;
    }
}

void monsters_take_turn(struct GameState *state) {
    for (int i = 0; i < state->monster_count; i++) {
        MonsterState *m = &state->monsters[i];
        if (!m->alive) continue;
        if (m->slow && (state->turn % 2 != 0)) continue;

        int moves = m->speed > 0 ? m->speed : 1;
        for (int s = 0; s < moves; s++) {
            monster_step(state, m);
            if (state->game_over) return;
        }
    }
}
