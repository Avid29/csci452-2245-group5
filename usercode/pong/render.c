#include "render.h"
#include "physics.h"

char base_buffer[PIXEL_COUNT];
char graphics_buffer [PIXEL_COUNT];

void
draw_base (void ) {
    // Clear background
    rect r_bck = {
        .pos = { .x = 0, .y = 0 },
        .size = {.x = PIXEL_WIDTH, .y = PIXEL_HEIGHT },
    };
	draw_rect(base_buffer, r_bck, SCREEN_BACKGROUND);

    // Render bounds
    rect r_bounds = {
        .pos = { .x = BOARD_OFFSET_X, .y = BOARD_OFFSET_Y },
        .size = { .x = BOARD_WIDTH, .y = BOARD_HEIGHT },
    };
    draw_outline(base_buffer, r_bounds, BOARD_OUTLINE_COLOR, BOARD_THICKNESS);

    // Draw center
    rect r_center = {
        .pos = { .x = 159, .y = 42 },
        .size = {.x = 2, .y = 10 },
    };

    for (int i = 0; i < 7; i++) {
        draw_rect(base_buffer, r_center, BOARD_CENTERLINE_COLOR);
        r_center.pos.y += 20;
    }
}

void
initialize_rendering (void ) {
    // Initialize the base buffer
    draw_base();
}

void
draw_paddles( void ){
    rect paddle = {
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
    rect pong = {
        .pos = ball_pos,
        .size = {.x = BALL_SIZE, .y = BALL_SIZE}
    };

    pong.pos.x += BOARD_OFFSET_X;
    pong.pos.y += BOARD_OFFSET_Y;

    draw_rect(graphics_buffer, pong, BALL_COLOR);
}

void
update_display() {
    // Clear physics objects
    umemcpy(graphics_buffer, base_buffer, PIXEL_COUNT);

    // Render physics objects
    draw_paddles();
    draw_ball();

    // Dump results
	write(CHAN_VGA, graphics_buffer, VGA_BUFFER_SIZE);
}