/**
** @file	vm.c
**
** @author	CSCI-452 class of 20245
**
** @brief	Kernel VM support
*/

#define	KERNEL_SRC

#include <common.h>

#include <kmem.h>
#include <vm.h>

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

// initial page directory, for when the kernel is starting up
//
// we use large (4MB) pages here to allow us to use a one-level
// paging hierarchy; the kernel will create a new page table
// hierarchy once memory is initialized

ALIGN(SZ_PAGE)
pde_t entrypgdir[N_PDE] = {
	// Map VA range [0, 4MB] to PA range [0, 4MB]
	[0] = PDE_P | PDE_W | PDE_PS;
	// Map VA range [KERN_BASE, KERN_BASE+4MB] to PA range [0, 4MB]
	[KERN_BASE >> PDIX_BITS] = PDE_P | PDE_W | PDE_PS;
};

// created page directory for the kernel
pde_t *kpdir;

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:	vm_isr
**
** Description: Page fault handler
**
** @param vector   Interrupt vector number
** @param code     Error code pushed onto the stack
*/
static void vm_isr( int vector, int code ) {

	// get whatever information we can from the fault
	pfec_t fault = (pfec_t) code;
	uint32_t addr = getcr2();

	// report what we found
	sprint( b256,
		"** page fault @ 0x%08x %cP %c %cM %cRSV %c %cPK %cSS %cHLAT %cSGZ",
			addr,
			fault.p    ? ' ' : '!',
			fault.w    ? 'W' : 'R',
			fault.us   ? 'U' : 'S',
			fault.rsvd ? ' ' : '!',
			fault.id   ? 'I' : 'D',
			fault.pk   ? ' ' : '!',
			fault.ss   ? ' ' : '!',
			fault.hlat ? ' ' : '!',
			fault.sgz  ? ' ' : '!'
	);

	// and give up
	PANIC( 0, b256 );
}
/*
** PUBLIC FUNCTIONS
*/

/**
** Name:	vm_init
**
** Description:  Initialize the VM module
*/
void vm_init( void ) {

#if TRACING_INIT
	cio_puts( " VM" );
#endif

	// install the page fault handler
	install_isr( VEC_PAGE_FAULT, vm_isr );
}

/**
** Name:	vm_pagedup
**
** Duplicate a page of memory
**
** @param old  Pointer to the first byte of a page
**
** @return a pointer to the new, duplicate page, or NULL
*/
void *vm_pagedup( void *old ) {
	void *new = (void *) km_page_alloc();
	if( new != NULL ) {
		memcpy( new, old, SZ_PAGE );
	}
	return new;
}

/**
** Name:	vm_ptdup
**
** Duplicate a page directory entry
**
** @param curr  Pointer to the entry to be duplicated
** @param dst   Pointer to where the duplicate should go
**
** @return true on success, else false
*/
bool_t vm_ptdup( pde_t *curr, pde_t *dst ) {

	// simplest case
	if( *curr == 0 ) {
		*dst = 0;
		return true;
	}

	// OK, we have an entry; allocate a page table
	pte_t *pt = (pte_t *) km_alloc_page();
	if( pt == NULL ) {
		return false;
	}

	// pointer to the first PTE in the current table
	pte_t *old = (pte_t *) ((uint32_t) *curr) & MOD4K_MASK);
	// pointer to the first PTE in the new table
	pte_t *new = pt;

	for( int i = 0 ; i < N_PTE; ++i ) {
		if( *old & PTE_P == 0 ) {
			*new = 0;
		} else {
			*new = *old;
		}
		++old;
		++new;
	}

	// assign the page table into the new page directory
	// upper 22 bits from 'pt', lower 12 from '*curr'
	*dst = (pde_t *) (
		(((uint32_t)pt) & MOD4K_MASK) |
		(((uint32_t)(*curr)) & MOD4K_BITS )
	);

	return true;
}

/**
** Return the address of the PTE corresponding to the virtual address
** 'va' within the address space controlled by 'pgdir'. If there is no
** page table for that VA and 'alloc' is true, create the necessary
** page table entries.
**
** @param pdir   Pointer to the page directory to be searched
** @param va     The virtual address we're looking for
** @param alloc  Should we allocate a page table if there isn't one?
**
** @return A pointer to the page table entry for this VA, or NULL
*/
pte_t *vm_getpte( pde_t *pdir, const void *va, bool_t alloc ) {
	pde_t *pde;
	pte_t *ptab;

	// get the PDIR entry for this virtual address
	pde = &pgdir[ PDX(va) ];

	// is it already set up?
	if(*pde & PTE_P){

		// yes!
		pgtab = (pte_t*)P2V(PTE_ADDR(*pde));

	} else {

		// no - should we create it?
		if( !alloc ) {
			// nope, so just return
			return NULL;
		}

		// yes - try to allocate a page table
		pgtab = (pte_t *) km_page_alloc();
		if( pgtab == NULL ) {
			WARNING( "can't allocate page table" );
			return NULL;
		}

		// who knows what was left in this page....
		memclr( pgtab, SZ_PAGE );

		// add this to the page directory
		//
		// we set this up to allow general access; this could be
		// controlled by setting access control in the page table
		// entries, if necessary.
		*pde = PDE_P | PDE_RW | PDE_US | V2P(pgtab);
	}

	// finally, return a pointer to the entry in the
	// page table for this VA
	return &pgtab[ PTX(va) ];
}

