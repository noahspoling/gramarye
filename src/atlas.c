// #include "raylib.h"
// #include "atlas.h"
// #include "array.h"
// #include "assert.h"
// #include "mem.h"

// Atlas* Atlas_new_empty(const char* imagePath) {
//     assert(imagePath);
//     Atlas* atlas;
//     NEW(atlas);
//     assert(atlas);
//     atlas->texture = LoadTexture(imagePath);
//     atlas->rectangles = Array_new(0, sizeof(Rectangle));
//     return atlas;
// }

// Atlas* Atlas_new_with_rects(const char* imagePath, int tileWidth, int tileHeight, int rows, int cols) {
//     assert(imagePath);
//     Atlas* atlas;
//     NEW(atlas);
//     assert(atlas);
//     atlas->texture = LoadTexture(imagePath);
//     atlas->rectangles = Array_new(0, sizeof(Rectangle));

//     for (int row = 0; row < rows; row++) {
//         for (int col = 0; col < cols; col++) {
//             Rectangle rect = {
//                 col * tileWidth,
//                 row * tileHeight,
//                 tileWidth,
//                 tileHeight
//             };
//             Array_push(atlas->rectangles, &rect);
//         }
//     }

//     return atlas;
// }

// void Atlas_Destroy(Atlas* atlas) {
//     assert(atlas);
//     UnloadTexture(atlas->texture);
//     Array_Destroy(atlas->rectangles);
//     FREE(atlas);
// }
// Texture2D* Atlas_GetTexture(const Atlas* atlas);
// Rectangle Atlas_GetRect(const Atlas* atlas, int index);
