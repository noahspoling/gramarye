#ifndef INPUT_PROVIDER_H
#define INPUT_PROVIDER_H

#include <stdbool.h>
#include "renderer/renderer.h"

// Backend-agnostic input key codes
typedef enum {
    INPUT_KEY_W,
    INPUT_KEY_A,
    INPUT_KEY_S,
    INPUT_KEY_D,
    INPUT_KEY_UP,
    INPUT_KEY_DOWN,
    INPUT_KEY_LEFT,
    INPUT_KEY_RIGHT,
    INPUT_KEY_F3,
    // Add more keys as needed
} InputKey;

// Backend-agnostic mouse button codes
typedef enum {
    INPUT_MOUSE_BUTTON_LEFT,
    INPUT_MOUSE_BUTTON_RIGHT,
    INPUT_MOUSE_BUTTON_MIDDLE,
} InputMouseButton;

// Forward declaration
typedef struct InputProvider InputProvider;

// Input provider vtable
typedef struct InputProviderVTable {
    // Key input
    bool (*is_key_pressed)(InputProvider* provider, InputKey key);
    
    // Mouse input
    bool (*is_mouse_button_pressed)(InputProvider* provider, InputMouseButton button);
    float (*get_mouse_wheel_move)(InputProvider* provider);
    RenderVector2 (*get_mouse_position)(InputProvider* provider);
} InputProviderVTable;

// Input provider structure
struct InputProvider {
    const InputProviderVTable* vtable;
    void* backendData;  // Backend-specific data
};

// Input provider interface functions
bool InputProvider_is_key_pressed(InputProvider* provider, InputKey key);
bool InputProvider_is_mouse_button_pressed(InputProvider* provider, InputMouseButton button);
float InputProvider_get_mouse_wheel_move(InputProvider* provider);
RenderVector2 InputProvider_get_mouse_position(InputProvider* provider);

#endif // INPUT_PROVIDER_H

