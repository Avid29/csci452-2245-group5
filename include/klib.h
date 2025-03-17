/*
** @file    klib.h
**
** @author  Warren R. Carithers
**
** Additional support functions for the kernel.
*/

#ifndef KLIB_H_
#define KLIB_H_

#include <common.h>

#ifndef ASM_SRC

#include <x86/ops.h>

/**
** Name:    put_char_or_code( ch )
**
** Description: Prints a character on the console, unless it
** is a non-printing character, in which case its hex code
** is printed
**
** @param ch    The character to be printed
*/
void put_char_or_code( int ch );

#endif  /* !ASM_SRC */

#endif
