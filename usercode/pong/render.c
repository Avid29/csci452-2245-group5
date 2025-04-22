#include "render.h"
#include "physics.h"

char base_buffer[PIXEL_COUNT];
char graphics_buffer[PIXEL_COUNT];

void
draw_base (void ) {
    // Clear background
    rect_t r_bck = {
        .pos = { .x = 0, .y = 0 },
        .size = {.x = PIXEL_WIDTH, .y = PIXEL_HEIGHT },
    };
	draw_rect(base_buffer, r_bck, SCREEN_BACKGROUND);

    // Render bounds
    rect_t r_bounds = {
        .pos = { .x = BOARD_OFFSET_X, .y = BOARD_OFFSET_Y },
        .size = { .x = BOARD_WIDTH, .y = BOARD_HEIGHT },
    };
    draw_outline(base_buffer, r_bounds, BOARD_OUTLINE_COLOR, BOARD_THICKNESS);

    // Draw center
    rect_t r_center = {
        .pos = { .x = 159, .y = 42 },
        .size = {.x = 2, .y = 10 },
    };

    for (int i = 0; i < 7; i++) {
        draw_rect(base_buffer, r_center, BOARD_CENTERLINE_COLOR);
        r_center.pos.y += 20;
    }
}

void
initialize_rendering ( void ) {
    // Initialize the base buffer
    draw_base();
}

void
draw_paddles( void ){
    rect_t paddle = {
        .size = { .x = PADDLE_WIDTH, .y = PADDLE_LENGTH }
    };

    // left paddle
    paddle.pos.x = BOARD_OFFSET_X + BOARD_THICKNESS + PADDLE_OFFSET_X;
    paddle.pos.y = BOARD_OFFSET_Y + left_paddle_pos;
    draw_rect(graphics_buffer, paddle, LEFT_PADDLE_COLOR);

    // right paddle
    paddle.pos.x = PIXEL_WIDTH - paddle.pos.x - PADDLE_WIDTH;
    paddle.pos.y = BOARD_OFFSET_Y + right_paddle_pos;
    draw_rect(graphics_buffer, paddle, RIGHT_PADDLE_COLOR);
}

void
draw_ball( void ){

    pos2d_t p = {
        .x = (short)ball_pos.x,
        .y = (short)ball_pos.y };

    rect_t ball = {
        .pos = p,
        .size = {.x = BALL_SIZE, .y = BALL_SIZE}
    };

    ball.pos.x += BOARD_OFFSET_X+BOARD_THICKNESS;
    ball.pos.y += BOARD_OFFSET_Y+BOARD_THICKNESS;

    draw_rect(graphics_buffer, ball, BALL_COLOR);
}

void
update_display( void ) {
    // Clear physics objects
    umemcpy(graphics_buffer, base_buffer, PIXEL_COUNT);

    // Render physics objects
    draw_paddles();
    draw_ball();

    // Dump results

    display(graphics_buffer);
}
