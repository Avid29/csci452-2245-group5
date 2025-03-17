/*
** @file	startup.S
**
** @author	Jon Coles
** @authors	Warren R. Carithers, K. Reek
**
** SP startup code.
**
** This code prepares the various registers for execution of
** the program.  It sets up all the segment registers and the
** runtime stack.  By the time this code is running, we're in
** protected mode already.
*/
	.arch	i386

#include <bootstrap.h>

/*
** Configuration options - define in Makefile
**
**	CLEAR_BSS	include code to clear all BSS space
**	OS_CONFIG	OS-related (vs. just standalone) variations
*/

/*
** A symbol for locating the beginning of the code.
*/
	.globl begtext

	.text
begtext:

/*
** The entry point. When we get here, we have just entered protected
** mode, so all the segment registers are incorrect except for CS.
*/
	.globl	_start

_start:
	cli			/* seems to be reset on entry to p. mode */
	movb	$NMI_ENABLE, %al  /* re-enable NMIs (bootstrap */
	outb	$CMOS_CMD	  /*   turned them off) */

/*
** Set the data and stack segment registers (code segment register
** was set by the long jump that switched us into protected mode).
*/
	xorl	%eax, %eax	/* clear EAX */
	movw	$GDT_DATA, %ax	/* GDT entry #3 - data segment */
	movw	%ax, %ds	/* for all four data segment registers */
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	movw	$GDT_STACK, %ax	/* entry #4 is the stack segment */
	movw	%ax, %ss

	movl	$TARGET_STACK, %esp	/* set up the system stack pointer */
	xorl	%ebp, %ebp	/* initial frame pointer is NULL */

#ifdef CLEAR_BSS
/*
** Zero the BSS segment
**
** These symbols are defined automatically by the linker.
*/
	.globl	__bss_start, _end
	.globl	kernel_esp

	movl	$__bss_start, %edi
clearbss:
	movl	$0, (%edi)
	addl	$4, %edi
	cmpl	$_end, %edi
	jb	clearbss
#endif	/* CLEAR_BSS */

/*
** Enable paging.
*/
	// enable large pages
	movl	%cr4, %eax
	orl	%(CR4_PSE), %eax
	movl	%eax, %cr4

	// set the page directory
	movl	$(V2P(entrypgdir)), %eax
	movl	%eax, %cr3

	// turn on paging
	movl	%cr0, %eax
	orl	$(CR0_PG), %eax
	movl	%eax, %cr0

	// reset our stack pointer
	movl	kernel_esp, %esp

/*
** Call the system initialization routine, and switch to
** executing at high addresses. We use an indirect call
** because the assembler wants to give us a PC-relative
** instruction here if we use a direct call.
**
** Alternate idea: push the address of isr_restore
** and just do an indirect jump?
*/
	.globl	main
	movl	$main, %eax
	call	*%eax

/*
** At this point, main() must have created the first user
** process, and we're ready to shift into user mode.  The user
** stack for that process must have the initial context in it;
** we treat this as a "return from interrupt" event, and just
** transfer to the code that restores the user context.
*/
	movl	$isr_restore, %eax
	jmp	*%eax   // defined in isrs.S
