#ifndef _parms_h
#define _parms_h

/*****************************************************************************

  parms.h

  Header file for okapi parameter definitions  Dec 1989

  Apr 92: made database struc typedef (DB)
          redefined index type 2
  Jun 90: moved struct index & struct pstgparms here from stdeqs
  Apr 90: moved database struct here from stdeqs.h

  
*****************************************************************************/


/* A few defines */

#define MAXBIBVOLS 16
#define MAXIXVOLS 16
#define MAXSTEMFILES 8

/* Field types and their names */

typedef enum field_type {
  PERS, CORP, NAMES, TITLE, MAIN_TITLE, SUBTITLE, DEWEY, SH, SH_SUBDIV, TEXT,
  PHRASE, LITERAL, LITERAL_NC, NUMBERS, YEAR, UDC, ANY, NONE, END
} FIELD_TYPE ;

struct field_type_name {
  FIELD_TYPE type ;
  char * name ;
} ;

#ifndef GLOBS
extern struct field_type_name F_t_names[] ;

#endif

/*****************************************************************************

  Indexing extraction regimes and their names

*****************************************************************************/

typedef enum ext_reg {
  no_reg = 0,
  title_words,
  title_phrase,
  name_words, 
  name_phrase,
  name_and_inits, personal_name, corp_name_phrase,
  surnames, 
  subject_heading, 
  words, phrase, 
  Dewey, udc, literal, literal_nc, 
  words3,
  lines,
  profile
} Extraction_regime ;    

struct ext_reg_name {
  Extraction_regime reg ;
  char * name ;
  BOOL valid ;
} ;

extern struct ext_reg_name Ext_reg_names[] ;


/*****************************************************************************

  Stemming functions and their names - see Stem_fns

*****************************************************************************/

#ifndef GLOBS

extern IFN Stem_fns[] ;

#endif /* GLOBS */

/*****************************************************************************

  Search group parameters for a database (file <dbname>.search_groups)
  Query expansion parameters

  Each parameter describes how terms should be extracted for a search type
  in indexing or for query expansion. There is one for each search group
  for each index for each database. 

  A database has a BSS_MAXIX array of Search_groups:
  1 for each group in each of the possible indexes. 

  The file format from which these are read <database>.search_groups is

  <name> dummy (was beast) index(0-2) reg_name stem_func_name gsl_name fields

  <name> is a sequence of alpha mnemonics separated by |
  <fields> consists of <fdnum> <fd_value[fdnum]> terminated by a negative
  fdnum (but there must not be a <fd_value> for the terminating -tve fdnum)

  The names are stored as a string of strings in the struct

  Each line is a parameter, line terminated by negative number

*****************************************************************************/

enum sg_status {
  SG_UNSET = 0,
  SG_READ_OK = 1,
  SG_GSL_OK = 2,
  SG_INDEX_OK = 4,
  SG_GSL_FAIL = 8,
  SG_INDEX_FAIL = 16,
  SG_COMPLETE = (SG_READ_OK|SG_GSL_OK|SG_INDEX_OK)
} ;

typedef struct search_group {
  enum sg_status status ;
  struct db *db ;		/* DB to which it belongs */
  int number ;			/* Number in Db.groups. Filled in when
				   Db opened. Not from parameter. */
  /*char search_names[NNAME_MAX] ;  String of names for this search group */
  char *search_names ;		/* (Oct 98) Attribute name(s) */
  int dummy ;			/* Obsolete since 94 or earlier. Power of 2 */
  int ixnum ;			/* Index number: 0- read from parm file */
  struct index *ix ;		/* Added June 97, now 1:1 SGs and indexes */
  int itype ;			/* Index type number. Added March 99. Until
				   new parameters, obtained from Db parm by
				   read_search_groups(). Later will be field
				   of an attribute parameter. */
  char *reg_name ;		/* Extraction regime name from file */
  Extraction_regime reg ;	/* Enum type decoded from parm file */
  char *stem_name ;		/* Stem function name from file */
  int (*stem_fn)() ;		/* Stem function address */
  char *gsl_name ;		/* GSL name from file */
  Gsl *gsl ;			/* Points to a global GSL - this might be
				   changed in the future */
  short fields[MAXNUMFLDS + 1] ;/* Terminated by negative number */
  short field_values[MAXNUMFLDS + 1] ; /*  */
				/* Weight adjustment factors
				   field_value[fdnum] is the value of
				   field fdnum (see pmerge2_ex for use)
				   Never been used in BSS (Oct 93) */
} Sg ;

