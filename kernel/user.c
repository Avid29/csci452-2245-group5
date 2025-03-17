/**
** @file	user.c
**
** @author	CSCI-452 class of 20245
**
** @brief	User-level code manipulation routines
*/

#define	KERNEL_SRC

#include <common.h>
#include <user.h>

#include <bootstrap.h>
#include <elf.h>

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

/*
** PRIVATE FUNCTIONS
*/

/**
** read_phdrs(addr,phoff,phentsize,phnum)
**
** Parses the ELF program headers and each segment described into memory.
**
** @param hdr  Pointer to the program header
** @param pcb  Pointer to the PCB (and its PDE)
**
** @return status of the attempt:
**     SUCCESS         everything loaded correctly
**     E_LOAD_LIMIT    more than N_LOADABLE PT_LOAD sections
**     other           status returned from vm_add()
*/
static int read_phdrs( elfhdr_t *hdr, pcb_t *pcb ) {
	elfphdr_t loadables[MAX_LOADS];
	uint_t n_loads = 0;

	// sanity check
	assert1( hdr != NULL );
	assert2( pcb != NULL );

	// iterate through the program headers
	uint_t nhdrs = hdr->e_phnum;
	uint_t hlen = hdr->e_phentsize;

	// pointer to the first header table entry
	elfphdr_t *curr = (elfphdr_t *) ((uint32_t) hdr + hdr->phoff);

	// process them all
	uint_t va = 0;
	int loaded = 0;
	for( uint_t i = 0; i < ndhrs; ++i, ++curr ) {

		if( curr->p_type != PT_LOAD ) {
			// not loadable --> we'll skip it
			continue;
		}

		if( loaded >= N_LOADABLE ) {
			return E_LOAD_LIMIT;
		}

		// important details
		uint_t mem = curr->memsz;
		uint_t va = curr->va;

		// set a pointer to the bytes within the object file
		char *data = (char *) (((uint32_t)hdr) + curr->offset);

		// copy the pages into memory
		int stat = vm_add( pcb, curr->p_flags & PF_W, curr->va,
				curr->memsz, data, curr->filesz );
		if( stat != SUCCESS ) {
			// TODO what else should we do here? check for memory leak?
			return stat;
		}

		// set the section table entry in the PCB
		pcb->sects[loaded].length = curr->memsize;
		pcb->sects[loaded].addr = curr->va;
		++loaded;
	}

	return SUCCESS;
}

