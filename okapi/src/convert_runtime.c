/******************************************************************************

  convert_runtime Nov 99 from
  convert_runtime_94 Apr 94 from convert_runtime_new etc
  convert_runtime_new July 93 from convert_runtime_trec_93 

  Best source for previous versions if 2.27

  Simplified by removing paragraph stuff and replacing by calls of
  make_para_rec() (type 2 parafiles only)

  Updated documentation

  Aug 2000: added explicit write fail messages

******************************************************************************/

/******************************************************************************


  Conversion of exchange format Okapi to runtime.

  Program reads an exchange format input stream consisting of a
  sequence of fields terminated by FIELD_MARK for each field of the
  database, the record being terminated by RECORD_MARK. 
  Normally FIELD_MARK will be 0x1e and RECORD_MARK 0x1d
  (as for MARC etc). Any shortage of fields is flagged and empty fields
  added to make up. Any excess of length of the respective maxima (for the
  two types of database) is simply truncated. As it goes, the program writes
  the bibfile, directory file, and when applicable the paragraph file.

  The bibfile may be split over up to 16 file systems (actually just
  pathnames, but they must be separate filesystems or it's pointless).

  Bibfile records consist, as they always have done, of

    <fixed field><record directory>[<field>]<padding>
      
  The length of <fixed field> is given by the parm db.fixed, but this is
  probably only being retained for compatibility.

  <record directory> consists of one length field for each field of the
  bib record. In non-text databases these are 2 bytes long, but for "text"
  they are 3 bytes.

  <padding> consists of enough bytes (traditionally containing '+'s but
  this is of no importance) to make the total record length a multiple of
  the parameter db.rec_mult (again, only retained for compatibility; always
  1).

  For paragraph file format see the documentation file paragraph_file,
  or make_para_rec.c

  Directory file record format is

    <offset in bibfile / rec_mult>

  where the offset is always bigendian and may be 4 or 5 bytes long.
  Note that the offset accumulates over all the bibfile volumes rather
  than restarting at zero for each one.

******************************************************************************/

#define VERSION "5.1 (BSS 2.4) 20020427"
#define CHECKREC 10000	/* Checkpoints and reports at multiples of this,
			   but see checkpoint arg */

#include "bss.h"
#include "charclass.h"

#define ODD 1
#define EVEN 2

#define MAXFCNOLEN 256
#define MAX_LIM_LENGTH 6

#ifdef HAVE_LL
#ifdef _WIN32
#define PTOTCHARS fprintf(stderr, "Total bibsize %I64d\n", totchars) 
#else
#define PTOTCHARS fprintf(stderr, "Total bibsize %lld\n", totchars) 
#endif
#else 
#define PTOTCHARS fprintf(stderr, "Total bibsize %.0f\n", (double)totchars)
#endif 


#define report \
  result = 0 ;\
  Cdb.nr = outrecs ;\
  Cdb.real_bibsize[current_vol] = chars_this_vol / Cdb.rec_mult ;\
  Cdb.maxreclen = maxreclen ;\
  if ( ! validate ) { \
    result = write_db_desc(argv[argc - 1], &Cdb) ;\
    if ( result < 0 ) {\
      fprintf(stderr, "%s: can't write database description file.\n", \
                       argv[0]) ;\
      fprintf(stderr, "Check permissions and start again.\n") ;\
      fprintf(stderr,\
       "If appending, truncate output files back to original sizes.\n") ;\
      exit(1) ;\
    }\
  } \
  if ( ! silent ) {\
    PTOTCHARS ; \
    fprintf(stderr,\
	  "%d records output this run, total %d\n", recs_this_run, outrecs) ;\
    if ( tot_parlen > 0 ) \
      fprintf(stderr, "Paragraph file size %d\n", tot_parlen) ;\
    if ( trunc_recs > 0 ) \
      fprintf(stderr, "%d truncated records longer than %d\n",\
	    trunc_recs, maxallowablereclen) ;\
    if ( finished ) {\
      if ( maxrecnum == 0 ) \
        fprintf(stderr, "Longest record is ??? with %d characters\n",\
  	    maxreclen) ;\
      else\
        fprintf(stderr, "Longest record is %d with %d characters\n",\
  	    maxrecnum, maxreclen) ;\
      fprintf(stderr, "Max paragraphs in a field is %d\n", max_paracount) ;\
      fprintf(stderr, "Max sentences in a field is %d\n", max_se_num + 1) ;\
    }\
  }