/******************************************************************************

  limits struct

  into which the contents of the limits file are read

  The file format is

    [<name> <bit>\n]

  where <name> is a character string without embedded white space of
  max length NNAME_MAX and <bit> is 1, 2, 4, .. 32768

  See db.limits

******************************************************************************/

struct limits {
  char name[NNAME_MAX] ;
  int lim ;			/* 1, 2, 4, .., 32768 */
} ;

#ifdef OLD_INTERACTIVE

/******************************************************************************
  fullparms struct

  Stores display information for "full" record displays
  Jan-March 1990 SW

  Normally there will be an array of fullparms. Each element describes the
  display for the given field number. The array ends when a field number
  99 is read

  'label' is a string to be output before the text starts. 'style', 'offset'
  and 'surround' determine the layout as follows:
  style 0 puts label on first line immediately followed by the text,
  succeeding lines indented to the position following the label;
  style 1 is label on first line with succeeding lines indented 'offset'
  spaces; style 2 is label on line by itself followed by text starting on next
  line. For 'surround' see comment below. 'Rendition' added 22 Apr 91.


*****************************************************************************/


#define LABLENGTH 64

struct fullparm {
  int fdnum ;   /* One record for each field. Ends when fdnum = 99 */
  int linesbefore ;
  char label[LABLENGTH] ;
	        /* Empty string if no label. Otherwise if labels to be
		   on same line as text they should normally all be the
		   same length. */
  int style ;  /*  0: normal, with label. 1: label on 1st line, other
		   lines not indented (unless by offset field). 2: label on
		   separate line above. */
  int offset ;	/* Extra offset beyond that implied by style and label. If
		   label on 1st line applies to succeeding lines (ignored
		   if style 0) */
  char surround ; /* 'N' or blank normal, 'Q' = quoted, 'P' = parens,
		     'L' in square brackets, 'V' angle brackets. */
  int rendition ; /* Not implemented yet */
} ;

/*****************************************************************************

  briefparms struct

  Stores display information for "brief" record displays
  Jan 1991 SW

  Normally there will be an array of briefparms. Each element describes the
  display for a given type of search. It is indexed by CorD, which is set
  according to the search type. The label elements are used to make the
  header line for a brief display.

  Up to NUMFLDS fields can be displayed in a line

*****************************************************************************/


#define B_LABLENGTH 64
#define B_NUMFLDS 6

struct briefparm {

  char type[8] ;		/* The name of the search type */
  struct b_p {
    char label[B_LABLENGTH] ;
    int fdnum ;
    int width ;
    int gap ;
  } b_p[B_NUMFLDS] ;
} ;

#endif /* OLD_INTERACTIVE */

/*****************************************************************************

Database struct 

*****************************************************************************/

