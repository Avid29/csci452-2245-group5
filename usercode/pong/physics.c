#include "physics.h"

const vec2d_t unit_x = { .x = 1, .y = 0};
const vec2d_t unit_y = { .x = 0, .y = 1};
const vec2d_t unit_NaN = { .x = NaN, .y = NaN};

void
bounce( vec2d_t axis, vec2d_t boundary ) {
    // Normalize axis of reflection
    axis = normalize(axis);

    // Calculate overstep past boundary
    float overstep = 1;
    if (!is_nan(boundary.x)) {
        float overlap = boundary.x - ball_pos.x;
        overstep = overlap / ball_velocity.x;
    }
    if (!is_nan(boundary.y)) {
        float overlap = boundary.y - ball_pos.y;
        overstep = max(overstep, overlap / ball_velocity.y);
    }

    // Jump back to boundary
	ball_pos.x -= ball_velocity.x * overstep;
	ball_pos.y -= ball_velocity.y * overstep;

	// Reflect velocity along the axis
    ball_velocity = reflect(ball_velocity, axis);

    // Calculate remaining ball movement
    float progress = 1 - overstep;
	ball_pos.x += ball_velocity.x * progress;
	ball_pos.y += ball_velocity.y * progress;
}

bool_t
in_range(float value, float low, float high) {
    return value > low && value < high;
}

float
dot(vec2d_t v1, vec2d_t v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
}

vec2d_t
reflect(vec2d_t vector, vec2d_t normal) {
    float cof = dot(vector, normal) * 2;
    normal.x *= cof;
    normal.y *= cof;

    vec2d_t ret;
    ret.x = vector.x - normal.x;
    ret.y = vector.y - normal.y;
    return ret;
}

vec2d_t
normalize(vec2d_t vector) {
    float len = sqrt(dot(vector, vector));
    vector.x /= len;
    vector.y /= len;
    return vector;
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
            vec2d_t normal = {.x = 1};
            normal.y = map_range(ball_pos.y, left_paddle_pos, left_paddle_pos + PADDLE_LENGTH, 0.1, -0.1);
            
            vec2d_t boundary = {.x = LEFT_PADDLE_X_HIGH, .y = NaN};
            bounce(normal, boundary);
        }

        // Check for right paddle collision
        if (in_range(ball_pos.x, RIGHT_PADDLE_X_LOW-BALL_SIZE, RIGHT_PADDLE_X_HIGH)
            && in_range(ball_pos.y, right_paddle_pos-BALL_SIZE, right_paddle_pos + PADDLE_LENGTH ))
        {
            vec2d_t normal = {.x = -1};
            normal.y = map_range(ball_pos.y, right_paddle_pos, right_paddle_pos + PADDLE_LENGTH, 0.1, -0.1);

            vec2d_t boundary = {.x = RIGHT_PADDLE_X_LOW-BALL_SIZE, .y = NaN};
            bounce(normal, boundary);
        }
        
        // Detect collision with top/bottom walls
        if ( !in_range(ball_pos.y, 0, PHYSICS_BOUNDS_Y-BALL_SIZE) ) {
            bounce(unit_y, unit_NaN);
            return;
        }

        // Detect collision with side walls
        // TODO: Score
        if ( !in_range(ball_pos.x, 0, PHYSICS_BOUNDS_X-BALL_SIZE) ){
            bounce(unit_x, unit_NaN);
            return;
        }
        return;
    }
}
