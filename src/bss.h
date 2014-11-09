#ifndef _bss_h
#define _bss_h

/******************************************************************************

  bss.h  SW March 92

  Okapi BSS header containing externs etc

******************************************************************************/
#include "defines.h"

#ifndef NGROUPS_MAX
#define NGROUPS_MAX 32
#endif

#define LOG_STEM "bss_syslog"

#define SELECTION_TREE_CROSSOVER 5 /* Probably 5 - 8 is about right */

#define MAXIXREAD 1000000	/* Inc from 4096 May 93, from 200000 Oct 96 */
#define MAXSETRECS 16384	/* Inc from 2048 Apr 95 */

/* Feb 96: for real-valued weights. See bss_combine and i0_parse+.y */

#define MAXWEIGHT FLT_MAX	/* Weight must be < this and > next */
#define MINWEIGHT -FLT_MAX
#define WEIGHT_NOT_ASSIGNED  ((double)MAXWEIGHT * 2)
#define Legalweight(w)  (w<MAXWEIGHT && w>MINWEIGHT)

/* OP definitions for combine(). Stored in Setrec.op */
/* NB if these are changed change translate_opcode() */

enum {
  OP_COPY = -4,			/* For limit only, or simply copy */
  OP_TOPN = -3,			/* Pseudo-op */
  OP_NOOP = -2,			/* Unrecognised op */
  OP_LKUP = -1,			/* Dummy. For Setrec, not combine() */
  OP_BM1 = 0,			/* Best match, weights & thresholds supplied */
  OP_BM2 = 8,			/* Inner product of termwt and pstg wt */
  OP_BM11 = 1,			/* Coeff of BM9 applied to F4 */
  OP_BM15 = 2,
  OP_BM1100 = 3,		/* Universal BM11 op with k1 and k2 */
  OP_BM1500 = 4,		/* Universal BM15 op with k1 and k2 */
  OP_BM25 = 5,			/* New "power" version of bm11 etc */
  OP_BM25R = 26,
  OP_BM25S = 27,
  OP_BM25W = OP_BM2,
  OP_BM250 =  6,
  OP_BM251 = 24,		/* New May 97 */
  /* OP_BM2500 = 7,		   2500/2500W redundant May 02 as 25/25W
				   behaves as they used to*/
  /* OP_BM2500W = 34, */
  OP_BM26 = 23,
  OP_BM2600 = 25,
  OP_BM30 = 28,
  OP_BM3000 = 29,
  OP_BM40 = 30,			/* "Language Model" March 2000 */
  OP_BM41 = 31,			/* Ditto */
  OP_BM42 = 32,			/* Global correction for LM */
  
  OP_AND1 = 10,			/* Normal AND, no weights in output */
  OP_AND2 = 11,			/* AND as limit */
  OP_ANDL = OP_AND2,
  OP_MARK = 22,			/* Oct 95 */
  OP_AND3 = OP_MARK,
  OP_OR1 = 12,
  
  OP_NOT1 = 14,			/* Normal A and-not B */
  OP_NOT2 = 15,			/* NOT as limit */
  OP_NOT3 = 16,			/* Removes set[1] from set[0] */
  OP_UNCOMBINE = OP_NOT3,
  OP_NOTL = OP_NOT2,
  OP_ADJ1 = 17,			/* Ignores stopwords */
  OP_ADJ2 = 18,			/* Counts stopwords */
  OP_SAMEF1 = 19,		/* Same field */
  OP_SAMES1 = 20,		/* Same field and sentence */
  OP_NEAR1 = 21,		/* Same field and sentence within <maxgap> */
  
  OP_LIMIT = 13,		/* March 96: decided to make limit an op */
} ;

/* Flags values for lookup and combine etc. Many of these values come
   through from the interface. The top 8 bits of FFL may contain a "mark"
   of up to 7 bits + sign. If op is OP_MARK they are copied to (or removed
   from if negative) the flags field of a bss_Gp (see below) struct. If op
   is not OP_MARK they would be ignored. If a set has marks they are inherited
   by any  derived set. This is determined by the HASMARKS bit of the set's
   pstgtype field. */

