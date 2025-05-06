/**
** @file	uconvert.h
**
** @author	Adam Dernis
**
** @brief	C declarations of conversion library functions
*/

#ifndef STDLIB_CONVERT_H_
#define STDLIB_CONVERT_H_

#include <common.h>

/**
** cvtuns0(buf,value) - local support routine for cvtuns()
**
** Convert a 32-bit unsigned value into a NUL-terminated character string
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return Pointer to the first unused byte in the buffer
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
char *ucvtuns0( char *buf, uint32_t value );

/**
** cvtuns(buf,value)
**
** Convert a 32-bit unsigned value into a NUL-terminated character string
**
** @param buf    Result buffer
** @param value  Value to be converted
**
** @return Length of the resulting buffer
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int ucvtuns( char *buf, uint32_t value );

/**
** cvtdec0(buf,value) - local support routine for cvtdec()
**
** convert a 32-bit unsigned integer into a NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
char *ucvtdec0( char *buf, int value );

/**
** cvtdec(buf,value)
**
** convert a 32-bit signed value into a NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int ucvtdec( char *buf, int32_t value );

/**
** cvthex(buf,value)
**
** convert a 32-bit unsigned value into a mininal-length (up to
** 8-character) NUL-terminated character string
**
** @param buf    Destination buffer
** @param value  Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int ucvthex( char *buf, uint32_t value );

/**
** cvtoct(buf,value)
**
** convert a 32-bit unsigned value into a mininal-length (up to
** 11-character) NUL-terminated character string
**
** @param buf   Destination buffer
** @param value Value to convert
**
** @return The number of characters placed into the buffer
**          (not including the NUL)
**
** NOTE:  assumes buf is large enough to hold the resulting string
*/
int ucvtoct( char *buf, uint32_t value );

/**
** bound(min,value,max)
**
** This function confines an argument within specified bounds.
**
** @param min    Lower bound
** @param value  Value to be constrained
** @param max    Upper bound
**
** @return The constrained value
*/
uint32_t ubound( uint32_t min, uint32_t value, uint32_t max );

#endif