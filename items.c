#include "items.h"
#include <stdlib.h>

void init_items(ItemsState *items) {
    items->gold = 0;
}

void free_items(ItemsState *items) {
    // No dynamic memory to free for ItemsState
}
