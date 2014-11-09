/******************************************************************************

  mergeonly.c SW May 93

  Prog to do single merge pass on an acceptable number of Okapi index
  runfiles.

  Jan 99: generalized to accept any input file names (one can be stdin)

******************************************************************************/

#include "defines.h"

BOOL silent =  FALSE ;
u_int Secs ;
OFFSET_TYPE imerge(int num, FILE **, FILE *, int itype) ;

int
main(int argc, char *argv[])
{
  int numruns ;
  char **infile_pathnames ;
  FILE **infiles ;
  int itype ;
  OFFSET_TYPE result ;
  BOOL already_got_stdin = FALSE ;
  int i ;
  static char *usage = 
 "Usage: %s numruns input_filenames index_type\
One of the input filenames can be - for stdin.\
Writes output to stdout.\n" ;

  if ( argc < 4 || strcmp(argv[1], "-h") == 0 ) goto usg ;

  numruns = atoi(argv[1]) ;
  if ( numruns <= 0 || argc < numruns + 3 ) goto usg ;
  infile_pathnames = (char **)malloc(numruns * sizeof(char *)) ;
  infiles = (FILE **)malloc(numruns * sizeof(FILE *)) ;
  if ( infile_pathnames == NULL || infiles == NULL ) {
    fprintf(stderr, "Can't allocate memory for input filenames\n") ;
    exit(1) ;
  }
  for ( i = 0 ; i < numruns ; i++ ) {
    infile_pathnames[i] = argv[2 + i] ;
    if ( strcmp(infile_pathnames[i], "-") == 0 ) {
      if ( already_got_stdin ) {
	fprintf(stderr, "Can't have more than one input from stdin\n") ;
	goto usg ;
      }
      infiles[i] = stdin ;
      already_got_stdin = TRUE ;
    }
    else {
      infiles[i] = fopen(infile_pathnames[i], "rb") ;
      if ( infiles[i] == NULL ) {
	fprintf(stderr, "Can't open input file %s\n", infile_pathnames[i]) ;
	exit(1) ;
      }
    }
  }
  itype=atoi(argv[2 + numruns]) ;
  if ( itype < 4 || (itype > 13 && itype != 20) ) {
    fprintf(stderr, "Can only handle index types 4 to 14 and 20\n") ;
    exit(1) ;
  }
  result = imerge(numruns, infiles, stdout, itype) ;
  if ( result < 0 ) exit(1) ;
  else exit(0) ;

 usg:
  fprintf(stderr, usage, argv[0]) ;
  exit(1) ;
}