enum {
FFL_NOPOS = 1,			/* If 1-bit set no positional data output
				   by combine (Nov 94) */
FFL_NOSAVE = 2,			/* Makes no output set if 2-bit set */
FFL_FILE = 4,	
FFL_MEM = 8,			/* Probably, if MEM no file, see doc */
FFL_OPEN = 16,			/* For setup_pstgs(). create if nec and open
				   files as appropriate. E.g. if just going
				   to be used for merge. */
FFL_KEEP = 32,			/* Keep file open on close_pstgs()
				   (default close). */
FFL_RECOVER_MEM = 64,		/* Hint to close_pstgs to recover allocated
				   or mapped memory. May write it to a file
				   if file doesn't exist. Not implemented. */
FFL_DELETE = 128,		/* Delete input sets after merge. Not
				   implemented. */
FFL_DO_STATS = 256,		/* Does weightsum and weightsumsq if true */
FFL_HOLD_MEM = 512,		/* Holds memory pages for set if possible */
FFL_NOTF = 1024,		/* See also FFL_NOPOS. In applications, NOTF
				   implies NOPOS (done at i0 level). */
FFL_NO_SELECTION_TREE = 2048,	/* Tells bss_do_combine() not to use selection
				   tree even if num >= crossover */
FFL_SELECTION_TREE = 4096,	/* Use selection tree regardless of crossover*/
FFL_NOSHOW = 8192,		/* Set not expected to be SHOWn, no weight
				   bucketing necessary. Unused (Apr 01). */
FFL_QUICK_COMBINE = 16384	/* Quick Willett-type combine (July 02) */
} ;


/* Need also perhaps way of indicating (and doing) field restriction(s).
   Might need another flags-type word with bit set for any wanted field. */

#define FFL_GETMARK(fl) ((fl)>>24)
#define FFL_DEMARK(fl)  ((fl) &= 0x00ffffff)

/* Set condition flags */

enum OP_TYPE {
  NO_TYPE = 0,

  BMLIKE = 16,
  ANDLIKE = 32,
  ADJLIKE = 64,
  ORLIKE = 128,

  NOTLIKE = 512,
  LIMITLIKE = 1024,
  TOPN = 2048,
  NO_MPW = 4096,		/* March 2000 for bm40/41 */
} ;

/******************************************************************************

  posting type flag values

******************************************************************************/

/* NB if these are changed change translate_pstgtype */

enum Ptypes {
  HASWT = 2,			/* Posting has a weight (Oct 00, this is now
				   score) field */
  HASTF = 4,			/* Posting might have neither positional
				   fields nor any term frequency field
				   (by default there is at least a TF field) */
  LONGPOS = 8,			/* Long positional fields */
  TEXTPOS = 16,			/* Long text-type pos fields */
  HASPOS = (LONGPOS|TEXTPOS),	/* Posting has positional fields */
  HASCONTRIBS = 32,		/* Should imply HASCOORD */
  /* TEXTPSTGTYPE = 64,		   ? redundant (July 98) */
  HASPASSAGES = 128,
  HASMARKS = 256,		/* Oct 95, OP_AND3 (OP_MARK) */
  LARGE_RN = 512,
  VARI_RN = 1024,
  HAS_AUXWT = 2048,		/* Oct 00 for new Robertson Sx,Sy scheme */
  HASCOORD = 4096,		/* Added Jan 01. Should be implied by
				   HASCONTRIBS */
  HAS_QTF = 8192,		/* For profile db */
  HAS_INDEX_WT = 16384,		/* May 02, for profiles (itype 20) */
/* <recnum> is 3 bytes by default, but 4 if LARGE_RN.
   VARI_RN not yet implemented (Oct 00).
   Weights etc are 4, LONGPOS and TEXTPOS are 4.
   The posting length or numpos pseudo-fields are "1 1/2"
   (1 or 2, see the functions fput_vshort() etc).
*/
  PTYPE12 = (TEXTPOS|HASTF),	/* Type 4, 6 or 8 index postings */
  PTYPE13 = (LONGPOS|HASTF),   /* Type 5, 7 or 9 index postings */
  PTYPE14 = HASTF,		/* Type 10 index postings */
  PTYPE15 = (PTYPE12|LARGE_RN), /* Type 12 index */
  PTYPE16 = (PTYPE13|LARGE_RN), /* Type 13 index */
  PTYPE17 = (PTYPE14|LARGE_RN),  /* Type 11 index */
  PTYPE18 = (PTYPE12|VARI_RN),  /* Type 15 index */
  PTYPE19 = (PTYPE13|VARI_RN),  /* Type 16 index */
  PTYPE20 = (PTYPE14|VARI_RN),  /* Type 14 index */
  PTYPE21 = (HAS_QTF|HAS_INDEX_WT) /* Type 20 index (profile dbs) */
} ;

