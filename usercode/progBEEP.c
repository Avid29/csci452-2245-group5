#ifndef PROGBEEP_INC_C_
#define PROGBEEP_INC_C_
#include <common.h>

/**
** User function main beep:  beep beep beep
**
** Beeeeeeep
**
** Invoked as:  progBEEP
*/

USERMAIN( progBEEP ) {
	char *name = argv[0] ? argv[0] : "nobody";
	char buf[128];

	cwrites("I'm beepin!\n");

	int ret = write(CHAN_BEEP, kubernetes_raw, kubernetes_raw_len);
	usprint(buf, " Returned from a beep write... Got %d\n", ret);
	cwrites(buf);

	// all done!
	exit( 0 );

	// this should really get us out of here
	return( 42 );
}
#endif