typedef struct db {
  char *parm_buffer ;  /* Points to database description string buffer */

  char *name ;         /* Database file name (e.g. lisa.84..89.3). This is
			  the file name of the run-time source, and is also
			  used as a prefix by some associated files. Case is
			  significant, leading and trailing blanks ignored */
  /* bib: obsolete: The pathname of the bibliographic file.
			  Only used until multivolume working (June 92) */
  /* Next 5 fields for db's which spread over more than one
     directory. Superseding bib */
  int lastbibvol ;     /* Number of directories for the bibfile -1 */
  char *bib_basename[MAXBIBVOLS] ; /* Basename of bibfile (made array 7/96) */
  char *bib_dir[MAXBIBVOLS] ;   /* */
  /* Next one to become obs June 98 */
  int bibsize[MAXBIBVOLS] ;     /* Max size in megabytes. convert_runtime
				   writes the exact sizes into the
				   real_bibsize fields below */
  OFFSET_TYPE real_bibsize[MAXBIBVOLS] ;/* Real vol size in bytes / rec_mult */
  char *display_name ; /* Database label which is shown for database
			  selection and on search input screens (e.g. LISA
			  1984-89). */
  char *explanation ;  /* User description. May contain escaped CRLF
			  sequences entered as '\r\n'. Example: "LISA 84-89
			  contains about 37,000 titles in the area of
			  information science and librarianship,\r\nmostly
			  published between 1982 and 1988" */
  int nr ;             /* Number of records in the database. Entered by
			  database manager. */
  int nf ;             /* Number of fields in each record. Entered by
			  database manager. */
  char *f_abbrevs[MAXNUMFLDS + 1] ;
                       /* Array of field name abbrevs for logging etc.
                          No abbrev can be given for fd 0. Max len 3 */
  int rec_mult ;       /* A small power of 2 (Sept 97: needn't be power of 2).
			  The length of each record is a multiple of
			  rec_mult. Entered by database manager.
			  For index type 0 only (obsolete) max size of 
			  database is 16 * rec_mult megabytes. A gigabyte
			  needs a rec_mult of 64 & this is probably about
			  the maximum reasonable value. There is an average
			  wastage of nearly half rec_mult bytes per record.
			  There is no practical limit for other index
			  types. */
  int ni ;             /* Number of index files.
			  The maximum is BSS_MAXIX. They are called
			  <ix_stem>.<index number>, where index number
			  goes from 0 to ni - 1. */
  char *ix_stem[BSS_MAXIX][MAXIXVOLS] ;
                       /* Directory and filename stem for index numbers
			  0 to ni - 1.
			  E.g. "/home/bibfiles/lisa/indexes/lisa".
			  CHANGED 14 July 90. Was just directory, to which
			  was added 'name'.
			  Everything but postings file is on 1st vol.
			  Nov 96: Note corresponding vols of different indexes
			  used to have the same stem. This was OK provided
			  there was only one BIG index.
			  Altered to a 2-D array Nov 96. */
  int last_ixvol[BSS_MAXIX] ;
  /* Next to be off_t */
  OFFSET_TYPE ix_volsize[BSS_MAXIX][MAXIXVOLS] ;

  /* Next 3 fields added 27 Nov 91. They are really indexing parameters,
     and perhaps shouldn't be here */
  int itype[BSS_MAXIX] ;    /* This (really enum) determines the index
			       structure. Jan 93: there are now types 0 1 2 3
			       Feb 98: Now 8, 9, 10 & probably 4-7 accepted.
			       May 02: also 11, 12, 13 and new 20. */
  int fixed ;          /* (Sept 97) I don't think fixed field is used any
			  more, always zero. has_year was removed some time
			  ago. */
                       /* Length of record's fixed field. In recent db's
			  it is determined by convert_runtime from has_lims
			  and has_year. */

  char *db_type ;      /* Name of database type. Added Feb 92. 
			  Used by convert_runtime_trec. Includes trec, cat
			  Now (Apr 94) "trec" is obsolete. There is a new
			  type "text". See the new fields recptrs and
			  paraptrs. Many new types Feb 98- */
  int i_db_type ;      /* Numeric version of db_type, for speed.
			  For conversions, see defines.h, DB_CAT_TYPE etc.
			  Bits used. Added Apr 94 */
  int maxreclen ;      /* No longer used (<Sept 97) */
                       /* New May 94. Filled in by convert_runtime. Only of
			  importance for databases of type text. Used for
			  deciding how much to malloc for recbuf */
  BOOL has_lims ;      /* Limit bits are no longer (< Sept 97) stored in the
			  record, just in associated .lims file. Value (true
			  or false) of has_lims is taken from db parm file.
			  True iff there are limit bits in the indexes.
			  The limit bits are the first 2
			  bytes of the record. (1st field of exch format)
			  (db->fixed must be >= 2). Added 27 May 92 */
  int parafile_type ;  /* (Sep 98) Old ASCII is type 0. New "binary"
			  type 1, variable binary type 2, will supersede
			  0 and 1 so I guess this is a temporary field */
  /* next 3 for profile-type dbs (Apr 02) */
  char *stemfunc ;
  char *extreg ;
  char *gsl ;
  /* Fields which are used at runtime & are not in the parameter file */
  int m_sfd[MAXBIBVOLS] ;	/* m_sfd[0] = sfd, others only used if
				   lastbibvol > 0 */
  FILE *current_bibf ;		/* File stream for SI (Dec 97 for external) */
  int current_sfd ;		/* The current one for multivol  */
  FILE *dirf ;			/* Added March 97: directory file stream */
				/* March 98: now unused I think */
  int dirfd[MAXNUMFLDS + 1] ;	/* Directory file descriptors. Now an array
				   (3/98) to allow for aux bibfiles. dirfd[0]
				   is for main bibfile. */
  int aux_bib[MAXNUMFLDS + 1] ;	/* 1 - nf for auxiliary bibfile descriptors
				   (probably only used for field 1 if at all)*/

  /* Next four fields added Apr 94. The first is to allow rapid indirect
     record access, the .dir file having been read in. At first only to
     be used for db_type text, change over sometime for other types.
     The second is only used for db_type text. */

  FILE *pardirf ;		/* (May 97) now a separate paragraph dir */
				/* Record size 4 */
  u_int *paraptrs ;		/* malloc'd array of paragraph file core
				   image offsets (equally these may be
				   .par file addresses) */
  BOOL parafile_in_core ;	/* Wd be true if parafile had been read
				   into core (prepare_paras() with code = 2),
				   unimplemented May 95. */
  int *parabuf ;		/* Normally address of buffer into which
				   paragraph records are read and decoded
				   (Sep 98, rep global) */
  int parabuf_size ;		/* In units of sizeof(int) */
  int parstat ;			/* See prepare_paras() for values. (Zero means
				   not done yet.) Replaces nopars Sep 01. */
  /* Apr 02: dir_recsize is replaced by dir_recsize + dir_lensize */
  int dir_recsize ;		/* Size of offset portion of document
				   directory record: 4 normal, 5 for "large"
				   db (Feb 98) */
  int dir_lensize ;		/* Replaces fddir_recsize if nf=1, o'wise 0 */
  
  BOOL no_drl ;			/* Always TRUE if nf > 1, generally false
				   otherwise. */
  int fddir_recsize ;		/* Size of field length record
				   (2 normal, 3 text, 0 if db has a .bibdrl
				   file -- see above) */
  FILE *parafile ;		/* Paragraph file, text databases (May 94). */
  int big_n ;			/* No. of indexed recs in the db.
				   Added Apr 92 replacing global for BSS only.
				   For text databases is big_n corresponding
				   to current_limit. Now ALL databases ?
				   (Nov 94), N_s[] obsolescent. */
  u_int current_limit ;
  u_short *lims ;		/* Array of Db.nr limit values, or NULL  */
  struct index *ix[BSS_MAXIX] ;	/* Index structs */
  int index_status[BSS_MAXIX] ;	/* See ST_ defines in bss_errors.h */
  FIELD_TYPE field_types[MAXNUMFLDS + 1] ;
				/* The [default] field type for each field
				   (29 April 1991) - replaces default_modes */
  int num_search_groups ;	/* Sept 97: assume this now same as ni*/
  Sg groups[BSS_MAXIX] ;	/* (Sept 97) should belong to index struct */
  int num_lims ;		/* New Mar 95: number of limit records. Used
				   by ix1 */
  int highest_lim ;		/* Ditto */
  struct limits limits[MAXLIMS] ; /* New 28 May 92. Only used if has_lims */
  /* May 94: decided to put recbuf here as malloc'd */
  char *recbuf ;
  int recbufsize ;
  /* Current record fields here Dec 98 were global */
  int current_recnum ;
  int current_reclen ;
  struct bss_setrec *current_record_srec ; /* Was set number, now can be
					      default NULL */

  BOOL open ;
} Db ;