/* Defines for set dump commands (Jan 96) */
enum {
  D_ALL,
  D_TTF,
  D_NP,
  D_ATTR,
  D_ERROR
} ;

/******************************************************************************

  bss_Gp

  The most general posting structure, used by read_pstg_unpacked()

  March 96: these are unnecessarily big. Should use just pointers for the
  three arrays instead of for only one of them.

******************************************************************************/

typedef struct para_rec {
  short fd ;			/* Field number */
  short dummy ;			/* Just to fill it if not packed */
  u_short sp, fp ;		/* Start & finish paragraph number (0- ) */
  float wt ;
} Para_rec ;

/* #define MAXPARRECS 255   Never used more than 2 (Apr 97) */
#define MAXPARRECS 2

/* Note (Oct 00): should a posting know its pstgtype?
   See notes_and_reminders. */

typedef struct bss_gp {
  u_short maxpos ;		/* With maxpara, implies size */
  u_short numpos ;		/* If ! HASPOS then this field holds tf
				   (the number of pos fields there were.
				   Hence working out how to read a
				   posting depends on knowledge of
				   the HASPOS bits.
				   (Jan 96) */
  short numpars ;
  short coord ;			/* No of nonzero tfs (might as well be short
				   altho only 0 - 255) */
  int rec ;			/* low 24 bits only unless database LARGE
				   (Feb 1998) */
  float score ;			/* Supersedes wt (Oct 00). Both wt and auxwt
				   may contribute to the score. wt is now only
				   used iff pstgtype & HAS_AUXWT */
  float wt ;			/* Jan 96. Oct 00: superseded by score, only
				   used iff pstgtype & HAS_AUXWT */
  float auxwt ;			/* Oct 00: y or any auxiliary weight.
				   Used iff pstgtype & HAS_AUXWT */
  u_short r ;			/* Little r for profile dbs (May 02) */
  u_char flags ;		/* New Oct 95: to hold rel info etc */
  u_char dummyflags ;
  Para_rec pars[MAXPARRECS] ;
  /* Next 2 fields only for op=OP_SPEC.., getting log regression data etc */
  struct str_tf {
    u_short str ;		/* Stream no (0 - 255) */
    u_short tf ;		/* Term frequency for this stream */
  } t[256] ;
  U_pos *p ;
} bss_Gp ;

/* Coordination level is number of nonzero tfs */

/* Packs down for file storage into
   
[<numprs>][<coord>][<numpos>]<rec>[<wt>][<flags>][<r>][<prs>][<trmno><tf>]][<pos>]
     1         1       1|2    3|4   4       2      2     10       2     2     4
     
*/

/******************************************************************************

  rrec and rbkt

  Structures for ephemeral sets (Aug 02)

  rrec is just a list element for an IRN (or any other int)
  rbkt is a header for a list of rrecs

******************************************************************************/

struct rrec {
  struct rrec *next ;
  int rn ;
} ;

struct rbkt {
  int count ;
  float minscore ;
  struct rrec *rlist ;
} ;

/******************************************************************************

  posting_flag values for bss_Gp
  
******************************************************************************/

#define PSTG_REL 1		/* 1-bit of flags field of bss_Gp */

/******************************************************************************

  SET_TYPES made enumerated Mar 01, atomic types elaborated and weighted
  type introduced.

******************************************************************************/

enum SET_TYPE {
  S_NO_TYPE = 0,
  S_P_ATOMIC = 1,		/* Pure atomic */
  S_A_ATOMIC = 2,		/* Adjacency of Q_ATOMICs */
  S_O_ATOMIC = 4,		/* OR of Q_ATOMICs */
  S_Q_ATOMIC = (S_P_ATOMIC|S_A_ATOMIC|S_O_ATOMIC),
  S_WEIGHTED = 8,
  S_MERGE = 16,
  S_LIMIT = 32,
  S_REMOVE = 64,
  S_RLIMIT = 128,
  S_PARTIAL = 256,		/* Mar 01: if op = topn or "targeted" */
  S_IRN = 512,
  S_NOSORT = 1024,		/* May 02: profile-type sets with scores
				   but not to be sorted for output */
  S_EPHEMERAL = 2048		/* One-shot. Deleted after output */
} ;

