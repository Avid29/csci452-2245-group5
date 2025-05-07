/**
** @file	graphics.h
**
** @author	Adam Dernis
**
** @brief	Graphics library definitions
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <common.h>

#define PIXEL_WIDTH     320
#define PIXEL_HEIGHT    200
#define PIXEL_COUNT     (PIXEL_WIDTH * PIXEL_HEIGHT)

// VGA Basic Palette colors
#define VGA_PALETTE_BLACK       0x00
#define VGA_PALLETE_DARKBLUE    0x01
#define VGA_PALLETE_DARKGREEN   0x02
#define VGA_PALLETE_DARKCYAN    0x03
#define VGA_PALLETE_DARKRED     0x04
#define VGA_PALLETE_PURPLE      0x05
#define VGA_PALLETE_BROWN       0x06
#define VGA_PALLETE_LIGHTGRAY   0x07
#define VGA_PALLETE_DARKGRAY    0x08
#define VGA_PALLETE_LIGHTBLUE   0x09
#define VGA_PALLETE_LIGHTGREEN  0x0A
#define VGA_PALLETE_LIGHTCYAN   0x0B
#define VGA_PALLETE_LIGHTRED    0x0C
#define VGA_PALLETE_PINK        0x0D
#define VGA_PALLETE_YELLOW      0x0E
#define VGA_PALETTE_WHITE       0x0F    

// VGA Palette pure RGB 
#define VGA_PALLETE_PURE_BLUE   0x20
#define VGA_PALLETE_PURE_RED    0x28
#define VGA_PALLETE_PURE_GREEN  0x2F

typedef struct vec2d_s
{
    float x;
    float y;
} vec2d_t;

typedef struct int2d_s
{
    short x;
    short y;
} int2d_t;

typedef int2d_t pos2d_t;
typedef int2d_t size2d_t;

typedef struct rect_s
{
    pos2d_t pos;
    size2d_t size;
} rect_t;

void
draw_rect(void *buf, rect_t r, char c);

void
draw_outline(void *buf, rect_t r, char c, int thickness);

#endif /* GRAPHICS_H */
