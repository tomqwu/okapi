#ifndef _defines_h
#define _defines_h

/******************************************************************************

  defines.h  SW Apr 92

  Low-level Okapi defines etc for both interactive and BSS systems

  Mostly taken from stdeqs.h

******************************************************************************/
#include "site.h"

#include MACHINE

#ifdef LARGEFILES
#define HAVE_LL
#endif

#ifdef NO_MMAP
#define NO_ATOMIC_MAP
#define NO_MLOCK
#undef HAVE_MADVISE
#undef HAVE_MMAP_ZERO
#endif

#if NEEDED
#ifndef _SSIZE_T		/* Not in BSD */
#ifndef ssize_t
#define	_SSIZE_T
typedef int	ssize_t;	/* used by functions which return a 
				   count of bytes or an error indication */
#endif
#endif
#endif

#include <stdio.h> 
#include <fcntl.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1
  
#define TIMER_LOAD_VAL 1000000	/* Start value for timers, doesn't matter so
				   long as it's pretty large */

/* MEM_BREAK_1 maybe no longer used Dec 98 */
#ifndef MEM_BREAK_1
#define MEM_BREAK_1 4		/* Pagesize * this. If less, malloc is used,
				   a zero memory mapping. Now in site.h */
#endif

#define MAXLINE1 16384		/* Max line length for various readline
				   calls */
#define PATHNAME_MAX 1024	/* Amount to allow for pathnames */
#define NNAME_MAX 256		/* Max length of various names: stem
				   functions, gsls, attribute name strings */

#define BLOCKSIZE 512
#define KSIZE (2 * BLOCKSIZE)

#define MAXNUMFLDS 30		/* Not sure why this isn't 31 or 32 */
#define MAXNUMFIELDS MAXNUMFLDS	/* I always forget it */
#define MAXNUMFIELDS_TEXT 4	/* For "text" databases */
#define BSS_MAXIX 16		/* Indexes per database, inc from 3 Jan 93 */

#define MAXGSLS 256		/* (March 99) now array is pointers, structs
				   allocated as required.
				   BSS_MAXIX (June 97), was 3 * BSS_MAXIX
				   since Apr 92, before that 3 */
#define MAXRECLEN 65535		/* Non-text database doc max length */
#define MAX_TEXTLEN 65536	/* Max length of e.g. a "sentence",
				   for extract() etc */
#define MAX_TEXT MAX_TEXTLEN
#define MAXTEXTRECLEN 16 * 1024 * 1024 /* Text database max doclength */
				       
#define MAXRECLEN_TEXT MAXTEXTRECLEN /* Synonym */
#define MAXLIMS 16		/* Max number of limit bits for a database */
#define LIMIT_MASK_SIZE 2	/* Bytes */

#define GOODREADSIZE (16 * BLOCKSIZE)
#define CHUNKSIZE (16 * BLOCKSIZE)
#define SCHUNKSIZE CHUNKSIZE

#define MAXEXPSTGLEN 12 + 256 * 4 /* Max extended index pstg length (Nov 91)
				     (allows 256 positional fields) */
#define MAXTERMSPERFIELD 2048	/* Max index terms per field (for
				   source recording during extraction).
				   Inc from 1024 to 2048 exp Apr 91
				   Inc from 128 to 1024 Oct 89. */
/* Used for dimensioning etc in bss_combine.c */
#define MAXMERGESTREAMS 1024	/* But you can't have that many unless in mem
				   or using Willett (mode=quick) */
#define MAXMERGE MAXMERGESTREAMS + 1
#define MAXMEMMERGE MAXMERGE	/* But might be higher  */

#define DEFAULT_PARAFILE_TYPE 2	/* Oct 99, for make_parafile  */
#define Delta3 0.001		/* Apr 99 */

#define TAB 0x9

