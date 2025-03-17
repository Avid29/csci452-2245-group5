/**
** @file	vm.h
**
** @author	CSCI-452 class of 20245
**
** @brief	Virtual memory-related declarations.
*/

#ifndef VM_H_
#define VM_H_

#include <types.h>

/*
** VM layout of the system
**
** User processes use the first 4MB of the 32-bit address space; see the
** next comment for details.
**
** Kernel virtual addresses are in the "higher half" range, beginning
** at 0x80000000.  We define our mapping such that virtual address
** 0x8nnnnnnn maps to physical address 0x0nnnnnnn, so converting between
** the two is trivial.
*/

/*
** VM layout of process' address space
**
** Processes are limited to the first 4MB of the 32-bit address space:
**
**  Address Range            Contents
**  =======================  ================================
**  0x00000000 - 0x00000fff  page 0 is inaccessible
**  0x00001000 - 0x000..fff  text occupies pages 1 - N
**  0x000..000 - 0x000..fff  data occupies pages N+1 - N+d
**  0x000..000 - 0x000..fff  bss occupies pages N+d+1 - N+d+b
**  0x000..000 - 0x003fdfff  unusable
**  0x003fe000 - 0x003fffff  stack occupies last two pages
**
** This gives us the following page table structure:
**
** Page directory:
**   Entries    Contents
**   ========   ==============================
**   0          point to PMT for address space
**   1 - 1023   invalid
**
** Page map table:
**   Entries         Contents
**   ========        ==============================
**   0               invalid
**   1 - N           text frames
**   N+1 - N+d       data frames
**   N+d+1 - N+d+b   bss frames
**   N+d+b+1 - 1021  invalid
**   1022 - 1023     stack frames
*/

/*
** General (C and/or assembly) definitions
*/

// user virtual addresses
#define USER_TEXT      0x00001000
#define USER_STACK     0x003fe000
#define	USER_STK_END   0x00400000

// how to find the addresses of the stack pages in the VM hierarchy
// user address space is the first 4MB of virtual memory
#define	USER_PDE       0
// the stack occupies the last two pages of the address space
#define USER_STK_PTE1  1022
#define USER_STK_PTE2  1023

// some important memory addresses
#define	KERN_BASE      0x80000000     // start of "kernel" memory
#define	EXT_BASE       0x00100000     // start of "extended" memory (1MB)
#define DEV_BASE       0xfe000000     // "device" memory
#define PHYS_TOP       0x7fffffff     // last usable physical address

// where the kernel actually lives
#define	KERN_PLINK     0x00010000
#define KERN_VLINK     (KERN_BASE + KERN_PLINK)

// physical/virtual converters
#ifndef ASM_SRC
// uses casting
#define V2P(a)            (((uint_t)(a)) - KERNEL_BASE)
#define P2V(a)            ((void *)(((uint_t)(a)) + KERNEL_BASE))
#else
// doesn't use casting
#define V2P(a)            ((a) - KERNEL_BASE)
#define P2V(a)            ((a) + KERNEL_BASE)
#endif /* !ASM_SRC */

// sizes of things
#define SZ_PAGE           0x00001000    // 4096 bytes
#define NUM_1KB           0x00000400    // 2^10
#define NUM_1MB           0x00100000    // 2^20
#define NUM_1GB           0x40000000    // 2^30
#define NUM_2GB           0x80000000    // 2^31
#define NUM_3GB           0xc0000000

// number of entries in a directory or table
#define N_PDE       1024
#define N_PTE       1024

// index bit positions in virtual addresses
#define PDIX_SHIFT  22
#define PTIX_SHIFT  12
#define P_IX_MASK    0x3ff

// extract the table indices from a 32-bit address
#define PDIX(v)     (((uint)(v)) >> PDIX_SHIFT) & P_IX_MASK)
#define PTIX(v)     (((uint)(v)) >> PTIX_SHIFT) & P_IX_MASK)

// page-size address rounding macros
#define SZ_PG_M1    MOD4K_BITS
#define SZ_PG_MASK  MOD4K_MASK
#define	PGUP(a)     (((a)+SZ_PG_M1) & SZ_PG_MASK)
#define PGDOWN(a)   ((a) & SZ_PG_MASK)

// page directory entry fields
#define	PDE_P		0x00000001
#define	PDE_RW		0x00000002
#define	PDE_US		0x00000004
#define	PDE_PWT		0x00000008
#define	PDE_PCD		0x00000010
#define	PDE_A		0x00000020
#define	PDE_AVL1	0x00000040
#define	PDE_MBZ		0x00000080
#define	PDE_AVL2	0x00000f00
#define	PDE_BA		0xfffff000

// page table entry fields
#define	PTE_P		0x00000001
#define	PTE_RW		0x00000002
#define	PTE_US		0x00000004
#define	PTE_PWT		0x00000008
#define	PTE_PCD		0x00000010
#define	PTE_A		0x00000020
#define	PTE_D		0x00000040
#define	PTE_PAT		0x00000080
#define	PTE_G		0x00000100
#define	PTE_AVL2	0x00000e00
#define	PTE_FA		0xfffff000

