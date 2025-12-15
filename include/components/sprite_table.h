#ifndef SPRITE_TABLE_H
#define SPRITE_TABLE_H

#include "table.h"

typedef Table_T SpriteTable;

SpriteTable SpriteTable_new();
void SpriteTable_add(SpriteTable *spriteTable, const char *key, int spriteIndex);
int SpriteTable_get(SpriteTable *spriteTable, const char *key);
void SpriteTable_remove(SpriteTable *spriteTable, const char *key);
void SpriteTable_free(SpriteTable *spriteTable);

#endif // SPRITE_TABLE_H