#ifndef PROGBEEP_INC_C_
#define PROGBEEP_INC_C_
#include <common.h>
#include <chom.raw.h>

/**
** User function main beep:  beep beep beep
**
** Beeeeeeep
**
** Invoked as:  progBEEP
*/

USERMAIN( progBEEP ) {
	char *name = argv[0] ? argv[0] : "nobody";

	write(CHAN_BEEP, chom_raw, chom_raw_len);

	// all done!
	exit( 0 );

	// this should really get us out of here
	return( 42 );
}
#endif
