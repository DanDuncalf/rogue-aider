#ifndef MONSTER_H
#define MONSTER_H

struct GameState;

typedef enum {
    MON_RAT = 0,
    MON_GOBLIN,
    MON_ORC,
    MON_TROLL,
    MON_DRAGON,
    MON_TYPE_COUNT
} MonsterType;

typedef struct {
    int x;
    int y;
    int hp;
    int max_hp;
    int damage;
    char glyph;
    const char *name;
    MonsterType type;
    int speed;  /* moves per player turn (1 or 2) */
    int slow;   /* nonzero: acts only every other turn */
    int alive;
} MonsterState;

void init_monster(MonsterState *monster, MonsterType type, int x, int y, int depth);
void free_monster(MonsterState *monster);

void spawn_monsters(struct GameState *state);
void monsters_take_turn(struct GameState *state);
int monster_at(struct GameState *state, int x, int y);
void player_attack_monster(struct GameState *state, int index);

#endif // MONSTER_H
