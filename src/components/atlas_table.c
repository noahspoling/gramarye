#include "atlas_table.h"
#include "simple_string_hash.h"
#include "mem.h"

AtlasTable AtlasTable_new() {
    return Table_new(16, str_cmp, str_hash);
}
void AtlasTable_add(AtlasTable *atlasTable, const char *key, Atlas *atlas) {
    char *keyCopy = Mem_alloc(strlen(key) + 1, __FILE__, __LINE__);
    strcpy(keyCopy, key);
    Table_put(*atlasTable, keyCopy, atlas);
}
Atlas *AtlasTable_get(AtlasTable *atlasTable, const char *key) {
    return Table_get(*atlasTable, key);
}
void AtlasTable_remove(AtlasTable *atlasTable, const char *key) {
    char *keyCopy = Mem_alloc(strlen(key) + 1, __FILE__, __LINE__);
    strcpy(keyCopy, key);
    Atlas *atlas = Table_remove(*atlasTable, keyCopy);
    if (atlas) {
        Mem_free(atlas, __FILE__, __LINE__);
    }
    Mem_free(keyCopy, __FILE__, __LINE__);
}
void AtlasTable_free(AtlasTable *atlasTable) {
    Table_free(atlasTable);
}
