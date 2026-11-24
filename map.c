#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN_LEAF_SIZE 10
#define MAX_ROOMS 32
#define BSP_DEPTH 5

typedef struct {
    int x, y, w, h;
} Rect;

static Rect rooms[MAX_ROOMS];
static int room_count;

static int rand_range(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}

static void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return p;
}

void init_map(MapState *map) {
    map->width = MAP_WIDTH;
    map->height = MAP_HEIGHT;

    map->tiles = xmalloc((size_t)map->height * sizeof(char *));
    map->visible = xmalloc((size_t)map->height * sizeof(unsigned char *));
    map->explored = xmalloc((size_t)map->height * sizeof(unsigned char *));
    for (int i = 0; i < map->height; i++) {
        map->tiles[i] = xmalloc((size_t)map->width * sizeof(char));
        map->visible[i] = xmalloc((size_t)map->width * sizeof(unsigned char));
        map->explored[i] = xmalloc((size_t)map->width * sizeof(unsigned char));
        memset(map->tiles[i], '#', (size_t)map->width);
        memset(map->visible[i], 0, (size_t)map->width);
        memset(map->explored[i], 0, (size_t)map->width);
    }
    map->stair_up_x = map->stair_up_y = -1;
    map->stair_down_x = map->stair_down_y = -1;
}

void free_map(MapState *map) {
    if (!map || !map->tiles) return;

    for (int i = 0; i < map->height; i++) {
        free(map->tiles[i]);
        free(map->visible[i]);
        free(map->explored[i]);
    }
    free(map->tiles);
    free(map->visible);
    free(map->explored);
    map->tiles = NULL;
    map->visible = NULL;
    map->explored = NULL;
}

static void make_room(MapState *map, Rect leaf) {
    if (room_count >= MAX_ROOMS) return;
    if (leaf.w < 6 || leaf.h < 5) return;

    int rw = rand_range(4, leaf.w - 2);
    int rh = rand_range(3, leaf.h - 2);
    int rx = rand_range(leaf.x + 1, leaf.x + leaf.w - rw - 1);
    int ry = rand_range(leaf.y + 1, leaf.y + leaf.h - rh - 1);

    for (int y = ry; y < ry + rh; y++) {
        for (int x = rx; x < rx + rw; x++) {
            map->tiles[y][x] = '.';
        }
    }
    rooms[room_count++] = (Rect){rx, ry, rw, rh};
}

static void bsp_split(MapState *map, Rect r, int depth) {
    if (depth <= 0 || (r.w < MIN_LEAF_SIZE * 2 && r.h < MIN_LEAF_SIZE * 2)) {
        make_room(map, r);
        return;
    }

    int split_horizontal;
    if (r.w >= MIN_LEAF_SIZE * 2 && r.h >= MIN_LEAF_SIZE * 2) {
        split_horizontal = rand() % 2;
    } else {
        split_horizontal = (r.h >= MIN_LEAF_SIZE * 2);
    }

    if (split_horizontal) {
        int cut = rand_range(MIN_LEAF_SIZE, r.h - MIN_LEAF_SIZE);
        bsp_split(map, (Rect){r.x, r.y, r.w, cut}, depth - 1);
        bsp_split(map, (Rect){r.x, r.y + cut, r.w, r.h - cut}, depth - 1);
    } else {
        int cut = rand_range(MIN_LEAF_SIZE, r.w - MIN_LEAF_SIZE);
        bsp_split(map, (Rect){r.x, r.y, cut, r.h}, depth - 1);
        bsp_split(map, (Rect){r.x + cut, r.y, r.w - cut, r.h}, depth - 1);
    }
}

static void dig_corridor(MapState *map, int x1, int y1, int x2, int y2) {
    int x = x1;
    int y = y1;

    if (rand() % 2) {
        while (x != x2) {
            if (map->tiles[y][x] == '#') map->tiles[y][x] = '.';
            x += (x2 > x) ? 1 : -1;
        }
        while (y != y2) {
            if (map->tiles[y][x] == '#') map->tiles[y][x] = '.';
            y += (y2 > y) ? 1 : -1;
        }
    } else {
        while (y != y2) {
            if (map->tiles[y][x] == '#') map->tiles[y][x] = '.';
            y += (y2 > y) ? 1 : -1;
        }
        while (x != x2) {
            if (map->tiles[y][x] == '#') map->tiles[y][x] = '.';
            x += (x2 > x) ? 1 : -1;
        }
    }
}

static int random_floor_in_room(Rect r, int *x, int *y) {
    *x = rand_range(r.x, r.x + r.w - 1);
    *y = rand_range(r.y, r.y + r.h - 1);
    return 1;
}

