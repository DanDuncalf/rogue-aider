#include "render.h"
#include "items.h"
#include "map.h"
#include <ncurses.h>

void render_init(void) {
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(CP_FLOOR, COLOR_WHITE, -1);
        init_pair(CP_ITEM, COLOR_YELLOW, -1);
        init_pair(CP_MONSTER, COLOR_RED, -1);
        init_pair(CP_PLAYER, COLOR_GREEN, -1);
        init_pair(CP_STAIRS, COLOR_CYAN, -1);
    }
}

static void draw_map(GameState *state) {
    MapState *map = &state->map;
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (map->visible[y][x]) {
                char t = map->tiles[y][x];
                if (t == '<' || t == '>') {
                    attron(COLOR_PAIR(CP_STAIRS) | A_BOLD);
                    mvaddch(y, x, t);
                    attroff(COLOR_PAIR(CP_STAIRS) | A_BOLD);
                } else if (t == '#') {
                    attron(COLOR_PAIR(CP_FLOOR) | A_BOLD);
                    mvaddch(y, x, t);
                    attroff(COLOR_PAIR(CP_FLOOR) | A_BOLD);
                } else {
                    attron(COLOR_PAIR(CP_FLOOR));
                    mvaddch(y, x, t);
                    attroff(COLOR_PAIR(CP_FLOOR));
                }
            } else if (map->explored[y][x]) {
                attron(A_DIM);
                mvaddch(y, x, map->tiles[y][x]);
                attroff(A_DIM);
            } else {
                mvaddch(y, x, ' ');
            }
        }
    }
}

static void draw_items(GameState *state) {
    attron(COLOR_PAIR(CP_ITEM) | A_BOLD);
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) {
        Item *it = &state->items.ground[i];
        if (it->active && state->map.visible[it->y][it->x]) {
            mvaddch(it->y, it->x, it->glyph);
        }
    }
    attroff(COLOR_PAIR(CP_ITEM) | A_BOLD);
}

static void draw_monsters(GameState *state) {
    attron(COLOR_PAIR(CP_MONSTER) | A_BOLD);
    for (int i = 0; i < state->monster_count; i++) {
        MonsterState *m = &state->monsters[i];
        if (m->alive && state->map.visible[m->y][m->x]) {
            mvaddch(m->y, m->x, m->glyph);
        }
    }
    attroff(COLOR_PAIR(CP_MONSTER) | A_BOLD);
}

void render_game(GameState *state) {
    erase();

    draw_map(state);
    draw_items(state);
    draw_monsters(state);

    attron(COLOR_PAIR(CP_PLAYER) | A_BOLD);
    mvaddch(state->player.y, state->player.x, '@');
    attroff(COLOR_PAIR(CP_PLAYER) | A_BOLD);

    mvprintw(MAP_HEIGHT, 0, "%s", state->message);
    clrtoeol();

    mvprintw(MAP_HEIGHT + 1, 0,
             "HP: %d/%d  Gold: %d  Level: %d  Weapon: %s%s",
             state->player.hp, state->player.max_hp,
             state->player.gold, state->depth,
             equipped_weapon_name(state),
             player_has_amulet(state) ? "  [Amulet]" : "");
    clrtoeol();

    attron(A_DIM);
    mvprintw(MAP_HEIGHT + 2, 0,
             "move: arrows/hjkl/ybn  >/<: stairs  g: get  u/i: use  d: drop  q: quit");
    attroff(A_DIM);

    refresh();
}

void render_end_screen(GameState *state) {
    erase();
    if (state->game_over == 2) {
        mvprintw(9, 28, "You escaped with the Amulet of Yendor!");
        mvprintw(11, 34, "YOU WIN!");
        mvprintw(13, 26, "Gold: %d   Turns: %d", state->player.gold, state->turn);
    } else {
        mvprintw(10, 36, "You die...");
        mvprintw(12, 24, "Level: %d   Gold: %d   Turns: %d",
                 state->depth, state->player.gold, state->turn);
    }
    mvprintw(15, 30, "Play again? [y/n]");
    refresh();
}