/* Nov 97: don't know that 1st two used, rest are new except TEXT */
#define DB_CAT_TYPE 1		/* These new Apr 94, for speed */
#define DB_AI_TYPE 2
/* New Dec 97: default is not text, not free format, not external */
#define DB_TEXT_TYPE 4		/* Can be long, and textpos type */
#define DB_EXTERNAL_TYPE 16	/* Nov 97: Db made of raw data */
#define DB_FREE_FORMAT_TYPE 32	/* Fields not specified */
#define DB_LARGE_TYPE 64	/* Bibfile vols may be large,
				   bibdir records are 5-byte */
#define DB_PROFILE_TYPE 128	/* Apr 02 */

/* #define DB_INDIRECT_TYPE 8	   Obsolete (< 1997), all dbs are like this
                                   Has indirect record addressing,
				   separate .lims file etc */

#define DB_OPEN_NOSEARCH 0
#define DB_OPEN_SEARCH 1
#define DB_OPEN_BM 2
#define DB_OPEN_LIMIT 4
#define DB_OPEN_SHOW 8		
#define DB_OPEN_PARAS 16	/* If true reads .par into Db.paraptrs */

#define FD_UNCHECKED 0
#define FD_CANT_OPEN -1

#define WEIGHT_DIST_GRAD 20	/* Was a local define somewhere (Nov 95) */

#define INT_SORT_THRESH 2	/* Threshold for insertion sorting in
				   gqsort_int(). 2 is easily best. Zero
				   makes infinite loop. */

/*****************************************************************************/

#define RECORD_MARK 0x1d	/* ('^]') Added Apr 94, replaces old HASH */
#define FIELD_MARK 0x1e         /* ('^^') */

/*****************************************************************************/

typedef int BOOL ;
typedef struct kls{
   char * p ;			/* Address of start */
   u_short l ;			/* Length (0..65535) */
} KLS ;				/* Known-length string */
typedef int (*PFI)();		/* Fn returning int */
typedef void (*PFV)() ;
typedef char *(*PFPC)() ;	/* Fn returning char * */
typedef struct {
  PFI func ;			/* Pointer to function */
  char *name ;			/* Function name */
} IFN ;

struct names {
  char *name ;
  int value ;
} ;

/*****************************************************************************

  From indexing.h May 93

******************************************************************************/

#define MAXEXITERMLEN 254	/* Max extended index term len (Nov 1991)
				   Reduced 256-254 Jan 99 */

/*****************************************************************************/

#define GSLTOKLEN 5 /* Length of "see" gsl record's token */

/*****************************************************************************

  Some function-like macros

*****************************************************************************/

#define cvlu(ch) (((ch) >= 'a' && (ch) <= 'z') ? (ch) - 'a' + 'A' : (ch))
/* Jan 99: replaced by array lookup */
/* #define cvul(ch) (((ch) >= 'A' && (ch) <= 'Z') ? (ch) - 'A' + 'a' : (ch)) */
extern u_char U_to_lc[] ;
#define cvul(c) U_to_lc[(int)c]


/* Macros for reading bytes as unsigned values */
#define cv1(p)  ((u_int) *((u_char *)p))

/* Replaced by function Feb 1990 */
/* Restored Oct 96 */
/* Modified Nov 96 */
#define cv2hl(p) (unsigned)((256 * (*(u_char *)(p))) + (*(u_char *)((u_char *)(p) + 1)))
#define r2hl(f) (unsigned)((256 * (unsigned)(getc(f))) + ((unsigned)(getc(f))))

/* Replaced by function Feb 1990 */
/* Restored Nov 96 */
#define cv3hl(p) (unsigned)((65536 * *(u_char *)(p)) + cv2hl((u_char *)(p) + 1))
#define r3hl(f) (unsigned)(65536 * (unsigned)(getc(f)) + 256 * (unsigned)(getc(f)) + (unsigned)(getc(f)))
#define cv4hl(p) (unsigned)((65536 * cv2hl(p)) + cv2hl((u_char *)(p) + 2))

#define cv5hl(p) (OFFSET_TYPE)((65536 * (OFFSET_TYPE)cv3hl(p)) + (OFFSET_TYPE)cv2hl((u_char *)(p) + 3))

