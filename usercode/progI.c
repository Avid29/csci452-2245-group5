#ifndef PROGI_INC_C_
#define PROGI_INC_C_
#include <common.h>

#ifndef MAX_CHILDREN
#define MAX_CHILDREN	50
#endif

/**
** User function I:  exit, fork, exec, kill, sleep, waitpid, write
**
** Reports, then loops spawing progW, sleeps, kills two children, then
** loops checking the status of all its children
**
** Invoked as:  progI [ x [ n ] ]
**	 where x is the ID character (defaults to 'i')
**		   n is the number of children to spawn (defaults to 5)
*/

USERMAIN( progI ) {
	char *name = argv[0] ? argv[0] : "nobody";
	int count = 5;	  // default child count
	char ch = 'i';	  // default character to print
	int nap = 5;	  // nap time
	char buf[128];
	char ch2[] = "*?*";
	uint_t children[MAX_CHILDREN];
	int nkids = 0;

	// process the command-line arguments
	switch( argc ) {
	case 3:	count = ustr2int( argv[2], 10 );
			// FALL THROUGH
	case 2:	ch = argv[1][0];
			break;
	case 1:	// just use the defaults
			break;
	default:
			usprint( buf, "%s: argc %d, args: ", name, argc );
			cwrites( buf );
			for( int i = 0; i <= argc; ++i ) {
				usprint( buf, " %s", argv[argc] ? argv[argc] : "(null)" );
				cwrites( buf );
			}
			cwrites( "\n" );
	}

	// secondary output (for indicating errors)
	ch2[1] = ch;

	// announce our presence
	write( CHAN_SIO, &ch, 1 );

	// set up the argument vector
	// we run:	progW 10 5

	char *argsw[] = { "progW", "W", "10", "5", NULL };

	for( int i = 0; i < count; ++i ) {
		int whom = spawn( (uint32_t) progW, argsw );
		if( whom < 0 ) {
			swrites( ch2 );
		} else {
			swritech( ch );
			children[nkids++] = whom;
		}
	}

	// let the children start
	sleep( SEC_TO_MS(nap) );

	// kill two of them
	int32_t status = kill( children[1] );
	if( status ) {
		usprint( buf, "!! %c: kill(%d) status %d\n", ch, children[1], status );
		cwrites( buf );
		children[1] = -42;
	}
	status = kill( children[3] );
	if( status ) {
		usprint( buf, "!! %c: kill(%d) status %d\n", ch, children[3], status );
		cwrites( buf );
		children[3] = -42;
	}

	// collect child information
	while( 1 ) {
		int n = waitpid( 0, NULL );
		if( n == E_NO_CHILDREN ) {
			// all done!
			break;
		}
		for( int i = 0; i < count; ++i ) {
			if( children[i] == n ) {
				usprint( buf, "== %c: child %d (%d)\n", ch, i, children[i] );
				cwrites( buf );
			}
		}
		sleep( SEC_TO_MS(nap) );
	};

	// let init() clean up after us!

	exit( 0 );

	return( 42 );  // shut the compiler up!
}
#endif
