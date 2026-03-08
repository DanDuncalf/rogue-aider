#include "render.h"
#include <ncurses.h>

void render_game(GameState *state) {
    clear();

    // Render map
    for (int y = 0; y < state->map.height; y++) {
        for (int x = 0; x < state->map.width; x++) {
            mvaddch(y, x, state->map.tiles[y][x]);
        }
    }

    // Render player
    mvaddch(state->player.y, state->player.x, '@');

    // Render monsters
    for (int i = 0; i < 5; i++) {
        mvaddch(state->monsters[i].y, state->monsters[i].x, 'M');
    }

    // Render status bar
    mvprintw(24, 0, "HP: %d Gold: %d Level: 1 Weapon: None", state->player.hp, state->player.gold);

    refresh();
}
