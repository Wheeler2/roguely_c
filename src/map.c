#include "map.h"
#include "SDL3/SDL_render.h"
#include "config.h"
#include "core/vec2.h"
#include "renderer.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

GameMap *game_map_ptr = NULL;

void map_init(GameMap *map, SDL_Renderer *renderer) {
  map->numRooms = 0;
  map->numEnemies = 1;
  game_map_ptr = map;

  // Load tile sheet
  map->tileSheet = *global_sprite_sheet;
  // Generate the dungeon
  map_generate(map);

  Enemy *enemy = &map->enemies[0];
  enemy_init(enemy);
}

bool map_rooms_overlap(Room *a, Room *b) {
  return (a->pos.x <= b->pos.x + b->width && a->pos.x + a->width >= b->pos.x &&
          a->pos.y <= b->pos.y + b->height && a->pos.y + a->height >= b->pos.y);
}

bool map_add_room(GameMap *map, Room room) {
  // Check bounds
  if (room.pos.x < 1 || room.pos.y < 1 ||
      room.pos.x + room.width >= MAP_WIDTH - 1 ||
      room.pos.y + room.height >= MAP_HEIGHT - 1) {
    return false;
  }

  // Check overlap with existing rooms
  for (int i = 0; i < map->numRooms; i++) {
    Room *other = &map->rooms[i];
    if (map_rooms_overlap(&room, other)) {
      return false;
    }
  }

  // Add room to map
  for (int y = room.pos.y; y < room.pos.y + room.height; y++) {
    for (int x = room.pos.x; x < room.pos.x + room.width; x++) {
      map->tiles[y][x] = TILE_FLOOR;
    }
  }

  map->rooms[map->numRooms++] = room;
  return true;
}

void map_create_corridor(GameMap *map, vec2 start, vec2 end) {
  // Horizontal then vertical corridor
  int x = start.x;
  int y = start.y;

  while (x != end.x) {
    map->tiles[y][x] = TILE_FLOOR;
    x += (x < end.x) ? 1 : -1;
  }

  while (y != end.y) {
    map->tiles[y][x] = TILE_FLOOR;
    y += (y < end.y) ? 1 : -1;
  }
}

void map_generate(GameMap *map) {
  srand(time(NULL));

  // Fill map with walls initially
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      map->tiles[y][x] = TILE_WALL;
      map->visible[y][x] = false;
      map->discovered[y][x] = false;
    }
  }

  // Try to place rooms
  for (int i = 0; i < MAX_ROOMS; i++) {
    int width = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);
    int height = MIN_ROOM_SIZE + rand() % (MAX_ROOM_SIZE - MIN_ROOM_SIZE + 1);

    Room room = {.pos = {.x = rand() % (MAP_WIDTH - width - 2) + 1,
                         .y = rand() % (MAP_HEIGHT - height - 2) + 1},
                 .width = width,
                 .height = height};

    if (map_add_room(map, room)) {
      // Connect to previous room
      if (map->numRooms > 1) {
        Room *prev = &map->rooms[map->numRooms - 2];
        vec2 start = {.x = prev->pos.x + prev->width / 2,
                      .y = prev->pos.y + prev->height / 2};
        vec2 end = {.x = room.pos.x + room.width / 2,
                    .y = room.pos.y + room.height / 2};
        map_create_corridor(map, start, end);
      }
    }
  }
  map_place_special_tiles(map);
}

void map_place_special_tiles(GameMap *map) {
  if (map->numRooms < 3)
    return; // Need at least 3 rooms

  // Place player in first room
  Room *startRoom = &map->rooms[0];
  map->playerStart = map_get_random_position_in_room(startRoom);

  // Place exit in last room
  Room *exitRoom = &map->rooms[map->numRooms - 1];
  map->exitPos = map_get_random_position_in_room(exitRoom);
  map->tiles[(int)map->exitPos.y][(int)map->exitPos.x] = TILE_EXIT;

  // Place key in middle room
  Room *keyRoom = &map->rooms[map->numRooms / 2];
  map->keyPos = map_get_random_position_in_room(keyRoom);
  map->tiles[(int)map->keyPos.y][(int)map->keyPos.x] = TILE_KEY;
}

vec2 map_get_random_position_in_room(const Room *room) {
  vec2 pos = {.x = room->pos.x + (rand() % (room->width - 2)) + 1,
              .y = room->pos.y + (rand() % (room->height - 2)) + 1};
  return pos;
}

