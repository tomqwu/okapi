/******************************************************************************

  mergeonly.c SW May 93

  Prog to do single merge pass on an acceptable number of Okapi index
  runfiles.

******************************************************************************/

#include "defines.h"

BOOL silent =  FALSE ;
u_int Secs ;
OFFSET_TYPE merge(int, int num, int outsuffix, char *, char *, BOOL, int itype,
	  int delete) ;

int
main(int argc, char *argv[])
{
  int itype ;
  int numfiles ;
  int firstsuff ;
  char *infile_stem ;
  OFFSET_TYPE result ;

  static char *usage = 
 "Usage: %s infile_stem firstsuff num index_type\n\
E.g. /vol/ok.1/ix1_ex.1109. 23 12 4\n\
Writes output to stdout.\n" ;

  if ( argc != 5 ) {
    fprintf(stderr, usage, argv[0]) ;
    exit(1) ;
  }

  infile_stem=argv[1] ;
  firstsuff=atoi(argv[2]) ;
  numfiles=atoi(argv[3]) ;
  itype=atoi(argv[4]) ;
		 

  result = 
     merge(firstsuff, numfiles, 9999, infile_stem, "rubbish", 1, itype, 0) ;
  if ( result < 0 ) exit(1) ;
  else exit(0) ;

}


