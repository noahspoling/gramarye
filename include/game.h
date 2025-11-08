#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "assert.h"
#include "mem.h"

typedef struct {
    int width;
    int height;
} GameWindow;



void InitGameWindow(GameWindow *window) {
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(window->width, window->height, "raylib [core] example - basic window");
}

void CloseGameWindow() {
    CloseWindow();
}

#endif // GAME_H