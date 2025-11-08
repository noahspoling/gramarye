#ifndef COMPONENTS_MAP_H
#define COMPONENTS_MAP_H

typedef struct Map {
    int width;
    int height;
    char** tiles; // 2D array of characters representing the map
} Map;

Map* create_map(int width, int height);
void destroy_map(Map* map);
void set_tile(Map* map, int x, int y, char tile);
char get_tile(const Map* map, int x, int y);

#endif // COMPONENTS_MAP_H