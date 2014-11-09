/*****************************************************************************
  ix1_ex.new.c
  renamed ix1_ex.c Dec 93
  renamed ix1.c May 95

  from ix1_ex with bss_extract

  Procedure is read args, open Db, then for recs in specified range, or all,
  read rec, extract terms according to parms specified by Db.search_groups,
  "output" each term (OUTPUT(), in extract()); if this returns -1 (no more mem)
  scrub the current record's terms (because a record mustn't cross an output
  run boundary), call outrun() and then continue with the current
  record. outrun() sorts and amalgamates the term records currently in
  the output buffer and writes them to a temp file. Temp files are
  numbered 0000 to 9999 (with pid etc). Finally, when all records processed,
  call merge() repeatedly til done. The final output goes to stdout and temp
  files are deleted and less flagged not to be.

  Output runs MUST be merged in the order produced because merge()
  does the minimum of merging. This involves leftover temp files being
  renamed (look at the code, near end of main()). This has changed now
  (July 92).

  Feb 96: All index types < 4 are obsolete and < 8 obsolescent.

  The output is to stdout, often piped to ixf

  May 95: changing so that for each merge file it finds the first temp
  dir with enough space, writes it to that, and if it's not tdir[0] makes
  a soft link to it on tdir[0]. [???] I think this works (Dec 97)

  Sept 96: Big Ns file and avedoclens file are obsolete. If needed, revert to
  earlier release.
  

*****************************************************************************/

#define NO_TRIAL		/* trial facility disabled July 2000 as not
				   properly implemented */

#include "bss_indexing.h"

#include "para.h"

#ifndef NO_SIGNAL_H
#include <signal.h>
#endif
#ifndef NO_HASRLIMIT
#include <sys/resource.h>
#endif
#ifndef NO_MMAP
#include <sys/mman.h>
#endif

FILE *nullfile ;		/* Connected to /dev/null if trial only */
extern FILE *Err_file ;

FIELD_TYPE field_type ;		/* Obsolescent */

int recnum ;
int fdnum ;			/* Current field number (rep global Nov 89) */
int recs_done = 0 ;
int savrecs_done ;
int srec = 0, frec = 0 ;	/* Start and finish record numbers */
int rec_incr ;			/* Increment for recnum, 1 unless trial */
int savrecnum ;			/* Saved record number to restart after
				   memory full */
int savtermnum = 0 ;		/* Saved number of terms output up to end of
				   last record */
int reclen, fieldlen, origfieldlen ;

#ifdef HAVE_LL
LL_TYPE incharsthisrun ;
LL_TYPE lastoutchars ;
LL_TYPE totoutchars ;
LL_TYPE meanoutchars ;
LL_TYPE totinchars ;
#else
double incharsthisrun ;
double lastoutchars ;
double totoutchars ;
double meanoutchars ;
double totinchars ;
#endif
int recsthisrun ;
int recslastrun ;
int totrecs ;

char *fieldp, *origfieldp ;

STRINGS sorttab ;		/* For storing the extracted keys */
  
Gsl *gs ;			/* The one being used */
  
size_t memavail = MEMAVAIL * 1024 * 1024 ; /* MEMAVAIL now given in MB */

/* Did some testing on this with TREC Apr 93. Found 19 bit over I think */
/* Index types 0-13 and 20 resp, but only anywhere near right for words.*/
int avekeylen[] = {
  11, 19, 15, 18, 13, 13, 13, 13, 14, 14, 10,
  11, 15, 15, 0, 0, 0, 0, 0, 0, 10
} ;

int maxmem, numkeys ;

char **sortptrs ;		/* Array of numkeys pointers */
char *sortspace ;		/* Array of maxmem characters */
  
char fnames[PATHNAME_MAX], fnames1[PATHNAME_MAX] ;

int lastsuffix = -1 ;		/* To distinguish merge file names */
                                /* Suffixes are 0000 - 9999 */
int firstsuffix = 0 ;		/* Earliest suffix still live */
int suffix = 0 ;		/* Only obs type 0 index */
int suff ;			/* = suffix or lastsuffix as reqd */
int numruns = 0 ;		/* Total number of temp files */
int estruns ;			/* Est. total number of runs */
int maxruns ;			/* Max runs wh can be merged in one pass */
int maxfiles = 64 - 3 ;		/* Reasonable value but hope to increase it,
				   see setrlimit() below */
int num_tdirs = 0 ;
int current_tdir = 0 ;		/* Starts with tdir[0] always */
char *tdirs[MAX_TDIRS] ;	/* Address of up to 10 dirs for tempfiles */
int tempspace[MAX_TDIRS] ;	/* Available space on each (in M) after
				   deducting minfree */
int tdir_minfree[MAX_TDIRS] ;	/* Should be M I think */
int tempsize[1000000] ;		/* Size of corresponding temporary file mko */
int tdirnum[1000000] ;		/* Dir number for file number n. (May 95) mko */

BOOL no_i = FALSE ;
BOOL do_doclens = FALSE ;
BOOL termunit = FALSE ;		/* But if TRUE doclen is in indexed terms not
				   input characters as it was up to end Nov 99 */
BOOL no_merge = FALSE ;
BOOL deltmp = FALSE ;		/* Delete temp files after each nonfinal
				   merge if true (used to be
				   always true -- May 95, now runtime arg) */
BOOL delfinal = FALSE ;		/* Delete final tempfiles */
BOOL Mmap = FALSE ;
BOOL Lock = FALSE ;		/* If TRUE and Mmap and user is root will try
				   to lock mapped pages in core */
BOOL silent = FALSE ;
BOOL trial = FALSE ;		/* If TRUE, testing only */

BOOL made_temp_files = FALSE ;

int dflg = 0 ;			/* Debug code - bits used.
				   1 bit displays phase1 etc
				   2 bit displays extracted terms. 
				   (Oct 96) now using BSS global Dbg */
BOOL finished = FALSE ;

int (*outrunfunc)() ;
int outrun_4(), outrun_10(), outrun_11(), outrun_12(), outrun_20() ;
int outrunchars ;		/* Chars written by outrunfunc(), or -1 */

int outflag ;
int sources ;			/* Apr 02 */

int result ;

#define DUMP 1			/* Clash with D_PSTGS*/
#define OUTDUMP 2		/* Clash with DD_PSTGS */