int process_record(void) ;
void statfield(int, int) ;
void statrec(int) ;

char *progname = "convert_runtime" ;

extern FILE *Err_file ;

char *convert_runtime_version = VERSION ;

struct db Cdb ;

int pluscount ;
int reclen = 0 ;
int outreclen ;
int newparas = 0 ;		/* Amount of para data written last rec */
int outrecs = 0 ;
int recs_this_run = 0 ;
int maxfdlens[MAXNUMFLDS] ;
int maxallowablereclen ;

int maxreclen = 0 ;			/* Max achieved */
int maxrecnum = 0 ;
int max_paracount = 0 ;
int max_se_num = 0 ;

int maxtoks ;

#ifdef HAVE_LL
LL_TYPE totchars = 0 ;		/* Total number of characters output */
LL_TYPE real_totchars = 0 ;	/* Sum of existing bibfile sizes at start */
#else
double totchars = 0 ;		/* Total number of characters output */
double real_totchars = 0 ;	/* Sum of existing bibfile sizes at start */
#endif
#ifdef HAVE_LL
LL_TYPE chars_this_vol = 0 ;
#else
int chars_this_vol = 0 ;
#endif

BOOL finished = FALSE ;
BOOL do_par = TRUE ;		/* By default, if istext then do_par is true */
BOOL truncated ;		/* Set TRUE by process_record if record
				   truncated */
int trunc_count ;		/* Number of chars by which rec truncated */
int trunc_recs = 0 ;
int tot_parlen = 0 ;
int hl_tot_parlen ;
char *recbuf ;			/* malloc'd to MAXRECLEN... */

char fcno[MAXFCNOLEN+1] ;	/* Record control number or primary key,
				   for error messages. Also used for
				   record-specific limit value. */

BOOL checkpointing = TRUE ;	/* May be overridden by the checkpoint arg */
int checkrec = CHECKREC ;
BOOL validate = FALSE ;
FILE *nullf ;

#if defined(HAVE_LL) && ! defined(LARGEFILES)
/* Want 8-byte value if making "large" DB */
LL_TYPE rec_addr ;		/* Record's file address */
LL_TYPE out_rec_addr ;	/* In case reversed for Intel machines */
#else
OFFSET_TYPE rec_addr ;	/* Record's file address */
OFFSET_TYPE out_rec_addr ;	/* In case reversed for Intel machines */
#endif
int ooffset ;
int oloffset ;

int par_addr ;			/* Offset of record in paragraph file */
int out_par_addr ;

FILE *runp, *dirp, *parp, *pardirp, *limp, *fopen();
FILE *bibfiles[MAXBIBVOLS] ;		/* For multivolume */
OFFSET_TYPE maxbibsize[MAXBIBVOLS] ;	/* June 98 */
int maxrecs = 1000000000 ;
int skipnum = 0 ;
BOOL istext = FALSE ;
int fixedlimit = 0 ;
BOOL treclimits = FALSE ;	/* If true uses certain predefined limit bits
			   for various record length ranges, etc */
BOOL halfcollection = FALSE ;	/* If true sets limit 1 or 2 acc as odd/even */

BOOL phoney_fcno = FALSE ;
BOOL silent = FALSE ;
char record_mark = RECORD_MARK ;
char field_mark = FIELD_MARK ;

int result ;