#define cv34hl(p) (unsigned)(((*p)&0x80)?(cv4hl(p)&0x7fffffff):cv3hl(p))

#define cv2lh(p) (unsigned)((*(u_char *)(p)) + (256 * *(unsigned char *)((u_char *)(p) + 1)))
#define cv3lh(p) (unsigned)(cv2lh(p) + (65536 * *(unsigned char *)((u_char *)(p) + 2)))
#define cv4lh(p) (unsigned)(cv2lh(p) + (65536 * cv2lh((u_char *)(p) + 2)))

#define cv5lh(p) (OFFSET_TYPE)((OFFSET_TYPE)cv2lh(p) + (65536 * (OFFSET_TYPE)cv3lh((u_char *)(p) + 2)))


#if HILO
#define cv2(p) cv2hl(p)
#define cv3(p) cv3hl(p)
#define cv4(p) cv4hl(p)
#define cv5(p) cv5hl(p)
#else
#define cv2(p) cv2lh(p)
#define cv3(p) cv3lh(p)
#define cv4(p) cv4lh(p)
#define cv5(p) cv5lh(p)
#endif

/* and for writing 2-byte and 4-byte values into char or BYTE arrays */
/* These macros cast the destination into pointer to unsigned char.
   I hope sign extension is avoided by casting the source to unsigned int */
/* See also w5hl as a function in utils.c */

#if HILO
#define w2(p, w) (*(u_char *)(p) = ((unsigned)(w)/256), *((u_char *)(p) +1) = ((unsigned)(w)%256) )
#define w4(p, w) (w2(p, ((unsigned)(w)/65536)), w2((u_char *)(p)+2, ((unsigned)(w)%65536)) )
#else
#define w2(p, w) ( * ((u_char *)(p)+1)=(unsigned)(w)/256, *(u_char *)(p)=(unsigned)(w)%256 )
#endif

#define w3hl(p, u) \
(*(u_char *)(p)=(((u_int)(u))>>8, *((u_char *)((p)+1))=((u_int)(u))>>4, \
  *((u_char *)((p)+2))=((u_int)(u)))) 

#define wf2hl(f, u) \
  (putc((u)/256,f), putc((u)%256,f))
#define wf3hl(f, u) \
  (putc((u)/65536,f), wf2hl(f,(u)%65536))

/* Macro for rounding non-negative real to nearest integer */
#define round(x) (int)((x) + 0.5)

/* Log function for weights etc */
#define lg(x) (double)(log((double)(x))/log(2) * 10.0)
#define alg(x) (int)(pow((double)2.0, (double)((double)x/(double)10.0)) + (double)0.5)
#define rlg(x) round(lg(x))

#define iceil(i,j) (((i)%(j))?(((i)/(j))+1):((i)/(j)))

#ifdef MMPUT

#define mput_vshort(u,p) \
((u)>16383)?0:(((u)>127)?(*(p)=(((u)>>7)|0x80), *((p)+1)=(u)%128, 2):\
(*(p)=(u), 1))

#define mget_vshort(p) \
((p)==NULL)?(-1):(((*(p))&0x80)?(((*(p))&0x7f)<<7)+*((p)+1):*(p))

#define st1or2(u,p) \
(u)<0x100 ? *(p)=(u)<<1,1 : (u<0x10000) ? *(p)=(((u)<<1)|1&0xff), *((p)+1)=(((u)>>7)&0xff), 2) : -2

#define ld1or2(l,p) \
(p)==NULL ? *l=0, -1 : ((*(p))&1)==0 ? *l=1,(*p)>>1 : *l=2,(((*p)+(*(p+1))<<8)>>1)


#endif


/* Length of a precount string */
#define len(s) (int)(cv1(s))

/* STRINGS is a slight generalisation of struct GSL */

