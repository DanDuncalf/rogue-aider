#include "items.h"
#include "game_state.h"
#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const struct {
    const char *name;
    int damage;
} weapon_types[] = {
    {"dagger", 2},
    {"short sword", 4},
    {"long sword", 6},
    {"greatsword", 9},
};
#define WEAPON_TYPE_COUNT 4

void init_items(ItemsState *items) {
    items->weapon_slot = -1;
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) items->ground[i].active = 0;
    for (int i = 0; i < MAX_INVENTORY; i++) items->inventory[i].active = 0;
}

void free_items(ItemsState *items) {
    // No dynamic memory to free for ItemsState
    (void)items;
}

void clear_ground_items(ItemsState *items) {
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) items->ground[i].active = 0;
}

static int free_ground_slot(ItemsState *items) {
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) {
        if (!items->ground[i].active) return i;
    }
    return -1;
}

static int free_inventory_slot(ItemsState *items) {
    for (int i = 0; i < MAX_INVENTORY; i++) {
        if (!items->inventory[i].active) return i;
    }
    return -1;
}

static void place_item(struct GameState *state, Item item) {
    int slot = free_ground_slot(&state->items);
    if (slot < 0) return;
    int x, y;
    for (int tries = 0; tries < 200; tries++) {
        if (!map_random_floor(&state->map, &x, &y)) return;
        if (x == state->player.x && y == state->player.y) continue;
        item.x = x;
        item.y = y;
        item.active = 1;
        state->items.ground[slot] = item;
        return;
    }
}

static int max_weapon_tier(int depth) {
    if (depth >= 7) return WEAPON_TYPE_COUNT - 1;
    if (depth >= 4) return 2;
    if (depth >= 2) return 1;
    return 0;
}

void spawn_items(struct GameState *state, int depth) {
    int count = 3 + rand() % 4;
    for (int i = 0; i < count; i++) {
        int roll = rand() % 100;
        if (roll < 40) {
            Item it;
            it.type = ITEM_GOLD;
            it.glyph = '$';
            it.value = 5 + rand() % (5 + depth * 5);
            snprintf(it.name, sizeof(it.name), "%d gold pieces", it.value);
            place_item(state, it);
        } else if (roll < 70) {
            Item it;
            it.type = ITEM_POTION;
            it.glyph = '!';
            it.value = 15 + depth * 3;
            snprintf(it.name, sizeof(it.name), "healing potion");
            place_item(state, it);
        } else {
            int tier = rand() % (max_weapon_tier(depth) + 1);
            Item it;
            it.type = ITEM_WEAPON;
            it.glyph = '/';
            it.value = weapon_types[tier].damage;
            snprintf(it.name, sizeof(it.name), "%s", weapon_types[tier].name);
            place_item(state, it);
        }
    }

    if (depth == 10) {
        Item it;
        it.type = ITEM_AMULET;
        it.glyph = '"';
        it.value = 0;
        snprintf(it.name, sizeof(it.name), "Amulet of Yendor");
        place_item(state, it);
    }
}

int inventory_count(struct GameState *state) {
    int n = 0;
    for (int i = 0; i < MAX_INVENTORY; i++) {
        if (state->items.inventory[i].active) n++;
    }
    return n;
}

int item_under_player(struct GameState *state) {
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) {
        Item *it = &state->items.ground[i];
        if (it->active && it->type != ITEM_GOLD
            && it->x == state->player.x && it->y == state->player.y) {
            return 1;
        }
    }
    return 0;
}

void item_collect_gold(struct GameState *state) {
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) {
        Item *it = &state->items.ground[i];
        if (it->active && it->type == ITEM_GOLD
            && it->x == state->player.x && it->y == state->player.y) {
            it->active = 0;
            state->player.gold += it->value;
            set_message(state, "You pick up %d gold.", it->value);
            return;
        }
    }
}

