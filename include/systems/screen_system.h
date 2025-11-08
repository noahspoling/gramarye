#ifndef SCREEN_SYSTEM_H
#define SCREEN_SYSTEM_H

#include "arena.h"
#include "array.h"
#include "stack.h"
#include "raylib.h"
#include "clay.h"

// Forward declarations

typedef struct ScreenSystem ScreenSystem;
typedef struct Screen Screen;

// Screen interface definition

typedef struct ScreenInterface {
    void (*init)(Screen* screen, Arena_T arena);
    void (*update)(Screen* screen, float deltaTime);
    void (*render)(Screen* screen);
    void (*cleanup)(Screen* screen);
    void (*handle_input)(Screen* screen, int key);
} ScreenInterface;

typedef enum {
    SCREEN_MENU,
    SCREEN_GAME,
    SCREEN_PAUSE,
    SCREEN_SETTINGS
} ScreenType;

typedef struct Screen {
    ScreenType type;
    ScreenInterface interface;
    void* data;
    Arena_T arena;
    bool initialized;
} Screen;

typedef struct ScreenSystem {
    Stack_T screen_stack;
    Arena_T system_arena;
    Array_T screen_pool;
    ScreenType current_screen_type;
    bool transition_pending;
} ScreenSystem;


// Screen system management

ScreenSystem* ScreenSystem_create(Arena_T arena);
void ScreenSystem_destroy(ScreenSystem* system);

// Screen stack operations

void ScreenSystem_push_screen(ScreenSystem* system, ScreenType type);
void ScreenSystem_pop_screen(ScreenSystem* system);
void ScreenSystem_switch_screen(ScreenSystem* system, ScreenType type);
Screen* ScreenSystem_get_current_screen(ScreenSystem* system);

// Main loop functions

void ScreenSystem_update(ScreenSystem* system, float deltaTime);
void ScreenSystem_render(ScreenSystem* system);
void ScreenSystem_handle_input(ScreenSystem* system, int key);

// Screen registration

void ScreenSystem_register_menu_screen(ScreenSystem* system);
void ScreenSystem_register_game_screen(ScreenSystem* system);
void ScreenSystem_register_settings_screen(ScreenSystem* system);

// Specific screen data structures
typedef struct MenuScreenData {
    bool menu_open;
    int selected_option;
    Array_T menu_items;  // Array of strings
} MenuScreenData;


typedef struct GameScreenData {
    // Your game state here
    // Floor* current_floor;  // When you implement the floor system
    Camera2D camera;
    bool paused;
} GameScreenData;

typedef struct SettingsScreenData {
    Array_T settings_options;
    int current_selection;
    bool audio_enabled;
    float master_volume;
} SettingsScreenData;



#endif // SCREEN_SYSTEM_H