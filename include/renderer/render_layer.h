#ifndef RENDER_LAYER_H
#define RENDER_LAYER_H

#include "renderer.h"
#include "arena.h"

typedef struct RenderLayer {
    RenderCommand* commands;
    size_t commandCount;
    size_t commandCapacity;
    Arena_T arena;
    int layerIndex;
} RenderLayer;

typedef struct RenderPipeline {
    RenderLayer* layers;
    size_t layerCount;
    size_t layerCapacity;
    Arena_T arena;
} RenderPipeline;

// Layer functions
RenderLayer* RenderLayer_create(Arena_T arena, int layerIndex);
void RenderLayer_add_command(RenderLayer* layer, const RenderCommand* cmd);
void RenderLayer_clear(RenderLayer* layer);

// Pipeline functions
RenderPipeline* RenderPipeline_create(Arena_T arena);
void RenderPipeline_add_layer(RenderPipeline* pipeline, RenderLayer* layer);
void RenderPipeline_execute(RenderPipeline* pipeline, Renderer* renderer);
void RenderPipeline_clear(RenderPipeline* pipeline);

#endif // RENDER_LAYER_H