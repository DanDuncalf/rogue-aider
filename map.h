#ifndef MAP_H
#define MAP_H

typedef struct {
    int width;
    int height;
    char **tiles;
} MapState;

void init_map(MapState *map);
void free_map(MapState *map);

#endif // MAP_H
