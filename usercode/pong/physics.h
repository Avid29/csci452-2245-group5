#include <common.h>

#include "defs.h" 
#include "graphics.h"

pos2d ball_pos;
int2d_t ball_velocity;

int left_paddle_pos;
int right_paddle_pos;

void
update_physics( int deltaT );