typedef struct uf_index {
  void *base ;			/* NULL until mapped or read in */
  int size ;
  void *ptr ;			/* Current position */
  char* current_term ;
  char *pathname ;
  struct uf_index *next_ufi ;	/* NULL if last */
} UF_INDEX ;

#define B_HILO 1
#define B_LOHI 0

typedef struct index {
  int status ; /* 0 initially, 1 when PI read, buffers assigned etc */
  int type ;		
  int byteorder ;		/* B_HILO or B_LOHI (Dec 97) 
				   But see note in notes_and_reminders */
  int num ;			/* Index number for database */
  int attr_num ;		/* Attribute number (now (Feb 96) a 1:1 corr
				   between attributes and indexes). */
  Sg *sg ;	/* This index's search group. */
  int pstgtype ;
  BOOL sxq ;			/* True if there is a soundex index -
				   never been used */
  int num_ufis ;		/* Number of unformated subindexes */
  int num_subsid_indexes ;
  struct index *subsid_indexes ; /* Subsidiary (formated) indexes */
  struct uf_index *ufis ;	/* Pointer to first UFI */
  char *pibuf ;
  char ** pptrs ;		/* Base of array of pointers to PI records */
  char *ovfbuf ;		/* Appears not to be used (June 90) */
  void *sibuf ;
  size_t schunksize ;		/* Added March 2002. Needs to be added to
				   a parameter. Currently always SCHUNKSIZE. */
  int sifd ;			/* File descriptor for SI */
  int last_ixvol ;		/* For multivol pstgs file (Apr 95) */
  OFFSET_TYPE ix_volsize[MAXIXVOLS] ;	/* Max size in bytes of volume */
  OFFSET_TYPE pfsize[MAXIXVOLS] ;	/* Actual size in bytes of volume */
  FILE *ofp[MAXIXVOLS] ;	/* OI stream, supersedes ofd for most types
				   5/92), Made array for multivol Apr 95. */
  int ofd[MAXIXVOLS] ;		/* Needed from Jan 96 for memory mapping */
  char *pfname[MAXIXVOLS] ;	/* Feb 97 (2.095) - see ixinit() */

  int numchunks ;		/* Number of chunks in SI (& records in PI) */
  int cchunk ;			/* Current SI chunk, -1 if invalid */
  
  struct index  *sx ;		/* Index's Soundex index, if Sxq true.
				   Hasn't been used since about 1987. */
  int *doclens ;		/* Length of indexed portion
				   of each record, read from .dlens file.
				   If NULL, doclen is taken as avedoclen[lim]
				   for all docs. */
  int has_doclens ;		/* Zero initially, means no knowledge. Set to 1
				   if doclens read (mapped) OK, 2 if read
				   (malloc'd) OK, -1 if hasn't any or can't.
				   (New May 98 for get_doclen(), mod Dec 98) */
  int zero_avedoclen ;
  int current_avedoclen ;	/* Ave doclen for current attribute/limit */
  /* See also below for CTTF etc */
  /* current_cttf appears unused (Apr 02) */
#if defined(HAVE_LL)
  LL_TYPE current_cttf ;	/* Total indexed tokens for current attr/limit*/
				/* (New Nov 99) */
#else
  double current_cttf ;
#endif
  int *doctoks ;		/* Number of indexed tokens in doc */
  int zero_avedoctoks ;
  int avedoctoks ;		/* Ave ditto for current limit */
  int *doctypes ;		/* Number of indexed types in doc */
  int zero_avedoctypes ;
  int avedoctypes ;		/* Ave ditto for current limit */
  BOOL lims_invalid ;		/* Set TRUE if lookup using limits file other
				   then the one used when the index was made.
				   NB Not set false when limit zero */
  /* Gsl field currently unused (March 99), but is set to ix->sg->gsl */
  Gsl *gsl ;			/* This index's search group's gsl */
  /* These are for unstemmed index's stemfiles to enable runtime stemming.
     Unused as at March 99 */
  FILE *stemfiles[MAXSTEMFILES]; /*  */
  char *stemfns[MAXSTEMFILES] ; /* Function names for the above (from the
				   global Stem_fns[].name) */
  int nrecs ;			/* N: number of indexed docs */
  /* Index statistics from <index_stem>.st (Apr 02) */
#ifdef HAVE_LL
  LL_TYPE cttf ;		/* tokens in index */
  LL_TYPE tnp ;			/* total postings */
#else
  double total_cttf ;
  double tnp ;
#endif /* HAVE_LL */
  int tnt ;			/* number of distinct indexed terms */
  u_int tottermlen ;		/* total length of above not incl any precount
				   or terminator */
} Ix ;

