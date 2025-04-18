/**
** @file	umem.c
**
** @author	AdamDernis
**
** @brief	C implementations of memory library functions
*/

#ifndef STDLIB_MEM_SRC_INC
#define STDLIB_MEM_SRC_INC

#include <common.h>

/**
** ublkmov(dst,src,len)
**
** Copy a word-aligned block from src to dst. Deals with overlapping
** buffers.
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void ublkmov( void *dst, const void *src, register uint32_t len ) {

	// verify that the addresses are aligned and
	// the length is a multiple of four bytes
	if( (((uint32_t)dst)&0x3) != 0 ||
		(((uint32_t)src)&0x3) != 0 ||
		(len & 0x3) != 0 ) {
		// something isn't aligned, so just use memmove()
		umemmove( dst, src, len );
		return;
	}

	// everything is nicely aligned, so off we go
	register uint32_t *dest = dst;
	register const uint32_t *source = src;

	// now copying 32-bit values
	len /= 4;

	if( source < dest && (source + len) > dest ) {
		source += len;
		dest += len;
		while( len-- > 0 ) {
			*--dest = *--source;
		}
	} else {
		while( len-- ) {
			*dest++ = *source++;
		}
	}
}

/**
** memclr(buf,len)
**
** Initialize all bytes of a block of memory to zero
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
*/
void umemclr( void *buf, register uint32_t len ) {
	register uint8_t *dest = buf;

	/*
	** We could speed this up by unrolling it and clearing
	** words at a time (instead of bytes).
	*/

	while( len-- ) {
			*dest++ = 0;
	}
}

/**
** memcpy(dst,src,len)
**
** Copy a block from one place to another
**
** May not correctly deal with overlapping buffers
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void umemcpy( void *dst, register const void *src, register uint32_t len ) {
	register uint8_t *dest = dst;
	register const uint8_t *source = src;

	/*
	** We could speed this up by unrolling it and copying
	** words at a time (instead of bytes).
	*/

	while( len-- ) {
		*dest++ = *source++;
	}
}

/**
** memmove(dst,src,len)
**
** Copy a block from one place to another. Deals with overlapping
** buffers.
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void umemmove( void *dst, const void *src, register uint32_t len ) {
	register uint8_t *dest = dst;
	register const uint8_t *source = src;

	/*
	** We could speed this up by unrolling it and copying
	** words at a time (instead of bytes).
	*/

	if( source < dest && (source + len) > dest ) {
		source += len;
		dest += len;
		while( len-- > 0 ) {
			*--dest = *--source;
		}
	} else {
		while( len-- ) {
			*dest++ = *source++;
		}
	}
}

/**
** memset(buf,len,value)
**
** initialize all bytes of a block of memory to a specific value
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
** @param value  Initialization value
*/
void umemset( void *buf, register uint32_t len, register uint32_t value ) {
	register uint8_t *bp = buf;

	/*
	** We could speed this up by unrolling it and copying
	** words at a time (instead of bytes).
	*/

	while( len-- ) {
		*bp++ = value;
	}
}

#endif