typedef struct strings {
  int maxnum, 			/* Max no. of strings */
      maxsize,		        /* Max no. of bytes string storage */
      n,			/* Current no. of strings */
      used ;			/* No. of bytes of storage used */
  char **p ;			/* Array of maxnum pointers  */
  char *data ;			/* Base of array of Maxsize bytes */
} STRINGS ;

/* GEN_STORAGE is a struct for storing items which contain a fixed component
   (fixed) and a variable component (variable). An instantiation may only be
   used for one size of item (i.e. fixsize) because individual sizes are not
   stored (this would be the next level of generalization). This struct is a
   generalization of STRINGS (q.v.)

   The first component of the item must be a pointer to the item's variable
   field(s). The struct should be aligned for any type, so that its component
   items can be simply copied back if required. */

typedef struct {
  int maxnum, 			/* Max no. of items */
      maxvar,   	        /* Max no. of bytes variable storage */
      n,			/* Current no. of items */
      used ;			/* No. of bytes of variable storage used */
  int fixsize ;			/* Sizeof item */
  char *fixed ;			/* Base of array of maxnum items of size
				   fixsize*/
  char *variable ;      	/* Base of array of maxvar bytes */
} GEN_STORAGE ;

/* INDEX_TERM is a struct for holding an index term as it is extracted into
   temporary storage. (Added May 91: not finalised)

   March 99: this has never been used, tho' something like it might be
   useful if I ever get round to it.
*/

struct index_term {
  char *term ;
  unsigned recaddr ;		/* Record's disk address or similar */
  int fbyte ;			/* Only 6 or 8 bits used in short indexes */
  u_char field ;			/* Not used in short indexes */
  u_char sentence ;		/* Not used in short indexes */
  u_char wordnum ;		/* Not used in short indexes */
  u_char gsclass ;		/* May need more info than this, e.g.? */
  short oset ;			/* Offset from start of field of the source
				   of this term (chars) */
  short slen ;			/* Length of source */
} ;

/******************************************************************************

  gsl struct

  Moved here from parms.h Apr 92

  March 99: the names now malloc'd not arrays[16] as formerly.

******************************************************************************/

typedef struct gsl {
  int num ;
  int num_links ;		/* Number of index structs holding this GSL.
				   (was a BOOL 'live') also (March 99) may
				   be open independently of a database.
				   May be freed when no links left.

				   NB March 99: not using num_links,
				   too difficult to implement.
				*/
  BOOL no_db ;			/* TRUE if it's setup with no open db
				   (see check_gsl()) */
  char *name ;			/* Basename; gsl source files
				   are always on Control_dir */
  int (* stem_fn) (char *, int, char *) ;
				/* The stemming function to be used */
  char *func_name ;		/* .. and its name */
  int numrecs ;			/* Number of records stored */
  char ** sptrs ;		/* Base of address array */
  char * data ;			/* Base of data */
} Gsl ;

/******************************************************************************

  token struct for recording position and length of tokens in text

  Moved from stdeqs.h Dec 92

******************************************************************************/

#define LONGPOS_MAX_F MAXNUMFIELDS
#define LONGPOS_MAX_S 2047
#define LONGPOS_MAX_T 511
#define LONGPOS_MAX_NT 15
#define LONGPOS_MAX_SW 7

#define TEXTPOS_MAX_F MAXNUMFIELDS_TEXT - 1
#define TEXTPOS_MAX_S 32766	/* Note top value (32767) is reserved */
#define TEXTPOS_MAX_T 255
#define TEXTPOS_MAX_NT 15
#define TEXTPOS_MAX_SW 7

#define MAXTEXTPOS 16383
#define MAXLONGPOS 255		/* Greatest number of pos fields in a posting*/

#define MAXTF 16383		/* Indexes with no pos records.
				   Should be 32767 */

struct hugepos {
  unsigned f : 5 ;
  unsigned s : 17 ;
  unsigned t : 10 ;
  unsigned nt : 5 ;
  unsigned sw : 3 ;
} ;


