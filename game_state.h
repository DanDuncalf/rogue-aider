#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "map.h"
#include "player.h"
#include "monster.h"
#include "items.h"

typedef struct {
    MapState map;
    PlayerState player;
    MonsterState monsters[5];
    ItemsState items;
} GameState;

GameState *init_game_state();
void free_game_state(GameState *state);

#endif // GAME_STATE_H