/******************************************************************************

  Setrec struct, used for all BSS sets

  Rearranged and modified many times, last >= May 02

******************************************************************************/

typedef struct bss_setrec {
  BOOL used ;			/* True if in use */
  int num ;			/* The record's index in the global array of
				   setrecs. Also acts as the
				   variable part of the postings filenames.
				   File names are bss_pf<0|1>_<num>.<pid> */
  int ctime ;			/* Creation time in seconds */
  int atime ;			/* Initially as ctime. Updated on leaving
				   setup_pstgs, open_pstgs, close_pstgs. */
  enum SET_TYPE set_type ;	/* (Mar 01) See enum SET_TYPES above.
				   Atomic status field removed March 96 */
  char *found_term ;		/* malloc'd storage for retrieved term if
				   successful single term search */
  char *search_term ;		/* Pure atomic only? */
  int search_type ;		/* 0: exact match, -2: glt, -1: gngt,
				   1: lnlt, 2: lgt. (If set_type==P_ATOMIC) */
  int search_result ;		/* As search_type, & 255 for complete failure.
				   Check this (May 93) */
#define MIXED_ATTRIBUTE -1
#define NO_ATTRIBUTE -2
  int attr_num ;		/* Seq number (0..) of the attribute used in
				   lookup (set_type 0). Maintained
				   through combine ops as far as poss. If no
				   single attribute applies this is given as
				   -1 (read "mixed") (New Dec 94) */
  /* Next few fields only used for term lookup sets (source type S_ATOMIC) */
  int ixnum ;			/* If set_type is 0, ixnum is that
				   of the index used, ix_chunk and ix_offset
				   the chunk and offset of the ITR which was
				   retrieved. (These enable retrieval of
				   next and previous terms.) */
  struct index *ix ;		/* Database's index struct. NULL if set
				   result of merge from more than one index. */
  int ix_chunk ;
  int ix_offset ;
  u_int limits ;		/* If set_type 0, the limits used in
				   lookup. Jan 96: should be used for any
				   type */
  /* Next two new in 2.05 Nov 96, not used yet */
  Db *db ;			/* Set's database */
  char *db_name ;		/* Might be used if set kept when db closed */

  BOOL adj_no_good ;		/* FALSE iff adj can be done on this set
				   (i.e. is output from pure OR and/or ADJ) */
  int op ;			/* op which produced this set */
  int n_streams ;		/* No of input streams, 1 if this is a
				   single term lookup, otherwise supposed to
				   be cumulative (but check sometime). */
  int pstgtype ;		/* See HASWT, HASPOS etc, above. This is
				   vital as packing/unpacking postings
				   depends on it. */
  double qtweight ;		/* Added 2/01: query term wt if atomic */
  int qtf ;			/* (3/01) Query term frequency if atomic */
  double k1, k2, k3, k7, k8, b ; /* (3/01) not sure which, when and how these
				   will be used */
  int score_function ;		/* Jan 01: default 0, if 1 or 2 (and set is
				   merge of weighted atomic sets) will use
				   auxiliary weight */
  double aw, gw ;		/* Weight thresholds in merge */
  double mpw ;			/* Max possible weight */
  double minpw ;		/* Min possible weight */
  double realmpw ;		/* Taking account even of weights of terms
				   with no postings (May 98, TREC-7 filt. */
  double maxweight ;		/* Highest weight achieved */
  double minweight ;		/* Not seriously used (Jan 96) */
  int nmpw, ngw, naw ;		/* Result (max poss weight, "good" weight,
				   total). If no weights all have same value.
				   Note naw is actual total no. of pstgs, not
				   necessarily the same as fsize because some
				   pstgs may not have been read. */
  int nmaxweight ;		/* Number with the highest weight attained */
  int target_number ;		/* (Aug 02) */
  int ttf ;			/* Actual total term frequency (merge as well
				   as atomic).*/
  int max_tf ;			/* Index types > 7 only. */
  /* Aug 02: next four fields should be replaced by gran and wbkts (with
   caution -- needs mods to get_setrec, bss_clear_setrec etc) */
  int weight_dist_grad ;	/* Scale for weight_dist[]. If 0 nothing
				   recorded, else in steps of mpw/<this> */
  float wt_bucket_mult ;	/* weight_dist_grad/mpw
				   Added Sep 99 to speed (?) DO_WEIGHTS
				   in weighted combine ops.
				   int(wt * this) gives bucket number */
  int weight_dist[WEIGHT_DIST_GRAD + 1] ; /* To store numbers of postings in
					     0-n%, n-2n%
					     and so on, of max poss wt */
  float weight_floors[WEIGHT_DIST_GRAD + 1] ;
  /* Changed back again from double to float Apr 99 following comparison
     problems in prepare_set_for_output() (qv) */
  /* Postings counted in weight_dist[j] have weight >= floor[j] and
     < floor[j + 1] (floor[WDG] considered infinite). Jan 96: shouldn't be
       needed, cheaply calculated on the fly */

  /* Fields for EPHEMERAL sets */
  int gran ;			/* Granularity of division of score range */
  struct rbkt *bkts ;		/* Array of gran + 1 buckets */
  float *scores ;		/* Array of accumulated scores */

  /* Two fields added Nov 94 for occasional statistical purposes (SER) */
  double weightsum ;		/* Just that */
  double weightsumsq ;		/* Sum of squares of weights */

  int totalp ;			/* Number of pstgs to be considered. If atomic,
				   and limit != 0, it's the number of
				   "unlimited" postings for the term. If from a
				   merge, it's the sum of the numbers of
				   postings in all the input streams, even if
				   this is greater than big N for the limit.
				   Used in estimating how much memory to
				   allocate for the set's posting storage.
				   (Rev Feb 96)*/
  int total_tf ;		/* Term frequency analogue of totalp */
  /* Postings are stored in file(s) and/or in memory. The memory may be
     a mapping of the index's postings file (if atomic) */

  /* FILES */

  /* There may be two files. File 0 is a raw and complete file of postings
     as produced by a lookup or combine. File 1 is a (possibly partial) file
     of pointers to the postings in File 0 in descending weight order.
     Each record in file 1 is the 4-byte offset of a record in file 0.
     File 1 (recfile) may be kept open between function calls, so a copy of
     the file pointer is kept, in fact both file pointers are kept if file is
     open). Note that in general file 1 is incomplete (one rarely needs to
     output all the postings in weight order, and if outputting sequentially
     file 1 is not needed). */

#define SET_FILE 1
#define SET_FILE_EXISTS 2
#define SET_FOPEN_READ 4
#define SET_FOPEN_WRITE 8
#define SET_FOPEN (SET_FOPEN_READ|SET_FOPEN_WRITE)
#define SET_FILE0_PF 16		/* Can read pstgs direct from index.
				   Always true for atomic set with limit 0
				   and not NOPOS */
#define SET_MEM 256
#define SET_MEM_MALLOC 512
#define SET_MEM_MAPPED 1024
#define SET_MEM_EXISTS (SET_MEM_MALLOC|SET_MEM_MAPPED)
  
#define SET_PF1_RECLEN sizeof(int) /* These are integer offsets */

  int output_flags[2] ;		/* Replaces file0_pf and gives other info. */
#define SET_NATORDER 1		/* Not sure these'll be used */
  
#define SET_WTORDER 2
  /* Next three for atomic (lookup) sets only */
  int pf_vol ;			/* Volume number in pstgs file (Apr 95),
				   really a byte at most (0-) */
#ifdef LARGEFILES
  OFFSET_TYPE pf_offset ;		/* Offset in own pf */
#else
  long pf_offset ;		/* Offset in own pf */
#endif
  FILE *global_pf ;		/* Index should have postings file open.
				   This is its pointer. (Feb 96). */
  int global_pf_length ;	/* Length of postings in Db's pf before
				   taking account of limits or nopos
				   (atomic only) */
#ifdef LARGEFILES
  OFFSET_TYPE global_pf_offset ; /* Offset in Db's pstgs file. */
#else
  long global_pf_offset ;	/* Offset in Db's pstgs file. */
#endif
  FILE *pfile[2] ;		/* File pointer file i, or NULL if the file is
				   closed or doesn't exist. */
  /* Feb 96: the '0' elements in the following 2 aren't used any more */
  int np[2] ;			/* Number of recs in file or memory <= naw */
  u_int pstgs_size[2] ;		/* Actual length of packed postings for this
				   set. Added Nov 95 for memory mapping.
				   If file exists it is equally the file size.
				   Corresponds to pfile[] etc. */
  char *pbase[2] ;		/* New June 95.
				   Memory address for base of postings, or
				   NULL if not in memory. pbase[1] should be
				   cast to int * because the memory contains
				   integer offsets (((int *)pbase[1])[n]
				   is the offset of posting n in pfile[0] or
				   from pbase[0] */
  char *pptr[2] ;		/* Pointer to next pstg at or above pbase. */
  int next_pstg[2] ;		/* Posting no. to which file or pptr points */
  char *membase[2] ;		/* If mmapped, membase in general lower
				   than pbase as has to be mult of pagesize */
  u_int memsize[2] ;		/* Actual memory mapped or malloced */
  bss_Gp *pstg ;		/* Current structured posting when needed */
  int raw_plength ;		/* (Raw) length of posting in the pstg field
				   (i.e. length when packed) */
} Setrec ;

