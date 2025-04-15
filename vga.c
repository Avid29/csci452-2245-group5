/**
** @file	vga.c
**
** @author	Adam Dernis
**
** @brief	VGA Display method implementations
**
*/

#define KERNEL_SRC

#include <common.h>

#include <vga.h>

void vga_dump( const char *buf){
    memcpy((void*)VGA_BASE, buf, VGA_BUFFER_SIZE/2);
}