/* memindex is a greatly cut down index struct for indexes entirely in
   memory. More fields to be added no doubt. SW July 02. */

typedef struct memindex {
  int status ;
  int itype ;
  int byteorder ;
  int nrecs ;
  int cttf ;
  int tnp ;
  int tnt ;
  Hashtab *mixtab ;		/* A table whose records are mitr's */
} Memindex ;


typedef struct mitr {		/* In-memory term record */
  struct mitr *next ;
  char *term ;
  int tnp ;
  int ttf ;
  void *startp ;		/* First posting, sould be any of the mpstgs */
} Mitr ;

#define MITR

struct mpstg_20 {		/* Filtering in-memory posting */
  u_int tf : 8 ;
  u_int rn : 24 ;
  float wt ;
  struct mpstg_20 *next ;
} ;

struct mpstg_10 {		/* Normal nopos index in-memory posting */
  u_int tf : 8 ;
  u_int rn : 24 ;
} ;

struct mpstg_8 {		/* Normal type 8 or 9 in-memory posting */
  u_int tf : 8 ;
  u_int rn : 24 ;
  U_pos *pos ;			/* First positional record */
} ;


  
/******************************************************************************

  New ix_parms struct SW June 92

  Derived during indexing from Search_groups for a specific index, keyed
  on fdnum, need up to 16 for each field

  < Jan 98. No beasts any more, so one per field

******************************************************************************/

struct ixparms {
  Extraction_regime reg ;
  int (*stem_fn)() ;
  Gsl *gsl ;
  int beasts ;			/* ORed together for each reg/fn/gsl comb */
} ;

extern struct ixparms ixparms[MAXNUMFLDS + 1][1] ;

extern int num_ixparms[MAXNUMFLDS + 1] ; /* No. of ixparms for each field */

/* These externs are in bss_extract.c. SHould be in some globs unless
   only wanted in indexing, in which case they should be in ixsubs.
   But I think they may be used in the bss if RF is implemented */

#endif /* _parms_h */

