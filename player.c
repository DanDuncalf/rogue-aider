#include "player.h"
#include <stdlib.h>

void init_player(PlayerState *player) {
    player->x = 40;
    player->y = 12;
    player->hp = 100;
    player->gold = 0;
}

void free_player(PlayerState *player) {
    // No dynamic memory to free for PlayerState
    (void)player;  // Use the parameter to avoid unused warning
}
