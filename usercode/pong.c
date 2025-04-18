#ifndef UPONG_INC_C_
#define UPONG_INC_C_

#include <common.h>

#include "pong/physics.c"
#include "pong/render.c"

USERMAIN(pong)
{
    initialize_rendering();

    // Initialize paddle pos
    left_paddle_pos = 25;
    right_paddle_pos = 55;

    // Spawn ball
    ball_pos.x = 50;
    ball_pos.y = 50;

    // Init velocity
    ball_velocity.x = 1;
    ball_velocity.y = 1;

    int time = gettime();

    // Begin game loop
    while (true)
    {
        // Detect change in time
        int newTime = gettime();
        int deltaT = newTime - time;

        update_physics(deltaT);
        update_display();
        time = newTime;
    }
}
#endif