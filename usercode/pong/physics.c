#include "physics.h"

void
update_physics(int deltaT) {
    for (int i = 0; i < deltaT; i++)
    {
        // Apply velocity
        ball_pos.x += ball_velocity.x;
        ball_pos.y += ball_velocity.y;
    
        // Detect collision with top/bottom walls
        if (ball_pos.y < BOARD_THICKNESS || ball_pos.y > BOARD_HEIGHT-BALL_SIZE-BOARD_THICKNESS){
            ball_velocity.y *= -1;
            ball_pos.y += ball_velocity.y;
        }

        // TODO: Paddle collision
        
        // Detect collision with side walls
        // TODO: Score
        if (ball_pos.x < BOARD_THICKNESS || ball_pos.x > BOARD_WIDTH-BALL_SIZE-BOARD_THICKNESS ){
            ball_velocity.x *= -1;
            ball_pos.x += ball_velocity.x;
        }
    }
}