char *Usage = "\
Usage: [-m[em] <mem>] "
#ifndef NO_LN
"[-t <tempdir> <minfree> [-t <tempdir> <minfree>]]\n"
#else
"[-t <tempdir> <space available>]\n"
#endif
"[-h[elp]] [-s[tart] <start_rec>] [-f[inish] <finish_rec>] [-silent]\n"
"[-quiet] [-d[ebug] <debug code>] [-c <control path>] [-maxruns <num>]\n"
#ifndef NO_TRIAL
"[-trial <trialnum>] "
#endif /* NO_TRIAL */
"[-[no]index] [-[no]doclens] [-termunit] [-[no]merge]\n"
"[-[no]deltmp] [-[no]delfinal] [-l[im] limit mask]"
#ifndef NO_MMAP
" [-mmap [-mlock]]"
#endif /* NO_MMAP */
"\n<database> <index number> > <output file>\n\n"
"Database name and index number must be the last two args, otherwise order\n"
"is unimportant.\n" ;

#ifndef NO_TRIAL
char helpstring[] = "\
Defaults & explanations:\
<database> is the database parameter file name (NOT path).\
<index number> must be within the range in the database parameter\
These two must appear last.\
<mem>: the unit is 1 MB, default 20\
<start>: default 1st record; <finish>: default last record.\
<tempdir>: temp directory for pre-merge files:\
  full pathname with or without trailing slash\
  You can allocate up to 10 of these, but they must be on separate file\
  systems. Default path %s\
  <minfree> is the amount of space in MB which must be left (default 0)\
Temporary filenames are ix1.<pid>.0000-9999\
<control directory>: directory for database parameter files,\
  default %s\
<maxruns> must be at least 2 & <= %d\
trial: every <trialnum>th record is processed (with output discarded),\
and then the program estimates the amount of space needed for runfiles\
and intermediate merge files.\
noindex inhibits output of index (default index)\
quiet inhibits some diagnostic output, silent almost all.\
nomerge prevents final merging and leaves temp files (default merge)\
doclens/nodoclens contributes/doesn't contribute to the doclength file\
if doclens, termunit makes doclength unit number of indexed terms\
otherwise it's the number of input characters.\
deltmp/nodeltmp deletes/doesn't delete the temporary files after nonfinal merges.\
delfinal/nodelfinal deletes/doesn't delete the temporary files after final merge.\
<limit mask> restricts indexing to docs which match the mask\n"
#ifndef NO_MMAP
"<mmap> if possible uses mapped memory instead of allocated (default is allocated)\
If mapping is supported and program is run as root you can specify -mlock\
which will try to lock all mapped pages into core (not in windows versions).\n"
#endif /* NO_MMAP */
"Defaults: maxruns %d, start and finish 1st and last rec of database\
trial no, index yes, doclens no, merge yes,\
quiet no, silent no, debug no, deltmp no, delfinal no.\
Program is often run with output piped to ixf, which puts the structure in.\
" ;
#else
char *helpmess ;
char helpstring[] = "\
Args & defaults:\n\
<database> is the database parameter file name (NOT path).\n\
<index number> must be within the range in the database parameter\n\
These two must appear last.\n\
<mem>: the unit is 1 MB, default 20, maximum 2047\n\
<start>: default 1st record; <finish>: default last record.\n"
#ifndef NO_LN
"<tempdir>: temp directory for pre-merge files:\n\
  full pathname with or without trailing slash\n\
  You can allocate up to 10 of these, but they must be on separate file\n\
  systems. Default path %s\n\
  <minfree> is the amount of space in MB which must be left (default 0)\n"
#else
"<tempdir>: temp directory for pre-merge files: full pathname with or without\n\
  trailing slash. Default path %s\n\
  <space available> is the amount of space in MB available on <tempdir>\n"
#endif /* NO_LN */
"Temporary filenames are ix1.<pid>.0000-9999\n\
<control directory>: directory for database parameter files,\n\
  default %s\n\
<maxruns> must be at least 2 & <= %d\n\
noindex inhibits output of index (default index)\n\
quiet inhibits some diagnostic output, silent almost all.\n\
nomerge prevents final merging and leaves temp files (default merge)\n\
doclens/nodoclens contributes/doesn't contribute to the doclength file\n\
if doclens, termunit makes doclength unit number of indexed terms\n\
otherwise it's the number of input characters.\n\
deltmp/nodeltmp deletes/doesn't delete the temporary files after nonfinal merges.\n\
delfinal/nodelfinal deletes/doesn't delete the temporary files after final merge.\n\
<limit mask> restricts indexing to docs which match the mask.\n"
#ifndef NO_MMAP
"<mmap> if possible uses mapped memory instead of allocated (default is allocated)\n\
If mapping is supported and program is run as root you can specify -mlock\n\
which will try to lock all mapped pages into core (not in windows versions).\n"
#endif /* NO_MMAP */
"Defaults: maxruns %d, start and finish 1st and last rec of database\n\
index yes, doclens no, merge yes,\n\
quiet no, silent no, debug no, deltmp no, delfinal no.\n\
Program is often run with output piped to ixf, which puts the structure in.\
" ;
#endif /* NO_TRIAL */

char trialmess[] = "\
This is a trial run only. All output will be redirected to /dev/null.\n\
After a 1:%d sample has been processed the program will report an\n\
estimate of the amount of space needed for runfiles and the amount needed\n\
for merging.\n" ;

char *progname ;

