#ifndef ATLAS_TABLE_H
#define ATLAS_TABLE_H

#include "table.h"
#include "atlas.h"

typedef Table_T AtlasTable;

AtlasTable AtlasTable_new();
void AtlasTable_add(AtlasTable *atlasTable, const char *key, Atlas *atlas);
Atlas *AtlasTable_get(AtlasTable *atlasTable, const char *key);
void AtlasTable_remove(AtlasTable *atlasTable, const char *key);
void AtlasTable_free(AtlasTable *atlasTable);

#endif // ATLAS_TABLE_H