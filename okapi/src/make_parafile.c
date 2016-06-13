/******************************************************************************

  make_parafile.c

  Usage: make_parafile <database>

  Makes type 2 parafile for database

  Completely rewritten Nov 99 SW using make_para_rec() instead of monolithic

  See make_para_rec.c for record layout.

******************************************************************************/

#include "bss.h"
#include "para.h"

#define I0q if ((res = i0(command, response)) < 0) {\
  sprintf(command, "perror") ;\
  i0(command, response) ;\
  fprintf(stderr, response) ;\
  goto error ; }

#ifdef DEBUG
#define I0 fprintf(stderr, "%s\n", command) ;\
if (res = i0(command, response) < 0) {\
  sprintf(command, "perror") ;\
  i0(command, response) ;\
  fprintf(stderr, response) ;\
  goto error ; }\
  else fprintf(stderr, response) ;
#else
#define I0 I0q
#endif

extern Db Cdb;
extern FILE *Err_file ;

char buf[1024] ;
char com[1024] ;
char command[4096] ;		/* Don't think ever large here */
char *response ;
int res ;

char *Usage = "Usage: %s Db_name[ filestem]\n" ;

int
main(int argc, char *argv[])
{
  FILE *parp = stdout;
  FILE *pardirp ;
  char *fdstarts[MAXNUMFIELDS] ;
  int fdlens[MAXNUMFIELDS] ;
  int fdoffsets[MAXNUMFIELDS] ;
  int N ;
  int i, j ;
  int sod ;
  int reclen ;
  int totlen, hl_totlen ;
  int maxtoks ;
  char fnames[512] ;
  char *filestem = NULL ;
  
  if ( argc < 2 || strcmp(argv[1], "-h") == 0 ) {
    fprintf(stderr, Usage, argv[1]) ;
    exit(1) ;
  }
  response = (char *)malloc(MAXTEXTRECLEN) ;
  if ( response == NULL ) {
    fprintf(stderr, "%s: out of memory\n", argv[0]) ;
    exit(1) ;
  }
  if ( argc >= 3 ) filestem = argv[2] ;
  iinit() ;
  sprintf(command, "ch %s\n", argv[1]) ;
  I0q ;
  if ( Cdb.parafile_type != DEFAULT_PARAFILE_TYPE ) {
    fprintf(stderr, "Can only make paragraph file type %d\n",
	    DEFAULT_PARAFILE_TYPE) ;
    exit(1) ;
  }
  sprintf(command, "set f=3\n") ;
  I0q ;
  sprintf(command, "d bign\n") ;
  I0q ;
  N = atoi(response) ;
  if ( filestem == NULL ) 
    sprintf(fnames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".par") ;
  else 
    sprintf(fnames, "%s%s", filestem, ".par") ;
  parp = fopen(fnames, "wb") ; 
  if ( parp == NULL ) {
    fprintf(stderr, "%s: can't open output paragraph file %s\n",
	    argv[0], fnames) ;
    exit(1) ;
  }
  if ( filestem == NULL )
    sprintf(fnames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".pardir") ;
  else
    sprintf(fnames, "%s%s", filestem, ".pardir") ;
  pardirp = fopen(fnames, "wb") ; 
  if ( pardirp == NULL ) {
    fprintf(stderr, "%s: can't open output paragraph directory file %s\n",
	    argv[0], fnames) ;
    exit(1) ;
  }
  if ( Cdb.i_db_type & DB_TEXT_TYPE ) maxtoks = TEXTPOS_MAX_T ;
  else maxtoks = LONGPOS_MAX_T ;
  totlen = 0 ;
  for ( i = 1 ; i <= N ; i++ ) {
    sprintf(command, "s rn=%d\n", i) ;
    I0 ;
    sscanf(response + 86, "%6d", &sod) ; /* Start of data  */
    for ( j = 1, reclen = 0 ; j <= Cdb.nf ; j++ ) {
      sscanf(response + 95 + 19 * (j - 1) + 3, "%8d", &fdoffsets[j]) ;
      fdstarts[j] = response + sod + fdoffsets[j] ;
      sscanf(response + 95 + 19 * (j - 1) + 11, "%8d", &fdlens[j]) ;
      reclen += make_para_rec(j, fdstarts[j], fdlens[j],
			      Cdb.nf * Cdb.fddir_recsize + fdoffsets[j],
			      parp, maxtoks) ;
    }
#if HILO
    hl_totlen = totlen ;
#else
    hl_totlen = reverse(totlen) ;
#endif
    fwrite((void *)&hl_totlen, sizeof(u_int), 1, pardirp) ;
    totlen += reclen ;
  }
  iexit() ;
  exit(0) ;
 error:
  iexit() ;
  exit(1) ;
}

