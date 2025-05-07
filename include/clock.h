/**
** @file	clock.h
**
** @author	CSCI-452 class of 20245
**
** @brief	Clock module declarations
*/

#ifndef CLOCK_H_
#define CLOCK_H_

#include <common.h>

/*
** General (C and/or assembly) definitions
*/

// conversion functions for seconds, ms, and ticks
// (SEC_TO_MS is defined in defs.h)
#define MS_TO_TICKS(n)           ((n*(1000/CLOCK_FREQ)))
#define SEC_TO_TICKS(n)          (MS_TO_TICKS(SEC_TO_MS(n)))
#define TICKS_TO_SEC(n)          ((n) / CLOCK_FREQ)
#define TICKS_TO_SEC_ROUNDED(n)  (((n)+(CLOCK_FREQ-1)) / CLOCK_FREQ)

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

// current system time
extern uint32_t system_time;
extern uint32_t clock_speed_khz;

/*
** Prototypes
*/

/**
** Name:	clk_init
**
** Clock module initialization
*/
void clk_init( void );

/**
** Name: sleep_micros
**
** Sleep for `micros` microseconds. Must be called after `clk_init()`
*/
void sleep_micros(unsigned int micros);
/**
** Name: sleep_micros_setup
**
** Set up global state for `micros` sleep. Must call `sleep_micros_finish` to
** actually wait.
*/
void sleep_micros_setup(unsigned int micros);
/**
** Name: sleep_micros_finish
**
** Wait for the timer set up by `sleep_micros_setup` to expire.
** Helpful to do useful work during the "sleep"
*/
void sleep_micros_finish(void);

#endif  /* !ASM_SRC */

#endif
