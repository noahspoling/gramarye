#ifndef DOUBLE_COORD_HASH
#define DOUBLE_COORD_HASH

typedef struct {
    double x;
    double y;
} double_coord_t;

static int double_coord_cmp(const void *x, const void *y);
static unsigned double_coord_hash(const void *key);

#endif //DOUBLE_COORD_HASH