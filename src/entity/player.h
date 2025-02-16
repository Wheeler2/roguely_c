#pragma once
#include "../map.h"
#include "../renderer.h"
#include "SDL3/SDL_stdinc.h"
#include <stdbool.h>

typedef struct {
  vec2 pos;
  Uint32 lastMoveTime;
  bool isMoving;
  bool hasKey;
} Player;

#define PLAYER_SPRITE_INDEX 25
#define MOVE_DELAY 150 // milliseconds

void player_init(Player *player, const GameMap *map);
void player_update(Player *player, const bool *keyState);
void player_render(Player *player, RendererContext *renderer_ctx);
void player_collect_key(Player *player);
bool player_has_key(const Player *player);
void player_unlock_exit(Player *player);
void player_cleanup(Player *player);