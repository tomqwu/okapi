#include "bss.h"
#include "parms.h"

/*****************************************************************************

  bss_globs for BSS perhaps temp Apr 92
  !

*****************************************************************************/

int Pid ;
int Pagesize ;
#ifdef LARGEFILES
int Largefiles = 1 ;		/* True if compiled with large file support
				   (e.g. if Solaris 2.6) */
#else
int Largefiles = 0 ;		/* True if compiled with large file support
				   (e.g. if Solaris 2.6) */
#endif

int silent ;			/* Added March 02 as used by imerge() */
int Maxfiles = MAXFILES ;	/* See bss_startup() (Aug 93) */
int Maxmergestreams = MAXMERGESTREAMS ;	/* (Aug 93) */
int Num_open_files = 0 ;	/* Added May 1993 -- using Jan 96,
				   see bss_fclose() etc */
char *Info_buf ;		/* Used by db_show etc. Malloc'd */

char Username[L_cuserid] ;	/* For user name (cuserid()) */
int Uid ;
int Gid ;
int N_groups ;			/* Actual number of groups for user */
int Groups[NGROUPS_MAX] ;

FILE *stdnothing ;		/* /dev/null */
int stdzero ;			/* fd for /dev/zero, used for mappings */
char *empstr = "" ;
IFN Stem_fns[] = {
   {wstem, "wstem"},
   {nostem, "nostem"},
   {sstem, "sstem"},
   {psstem, "psstem"},
   {(int (*)())NULL, NULL},
} ;

char *Okapi, *BSS ;
char *Control_dir = CONTROL_DIR ;
char *Temp_dir = TEMP_DIR ;
char *Bib_dir = BIB_DIR ;
char *Localbib_dir = LOCALBIB_DIR ;

/******************************************************************************

  Current database and its defaults

******************************************************************************/

char *Db_names ;		/* Buffer for database names */
				/* Feb 98: now allocated by read_db_avail(),
				   reallocd as required */
int Db_num ;			/* Number of available databases for user,
				   the number of pairs of strings in D_names */
Db Cdb ;			/* Single global database for now */

Db *Current_db ;		/* Pointer to (open) current database
				   (needed when more than one db per session)*/
char *Current_db_name ;		/* Pointed to name field of current database.
				   NULL if none open */
char Lim_names[256] ;		/* Buffer for limit names for current db */
int Lim_num ;			/* Number of limit names in Lim_names */
int Current_lim = 0 ;		/* Current limit bit setting */
char *Current_lim_name ;	/* Current limit name (=NULL if none set) */

int Current_ixnum = 0 ;

/* (Nov 99) Don't think the next 3 are used at present */
char Attr_names[256] ;		/* Buffer for search group names for curr db */
int Attr_num ;			/* Number of search group names in Lim_names */
char *Current_Attr_name ;	/* Current search group (=NULL if none set) */

/*****************************************************************************/

Gsl *Gsls[MAXGSLS] ;		/* Global gsls. By default Gsls[0] is the one
				   used in processing free keyword search
				   statements. They should be attached, by
				   name, to indexes (each index may have up
				   to three, each pointing to the appropriate
				   element of Gsls[]).

				   June 97: now (and for some time past)
				   only one per index. Gsls[0] is not
				   special.

				   March 99: now larger (currently 256) array
				   of pointers. */

int Num_gsls ;

/******************************************************************************

  Global BSS set records

******************************************************************************/

Setrec *Setrecs[MAXSETRECS + 1] ; /* The last one is a dummy used when combining
				     sets without saving */
int Num_Setrecs = 0 ;

/******************************************************************************

  Hion and Hioff, vt100, really only for debugging, it's up to an interface
  to do this sort of thing.

******************************************************************************/

char Hion[]  = "\033[7m" ;	
char Hioff[] = "\033[0m" ;


struct hugepos Hpos ;
