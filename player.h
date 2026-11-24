#ifndef PLAYER_H
#define PLAYER_H

struct GameState;

typedef struct {
    int x;
    int y;
    int hp;
    int max_hp;
    int gold;
    int base_damage;
} PlayerState;

void init_player(PlayerState *player);
void free_player(PlayerState *player);

/* Attempt a move (or bump attack). Returns 1 if the action consumed a turn. */
int player_move(struct GameState *state, int dx, int dy);

#endif // PLAYER_H
