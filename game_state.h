#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "map.h"
#include "player.h"
#include "monster.h"
#include "items.h"

typedef struct GameState {
    MapState map;
    PlayerState player;
    MonsterState *monsters;
    int monster_count;
    ItemsState items;
    int depth;
    int turn;
    int game_over;  /* 0 = running, 1 = dead, 2 = won */
    char message[160];
} GameState;

GameState *init_game_state(void);
void free_game_state(GameState *state);
/* arrive: 0 = random spot, 1 = place on '<' (descended), 2 = place on '>' (ascended) */
void new_level(GameState *state, int depth, int arrive);
void set_message(GameState *state, const char *fmt, ...);

#endif // GAME_STATE_H
