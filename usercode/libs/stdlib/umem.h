/**
** @file	umem.h
**
** @author	AdamDernis
**
** @brief	C declaration of memory library functions
*/

#ifndef STDLIB_MEM_H_
#define STDLIB_MEM_H_

#include <common.h>

/**
** blkmov(dst,src,len)
**
** Copy a word-aligned block from src to dst. Deals with overlapping
** buffers.
**
** If the buffer addresses aren't word-aligned or the length is not a
** multiple of four, calls memmove().
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void ublkmov( void *dst, const void *src, register uint32_t len );

/**
** memset(buf,len,value)
**
** initialize all bytes of a block of memory to a specific value
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
** @param value  Initialization value
*/
void umemset( void *buf, register uint32_t len, register uint32_t value );

/**
** memclr(buf,len)
**
** Initialize all bytes of a block of memory to zero
**
** @param buf    The buffer to initialize
** @param len    Buffer size (in bytes)
*/
void umemclr( void *buf, register uint32_t len );

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
void umemcpy( void *dst, register const void *src, register uint32_t len );

/**
** memmove(dst,src,len)
**
** Copy a block from one place to another. Deals with overlapping
** buffers
**
** @param dst   Destination buffer
** @param src   Source buffer
** @param len   Buffer size (in bytes)
*/
void umemmove( void *dst, register const void *src, register uint32_t len );

#endif