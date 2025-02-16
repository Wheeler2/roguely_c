#pragma once
#include "SDL3/SDL_render.h"
#include "config.h"
#include "core/vec2.h"
#include "entity/enemy.h"
#include "renderer.h"
#include "sprite_sheet.h"
#include <stdint.h>

#define MAX_ROOMS 15
#define MIN_ROOM_SIZE 5
#define MAX_ROOM_SIZE 10
#define FOV_RADIUS 7
#define MAX_ENEMIES 2

typedef enum {
  TILE_FLOOR = 0,
  TILE_WALL = 637,
  TILE_KEY = 571,
  TILE_EXIT = 441
} TileType;

typedef struct {
  vec2 pos;
  int width, height;
} Room;

typedef struct {
  TileType tiles[MAP_HEIGHT][MAP_WIDTH];
  bool discovered[MAP_HEIGHT][MAP_WIDTH];
  bool visible[MAP_HEIGHT][MAP_WIDTH];
  Room rooms[MAX_ROOMS];
  int numRooms;
  SpriteSheet tileSheet;
  vec2 playerStart;
  vec2 exitPos;
  vec2 keyPos;
  Enemy enemies[MAX_ENEMIES];
  int numEnemies;
} GameMap;

extern GameMap *game_map_ptr;

void map_init(GameMap *map, SDL_Renderer *renderer);
void map_render(const GameMap *map, RendererContext *ctx);
bool map_add_room(GameMap *map, Room room);
void map_generate(GameMap *map);
void map_create_corridor(GameMap *map, vec2 start, vec2 end);
bool map_rooms_overlap(Room *a, Room *b);
vec2 map_get_random_position_in_room(const Room *room);
void map_place_special_tiles(GameMap *map);
bool map_is_walkable(const GameMap *map, vec2 pos);
void map_regenerate(GameMap *map);
void map_update_fov(GameMap *map, vec2 playerPos);
bool map_is_in_bounds(const GameMap *map, vec2 pos);
bool map_has_line_of_sight(const GameMap *map, vec2 start, vec2 target);
void map_get_line(vec2 start, vec2 end, vec2 *points, int *num_points);
void map_cleanup(GameMap *map);