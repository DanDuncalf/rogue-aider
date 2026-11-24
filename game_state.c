#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "game_state.h"
#include "map.h"
#include "player.h"
#include "monster.h"
#include "items.h"

#define MAX_DEPTH 10

GameState *init_game_state(void) {
    GameState *state = malloc(sizeof(GameState));
    if (!state) {
        perror("Failed to allocate memory for GameState");
        exit(EXIT_FAILURE);
    }

    state->monsters = NULL;
    state->monster_count = 0;
    state->depth = 0;
    state->turn = 0;
    state->game_over = 0;
    state->message[0] = '\0';

    init_map(&state->map);
    init_player(&state->player);
    init_items(&state->items);

    new_level(state, 1, 0);
    set_message(state, "Find the Amulet of Yendor on level %d and escape!", MAX_DEPTH);

    return state;
}

void free_game_state(GameState *state) {
    if (!state) return;

    free_map(&state->map);
    free_player(&state->player);
    for (int i = 0; i < state->monster_count; i++) {
        free_monster(&state->monsters[i]);
    }
    free(state->monsters);
    free_items(&state->items);

    free(state);
}

void new_level(GameState *state, int depth, int arrive) {
    state->depth = depth;

    map_generate(&state->map, 1, depth < MAX_DEPTH);
    clear_ground_items(&state->items);
    spawn_items(state, depth);

    /* Place the player. */
    int placed = 0;
    if (arrive == 1 && state->map.stair_up_x >= 0) {
        state->player.x = state->map.stair_up_x;
        state->player.y = state->map.stair_up_y;
        placed = 1;
    } else if (arrive == 2 && state->map.stair_down_x >= 0) {
        state->player.x = state->map.stair_down_x;
        state->player.y = state->map.stair_down_y;
        placed = 1;
    }
    if (!placed) {
        int x, y;
        if (map_random_floor(&state->map, &x, &y)) {
            state->player.x = x;
            state->player.y = y;
        }
    }

    spawn_monsters(state);
    map_compute_fov(&state->map, state->player.x, state->player.y);
}

void set_message(GameState *state, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(state->message, sizeof(state->message), fmt, args);
    va_end(args);
}