void map_update_fov(GameMap *map, vec2 playerPos) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      map->visible[y][x] = false;
    }
  }

  for (int y = -FOV_RADIUS; y <= FOV_RADIUS; y++) {
    for (int x = -FOV_RADIUS; x <= FOV_RADIUS; x++) {
      vec2 offset = {x, y};
      vec2 target = vec2_add(playerPos, offset);

      // Check if within FOV radius and in bounds
      if (vec2_dist(playerPos, target) <= FOV_RADIUS &&
          map_is_in_bounds(map, target)) {

        // Check line of sight
        if (map_has_line_of_sight(map, playerPos, target)) {
          map->visible[target.y][target.x] = true;
          map->discovered[target.y][target.x] = true;
        }
      }
    }
  }
}

void map_render(const GameMap *map, RendererContext *ctx) {

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      vec2 pos = {x, y};

      if (!map->discovered[y][x]) {
        // Render as a wall if not discovered
        renderer_draw_sprite(ctx, *global_sprite_sheet, TILE_WALL, pos, 50);
        continue;
      }

      // Get the appropriate sprite index based on tile type
      int spriteIndex = map->tiles[y][x];

      // Apply darkness to non-visible but discovered tiles
      if (!map->visible[y][x]) {
        renderer_draw_sprite(ctx, *global_sprite_sheet, spriteIndex, pos, 25);
      } else {
        renderer_draw_sprite(ctx, *global_sprite_sheet, spriteIndex, pos, 255);
      }

      // Render enemies
      for (int i = 0; i < map->numEnemies; i++) {
        enemy_render(&map->enemies[i], ctx);
      }
    }
  }
}

bool map_has_line_of_sight(const GameMap *map, vec2 start, vec2 target) {
  vec2 points[100];
  int num_points = 0;
  map_get_line(start, target, points, &num_points);

  bool hitWall = false;
  for (int i = 0; i < num_points; i++) {
    vec2 p = points[i];

    if (!map_is_in_bounds(map, p)) {
      return false;
    }

    if (map->tiles[(int)p.y][(int)p.x] == TILE_WALL) {
      // If this is the target point, make it visible
      if (p.x == target.x && p.y == target.y) {
        return true;
      }
      // If we've already hit a wall before, stop
      if (hitWall) {
        return false;
      }
      // Mark that we've hit our first wall
      hitWall = true;
    }
    if (hitWall) {
      return false;
    }
  }
  return true;
}

bool map_is_walkable(const GameMap *map, vec2 pos) {
  TileType tile = map->tiles[(int)pos.y][(int)pos.x];
  return tile == TILE_FLOOR || tile == TILE_KEY || tile == TILE_EXIT;
}

void map_regenerate(GameMap *map) {
  map->numRooms = 0;
  map->numEnemies = 0;

  for (int i = 0; i < MAX_ROOMS; i++) {
    map->rooms[i] = (Room){0};
  }

  map->playerStart = (vec2){0, 0};
  map->exitPos = (vec2){0, 0};
  map->keyPos = (vec2){0, 0};

  map_generate(map);

  // Place enemies in random rooms
  int numEnemies = 1 + rand() % MAX_ENEMIES;
  for (int i = 0; i < numEnemies; i++) {
    Enemy *enemy = &map->enemies[map->numEnemies++];
    enemy_init(enemy);
  }
}

bool map_is_in_bounds(const GameMap *map, vec2 pos) {
  return pos.x >= 0 && pos.x < MAP_WIDTH && pos.y >= 0 && pos.y < MAP_HEIGHT;
}

void map_get_line(vec2 start, vec2 end, vec2 *points, int *num_points) {
  int x1 = (int)start.x;
  int y1 = (int)start.y;
  int x2 = (int)end.x;
  int y2 = (int)end.y;

  int dx = abs(x2 - x1);
  int dy = abs(y2 - y1);
  int x = x1;
  int y = y1;
  int n = 1 + dx + dy;
  int x_inc = (x2 > x1) ? 1 : -1;
  int y_inc = (y2 > y1) ? 1 : -1;
  int error = dx - dy;
  dx *= 2;
  dy *= 2;

  *num_points = 0;

  for (; n > 0; --n) {
    points[*num_points] = (vec2){x, y};
    (*num_points)++;

    if (error > 0) {
      x += x_inc;
      error -= dy;
    } else {
      y += y_inc;
      error += dx;
    }
  }
}

void map_cleanup(GameMap *map) {
  map->tileSheet.texture = NULL;
  game_map_ptr = NULL;
}