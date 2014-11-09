/******************************************************************************

  New header file for ix1
  
  SW Aug 2001

******************************************************************************/

#ifndef _bss_indexing_h
#define _bss_indexing_h

#include "bss.h"
#include "bss_errors.h"

#define MEMAVAIL 20		/* Data space for output keys and pointers
				   New default 20 MB Aug 01, used to be 4 */
#ifndef _WIN32
#define MAX_TDIRS 10
#else
#define MAX_TDIRS 1
#endif /* _WIN32 */
int (*stemfn)(char *, int, char *) ;

void initrun(STRINGS *) ;
int make_run(void) ;
int estimate(void) ;
void icleanup() ;
void delete_temp_files(void) ;

#endif
