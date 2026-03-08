#include "monster.h"
#include <stdlib.h>

void init_monster(MonsterState *monster) {
    monster->x = 0;
    monster->y = 0;
    monster->hp = 20;
}

void free_monster(MonsterState *monster) {
    // No dynamic memory to free for MonsterState
    (void)monster;  // Use the parameter to avoid unused warning
}
