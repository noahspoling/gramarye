/* $Id$ */
#ifndef FLOOR_H
#define FLOOR_H

#include "table.h"
#include "array.h"
#include "map.h"
#include "key.h"

typedef struct Room {
    int id;
    int x, y;
    Map* map;
} Room;

typedef struct Connection {
    int room1_id;
    int room2_id;
    KeyType key_type;
    int key_id;
    bool locked;
} Connection;

typedef struct Floor {
    unsigned int level;
    Table_T rooms;
    Array_T connections;
    Room* current_room;
    Array_T floor_keys;
} Floor;

Floor *Floor_create(unsigned int level);
void Floor_destroy(Floor *floor);


#endif // FLOOR_H
