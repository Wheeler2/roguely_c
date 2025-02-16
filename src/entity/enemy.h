#pragma once
#include "../core/vec2.h"
#include "../renderer.h"
#include <stdbool.h>

#define GHOST_SPRITE_INDEX 419 // Adjust based on your sprite sheet
#define GHOST_MOVE_DELAY 500   // Milliseconds between moves

typedef struct {
  vec2 pos;
  Uint32 lastMoveTime;
  bool isMoving;
} Enemy;

void enemy_init(Enemy *enemy);
void enemy_update(Enemy *enemy, vec2 pos);
void enemy_render(const Enemy *enemy, RendererContext *ctx);