int
main(int argc, char *argv[])
{
  char filenames[512] ;
  int current_vol = 0 ;
  BOOL appending = FALSE ;
  int i ;
  char *s ;
  int c ;

  Err_file = stderr ;
  if (argc < 2) goto arg_error ;

  s = getenv("BSS_PARMPATH") ;
  if ( s != NULL ) Control_dir = s ;
  else Control_dir = CONTROL_DIR ;
  qaddslash(Control_dir) ;

  if ( strcmp(argv[argc - 1], "-h") == 0 ||
       strcmp(argv[argc - 1], "-help") == 0 )
    goto arg_error ;

  if ( strcmp(argv[argc - 1], "-version") == 0 ) {
    fprintf(stderr, "Version %s\n", convert_runtime_version) ;
    exit(0) ;
  }
  for ( i = 1 ; i < argc - 1 ; i++ ) {
    if ( strcmp(argv[i], "-c") == 0 ) Control_dir = argv[++i] ;
    else if ( strcmp(argv[i], "-a") == 0 )
      appending = TRUE ;
    else if ( strcmp(argv[i], "-nopar") == 0 )
      do_par = FALSE ;
    else if ( strcmp(argv[i], "-num") == 0 ) 
      maxrecs = atoi(argv[++i]) ;
    else if ( strcmp(argv[i], "-skip") == 0 ) 
      skipnum = atoi(argv[++i]) ;
    else if ( strcmp(argv[i], "-checkpoint") == 0 ) 
      checkrec = atoi(argv[++i]) ;
    else if ( strcmp(argv[i], "-treclimits") == 0 ) {
      treclimits = TRUE ;
      halfcollection = TRUE ;
    }
    else if ( strcmp(argv[i], "-halfcollection") == 0 )
      halfcollection = TRUE ;
    else if ( strcmp(argv[i], "-fixedlimit") == 0 ) 
      fixedlimit |= atoi(argv[++i]) ;
    else if ( strcmp(argv[i], "-phoney_fcno") == 0 ) 
      phoney_fcno = 1 ;
    else if ( strcmp(argv[i], "-rm") == 0 ) 
      record_mark = *argv[++i] ;
    else if ( strcmp(argv[i], "-fm") == 0 ) 
      field_mark = *argv[++i] ;
    else if ( strcmp(argv[i], "-validate") == 0 ) validate = TRUE ;
    else if ( strcmp(argv[i], "-silent") == 0 ) silent = TRUE ;
    else if ( strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-h") == 0 ) 
      goto arg_error ;
    else if ( strcmp(argv[i], "-version") == 0 ) {
      fprintf(stderr, "Version %s\n", convert_runtime_version) ;
      exit(0) ;
    }
    else {
      fprintf(stderr, "Unknown arg %s\n", argv[i]) ;
      goto arg_error ;
    }
  }
  if ( validate ) appending = 0 ;
  if ( validate ) nullf = fopen("/dev/null", "w") ;
  if ( ! validate ) {
    int ch = getchar() ;
    if ( ch != 0 && ch != EOF ) ungetc(ch, stdin) ;
  }
  if ( read_db_desc(argv[argc - 1], &Cdb) != 1 ) {
    fprintf(stderr, "Error reading parameter file %s\n", argv[argc-1]) ;
    exit(1) ;
  }
  if ( (Cdb.i_db_type & DB_LARGE_TYPE) && sizeof(rec_addr) < 5 ) {
    fprintf(stderr, "Can't make 'large'-type DB, no LL_TYPE data type\n") ;
    exit(1) ;
  }
  for ( i = 0 ; i <= Cdb.lastbibvol ; i++ )
    maxbibsize[i] = 1024 * 1024 * (OFFSET_TYPE)Cdb.bibsize[i] ;
  if ( Cdb.i_db_type & DB_TEXT_TYPE ) {
    istext = TRUE ;
    recbuf = (char *)malloc(MAXRECLEN_TEXT) ;
    maxallowablereclen = MAXRECLEN_TEXT - 512 ;
  }
  else {
    istext = FALSE ;
    do_par = FALSE ;
    recbuf = (char *)malloc(MAXRECLEN) ;
    maxallowablereclen = MAXRECLEN - 512 ;
  }
  if ( recbuf == NULL ) {
    fprintf(stderr, "%s: out of memory allocating recbuf\n", progname) ;
    exit(1) ;
  }

  maxreclen = 0 ;
  if ( appending ) maxreclen = Cdb.maxreclen ;
  /* Open output runtime file(s) */
  if ( ! appending ) {
    for ( i = 0 ; i <= Cdb.lastbibvol ; i++ ) {
      sprintf(filenames, "%s%s", Cdb.bib_dir[i], Cdb.bib_basename[i]) ;
      if ( validate ) bibfiles[i] = nullf ;
      else bibfiles[i] = fopen(filenames, "wb") ;
      if ( bibfiles[i] == NULL ) {
	fprintf(stderr, "%s: can't open output runtime file %s\n",
		progname, filenames) ;
	exit(1) ;
      }
    }
    runp = bibfiles[0] ;
  }
  else {			/* Appending */
    /* Dangerous. Shouldn't use existing parm, use ftell() */
    for ( i = 0 ; i <= Cdb.lastbibvol && Cdb.real_bibsize[i] > 0 ; i++ )
      totchars += Cdb.real_bibsize[i] ;
    i-- ;
    if ( Cdb.real_bibsize[i] >= maxbibsize[i] ) {
      if ( i == Cdb.lastbibvol ) {
	fprintf(stderr, "%s: no room to append\n", progname) ;
	exit(1) ;
      }
      current_vol = i + 1 ;
    }
    else current_vol = i ;
    totchars *= Cdb.rec_mult ;

    for ( i = 0, real_totchars = 0 ; i < current_vol ; i++ ) {
      sprintf(filenames, "%s%s", Cdb.bib_dir[i], Cdb.bib_basename[i]) ;
      real_totchars += fsize(filenames) ;
    }
    for ( i = current_vol ; i <= Cdb.lastbibvol ; i++ ) {
      sprintf(filenames, "%s%s", Cdb.bib_dir[i], Cdb.bib_basename[i]) ;
      if ( Cdb.real_bibsize[i] > 0 ) /* Current vol only, or none */
	bibfiles[i] = fopen(filenames, "ab") ;
      else
	bibfiles[i] = fopen(filenames, "wb") ;
      if ( bibfiles[i] == NULL ) {
	fprintf(stderr, "Can't open output runtime file %s\n",
		filenames) ;
	exit(1) ;
      }
      /* (Aug 00) Using ofsize() (i.e. fstat) instead of ftell
	 as Windows doesn't have ftell for large files */
/*
#ifdef LARGEFILES
      real_totchars += ftello(bibfiles[i]) ;
#else
      real_totchars += ftell(bibfiles[i]) ;
#endif
*/
      real_totchars += ofsize(fileno(bibfiles[i])) ;
    }
    if ( real_totchars != totchars ) {
#ifdef HAVE_LL
      fprintf(stderr,
#ifdef _WIN32
"Total bibsize in parameter file (%I64d) disagrees with real total size (%I64d)\n",
#else
"Total bibsize in parameter file (%lld) disagrees with real total size (%lld)\n",
#endif
	      totchars, real_totchars) ;
#else
      fprintf(stderr,
"Total bibsize in parameter file (%.0f) disagrees with real total size (%.0f)\n",
	      totchars, real_totchars) ;
#endif /* HAVE_LL */
      fprintf(stderr, "Truncate last file or correct the parameter.\n") ;
      exit(1) ;
    }
    chars_this_vol = Cdb.real_bibsize[current_vol] * Cdb.rec_mult ;
    runp = bibfiles[current_vol] ;
  } /* (appending) */

  /* Open output directory file. It goes in 1st volume.
     Modified Apr 02 for 1-field files, to make .bibdrl giving doclength
     as well as offset. */
  if ( Cdb.nf == 1 && ! Cdb.no_drl ) 
    sprintf(filenames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".bibdrl") ;
  else { /* (nf > 1 or no_drl) */
    sprintf(filenames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".bibdir") ;
  }
  if ( validate ) dirp = nullf ;
  else {
    if ( ! appending ) dirp = fopen(filenames, "wb") ;
    else dirp = fopen(filenames, "ab") ;
  }
  if ( dirp == NULL ) {
    fprintf(stderr, "%s: can't open output directory file %s\n",
	    progname, filenames) ;
    exit(1) ;
  }
  /* ooffset is offset of address in out_rec_addr  */
  ooffset = sizeof(out_rec_addr) - Cdb.dir_recsize ;
  oloffset = sizeof(outreclen) - Cdb.dir_lensize ; /* Only if nf=1 and has
						      .bibdrl file */
  /* Open output paragraph file */
  /* It goes in 1st volume */
  if ( istext && do_par ) {
    sprintf(filenames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".par") ;
    if ( validate ) parp = nullf ;
    else {
      if ( ! appending ) parp = fopen(filenames, "wb") ; 
      else parp = fopen(filenames, "ab") ;
    }
    if ( parp == NULL ) {
      fprintf(stderr, "%s: can't open output paragraph file %s\n",
	      progname, filenames) ;
      exit(1) ;
    }
    /* See note above re ftell() */
/*
#ifdef LARGEFILES
    tot_parlen = ftello(parp) ;
#else
    tot_parlen = ftell(parp) ;	
#endif
*/
    tot_parlen = ofsize(fileno(parp)) ;

    sprintf(filenames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".pardir") ;
    if ( validate ) pardirp = nullf ;
    else {
      if ( ! appending ) pardirp = fopen(filenames, "wb") ; 
      else pardirp = fopen(filenames, "ab") ;
    }
    if ( pardirp == NULL ) {
      fprintf(stderr, "%s: can't open output paragraph directory file %s\n",
	      progname, filenames) ;
      exit(1) ;
    }
    maxtoks = TEXTPOS_MAX_T ;	/* Max tokens in a sentence */
  }

  /* Open output limits file */
  /* It goes in 1st volume */
  if ( Cdb.has_lims ) {
    sprintf(filenames, "%s%s%s", Cdb.bib_dir[0], Cdb.name, ".lims") ;
    if ( validate ) limp = nullf ;
    else {
      if ( ! appending ) limp = fopen(filenames, "wb") ;
      else limp = fopen(filenames, "ab") ;
    }
    if ( limp == NULL ) {
      fprintf(stderr, "%s: can't open output limits file %s\n",
	      progname, filenames) ;
      exit(1) ;
    }
  }

  if ( appending ) {		/* Find start rec_addr etc */
    /* rec_addr = (unsigned int)(totchars / (LL_TYPE)Cdb.rec_mult) ;*/
    /* Above only right if no LL_TYPE; will give rubbish if rec_addr >= 4GB
       and wd give error if no LL_TYPE */
#ifdef HAVE_LL       
    /* rec_addr OFFSET_TYPE (8-byte) and totchars LL_TYPE */
    rec_addr = totchars / (LL_TYPE)Cdb.rec_mult ;
#else
    /* rec_addr OFFSET_TYPE (4-bytes) and totchars double */
    rec_addr = (unsigned)(totchars / Cdb.rec_mult) ;
#endif
#if ! HILO
    if ( sizeof(rec_addr) == 8 ) out_rec_addr = reverse_64(rec_addr) ;
    else out_rec_addr = reverse(rec_addr) ; /* sizeof == 4 - should cast to
						OFFSET_TYPE or LL_TYPE */
#else
    out_rec_addr = rec_addr ;
#endif /* HILO */
    outrecs = Cdb.nr ;
  } /* (appending) */

  /* Skipping if called for: there's an argument for doing this before
     opening files etc */
  if ( skipnum ) {
    if ( ! silent ) fprintf(stderr, "Skipping %d record(s)..", skipnum) ;
    for ( i = 0 ; i < skipnum ; i++ ) {
      while ( (c = getchar()) != record_mark ) {
	if ( c == EOF && feof(stdin) ) {
	  finished = TRUE ;
	  break ;
	}
      }
      if ( finished ) break ;
    }
    if ( ! silent ) {
      if ( finished )
	fprintf(stderr, "\nReached EOF after skipping %d of %d record(s)\n",
		i, skipnum) ;
      else fprintf(stderr, "\nSkipped %d records\n", skipnum) ;
    }
  }

  while ( outrecs < maxrecs && ! finished ) { 
    
    /* Next line used to have both sides / by rec_mult,
       is this OK? */
    if ( chars_this_vol >= maxbibsize[current_vol] ) {
      Cdb.real_bibsize[current_vol] = chars_this_vol / Cdb.rec_mult ;
      if ( current_vol >= Cdb.lastbibvol ) {
	fprintf(stderr, "File won't fit. Stopped after %d recs\n", outrecs) ;
	break ;
      }
      current_vol++ ;
      if ( ! silent )
#ifdef HAVE_LL
	fprintf(stderr,
#ifdef _WIN32
    "Moved to vol %d after total %I64d chars (%I64d this vol) and rec %d\n",
#else
	"Moved to vol %d after total %lld chars (%lld this vol) and rec %d\n",
#endif
		current_vol, totchars, chars_this_vol, outrecs) ;
#else
	fprintf(stderr,
	"Moved to vol %d after total %f chars (%d this vol) and rec %d\n",
		current_vol, totchars, chars_this_vol, outrecs) ;
#endif
      runp = bibfiles[current_vol] ;
      chars_this_vol = 0 ;
    }

    reclen = process_record() ;
    if ( truncated ) {
      if ( ! silent ) fprintf(stderr, "%s truncated  (length %d)\n",
	      fcno, reclen + trunc_count) ;
      trunc_recs++ ;
    }
    if ( reclen == EOF ) {
      if ( ! silent )
	fprintf(stderr, "Reached end of input, last rec %s\n", fcno);
      break ;
    }
    /* Don't know whether this can occur? */
    if ( reclen < 0 ) {
      fprintf(stderr, "Stopped processing, error, last rec %s\n", fcno);
      break ;
    }
    
    totchars += reclen ;
    chars_this_vol += reclen ;
    statrec(reclen) ;
#if ! HILO
    outreclen = reverse(reclen) ;
#else
    outreclen = reclen ;
#endif
    outrecs++ ;
    recs_this_run++ ;

    /* out_rec_addr is always stored HILO */
    if ( fwrite((char *)&out_rec_addr + ooffset, (size_t)Cdb.dir_recsize,
		(size_t)1, dirp) != 1 ) {
      fprintf(stderr, "%s: directory file write fail, last rec %s\n",
	      progname, fcno) ;
      break ;
    }
    if ( Cdb.nf == 1 && ! Cdb.no_drl ) {
      if ( fwrite((char *)&outreclen + oloffset, (size_t)Cdb.dir_lensize,
		  (size_t)1, dirp) != 1 ) {
	fprintf(stderr, "%s: directory file write fail, last rec %s\n",
		progname, fcno) ;
	break ;
      }
    }
    
    rec_addr += reclen / Cdb.rec_mult ; /*Increment by length of last record*/
    /* Change */
    if ( sizeof(rec_addr) == 4 && rec_addr > (OFFSET_TYPE)0x7fffffff ) {
      fprintf(stderr,
	      "Address overflow after %s, start again with larger rec_mult?\n",
	      fcno) ;
      break ;
    }
    /* else */
#if ! HILO
    if ( sizeof(rec_addr) == 8 ) out_rec_addr = reverse_64(rec_addr) ;
    else /* 4 */ out_rec_addr = reverse(rec_addr) ;
#else
    out_rec_addr = rec_addr ;
#endif /* HILO */

    if ( checkrec && (recs_this_run % checkrec) == 0 ) {
      if ( ! silent ) fprintf(stderr, "Rec %d fcno %s\n", outrecs, fcno) ;
      if ( checkpointing ) {
	if ( ! silent )
	  fprintf(stderr, "%s: checkpointing at %d\n", progname, outrecs) ;
	fflush(runp) ;
	fflush(dirp) ;
      
	if ( Cdb.has_lims ) fflush(limp) ;
	if ( istext && do_par ) {
	  fflush(parp) ;
	  fflush(pardirp) ;
	}
	report ;
      }
    }
  } /* (while 1) */

  if ( ! validate ) {
    fclose(dirp) ;
    if ( istext && do_par ) {
      fclose(parp) ;
      fclose(pardirp) ;
    }
    for ( i = 0 ; i <= Cdb.lastbibvol ; i++ ) fclose(bibfiles[i]) ;
  }
  
  silent = FALSE ;		/* Always reports at end */
  if ( recs_this_run > 0 ) report ;

  exit(0) ;

 arg_error:

  fprintf(stderr,
"Usage: %s [-c <ctrl directory>] [-a] [-num <maxrecs>]\n\
[-treclimits] [-fixedlimit <fixedlim>] [-halfcollection] [-version] [-help]\n\
[-rm <record terminator character>] [-fm <field terminator character>]\n\
[-phoney_fcno] [-skip <skipnum>] [-checkpoint <interval>] [-nopar]\n\
[-silent] <database name> < <input file>\n", *argv) ;
fprintf(stderr, "Database name (parameter file name) must come last\n") ;
fprintf(stderr,
"Database parameter file will be read from the arg following -c,\n\
the environment variable BSS_PARMPATH, or the predefined CONTROL_DIR,\n\
in that order.\n") ;
fprintf(stderr,
"-a causes database to be appended to an existing one of the same name.\n") ;
fprintf(stderr,
"-num <maxrecs> limits total database size to <maxrecs> records.\n") ;
fprintf(stderr,
"-skip <num> causes input records to be skipped before processing starts.\n");
fprintf(stderr,
"-checkpoint <num> causes files to be flushed and stats displayed after\n\
every <num> output records. Default 10000, <num>=0 prevents checkpointing.\n");
fprintf(stderr,
"-treclimits inserts predefined doclength limit bits (see the code).\n") ;
fprintf(stderr,
"-fixedlimit ORs the following arg into the limits field of each record.\n") ;
fprintf(stderr,
"-halfcollection sets the '1' or '2' bit in the limits field\n\
according as the record number is odd or even.\n") ;
fprintf(stderr,
"-phoney_fcno puts a dummy entry in the second field of each paragraph\n\
record (use this arg if field 1 may have more than 1 'word' in it).\n") ;
fprintf(stderr, 
"-nopar prevents paragraph file being made (only applies if text database)\n");
  exit(1) ;
}

/******************************************************************************

  process_record()

  Reads exchange format from stdin, writes fixed field if any, (usually
  none now (Jan 98)), directory fields, and the text of each field, all to the
  appropriate place in global recbuf.

  It skips white space and any newlines at the start of a record.
  Record directory contains 2 (nontext db) or 3 (text db)
  bytes for each field giving length of field in HILO (NB even if LOHI
  processor).

******************************************************************************/


int process_record(void)
/* Reads stdin, writes runp
   Skips white space and nl.

   The directory follows, 2 bytes in hi-lo for the length of each of the
   variable fields, or 3 bytes if text.
   Directory length is therefore Db.nf * 2 or 3, and
   variable fields start at record + fixed + Db.nf * 2 or 3.

*/
{
 
  char *dp = recbuf + Cdb.fixed ; /* Writes directory fields */
  /* char *ffp = recbuf ; unused   Writes the fixed field */
  char *bufp ;			/* Writes other fields */

  char *p ;			/* Points to fcno etc */

  int ch ;
  int i ;
  int fdlen = 0 ;
  int outfdlen ;
  int reclen = 0 ;
  int fdcount = 0 ;
  int dirsize = Cdb.fddir_recsize ; /* 2 or 3 acc to db type */
  u_short limits = 0 ;
  char lim_buf[MAX_LIM_LENGTH] ;
  char *lp ;
  int limlen = 0 ;
  int se_num ;
  char *fdstarts[MAXNUMFIELDS] ;
  int fdlens[MAXNUMFIELDS] ;
  int extrachars = 0 ;
  int fclen = 0 ;
  int res ;

  truncated = FALSE ; trunc_count = 0 ;

  bufp = recbuf + Cdb.fixed + (Cdb.nf) * Cdb.fddir_recsize ;
  reclen = Cdb.fixed + (Cdb.nf) * Cdb.fddir_recsize ;/* length of header */

  /* Skip to beginning of record in case extra space between */
  /* Get rid of white space between recs */
  while ( isspace(ch = getchar()) ) ;
  if ( feof(stdin) ) return EOF ; /* Normal EOF return, after end of record */
  ungetc(ch, stdin) ;

  p = fcno ;
  lp = lim_buf ;
  fdstarts[fdcount] = bufp ;
  while ( 1 ) {
    if ( (ch = getchar()) == EOF && feof(stdin) ) break ;
    if ( ch == record_mark ) break ;
    if ( ch == field_mark ) {
      /* Remove any white space at end except a single newline */
      while ( fdlen > 0 && tok_space(*(bufp - 1)) ) {
	bufp-- ; fdlen-- ; reclen-- ;
      }
      if ( fdlen > 0 && istext ) {
	*bufp++ = '\n' ; fdlen++ ; reclen++ ;
      }
      fdlens[fdcount] = fdlen ;
      fdcount++ ;
      if ( fdcount == 1 ) *p = '\0' ; /* String may be read as FCNO */
      if ( Cdb.has_lims ) {
	if ( fdcount == Cdb.nf ) { /* Set up "fixed" limit values. This is
				      independent of (and takes precedence
				      over) any limit which may be given in
				      this record's input. */
	  if ( fixedlimit ) limits |= fixedlimit ;
	  if ( treclimits ) {
	    if ( reclen < 0x200 ) limits |= 128 ;
	    else if ( reclen < 0x800 ) limits |= 256 ;
	    else if ( reclen < 0x2000 ) limits |= 512 ;
	    else if ( reclen < 0x8000 ) limits |= 1024 ;
	    else if ( reclen < 0x20000 ) limits |= 2048 ;
	    else if ( reclen < 0x80000 ) limits |= 4096 ;
	    else limits |= 8192 ;
	  }
	  if ( halfcollection ) {
	    if ( outrecs % 2 ) limits |= ODD ;
	    else limits |= EVEN ;
	  }
	} /* fdcount == Cdb.nf */
	if ( fdcount == Cdb.nf + 1 ) {
	  *lp = '\0' ;
	  limits |= atoi(lim_buf) ;
	}
#if ! HILO
	limits = reverse_16(limits) ;
#endif
	res = fwrite((char *)&limits, 2, 1, limp) ;
	if ( res < 1 ) {
	  fprintf(stderr, "%s: lims file write error, last rec %s\n",
		  progname, fcno) ;
	  return -4 ;
	}
      }	/* (has_lims) */
      if ( fdcount <= Cdb.nf ) {
	statfield(fdcount, fdlen) ;
	if ( Cdb.nf > 1 || Cdb.no_drl ) {	/* Apr 02: for 1-field dbs */
#if ! HILO
	  outfdlen = reverse(fdlen) ;
#else
	  outfdlen = fdlen ;
#endif
	  if ( dirsize == 3 )  memcpy(dp, (char *)&outfdlen + 1, dirsize) ;
	  else memcpy(dp, (char *)&outfdlen + 2, dirsize) ;
	  dp += dirsize ;
	}
      }
      fdlen = 0 ;
      if ( (!Cdb.has_lims && fdcount >= Cdb.nf) || (fdcount >= Cdb.nf + 1) ) {
	/* Then there shouldn't be any more nonwhite chars before
	   record_mark. Note that an apparent extra field would be tolerated
	   and read as a limit value if has_lims is true. */
	while ( (ch = getchar()) != record_mark ) {
	  if ( ! isspace(ch) ) extrachars++ ;
	}
	ungetc(ch, stdin) ;
	if ( extrachars ) {
	  if ( ! silent )
	    fprintf(stderr, "%d chars after last field in %s, skipping rec\n",
		    extrachars, fcno) ;
	}
	if ( feof(stdin) ) reclen = -1 ;
      }
      /* Get rid of any leading white space at start of next field */
      if ( fdcount < Cdb.nf ) {
	while ( isspace(ch = getchar()) ) ;
	ungetc(ch , stdin) ;
	fdstarts[fdcount] = bufp ;
      }
    } /* (field mark read) */
    else {	/* Ordinary character read */
      if ( reclen > maxallowablereclen ) {
	trunc_count++ ;
	continue ;
      }
      if ( fdcount < Cdb.nf ) {
	*bufp++ = ch ;
	reclen++ ;
	fdlen++ ;
      }
      /* Field 1 is copied to FCNO for diagnostics. If there is only one field
	 or field 1 is a normal field then it's likely to be truncated
	 in FCNO. If there is a nf+1th field this contains a limit
	 mask. */
      if ( fdcount == 0 && Cdb.nf > 1 && fclen < MAXFCNOLEN ) {
	*p++ = ch ;
	fclen++ ;
      }
      if ( Cdb.has_lims && fdcount == Cdb.nf && limlen < MAX_LIM_LENGTH ) {
	*lp++ = ch ;
	limlen++ ;
      }
    }
  }				/* End of loop for each character */

  if ( reclen <= 0 ) return reclen ;

  /* write out record */
  bufp = recbuf ;
  res = fwrite(bufp, reclen, 1, runp) ;
  if ( res < 1 ) {
    fprintf(stderr, "%s: record write fail, last rec %s\n", progname, fcno) ;
    return -3 ;
  }
  bufp += reclen ;
  /* make multiple of rec_mult bytes */
  if ( (i = (reclen % Cdb.rec_mult)) ) i = Cdb.rec_mult - i ;
  while ( i-- ) {
    if ( putc('+', runp) == EOF ) {
      fprintf(stderr, "%s: record write fail, last rec %s\n", progname, fcno) ;
      return -3 ;
    }
    reclen++ ;
    pluscount++ ; 
  }

  /* Do paragraphs (now Nov 99 parafile_type always 2 */
  /* Temp paragraph file in "plain" */
  if ( istext && do_par ) {
    int parlen ;
#if HILO
    hl_tot_parlen = tot_parlen ;
#else
    hl_tot_parlen = reverse(tot_parlen) ;
#endif
    for ( i = 0, parlen = 0 ; i < Cdb.nf ; i++ ) {
      se_num = 0 ;
      parlen += make_para_rec(i + 1, fdstarts[i], fdlens[i],
			      fdstarts[i] - recbuf, parp, maxtoks) ;
    }
    fwrite((char *)&hl_tot_parlen, sizeof(u_int), 1, pardirp) ;
    if ( ferror(parp) || ferror(pardirp) ) {
      fprintf(stderr, "%s: paragraph or pardir write fail, last rec %s\n",
	      progname, fcno) ;
      return -3 ;
    }
    tot_parlen += parlen ;
  }
  return reclen ;
}

void statfield(int fnum, int l)
     /* maintain and print length of longest fields */
{
  if (l <= maxfdlens[fnum]) return ;
  maxfdlens[fnum] = l ;
  if ( ! silent )
    fprintf(stderr, "Fd %d, l %d, fcno %s\n", fnum, l, fcno) ;
  return ;
}

void statrec(int l)
{
  if (l <= maxreclen) return ;
  maxreclen = l ;
  maxrecnum = outrecs + 1 ;
  if ( ! silent )
    fprintf(stderr, "%d: %d\n", maxrecnum, maxreclen) ; 
  return ;
} 


