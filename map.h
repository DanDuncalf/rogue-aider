#ifndef MAP_H
#define MAP_H

/* The dungeon occupies rows 0..MAP_HEIGHT-1; row MAP_HEIGHT is the message
 * line, row MAP_HEIGHT+1 the status bar and row MAP_HEIGHT+2 the keybind
 * help line (fits an 80x24 terminal). */
#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define FOV_RADIUS 8

typedef struct {
    int width;
    int height;
    char **tiles;           /* '#', '.', '<', '>' */
    unsigned char **visible;
    unsigned char **explored;
    int stair_up_x, stair_up_y;
    int stair_down_x, stair_down_y;
} MapState;

void init_map(MapState *map);
void free_map(MapState *map);
void map_generate(MapState *map, int place_up, int place_down);
void map_compute_fov(MapState *map, int px, int py);
int map_is_walkable(MapState *map, int x, int y);
int map_transparent(MapState *map, int x, int y);
int map_random_floor(MapState *map, int *x, int *y);

#endif // MAP_H
