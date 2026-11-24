#include "player.h"
#include "game_state.h"
#include "map.h"
#include "monster.h"
#include "items.h"
#include <stdlib.h>

void init_player(PlayerState *player) {
    player->x = 40;
    player->y = 12;
    player->hp = 30;
    player->max_hp = 30;
    player->gold = 0;
    player->base_damage = 2;
}

void free_player(PlayerState *player) {
    // No dynamic memory to free for PlayerState
    (void)player;  // Use the parameter to avoid unused warning
}

int player_move(struct GameState *state, int dx, int dy) {
    int nx = state->player.x + dx;
    int ny = state->player.y + dy;

    int target = monster_at(state, nx, ny);
    if (target >= 0) {
        player_attack_monster(state, target);
        return 1;
    }

    if (!map_is_walkable(&state->map, nx, ny)) {
        return 0;  /* walking into a wall costs no turn */
    }

    state->player.x = nx;
    state->player.y = ny;

    item_collect_gold(state);
    if (item_under_player(state)) {
        set_message(state, "You see something here. Press 'g' to pick it up.");
    }
    return 1;
}
