#include "game_state.h"
#include "map.h"
#include "player.h"
#include "monster.h"
#include "items.h"

GameState *init_game_state() {
    GameState *state = malloc(sizeof(GameState));
    if (!state) {
        perror("Failed to allocate memory for GameState");
        exit(EXIT_FAILURE);
    }

    // Initialize map, player, monsters, and items
    init_map(&state->map);
    init_player(&state->player);
    for (int i = 0; i < 5; i++) {
        init_monster(&state->monsters[i]);
    }
    init_items(&state->items);

    return state;
}

void free_game_state(GameState *state) {
    if (!state) return;

    // Free map, player, monsters, and items
    free_map(&state->map);
    free_player(&state->player);
    for (int i = 0; i < 5; i++) {
        free_monster(&state->monsters[i]);
    }
    free_items(&state->items);

    free(state);
}
