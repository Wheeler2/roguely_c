#pragma once

#define GLOBAL_SCALE 2.0f
#define TILE_SIZE 16
#define MAP_WIDTH 50
#define MAP_HEIGHT 30

#define SCALED_TILE_SIZE (16 * GLOBAL_SCALE)

#define WINDOW_WIDTH (MAP_WIDTH * SCALED_TILE_SIZE)
#define WINDOW_HEIGHT (MAP_HEIGHT * SCALED_TILE_SIZE)

#define GLOBAL_SPRITE_SHEET_PATH "/assets/monochrome_packed.png"