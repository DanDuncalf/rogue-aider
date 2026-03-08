#ifndef ITEMS_H
#define ITEMS_H

typedef struct {
    int gold;
    // Add more item attributes as needed
} ItemsState;

void init_items(ItemsState *items);
void free_items(ItemsState *items);

#endif // ITEMS_H