#define	PG_CODE		(PTE_USER | PTE_PRESENT)
#define	PG_DATA		(PTE_USER | PTE_RW | PTE_PRESENT)

// error code bit assignments for page faults
#define PF_P        0x00000001
#define PF_W        0x00000002
#define PF_US       0x00000004
#define PF_RSVD     0x00000008
#define PF_ID       0x00000010
#define PF_PK       0x00000020
#define PF_SS       0x00000040
#define PF_HLAT     0x00000080
#define PF_SGX      0x00008000
#define	PF_UNUSED   0xffff7f00

#ifndef ASM_SRC

/*
** Start of C-only definitions
*/

/*
** Types
*/

// page directory entries

// as a 32-bit word
typedef uint32_t pde_t;

// PDE broken out into fields
typedef struct pde_s {
	uint_t p    :1;   // present
	uint_t rw   :1;   // writable
	uint_t us   :1;   // user/supervisor
	uint_t pwt  :1;   // cache write-through
	uint_t pcd  :1;   // cache disable
	uint_t avl1 :1;   // available (1)
	uint_t mbz  :1;   // must be zero
	uint_t avl2 :3;   // available (2)
	uint_t fa   :22;  // frame address
} pdef_t;

// page table entries

// as a 32-bit word
typedef uint32_t pte_t;

// broken out into fields
typedef struct pte_s {
	uint_t p   :1;    // present
	uint_t rw  :1;    // writable
	uint_t us  :1;    // user/supervisor
	uint_t pwt :1;    // cache write-through
	uint_t pcd :1;    // cache disable
	uint_t a   :1;    // accessed
	uint_t d   :1;    // dirty
	uint_t pat :1;    // page attribute table in use
	uint_t g   :1;    // global
	uint_t avl :3;    // available
	uint_t fa  :22;   // frame address
} ptef_t;

// page fault error code bits
// comment: meaning when 1 / meaning when 0
typedef struct pfec_s {
	uint_t p    :1;		// page-level protection violation / !present
	uint_t w    :1;		// write / read
	uint_t us   :1;		// user-mode access / supervisor-mode access
	uint_t rsvd :1;		// reserved bit violation / not
	uint_t id   :1;		// instruction fetch / data fetch
	uint_t pk   :1;		// protection-key violation / !pk
	uint_t ss   :1;		// shadow stack access / !ss
	uint_t hlat :1;		// HLAT paging / ordinary paging or access rights
	uint_t xtr1 :7;		// unused
	uint_t sgz  :1;		// SGX-specific access control violation / !SGX
	uint_t xtr2 :16;	// more unused
} pfec_t;

// Mapping descriptor for VA::PA mappings
typedef struct mapping_t {
	uint32_t virt_start;  // starting virtual address for this range
	uint32_t phys_start;  // first physical address in the range
	uint32_t phys_end;    // last physical address in the range
	uint32_t perm;        // access control
} mapping_t;

/*
** Globals
*/

// initial page directory, for when the kernel is starting up
extern pde_t entrypgdir[];

// created page directory for the kernel
extern pde_t *kpdir;

/*
** Prototypes
*/

/**
** Name:	vm_init
**
** Initialize the VM module
**
** Note: should not be called until after the memory free list has
** been set up.
*/
void vm_init( void );

/**
** Name:	vm_mkkvm
**
** Create the kernel's page table hierarchy
*/
pde_t *vm_mkkvm( void );

/**
** Name:	vm_mkuvm
**
** Create the page table hierarchy for a user process
**
** @param p   The PCB of the user process
*/
pde_t *vm_mkuvm( pcb_t *p );

/**
** Name:	vm_set_kvm
**
** Switch the page table register to the kernel's page directory
*/
void vm_set_kvm( void );

/**
** Name:	vm_set_uvm
**
** Switch the page table register to the page directory for a user process.
**
** @param p   The PCB of the user process
*/
void vm_set_uvm( pcb_t *p );

/**
** Name:    vm_add
**
** Add pages to the page hierarchy for a process, copying data into
** them if necessary.
**
** @param pdir   Pointer to the page directory to modify
** @param wr     "Writable" flag for the PTE
** @param va     Starting VA of the range
** @param size   Amount of physical memory to allocate
** @param data   Pointer to data to copy, or NULL
** @param bytes  Number of bytes to copy
**
** @return status of the allocation attempt
*/
int vm_add( pde_t *pdir, bool_t wr, void *va, uint32_t size,
		char *data, uint32_t bytes );

/**
** Name:	vm_free
**
** Deallocate a page table hierarchy and all physical memory frames
** in the user portion.
**
** @param pdir  Pointer to the page directory
*/
void vm_free( pde_t *pdir );

#endif  /* !ASM_SRC */

#endif
