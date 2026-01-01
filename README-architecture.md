# Gramarye Architecture

Overall architecture and module relationships for the Gramarye game engine.

## Module Overview

The Gramarye engine is split into multiple independent modules:

1. **gramarye-libcore** - Foundation library with data structures and utilities
2. **gramarye-ecs** - Entity Component System framework
3. **gramarye-components** - Component struct definitions
4. **gramarye-component-functions** - ECS functions for components
5. **gramarye-chunk-controller** - Chunk management and tile updates
6. **gramarye-chunk-renderer** - Chunk-based rendering system
7. **gramarye-event-bus** - Pub/sub event system
8. **gramarye-query-engine** - Query language for ECS debugging
9. **gramarye-renderer-interface** - Abstract renderer interface
10. **gramarye-raylib-implementation** - Raylib renderer implementation
11. **gramarye** - Main game (this repository)

## Dependency Graph

```
gramarye-libcore (foundation)
    ├── gramarye-ecs
    │   ├── gramarye-component-functions
    │   │   ├── gramarye-components
    │   │   └── gramarye-chunk-renderer
    │   └── gramarye-query-engine
    ├── gramarye-event-bus
    │   └── gramarye-chunk-controller
    │       └── gramarye-chunk-renderer (optional)
    ├── gramarye-renderer-interface
    │   ├── gramarye-raylib-implementation
    │   └── gramarye-chunk-renderer
    └── gramarye (main game)
        └── (uses all modules)
```

## Module Relationships

### Foundation Layer

**gramarye-libcore**
- Provides: Table, Arena, Sets, Lists, Hash functions, Threading
- Used by: All other modules
- Dependencies: None (pure C library)

### ECS Layer

**gramarye-ecs**
- Provides: Entity management, Component storage, System registration, Query utilities
- Dependencies: gramarye-libcore
- Used by: gramarye-component-functions, gramarye-query-engine, gramarye (main game)

**gramarye-components**
- Provides: Component struct definitions (Position, Health, Sprite, etc.)
- Dependencies: gramarye-libcore
- Used by: gramarye-component-functions, gramarye-chunk-controller, gramarye-chunk-renderer

**gramarye-component-functions**
- Provides: ECS helper functions (Position_add, Health_get, etc.)
- Dependencies: gramarye-ecs, gramarye-components, gramarye-libcore
- Used by: gramarye-chunk-renderer, gramarye (main game)

### Rendering Layer

**gramarye-renderer-interface**
- Provides: Abstract Renderer, InputProvider, Camera interfaces
- Dependencies: None (pure interface)
- Used by: gramarye-raylib-implementation, gramarye-chunk-renderer, gramarye (main game)

**gramarye-raylib-implementation**
- Provides: Concrete raylib implementation of renderer interface
- Dependencies: gramarye-renderer-interface, raylib
- Used by: gramarye (main game)

### Chunk System

**gramarye-chunk-controller**
- Provides: Tile update queue, Chunk dirty tracking, Event publishing
- Dependencies: gramarye-libcore, gramarye-components, gramarye-event-bus
- Used by: gramarye (main game)

**gramarye-chunk-renderer**
- Provides: Chunk-based rendering, Observer system, Coordinate transformations
- Dependencies: gramarye-libcore, gramarye-ecs, gramarye-components, gramarye-component-functions, gramarye-renderer-interface
- Optional: gramarye-chunk-controller (for dirty tracking)
- Used by: gramarye (main game)

### Communication Layer

**gramarye-event-bus**
- Provides: Pub/sub event system
- Dependencies: gramarye-libcore
- Used by: gramarye-chunk-controller, gramarye (main game)

### Utility Layer

**gramarye-query-engine**
- Provides: SQL-like query language for ECS debugging
- Dependencies: gramarye-ecs, gramarye-libcore
- Used by: gramarye (main game, optional)

## Data Flow

### Tile Update Flow

```
User Input
    ↓
TileEditSystem (queues update)
    ↓
TileUpdateQueue
    ↓
ChunkManagerSystem (processes updates)
    ├── Apply to Tilemap
    ├── Mark chunks dirty
    └── Publish EVENT_TILE_UPDATED
    ↓
ChunkRenderSystem (checks dirty chunks)
    ├── Re-render dirty chunks
    └── Clear dirty flags
```

### Rendering Flow

```
Game Loop
    ↓
InputSystem (poll input)
    ↓
GameSystem_frame()
    ├── Process input commands
    ├── Update chunk manager (process tile updates)
    ├── Update chunk renderer (load/unload chunks)
    ├── Update camera
    └── Render
        ├── ChunkRenderSystem_render() (chunk textures)
        └── RenderSystem_render() (entities, UI)
```

### Component Access Flow

```
System needs component
    ↓
Include gramarye-component-functions header
    ├── Includes component struct definition
    └── Provides ECS functions
    ↓
Use Position_get(), Health_add(), etc.
    ↓
Functions use gramarye-ecs internally
```

## Include Order Guidelines

### Component Functions

When using component functions, include order matters:

```c
// 1. Component-functions headers first (they include struct definitions)
#include "core/position.h"  // Position struct + Position_get()
#include "textures/atlas.h"  // Atlas struct (with raylib types) + Atlas_getRect()

// 2. Then other headers
#include "tilemap/tilemap.h"
```

### Chunk Renderer

```c
// 1. Component-functions headers (for Position_get, Atlas_getRect)
#include "core/position.h"
#include "textures/atlas.h"

// 2. Chunk renderer header
#include "gramarye_chunk_renderer/chunk_render_system.h"

// 3. Optional: Chunk controller (checked via __has_include)
#ifdef __has_include
    #if __has_include("gramarye_chunk_controller/chunk_manager_system.h")
        #include "gramarye_chunk_controller/chunk_manager_system.h"
    #endif
#endif
```

## Memory Management

All modules use arena-based memory management:

- **Arena Allocation**: All dynamic allocations use `Arena_alloc()`
- **Lifetime**: Memory is freed when arena is destroyed
- **No Manual Cleanup**: Most structures don't need explicit cleanup (arena handles it)

## Threading

Threading is optional and controlled by `USE_THREADING`:

- **Event Bus**: Thread-safe when `USE_THREADING` is defined
- **Tile Update Queue**: Thread-safe when `USE_THREADING` is defined
- **Input System**: Currently single-threaded (threading code is commented out)

## Module Independence

Modules are designed to be independent:

- **gramarye-chunk-renderer** can work without **gramarye-chunk-controller** (uses local dirty tracking)
- **gramarye-query-engine** is optional (debugging tool)
- **gramarye-raylib-implementation** can be replaced with another renderer implementation

This allows:
- Using modules individually in other projects
- Replacing implementations (e.g., different renderer backend)
- Optional features (e.g., query engine only in debug builds)

## Integration Patterns

### Minimal Integration

Just ECS and components:

```c
// Only need: libcore, ecs, components, component-functions
#include "gramarye_ecs/ecs.h"
#include "core/position.h"
```

### Full Integration

All systems:

```c
// Use all modules
#include "gramarye_chunk_renderer/chunk_render_system.h"
#include "gramarye_chunk_controller/chunk_manager_system.h"
#include "gramarye_event_bus/event_bus.h"
// ... etc
```

## Future Extensibility

The architecture supports:

- **Multiple Renderer Backends**: Implement gramarye-renderer-interface with different backends
- **Custom Components**: Add new components in gramarye-components and functions in gramarye-component-functions
- **Optional Systems**:** Make systems optional via CMake options
- **Plugin System**: Add systems as plugins (future work)

