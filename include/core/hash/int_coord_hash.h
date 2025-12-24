#ifndef INT_COORD_HASH
#define INT_COORD_HASH

#include <stdbool.h>

typedef struct {
    int x;
    int y;
} IntCoord;

unsigned int IntCoord_hash(const void *key);
int IntCoord_cmp(const void *x, const void *y);

#endif //INT_COORD_HASH