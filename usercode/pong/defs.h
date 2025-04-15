#include <common.h>

#ifndef PONG_DEFS_H_
#define PONG_DEFS_H_

#define BOARD_OFFSET_X          10
#define BOARD_OFFSET_Y          25

#define BOARD_WIDTH             PIXEL_WIDTH - (BOARD_OFFSET_X * 2)
#define BOARD_HEIGHT            165

#define BOARD_THICKNESS         4

#define SCREEN_BACKGROUND       VGA_PALETTE_BLACK
#define BOARD_OUTLINE_COLOR     VGA_PALETTE_WHITE
#define BOARD_CENTERLINE_COLOR  VGA_PALLETE_DARKGRAY

#define BALL_COLOR              VGA_PALLETE_PURE_GREEN
#define BALL_SIZE               8

#define LEFT_PADDLE_COLOR       VGA_PALLETE_PURE_BLUE
#define RIGHT_PADDLE_COLOR      VGA_PALLETE_PURE_RED

#define PADDLE_OFFSET_X         5
#define PADDLE_LENGTH           33
#define PADDLE_WIDTH            4

#endif