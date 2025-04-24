#include "physics.h"

#define X_AXIS 0
#define Y_AXIS 1

#define repeat2(x) x x
#define repeat3(x) x x x
#define repeat4(x) repeat2(repeat2(x))
#define repeat5(x) repeat2(repeat2(x)) x
#define repeat8(x) repeat2(repeat4(x))
#define repeat16(x) repeat2(repeat8(x))
#define repeat32(x) repeat2(repeat16(x))
#define repeat64(x) repeat2(repeat32(x))
#define repeat128(x) repeat2(repeat64(x))
#define repeat256(x) repeat2(repeat128(x))
#define repeat512(x) repeat2(repeat256(x))
#define repeat1024(x) repeat2(repeat512(x))
#define repeat2048(x) repeat2(repeat1024(x))
#define repeat4096(x) repeat2(repeat2048(x))

void
bounce( char axis ) {
	if (axis > 1) {
		axis = Y_AXIS;
	}

	// TODO: Stop cheating on the bounce physics
    // NOTE: Take a normal vector instead of an axis,
    // and calculate the actual new position instead of
    // negating movement along the bounce axis.   

	// Bounce on the axis
	((float*) &ball_velocity)[(unsigned char)axis] *= -1;
	((float*) &ball_pos)[(unsigned char)axis] += ((float*) &ball_velocity)[(unsigned char)axis];
}

bool_t
in_range(float value, float low, float high) {
    return value > low && value < high;
}

void
update_physics(int deltaT) {
    for (int i = 0; i < deltaT; i++)
    {
        // Apply velocity
        ball_pos.x += ball_velocity.x;
        ball_pos.y += ball_velocity.y;
      
        // Check for left paddle collision
        if (in_range(ball_pos.x, LEFT_PADDLE_X_LOW-BALL_SIZE, LEFT_PADDLE_X_HIGH)
            && in_range(ball_pos.y, left_paddle_pos-BALL_SIZE, left_paddle_pos + PADDLE_LENGTH ))
        {
            bounce(X_AXIS);
        }

        // Check for right paddle collision
        if (in_range(ball_pos.x, RIGHT_PADDLE_X_LOW-BALL_SIZE, RIGHT_PADDLE_X_HIGH)
            && in_range(ball_pos.y, right_paddle_pos-BALL_SIZE, right_paddle_pos + PADDLE_LENGTH ))
        {
            bounce(X_AXIS);
        }
        
        repeat2048(__asm__ __volatile__ ("nop");)
        repeat1024(__asm__ __volatile__ ("nop");)
        // repeat512(__asm__ __volatile__ ("nop");)


        // Detect collision with top/bottom walls
        if ( !in_range(ball_pos.y, 0, PHYSICS_BOUNDS_Y-BALL_SIZE) ) {
           bounce(Y_AXIS);
           return;
        }

        // Detect collision with side walls
        // TODO: Score
        if ( !in_range(ball_pos.x, 0, PHYSICS_BOUNDS_X-BALL_SIZE) ){
            bounce(X_AXIS);
            return;
        }
    }
}
