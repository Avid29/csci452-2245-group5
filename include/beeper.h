/**
** @file	beeper.h
**
** @author	Mary Strodl <mstrodl@csh.rit.edu>
**
** @brief	Beep speaker handling
*/

#ifndef BEEPER_H_
#define BEEPER_H_

#include <common.h>

#ifndef ASM_SRC

#ifdef KERNEL_SRC

/**
** Name: beeper_write
**
** Play PCM audio sample in `buf` over the PC speaker
*/
int beeper_write(uint8_t *buf, uint_t length);

#endif  /* KERNEL_SRC */

#endif  /* !ASM_SRC */

#endif