// This table defines the kernel's mappings, which are present in
// every process's page table.
static mapping_t kmap[] = {
	{ KERN_BASE, 0,              EXT_BASE,  PDE_RW },
	{ KERN_LINK, V2P(KERN_LINK), V2P(data), 0      },
	{ data,      V2P(data),      KERN_BASE, PDE_RW },
	{ DEV_BASE,  DEV_BASE,       0,         PDE_RW }
}

static const uint_t n_kmap = sizeof(kmap) / sizeof(kmap[0]);

// Set up kernel part of a page table.
pde_t *vm_mkkvm(void)
{
	pde_t *pgdir;
	mapping_t *k;

	// allocate the page directory
	pde_t *pgdir = km_page_alloc();
	if( pgdir == NULL ) {
		return NULL;
	}

	// clear it out to disable all the entries
	memclr( pgdir, SZ_PAGE );

	// map in all the page ranges
	k = kmap;
	for( int i = 0; i < n_kmap; ++i, ++k ) {
		// TODO complete this
	}

	return pgdir;
}

/*
** Creates an initial user VM table hierarchy by copying the
** system entries into a new page directory.
**
** @return a pointer to the new page directory, or NULL
*/
pde_t *vm_mkuvm( void ) {
	uint_t n;

	// allocate the directory
	pde_t *new = (pde_t *) km_page_alloc();
	if( new == NULL ) {
		return NULL;
	}

	// iterate through the kernel page directory
	pde_t *curr = kpdir;
	pde_t *dst = new;
	for( int i = 0; i < N_PDE; ++i ) {

		if( *curr != 0 ) {
			// found an active one - duplicate it
			if( !vm_ptdup(dst,curr) ) {
				return NULL;
			}
		}

		++curr;
		++dst;
	}

	return new;

}

/**
** Name:	vm_set_uvm
**
** Switch the page table register to the kernel's page directory.
*/
void vm_set_kvm( void ) {
	lcr3( V2P(kpgdir) );   // switch to the kernel page table
}

/**
** Name:	vm_set_uvm
**
** Switch the page table register to the page directory for a user process.
**
** @param p  PCB of the process we're switching to
*/
void vm_set_uvm( pcb_t *p ) {
	assert( p != NULL );
	assert( p->pgdir != NULL );

	lcr3( V2P(p->pgdir) );  // switch to process's address space
}

/**
** Name:	vm_add
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
		char *data, uint32_t bytes ) {

	// how many pages to we need?
	uint_t npages = (size & MOD4K_BITS ? PGUP(size) : size) >> MOD4K_SHIFT;
	uint_t offset = 0;

	// permission set for the PTEs
	uint_t entrybase = PTE_PRESENT | PTE_US;
	if( wr ) {
		entrybase |= PTE_RW;
	}

	// iterate through the pages

	for( int i = 0; i < npages; ++i ) {

		// figure out where this page will go in the hierarchy
		pte_t *pte = vm_getpte( pdir, va, true );
		if( pte == NULL ) {
			// TODO if i > 0, this isn't the first frame - is
			// there anything to do about other frames?
			// POSSIBLE MEMORY LEAK?
			return E_NO_MEMORY;
		}

		// allocate the frame
		void *page = km_page_alloc();
		if( page == NULL ) {
			// TODO same question here
			return E_NO_MEMORY;
		}

		// clear it all out
		memclr( page, SZ_PAGE );

		// create the PTE for this frame
		uint32_t entry = (uint32_t) ((page & MOD4K_MASK) | entrybase);
		*pte = entry;

		// copy data if we need to
		if( data != NULL && bytes > 0 ) {
			// how much to copy
			uint_t num = bytes > SZ_PAGE ? SZ_PAGE : bytes;
			// do it!
			pcopy( page, data, num );
			// adjust all the pointers
			data += num;   // where to continue
			bytes -= num;  // what's left to copy
		}

		// bump the virtual address
		va += SZ_PAGE;
	}

	return SUCCESS;

}

// TODO more to come!