int item_pickup(struct GameState *state) {
    for (int i = 0; i < MAX_GROUND_ITEMS; i++) {
        Item *it = &state->items.ground[i];
        if (!it->active || it->x != state->player.x || it->y != state->player.y) {
            continue;
        }
        if (it->type == ITEM_GOLD) {
            item_collect_gold(state);
            return 1;
        }
        int slot = free_inventory_slot(&state->items);
        if (slot < 0) {
            set_message(state, "Your pack is full.");
            return 0;
        }
        state->items.inventory[slot] = *it;
        it->active = 0;
        if (state->items.inventory[slot].type == ITEM_AMULET) {
            set_message(state, "You got the Amulet of Yendor! Escape to the surface!");
        } else {
            set_message(state, "You pick up the %s.", state->items.inventory[slot].name);
        }
        return 1;
    }
    set_message(state, "There is nothing here to pick up.");
    return 0;
}

static void remove_inventory_item(ItemsState *items, int slot) {
    for (int i = slot; i < MAX_INVENTORY - 1; i++) {
        items->inventory[i] = items->inventory[i + 1];
    }
    items->inventory[MAX_INVENTORY - 1].active = 0;
    if (items->weapon_slot == slot) {
        items->weapon_slot = -1;
    } else if (items->weapon_slot > slot) {
        items->weapon_slot--;
    }
}

int item_use(struct GameState *state, int slot) {
    if (slot < 0 || slot >= MAX_INVENTORY || !state->items.inventory[slot].active) {
        return 0;
    }
    Item *it = &state->items.inventory[slot];
    switch (it->type) {
        case ITEM_POTION: {
            int healed = it->value;
            if (state->player.hp + healed > state->player.max_hp) {
                healed = state->player.max_hp - state->player.hp;
            }
            state->player.hp += healed;
            set_message(state, "You drink the %s and recover %d HP.", it->name, healed);
            remove_inventory_item(&state->items, slot);
            return 1;
        }
        case ITEM_WEAPON:
            state->items.weapon_slot = slot;
            set_message(state, "You wield the %s.", it->name);
            return 1;
        case ITEM_AMULET:
            set_message(state, "The Amulet of Yendor glows softly.");
            return 1;
        case ITEM_GOLD:
            set_message(state, "You can't use gold.");
            return 0;
    }
    return 0;
}

int item_drop(struct GameState *state, int slot) {
    if (slot < 0 || slot >= MAX_INVENTORY || !state->items.inventory[slot].active) {
        return 0;
    }
    int ground = free_ground_slot(&state->items);
    if (ground < 0) {
        set_message(state, "There is no room to drop that here.");
        return 0;
    }
    Item it = state->items.inventory[slot];
    it.x = state->player.x;
    it.y = state->player.y;
    state->items.ground[ground] = it;
    if (it.type == ITEM_AMULET) {
        set_message(state, "You drop the Amulet of Yendor. A bad idea?");
    } else {
        set_message(state, "You drop the %s.", it.name);
    }
    remove_inventory_item(&state->items, slot);
    return 1;
}

int player_has_amulet(struct GameState *state) {
    for (int i = 0; i < MAX_INVENTORY; i++) {
        Item *it = &state->items.inventory[i];
        if (it->active && it->type == ITEM_AMULET) return 1;
    }
    return 0;
}

const char *equipped_weapon_name(struct GameState *state) {
    int slot = state->items.weapon_slot;
    if (slot >= 0 && slot < MAX_INVENTORY && state->items.inventory[slot].active) {
        return state->items.inventory[slot].name;
    }
    return "bare hands";
}

int player_attack_damage(struct GameState *state) {
    int slot = state->items.weapon_slot;
    int weapon = 0;
    if (slot >= 0 && slot < MAX_INVENTORY && state->items.inventory[slot].active) {
        weapon = state->items.inventory[slot].value;
    }
    return state->player.base_damage + weapon;
}
