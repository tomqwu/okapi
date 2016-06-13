#ifndef _i1_defs_h
#define _i1_defs_h

/* Defines for the interface */

#include "bss.h"
#include <signal.h>


#define HUUGE    	1024 * 1024 * 4
#define ENORMOUS HUUGE * 2	/* Temp 5/94 until have "accept" and "prefer" commands */

char Command[HUUGE];
/* char Results[ENORMOUS]; */

#endif /* _i1_defs_h */
