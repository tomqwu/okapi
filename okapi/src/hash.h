/******************************************************************************
									         hash.h  SW Jan 02
  
  Temporary header file for filtering-type routines (see hashsubs.c)
  
******************************************************************************/

#ifndef _hash_h
#define _hash_h

typedef enum {			/* For hash_search() */
  H_ADD,			/* record or NULL if no memory */
  H_SEARCH,			/* record or NULL if not present */
  H_DELETE
} Hash_action ;

typedef enum {
  HASH_GEN = 0,			/* Same as MIN exc recsize is given as an
				   arg to make_hashtab */
  HASH_MIN = 1,			/* Table of Genrec */
  HASH_MED = 2,			/*          Med_termrec */
  HASH_FULL = 3,		/*          Full_termrec */
  HASH_SPEC1 = 4		/* Modified Med_termrec */
} Hash_type ;

/******************************************************************************

  hashtab  Generalized hash table header record

  SW Jan 02

******************************************************************************/

typedef struct hashtab {
  Hash_type tabtype ;
  struct genrec **tab ;		/* Array of pointers to top level records */
  int tabsize ;			/* Array size */
  int recsize ;			/* Record size */
  u_int seed ;			/* Sometimes needed for hash function */
  char *funcname ;
  /* Note the int length arg is only used by some hashfunctions */
  int (*hashfn)(char *, int, struct hashtab *) ; /* Function is looked up by
						    name */
  int n_occ ;			/* Number of populated top level slots */
  int n_keys ;			/* Number of distinct keys */
  int depth_sum ;		/* Sum of depths (top level = 0) */
  int max_depth ;
  int tmem, dmem ;		/* Term, data memory resp */
  int n_comps ;			/* Number of string comparisons */
  int last_index ;		/* May 02: added for traverse_hash() */
  int last_depth ;		/* 0-. Depth of last record in lastindex.
				   Initially must be -1. */
} Hashtab ;
  
/******************************************************************************

  A genrec is simply the first two fields of any of the term hash-record
  structures -- for programming convenience.

  See full_termrec, med_termrec and spec1_termrec

******************************************************************************/

typedef struct genrec {
  struct genrec *next ;
  char *term ; 
} Genrec ;


/******************************************************************************

  full_termrec can store all the information from an ITR.
  Note full_ and med_termrec are the same as far as the third field (ttf).

  It's assumed that strings are null-terminated.

******************************************************************************/


typedef struct full_termrec {
  struct full_termrec *next ;
  char *term ;
  int ttf ;
  int tnp ;
  u_short maxtf ;
  u_short termlen ;
  int pstglen ;			/* Total length of posting data */
  u_short limits ;
  u_short pstg_vol ;		/* Normally zero. See next field. */
  off_t pstg_offset ;		/* normally offset regarding postings as
				   logically a single file (cf. bibfiles)
				   but if pstg_vol > 0 offset in that vol
				   (for backwards compat). */
  int count1, count2 ;		/* Access counts for instance */
} Full_termrec ;

/******************************************************************************

  Med_termrec stores enough information to work out termweights, but
  doesn't allow retrieval. If used for storage of terms from a single doc
  termweights may be stored in the "u" field instead of number of postings.

  Strings are null-terminated.

  Spec1_termrec as Med but has additional int field can be used for set
  number

******************************************************************************/

typedef struct med_termrec {
  struct med_termrec *next ;
  char *term ;
  int ttf ;
  union {
    int tnp ;
    float wt ;
  } u ;
} Med_termrec ;

typedef struct spec1_termrec {
  struct spec1_termrec *next ;
  char *term ;
  int ttf ;
  union {
    int tnp ;
    float wt ;
  } u ;
  int qtf ;
} Spec1_termrec ;

/******************************************************************************

  transfer_itr

  Used for generalized parsing of ITRs, not for storage

******************************************************************************/

typedef struct transfer_itr {
  int itype ;			/* Index type number, probably unnecessary */
  char *term ;			/* Just a pointer */
  int termlen ;
  u_short limits ;
  u_short maxtf ;
  int ttf ;
  int tnp ;
  int plength ;
  int pstg_vol ;
  off_t pstg_offset ;
} Transfer_itr ;

/******************************************************************************


******************************************************************************/

int bitwisehash(char *, int dummy, Hashtab *) ;
int bwhashpwr2(char *, int dummy, Hashtab *) ;
int bwhash_kl(char *, int length, Hashtab *) ;
int bwhashpwr2_kl(char *, int length, Hashtab *) ;
Hashtab *make_hashtab(Hash_type, int, char *, u_int, int recsize) ;
void free_tab(Hashtab *) ;
void *hash_search(Hashtab *, char *, int, Hash_action, BOOL) ;
void *hash_add_itr(Hashtab *table, Transfer_itr *tr) ;
Hashtab *hash_si(Hash_type, int, char *funcname, int, FILE *, int *) ;
void *traverse_hash(Hashtab *) ;

#define HASHSEED 1159241

#endif /* _hash_h */
