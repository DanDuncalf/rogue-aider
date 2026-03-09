#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>
#include "game_state.h"
#include "render.h"

void cleanup(int sig) {
    endwin();
    exit(0);
}

int main() {
    // Initialize ncurses
    initscr();
    raw();  // Line buffering disabled
    noecho();  // Don't echo input characters
    keypad(stdscr, TRUE);  // Enable special keys like arrow keys

    // Handle signals to ensure clean termination
    signal(SIGINT, cleanup);

    // Initialize game state
    GameState *state = init_game_state();

    // Main game loop
    while (1) {
        // Render the game
        render_game(state);

        // Game logic goes here
        refresh();
        usleep(100000);  // Sleep for 100ms to control frame rate
    }

    // Cleanup game state
    free_game_state(state);

    // Cleanup ncurses
    endwin();

    return 0;
}
