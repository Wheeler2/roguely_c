#include "vec2.h"
#include <math.h>

vec2 vec2_add(vec2 a, vec2 b) { return (vec2){a.x + b.x, a.y + b.y}; }
vec2 vec2_sub(vec2 a, vec2 b) { return (vec2){a.x - b.x, a.y - b.y}; }
vec2 vec2_mult(vec2 a, vec2 b) { return (vec2){a.x * b.x, a.y * b.y}; }
bool vec2_eq(vec2 a, vec2 b) { return a.x == b.x && a.y == b.y; }
float vec2_dist(vec2 a, vec2 b) {
  return (float)sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}