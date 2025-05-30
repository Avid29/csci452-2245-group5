/**
** @file	ustrings.c
**
** @author	AdamDernis
**
** @brief	C implementations of string library functions
*/

#ifndef STDLIB_STRINGS_SRC_INC
#define STDLIB_STRINGS_SRC_INC

#include "ustrings.h"

/**
** pad(dst,extra,padchar) - generate a padding string
**
** @param dst     Pointer to where the padding should begin
** @param extra   How many padding bytes to add
** @param padchar What character to pad with
**
** @return Pointer to the first byte after the padding
**
** NOTE: does NOT NUL-terminate the buffer
*/
char *upad( char *dst, int extra, int padchar ) {
	while( extra > 0 ){
		*dst++ = (char) padchar;
		extra -= 1;
	}
	return dst;
}

/**
** padstr(dst,str,len,width,leftadjust,padchar - add padding characters
**                                               to a string
**
** @param dst        The destination buffer
** @param str        The string to be padded
** @param len        The string length, or -1
** @param width      The desired final length of the string
** @param leftadjust Should the string be left-justified?
** @param padchar    What character to pad with
**
** @return Pointer to the first byte after the padded string
**
** NOTE: does NOT NUL-terminate the buffer
*/
char *upadstr( char *dst, char *str, int len, int width,
	int leftadjust, int padchar ) {
int extra;

// determine the length of the string if we need to
if( len < 0 ){
len = ustrlen( str );
}

// how much filler must we add?
extra = width - len;

// add filler on the left if we're not left-justifying
if( extra > 0 && !leftadjust ){
dst = upad( dst, extra, padchar );
}

// copy the string itself
for( int i = 0; i < len; ++i ) {
*dst++ = str[i];
}

// add filler on the right if we are left-justifying
if( extra > 0 && leftadjust ){
dst = upad( dst, extra, padchar );
}

return dst;
}

/**
** sprint(dst,fmt,...) - formatted output into a string buffer
**
** @param dst The string buffer
** @param fmt Format string
**
** The format string parameter is followed by zero or more additional
** parameters which are interpreted according to the format string.
**
** NOTE:  assumes the buffer is large enough to hold the result string
**
** NOTE:  relies heavily on the x86 parameter passing convention
** (parameters are pushed onto the stack in reverse order as
** 32-bit values).
*/
void usprint( char *dst, char *fmt, ... ) {
	int32_t *ap;
	char buf[ 12 ];
	char ch;
	char *str;
	int leftadjust;
	int width;
	int len;
	int padchar;

	/*
	** Get characters from the format string and process them
	**
	** We use the "old-school" method of handling variable numbers
	** of parameters.  We assume that parameters are passed on the
	** runtime stack in consecutive longwords; thus, if the first
	** parameter is at location 'x', the second is at 'x+4', the
	** third at 'x+8', etc.  We use a pointer to a 32-bit thing
	** to point to the next "thing", and interpret it according
	** to the format string.
	*/
	
	// get the pointer to the first "value" parameter
	ap = (int *)(&fmt) + 1;

	// iterate through the format string
	while( (ch = *fmt++) != '\0' ){
		/*
		** Is it the start of a format code?
		*/
		if( ch == '%' ){
			/*
			** Yes, get the padding and width options (if there).
			** Alignment must come at the beginning, then fill,
			** then width.
			*/
			leftadjust = 0;
			padchar = ' ';
			width = 0;
			ch = *fmt++;
			if( ch == '-' ){
				leftadjust = 1;
				ch = *fmt++;
			}
			if( ch == '0' ){
				padchar = '0';
				ch = *fmt++;
			}
			while( ch >= '0' && ch <= '9' ){
				width *= 10;
				width += ch - '0';
				ch = *fmt++;
			}

			/*
			** What data type do we have?
			*/
			switch( ch ) {

			case 'c':  // characters are passed as 32-bit values
				ch = *ap++;
				buf[ 0 ] = ch;
				buf[ 1 ] = '\0';
				dst = upadstr( dst, buf, 1, width, leftadjust, padchar );
				break;

			case 'd':
				len = ucvtdec( buf, *ap++ );
				dst = upadstr( dst, buf, len, width, leftadjust, padchar );
				break;

			case 's':
				str = (char *) (*ap++);
				dst = upadstr( dst, str, -1, width, leftadjust, padchar );
				break;

			case 'x':
				len = ucvthex( buf, *ap++ );
				dst = upadstr( dst, buf, len, width, leftadjust, padchar );
				break;

			case 'o':
				len = ucvtoct( buf, *ap++ );
				dst = upadstr( dst, buf, len, width, leftadjust, padchar );
				break;

			case 'u':
				len = ucvtuns( buf, *ap++ );
				dst = upadstr( dst, buf, len, width, leftadjust, padchar );
				break;

			}
		} else {
			// no, it's just an ordinary character
			*dst++ = ch;
		}
	}

	// NUL-terminate the result
	*dst = '\0';
}

/**
** str2int(str,base) - convert a string to a number in the specified base
**
** @param str   The string to examine
** @param base  The radix to use in the conversion
**
** @return The converted integer
*/
int ustr2int( register const char *str, register int base ) {
	register int num = 0;
	register char bchar = '9';
	int sign = 1;

	// check for leading '-'
	if( *str == '-' ) {
		sign = -1;
		++str;
	}

	if( base != 10 ) {
		bchar = '0' + base - 1;
	}

	// iterate through the characters
	while( *str ) {
		if( *str < '0' || *str > bchar )
			break;
		num = num * base + *str - '0';
		++str;
	}

	// return the converted value
	return( num * sign );
}

/**
** strcat(dst,src) - append one string to another
**
** @param dst The destination buffer
** @param src The source buffer
**
** @return The dst parameter
**
** NOTE:  assumes dst is large enough to hold the resulting string
*/
char *ustrcat( register char *dst, register const char *src ) {
	register char *tmp = dst;

	while( *dst )  // find the NUL
		++dst;

	while( (*dst++ = *src++) )  // append the src string
		;

	return( tmp );
}

/**
** strcmp(s1,s2) - compare two NUL-terminated strings
**
** @param s1 The first source string
** @param s2 The second source string
**
** @return negative if s1 < s2, zero if equal, and positive if s1 > s2
*/
int ustrcmp( register const char *s1, register const char *s2 ) {

	while( *s1 != 0 && (*s1 == *s2) )
		++s1, ++s2;

	return( *s1 - *s2 );
}

/**
** strcpy(dst,src) - copy a NUL-terminated string
**
** @param dst The destination buffer
** @param src The source buffer
**
** @return The dst parameter
**
** NOTE:  assumes dst is large enough to hold the copied string
*/
char *ustrcpy( register char *dst, register const char *src ) {
	register char *tmp = dst;

	while( (*dst++ = *src++) )
		;

	return( tmp );
}

/**
** strlen(str) - return length of a NUL-terminated string
**
** @param str The string to examine
**
** @return The length of the string, or 0
*/
uint32_t ustrlen( register const char *str ) {
	register uint32_t len = 0;

	while( *str++ ) {
		++len;
	}

	return( len );
}

#endif