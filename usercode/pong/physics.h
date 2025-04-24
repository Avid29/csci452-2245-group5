#include <common.h>
#include <lib.h>

#include "defs.h"

vec2d_t ball_pos;
vec2d_t ball_velocity;

float left_paddle_pos;
float right_paddle_pos;

void
update_physics( int deltaT );
