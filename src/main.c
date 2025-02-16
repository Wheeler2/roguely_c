#include "SDL3/SDL_init.h"
#include "SDL3/SDL_stdinc.h"
#include "config.h"
#include "entity/enemy.h"
#include "entity/player.h"
#include "map.h"
#include "renderer.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>

#define GAME_OVER_DELAY 2000              // 2 seconds delay before restart
#define GAME_START_INSTRUCTIONS_TIME 5000 // 3 seconds delay before disappearing

bool game_over = false;
bool game_started = false;
Uint32 game_start_time = 0;
Uint32 game_over_start_time = 0;
int running = 1;

int SDL_main(int argc, char *argv[]) {
  RendererContext renderer_ctx;
  SDL_Event event;
  GameMap gameMap;
  Player player;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return 1;
  }

  // Initialize renderer
  if (!init_renderer(&renderer_ctx, "Roguely", WINDOW_WIDTH, WINDOW_HEIGHT)) {
    return 1;
  }

  // Initialize global sprite sheet
  init_global_sprite_sheet(renderer_ctx.renderer, TILE_SIZE, 0);

  // Initialize game
  map_init(&gameMap, renderer_ctx.renderer);
  player_init(&player, &gameMap);
  game_start_time = SDL_GetTicks();

  // Main game loop
  while (running) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = 0;
      }
    }
    const bool *keyState = SDL_GetKeyboardState(NULL);

    // Handle game over state
    if (game_over) {
      if (SDL_GetTicks() - game_over_start_time >= GAME_OVER_DELAY) {
        // Reset game
        map_regenerate(&gameMap);
        player_init(&player, &gameMap);
        game_over = false;
      }
    } else {
      player_update(&player, keyState);

      for (int i = 0; i < gameMap.numEnemies; i++) {
        enemy_update(&gameMap.enemies[i], player.pos);
      }

      for (int i = 0; i < gameMap.numEnemies; i++) {
        if (vec2_eq(player.pos, gameMap.enemies[i].pos)) {
          printf("Player caught by ghost! Game Over.\n");
          game_over = true;
          game_over_start_time = SDL_GetTicks();
          break;
        }
      }
    }

    renderer_begin_frame(&renderer_ctx);

    map_render(&gameMap, &renderer_ctx);
    player_render(&player, &renderer_ctx);

    if (!game_started) {
      render_draw_text(&renderer_ctx, *global_sprite_sheet,
                       "FIND THE KEY AND ESCAPE", (vec2){MAP_WIDTH / 2 - 12, 1},
                       255);
      render_draw_text(&renderer_ctx, *global_sprite_sheet, "DONT GET CAUGHT",
                       (vec2){MAP_WIDTH / 2 - 7, 3}, 255);

      if (SDL_GetTicks() - game_start_time > GAME_START_INSTRUCTIONS_TIME) {
        game_started = true;
      }
    }

    if (game_over) {
      render_draw_text(&renderer_ctx, *global_sprite_sheet, "GAME OVER",
                       (vec2){MAP_WIDTH / 2 - 4, MAP_HEIGHT / 2}, 255);

      if (GAME_OVER_DELAY - (SDL_GetTicks() - game_over_start_time) < 1000) {
        render_draw_text(&renderer_ctx, *global_sprite_sheet, "RESTARTING",
                         (vec2){MAP_WIDTH / 2 - 4, MAP_HEIGHT / 2 + 1}, 255);
      }

      printf("Restarting in %.1f seconds...\n",
             (GAME_OVER_DELAY - (SDL_GetTicks() - game_over_start_time)) /
                 1000.0f);
    }

    renderer_end_frame(&renderer_ctx);
  }

  // Cleanup
  player_cleanup(&player);
  map_cleanup(&gameMap);
  renderer_cleanup(&renderer_ctx);
  SDL_Quit();

  return 0;
}