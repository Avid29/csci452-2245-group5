#ifndef UPONG_INC_C_
#define UPONG_INC_C_

#include <common.h>

#include "pong/physics.c"
#include "pong/render.c"

float left_vel;
float right_vel;

void
update_paddle_pos() {
    left_paddle_pos += left_vel;
    right_paddle_pos += right_vel;

    if (!in_range(left_paddle_pos, 0, PHYSICS_BOUNDS_Y-PADDLE_LENGTH)){
        left_vel *= -1;
        left_paddle_pos += left_vel;
    }

    if (!in_range(right_paddle_pos, 0, PHYSICS_BOUNDS_Y-PADDLE_LENGTH)){
        right_vel *= -1;
        right_paddle_pos += right_vel;
    }
}

USERMAIN(pong)
{
    initialize_rendering();

    // Initialize paddle pos
    left_paddle_pos = 25;
    right_paddle_pos = 55;

    left_vel = .1;
    right_vel = -.15;

    // Spawn ball
    ball_pos.x = 50;
    ball_pos.y = 50;

    // Init velocity
    ball_velocity.x = .2;
    ball_velocity.y = .2;

    int time = gettime();

    // Begin game loop
    while (true)
    {
        // Detect change in time
        int newTime = gettime();
        int deltaT = newTime - time;

        update_paddle_pos();
        update_physics(deltaT);
        update_display();
        time = newTime;
    }
}
#endif
