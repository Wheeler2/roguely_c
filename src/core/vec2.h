#pragma once

#include <stdbool.h>
typedef struct {
  int x, y;
} vec2;

vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_mult(vec2 a, vec2 b);
bool vec2_eq(vec2 a, vec2 b);
float vec2_dist(vec2 a, vec2 b);