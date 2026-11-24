#ifndef RENDER_H
#define RENDER_H

#include "game_state.h"

/* Color pair ids (initialized by render_init). */
#define CP_FLOOR 1
#define CP_ITEM 2
#define CP_MONSTER 3
#define CP_PLAYER 4
#define CP_STAIRS 5

void render_init(void);
void render_game(GameState *state);
void render_end_screen(GameState *state);

#endif // RENDER_H
