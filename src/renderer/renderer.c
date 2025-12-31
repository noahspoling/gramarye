#include "renderer/renderer.h"
#include <stdlib.h>


void Renderer_init(Renderer* renderer, int width, int height, const char* title, uint32_t flags) {
    if (renderer && renderer->vtable && renderer->vtable->init) {
        renderer->vtable->init(renderer, width, height, title, flags);
    }
}

void Renderer_close(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->close) {
        renderer->vtable->close(renderer);
    }
}

void Renderer_begin_frame(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->begin_frame) {
        renderer->vtable->begin_frame(renderer);
    }
}

void Renderer_end_frame(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->end_frame) {
        renderer->vtable->end_frame(renderer);
    }
}

void Renderer_execute_command(Renderer* renderer, const RenderCommand* command) {
    if (renderer && renderer->vtable && renderer->vtable->execute_command && command) {
        renderer->vtable->execute_command(renderer, command);
    }
}

void Renderer_execute_commands(Renderer* renderer, const RenderCommand* commands, size_t count) {
    if (renderer && renderer->vtable && renderer->vtable->execute_commands && commands) {
        renderer->vtable->execute_commands(renderer, commands, count);
    }
}

void* Renderer_load_texture(Renderer* renderer, const char* path) {
    if (renderer && renderer->vtable && renderer->vtable->load_texture) {
        return renderer->vtable->load_texture(renderer, path);
    }
    return NULL;
}

void Renderer_unload_texture(Renderer* renderer, void* textureHandle) {
    if (renderer && renderer->vtable && renderer->vtable->unload_texture) {
        renderer->vtable->unload_texture(renderer, textureHandle);
    }
}

void* Renderer_load_font(Renderer* renderer, const char* path, float fontSize) {
    if (renderer && renderer->vtable && renderer->vtable->load_font) {
        return renderer->vtable->load_font(renderer, path, fontSize);
    }
    return NULL;
}

void Renderer_unload_font(Renderer* renderer, void* fontHandle) {
    if (renderer && renderer->vtable && renderer->vtable->unload_font) {
        renderer->vtable->unload_font(renderer, fontHandle);
    }
}

bool Renderer_should_close(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->should_close) {
        return renderer->vtable->should_close(renderer);
    }
    return false;
}

float Renderer_get_delta_time(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->get_delta_time) {
        return renderer->vtable->get_delta_time(renderer);
    }
    return 0.0f;
}

RenderVector2 Renderer_get_mouse_position(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->get_mouse_position) {
        return renderer->vtable->get_mouse_position(renderer);
    }
    return (RenderVector2){0.0f, 0.0f};
}

RenderVector2 Renderer_get_window_size(Renderer* renderer) {
    if (renderer && renderer->vtable && renderer->vtable->get_window_size) {
        return renderer->vtable->get_window_size(renderer);
    }
    return (RenderVector2){0.0f, 0.0f};
}