extern Setrec *Setrecs[MAXSETRECS + 1] ;
extern int Num_Setrecs ;

/******************************************************************************

  Macro for returning whether recnum satisfies limit
  Should for speed check Db.lims exists outside this
******************************************************************************/

#define Sats_limit(rec,limit) \
 ((Cdb.lims && (((Cdb.lims[(rec) - 1] & (limit)) == (limit)) || ((rec == INT_MAX)))))

#define Getlimit(rec) \
     (Cdb.lims == NULL) ? 0 : Cdb.lims[(rec) - 1]

extern char *BSS, *Okapi ;
extern char *version, *vdate ;
extern char *Info_buf ;

extern int Pid ;		/* bss_globs.c */
extern int Pagesize ;
extern char Username[] ;
extern int Uid ;
extern int Gid ;
extern int N_groups ;
extern int Groups[] ;
extern int Rec_db ;		/* bss_globs (recording open and close db) */
extern int Maxfiles ;
extern int Largefiles ;
extern int Num_open_files ;

/******************************************************************************

  Data record and address types -- external databases, June 99									       
******************************************************************************/

typedef enum {
  One_per_file = 1,
  N_per_file
} ADDR_TYPE ;

typedef enum {
  Doctype1 = 1,
  Doctype2
} DOC_TYPE ;

