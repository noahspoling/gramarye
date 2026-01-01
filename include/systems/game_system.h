#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include <stdbool.h>
#include "raylib.h"

#include "arena.h"
#include "clay.h"
#include "gramarye_renderer/input_provider.h"
#include "gramarye_renderer/renderer.h"

typedef struct GameSystem GameSystem;

// Owns all game state: tilemap, entities, camera, render targets, etc.
GameSystem* GameSystem_create(Arena_T arena, int mapSize, int tileSize, Vector2 logicalSize, Renderer* renderer, InputProvider* inputProvider);
void GameSystem_destroy(GameSystem* game);

// Called once per-frame (handles input/movement/camera + rendering)
void GameSystem_frame(GameSystem* game, float dt);

#endif // GAME_SYSTEM_H


