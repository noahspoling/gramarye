#ifndef KEY_H
#define KEY_H

typedef enum  {
    KEY_NONE = 0,
    KEY_FLOOR = 1,
    KEY_SPECIAL = 2
} KeyType;

typedef struct Key {
    KeyType type;
    int id;
} Key;

void init_key(Key* key, KeyType type, int id);
void free_key(Key* key);



#endif // KEY_H