/******************************************************************************

  trec_stats codes, from i0_defs.h June 99

******************************************************************************/

/* Codes for trec_stats function */
#define TS_STANDARD 0
#define TS_LONG 1
#define TS_SHORT 2
#define TS_LF1 101		/* 3r - 2n */
#define TS_LF2 102		/* 3r - n */
#define TS_LF3 103		/* 4r - n NB not sure this is right, check
				   TREC-7 */
#define TS_NF1 201		/* 6r^.5 - n */
#define TS_NF2 202		/* 6r^.8 - n */

/******************************************************************************

  Highlight on/off tables: type 3 indexes  SW Jan 93

  Set up by bss_highlight_rec()and passed to hl_next_sentence()
  Prob more or less obsolete (July 95)

******************************************************************************/

#ifdef OLD_HIGHLIGHT

struct highlight_table {
  int size ;			/* Number of records */
  int ons[256] ;
  int offs[256] ;
} ;

#endif /* OLD_HIGHLIGHT */

/******************************************************************************

  hl_rec struct for storing highlighting info for SHOW commands

  July 95 SW

******************************************************************************/

struct hl_rec {
  char code ;
  char oset[8] ;
  char l[8] ;
} ;


/******************************************************************************

  From extract.h May 1993

******************************************************************************/

/*****************************************************************************

  extract.h  SW June 1992

  Header file for bss_extract and indexing progs

*****************************************************************************/

/* Output types & places */

/* Places */

#define EX_FILE 1
#define EX_STRINGS 2
#define EX_GEN 4
#define EX_BUF 8
#define EX_HASH 0x10

#define EX_LARGE_RN 0x200	/* See also pstgtype LARGE_RN */
#define EX_VARI_RN  0x400	/* See also pstgtype VARI_RN */


