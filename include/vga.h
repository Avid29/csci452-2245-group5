/**
** @file	vga.h
**
** @author	Adam Dernis
**
** @brief	VGA Display declarations
**
*/

#ifndef VGA_H
#define	VGA_H

#include <common.h>

#define VGA_BASE        0xA0000
#define VGA_BUFFER_SIZE 0x20000
#define SCREEN_WIDTH    300
#define SCREEN_HEIGHT   200
#define SCREEN_PIXELS   (SCREEN_WIDTH * SCREEN_HEIGHT)

/**
** cio_write
**
** Copies a buffer to the VGA section
**
** @param buf     the buffer of pixels
*/
void vga_dump( const char *buf);

#endif