void map_generate(MapState *map, int place_up, int place_down) {
    for (int y = 0; y < map->height; y++) {
        memset(map->tiles[y], '#', (size_t)map->width);
        memset(map->visible[y], 0, (size_t)map->width);
        memset(map->explored[y], 0, (size_t)map->width);
    }
    map->stair_up_x = map->stair_up_y = -1;
    map->stair_down_x = map->stair_down_y = -1;

    room_count = 0;
    bsp_split(map, (Rect){0, 0, map->width, map->height}, BSP_DEPTH);

    /* Connect consecutive rooms with L-shaped corridors. Rooms are created
     * in tree order, so neighbours in the list are usually adjacent. */
    for (int i = 1; i < room_count; i++) {
        int x1 = rooms[i - 1].x + rooms[i - 1].w / 2;
        int y1 = rooms[i - 1].y + rooms[i - 1].h / 2;
        int x2 = rooms[i].x + rooms[i].w / 2;
        int y2 = rooms[i].y + rooms[i].h / 2;
        dig_corridor(map, x1, y1, x2, y2);
    }

    if (room_count == 0) return;

    int down_room = rand() % room_count;
    int up_room = rand() % room_count;
    if (room_count > 1 && up_room == down_room) {
        up_room = (up_room + 1) % room_count;
    }

    if (place_down) {
        int x, y;
        random_floor_in_room(rooms[down_room], &x, &y);
        map->tiles[y][x] = '>';
        map->stair_down_x = x;
        map->stair_down_y = y;
    }
    if (place_up) {
        int x, y;
        random_floor_in_room(rooms[up_room], &x, &y);
        if (map->tiles[y][x] == '.') {
            map->tiles[y][x] = '<';
            map->stair_up_x = x;
            map->stair_up_y = y;
        }
    }
}

int map_is_walkable(MapState *map, int x, int y) {
    if (x < 0 || y < 0 || x >= map->width || y >= map->height) return 0;
    char t = map->tiles[y][x];
    return t == '.' || t == '<' || t == '>';
}

int map_transparent(MapState *map, int x, int y) {
    if (x < 0 || y < 0 || x >= map->width || y >= map->height) return 0;
    return map->tiles[y][x] != '#';
}

int map_random_floor(MapState *map, int *x, int *y) {
    for (int tries = 0; tries < 2000; tries++) {
        int rx = rand() % map->width;
        int ry = rand() % map->height;
        if (map->tiles[ry][rx] == '.') {
            *x = rx;
            *y = ry;
            return 1;
        }
    }
    /* Fallback: linear scan. */
    for (int ry = 0; ry < map->height; ry++) {
        for (int rx = 0; rx < map->width; rx++) {
            if (map->tiles[ry][rx] == '.') {
                *x = rx;
                *y = ry;
                return 1;
            }
        }
    }
    return 0;
}

/* Recursive shadowcasting (Bjorn Bergstrom's algorithm), one octant. */
static void cast_light(MapState *map, int cx, int cy, int row,
                       float start, float end, int radius,
                       int xx, int xy, int yx, int yy) {
    if (start < end) return;

    float new_start = 0.0f;
    int blocked = 0;
    for (int distance = row; distance <= radius && !blocked; distance++) {
        int delta_y = -distance;
        for (int delta_x = -distance; delta_x <= 0; delta_x++) {
            int cur_x = cx + delta_x * xx + delta_y * xy;
            int cur_y = cy + delta_x * yx + delta_y * yy;
            float left_slope = (delta_x - 0.5f) / (delta_y + 0.5f);
            float right_slope = (delta_x + 0.5f) / (delta_y - 0.5f);

            if (cur_x < 0 || cur_y < 0 || cur_x >= map->width || cur_y >= map->height
                || start < right_slope) {
                continue;
            } else if (end > left_slope) {
                break;
            }

            if (delta_x * delta_x + delta_y * delta_y <= radius * radius) {
                map->visible[cur_y][cur_x] = 1;
                map->explored[cur_y][cur_x] = 1;
            }

            if (blocked) {
                if (!map_transparent(map, cur_x, cur_y)) {
                    new_start = right_slope;
                    continue;
                } else {
                    blocked = 0;
                    start = new_start;
                }
            } else if (!map_transparent(map, cur_x, cur_y) && distance < radius) {
                blocked = 1;
                cast_light(map, cx, cy, distance + 1, start, left_slope,
                           radius, xx, xy, yx, yy);
                new_start = right_slope;
            }
        }
    }
}

void map_compute_fov(MapState *map, int px, int py) {
    static const int octants[8][4] = {
        {1, 0, 0, 1}, {0, 1, 1, 0}, {0, -1, 1, 0}, {-1, 0, 0, 1},
        {-1, 0, 0, -1}, {0, -1, -1, 0}, {0, 1, -1, 0}, {1, 0, 0, -1}
    };

    for (int y = 0; y < map->height; y++) {
        memset(map->visible[y], 0, (size_t)map->width);
    }
    if (px >= 0 && py >= 0 && px < map->width && py < map->height) {
        map->visible[py][px] = 1;
        map->explored[py][px] = 1;
    }

    for (int i = 0; i < 8; i++) {
        cast_light(map, px, py, 1, 1.0f, 0.0f, FOV_RADIUS,
                   octants[i][0], octants[i][1], octants[i][2], octants[i][3]);
    }
}
