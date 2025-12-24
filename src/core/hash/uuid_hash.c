#include "core/hash/uuid_hash.h"
#include <stdint.h>
#include <stddef.h>

uint32_t hash_uuid(uuid_key_t uuid) {
    // Mix the two 64-bit parts using a simple hash function
    uint64_t mixed = uuid.high ^ uuid.low;
    // Simple hash combining high and low 32-bit parts
    return (uint32_t)(mixed ^ (mixed >> 32));
}

// Or use byte-level hashing:
uint32_t hash_uuid_bytes(uuid_key_t uuid) {
    uint8_t *bytes = (uint8_t*)&uuid;
    uint32_t hash = 5381;
    for (size_t i = 0; i < sizeof(uuid); i++) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    return hash;
}

