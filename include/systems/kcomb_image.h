#pragma once

#include <stdint.h>

// Generate a black/white k-comb wrapping image of two spheres.
// The image consists of two rows of k overlapping circles, offset to form
// a comb-like pattern (top vs bottom sphere projections). White = sphere area,
// black = background.
//
// Parameters:
// - outPngPath: filesystem path to write the PNG
// - width, height: output image dimensions in pixels
// - k: number of circle cells per row (recommended 3..6)
// - marginPx: optional margin around the pattern (in pixels)
//
// Returns 0 on success, non-zero on error.
int KComb_GeneratePNG(const char *outPngPath, int width, int height, int k, int marginPx);