typedef struct {		/* Positional fields for "text" dbs -- HILO
				   processors, type 4, 6, 8, 12 indexes */
  unsigned f : 2 ;              /* Reluctantly set max fds to 4 to get enough
				   sentences. I suppose this could be max fds
				   contributing to this index, with a mapping
				   of numbers if necessary (May 94) */
  unsigned s : 15 ;		
  unsigned t : 8 ;
  unsigned nt : 4 ;
  unsigned sw : 3 ;
} Pos_text ;

typedef Pos_text Pos_text_hl ;

typedef struct {		/* Positional fields for "text" dbs -- non-HILO
				   processors */
  unsigned sw : 3 ;
  unsigned nt : 4 ;
  unsigned t : 8 ;
  unsigned s : 15 ;		
  unsigned f : 2 ;  
} Pos_text_lh ;

typedef struct {		/* Positional fields for type 5, 7, 9, 13 indexes*/
  unsigned f : 5 ;
  unsigned s : 11 ;
  unsigned t : 9 ;
  unsigned nt : 4 ;
  unsigned sw : 3 ;
} Pos6 ;

typedef Pos6 Pos6_hl ;

typedef Pos6_hl Longpos_hl ;

typedef struct {
  unsigned sw : 3 ;
  unsigned nt : 4 ;
  unsigned t : 9 ;
  unsigned s : 11 ;
  unsigned f : 5 ;
} Pos6_lh ;

typedef Pos6_lh Longpos_lh ;

typedef struct {
  unsigned s : 16 ;
  unsigned t : 9 ;
  unsigned nt : 4 ;
  unsigned sw : 3 ;
} Pos_no_f_hl ;

typedef struct {
  unsigned sw : 3 ;
  unsigned nt : 4 ;
  unsigned t : 9 ;
  unsigned s : 16 ;
} Pos_no_f_lh ;

typedef struct {
  unsigned t : 24 ;
  unsigned nt : 8 ;
} Pos_t_nt_hl ;

typedef struct {
  unsigned nt : 8 ;
  unsigned t : 24 ;
} Pos_t_nt_lh ;


typedef union u_pos {
  unsigned pos ;
  Pos_text_hl pthl ;
  Pos_text_lh ptlh ;
  Longpos_hl  lphl ;
  Longpos_lh  lplh ;
  Pos_no_f_hl pnfhl ;
  Pos_no_f_lh pnflh ;
  Pos_t_nt_hl ptnthl ;
  Pos_t_nt_lh ptntlh;
} U_pos ;

/*****************************************************************************

Externs (mostly in Globs.c)

*****************************************************************************/
#include "hash.h"
#include "parms.h"

extern FILE *stdnothing ;
extern int stdzero ;
extern char *empstr ;
extern char D_name[] ;		/* Current database name (e.g. -D on command
				   line). Was Sourcefile = Bibliographic file
				   pathname. Altered June 90 */
extern char *Db_names ;		/* Names of available databases for
				   read_db_avail() */
extern int Db_num ;		/* Number of avail db's (ret by
				   read_db_avail() ) */
extern int Maxfiles, Maxmergestreams ;
extern Db Cdb ;		/* In globs.c */
extern Db *Current_db ;	/* This & next perhaps only in bss_globs */
extern char *Current_db_name ;

/* Next two were in parms.h (Apr 92) */
extern Gsl *Gsls[MAXGSLS] ;
extern int Num_gsls ;

extern char Site[] ;		/* Site name: determines printmessage,
				   welcome message etc. */

extern int LWD ; /* Length to compare for Cpwd (utils) */

/* Tables of punctuation and indexing marks */

extern char striptab[] ;  /* Indexing marks stripped for record display */
extern char indtab[] ;    /* Used by phase1() to change mode */

extern char *Control_dir, *Temp_dir, *Bib_dir, *Okapi, *BSS, *Localbib_dir ;

/* Macro replacing isequal() */

#define isequal(s1,s2,l) (! memcmp(s1,s2,l))

#endif /* _defines_h */