/* General types */
/* IX0 - IX3 probably obsolete Jan 96 */
/* #define EX_IX0 0x100		OBSOLETE term rec beast */
#define EX_IX4 0x1000		/* term recnum pos_text (index types 4,6,8) */
#define EX_IX5 0x2000		/* term recnum pos6 (index types 5,7,9) */
#define EX_IX10 0x4000		/* term recnum (index type 10) */
#define EX_IX11 EX_IX10|EX_LARGE_RN /* term 4-byte-recnum (type 11) */
#define EX_IX12 EX_IX4|EX_LARGE_RN /* term 4-byte-recnum pos_text (type 12) */
#define EX_IX13 EX_IX5|EX_LARGE_RN /* term 4-byte-recnum pos6 (type 13) */
#define EX_IX14 EX_IX10|EX_VARI_RN /* term 1-4-byte-recnum (not yet) */
#define EX_IX15 EX_IX4|EX_VARI_RN /* term 1-4-byte-recnum pos_text (not yet) */
#define EX_IX16 EX_IX5|EX_VARI_RN /* term 1-4-byte-recnum pos6 (not yet) */
#define EX_IX20 0x8000		/* July 02 for profile db */

#define EX_PARSE 0x40000		/*  */
#define EX_SUPERPARSE 0x80000

#define IX 0x100000
#define BLURBY 0x200000		/* Prevents output of I_terms from TEXT
				   fields */
#define PRECOUNT 0x400000	/* Else null-terminated */

#define EX_SOURCE 0x800000
#define EX_TEXT 0x1000000
#define EX_NO_IRN 0x2000000	/* March 99 for "indexing" single document */
#define EX_DIAG 0x40000000

#define EX_OP_IX4 (EX_IX4|IX|PRECOUNT|EX_STRINGS|EX_SOURCE)
#define EX_OP_IX5 (EX_IX5|IX|PRECOUNT|EX_STRINGS|EX_SOURCE)
#define EX_OP_IX10 (EX_IX10|IX|PRECOUNT|EX_STRINGS)
#define EX_OP_IX11 (EX_IX11|IX|PRECOUNT|EX_STRINGS)
#define EX_OP_IX12 (EX_IX12|IX|PRECOUNT|EX_STRINGS|EX_SOURCE)
#define EX_OP_IX13 (EX_IX13|IX|PRECOUNT|EX_STRINGS|EX_SOURCE)
#define EX_OP_IX20 (EX_IX20|IX|PRECOUNT|EX_STRINGS)
/*#define EX_OP_QE  (EX_QE|PRECOUNT|EX_STRINGS)  Redundant */
/*#define EX_SUPER (EX_QE|EX_STRINGS|EX_SOURCE)  Redundant */
/*#define EX_SUPER_NOSOURCE (EX_QE|EX_STRINGS)   Redundant */
#define EX_USER_PARSE (EX_PARSE|EX_BUF)
#define EX_USER_SUPERPARSE (EX_SUPERPARSE|EX_BUF|EX_SOURCE)
#define EX_USER_SUPERPARSE_NOSOURCE (EX_SUPERPARSE|EX_BUF)
#define EX_USER_HASH (EX_PARSE|EX_HASH)	/* Filtering March 02 */

struct outrec {
  int type ;			/* Output type: how to output the term */
  char *term ;			
  int tl ;			
  char *source_term ;
  int stl ;
  char *output ;		/* This may be cast to struct of some kind or
				   just a character array */
  int outlen ;			/* Apr 02: maintained where meaningful */
  u_int rec ;			/* Record's address if any, or recnum */
  u_short limits ;
  char sgnum ;			/* Search group number, ? only used in QE */
  char gsclass ;		/* Added Aug 92 for QE
				   Probably only blank or 'I' of interest */
  int fdnum ;			/* Positional info */
  int senum ;
  int wdnum ;			/* Also token number */
  int nt ;			/* Number of tokens in a term */
  int sw ;			/* Number of preceding stopwords */
  U_pos p ;			/* Universal positional record */
  short source_offset ;		/* Source info for highlighting termlist */
  short source_len ;
  float wt ;			/* Added July 02 for profile dbs */
  int tf ;			/*              ditto            */
} ;

#define MAGIC_MERGEFILE_BE 0xfffffff0 /* Ident word at start of mergefile */
#define MAGIC_MERGEFILE_LE 0xfffffff1
#if HILO
#define MAGIC_MERGEFILE MAGIC_MERGEFILE_BE
#else
#define MAGIC_MERGEFILE MAGIC_MERGEFILE_LE
#endif /* HILO */

#define IXF_REPORTLEVEL 1024

#include "bss_protos.h"

#endif /* ndef _bss_h */
