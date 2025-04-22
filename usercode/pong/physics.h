#include <common.h>
#include <lib.h>

#include "defs.h"

vec2d_t ball_pos;
vec2d_t ball_velocity;

int left_paddle_pos;
int right_paddle_pos;

void
update_physics( int deltaT );
