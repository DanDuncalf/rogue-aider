#ifndef ITEMS_H
#define ITEMS_H

struct GameState;

#define MAX_GROUND_ITEMS 64
#define MAX_INVENTORY 16

typedef enum {
    ITEM_GOLD = 0,
    ITEM_POTION,
    ITEM_WEAPON,
    ITEM_AMULET
} ItemType;

typedef struct {
    ItemType type;
    int x, y;      /* ground position (meaningless while in inventory) */
    int value;     /* gold amount, heal amount, or weapon damage */
    char glyph;
    char name[32];
    int active;    /* slot in use */
} Item;

typedef struct {
    Item ground[MAX_GROUND_ITEMS];
    Item inventory[MAX_INVENTORY];
    int weapon_slot;  /* inventory index of equipped weapon, -1 = none */
} ItemsState;

void init_items(ItemsState *items);
void free_items(ItemsState *items);

void clear_ground_items(ItemsState *items);
void spawn_items(struct GameState *state, int depth);

/* Auto-collect gold under the player's feet (called after movement). */
void item_collect_gold(struct GameState *state);
/* Is there a non-gold item on the player's tile? */
int item_under_player(struct GameState *state);

int item_pickup(struct GameState *state);
int item_use(struct GameState *state, int slot);
int item_drop(struct GameState *state, int slot);

int inventory_count(struct GameState *state);
int player_has_amulet(struct GameState *state);
const char *equipped_weapon_name(struct GameState *state);
int player_attack_damage(struct GameState *state);

#endif // ITEMS_H
