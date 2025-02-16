#include "enemy.h"
#include "../map.h"
#include "../sprite_sheet.h"
#include <SDL3/SDL.h>

void enemy_init(Enemy *enemy) {
  // Place ghost in random room (not the first room where player starts)
  const Room *room =
      &game_map_ptr->rooms[1 + rand() % (game_map_ptr->numRooms - 1)];
  enemy->pos = map_get_random_position_in_room(room);
  enemy->lastMoveTime = 0;
  enemy->isMoving = false;
}

void enemy_update(Enemy *enemy, vec2 playerPos) {
  Uint32 currentTime = SDL_GetTicks();

  // Only move after delay
  if (currentTime - enemy->lastMoveTime < GHOST_MOVE_DELAY) {
    return;
  }

  // Calculate direction to player
  int dx = (playerPos.x > enemy->pos.x)   ? 1
           : (playerPos.x < enemy->pos.x) ? -1
                                          : 0;
  int dy = (playerPos.y > enemy->pos.y)   ? 1
           : (playerPos.y < enemy->pos.y) ? -1
                                          : 0;

  // Try horizontal movement
  vec2 newPos = {enemy->pos.x + dx, enemy->pos.y};
  if (dx != 0 && map_is_walkable(game_map_ptr, newPos)) {
    enemy->pos = newPos;
    enemy->lastMoveTime = currentTime;
    return;
  }

  // Try vertical movement
  newPos = (vec2){enemy->pos.x, enemy->pos.y + dy};
  if (dy != 0 && map_is_walkable(game_map_ptr, newPos)) {
    enemy->pos = newPos;
    enemy->lastMoveTime = currentTime;
  }
}

void enemy_render(const Enemy *enemy, RendererContext *ctx) {
  // Only render if tile is visible
  if (game_map_ptr->visible[enemy->pos.y][enemy->pos.x]) {
    renderer_draw_sprite(ctx, *global_sprite_sheet, GHOST_SPRITE_INDEX,
                         enemy->pos, 255);
  }
}