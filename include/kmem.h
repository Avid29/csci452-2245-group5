/**
** @file	kmem.h
**
** @author	Warren R. Carithers
** @author	Kenneth Reek
** @author	4003-506 class of 20013
**
** @brief	Support for dynamic memory allocation within the OS.
**
** The list of free blocks is ordered by address to facilitate
** combining freed blocks with adjacent blocks that are already
** free.
**
** All requests for memory are satisfied with blocks that are
** an integral number of 4-byte words.  More memory may be
** provided than was requested if the fragment left over after
** the allocation would not be large enough to be useable.
*/

#ifndef KMEM_H_
#define KMEM_H_

#define KERNEL_SRC

// standard types etc.
#include <common.h>

/*
** General (C and/or assembly) definitions
*/

// Slab and slice sizes, in bytes

#define SZ_SLAB     SZ_PAGE
#define SZ_SLICE    (SZ_SLAB / 4)

#ifndef ASM_SRC

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

/*
** Prototypes
*/

/**
** Name: km_init
**
** Find what memory is present on the system and
** construct the list of free memory blocks.
**
** Dependencies:
**    Must be called before any other init routine that uses
**    dynamic storage is called.
*/
void km_init( void );

/**
** Name:    km_dump
**
** Dump the current contents of the freelist to the console
*/
void km_dump( void );

/*
** Functions that manipulate free memory blocks.
*/

/**
** Name:    km_page_alloc
**
** Allocate a page of memory from the free list.
**
** @return a pointer to the beginning of the allocated page,
**         or NULL if no memory is available
*/
void *km_page_alloc( void );

/**
** Name:    km_page_free
**
** Returns a memory block to the list of available blocks,
** combining it with adjacent blocks if they're present.
**
** CRITICAL ASSUMPTION:  multi-page blocks will be freed one page
** at a time!
**
** @param[in] block   Pointer to the page to be returned to the free list
*/
void km_page_free( void *block );

/**
** Name:    km_slice_alloc
**
** Dynamically allocates a slice (1/4 of a page).  If no
** memory is available, we panic.
**
** @return a pointer to the allocated slice
*/
void *km_slice_alloc( void );

/**
** Name:    km_slice_free
**
** Returns a slice to the list of available slices.
**
** We make no attempt to merge slices, as they are independent
** blocks of memory (unlike pages).
**
** @param[in] block  Pointer to the slice (1/4 page) to be freed
*/
void km_slice_free( void *block );

#endif /* !ASM_SRC */

#endif
