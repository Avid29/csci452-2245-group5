#include "physics.h"

void
update_physics(int deltaT) {
    for (int i = 0; i < deltaT; i++)
    {
        // Apply velocity
        ball_pos.x += ball_velocity.x;
        ball_pos.y += ball_velocity.y;
    
        // Detect collision with top/bottom walls
        if ( ball_pos.y < 0 || ball_pos.y > PHYSICS_BOUNDS_Y-BALL_SIZE ){
            ball_velocity.y *= -1;
            ball_pos.y += ball_velocity.y;
        }

        // TODO: Paddle collision
       
        // Detect collision with side walls
        // TODO: Score
        if ( ball_pos.x < 0 || ball_pos.x > PHYSICS_BOUNDS_X-BALL_SIZE ){
            ball_velocity.x *= -1;
            ball_pos.x += ball_velocity.x;
        }
    }
}
