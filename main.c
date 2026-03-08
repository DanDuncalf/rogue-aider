#include <ncurses.h>
#include <signal.h>
#include <stdlib.h>

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

    // Main game loop
    while (1) {
        // Game logic goes here
        refresh();
        usleep(100000);  // Sleep for 100ms to control frame rate
    }

    // Cleanup ncurses
    endwin();

    return 0;
}
