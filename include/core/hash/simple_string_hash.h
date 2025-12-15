#include <string.h>

static int str_cmp(const void *x, const void *y) {
    return strcmp((const char *)x, (const char *)y);
}
static unsigned str_hash(const void *key) {
    // Simple string hash (djb2)
    const char *str = (const char *)key;
    unsigned hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}