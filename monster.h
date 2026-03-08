#ifndef MONSTER_H
#define MONSTER_H

typedef struct {
    int x;
    int y;
    int hp;
    // Add more monster attributes as needed
} MonsterState;

void init_monster(MonsterState *monster);
void free_monster(MonsterState *monster);

#endif // MONSTER_H
