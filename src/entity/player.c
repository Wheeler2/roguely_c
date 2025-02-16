#include "player.h"
#include "../sprite_sheet.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL.h>
#include <stdio.h>

void player_init(Player *player, const GameMap *map) {
  player->pos = map->playerStart;
  player->lastMoveTime = 0;
  player->isMoving = false;
  player->hasKey = false;
  map_update_fov(game_map_ptr, player->pos);
}

void player_update(Player *player, const bool *keyState) {
  vec2 move = {0, 0};
  Uint32 currentTime = SDL_GetTicks();

  if (keyState[SDL_SCANCODE_W])
    move.y--;
  else if (keyState[SDL_SCANCODE_S])
    move.y++;
  else if (keyState[SDL_SCANCODE_A])
    move.x--;
  else if (keyState[SDL_SCANCODE_D])
    move.x++;

  if (move.x == 0 && move.y == 0) {
    player->isMoving = false;
    return;
  } else {
    player->isMoving = true;
  }

  if (currentTime - player->lastMoveTime < MOVE_DELAY || !player->isMoving) {
    return;
  }

  vec2 newPos = vec2_add(player->pos, move);
  if (map_is_walkable(game_map_ptr, newPos)) {
    player->pos = newPos;
    player->lastMoveTime = currentTime;
    map_update_fov(game_map_ptr, newPos);
    printf("Player moved to: %d, %d\n", player->pos.x, player->pos.y);

    // Check if player collected the key
    if (vec2_eq(newPos, game_map_ptr->keyPos)) {
      player_collect_key(player);
    }

    // Check if player reached the exit
    if (newPos.x == game_map_ptr->exitPos.x &&
        newPos.y == game_map_ptr->exitPos.y) {
      if (player_has_key(player)) {
        player_unlock_exit(player);
      } else {
        printf("Player needs a key to exit!\n");
      }
    }
  }
}

void player_render(Player *player, RendererContext *renderer_ctx) {
  renderer_draw_sprite(renderer_ctx, *global_sprite_sheet, PLAYER_SPRITE_INDEX,
                       player->pos, 255);
}

void player_collect_key(Player *player) {
  player->hasKey = true;
  game_map_ptr->tiles[game_map_ptr->keyPos.y][game_map_ptr->keyPos.x] =
      TILE_FLOOR;
  printf("Player collected the key!\n");
}

bool player_has_key(const Player *player) { return player->hasKey; }

void player_unlock_exit(Player *player) {
  printf("Player reached the exit!\n");
  map_regenerate(game_map_ptr);
  player_init(player, game_map_ptr);
}

void player_cleanup(Player *player) {}