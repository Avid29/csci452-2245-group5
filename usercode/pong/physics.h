#include <common.h>
#include <lib.h>

#include "defs.h"

vec2d_t ball_pos;
vec2d_t ball_velocity;

float left_paddle_pos;
float right_paddle_pos;

float dot(vec2d_t v1, vec2d_t v2);

vec2d_t reflect(vec2d_t vector, vec2d_t normal);

vec2d_t normalize(vec2d_t vector);

void
update_physics( int deltaT );
