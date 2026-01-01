# Gramarye Game Engine

Main game repository for the Gramarye game engine. A modular, ECS-based game engine built with C.

## Overview

Gramarye is a modular game engine built around an Entity Component System (ECS) architecture. The engine is split into multiple independent libraries that can be used together or separately, providing flexibility and reusability.

## Features

- **Entity Component System**: Generic ECS framework for game entity management
- **Chunk-based Rendering**: Efficient rendering of large tilemaps using chunk caching
- **Modular Architecture**: Independent libraries for different engine subsystems
- **Renderer Abstraction**: Backend-agnostic rendering interface (currently raylib implementation)
- **Event System**: Pub/sub event bus for decoupled system communication
- **Component System**: Separated component definitions and functions for clean dependencies

## Building

### Prerequisites

- CMake 3.27 or higher
- C compiler (GCC, Clang, or MSVC)
- raylib 5.5 (fetched automatically via CMake)
- Emscripten (optional, for web builds)

### Build Steps

```bash
# Clone the repository
git clone <repository-url>
cd gramarye

# Build for native platform
./build.sh

# Or manually:
mkdir build
cd build
cmake ..
make

# Run the game
./game
```

### Web Build

```bash
# Build for WebAssembly
./build_web.sh

# Or manually:
mkdir build-web
cd build-web
emcmake cmake ..
emmake make

# Run with a local server
python3 -m http.server 8000
# Then open http://localhost:8000/game.html
```

### Using Local Modules

To use local module directories instead of fetching from git:

```bash
cmake -DUSE_LOCAL_MODULES=ON ..
```

This is useful when developing multiple modules simultaneously.

## Running

### Native

```bash
./run.sh
# Or
./bin/game
```

### Web

```bash
./run_web.sh
# Or serve build-web/game.html with a web server
```

## Project Structure

```
gramarye/
├── src/              # Game source code
│   ├── main.c       # Entry point
│   ├── systems/     # Game systems
│   └── components/  # Game-specific components
├── include/         # Game headers
├── resources/       # Game assets (textures, fonts)
├── CMakeLists.txt   # Build configuration
└── README-*.md      # Additional documentation
```

## Documentation

- [README-architecture.md](README-architecture.md) - Overall architecture and module relationships
- [README-systems.md](README-systems.md) - Game systems documentation
- [README-components.md](README-components.md) - Game-specific components
- [README-rendering.md](README-rendering.md) - Rendering pipeline and camera system

## Module Dependencies

The main game depends on these modules (see README-architecture.md for details):

- gramarye-libcore - Core utilities
- gramarye-ecs - Entity Component System
- gramarye-components - Component struct definitions
- gramarye-component-functions - ECS component functions
- gramarye-chunk-controller - Chunk management
- gramarye-chunk-renderer - Chunk rendering
- gramarye-event-bus - Event system
- gramarye-renderer-interface - Renderer abstraction
- gramarye-raylib-implementation - Raylib renderer implementation

## Configuration

### Bootstrap Constants

Defined in `src/main.c`:

- `TILE_SIZE`: Size of each tile in pixels (default: 16)
- `MAP_SIZE`: Size of the tilemap in tiles (default: 128)
- `ScreenWidth`: Window width (default: 1600)
- `ScreenHeight`: Window height (default: 900)

These are bootstrap constants - GameSystem owns their usage after initialization.

## Development

### Adding New Systems

1. Create system header in `include/systems/`
2. Create system implementation in `src/systems/`
3. Register system in `GameSystem_frame()` or appropriate update function
4. Document in README-systems.md

### Adding New Components

1. Define component struct in gramarye-components
2. Add component functions in gramarye-component-functions (if needed)
3. Register component type in game initialization
4. Document in README-components.md

## Testing

```bash
# Build and run tests
./build_test.sh

# Or manually:
cd tests
mkdir build
cd build
cmake ..
make
./test_runner
```

## License

[Add your license information here]

## Contributing

[Add contribution guidelines here]