/**
** Name:	stack_setup
**
** Set up the stack for a new process
**
** @param pcb    Pointer to the PCB for the process
** @param entry  Entry point for the new process
** @param args   Argument vector to be put in place
**
** @return A pointer to the context_t on the stack, or NULL
*/
context_t *stack_setup( pcb_t *pcb, uint32_t entry, char **args ) {

	/*
	** First, we need to count the space we'll need for the argument
	** vector and strings.
	*/

	int argbytes = 0;
	int argc = 0;

	while( args[argc] != NULL ) {
		int n = strlen( args[argc] ) + 1;
		// can't go over one page in size
		if( (argbytes + n) > SZ_PAGE ) {
			// oops - ignore this and any others
			break;
		}
		argbytes += n;
		++argc;
	}

	// Round up the byte count to the next multiple of four.
	argbytes = (argbytes + 3) & MOD4_MASK;

	/*
	** Allocate the arrays.  We are safe using dynamic arrays here
	** because we're using the OS stack, not the user stack.
	**
	** We want the argstrings and argv arrays to contain all zeroes.
	** The C standard states, in section 6.7.8, that
	**
	**   "21 If there are fewer initializers in a brace-enclosed list
	**       than there are elements or members of an aggregate, or
	**       fewer characters in a string literal used to initialize an
	**       array of known size than there are elements in the array,
	**       the remainder of the aggregate shall be initialized
	**       implicitly the same as objects that have static storage
	**       duration."
	**
	** Sadly, because we're using variable-sized arrays, we can't
	** rely on this, so we have to call memclr() instead. :-(  In
	** truth, it doesn't really cost us much more time, but it's an
	** annoyance.
	*/

	char argstrings[ argbytes ];
	char *argv[ argc + 1 ];

	CLEAR( argstrings );
	CLEAR( argv );

	// Next, duplicate the argument strings, and create pointers to
	// each one in our argv.
	char *tmp = argstrings;
	for( int i = 0; i < argc; ++i ) {
		int nb = strlen(args[i]) + 1; // bytes (incl. NUL) in this string
		strcpy( tmp, args[i] );   // add to our buffer
		argv[i] = tmp;              // remember where it was
		tmp += nb;                  // move on
	}

	// trailing NULL pointer
	argv[argc] = NULL;

	/*
	** The pages for the stack were cleared when they were allocated,
	** so we don't need to remember to do that.
	**
	** We reserve one longword at the bottom of the stack to hold a
	** pointer to where argv is on the stack.
	**
	** The user code was linked with a startup function that defines
	** the entry point (_start), calls main(), and then calls exit()
	** if main() returns. We need to set up the stack this way:
	** 
	**      esp ->  context      <- context save area
	**              ...          <- context save area
	**              context      <- context save area
	**              entry_pt     <- return address for the ISR
	**              argc         <- argument count for main()
	**         /->  argv         <- argv pointer for main()
	**         |     ...         <- argv array w/trailing NULL
	**         |     ...         <- argv character strings
	**         \--- ptr          <- last word in stack
	**
	** Stack alignment rules for the SysV ABI i386 supplement dictate that
	** the 'argc' parameter must be at an address that is a multiple of 16;
	** see below for more information.
	*/

	// Pointer to the last word in stack. We get this from the 
	// VM hierarchy. Get the PDE entry for the user address space.
	pde_t stack_pde = pcb->pagedir[USER_PDE];

	// The PDE entry points to the PT, which is an array of PTE. The last
	// two entries are for the stack; pull out the last one.
	pte_t stack_pte = ((pte_t *)(stack_pde & MOD4K_MASK))[USER_STK_PTE2];

	// OK, now we have the PTE. The frame address of the last page is
	// in this PTE. Find the address immediately after that.
	uint32_t *ptr = (uint32_t *)
		((uint32_t)(stack_pte & MOD4K_MASK) + SZ_PAGE);

	// Pointer to where the arg strings should be filled in.
	char *strings = (char *) ( (uint32_t) ptr - argbytes );

	// back the pointer up to the nearest word boundary; because we're
	// moving toward location 0, the nearest word boundary is just the
	// next smaller address whose low-order two bits are zeroes
	strings = (char *) ((uint32_t) strings & MOD4_MASK);

	// Copy over the argv strings.
	memcpy( (void *)strings, argstrings, argbytes );

	/*
	** Next, we need to copy over the argv pointers.  Start by
	** determining where 'argc' should go.
	**
	** Stack alignment is controlled by the SysV ABI i386 supplement,
	** version 1.2 (June 23, 2016), which states in section 2.2.2:
	**
	**   "The end of the input argument area shall be aligned on a 16
	**   (32 or 64, if __m256 or __m512 is passed on stack) byte boundary.
	**   In other words, the value (%esp + 4) is always a multiple of 16
	**   (32 or 64) when control is transferred to the function entry
	**   point. The stack pointer, %esp, always points to the end of the
	**   latest allocated stack frame."
	**
	** Isn't technical documentation fun?  Ultimately, this means that
	** the first parameter to main() should be on the stack at an address
	** that is a multiple of 16.
	**
	** The space needed for argc, argv, and the argv array itself is
	** argc + 3 words (argc+1 for the argv entries, plus one word each
	** for argc and argv).  We back up that much from 'strings'.
	*/

	int nwords = argc + 3;
	uint32_t *acptr = ((uint32_t *) strings) - nwords;

	/*
	** Next, back up until we're at a multiple-of-16 address. Because we
	** are moving to a lower address, its upper 28 bits are identical to
	** the address we currently have, so we can do this with a bitwise
	** AND to just turn off the lower four bits.
	*/

	acptr = (uint32_t *) ( ((uint32_t)acptr) & MOD16_MASK );

	// copy in 'argc'
	*acptr = argc;

	// next, 'argv', which follows 'argc'; 'argv' points to the
	// word that follows it in the stack
	uint32_t *avptr = acptr + 2;
	*(acptr+1) = (uint32_t) avptr;

	/*
	** Next, we copy in all argc+1 pointers.
	*/

	// Offset into the string area on the stack
	uint32_t offset = 0;

	// Adjust and copy the string pointers.
	for( int i = 0; i <= argc; ++i ) {
		if( argv[i] != NULL ) {
			// an actual pointer - adjust it and copy it in
			*avptr = strings;
			// skip to the next entry in the array
			strings += strlen(argv[i]) + 1;
		} else {
			// end of the line!
			*avptr = NULL;
		}
		++avptr;
	}

	/*
	** Now, we need to set up the initial context for the executing
	** process.
	**
	** When this process is dispatched, the context restore code will
	** pop all the saved context information off the stack, including
	** the saved EIP, CS, and EFLAGS. We set those fields up so that
	** the interrupt "returns" to the entry point of the process.
	*/

	// Locate the context save area on the stack.
	context_t *ctx = ((context_t *) avptr) - 1;

	/*
	** We cleared the entire stack earlier, so all the context
	** fields currently contain zeroes.  We now need to fill in
	** all the important fields.
	*/

	ctx->eflags = DEFAULT_EFLAGS;    // IE enabled, PPL 0
	ctx->eip = entry;                // initial EIP
	ctx->cs = GDT_CODE;              // segment registers
	ctx->ss = GDT_STACK;
	ctx->ds = ctx->es = ctx->fs = ctx->gs = GDT_DATA;

	/*
	** Return the new context pointer to the caller.  It will be our
	** caller's responsibility to schedule this process.
	*/
	
	return( ctx );
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:	user_init
**
** Initializes the user support module.
*/
void user_init( void ) {

#if TRACING_INIT
	cio_puts( " User" );
#endif 

	// not really much to do here
}

/**
** Name:	user_locate
**
** Locates a user program in the user code archive.
**
** @param what   The ID of the user program to find
**
** @return a pointer to the code archive, or NULL
*/
void *user_locate( uint_t what ) {
	// TODO
}

/**
** Name:    user_duplicate
**
** Duplicates the memory setup for an existing process.
**
** @param new   The PCB for the new copy of the program
** @param old   The PCB for the existing the program
**
** @return the status of the duplicate attempt
*/
int user_duplicate( pcb_t *new, pcb_t *old ) {

	// We need to do a recursive duplication of the process address
	// space of the current process. First, we create a new user
	// page directory. Next, we'll duplicate the USER_PDE page
	// table. Finally, we'll go through that table and duplicate
	// all the frames.

	// create the initial VM hierarchy
	pde_t *pdir = vm_mkuvm();
	if( pdir == NULL ) {
		return E_NO_MEMORY;
	}
	new->pagedir = pdir;

	// next, add a USER_PDE page table that's a duplicate of the
	// current process' page table
	if( !vm_ptdup(old->pagedir,new->pagedir) ) {
		// TODO check for memory leak?
		return E_NO_MEMORY;
	}

	// now, iterate through the entries, replacing the frame
	// numbers with duplicate frames
	pte_t *pt = (pte_t *) (pdir[USER_PDE]);
	for( int i = 0; i < N_PTE; ++i ) {
		// if this entry is present,
		if( *pt & PTE_PRESENT ) {
			// duplicate the page
			void *tmp = vm_pagedup( *pt );
			// replace the old frame number with the new one
			*pt = (pte_t) (((uint32_t)tmp) | (*pt & MOD4K_BITS));
		} else {
			*pt = 0;
		}
		++pt;
	}

	return SUCCESS;
}

/**
** Name:	user_load
**
** Loads a user program from the user code archive into memory.
** Allocates all needed frames and sets up the VM tables.
**
** @param prog   A pointer to the program ELF file in memory
** @param pcb    The PCB for the program being loaded
** @param args   The argument vector for the program
**
** @return the status of the load attempt
*/
int user_load( void *prog, pcb_t *pcb, const char **args ) {

	// NULL pointers are bad!
	assert1( prog != NULL );
	assert1( pcb != NULL );
	assert1( args != NULL );

	// verify the ELF header
	elfhdr_t *hdr = (elfhdr_t *) prog;

	if( ((elfhdr_t *)prog)->e_magic != ELF_MAGIC ) {
		return E_BAD_PARAM;
	}

	// allocate a page directory
	// TODO what if there already is one???
	pcb->pagedir = vm_mkuvm();
	if( pcb->pagedir == NULL ) {
		// TODO check for memory leak?
		return E_NO_MEMORY;
	}

	// read all the program headers
	int stat = read_phdrs( hdr, pcb );
	if( stat != SUCCESS ) {
		// TODO figure out a better way to deal with this
		PANIC( 0, "user_load: phdr read failed" );
	}

	// next, set up the runtime stack - just like setting up loadable
	// sections, except nothing to copy
	stat = vm_add( pcb->pagedir, true, (void *) USER_STACK,
			SZ_USTACK, NULL, 0 );
	if( stat != SUCCESS ) {
		// TODO yadda yadda...
		return E_FAILURE;
	}

	// set up the command-line arguments
	pcb->context = stack_setup( pcb, hdr->e_entry, args );

	return SUCCESS;
}

/**
** Name:	user_unload
**
** "Unloads" a user program. Deallocates all memory frames and
** cleans up the VM structures.
**
** @param pcb   The PCB of the program to be unloaded
**
** @return a pointer to the code archive, or NULL
*/
void user_unload( pcb_t *pcb ) {
	// TODO
}
