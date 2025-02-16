#pragma once
#include "core/vec2.h"
#include "sprite_sheet.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

static const int ALPHABET_SPRITES[] = {
    917, 918, 919, 920, 921, 922, 923, 924, 925, 926, 927, 928, 929,
    966, 967, 968, 969, 970, 971, 972, 973, 974, 975, 976, 977, 978};

typedef struct {
  SDL_Renderer *renderer;
  SDL_Window *window;
} RendererContext;

bool init_renderer(RendererContext *ctx, const char *title, int width,
                   int height);
void renderer_begin_frame(RendererContext *ctx);
void renderer_end_frame(RendererContext *ctx);
void renderer_cleanup(RendererContext *ctx);
void renderer_draw_sprite(RendererContext *ctx, SpriteSheet sheet, int index,
                          vec2 pos, Uint8 darkness);
void render_draw_text(RendererContext *ctx, SpriteSheet sheet, const char *text,
                      vec2 pos, Uint8 darkness);