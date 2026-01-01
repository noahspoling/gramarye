#ifndef INPUT_H
#define INPUT_H

#include "renderer/renderer.h"

typedef struct InputVTable {
    void (*init)(Input* input);
    void (*close)(Input* input);
    void (*poll)(Input* input);
    void (*process)(Input* input);
    void (*destroy)(Input* input);
} InputVTable;

typedef struct Input {
    const InputVTable* vtable;
    void* backendData;
} Input;

Input* Input_create(Arena_T arena);
void Input_destroy(Input* input);
void Input_poll(Input* input);
void Input_process(Input* input);
void Input_destroy(Input* input);

#endif // INPUT_H