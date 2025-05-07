#ifndef UPONG_INC_C_
#define UPONG_INC_C_

#include <common.h>
#include <cio.h>

#include "pong/PS2PongController.h"

#include "pong/physics.c"
#include "pong/render.c"

// The PS2 keyboard pong controller
PS2PongController_t pongController;

// Speed of the paddles
float left_vel;
float right_vel;

void
update_paddle_pos() {
    // Update the paddles according to keyboard input
    if(PS2PongController_Update(&pongController)) {
        if(pongController.inputStates.reset) {
	    // Reset ball
            ball_pos.x = 100;
            ball_pos.y = 50;
            ball_vel.x = -0.3;
            ball_vel.y = 0.4;
            return;
	}

        left_vel  = -PADDLE_VEL_SCALE*(pongController.inputStates.lUp 
	                             - pongController.inputStates.lDown);
	right_vel = -PADDLE_VEL_SCALE*(pongController.inputStates.rUp
	                             - pongController.inputStates.rDown);
    }

    // Pretend to move...
    float tmp1 = left_paddle_pos + left_vel;
    float tmp2 = right_paddle_pos + right_vel;

    // If the left paddle would move out of range, clamp it
    if (!in_range(tmp1, 0, PHYSICS_BOUNDS_Y-PADDLE_LENGTH)){
	tmp1 = left_paddle_pos;
    }

    // If the right paddle would move out of range, clamp it
    if (!in_range(tmp2, 0, PHYSICS_BOUNDS_Y-PADDLE_LENGTH)){
        tmp2 = right_paddle_pos;
    }

    // Actually update the paddle positions
    left_paddle_pos = tmp1;
    right_paddle_pos = tmp2;
}

USERMAIN(pong)
{
    initialize_rendering();
    PS2PongController_Init(&pongController);

    // Initialize paddle pos
    left_paddle_pos = 25;
    right_paddle_pos = 55;

    left_vel = 0.0;
    right_vel = 0.0;

    // Spawn ball
    ball_pos.x = 50;
    ball_pos.y = 50;

    // Init velocity
    ball_vel.x = .5;
    ball_vel.y = .3;

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
