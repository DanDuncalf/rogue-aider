#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x;
    int y;
    int hp;
    int gold;
    // Add more player attributes as needed
} PlayerState;

void init_player(PlayerState *player);
void free_player(PlayerState *player);

#endif // PLAYER_H
