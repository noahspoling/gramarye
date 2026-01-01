# Gramarye Rendering System

Documentation for the rendering pipeline, camera system, and rendering architecture.

## Rendering Architecture

The rendering system uses a layered architecture:

1. **Renderer Interface** (gramarye-renderer-interface) - Abstract rendering API
2. **Renderer Implementation** (gramarye-raylib-implementation) - Concrete raylib implementation
3. **Chunk Renderer** (gramarye-chunk-renderer) - Chunk-based tilemap rendering
4. **Render System** (gramarye/src/systems/render_system.c) - Entity and UI rendering

## Renderer Interface

The renderer interface provides backend-agnostic rendering:

- **Render Commands**: Abstract command structure for drawing operations
- **Camera Transformations**: World-to-screen and screen-to-world coordinate conversion
- **Window Management**: Window creation, configuration, lifecycle
- **Input Provider**: Backend-agnostic input handling

### Render Commands

Render commands are executed via `Renderer_execute_command()`:

- `RENDER_COMMAND_TYPE_RECTANGLE`: Draw filled rectangle
- `RENDER_COMMAND_TYPE_RECTANGLE_LINES`: Draw rectangle outline
- `RENDER_COMMAND_TYPE_TEXTURE`: Draw texture with source/destination rectangles
- `RENDER_COMMAND_TYPE_TEXTURE_PRO`: Draw texture with rotation and origin
- `RENDER_COMMAND_TYPE_TEXT`: Draw text
- `RENDER_COMMAND_TYPE_CLEAR`: Clear background

### Coordinate Spaces

- **World Space**: Game world coordinates (tile coordinates * tile size)
- **Screen Space**: Window pixel coordinates
- **Logical Space**: Render texture logical size (for aspect fitting)

## Camera System

The camera system provides 2D camera functionality with zoom and aspect fitting.

### Camera Structure

```c
typedef struct {
    Vector2 pos;           // World position in pixels
    float zoom;            // Zoom factor (1.0 = 1:1)
    float minZoom;         // Minimum zoom level
    float maxZoom;         // Maximum zoom level
    float speed;           // Pan speed (unused currently)
    Vector2 logicalSize;   // Render texture logical size
} Camera2DEx;
```

### Aspect Fit

Aspect fit ensures the game renders at the correct aspect ratio regardless of window size:

```c
typedef struct {
    Rectangle dest;  // Destination rectangle on screen (letterboxed/pillarboxed)
    float scale;      // Scale from logical to screen
} AspectFit;
```

The aspect fit is computed based on:
- Logical size (game's intended resolution)
- Window size (actual window dimensions)
- Maintains aspect ratio with letterboxing/pillarboxing

### Camera Functions

- `Camera_Init()`: Initialize camera with logical size
- `Camera_ComputeAspectFit()`: Compute aspect fit for current window
- `Camera_WorldToScreen()`: Convert world coordinates to screen coordinates
- `Camera_ScreenToWorld()`: Convert screen coordinates to world coordinates
- `Camera_ClampToBounds()`: Clamp camera to stay within map bounds

### Camera Update Flow

1. **Follow Player**: Update camera position to follow player entity
2. **Compute Aspect Fit**: Calculate aspect fit for current window size
3. **Clamp to Bounds**: Ensure camera stays within map boundaries

## Rendering Pipeline

### Frame Rendering Order

1. **Begin Frame**: `Renderer_begin_frame()` - Clear and prepare for drawing
2. **Clear Background**: Draw full-screen rectangle for background
3. **Render Chunks**: `ChunkRenderSystem_render()` - Render tilemap chunks
4. **Render Entities**: Render entities (player, NPCs, etc.)
5. **Render UI**: Render UI elements and debug overlays
6. **End Frame**: `Renderer_end_frame()` - Present frame

### Chunk Rendering

Chunks are rendered via gramarye-chunk-renderer:

1. **Observer-Based Loading**: Chunks within render radius of observers are loaded
2. **Chunk Textures**: Each chunk is rendered to a render texture once
3. **Caching**: Chunk textures are cached until marked dirty
4. **Screen Rendering**: Visible chunks are rendered to screen using camera transform

Chunk rendering uses the renderer interface for:
- World-to-screen coordinate conversion
- Camera zoom and aspect fit
- Render texture management

### Entity Rendering

Entities are rendered by:

1. **Get Components**: Retrieve Position and Sprite components
2. **World to Screen**: Convert entity world position to screen position
3. **Get Sprite Rect**: Get source rectangle from Atlas
4. **Draw Sprite**: Draw sprite using renderer (currently raylib directly - should use renderer interface)

### Debug Rendering

Debug overlays are rendered when debug mode is enabled:

- **Last Click**: Red rectangle at last clicked tile position
- **Tile Coordinates**: (future) Display tile coordinates
- **Chunk Boundaries**: (future) Display chunk boundaries

## Coordinate Transformations

### World to Screen

```c
Vector2 screenPos = Camera_WorldToScreen(&camera, aspectFit, worldPos);
```

Transformation steps:
1. Apply camera position offset
2. Apply zoom
3. Apply aspect fit scale
4. Convert to screen coordinates

### Screen to World

```c
Vector2 worldPos = Camera_ScreenToWorld(&camera, aspectFit, screenPos);
```

Inverse transformation:
1. Convert from screen coordinates
2. Apply inverse aspect fit scale
3. Apply inverse zoom
4. Apply inverse camera position offset

### Tile Coordinates

Tile coordinates are converted via ChunkRenderSystem:

```c
int tileX, tileY;
ChunkRenderSystem_handle_click(&chunkRenderer, mousePos, &camera, &aspectFit, &tileX, &tileY);
```

This converts:
1. Screen coordinates → World coordinates
2. World coordinates → Tile coordinates (using floor division)

## Rendering Backend

Currently uses raylib via gramarye-raylib-implementation:

- **Window Creation**: raylib's `InitWindow()`
- **Drawing**: raylib's draw functions (`DrawRectangle`, `DrawTexturePro`, etc.)
- **Input**: raylib's input functions (`IsKeyPressed`, `GetMousePosition`, etc.)

The renderer interface abstraction allows switching to other backends (OpenGL, Vulkan, etc.) by providing a different implementation.

## Performance Considerations

### Chunk Caching

- Chunks are rendered to textures once and cached
- Only re-rendered when marked dirty
- Reduces draw calls significantly for large tilemaps

### Render Commands

- Render commands are executed immediately (no batching currently)
- Future: Could batch commands for better performance

### Aspect Fit

- Aspect fit is computed once per frame
- Uses renderer interface to get window dimensions
- Fallback to fixed size if renderer unavailable

## Future Improvements

- **Entity Rendering via Renderer Interface**: Currently uses raylib directly, should use renderer interface
- **Render Command Batching**: Batch commands for better performance
- **Multiple Render Layers**: Support for multiple render layers (background, entities, UI)
- **Particle System**: Add particle rendering system
- **Lighting**: Add lighting system
- **Post-Processing**: Add post-processing effects

## Integration with Other Systems

### Input System

Input system provides mouse position in screen coordinates, which is converted to world/tile coordinates for rendering.

### Movement System

Movement system updates entity positions, which are then rendered by the render system.

### Tile Edit System

Tile edit system uses screen-to-world conversion to determine which tile was clicked.

### Chunk Systems

Chunk renderer and chunk manager work together:
- Chunk manager marks chunks dirty when tiles change
- Chunk renderer checks dirty state and re-renders chunks
- Dirty flags are cleared after rendering

