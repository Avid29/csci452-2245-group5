/*
** @file klibc.c
**
** @author  Warren R. Carithers
**
** Additional support functions for the kernel.
**
*/

#include <klib.h>
#include <cio.h>

/**
** Name:    put_char_or_code( ch )
**
** Description: Prints a character on the console, unless it
** is a non-printing character, in which case its hex code
** is printed
**
** @param ch    The character to be printed
*/
void put_char_or_code( int ch ) {

    if( ch >= ' ' && ch < 0x7f ) {
        cio_putchar( ch );
    } else {
        cio_printf( "\\x%02x", ch );
    }
}
