#include "sprite_sheet.h"
#include "SDL3/SDL_init.h"
#include "config.h"
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>

SpriteSheet *global_sprite_sheet;

SpriteSheet create_sprite_sheet(SDL_Renderer *renderer, const char *filepath,
                                int tileSize, int spacing) {
  SDL_Surface *surface = IMG_Load(filepath);
  if (!surface) {
    SDL_Log("Unable to load image %s", SDL_GetError());
    SDL_Quit();
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    SDL_Log("Unable to create texture from %s", SDL_GetError());
    SDL_DestroySurface(surface);
    SDL_Quit();
  }

  int width = texture->w;
  int height = texture->h;

  SpriteSheet sheet;
  sheet.texture = texture;
  sheet.tileSize = tileSize;
  sheet.spacing = spacing;
  sheet.rows = (height + spacing) / (tileSize + spacing);
  sheet.cols = (width + spacing) / (tileSize + spacing);
  sheet.width = width;
  sheet.height = height;

  if (sheet.rows == 0 || sheet.cols == 0) {
    SDL_Log("Sprite sheet is empty");
    destroy_sprite_sheet(&sheet);
    SDL_Quit();
  }

  // Clean up the surface as it's no longer needed after creating the texture
  SDL_DestroySurface(surface);

  return sheet;
}

void init_global_sprite_sheet(SDL_Renderer *renderer, int tileSize,
                              int spacing) {
  if (!global_sprite_sheet) {
    global_sprite_sheet = malloc(sizeof(SpriteSheet));
    if (!global_sprite_sheet) {
      SDL_Log("Failed to allocate memory for global sprite sheet");
      return;
    }
  }
  *global_sprite_sheet = create_sprite_sheet(renderer, GLOBAL_SPRITE_SHEET_PATH,
                                             tileSize, spacing);
}

SDL_Rect get_sprite_rect(const SpriteSheet *sheet, int index) {
  int row = index / sheet->cols;
  int col = index % sheet->cols;

  SDL_Rect rect = {.x = col * (sheet->tileSize + sheet->spacing),
                   .y = row * (sheet->tileSize + sheet->spacing),
                   .w = sheet->tileSize,
                   .h = sheet->tileSize};

  return rect;
}

SDL_FRect get_sprite_rect_f(const SpriteSheet *sheet, int index) {
  int row = index / sheet->cols;
  int col = index % sheet->cols;

  SDL_FRect rect = {.x = col * (sheet->tileSize + sheet->spacing),
                    .y = row * (sheet->tileSize + sheet->spacing),
                    .w = sheet->tileSize,
                    .h = sheet->tileSize};

  return rect;
}

void destroy_sprite_sheet(SpriteSheet *sheet) {
  if (sheet && sheet->texture) {
    SDL_DestroyTexture(sheet->texture);
    sheet->texture = NULL;
  }
  free(sheet);
}