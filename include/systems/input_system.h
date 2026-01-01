#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <stdbool.h>
#include "raylib.h"  // Still needed for Vector2 type in InputCommand

#include "arena.h"
#include "input/input_provider.h"

typedef struct InputSystem InputSystem;

typedef enum {
    Cmd_None = 0,
    Cmd_Move,
    Cmd_Zoom,
    Cmd_ToggleDebug,
    Cmd_PlaceTile
} InputCommandType;

typedef struct InputCommand {
    InputCommandType type;
    union {
        struct { int dx; int dy; } move;
        struct { float wheel; } zoom;
        struct { Vector2 mousePos; } place;
    } as;
} InputCommand;

InputSystem* InputSystem_create(Arena_T arena, InputProvider* inputProvider);
void InputSystem_destroy(InputSystem* sys);

// Main thread only: polls Raylib input APIs and publishes a snapshot to the input thread.
void InputSystem_poll_and_publish(InputSystem* sys);

// Main thread only: non-blocking pop of next available command.
bool InputSystem_pop(InputSystem* sys, InputCommand* outCmd);

#endif // INPUT_SYSTEM_H


