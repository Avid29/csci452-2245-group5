/**
** @file	user.h
**
** @author	CSCI-452 class of 20245
**
** @brief	Declarations of user-level code management routines
*/

#ifndef USER_H_
#define USER_H_

#include <common.h>

#include <procs.h>

/*
** General (C and/or assembly) definitions
*/

#ifndef ASM_SRC

/*
** Start of C-only definitions
*/

/*
** Types
*/

typedef struct prog_s {
	int TBD;	// coming soon!
} prog_t;

/*
** Globals
*/

/*
** Prototypes
*/

/**
** Name:	user_init
**
** Initializes the user support module.
*/
void user_init( void );

/**
** Name:	user_locate
**
** Locates a user program in the user code archive.
**
** @param what   The ID of the user program to find
**
** @return a pointer to the code archive, or NULL
*/
prog_t *user_locate( uint_t what );

/**
** Name:	user_duplicate
**
** Loads a user program from the user code archive into memory.
** Allocates all needed frames and sets up the VM tables.
**
** @param new   The PCB for the new copy of the program
** @param old   The PCB for the existing the program
**
** @return the status of the duplicate attempt
*/
int user_duplicate( pcb_t *new, pcb_t *old );

/**
** Name:	user_load
**
** Loads a user program from the user code archive into memory.
** Allocates all needed frames and sets up the VM tables.
**
** @param prog   A pointer to the code archive entry
** @param pcb    The PCB for the program being loaded
** @param args   The argument vector for the program
**
** @return the status of the load attempt
*/
int user_load( prog_t *prog, pcb_t *pcb, const char **args );

/**
** Name:	user_cleanup
**
** "Unloads" a user program. Deallocates all memory frames and
** cleans up the VM structures.
**
** @param pcb   The PCB of the program to be cleaned up
**
** @return a pointer to the code archive, or NULL
*/
void user_cleanup( pcb_t *pcb );

#endif  /* !ASM_SRC */

#endif
