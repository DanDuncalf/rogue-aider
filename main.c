#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_state.h"
#include "render.h"
#include "player.h"
#include "monster.h"
#include "items.h"
#include "map.h"

static volatile sig_atomic_t quit_requested = 0;

static void on_sigint(int sig) {
    (void)sig;
    quit_requested = 1;  /* checked in the main loop; endwin() runs there */
}

/* Inventory overlay. mode: 0 = use, 1 = drop. Returns slot or -1. */
static int inventory_menu(GameState *state, int mode) {
    for (;;) {
        erase();
        mvprintw(0, 0, mode == 0 ? "Use which item?" : "Drop which item?");
        int n = 0;
        for (int i = 0; i < MAX_INVENTORY; i++) {
            Item *it = &state->items.inventory[i];
            if (!it->active) continue;
            mvprintw(2 + n, 2, "%c) %s%s", 'a' + i, it->name,
                     i == state->items.weapon_slot ? " (equipped)" : "");
            n++;
        }
        if (n == 0) {
            mvprintw(2, 2, "(empty)");
        }
        mvprintw(2 + n + 1, 0, "Press a letter, or ESC to cancel.");
        refresh();

        int ch = getch();
        if (quit_requested) return -1;
        if (ch == 27 || ch == ' ' || ch == 'q') return -1;
        if (ch >= 'a' && ch < 'a' + MAX_INVENTORY) {
            int slot = ch - 'a';
            if (state->items.inventory[slot].active) return slot;
        }
    }
}

static int confirm_quit(void) {
    mvprintw(MAP_HEIGHT, 0, "Really quit? [y/n]");
    clrtoeol();
    refresh();
    int ch = getch();
    return ch == 'y' || ch == 'Y';
}

static void handle_stairs(GameState *state, int down) {
    char tile = state->map.tiles[state->player.y][state->player.x];
    if (down) {
        if (tile != '>') {
            set_message(state, "There are no stairs down here.");
            return;
        }
        new_level(state, state->depth + 1, 1);
        set_message(state, "You descend to level %d.", state->depth);
    } else {
        if (tile != '<') {
            set_message(state, "There are no stairs up here.");
            return;
        }
        if (state->depth == 1) {
            if (player_has_amulet(state)) {
                state->game_over = 2;
            } else {
                set_message(state, "You can't leave without the Amulet of Yendor!");
            }
            return;
        }
        new_level(state, state->depth - 1, 2);
        set_message(state, "You climb to level %d.", state->depth);
    }
}

static void run_game(GameState *state) {
    while (!quit_requested && !state->game_over) {
        map_compute_fov(&state->map, state->player.x, state->player.y);
        render_game(state);

        int ch = getch();
        if (quit_requested) break;
        if (ch == ERR) continue;

        int consumed = 0;
        int dx = 0, dy = 0;
        switch (ch) {
            case KEY_UP:    case 'k': dy = -1; break;
            case KEY_DOWN:  case 'j': dy = 1;  break;
            case KEY_LEFT:  case 'h': dx = -1; break;
            case KEY_RIGHT: case 'l': dx = 1;  break;
            case 'y': dx = -1; dy = -1; break;
            case 'b': dx = -1; dy = 1;  break;
            case 'n': dx = 1;  dy = 1;  break;
            case '>':
                handle_stairs(state, 1);
                break;
            case '<':
                handle_stairs(state, 0);
                break;
            case 'g':
                consumed = item_pickup(state);
                break;
            case 'i':
            case 'u': {
                int slot = inventory_menu(state, 0);
                if (slot >= 0) consumed = item_use(state, slot);
                break;
            }
            case 'd': {
                int slot = inventory_menu(state, 1);
                if (slot >= 0) consumed = item_drop(state, slot);
                break;
            }
            case 'q':
            case 'Q':
                if (confirm_quit()) return;
                break;
            default:
                break;
        }

        if (dx != 0 || dy != 0) {
            consumed = player_move(state, dx, dy);
        }

        if (consumed && !state->game_over) {
            state->turn++;
            monsters_take_turn(state);
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    signal(SIGINT, on_sigint);

    initscr();
    raw();              // Line buffering disabled
    noecho();           // Don't echo input characters
    keypad(stdscr, TRUE);  // Enable special keys like arrow keys
    curs_set(0);
    render_init();

    if (COLS < MAP_WIDTH || LINES < MAP_HEIGHT + 3) {
        endwin();
        fprintf(stderr, "Terminal too small: need at least %dx%d.\n",
                MAP_WIDTH, MAP_HEIGHT + 3);
        return EXIT_FAILURE;
    }

    for (;;) {
        GameState *state = init_game_state();

        run_game(state);

        int play_again = 0;
        if (state->game_over && !quit_requested) {
            render_end_screen(state);
            int ch = getch();
            play_again = (ch == 'y' || ch == 'Y');
        }

        free_game_state(state);

        if (!play_again || quit_requested) break;
    }

    endwin();

    return 0;
}
