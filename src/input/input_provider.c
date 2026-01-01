#include "input/input_provider.h"

// Input provider interface implementation - delegates to vtable

bool InputProvider_is_key_pressed(InputProvider* provider, InputKey key) {
    if (provider && provider->vtable && provider->vtable->is_key_pressed) {
        return provider->vtable->is_key_pressed(provider, key);
    }
    return false;
}

bool InputProvider_is_mouse_button_pressed(InputProvider* provider, InputMouseButton button) {
    if (provider && provider->vtable && provider->vtable->is_mouse_button_pressed) {
        return provider->vtable->is_mouse_button_pressed(provider, button);
    }
    return false;
}

float InputProvider_get_mouse_wheel_move(InputProvider* provider) {
    if (provider && provider->vtable && provider->vtable->get_mouse_wheel_move) {
        return provider->vtable->get_mouse_wheel_move(provider);
    }
    return 0.0f;
}

RenderVector2 InputProvider_get_mouse_position(InputProvider* provider) {
    if (provider && provider->vtable && provider->vtable->get_mouse_position) {
        return provider->vtable->get_mouse_position(provider);
    }
    return (RenderVector2){0.0f, 0.0f};
}