int
main(int argc, char *argv[])
{
  int senum ;
  int sentence_max_tokens ;
  int max_sentences ;
  int itype ;			/* Index type */
  int outlen ;
  char *findfld() ;
  int mergesize ;
  int trialnum ;
  int levels, runsleft ;
  int num_merges_this_level ;
  int runsthislevel ;
  int runstodo ;
  u_short lim_mask = 0 ;
  Extraction_regime reg ;

  int i, jj ;

  int arg ;
  int index_number ;

  int nk ;

  OFFSET_TYPE mergeres ;

  int skipped_recs = 0 ;

  int *doclens ;		/* malloc'd array of doclens for the index.*/

  char ofstem[PATHNAME_MAX] ;

  /* Few vars for type 3 index */

  int input_this_rec ;
  int terms_this_rec ;
  int numtoks ;
  int *tokens = NULL ;		/* tokenize_text() creates and resizes
				   the array as required. It is never
				   freed. */

  char *st ;
  int numpr = 0 ;

  Err_file = stderr ;
  Pagesize = getpagesize() ;
  progname = lastslash(argv[0]) ;

  /* Set up maxfiles */

#ifndef NO_HASRLIMIT
  {
    struct rlimit rlp ;
    getrlimit(RLIMIT_NOFILE, &rlp) ;
    maxfiles = rlp.rlim_cur - 3 ;
  }
#endif /* NO_HASRLIMIT */

  maxruns = maxfiles - 5 ;
  if ( (st = getenv("BSS_PARMPATH")) != NULL ) Control_dir = st ;
  else Control_dir = CONTROL_DIR ;
  if ( (st = getenv("BSS_TEMPPATH")) != NULL ) Temp_dir = st ;
  else Temp_dir = TEMP_DIR ;
  helpmess = malloc(strlen(helpstring) + 256) ;
  sprintf(helpmess, helpstring, Temp_dir, Control_dir,
	  maxfiles, maxfiles) ;

  if ( argc < 3 ) goto badargs ;

  for ( arg = 1 ; arg < argc - 2 ; arg++ ) {
    if ( strcmp(argv[arg], "-c") == 0 ) {
      Control_dir = argv[arg + 1] ;
    }
  }

  if ( open_db(argv[argc - 2], &Cdb, DB_OPEN_NOSEARCH) == -1 ) {
    fprintf(stderr, "\nError opening database %s",
	    argv[argc - 2]) ;
    goto badargs ;
  }

  if ( Cdb.nr == 0 ) {
    if ( ! silent ) fprintf(stderr, "No records, nothing to index\n") ;
    exit(0) ;
  }

  maxfiles -= Cdb.lastbibvol + 1 + 1 ;
  index_number = atoi(argv[argc - 1]) ;
  if ( index_number < 0 || index_number >= Cdb.ni ) goto badargs ;

  itype = Cdb.itype[index_number] ;
  if ( itype < 4 || (itype > 13 && itype != 20) ) {
    fprintf(stderr, "\nDon't know about index type %d", itype) ;
    exit(1) ;
  }
    
  if ( setup_ixparms(index_number) < 0 ) {
    fprintf(stderr, "setup_gsl_sg() failed\n") ;
    exit(1) ;
  }

  for ( arg = 1 ; arg < argc - 2 ; arg++ ) {
    BOOL found ;
    int minfreetemp ;
    char *tdtemp ;

    if ( strcmp(argv[arg], "-s") == 0 || strcmp(argv[arg], "-start") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      srec = atoi(argv[++arg]) ;
    }
    else if ( strcmp(argv[arg], "-f") == 0 ||
	      strcmp(argv[arg], "-finish") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      frec = atoi(argv[++arg]) ;
    }
      else if ( strcmp(argv[arg], "-maxruns") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      maxruns = atoi(argv[++arg]) ;
      if ( maxruns > maxfiles || maxruns < 2 ) {
	fprintf(stderr, "\nmaxruns must be > 1 and <= %d", maxfiles) ;
	goto badargs ;
      }
      if ( maxruns > maxfiles - 4 )
	fprintf(stderr,
		"\nWarning: maxruns may be reduced from %d by the system",
		maxruns) ;
    }
    else if ( strcmp(argv[arg], "-m") == 0 ||
	      strcmp(argv[arg], "-mem") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      memavail = atoi(argv[++arg]) ;
      if ( memavail > 2047 ) {
	fprintf(stderr, "Can't allocate more than 2047 MB of memory\n") ;
	goto badargs ;
      }
      memavail *= 1024 * 1024 ;
    }
    else if ( strcmp(argv[arg], "-t") == 0 ) {
      if ( arg >= argc - 3 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      else if ( num_tdirs == MAX_TDIRS ) {
	fprintf(stderr, "\nCan't have more than %d temp dirs", MAX_TDIRS) ;
	goto badargs ;
      }
#ifndef NO_LN
      minfreetemp = atoi(argv[arg + 2]) ;
      if (minfreetemp > 4095) {
	fprintf(stderr,
		"\nMin free space on a temp dir can't be more than 4095 MB");
	goto badargs ;
      }
#else
      /* In this case minfreetemp is used for free space */
      minfreetemp = atoi(argv[arg + 2]) ;
#endif /* NO_LN */
      tdtemp = argv[arg + 1] ; 
      for ( i = 0, found = FALSE ; i < num_tdirs ; i++ ) {
	if ( strcmp(tdtemp, tdirs[i]) == 0 ) {
	  found = TRUE ;
	  break ;
	}
      }
      if ( ! found ) {
	tdirs[num_tdirs] = tdtemp ;
#ifndef NO_LN
	tdir_minfree[num_tdirs++] = minfreetemp ;
#else
	tempspace[num_tdirs] = minfreetemp ;
	tdir_minfree[num_tdirs++] = 0 ;
#endif
      }
      else {
	fprintf(stderr, "\nAlready got %s as a temp dir - ignoring", tdtemp) ;
	free(tdtemp) ;
      }
      arg += 2 ;
    }
    else if ( strcmp(argv[arg], "-d") == 0 ||
	      strcmp(argv[arg], "-debug") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      dflg = atoi(argv[++arg]) ;
    }
    else if ( strcmp(argv[arg], "-silent") == 0 ) silent = TRUE ;
    else if ( strcmp(argv[arg], "-quiet") == 0 ) silent = TRUE ;
    else if ( strcmp(argv[arg], "-index") == 0 ) no_i = FALSE ;
    else if ( strcmp(argv[arg], "-noindex") == 0 ) no_i = TRUE ;
    else if ( strcmp(argv[arg], "-merge") == 0 ) no_merge = FALSE ;
    else if ( strcmp(argv[arg], "-nomerge") == 0 ) no_merge = TRUE ;
    else if ( strcmp(argv[arg], "-doclens") == 0 ) do_doclens = TRUE ;
    else if ( strcmp(argv[arg], "-nodoclens") == 0 ) do_doclens = FALSE ;
    else if ( strcmp(argv[arg], "-termunit") == 0 ) termunit = TRUE ;
    else if ( strcmp(argv[arg], "-deltmp") == 0 ) deltmp = TRUE ;
    else if ( strcmp(argv[arg], "-nodeltmp") == 0 ) deltmp = FALSE ;
    else if ( strcmp(argv[arg], "-delfinal") == 0 ) delfinal = TRUE ;
    else if ( strcmp(argv[arg], "-nodelfinal") == 0 ) delfinal = FALSE ;
#ifndef NO_MMAP
    else if ( strcmp(argv[arg], "-mmap") == 0 ) Mmap = TRUE ;
    else if ( strcmp(argv[arg], "-mlock") == 0 ) Lock = TRUE ;
#endif
#ifndef NO_TRIAL
    else if ( strcmp(argv[arg], "-trial") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      trial = TRUE ;
      trialnum = atoi(argv[++arg]) ;
      if ( trialnum < 1 ) {
	fprintf(stderr,
		"\nTrial sample number must be at least 1") ;
	goto badargs ;
      }
    }
#endif /* NO_TRIAL */
    else if ( strcmp(argv[arg], "-l") == 0 || strcmp(argv[arg], "-lim") == 0 ) {
      if ( arg >= argc -2 ) {
	fprintf(stderr, "\nNot enough args following %s", argv[arg]) ;
	goto badargs ;
      }
      lim_mask = atoi(argv[++arg]) ;
    }
    else if ( strcmp(argv[arg], "-h") == 0 ||
	     strcmp(argv[arg], "-help") == 0 ) {
      fprintf(stderr, "%s\n%s\n", Usage, helpmess) ;
      exit(1) ;
    }
    
    else if ( strcmp(argv[arg], "-c") == 0 ) arg++ ;
    else {
      fprintf(stderr, "\nUnknown arg %s\n", argv[arg]) ;
      exit(1) ;
    }
  }

  if ( ! silent ) {
    fprintf(stderr, "\nStage 1 indexing program %s", progname) ;
    fprintf(stderr,
	    "\nRevision dates: major Sep 24 1996, minor Aug 30 2001\n") ;
  }

  Dbg = ( dflg & 0xfffffffc ) ;	/* Think this is right (excluding 1|2) */
  bss_syslog = stderr ;

  if ( no_i ) no_merge = TRUE ;

#ifdef NO_MMAP
  if ( Mmap ) {
    fprintf(stderr, "Memory mapping not supported in this version\n") ;
    Mmap = FALSE ;
  }
#endif /* NO_MMAP */
    
#ifdef NO_MLOCK
  if ( Lock ) {
    fprintf(stderr, "Memory locking not supported in this version\n") ;
    Lock = FALSE ;
  }
#endif /* NO_MLOCK */
  
  if ( lim_mask ) {
    result = prepare_limits(&Cdb) ;
    if ( result == 2 ) {
      fprintf(stderr, "Database appears not to have limits\n") ;
      exit(1) ;
    }
    else if ( result < 0 ) {
      fprintf(stderr, "Can't open .lims file\n") ;
      exit(1) ;
    }
  }
#ifndef NO_TRIAL
  if ( trial ) do_doclens = FALSE ;
#endif

  /* Used to read any existing doclens records here, now no point */

  if ( do_doclens ) {
    if ( (doclens = (int *)calloc(Cdb.nr, sizeof(int))) == NULL ) {
      fprintf(stderr, "Can't allocate memory for doclens array, quitting\n") ;
      exit(1) ;
    }
  } /* (do_doclens) */

  if ( srec == 0 ) srec = 1 ;
  if ( srec <= 0 || srec > Cdb.nr ) {
    fprintf(stderr, "\nStart rec must be between 1 and %d\n", Cdb.nr) ;
    exit(1) ;
  }
  if ( frec == 0 ) frec = Cdb.nr ;
  if ( frec <= 0 || frec < srec || frec > Cdb.nr ) {
    fprintf(stderr, "\nFinish rec must be between start rec and %d\n", Cdb.nr) ;
    exit(1) ;
  }

  /* Check space & write perm on temp dirs */

  if ( num_tdirs == 0 ) {
    tdirs[num_tdirs++] = Temp_dir ;
    tdir_minfree[0] = 0 ;
  }

  if ( num_tdirs && ! silent )
    fprintf(stderr, "%d temporary directory/ies:", num_tdirs) ;
  for ( i = 0 ; i < num_tdirs ; i++ ) {
    if ( ! silent ) fprintf(stderr, "%s, %d megabytes reserved\n",
	    tdirs[i], tdir_minfree[i]) ;
#ifndef NO_DF
    tempspace[i] = dfk(tdirs[i]) / 1024 ;
    if ( tempspace[i] <= 0 ) {
      fprintf(stderr, "\nUnable to write on %s", tdirs[i]) ;
      fprintf(stderr, "\nTry again without it or try another path") ;
      /* But does dfk() check writeability??*/
      exit(1) ;
    }
    else if ( tempspace[i] - tdir_minfree[i] < 0 ) {
      fprintf(stderr, "\nNo usable space on %s, try again without it",
	      tdirs[i]) ;
      exit(1) ;
    }
    else if ( (tempspace[i] - tdir_minfree[i]) < 5 ) {
      fprintf(stderr, "\nNot enough space on %s to bother with", tdirs[i]) ;
      fprintf(stderr, "\nTry again without it or try another path") ;
      exit(1) ;
    }
    else {
      tempspace[i] -= tdir_minfree[i] ;
      if ( ! silent ) fprintf(stderr, "\n%d megabytes available on %s",
	      tempspace[i], tdirs[i]) ;
    }
#endif /* NO_DF */
  }

  if ( itype == 4 || itype == 6 || itype == 8) {
    outrunfunc = outrun_4 ;
    outflag = EX_OP_IX4 ;
  }
  else if ( itype == 5 || itype == 7 || itype == 9) {
    outrunfunc = outrun_4 ;	/* sic */
    outflag = EX_OP_IX5 ;
  }
  else if ( itype == 10 /* || itype == 20 */ ) {
    outrunfunc = outrun_10 ;	
    outflag = EX_OP_IX10 ;
  }
  else if ( itype == 11 ) {	/* Like 10 but 4-byte recnums */
    outrunfunc = outrun_11 ;	
    outflag = EX_OP_IX11 ;
  }
  else if ( itype == 12 ) {	/* Like 8 but 4-byte recnums */
    outrunfunc = outrun_12 ;	
    outflag = EX_OP_IX12 ;
  }
  else if ( itype == 13 ) {	/* Like 7 but 4-byte recnums */
    outrunfunc = outrun_12 ;	/* sic */
    outflag = EX_OP_IX13 ;
  }
  else if ( itype == 20 ) {	/* Profile type */
    outrunfunc = outrun_20 ;	/* July 02: tried to use _10 but failed */
    outflag = EX_OP_IX20 ;
  }
  if ( no_i ) outflag = 0 ;
  if ( dflg & OUTDUMP )
    outflag |= EX_DIAG ; /* Note same as DD_PSTGS in BSS */

  sources = (outflag & EX_SOURCE) ; /* Apr 02 */

  numkeys = memavail / ( avekeylen[itype] + sizeof(char *) ) ;
  maxmem = memavail - numkeys * sizeof(char *) ;
  if ( ! silent )
    fprintf(stderr, "\n%d max string space, %d max strings", maxmem, numkeys) ;

#ifndef NO_MMAP
  if ( Mmap ) {
#ifndef NO_MLOCK
    if ( Lock ) {
      result = mlockall(MCL_FUTURE) ;
      if ( result < 0 && ! silent )
	fprintf(stderr, "\nMapped memory lock failed") ;
    }
#endif /* NO_MLOCK */
    result = open("/dev/zero", O_RDWR) ;
    if ( result < 0 ) {
      fprintf(stderr, "\nNo /dev/zero, can't map memory") ;
      goto fatal ;
    }
    sortptrs = (char **)mmap(0, (size_t)((unsigned)numkeys * sizeof(char *)),
			     PROT_READ|PROT_WRITE, MAP_SHARED, result, 0) ;
    sortspace = ( char *)mmap(0, (size_t)((unsigned)maxmem),
			      PROT_READ|PROT_WRITE, MAP_SHARED, result, 0) ;
    close(result) ;
    if ( sortptrs == (char **)(-1) || sortspace == (char *)(-1) ) {
      fprintf(stderr,
	      "\nCan't map enough memory - try allocating less");
      goto fatal ;
    }
  }
  else /* not Mmap */
#else
    if ( Mmap )
      fprintf(stderr,
	      "Memory mapping not supported, will use malloc\n") ;
#endif /* NO_MMAP */
    {
    sortptrs = (char **) malloc((unsigned)numkeys * sizeof(char *)) ;
    sortspace = ( char *) malloc((unsigned)maxmem) ;
    if ( sortptrs == NULL || sortspace == NULL ) {
      fprintf(stderr,
	      "\nOut of memory - try allocating less");
      goto fatal ;
    }
  }

#ifndef _WIN32
  signal(SIGHUP, icleanup) ;
  signal(SIGINT, icleanup) ;
  signal(SIGQUIT, icleanup) ;
#endif

  /* Initialise term extraction parameters  */
  
  sorttab.p = sortptrs ;
  sorttab.data = sortspace ;
  sorttab.maxnum = numkeys ;
  sorttab.maxsize = maxmem ;

#ifndef NO_TRIAL
  if ( trial ) {
    nullfile = fopen("/dev/null", "wb") ;
    maxfiles-- ;
    fprintf(stderr, trialmess, trialnum) ;
  }
#endif

  if ( maxruns > maxfiles ) {
    if ( ! silent )
      fprintf(stderr, "\nReducing maxruns (%d) to %d", maxruns, maxfiles) ;
    maxruns = maxfiles ;
  }
  if ( maxruns > 245 ) {	/* Don't trust _file > 255 even on SysV */
    if ( ! silent )
      fprintf(stderr, "\nReducing maxruns (%d) to %d", maxruns, 245) ;
    maxruns = 245 ;
  }

  recnum = savrecnum = srec ;
  if ( trial ) rec_incr = trialnum ;
  else rec_incr = 1 ;

  if ( itype == 4 || itype == 6 || itype == 8 || itype == 12 ) {
    sentence_max_tokens = TEXTPOS_MAX_T ;
    max_sentences = TEXTPOS_MAX_S ;
  }
  if ( itype == 10 || itype == 11 || itype == 20 ) {
    sentence_max_tokens = MAXTF ;
    max_sentences = TEXTPOS_MAX_S ; /* Don't think it really applies */
  }
  else {
    sentence_max_tokens = LONGPOS_MAX_T ;
    max_sentences = LONGPOS_MAX_S ;
  }
  /* Bit spurious for index type 10 or 11 or 20 where it doesn't matter
     as no positional fields */
 
 main_loop:
  
  initrun(&sorttab) ;

  for ( ; recnum <= frec ; recnum += rec_incr, recs_done++ ) {

    savrecnum = recnum ;
    savtermnum = sorttab.n ;
    savrecs_done = recs_done ;
    input_this_rec = terms_this_rec = 0 ;
    if ( (lim_mask && ! Sats_limit(recnum, lim_mask)) ) {
      continue ;
    }
    reclen = rd_recnum(recnum, -1, &Cdb) -
      Cdb.fddir_recsize * Cdb.nf - Cdb.fixed ;
    if ( ! silent && recnum % 500 == 0 ) {
      if ( numpr >= 8 ) numpr = 0 ;
      if ( numpr == 0 ) fprintf(stderr, "\n") ;
      fprintf(stderr, "%9d", recnum) ;
      numpr++ ;
    }
    if ( dflg & (DUMP|OUTDUMP)) fprintf(stderr, "\nRec %d", recnum) ;
    /* incharsthisrun += reclen ; */
    /* totinchars += reclen ; */
    recsthisrun++ ;
    totrecs++ ;
    if ( dflg & DUMP )
      /* Prevents D_PSTGS */
      fprintf(stderr, "\nRecord %d:", recnum) ;
    if (reclen < 0 ) {
      fprintf(stderr, "\nCan't read record %d, skipping", recnum) ;
      skipped_recs++ ;
      if ( skipped_recs > Cdb.nr / 1000 ) {
	fprintf(stderr, "\nToo many bad records, (%d), giving up",
		skipped_recs) ;
	goto fatal ;
      }
      continue ;
    }

    if ( ! silent && (dflg & 5) && recnum % 100 == 0 ) {
      fprintf(stderr, "%d %d\n", recnum, sorttab.n) ;
    }

    for ( fdnum = 1 ; fdnum <= Cdb.nf ; fdnum++ ) {
      if ( num_ixparms[fdnum] == 0 ) continue ;

      origfieldp=fieldp=findfld(fdnum, Cdb.recbuf, &fieldlen, &Cdb, reclen) ;
      origfieldlen = fieldlen ;
      if ( dflg & DUMP )
	fprintf(stderr, "Field %d:\n%.*s\n", fdnum, fieldlen, fieldp) ;
      if ( fieldlen <= 0 ) continue ;
      field_type = Cdb.field_types[fdnum] ;
      for ( jj = 0 ; jj < num_ixparms[fdnum] ; jj++ ) {
	reg = ixparms[fdnum][jj].reg ;
	gs = ixparms[fdnum][jj].gsl ;
	stemfn = ixparms[fdnum][jj].stem_fn ;

	fieldp = origfieldp ; fieldlen = origfieldlen ;
	{ /* One extract call */
	  /* N.B. Can't use extract_text() here in its present form (Apr 02)
	     as it can't handle output to a STRINGS struct. */
	  int l = fieldlen ;
	  char *p = fieldp ;
	  char *sp ;
	  int pl, sl ;
	  int paranum = 0 ;
	  int stopped ;		/* Added Jan 98: see phase2_token and
				   extract() */
	  senum = 0 ;
	  while ( l > 0 ) {
	    if ( itype == 4 || itype == 6 || itype == 8 /* || itype == 10 */ ||
		 itype == 12 )	/* silly */
	      pl = find_para(p, l, INDENT|GAP) ; /* or cd use parafile, because
						    must be consistent with
						    rule used */
	    else
	      /* itype 5 or 7 or 9 or 10 or 11 or 20 - tho' find_para shd be OK
		 (but cdnt use parafile) */
	      pl = l ;
	    stopped = FALSE ;
	    input_this_rec += pl ;
	    if ( dflg & DUMP ) fprintf(stderr, "Para %d\n", paranum) ;
	    paranum++ ;
	    l -= pl ; sp = p ; p += pl ;
	    if (no_i) continue ;
	    while ( pl > 0 ) {
	      sl = find_sentence(sp, pl, 0, sentence_max_tokens, 0) ;
	      if ( dflg & DUMP ) {
		fprintf(stderr, "Sentence %d\n", senum) ;
		fprintf(stderr, "**%.*s**\n", sl, sp) ;
	      }
	      if ( sources ) {
		numtoks = tokenize_text(sp, sl, &tokens) ;
		if ( dflg & DUMP ) fprintf(stderr, "%d toks\n", numtoks) ;
		if ( numtoks > sentence_max_tokens ) 
		  if ( ! silent )
		    fprintf(stderr, "\nrec %d: %d toks in sntce %d, max is %d",
			    recnum, numtoks, senum, sentence_max_tokens) ;
	      }
	      nk = extract(sp, sl, reg, gs, stemfn, &sorttab, outflag,
			   recnum, 0, fdnum - 1, senum, tokens, &stopped,
			   &outlen) ;
	      pl -= sl ; sp += sl ;
	      if ( nk == -1 ) break ;
	      terms_this_rec += nk ;
	      senum++ ; 
	    }
	    if ( nk == -1 ) break ;
	  } /* ( l > 0 ) */
	  /* This depends on index type, put right sometime */
	  /* Above comment meaning? */
	  if ( senum > max_sentences )
	    if ( ! silent ) fprintf(stderr,
    "\nrec %d: %d sentences, max is %d, no pos recs generated for the excess",
		    recnum, senum, max_sentences) ;
	} /* (One extract call) */
	      
	if ( nk == -1 && ! no_i ) {
	  recsthisrun-- ;
	  totrecs-- ;
	  if ( ! silent )
	    fprintf(stderr,
		    "\nRec %d, mean lgth this run %.0f, mean so far %.0f",
		    recnum - 1, (double)(incharsthisrun/recsthisrun),
		    (double)(totinchars/totrecs)) ;
	  sorttab.n = savtermnum ;
	  suff = ++lastsuffix ;

	  if ( make_run() == -1 ) goto fatal ;
#ifndef NO_TRIAL
	  if ( trial ) estimate() ;
#endif
	  goto main_loop ;
	}			/* (nk = -1) */
	if ( nk < -1 && ! no_i ) {
	  fprintf(stderr, "\nextract returned %d", nk) ;
	  fprintf(stderr, "\n%.*s", origfieldlen, origfieldp) ;
	}
      } /* (another parm this field) */
    }     /* while more fields */

    incharsthisrun += input_this_rec ;
    totinchars += input_this_rec ;

    if ( do_doclens ) {
      if ( termunit ) input_this_rec = terms_this_rec ;
      doclens[recnum - 1] = input_this_rec ;
    }
    continue ;

  } /* while more records */


  if ( ! silent )
#ifdef HAVE_LL
#ifdef _WIN32
    fprintf(stderr,
	    "\nDone last record %d, total read %I64d, \n\
mean lgth this run %.0f, overall mean %.0f",
	    recnum - 1, totinchars, (double)(incharsthisrun/recsthisrun),
	    (double)(totinchars/totrecs)) ;
#else
    fprintf(stderr,
	    "\nDone last record %d, total read %lld, \n\
mean lgth this run %.0f, overall mean %.0f",
	    recnum - 1, totinchars, (double)(incharsthisrun/recsthisrun),
	    (double)(totinchars/totrecs)) ;
#endif
#else
    fprintf(stderr,
	    "\nDone last record %d, total read %.0f, \n\
mean lgth this run %.0f, overall mean %.0f",
	    recnum - 1, totinchars, (double)(incharsthisrun/recsthisrun),
	    (double)(totinchars/totrecs)) ;
#endif
  suff = ++lastsuffix ;

  if ( make_run() == -1 ) goto fatal ;

  if ( do_doclens ) {
    int dfd ;
    int numrecs = frec - srec + 1 ;
    int oflags ;
    int result ;

    oflags = (O_WRONLY|O_CREAT|O_BINARY) ;
    if ( numrecs == Cdb.nr ) oflags |= O_TRUNC ;
    sprintf(ofstem, "%s"D_C"%s.%d.dlens",
	    dirnameh(Cdb.ix_stem[index_number][0]), Cdb.name, index_number) ;
    if ( (dfd = open(ofstem, oflags, 0644)) >= 0 ) {
      result = (int)lseek(dfd, (OFFSET_TYPE)((srec - 1) * sizeof(int)),
			  SEEK_SET) ;
      if ( result == -1 ) {
	fprintf(stderr, "Can't seek to rec %d in doclens file %s\n",
		srec - 1, ofstem) ;
	fprintf(stderr, "Doclens file not written\n") ;
      }
      else {
	result =
	  (int)write(dfd, (void *)(doclens + srec - 1),
		     (size_t)(sizeof(int) * numrecs) ) ;
	if ( result == sizeof(int) * numrecs ) {
	  if ( ! silent )
	    fprintf(stderr, "Written %d records to doclens file\n",
		  result / sizeof(int)) ;
	}
	else {
	  if ( result > 0 )
	    fprintf(stderr, "Only written %d of %d records to doclens file\n",
		    result / sizeof(int), Cdb.nr) ;
	  else fprintf(stderr, "Failed to write doclens file\n") ;
	}
      } /* (seek OK) */
      close(dfd) ;
    }
    else fprintf(stderr, "Couldn't open doclens file %s, check permissions\n",
		 ofstem) ;
  } /* (do_doclens) */

  suffix++ ;


  if ( ! no_merge ) {
    /* Merge until done */
    if ( ! silent ) fprintf(stderr, "\nEntering merge, numruns=%d", numruns) ;

    finished = FALSE ;
    runsleft = numruns ;
    levels = 0 ;
    while (runsleft > maxruns) {
      levels++ ;
      if ( lige(runsleft, maxruns) < maxruns ) {
	/* Find least m s.t. lige(runsleft, m) <= maxruns */
	for ( i = maxruns ; i > 0 ; i-- )
	  if ( lige(runsleft, i) > maxruns ) break ;
	mergesize = i + 1 ;
      }
      else mergesize = maxruns ;
      num_merges_this_level = lige(runsleft, mergesize) ;

      for ( i = 0, runsthislevel = numruns ; i < num_merges_this_level ; i++ ){
	runstodo = (runsthislevel >= mergesize) ? mergesize : runsthislevel ;
	if ( ! silent ) {
	  if ( runstodo > 1 ) {
	    if ( ! silent )
	      fprintf(stderr,
		      "\nMerging %d runs %d - %d into %d",
		      runstodo, firstsuffix, firstsuffix + runstodo -1,
		      lastsuffix + 1) ;
	  }
	  else if ( ! silent )
	    fprintf(stderr, "\n\"Merging\" one run from %d to %d",
		    firstsuffix, lastsuffix + 1) ;
	}
	if ( ! no_i ) {
	  /* About how much output? */
	  int j ;
	  int howmuch ;		/* Now MB */
	  for ( j = firstsuffix, howmuch = 0 ; j < firstsuffix + runstodo ;
	       j++ )
	    howmuch += (tempsize[j] / 1024 / 1024) + 1 ;
	  fprintf(stderr, "\nMerge output est %d MB", howmuch) ;
	  /* Where to put it? */
	  for ( j = 0 ;
	       j < num_tdirs && tempspace[j] < howmuch ; j++ ) ;
	  if ( j == num_tdirs ) { /* Can't do it */
	    fprintf(stderr,
   "\nNo more temporary file paths with free space, need about %d MB.",
		    howmuch);
#if ! defined(NO_SIGSTOP) && ! defined (NO_DF)
	    fprintf(stderr, "\nStopping the process (pid=%d)", (int)getpid()) ;
	    fprintf(stderr,
    "\nPerhaps you can delete some files or move and link some files from %s",
		    tdirs[0]) ;
	    fprintf(stderr, "\nSend CONT signal when ready") ;
	    kill(getpid(), SIGSTOP) ;
	    for ( j = 0 ; j < num_tdirs ; j++ ) 
	      tempspace[j] = dfk(tdirs[j])/1024 - tdir_minfree[j] ;
	    j = 0 ;
#else
	    goto fatal ;
#endif /* ! NO_SIGSTOP && ! NO_DF */
	  }

	  current_tdir = j ;
	  if ( ! silent && j != current_tdir ) {
	    fprintf(stderr,
		    "\nMerging onto temp dir %s, available space %d MB",
		    tdirs[j], tempspace[current_tdir]) ;
	  }
	  if ( ! silent ) 
	    fprintf(stderr, "\n%d megabytes available on %s",
		    tempspace[current_tdir], tdirs[current_tdir]) ;
	  sprintf(fnames, "%s"D_C"%s.%d.",
		  tdirs[0], progname, (int)getpid()) ;
	  sprintf(fnames1, "%s"D_C"%s.%d.", tdirs[current_tdir],
		  progname, (int)getpid());
	  mergeres =
	    merge(firstsuffix, runstodo, ++lastsuffix, fnames, fnames1,
		  0, itype, deltmp) ;
	  if ( mergeres < 0 ) {
	    fprintf(stderr,
		    "\nMerge has failed. You will need to delete temp files") ;
	    goto fatal ;
	  }
	  else {
#ifndef NO_DF
	    tempspace[current_tdir] =
	      dfk(tdirs[current_tdir])/1024 - tdir_minfree[current_tdir] ;
#else
	    tempspace[current_tdir] -= mergeres / 1024 / 1024 ; 
#endif
	    tempsize[lastsuffix] = mergeres ;
	    tdirnum[lastsuffix] = current_tdir ;
	    if ( deltmp ) {
	      for ( j = firstsuffix ; j < firstsuffix + runstodo ; j++ ) {
#ifndef NO_DF
		tempspace[tdirnum[j]] =
		  dfk(tdirs[tdirnum[j]])/1024 - tdir_minfree[tdirnum[j]] ;
#else
		tempspace[tdirnum[j]] -= tempsize[j] / 1024 / 1024 ;
#endif
	      }
	    }
	    if ( ! silent ) {
	      char temp[512] ;
	      sprintf(temp, "%s%06d", fnames1, lastsuffix) ;
	      if ( sizeof(OFFSET_TYPE) == 8 )
#ifdef _WIN32
		fprintf(stderr, "\nOutput %I64d chars on %s",
			(LL_TYPE)mergeres, temp) ;
#else
		fprintf(stderr, "\nOutput %lld chars on %s",
			(LL_TYPE)mergeres, temp) ;
#endif
	      else
		fprintf(stderr, "\nOutput %d chars on %s",
			(int)mergeres, temp) ;
	    }
	  }
	}
	firstsuffix += runstodo ; 
	numruns -= runstodo - 1 ;
	runsthislevel -= runstodo ;
      } /* Done all merges this level */
      runsleft = numruns ;
      if ( ! silent ) fprintf(stderr,
	      "\nDone level %d, runs remaining: %d", levels, runsleft) ;
    } /* while (runsleft > maxruns) */
    
    if ( ! silent ) fprintf(stderr, "\nFinal merge") ;
    finished = TRUE ;
    sprintf(fnames, "%s"D_C"%s.%d.", tdirs[0], progname, (int)getpid()) ;
    if ( ! silent ) {
      if ( numruns > 1 ) 
	fprintf(stderr, "\nMerging %d runs (%d - %d) onto stdout",
		numruns, firstsuffix, numruns + firstsuffix - 1) ;
      else fprintf(stderr, "\n\"Merging\" one run (%d) onto stdout",
		   firstsuffix) ;
    }
    if ( ! no_i ) {
      /* Don't delete tempfiles on final unless delfinal */
      /* mko change to 9,999,999 possible records to merge */
      mergeres = merge(firstsuffix, numruns, 9999999, fnames, "",
		       finished, itype, delfinal) ;
      if ( mergeres < 0 ) {
	fprintf(stderr, "\nFinal merge failed") ;
	goto fatal ;
      }
    }
  } /* (not no_merge) */

  fclose(stdout) ; /* and let ixf finish if it's running */

  exit(0) ;
  
 fatal:

  fprintf(stderr, "\nFatal error") ;
  printf("%s aborting\n", argv[0]) ; /* To tell ixf_ex */
  icleanup(0) ;			/* Doesn't return */

 badargs:
  fprintf(stderr, "%s\n%s\n", Usage, helpmess) ;
  exit(1) ;

}

/*****************************************************************************
  Initrun() - initialise for an output run (uses globals)
*****************************************************************************/

void initrun(STRINGS *sorttab)
{
  sorttab->n = sorttab->used = 0 ;
  recnum = savrecnum ;
  /******  recs_to_ */
  recs_done = savrecs_done ;
  incharsthisrun = 0 ;
  recslastrun = recsthisrun ;
  recsthisrun = 0 ;
}

/******************************************************************************

  delete_temp_files

******************************************************************************/

void delete_temp_files(void)
{
  char command[512] ;
  return ;  
  if ( ! made_temp_files || trial ) return ;
  sprintf(command, "rm -f %s* %s*", fnames, fnames1) ;
  if ( ! silent ) {
    fprintf(stderr, "\nDeleting temporary files") ;
    fprintf(stderr, "\nAbout to do %s", command) ;
  }
  system(command) ;
}

/******************************************************************************

  icleanup()

******************************************************************************/

void icleanup(int code)
{
  if ( made_temp_files ) delete_temp_files() ;
  exit(0) ;
}

/******************************************************************************

  estimate()

******************************************************************************/

#ifndef NO_TRIAL

int estimate(void)
{
  int rnsize, rnsleft, estrns, lvls = 0, maxmgspace = 0, mgsize ;
  int ii ;

  estrns = (frec - srec + 1)/savrecs_done * numruns ;
  fprintf(stderr, 
	  "\nEstimating storage requirements with memory %d MB for runfiles\n\
and merge output", memavail/(1024 * 1024)) ;
  fprintf(stderr,
	  "\nEstimated %d level 0 runfiles of about %d K = %d K total",
	  estrns, (int)(meanoutchars/1024),
	  (int)(meanoutchars / 1024 * estrns)) ;
  
  rnsleft = estrns ;
  lvls = 0 ;
  rnsize = meanoutchars ; 
  maxmgspace = 0 ;
  while (rnsleft > maxruns) {
    lvls++ ;
    if ( lige(rnsleft, maxruns) < maxruns ) {
      /* Find least m s.t. lige(rnsleft, m) <= maxruns */
      for ( ii = maxruns ; ii > 0 ; ii-- )
	if ( lige(rnsleft, ii) > maxruns ) break ;
      mgsize = ii + 1 ;
    }
    else mgsize = maxruns ;
    rnsize = mgsize * rnsize ;
    if ( rnsize > maxmgspace ) maxmgspace = rnsize ;
    fprintf(stderr,
	    "\nLevel %d, runs on entry %d, mergesize %d, output runsize %d,\n\
output runs %d",
	    lvls, rnsleft, mgsize, rnsize,
	    lige(rnsleft, mgsize)) ;
    rnsleft = lige(rnsleft, mgsize) ;
  }
  fprintf(stderr, "\nMax mergespace %d", maxmgspace) ;
  return 0 ;
}

#endif /* NO_TRIAL */

/******************************************************************************


******************************************************************************/

int make_run(void)
{
  /* Find 1st temp_dir with enough space */
  if ( numruns >= 1 && ! trial && ! no_i ) {
    int j ;
    /* Note meanoutchars will initially be zero */
#if ! defined(NO_SIGSTOP) && ! defined(NO_DF)
    /* Note this is pointless unless we have dfk() function */
    for ( j = 0 ;
         j < num_tdirs && tempspace[j] < meanoutchars * 3 / 2 / 1024 / 1024 ;
         j++ ) ;
    /* (3/2 arbitraryish) */
    if ( j == num_tdirs ) { /* Can't do it */
      fprintf(stderr, "\nNo more temporary file paths with free space.") ;
      fprintf(stderr, "\nStopping the process (pid=%d)", (int)getpid()) ;
      fprintf(stderr, "\nAdvise delete some files or move and link") ;
      fprintf(stderr, "\nSend CONT signal when ready") ;
      kill(getpid(), SIGSTOP) ;
      for ( j = 0 ; j < num_tdirs ; j++ ) 
	tempspace[j] = dfk(tdirs[j])/1024 - tdir_minfree[j] ;
      return make_run() ;
    }
    else {
      if ( ! silent && j != current_tdir ) {
        fprintf(stderr,
                "\nMoving to temp dir %s after record %d",
                tdirs[j], recnum - 1) ;
      }
      current_tdir = j ;
      if ( ! silent )
        fprintf(stderr, "\n%d megabytes available on %s",
                tempspace[current_tdir], tdirs[current_tdir]) ;
    }
  }
#else
    for ( j = 0 ;
         j < num_tdirs && tempspace[j] > 0 &&
	    tempspace[j] < meanoutchars * 3 / 2 / 1024 / 1024 ;
	  /* Added > 0 condition so that no warning is output if no -t arg */
         j++ ) ;
    /* (3/2 arbitraryish) */
    if ( j == num_tdirs ) { /* Can't do it */
      fprintf(stderr,
 "Running out of temporary file space. Next output run after this one may fail\n") ;
    }
    else {
      if ( ! silent && j != current_tdir ) {
        fprintf(stderr,
                "\nMoving to temp dir %s after record %d",
                tdirs[j], recnum - 1) ;
      }
      current_tdir = j ;
      if ( ! silent )
        fprintf(stderr, "%d megabytes available on %s",
                tempspace[current_tdir], tdirs[current_tdir]) ;
    }
  }
#endif /* NO_SIGSTOP etc */

  sprintf(fnames, "%s"D_C"%s.%d.%06d",
          tdirs[current_tdir], progname, (int)getpid(), suff) ;
  outrunchars = outrunfunc(&sorttab, fnames, dflg, silent,
                           no_i || trial) ;
  if ( outrunchars < 0 ) {
    fprintf(stderr, "outrun() failed, perhaps file system full") ;
    return -1 ;
  }
  suffix++ ; numruns++ ; 
  lastoutchars = outrunchars ;
  totoutchars += outrunchars ;
  meanoutchars = totoutchars / numruns ;
  if ( ! trial && ! no_i ) {
#ifndef NO_DF
    tempspace[current_tdir] =
      dfk(tdirs[current_tdir])/1024 - tdir_minfree[current_tdir] ; ;
#else
    tempspace[current_tdir] -= lastoutchars / 1024 / 1024 ;
#endif /* NO_DF */
    tempsize[suff] = lastoutchars ;
    tdirnum[suff] = current_tdir ;
#ifndef NO_LN
    if ( current_tdir != 0 ) { /* Make symbolic link */
      char temp[PATHNAME_MAX] ;
      sprintf(temp, "%s"D_C"%s.%d.%06d",
	      tdirs[0], progname, (int)getpid(), suff) ;
      if ( symlink(fnames, temp) == 0 ) {
	if ( ! silent ) fprintf(stderr, "\nCreated link %s for %s",
				temp, fnames) ;
      }
      else {
	fprintf(stderr, "\nCan't make link %s to %s",
		temp, fnames) ;
	return -1 ;
      }
    }
#endif /* NO_LN */
  }
  if ( ! trial ) made_temp_files = TRUE ;
  
  return 0 ;
}

