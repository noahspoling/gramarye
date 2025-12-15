#include "int_hash.h"

static int int_cmp(const void *x, const void *y) {
    return *(const int*)x != *(const int*)y;
}

static unsigned int_hash(const void *key) {
    return *(const int*)key;
}