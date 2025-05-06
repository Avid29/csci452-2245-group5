#ifndef UPONG_INC_C_
#define UPONG_INC_C_

#include <common.h>
#include <cio.h>

#include "PS2Controller.h"

#include "pong/physics.c"
#include "pong/render.c"

PS2Controller_t pongController;

float left_vel;
float right_vel;

void
update_paddle_pos() {

    if(pongController.DataAvailable()) {
        uint8_t data;
        pongController.ReadData(&data);

        switch(data) {
        case 0x50:
	    right_vel = 0.25;
	    break;
        case 0xD0:
	    right_vel = 0;
            break;
	case 0x48:
	    // Up key pressed
	    right_vel = -0.25;
	    break;
	case 0xC8:
	    right_vel = 0;
	    break;
	}
    }

    float tmp1 = left_paddle_pos + left_vel;
    float tmp2 = right_paddle_pos + right_vel;

    if (!in_range(tmp1, 0, PHYSICS_BOUNDS_Y-PADDLE_LENGTH)){
	tmp1 = left_paddle_pos;
    }

    if (!in_range(tmp2, 0, PHYSICS_BOUNDS_Y-PADDLE_LENGTH)){
        tmp2 = right_paddle_pos;
    }

    left_paddle_pos = tmp1;
    right_paddle_pos = tmp2;
}

USERMAIN(pong)
{
    initialize_rendering();
    PS2Controller_Init(&pongController);

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
