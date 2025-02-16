#pragma once
#include "SDL3/SDL_rect.h"
#include <SDL3/SDL.h>

typedef struct {
  SDL_Texture *texture;
  int tileSize; // Size of each sprite tile (assuming square tiles)
  int rows;     // Number of rows in sprite sheet
  int cols;     // Number of columns in sprite sheet
  int width;
  int height;
  int spacing; // Spacing between sprites (if any)
} SpriteSheet;

extern SpriteSheet *global_sprite_sheet;

SpriteSheet create_sprite_sheet(SDL_Renderer *renderer, const char *filepath,
                                int tileSize, int spacing);
void init_global_sprite_sheet(SDL_Renderer *renderer, int tileSize,
                              int spacing);
SDL_Rect get_sprite_rect(const SpriteSheet *sheet, int index);
SDL_FRect get_sprite_rect_f(const SpriteSheet *sheet, int index);
void destroy_sprite_sheet(SpriteSheet *sheet);