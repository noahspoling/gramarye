#ifndef UUID_HASH_H
#define UUID_HASH_H

#include <stdint.h>
#include <stddef.h>
#include "int_hash.h"

typedef struct {
    uint64_t high;
    uint64_t low;
} uuid_key_t;

// Hash function declarations
uint32_t hash_uuid(uuid_key_t uuid);
uint32_t hash_uuid_bytes(uuid_key_t uuid);

#endif // UUID_HASH_H