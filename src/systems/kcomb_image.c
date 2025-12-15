#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include "systems/kcomb_image.h"

// Helper: clamp int
static inline int iclamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

// Draw k-comb of two rows of circles: top row (k circles) and bottom row (k circles)
// Bottom row is horizontally offset by half spacing and vertically separated to create lens-like overlaps.
// This produces a simple approximation of two wrapped sphere combs.
int KComb_GeneratePNG(const char *outPngPath, int width, int height, int k, int marginPx) {
    if (!outPngPath || width <= 0 || height <= 0 || k < 1) return 1;

    // Create blank image
    Image img = GenImageColor(width, height, BLACK);

    // Compute drawable area
    int m = iclamp(marginPx, 0, (width < height ? width : height) / 4);
    int drawW = width - 2 * m;
    int drawH = height - 2 * m;
    if (drawW <= 0 || drawH <= 0) {
        UnloadImage(img);
        return 2;
    }

    // Layout parameters
    // Horizontal spacing between circle centers in each row
    float spacingX = (float)drawW / (float)k;
    // Radius chosen to create overlaps: slightly larger than half spacing
    float radius = 0.6f * spacingX * 0.5f; // ~0.3 * spacingX
    if (radius < 2.0f) radius = 2.0f;

    // Vertical separation between the two rows; tune to get good lens overlaps
    float sepY = (float)drawH * 0.35f; // 35% of drawable height
    // Center Y positions
    float cyTop = m + drawH * 0.3f;
    float cyBot = m + drawH * 0.3f + sepY;

    // X start positions
    float startXTop = m + spacingX * 0.5f; // centers at half spacing from left
    float startXBot = m + spacingX * 0.5f + spacingX * 0.5f; // offset by half spacing

    // Draw the two rows in white
    // Top row
    for (int i = 0; i < k; ++i) {
        float cx = startXTop + i * spacingX;
        // Safety: Skip if outside image bounds
        if (cx + radius < 0 || cx - radius > width) continue;
        ImageDrawCircle(&img, (int)lroundf(cx), (int)lroundf(cyTop), (int)lroundf(radius), WHITE);
    }

    // Bottom row
    for (int i = 0; i < k; ++i) {
        float cx = startXBot + i * spacingX;
        if (cx + radius < 0 || cx - radius > width) continue;
        ImageDrawCircle(&img, (int)lroundf(cx), (int)lroundf(cyBot), (int)lroundf(radius), WHITE);
    }

    // Export PNG
    bool ok = ExportImage(img, outPngPath);
    UnloadImage(img);
    return ok ? 0 : 3;
}
