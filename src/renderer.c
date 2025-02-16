#include "renderer.h"
#include "SDL3/SDL_render.h"
#include "config.h"
#include "core/vec2.h"
#include "sprite_sheet.h"
#include <stdbool.h>
#include <stdio.h>

bool init_renderer(RendererContext *ctx, const char *title, int width,
                   int height) {
  // Create window
  ctx->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);

  if (!ctx->window) {
    printf("Window creation failed: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  // Create renderer
  ctx->renderer = SDL_CreateRenderer(ctx->window, NULL);

  if (!ctx->renderer) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(ctx->window);
    SDL_Quit();
    return false;
  }

  return true;
}

void renderer_begin_frame(RendererContext *ctx) {
  SDL_SetRenderDrawColor(ctx->renderer, 64, 64, 64, 255);
  SDL_RenderClear(ctx->renderer);
}
void renderer_end_frame(RendererContext *ctx) {
  SDL_RenderPresent(ctx->renderer);
}

void renderer_cleanup(RendererContext *ctx) {
  if (global_sprite_sheet) {
    destroy_sprite_sheet(global_sprite_sheet);
    global_sprite_sheet = NULL;
  }
  SDL_DestroyRenderer(ctx->renderer);
  SDL_DestroyWindow(ctx->window);
}

void renderer_draw_sprite(RendererContext *ctx, SpriteSheet sheet, int index,
                          vec2 pos, Uint8 darkness) {
  SDL_FRect dst = {pos.x * SCALED_TILE_SIZE, pos.y * SCALED_TILE_SIZE,
                   SCALED_TILE_SIZE, SCALED_TILE_SIZE};
  SDL_FRect src = get_sprite_rect_f(&sheet, index);

  SDL_SetTextureColorMod(sheet.texture, darkness, darkness, darkness);
  SDL_RenderTexture(ctx->renderer, sheet.texture, &src, &dst);
}

void render_draw_text(RendererContext *ctx, SpriteSheet sheet, const char *text,
                      vec2 pos, Uint8 darkness) {
  for (int i = 0; text[i] != '\0'; i++) {
    int spriteIndex = ALPHABET_SPRITES[text[i] - 'A'];
    renderer_draw_sprite(ctx, sheet, spriteIndex, (vec2){pos.x + i, pos.y},
                         darkness);
  }
}