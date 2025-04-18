/**
** @file	uconvert.c
**
** @author	Adam Dernis
**
** @brief	C implementations of conversion library functions
*/

#ifndef STDLIB_CONVERT_SRC_INC
#define STDLIB_CONVERT_SRC_INC

#include "uconvert.h"

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
uint32_t ubound( uint32_t min, uint32_t value, uint32_t max ) {
	if( value < min ){
		value = min;
	}
	if( value > max ){
		value = max;
	}
	return value;
}

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
int ucvtdec( char *buf, int32_t value ) {
	char *bp = buf;

	if( value < 0 ) {
		*bp++ = '-';
		value = -value;
	}

	bp = ucvtdec0( bp, value );
	*bp  = '\0';

	return( bp - buf );
}

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
char *ucvtdec0( char *buf, int value ) {
	int quotient;

	quotient = value / 10;
	if( quotient < 0 ) {
		quotient = 214748364;
		value = 8;
	}
	if( quotient != 0 ) {
		buf = ucvtdec0( buf, quotient );
	}
	*buf++ = value % 10 + '0';
	return buf;
}

/**
** cvthex(buf,value)
**
** convert a 32-bit unsigned value into a minimal-length (up to
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
int ucvthex( char *buf, uint32_t value ) {
	const char hexdigits[] = "0123456789ABCDEF";
	int chars_stored = 0;

	for( int i = 0; i < 8; i += 1 ) {
		uint32_t val = value & 0xf0000000;
		if( chars_stored || val != 0 || i == 7 ) {
			++chars_stored;
			val = (val >> 28) & 0xf;
			*buf++ = hexdigits[val];
		}
		value <<= 4;
	}

	*buf = '\0';

	return( chars_stored );
}

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
int ucvtoct( char *buf, uint32_t value ) {
	int i;
	int chars_stored = 0;
	char *bp = buf;
	uint32_t val;

	val = ( value & 0xc0000000 );
	val >>= 30;
	for( i = 0; i < 11; i += 1 ){

		if( i == 10 || val != 0 || chars_stored ) {
			chars_stored = 1;
			val &= 0x7;
			*bp++ = val + '0';
		}
		value <<= 3;
		val = ( value & 0xe0000000 );
		val >>= 29;
	}
	*bp = '\0';

	return bp - buf;
}

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
int ucvtuns( char *buf, uint32_t value ) {
	char    *bp = buf;

	bp = ucvtuns0( bp, value );
	*bp = '\0';

	return bp - buf;
}

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
char *ucvtuns0( char *buf, uint32_t value ) {
	uint32_t quotient;

	quotient = value / 10;
	if( quotient != 0 ){
		buf = ucvtdec0( buf, quotient );
	}
	*buf++ = value % 10 + '0';
	return buf;
}

#endif