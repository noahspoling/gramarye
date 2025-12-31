#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct Renderer Renderer;
typedef struct RenderTexture RenderTexture;
typedef struct RenderFont RenderFont;

typedef struct {
    float x, y, width, height;
} RenderRect;

typedef struct {
    float x, y;
} RenderVector2;

typedef struct {
    uint8_t r, g, b, a;
} RenderColor;

typedef enum {
    RENDER_COMMAND_TYPE_NONE,
    RENDER_COMMAND_TYPE_TRIANGLE,
    RENDER_COMMAND_TYPE_RECTANGLE,
    RENDER_COMMAND_TYPE_RECTANGLE_LINES,
    RENDER_COMMAND_TYPE_TEXTURE,
    RENDER_COMMAND_TYPE_TEXTURE_PRO,
    RENDER_COMMAND_TYPE_TEXT,
    RENDER_COMMAND_TYPE_CIRCLE,
    RENDER_COMMAND_TYPE_BEGIN_LAYER,
    RENDER_COMMAND_TYPE_END_LAYER,
    RENDER_COMMAND_TYPE_BEGIN_2D,
    RENDER_COMMAND_TYPE_END_2D,
    RENDER_COMMAND_TYPE_BEGIN_3D,
    RENDER_COMMAND_TYPE_END_3D,
    RENDER_COMMAND_TYPE_BEGIN_UI,
    RENDER_COMMAND_TYPE_END_UI,
    RENDER_COMMAND_TYPE_BEGIN_CLIP,
    RENDER_COMMAND_TYPE_END_CLIP,
    RENDER_COMMAND_TYPE_BEGIN_DEBUG,
    RENDER_COMMAND_TYPE_END_DEBUG,
    RENDER_COMMAND_TYPE_CUSTOM,
} RenderCommandType;

typedef struct {
    RenderCommandType type;
    RenderRect bounds;
    RenderColor color;
    union {
        struct {
            void* textureHandle;
            RenderRect srcRect;
            float rotation;
            RenderVector2 origin;
        } texture;
        struct {
            float thickness;
        } lines;
        struct {
            const char* text;
            void* fontHandle;
            float fontSize;
            float spacing;
        } text;
    } data;
} RenderCommand;

typedef struct RendererVTable {

    //Initialization
    void (*init)(Renderer* renderer, int width, int height, const char* title, uint32_t flags);
    void (*close)(Renderer* renderer);

    //Frame management
    void (*begin_frame)(Renderer* renderer);
    void (*end_frame)(Renderer* renderer);

    //Drawing
    void (*execute_command)(Renderer* renderer, const RenderCommand* command);
    void (*execute_commands)(Renderer* renderer, const RenderCommand* commands, size_t count);

    // Resources
    void* (*load_texture)(Renderer* renderer, const char* path);
    void (*unload_texture)(Renderer* renderer, void* textureHandle);
    void* (*load_font)(Renderer* renderer, const char* path, float fontSize);
    void (*unload_font)(Renderer* renderer, void* fontHandle);

    // Query functions
    bool (*should_close)(Renderer* renderer);
    float (*get_delta_time)(Renderer* renderer);
    RenderVector2 (*get_mouse_position)(Renderer* renderer);
    RenderVector2 (*get_window_size)(Renderer* renderer);
} RendererVTable;

struct Renderer {
    const RendererVTable* vtable;
    void* backendData;  // Backend-specific data
};

// Renderer functions
void Renderer_init(Renderer* renderer, int width, int height, const char* title, uint32_t flags);
void Renderer_close(Renderer* renderer);
void Renderer_begin_frame(Renderer* renderer);
void Renderer_end_frame(Renderer* renderer);
void Renderer_execute_command(Renderer* renderer, const RenderCommand* command);
void Renderer_execute_commands(Renderer* renderer, const RenderCommand* commands, size_t count);
void* Renderer_load_texture(Renderer* renderer, const char* path);
void Renderer_unload_texture(Renderer* renderer, void* textureHandle);
void* Renderer_load_font(Renderer* renderer, const char* path, float fontSize);
void Renderer_unload_font(Renderer* renderer, void* fontHandle);
bool Renderer_should_close(Renderer* renderer);
float Renderer_get_delta_time(Renderer* renderer);
RenderVector2 Renderer_get_mouse_position(Renderer* renderer);
RenderVector2 Renderer_get_window_size(Renderer* renderer);

#endif // RENDERER_H