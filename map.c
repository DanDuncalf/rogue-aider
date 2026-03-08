#include "map.h"
#include <stdlib.h>
#include <string.h>

#define MIN_ROOM_SIZE 5

void init_map(MapState *map) {
    map->width = 80;
    map->height = 24;

    map->tiles = malloc(map->height * sizeof(char *));
    for (int i = 0; i < map->height; i++) {
        map->tiles[i] = malloc(map->width * sizeof(char));
        memset(map->tiles[i], '#', map->width);
    }
}

void free_map(MapState *map) {
    if (!map || !map->tiles) return;

    for (int i = 0; i < map->height; i++) {
        free(map->tiles[i]);
    }
    free(map->tiles);
}
