#include "bss.h"
#include "charclass.h"
#include "bss_errors.h"

#include <errno.h>
/*#include <sys/file.h>*/
#include <memory.h>
#ifndef NO_MMAP
#include <sys/mman.h>
#endif
#include <sys/stat.h>

/*#include <utmp.h> Not needed as no record_db() */

#define TRY_OPEN 0			/* For try_paths() */
#define TRY_FOPEN 1

#ifndef HOLD_DOCLENS
#define HOLD_DOCLENS 1
#endif

/*****************************************************************************

  bss_db.c Apr 92 SW

  Made up from db.c and parms.c

*****************************************************************************/




/* For decoding Db.field_types file */

struct field_type_name F_t_names[] = {
  {PERS, "PERS"},
  {CORP, "CORP"},
  {NAMES, "NAMES"},
  {TITLE, "TITLE"},
  {MAIN_TITLE, "MAIN_TITLE"},
  {SUBTITLE, "SUBTITLE"},
  {DEWEY, "DEWEY"},
  {SH, "SH"},
  {SH_SUBDIV, "SH_SUBDIV"},
  {TEXT, "TEXT"},
  {PHRASE, "PHRASE"},
  {LITERAL, "LITERAL"},
  {LITERAL_NC, "LITERAL_NC"},
  {NUMBERS, "NUMBERS"},
  {YEAR, "YEAR"},
  {UDC, "UDC"},
  {ANY, "ANY"},
  {NONE, "NONE"},
  {END, "END"},
} ;


/* For decoding literal names from search group parm files */

struct ext_reg_name Ext_reg_names[] = {
  {title_words, "title_words", 0},
  {name_words, "name_words", 0},
  {name_phrase, "name_phrase", 1},
  {words, "words", 0},
  {title_phrase, "title_phrase", 1},
  {name_and_inits, "name_and_inits", 0},
  {personal_name, "personal_name", 1},
  {corp_name_phrase, "corp_name_phrase", 1},
  {surnames, "surnames", 0},
  {phrase, "phrase", 1},
  {subject_heading, "subject_heading", 1},
  {Dewey, "Dewey", 1},
  {udc, "UDC", 1},
  {literal, "literal", 1},
  {literal_nc, "literal_nc", 1},
  {words3, "words3", 1},
  {lines, "lines", 1},
  {profile, "profile", 1},
  {no_reg, NULL, 1}
} ;

/******************************************************************************

  BOOL is_db_open(db) SW Jan 93
  
******************************************************************************/

BOOL

is_db_open(Db *db)
{
  if ( db == NULL || ! db->open ) return FALSE ;
  else return TRUE ;
}

/*****************************************************************************

  read_db_avail() new version Feb 92 SW

  The number of databases available is "returned" in the supplied argument
  num_db. 

  If num_db >= 0 the function returns a pointer to a static buffer containing
  a sequence of terminated strings.

  The strings are in pairs, the first of each pair being the database name
  and the second the database "display name".

  The buffer is terminated by an empty string.

  If num_db < 0 on return the return value is a NULL pointer.

  read_db_avail()

  The databases available to user are placed in the supplied buffer,
  each followed by its display_name, all as terminated strings. The buffer
  ends with two '\0' chars. Count of databases is returned.
  Error conditions: returns -1 if db_avail file can't be read. This will
  be fatal. There are warnings if a db is skipped because its parameter file
  can't be read or if there isn't enough room in the supplied buffer to store
  the db name and its description. If there are no db's available to this user
  that isn't in itself fatal, but the calling function should exit with a
  message to the user (function returns 0).

  Permissions are controlled as follows. Each line of the file
  Control_dir/"db_avail"  starts with a database name and this is followed
  by zero or more parameters, read from left to right. As soon as this user
  is resolved no more parms are read from this line. The parms control access
  by gid or login name. If parm is '*' this db is available to anyone.
  If parm is <gid> of current user this db is available to this user. If parm
  is -<gid> this db is not available to this user. If parm is the login name
  of the user this db is available. As soon as a parm has been read which
  applies to the user no more parms are read from the current line.

  Login name is determined by using cuserid.

  Feb 98: changes.

  To save opening all the db parm files the func now just reads names of
  available dbs from db_avail. For descriptions of all databases
  ("info databases") see bss_describe_all_databases().

  Determination of permission is also different. All groups to which
  user belongs are considered, and a negative group permission could not
  inhibit permission unless all user's groups occur preceded by minus sign.
  Additionally, "-<username>" may occur. Note that a number is assumed to
  be a gid.

  The use of cuserid() is unfortunate as at least in SPARC SVR4 this is
  calls funcs which are not in the static C-library.

  The static buffer Db_names is now allocated as required instead of being
  fixed at 2048 or whatever. Function is called from bss_startup() (only)
  but there's no reason why later calls should not be made if that ever
  becomes useful.

  
  May 98: no longer called from bss_startup(), but called every time such info
  is needed. The first call makes Db_names non-null, and every call returns Db_num.
  If Db_names is not NULL it is assumed to have been done, and does nothing
  except return Db_num. Note that it is not enough for a function to simply look
  at the value of Db_num. This is initially zero, but in the event that no
  databases are available Db_num will be zero but Db_names non-null.
  In the event of an error (malloc failure, can't find a db_avail file) the
  func returns -1.
*****************************************************************************/

int
read_db_avail(void)
{
  char fname[PATHNAME_MAX] ;
  char tempbuf[MAXLINE1] ;
  char parm[128] ;
  char tempdb[128] ;
  char *p, *op ;

  FILE *db_avail ;		/* General db_avail */
  int avail_fsize ;		/* Size of db_avail */
  /* FILE *db_file ;	(unused) Database description file */

  BOOL ok ;		
  int groupsleft ;
  int cnt = 0 ;			/* Db counter */
  int lenread ;
  int res ;
  int l ;
  BOOL no_perms = FALSE ;
  extern char Username[] ;	/* (globs) For username from cuserid */
  extern char *Db_names ;
  static char *func = "read_db_avail" ;

  if ( Db_names != NULL ) return Db_num ;
#ifdef NO_PERMS
  no_perms = TRUE ;
#endif
  sprintf(fname, "%s"D_C"%s", Control_dir, "db_avail") ;
  if ( (db_avail = bss_fopen(fname, "r")) == NULL ) {
    sprintf(Err_buf, "couldn't read %s", fname) ;
    sys_err(func) ;
    return -1 ;
  }
  /* Can't need more than filesize bytes to store names and even
     descriptions as well */
  avail_fsize = (int)ofsize((int)fileno(db_avail)) ;
  Db_names = bss_malloc(avail_fsize) ;
  if ( Db_names == NULL ) {
    prepare_error(SYS_NO_MEM_1, func, itoascii(avail_fsize)) ;
    return -1 ;
  }
  op = Db_names ;
  while ( (l = readlln(db_avail, tempbuf, MAXLINE1)) != EOF ) {
    if ( l == 0 || *tempbuf == '#' ) continue ;
    lenread = 0 ;
    res = sscanf(tempbuf, "%s%n", tempdb, &lenread) ;
    if ( res <= 0 ) continue ;
    /* Now have name of db */
    /* Allow root */
    if ( Uid == 0 || no_perms ) ok = 1 ;
    else {
      ok = 0 ;
      p = tempbuf + lenread ;
      groupsleft = N_groups ;
      while ( (res = sscanf(p, "%s%n", parm, &lenread)) == 1 ) {
	p += lenread ;
	if ( *parm == '*' ) {
	  ok = 1 ;
	  break ;
	}
	if ( strcmp(parm, Username) == 0 ) {
	  ok = 1 ;
	  break ;
	}
	if ( *parm == '-' && strcmp(parm + 1, Username) == 0 ) {
	  ok = 0 ;
	  break ;
	}
	if ( inw(abs(atoi(parm)), Groups, N_groups) ) {
	  ok = ( atoi(parm) > 0 ) ;
	  /* If all user's groups negative in parm line, exclude
	     even if followed by positive username */
	  if ( ! ok ) groupsleft-- ;
	  if ( ok || groupsleft == 0 ) break ;
	}
      }
    }

    if ( ok ) {
      sprintf(op, "%s", tempdb) ;
      op += strlen(op) + 1 ;
      cnt++ ;
    }
  }
  Db_names = bss_realloc(Db_names, op - Db_names ) ;
  bss_fclose(db_avail) ;
  return (Db_num = cnt) ;
}


/******************************************************************************

  open_bibfile()

  SW May 97, taken from open_db

  Opens if necessary and attaches to db struct's appropriate field

  Returns 1 if already done, 2 if bibfile is empty, 0 otherwise if successful,
  < 0 otherwise

******************************************************************************/

/******************************************************************************

  setup_recbuf()

  Called only from open_bibfile()

******************************************************************************/

static int
setup_recbuf(Db *db, int size)
{
  if ( db->recbuf != NULL ) return 1 ;
  if ( (db->recbuf = bss_malloc(size)) == NULL ) return -5 ;
  db->recbufsize = size ;
  return 0 ;
}

/******************************************************************************

  open_bibdir(db, auxnum)

  Apr 02: now looks first for .bibdrl (offset + length) if nf==1
  Also can now set has_drl and dir_recsize.

******************************************************************************/

int
open_bibdir(Db *db, int auxnum)	/* Called by recnum_to_addr() */
{
  char pathname[PATHNAME_MAX], endbit[512] ;
  int result = db->dirfd[auxnum] ;
  static char *func = "open_bibdir" ;

  if ( result > 0 ) return result ; /* Already open */
  if ( result < 0 ) return FD_CANT_OPEN ; /* Tried already and failed */
  result = -1 ;
  if ( auxnum == 0 ) {
    if ( db->nf == 1 && ! db->no_drl ) sprintf(endbit, "%s.bibdrl", db->name) ;
    else sprintf(endbit, "%s.bibdir", db->name) ;
  }
  else sprintf(endbit, "%s.%d.bibdir", db->name, auxnum) ;
  result = db->dirfd[auxnum] =
    try_paths(db->bib_dir[0], endbit, TRY_OPEN, NULL, O_RDONLY|O_BINARY,
	      pathname) ;
  if ( Dbg & D_DB ) fprintf(bss_syslog, "Tried bibdir %s, result %d\n",
			    pathname, result) ;
  if ( result > 0 ) {
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Found .bibdir file on %s\n", pathname) ;
  }
  else {
    if ( auxnum == 0 ) {
      sprintf(Err_buf, "can't open directory file %s", endbit) ;
      sys_err(func) ;
    }
    return FD_CANT_OPEN ;
  }
  return result ;
}


int
open_bibfile(Db *db, int vol)	/* Called by rdr_n() & rdr_extern() */
{
  char pathname[PATHNAME_MAX] ;
  static char *func = "open_bibfile" ;

  if ( db->m_sfd[vol] > 0 && db->recbuf != NULL ) return 1 ;
  if ( setup_recbuf(db, GOODREADSIZE) < 0 ) return -5 ;
  if ( db->m_sfd[vol] == 0 ) {
    if ( db->real_bibsize[vol] > (OFFSET_TYPE)0 ) {
      db->m_sfd[vol] = 
	try_paths(db->bib_dir[vol], db->bib_basename[vol], TRY_OPEN, NULL,
		  O_RDONLY|O_BINARY, pathname) ;
      if ( db->m_sfd[vol] >= 0 ) {
	if ( Dbg & D_DB )
	  fprintf(bss_syslog, "Opened bibfile vol %d %s\n", vol, pathname) ;
	if ( (db->i_db_type & DB_EXTERNAL_TYPE) && vol == 0 ) {
	  db->current_bibf = fdopen(db->m_sfd[vol], "r") ;
	  /* Check for NULL ? */
	}
      }
      else {
	sprintf(Err_buf, "can't open bibfile %s vol %d",
		db->bib_basename[vol], vol) ;
	sys_err(func) ;
	return -1 ;
      }
    }
    else return 2 ;		/* Empty bibfile */
  }
  return 0 ;
}

/******************************************************************************

  open_db(name, db_struct, applications)

  Opens the named database and attaches it to the supplied struct.

  Application names (dbflags) are no longer used (Sep 01) although the
  code is still there. Apart perhaps from DB_OPEN_SEARCH they appear to
  be fairly foolproof but are no longer needed as most facilities are
  loaded when required (indexes opened etc).

******************************************************************************/


static int 
calc_avedoclen(Db *, int, u_int) ;

int
open_db(char *name, Db *db, int flags)
/* flags: See defines.h for values, used to be 1: searching, 0: indexing,
   but now more values. */
{
  /* char pathname[PATHNAME_MAX] ; unused */
  /* char endbit[512] ; unused */
  int result ;
  int retcode = 0 ;
  /* Sg *g ; (unused) */
  static char func[] = "open_db" ;

  /* Read database description */

  if ( Dbg & D_DB )
    fprintf(bss_syslog, "Reading database parameter %s\n", name) ;
  result = read_db_desc(name, db ) ;
  if ( result != ST_OK ) {
    sprintf(Err_buf, "read_db_desc didn't return ST_OK") ;
    sys_err(func) ;
    retcode = result ;
    goto error_ret ;
  }

  db->big_n = db->nr ;

#ifdef USE_DBFLAGS
  if ( flags & DB_OPEN_SHOW ) {
    /* Set up recbuf and open bib files */
    for ( i = 0 ; i <= db->lastbibvol ; i++ ) {
      if ( open_bibfile(db, i) < 0 ) {
	retcode = -1 ;
	goto error_ret ;
      }
    }
    if ( Dbg & D_DB ) fprintf(bss_syslog, "Opened source file(s)\n") ;
  }
#endif /* USE_DBFLAGS */

  /* Note (Aug 2000) field_types unused */
  result = read_field_types(name, "field_types", db->field_types ) ;
  if ( result < 0 ) {
    sprintf(Err_buf, "read_field_types returned %d", result) ;
    sys_err(func) ;
    retcode = -1 ;
    goto error_ret ;
  }
  if ( Dbg & D_DB ) {
    if ( result == 0 ) fprintf(bss_syslog, "No field_types parameters\n") ;
    else fprintf(bss_syslog, "Read %d field_types parameters\n", result) ;
  }
  result = read_search_groups(db, db->name, db->groups) ;
  if ( result < 0 ) {
    sprintf(Err_buf, "read_search_groups returned %d < 0", result) ;
    sys_err(func) ;
    retcode = -1 ;
    goto error_ret ;
  }
  if ( (Dbg & D_DB) )
    fprintf(bss_syslog, "Read %d search groups parameter(s)\n", result) ;
  if ( db->has_lims ) {	/* Read limits parameters */
    result = read_limits(db) ;
    if ( result < 0 ) {
      sprintf(Err_buf, "read_limits failed") ;
      sys_err(func) ;
      retcode = -1 ;
      goto error_ret ;
    }
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Read limits parameters\n") ;
  }
  /* Open directory file, now do for all databases */
  /* if ( (retcode = open_bibdir(db)) < 0 ) goto error_ret ;*/

#ifdef USE_DBFLAGS
  if ( flags & DB_OPEN_SEARCH ) {
    /* Initialise index(es). Temporary code during changeover June 90 */
    for ( i = 0, g = db->groups ; i < db->num_search_groups ; i++, g++ ) {
      /* Redundant ix[i] assignment, done already by ixinit() */
      if ( (db->ix[g->ixnum] = ixinit(db, g)) == NULL ) {
	sprintf(Err_buf,
		"warning: can't open index number %d for attribute \"%s\"",
		g->ixnum, g->search_names) ;
	sys_err(func) ;
	/* No longer a fatal error (May 95) */
      }
      else {
	if ( Dbg & D_DB )
	  fprintf(bss_syslog, "Opened index #%d for attribute \"%s\"\n",
		  g->ixnum, g->search_names) ;
	if ( flags & DB_OPEN_BM ) {
	  if ( read_doclens(db, g->ixnum) < 0 ) {
	    retcode = -1 ;
	    goto error_ret ;
	  }
	  set_avedoclen(db, g->ixnum, calc_avedoclen(db, g->ixnum, 0)) ;
	}
      }
    }
  } /* (if flags & DB_OPEN_SEARCH) */

  if ( flags & DB_OPEN_PARAS ) {
    if ( prepare_paras(db, 1) < 0 ) {
      retcode = -1 ;
      goto error_ret ;
    }
  }

  if ( flags & DB_OPEN_LIMIT ) {
    if ( prepare_limits(db) < 0 /* || open_N_s(db) < 0 */ ) {
      retcode = -1 ;
      goto error_ret ;
    }
  }
#endif /* USE_DBFLAGS */  
  
  clear_current_displayset() ;
  /* (No longer done (Nov 99) Try append db open record to db_acct file 
     record_db(name, "open") ; */
  db->open = TRUE ;
  return 0 ;

error_ret:
  /* record_db(name, "failed") ; */
  return retcode ;
      
}

/*****************************************************************************

  close_db  Apr 92

  Same as close_db()  SW June 1990

  closes database & frees allocated memory

  See also bss_open_db()

  March 99: GSLs no longer freed. If same GSL used by next db will attach
  to already open copy.

*****************************************************************************/

int
close_db(Db *db, int flags)
{
  int i ;
  if ( db->parm_buffer == NULL ) return -1 ;
  for ( i = 0 ; i <= MAXNUMFLDS ; i++ )
    if ( db->dirfd[i] > 0 ) bss_close(db->dirfd[i]) ;
  if ( db->pardirf != NULL) bss_fclose(db->pardirf) ;
  for ( i = 0 ; i <= db->lastbibvol ; i++ )
    if ( db->m_sfd[i] > 0 ) bss_close(db->m_sfd[i]) ;
  if ( db->parafile != NULL ) bss_fclose(db->parafile) ;
  free_search_groups(db) ;
  if ( flags ) for ( i = 0 ; i < db->ni ; i++ ) ix_free(db->ix[i]) ;
  free_buf(db) ;		/* Frees various malloc'd fields */
  /* free_all_gsls() ; */
  db->open = FALSE ;
  /* record_db(Current_db_name, "close") ; (Nov 99) no longer done */
  Current_db_name = NULL ;
  if ( Dbg & D_DB ) fprintf(bss_syslog, "Closed database\n") ;
  return 0 ;
}

/******************************************************************************

  
******************************************************************************/


/******************************************************************************

  open_N_s(db)

  Opens the N_s file, returns 0 if OK else -

  Sept 96: no longer used. See calc_bign().

******************************************************************************/

/******************************************************************************

  calc_bign()  SW Sept 96

  Calculates big N for the given limit mask by going through the limits
  array if it exists. Assumes check_limit() was OK.

******************************************************************************/

int
calc_bign(Db *db, u_int limit)
{
  register int N, j ;
  u_short l = limit ;
  if ( limit == 0 ) return db->nr ;
  for ( j = 1, N = 0 ; j <= db->nr ; j++ )
    if ( Sats_limit(j, l) ) N++ ;
  if ( limit == db->current_limit ) set_bign(db, N) ;
  return N ;
}

void
set_bign(Db *db, int N)
{
  db->big_n = N ;
}

int
get_bign(Db *db, u_int limit)
{
  u_short l = limit ;
  int N ;
  if ( l == db->current_limit ) N = db->big_n ;
  else N = calc_bign(db, limit) ;
  return N ;
}

/*****************************************************************************

  read_field_types

  Read field types file into array of FIELD_TYPE
  The file format is <fdnum> <type> ... 
  The type name "END" is a dummy and must not occur in the file.
  Any fields not in file will be given type NONE

*****************************************************************************/

int
read_field_types(char *db_name, char *suffix, FIELD_TYPE types[])
{
  FILE * f ;		  /* file pointer for parameter file */
  int parmcount = 0 ;
  struct field_type_name *ftn = F_t_names ;
  int i, fdnum ;
  int result ;
  int found ;
  char fname[PATHNAME_MAX] ;	/* For full pathname */
  char name[128] ;

  /* Read file */
  sprintf(fname, "%s"D_C"%s.%s", Control_dir, db_name, suffix) ;
  f = bss_fopen(fname, "r") ;
  if ( f == NULL ) return 0 ;
  for ( i = 0 ; i <= MAXNUMFLDS ; i++ ) types[i] = NONE ;
  while ( (result = fscanf(f, "%d %s", &fdnum, name)) == 2 ) {
    if ( fdnum > MAXNUMFLDS || fdnum < 0 ) break ; /* Assume end */
    for ( found = 0, ftn = F_t_names ; ftn->type != END ; ftn++ ) {
      if ( strcasecmp(name, ftn->name) == 0 ) {
	found = 1 ;
	break ;
      }
    }
    if ( ! found ) {
      fprintf(Err_file, "Unknown field type name %s in %s\n",
	      name, fname) ;
      return -1 ;
    }
    parmcount++ ;
    types[fdnum] = ftn->type ;
  }
  bss_fclose(f) ;
  return parmcount ;
}

/*****************************************************************************

  read_search_groups

  Read search groups file into array of Search_group structs
  For file format see parms.h

  Returns - on error or number of search group parameters read
  (may be zero (July 97) -- doesn't bother if db->ni = 0)

  Feb 98: now three args. If db is not NULL behaviour is more fussy.
  If db is NULL just reading for an "info" command.

*****************************************************************************/

int
read_search_groups(Db *db, char *dbname, Sg *groups)
{
  FILE * f ;			/* file pointer for parameter file */
  int parmcount = 0 ;
  int i ;
  int result ;
  /* int found ; unused */
  char *p ;
  char ch ;
  char fname[PATHNAME_MAX] ;		/* For full pathname */
  char names[NNAME_MAX] ;	/* For search group names */
  int lname ;
  char rubbish[256] ;
  Sg *g = groups, *h ;
  int groupnum = 0 ;

  int dummy, ixnum ;
  /* struct ext_reg_name *ern ; unused */
  int fd, dodgy_factor ;	/* Field numbers and their weight adjustment
				   factor */
  int numfields ;

  static char func[] = "read_search_groups" ;

  if ( db != NULL && db->ni == 0 ) {
    if ( Dbg & D_DB )
      fprintf(bss_syslog,
	      "No indexes, not looking for search_groups parms\n") ;
    return 0 ;
  }
  /* Open file */
  sprintf(fname, "%s"D_C"%s.%s", Control_dir, dbname, "search_groups") ;
  f = bss_fopen(fname, "r") ;
  if ( f == NULL ) {
    sprintf(Err_buf, "can't open search groups file %s", fname) ;
    sys_err(func) ; 
    return -1 ;
  }

  while ( 1 ) {
    if ( parmcount >= BSS_MAXIX ) {
      sprintf(Err_buf,
	      "warning: too many lines in search_groups file, ignoring") ;
      sys_err(func) ;
      result = 0 ;
      break ;
    }
    result = fscanf(f, "%s", names) ;
    if ( result == EOF ) break ;
    if ( *names == '#' ) {
      fgets(rubbish, 255, f) ;
      continue ;
    }
    result += fscanf(f, "%d%d", &dummy, &ixnum) ;
    if ( result != 3 ) {
      sprintf(Err_buf, "incomplete parameter set") ;
      sys_err(func) ;
      result = -2 ;
      break ;
    }
    if ( (lname = strlen(names)) > NNAME_MAX - 1 ) {
      sprintf(Err_buf, "search group names too long (%d), must be < %d",
	      lname, NNAME_MAX) ;
      sys_err(func) ;
      result = -3 ;
      break ;
    }
    for ( h = groups ; h < g ; h++ ) {
      if ( ixnum == h->ixnum ) {
	sprintf(Err_buf, "duplicate parameter record for index %d",
		ixnum) ;
	sys_err(func) ;
	result = -3 ;
	break ;
      }
    }
    if (result == -3) break ;

    g->db = db ;		/* (Feb 99) */
    p = g->search_names = bss_malloc(strlen(names) + 2) ;
    strcpy(p, names) ;
    while ( (ch = *p) ) {	/* Replace '|' by '\0' */
      if ( ch == '|' || ch == ':' || ch == ',') *p = '\0' ;
      p++ ;
    }
    *++p = '\0' ;		/* Two \0 on end terminates string of strings*/

    g->dummy = dummy ; g->ixnum = ixnum ; 
    g->itype = db->itype[g->ixnum] ;
    result = fscanf(f, "%s", names) ;
    if ( (lname = strlen(names)) >= NNAME_MAX ) {
      sprintf(Err_buf,
      "search group extraction regime name too long (%d), must be < %d",
	      lname, NNAME_MAX) ;
      sys_err(func) ;
      result = -3 ;
      break ;
    }
    g->reg_name = bss_strdup(names) ;
    result += fscanf(f, "%s", names) ;
    if ( (lname = strlen(names)) >= NNAME_MAX ) {
      sprintf(Err_buf,
      "search group stem function name too long (%d), must be < %d",
	      lname, NNAME_MAX) ;
      sys_err(func) ;
      result = -3 ;
      break ;
    }
    g->stem_name = bss_strdup(names) ;
    result += fscanf(f, "%s", names) ;
    if ( (lname = strlen(names)) >= NNAME_MAX ) {
      sprintf(Err_buf,
      "search group gsl filename too long (%d), must be < %d",
	      lname, NNAME_MAX) ;
      sys_err(func) ;
      result = -3 ;
      break ;
    }
    g->gsl_name = bss_strdup(names) ;
    if ( result != 3 ) {
      sprintf(Err_buf, "incomplete parameter set") ;
      sys_err(func) ;
      result = -2 ;
      break ;
    }
    /* Check reg_name */
    if ( ! (g->reg = check_ext_reg(g->reg_name)) ) {
      sprintf(Err_buf, "unknown or invalid extraction regime %s", g->reg_name) ;
      sys_err(func) ;
      result = -3 ;
      break ;
    }
    /* Decode stem function name */
    if ( ! (g->stem_fn = check_stemfn(g->stem_name)) ) {
      sprintf(Err_buf, "unknown stem function %s", g->stem_name) ;
      sys_err(func) ;
      result = -3 ;
      break ;
    }

    /* Check gsl file if for real */
    /* Removed Nov 98 */
  
    /* Do field numbers */
    for ( i = 0, fd = 0, numfields = 0 ; i < MAXNUMFLDS && fd >= 0 ; i++ ) {
      result = fscanf(f, "%d", &fd) ;
      g->fields[i] = fd ;
      if ( fd < 0 || result != 1 ) break ;
      result = fscanf(f, "%d", &dodgy_factor) ;
      if ( dodgy_factor >= 0 ) {
	g->field_values[fd] = dodgy_factor ;
	numfields++ ;
      }
      else break ;
    }
    if ( numfields < 1 || result == 0 || fd > 0 || dodgy_factor < 0 ) {
      sprintf(Err_buf,
	      "incomplete parameter set (field numbers & values)\n") ;
      sprintf(Err_buf + strlen(Err_buf),
	      "Last parms read: fd=%d, fd_value=%d",
	      fd, dodgy_factor) ;
      sys_err(func) ;
      result = -2 ;
      break ;
    }
    if ( db != NULL && ixnum >= db->ni ) {
      if ( db->ni == 1 )
	sprintf(Err_buf,
		"ignoring search group parameter for index number %d, only 1 index",
		ixnum) ;
      else
	sprintf(Err_buf,
		"ignoring search group parameter for index number %d, only %d indexes",
		ixnum, db->ni) ;
      sys_err(func) ;
      continue ;
    }
    g->number = groupnum++ ;	/* (Oct 93) */
    g->status |= SG_READ_OK ;
    parmcount++ ;
    g++ ;
  }

  bss_fclose(f) ;
  
  if ( parmcount < 1 || (result != EOF && result < 0) ) return -1 ;
  if ( db != NULL ) db->num_search_groups = parmcount ;
  return parmcount ;
}

int
free_search_group(Sg *sg)
{
  if ( sg == NULL ) return 3 ;	/* Nothing to do */
  if ( sg->search_names != NULL ) bss_free(sg->search_names) ;
  if ( sg->reg_name != NULL ) bss_free(sg->reg_name) ;
  if ( sg->stem_name != NULL ) bss_free(sg->stem_name) ;
  if ( sg->gsl_name != NULL ) bss_free(sg->gsl_name) ;
  memset((void *)sg, 0, sizeof(Sg)) ;
  return 0 ;
}

int
free_search_groups(Db *db)
{
  int j ;
  if ( db == NULL ) return 0 ;
  for ( j = 0 ; j < db->num_search_groups ; j++ ) {
    free_search_group(&db->groups[j]) ;
  }
  return j ;
}

/*****************************************************************************

  find_search_group etc

  was find_search_name(word, length, names)  SW May 1991

  Functions for finding search group parms by name and by beast and index

  Searches array of search_names for word. Returns search_names struct if
  found else NULL. Used by parse() (subjsrch.c).

  27 Jan 92: mod as read_search_names() (qv)
  4 June 92: now returns Search_group struct or NULL if not found or
  imperfect (SG.STATUS == SG_UNSET).

*****************************************************************************/

Sg *
find_search_group_by_name(Db *db, char *attr)
{
  int i ;
  BOOL found ;
  Sg *sg ;
  char buf[NNAME_MAX] ;

  if ( db == NULL ) return NULL ;
  sg = db->groups ;
  if ( attr == NULL || strcasecmp(attr, "default") == 0 ) {
    /* Then assume first search group */
    if ( sg->status == SG_UNSET ) sg = NULL ;
  }
  else {
    strcpy(buf, attr) ;
    for ( found = FALSE, i = 0 ;
	  i < db->num_search_groups ; sg++, i++ ) {
      if ( stringscan(buf, sg->search_names) ) {
	found = TRUE ;
	break ;
      }
    }
    if ( ! found ) sg = NULL ;
  }
  return sg ;
}

/******************************************************************************

find_search_group_by_value()

Probably unused (Dec 94). Using attribute names at top level.

******************************************************************************/

/******************************************************************************

  find_field_num(char *name)  SW June 01

  Returns field number corresponding to name, for current database.
  Returns -1 if no such name or no database open.

******************************************************************************/

int
find_field_num(char *fd_name)
{
  int fdnum ;
  if ( ! is_db_open(&Cdb) ) return -1 ;
  for ( fdnum = 1 ; fdnum <= Cdb.nf ; fdnum++ )
    if ( strcasecmp(Cdb.f_abbrevs[fdnum], fd_name) == 0 ) return fdnum ;
  return -1 ;
}

/******************************************************************************

  read_para_dir()  SW Apr 94

  May 97: now standard to have a separate 4-byte-record para dir

  Read the paragraph fields from the .dir file for a database into
  db's paraptrs array

  The entries are file or core offsets in the .par file.

  Returns 0 if OK, -1 if no memory, -2 if can't open file, -3 if file
  appears short

  See also read_dir()

******************************************************************************/

static int
read_para_dir(Db *db)
{
  FILE *f ;			/* March 97: see note below */
  int rec, i ;
  u_int *para_ptrs, *pp ;
  static char *func = "read_para_dir" ;

  if ( db->paraptrs != NULL ) return 0 ;
  if ( (f = db->pardirf) == NULL ) {
      sprintf(Err_buf, "Paragraph directory file not open") ;
      sys_err(func) ;
      return -2 ;
  }
  para_ptrs = (u_int *)bss_calloc(db->nr, sizeof(u_int)) ;
  if ( para_ptrs == NULL ) {
    mem_err(func, "paragraph pointers", db->nr * sizeof(u_int)) ;
    return -1 ;
  }
  rewind(f) ;
  for ( i = 0, pp = para_ptrs ; i < db->nr ; i++, pp++ ) {
#if HILO
    if ( ( rec = getw(f) ) == EOF ) {
#else
    if ( ( rec = getw_reverse(f) ) == EOF ) {
#endif
      sprintf(Err_buf,
	      "only %d recs in paragraph directory, should be %d", i, Cdb.nr) ;
      sys_err(func) ;
      return -3 ;
    }
    *pp = rec ;
  }
  db->paraptrs = para_ptrs ;
  if ( Dbg & D_DB )
    fprintf(bss_syslog, "Read para file pointers\n") ;
  return 0 ;
}
  
/******************************************************************************

  prepare_paras()  SW May 95

  Check state of paragraph info and prepare it if possible.

  Opens paragraph directory.
  If the argument "code" = 0 it opens the .par file. If code = 1 it reads
  paragraph file offsets into db->paraptrs, leaving the file open.
  if code = 2 it makes a memory image of the .par file (as well as reading
  the offsets into paraptrs), and closes the file.

  NB: code = 2 is unimplemented.

  Returns 1 + code value if OK, -2 if inappropriate database type,
  -1 if .par or .pardir files fail, or have already failed, to open or read.
  The non-zero values are stored in db->parstat.

  The -2 value is set up by read_db_desc() ;
  
******************************************************************************/

int
prepare_paras(Db *db, int code)
{
  char pathname[PATHNAME_MAX] ;
  char endbit[512] ;
  /* int retval ; (unused) */
  static char *func = "prepare_paras" ;

  if ( db->parstat < 0 ) return db->parstat ;
  if ( db->parstat >= 1 + code ) return db->parstat ;
  /* If code is 0 parstat 1 or 2 are OK but if code is 1 parstat must be 2 */
  /* First check or open paragraph directory */
  sprintf(endbit, "%s.pardir", db->name) ;
  db->pardirf = (FILE *)try_paths(db->bib_dir[0], endbit, TRY_FOPEN, "rb", 0,
				  pathname) ;
  if ( db->pardirf != NULL ) {
    if ( Dbg & (D_DB | D_PARAS) )
      fprintf(bss_syslog, "Found para dir on %s\n", pathname) ;
  }
  else {
    sprintf(Err_buf, "no paragraph directory") ;
    sys_err(func) ;
    return (db->parstat = -1) ;
  }
  sprintf(endbit, "%s.par", db->name) ;
  db->parafile = 
    (FILE *)try_paths(db->bib_dir[0], endbit, TRY_FOPEN, "rb", 0,
		      pathname) ;
  if ( Dbg & D_DB )
    if ( db->parafile != NULL )
      fprintf(bss_syslog, "Found .par file on %s\n", pathname) ;
  if ( db->parafile == NULL ) {
    sprintf(Err_buf, "can't open paragraph file %s", endbit) ;
    sys_err(func) ;
    return (db->parstat = -1) ;
  }
  else if ( Dbg & D_DB ) fprintf(bss_syslog, "Opened paragraph file\n") ;
  if ( code == 0 ) return (db->parstat = 1) ;
  if ( read_para_dir(db) < 0 ) return ( db->parstat = -1 );
  return (db->parstat = 1 + code) ;
}
  
/******************************************************************************

  prepare_limits()  SW Aug 94

  Check whether .lims has been read, and, if not, read it in (etc)

  Returns 0 if OK, 2 inappropriate database type or no limits,
  1 if already done,   < 0 on failure.

******************************************************************************/
     
int
prepare_limits(Db *db)
{
  int fd ;
  char buf[PATHNAME_MAX] ;
  int result ;
  int j ;
  static char *func = "prepare_limits" ;

  if ( ! db->has_lims )
    return 2 ;			/* Not appropriate */
  if ( db->lims != NULL ) return 1 ; /* Already done */
  sprintf(buf, "%s%s.lims", db->bib_dir[0], db->name) ;
  if ( (fd = bss_open(buf, O_RDONLY|O_BINARY)) < 0 ) {
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Can't find limits file %s\n", buf) ;
    sprintf(Err_buf, "warning: can't open %s, limits won't work", buf) ;
    sys_err(func) ;
    return -2 ;		/* Can't do it */
  }
  if ( Dbg & D_DB )
    fprintf(bss_syslog, "Opened limits file %s\n", buf) ;
  db->lims = (u_short *)bss_calloc(db->nr, LIMIT_MASK_SIZE) ;
  if ( db->lims != NULL ) {
    result = read(fd, db->lims, LIMIT_MASK_SIZE * db->nr) ;
    bss_close(fd) ;
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Read %d bytes from limits file\n", result) ;
    if ( result != LIMIT_MASK_SIZE * db->nr ) {
      sprintf(Err_buf,
 "warning: only read %d bytes from limits file but %d recs in Db",
	      result, db->nr) ;
      sys_err(func) ;
    }
#if ! HILO
    for ( j = 0 ; j < db->nr ; j++ ) db->lims[j] = reverse_16(db->lims[j]) ;
#endif
  }
  else {
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Can't allocate %d bytes for limits array",
	      LIMIT_MASK_SIZE * db->nr) ;
    sprintf(Err_buf, "can't allocate %d bytes for limits array",
	    LIMIT_MASK_SIZE * db->nr) ;
    sys_err(func) ;
    return -2 ;
  }
  return 0 ;
}

/******************************************************************************

  read_limits()  SW May 92

  Read the .limits parameter file for a database into an array of limits structs

  The limits structs are normally db.limits[]

  The file format is

    [<name> <bit>\n]

  where <name> is a character string without embedded white space of max
  length 15 and <bit> is 1, 2, 4, .. 32768

  The filename is <dbname>.limits

  March 95: now returns the number of limit records read, or - on error
  and a reasonable upper bound on possible limit values.

  Won't necessarily detect repeated limit values but can bum out if upper
  bound seems to go over 65535.

******************************************************************************/

int
read_limits(Db *db)
{
  struct limits *limits = db->limits ;
  FILE * f ;		  /* file pointer for parameter file */
  int parmcount = 0 ;
  int result ;
  char buf[256] ;
  char limname[NNAME_MAX] ;
  int bit ;
  static char *func = "read_limits" ;
  static char *helpmes =
    "limits file format: [<name> <bit><nl>] where strlen(name) < 16 && \
bit = 1, 2, 4, .., 32768" ;

  /* Read file */
  sprintf(buf, "%s"D_C"%s.%s", Control_dir, db->name, "limits") ;
  f = bss_fopen(buf, "rb") ;
  if ( f == NULL ) {
    sprintf(Err_buf, "can't read limits file %s", buf) ; 
    sys_err(func) ;
    return -1 ;
  }

  /* Should check for duplicate names and/or bit values */
  db->highest_lim = 0 ;
  while ( fgets(buf, 256, f) != NULL && parmcount < 16) {
    if ( *buf == '#' || *buf == '\0' ) continue ;
    result = sscanf(buf, "%s%d", limname, &bit) ;
    if ( result < 2 || strlen(limname) > 15 ||
	! ispower2(bit) || bit > 32768 ) {
      sprintf(Err_buf, "funny limits file\n%s", helpmes) ;
      sys_err(func) ;
      bss_fclose(f) ;
      return -1 ;
    }
    strcpy(limits->name, limname) ; limits->lim = bit ;
    if ( db->highest_lim > 65535 ) {
      sprintf(Err_buf, "funny limits file\n%s", helpmes) ;
      sys_err(func) ;
      bss_fclose(f) ;
      return -1 ;
    }      
    db->highest_lim += bit ;
    parmcount++ ;
    limits++ ;
  }
  bss_fclose(f) ;
  db->num_lims = parmcount ;
  return parmcount ;
}

/******************************************************************************

  find_limit() SW May 92

  Return limit bit corresponding to limit name, 0 if not found or db not open

******************************************************************************/

int
find_limit(char *limit_name)
{
  int i ;
  struct limits *lims = Cdb.limits ;

  if ( ! Cdb.open ) return 0 ;
  for ( i = 0 ; i < 16 && *lims->name != '\0' ; i++, lims++ )
    if ( strcmp(limit_name, lims->name) == 0 )
      return lims->lim ;
  return 0 ;
}

/******************************************************************************

  char *bss_limits_avail(&num_lims)  SW July 92

  returns the address of a list of limit names, NULL if no limits
  and places the number of limits available in num_lims

******************************************************************************/

char *
bss_limits_avail(int *num_lims)
{
  int i ;
  static char limnames[256] ;
  char *p = limnames ;

  if ( ! Cdb.open || ! Cdb.has_lims ) {
    *num_lims = 0 ;
    return NULL ;
  }
  for ( i = 0 ; i < 16 && Cdb.limits[i].lim ; i++ ) {
    sprintf(p, "%s", Cdb.limits[i].name) ;
    p += strlen(p) + 2 ;
  }
  *p = '\0' ;
  *num_lims = i ;
  return limnames ;
}
      
/******************************************************************************

  read_dir()  SW Apr 94

  Read the .dir file for a database into db's recptrs array

  The entries are record's file address / rec_mult

  Returns 0 if OK, -1 if no memory, -2 if can't open file, -3 if file
  appears short

  Believe not used at present (SW March 95)

  See also read_para_dir()

  NB Doesn't allow for 5-byte directory records

  ? Never been used. Removed March 98

******************************************************************************/
  
/*****************************************************************************

  Ixinit 6 July 87 - initialises an INDEX structure, allocates buffers and
		     reads PI. If Sx is nonzero does the same for its 
		     Soundex sub-index.

		     Returns address of index or NULL on error

  Previously in searchsubs.c		     
  Revision 15 Jan 88 - now creates index as well, attaches it to the db
  struct (May 95), & returns pointer to it     

  May 95: now harmless to call repeatedly, simply returns the index's
  address if already non-NULL. Thus it is harmless to call it on every
  lookup operation.

  Nov 96: have changed .oi file naming several times over the last year or
  so; now, if pathname in parameter ends in slash db->name is appended,
  otherwise the given name is used (in both cases adding .<ixnum>.oi).
  If you may want one or more volumes of it on Temp_dir directory alone
  is not enough except for the first volume: every subsequent volume must
  have an individual name (if there's more than one).

  May 97: now assuming (first) that every ix_stem is a pathname wanting
  only .<num>.?i.

  Primary and secondary files are canonically on the FIRST ix_stem for the
  index. Postings files use up to eight (I think). Not assuming vol numbers
  any more. Each file is tried for first on BSS_TEMPPATH then on LOCALBIB,
  (at present this defaults to . and we use links from TEMPPATH otherwise).

  If trying the full ixstem fails, then the database name is appended to
  the DIRNAME(ix_stem), and opening is retried. This may be discontinued
  when the new separate index parameters are introduced: we'll probably
  have separate ix_dir and ix_basename, as for the text files.

  March 99: now only called via check_index(&Db, &Sg), Sg must be already
  read and GSL set up (see check_attr()).

*****************************************************************************/
Ix *
ixinit(Db *db, Sg *sg) 
{
  BOOL sx ;			/* If true, would be a "soundex" attachment,
				   unused since 1986 or thereabouts */
  Ix *index ;
  FILE *f ;
  char *bp, **pp ;

  int psize ;			/* Length of PI file */
  int c, nrecs ;
  /* int found ; unused */
  int j ;
  int status ;
  /* int result ; unused */
  char endbit[PATHNAME_MAX] ;
  char pathname[PATHNAME_MAX] ;
  static char *func = "ixinit" ;

  if ( (index = sg->ix) != NULL ) /* Already done? */
    return index ;
  
  if ( (index = (Ix *)bss_calloc(1, sizeof(Ix))) == NULL ) {
    mem_err(func, "index", sizeof(Ix)) ;
    status = ST_FAIL_MEM ;
    goto ix_error ;
  }

  sx = psize = nrecs = 0 ;
  status = ST_OK ;

  index->type = sg->itype ;
  index->schunksize = SCHUNKSIZE ; /* Temp March 02, until this is in a
				      parameter */
  switch (index->type) {
    /* Nearly all obsolete and deleted Jan 96 */
  case 8:
  case 6:
  case 4:
    index->pstgtype = PTYPE12 ;
    break ;
  case 9:
  case 7:
  case 5:
    index->pstgtype = PTYPE13 ;
    break ;
  case 10:
    index->pstgtype = PTYPE14 ;
    break ;
  case 20:
    index->pstgtype = PTYPE21 ;
    break ;
  case 12:
    index->pstgtype = PTYPE15 ;
    break ;
  case 13:
    index->pstgtype = PTYPE16 ;
    break ;
  case 11:
    index->pstgtype = PTYPE17 ;
    break ;
  default:
    sprintf(Err_buf, "Can't handle index type %d", index->type) ;
    sys_err(func) ;
    goto ix_error ;
    break ;
  } /* (switch) */
  index->num = sg->ixnum ;
  index->sg = sg ;
  index->attr_num = sg->number ;

  if (sx) index->sxq = 1 ;
  index->last_ixvol = db->last_ixvol[index->num] ;
  for ( j = 0 ; j <= index->last_ixvol ; j++ ) 
    index->ix_volsize[j] = db->ix_volsize[index->num][j] ;

  /* Read and setup primary index */
  sprintf(endbit, "%s.%d.pi",
	  lastslash(db->ix_stem[index->num][0]), index->num) ;
  f =
    (FILE *)try_paths(dirnameh(db->ix_stem[index->num][0]), endbit, TRY_FOPEN,
		      "rb", 0, pathname) ;
  if ( f == NULL ) {
    sprintf(endbit, "%s.%d.pi", db->name, index->num) ;
    f = (FILE *)
      try_paths(dirnameh(db->ix_stem[index->num][0]), endbit, TRY_FOPEN,
		"rb", 0, pathname) ;
  }
  if ( f == NULL ) {
    sprintf(Err_buf, "can't open primary index %s", endbit) ;
    sys_err(func) ;
    status = ST_FAIL_FOPEN ;
    goto ix_error ;
  }
  psize = getw(f) ;
  index->numchunks = getw(f) ;
  if ( ferror(f) || psize <= 0 || index->numchunks <= 0 ) {
    sprintf(Err_buf, "can't determine primary index size %s", pathname) ;
    sys_err(func) ;
    status = ST_FAIL_READ ;
    goto ix_error ;
  }
  else {
    if ( (bp = index->pibuf = bss_malloc(psize)) == NULL ||
  (pp = index->pptrs = (char **)bss_malloc(index->numchunks * sizeof(char *)))
	== NULL ) {
      mem_err(func, "primary index",
	      psize + index->numchunks * sizeof(char *));
      status = ST_FAIL_MEM ;
      goto ix_error ;
    }

    while ( (c = getc(f)) != EOF ) {
      *pp++ = bp ;
      *bp++ = c ;
      if ( fread(bp, (size_t)c, (size_t)1, f) != 1 ) {
	sprintf(Err_buf, "read fail on primary index %s", pathname) ;
	sys_err(func) ;
	bss_fclose(f) ;
	status = ST_FAIL_READ ;
	goto ix_error ;
      }
      bp += c ;
      nrecs++ ;
    }

    bss_fclose(f) ;
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Read %d recs from .pi file\n", nrecs) ;

    /* Open SI */
    sprintf(endbit, "%s.%d.si",
	    lastslash(db->ix_stem[index->num][0]), index->num) ;
    index->sifd =
      try_paths(dirnameh(db->ix_stem[index->num][0]), endbit, TRY_OPEN, NULL,
		O_RDONLY|O_BINARY, pathname) ;

    if ( index->sifd < 0 ) {
      sprintf(endbit, "%s.%d.si", db->name, index->num) ;
      index->sifd = 
	try_paths(dirnameh(db->ix_stem[index->num][0]), endbit, TRY_OPEN,
		  NULL, O_RDONLY|O_BINARY, pathname) ;
    }
    if ( index->sifd < 0 ) {
      fprintf(Err_file, "Can't open secondary index %s\n", endbit) ;
      status = ST_FAIL_FOPEN ;
      goto ix_error ;
    }
    if ( (index->sibuf = (char *)bss_calloc(index->schunksize, 1)) == NULL ) {
      sprintf(Err_buf,
	      "unable to allocate %d bytes for secondary index read buffer",
	      index->schunksize) ;
      sys_err(func) ;
      status = ST_FAIL_MEM ;
      goto ix_error ;
    }
    if ( Dbg & D_DB ) fprintf(bss_syslog, "Opened .si file\n") ;
    /* Open postings file(s) */
    for ( j = 0 ; j <= index->last_ixvol ; j++ ) {
      sprintf(endbit, "%s.%d.oi",
	      lastslash(db->ix_stem[index->num][j]), index->num) ;
      /* Had [num][0] in here, I think it should be [num][j] so changed
	 (Aug 97) */
      index->ofp[j] = 
	(FILE *)try_paths(dirnameh(db->ix_stem[index->num][j]), endbit,
			  TRY_FOPEN, "rb", 0, pathname) ;
      if ( index->ofp[j] == NULL ) {
	sprintf(Err_buf, "can't open postings file %s", endbit) ;
	sys_err(func) ;
	status = ST_FAIL_FOPEN ;
	goto ix_error ;
      }	
      else {
	index->pfname[j] = bss_strdup(pathname) ; /* Error check ? */
	if ( Dbg & D_DB )
	  fprintf(bss_syslog,
		  "Opened vol %d of pstgs file %s\n", j, pathname) ;
      }
    }

    for ( j = 0 ; j <= index->last_ixvol ; j++ ) {
      index->ofd[j] = fileno(index->ofp[j]) ;
      index->pfsize[j] = lseek(index->ofd[j], (OFFSET_TYPE)0, SEEK_END) ;
      if ( Dbg & D_DB ) {
	fprintf(bss_syslog, "Index %d, ofd[%d] = %d\n",
		index->num, j, index->ofd[j]) ;
#ifdef HAVE_LL
#ifdef _WIN32
	fprintf(bss_syslog, "Filesize %I64d\n", (LL_TYPE)index->pfsize[j]) ;
#else
	fprintf(bss_syslog, "Filesize %lld\n", (LL_TYPE)index->pfsize[j]) ;
#endif
#else
	fprintf(bss_syslog, "Filesize %d\n", (long)index->pfsize[j]) ;
#endif
      }
    }
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "Opened postings file(s)\n") ;
  } /* (PI nonempty) */

  /* Read status file */
  sprintf(endbit, "%s.%d.st",
	  lastslash(db->ix_stem[index->num][0]), index->num) ;
  f = 
    (FILE *)try_paths(dirnameh(db->ix_stem[index->num][0]), endbit, TRY_FOPEN,
		      "rb", 0, pathname) ;
  if ( f == NULL ) {
    sprintf(endbit, "%s.%d.st", db->name, index->num) ;
    f = 
      (FILE *)try_paths(dirnameh(db->ix_stem[index->num][0]), endbit, TRY_FOPEN,
		"rb", 0, pathname) ;
    if ( f == NULL ) {
      fprintf(bss_syslog, "Can't open index status file %s\n", endbit) ;
      index->cttf = index->tnp = index->tnt = index->tottermlen = -1 ;
    }
  }
  else {
    fscanf(f, "%Ld %*s %Ld %*s %d %*s %u",
	   &index->cttf, &index->tnp, &index->tnt, &index->tottermlen) ;
    bss_fclose(f) ;
  }
  if ( Dbg & D_DB ) 
    fprintf(stderr, "cttf=%lld tnp=%lld tnt=%d tot_termlen=%u\n",
	    index->cttf, index->tnp, index->tnt, index->tottermlen) ;
  index->gsl = sg->gsl ;
  index->cchunk = -1 ; /* Impossible value */
  status = ST_OK ;
  db->ix[index->num] = index ;
  db->index_status[index->num] = index->status = status ;
  return(index) ;

 ix_error:
  db->index_status[index->num] = status ;
  ix_free(index) ;
  return NULL ;
}

/*****************************************************************************

  ix_free()  June 90 SW

  Frees memory allocated for an index by ixinit() (qv)

  Returns 0 if OK, negative if index appears not to have been initialised
  (index.status = 0) or if index is NULL

*****************************************************************************/

int
ix_free(Ix *index)
{
  int j ;

  if ( index == NULL || index->status == 0 ) return -1 ;
  if ( index->sxq ) ix_free(index->sx) ;
  for ( j = 0 ; j <= index->last_ixvol ; j++ ) {
    if ( index->ofp[j] != NULL ) bss_fclose(index->ofp[j]) ;
    if ( index->pfname[j] != NULL ) bss_free(index->pfname[j]) ;
  }
  if ( index->sifd != 0 ) bss_close(index->sifd) ;
  if ( index->sibuf != NULL ) bss_free(index->sibuf) ;
  if ( index->pptrs != NULL ) bss_free(index->pptrs) ;
  if ( index->pibuf != NULL ) bss_free(index->pibuf) ;
  if ( index->doclens != NULL ) {
    if ( index->has_doclens == 2 ) {
      bss_free(index->doclens) ;
      if ( Dbg & D_DB )
	fprintf(bss_syslog, "Freed index %d's doclens buffer\n", index->num) ;
    }
#ifndef NO_MMAP
    else if ( index->has_doclens == 1 ) {
      bss_munmap(index->doclens, iceil((Cdb.nr * sizeof(int)), Pagesize)) ;
      if ( Dbg & D_DB )
	fprintf(bss_syslog, "Unmapped index %d's doclens buffer\n",
		index->num) ;
    }
#endif
  }
  for ( j = 0 ; j < MAXSTEMFILES ; j++ ) {
    if ( index->stemfiles[j] != NULL ) bss_fclose(index->stemfiles[j]) ;
  }    
  bss_free(index) ;
  if ( Dbg & D_DB ) fprintf(bss_syslog, "Freed index buffers\n") ;
  return 0 ;
}

/******************************************************************************

  read_doclens(db, ixnum)   SW May 95

  Removed from ixinit(). Is done only if needed by bss_do_combine() or called for
  by the open_db() flags argument.

  Returns 0 if OK, 2 if already done, -2 if can't find or read doclens file
  (this means BM ops treat doclen as avedoclen for all docs), -1 if fatal
  (out of mem).

  If no error and it's not already been done sets current avedoclen to
  the value corresponding to the current limit.

  Mod May 98 to set and use has_doclens.

******************************************************************************/

int
read_doclens(Db *db, int ixnum)
{
  Ix *ix = db->ix[ixnum] ;
  int fd ;
  char pathname[PATHNAME_MAX] ;
  char endbit[PATHNAME_MAX] ;
#ifdef NO_MMAP
  int nread ;
#endif
  static char *func = "read_doclens" ;

  if ( ix->has_doclens > 0 ) return 2 ;	/* Already done */
  else if ( ix->has_doclens == -1 ) return -2 ; /* Already tried */
  else {
    sprintf(endbit, "%s.%d.dlens", lastslash(db->ix_stem[ixnum][0]), ixnum) ;
    fd =
      try_paths(dirnameh(db->ix_stem[ixnum][0]), endbit, TRY_OPEN, NULL,
		O_RDONLY|O_BINARY, pathname) ;
    if ( fd < 0 ) {
      sprintf(endbit, "%s.%d.dlens", db->name, ixnum) ;
      fd =
	try_paths(dirnameh(db->ix_stem[ixnum][0]), endbit, TRY_OPEN, NULL,
		  O_RDONLY|O_BINARY, pathname) ;
    }
    if ( fd < 0 ) {
      ix->has_doclens = -1 ;
      sprintf(Err_buf,
	      "warning: can't open .doclens file %s for index %d\n",
	      endbit, ixnum) ;
      sys_err(func) ;
      return -2 ;
    }
    else {
      if ( Dbg & D_DB ) fprintf(bss_syslog, "Opened doclens file %s\n",
				pathname) ;

#ifndef NO_MMAP
      ix->doclens =
	(int *)bss_mmap(0, iceil((db->nr * sizeof(int)), Pagesize),
			PROT_READ/*|PROT_WRITE*/, MAP_SHARED, fd,
			(OFFSET_TYPE)0, HOLD_DOCLENS) ;
      if ( ix->doclens == (int *)-1 ) {
	internal_err(func, "can't map memory for doclens array") ;
	ix->doclens = (int *)NULL ;
	ix->has_doclens = -1 ;
	bss_close(fd) ;
	return -1 ;
      }
      ix->has_doclens = 1 ;	/* mmap */
      if ( Dbg & D_DB )
	fprintf(bss_syslog, "Mapped doclens file\n") ;
#else
      ix->doclens = (int *)bss_calloc(sizeof(int), db->nr) ;
      if ( ix->doclens == (int *)NULL ) {
	internal_err(func, "can't allocate memory for doclens array") ;
	ix->has_doclens = -1 ;
	bss_close(fd) ;
	return -1 ;
      }
      nread = read(fd, ix->doclens, db->nr * sizeof(int)) ;
      if ( nread < db->nr * sizeof(int) ) {
	internal_err(func, "apparently incomplete doclens file") ;
	ix->has_doclens = -1 ;
	bss_close(fd) ;
	return -1 ;
      }
      ix->has_doclens = 2 ;	/* malloc */
      if ( Dbg & D_DB )
	fprintf(bss_syslog, "Read doclens file\n") ;
#endif
      bss_close(fd) ;
    }
    set_avedoclen(db, ixnum, get_avedoclen(db, ixnum, db->current_limit)) ;
  }
  return 0 ;
}

/******************************************************************************

  get_doclen  SW May 97, Feb 99

  See also bss_get_doclen(), get_doclen_pstg()

******************************************************************************/

int
get_doclen(Db *db, int ixnum, int irn)
{
  return db->ix[ixnum]->doclens[irn - 1] ;
}

/******************************************************************************

  open_stemfile(ixnum, stem_function_name)   SW May 95

  Calling function handles all errors except can't open the file.

******************************************************************************/

int
open_stemfile(Db *db, int ixnum, char *fn)
{
  Ix *ix = db->ix[ixnum] ;
  FILE *f ;
  char fname[PATHNAME_MAX] ;
  int j ;

  /* Is it open already? */
  for ( j = 0 ; j < MAXSTEMFILES && ix->stemfiles[j] != NULL ; j++ )
    if ( strcmp(ix->stemfns[j], fn) == 0 ) /* We have it */
      return 2 ;
  /* Haven't got it. If room in the tables, open it. */
  if ( j >= MAXSTEMFILES ) return -2 ; /* No room */
  sprintf(fname, "%s"D_C"%s.%d.stemfile.%s",
	  dirnameh(db->ix_stem[ixnum][0]), db->name, ixnum, fn) ;
  if ( (f = bss_fopen(fname, "r")) == NULL ) {
    sprintf(fname, "%s"D_C"%s.v0.%d.stemfile.%s",
	    dirnameh(db->ix_stem[ixnum][0]), db->name, ixnum, fn) ;
    f = bss_fopen(fname, "r") ;
  }
  if ( f == NULL )
    return -1 ;			/* No such file probably, calling function
				   handles error, may be no such function */
  ix->stemfiles[j] = f ;
  /*  ix->stemnames[j] = Have to look for it in the Stem_fns */
  return 0 ;
}
				   
/******************************************************************************

  New version, not using avedoclen file   SW Sep 96

  Returns -1 if no doclens.

  Cf calc_ and set_ bign()

  June 00: return zero if no docs (N == 0)

******************************************************************************/

static int
calc_avedoclen(Db *db, int ixnum, u_int limit)
{
  Ix *ix = db->ix[ixnum] ;
  int *doclens = ix->doclens ;
  double N  = (double)get_bign(db, limit) ;
  u_short l = limit ;
  register double tl = 0.0 ;
  int avedoclen ;
  register int j ;

  if ( doclens == NULL ) {
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "calc_avedoclen, NULL doclens\n") ;
    return -1 ;
  }
  if ( N == 0 ) return 0 ;
  if ( limit == 0 )
    for ( j = 0 ; j < N ; j++, doclens++ ) tl += *doclens ;
  else {
    for ( j = 1 ; j <= db->nr ; j++ ) 
      if ( Sats_limit(j, l) ) tl += doclens[j] ;
  }
  avedoclen = tl / N ;		/* Is this right for nonzero limit? */
  if ( Dbg & D_DB )
    fprintf(bss_syslog, "Avedoclen for index %d, limit %d is %d\n",
	    ixnum, (int)limit, avedoclen) ;
  return avedoclen ;
}
  
int
get_avedoclen(Db *db, int ixnum, u_int limit)
{
  int avedoclen = 0 ;
  Ix *ix = db->ix[ixnum] ;

  if ( ix != NULL ) {
    if ( limit == db->current_limit && ix->current_avedoclen > 0 )
      avedoclen = ix->current_avedoclen ;
    else avedoclen = calc_avedoclen(db, ixnum, limit) ;
    if ( limit == db->current_limit ) ix->current_avedoclen = avedoclen ;
  }
  return avedoclen ;
}

/******************************************************************************

  (Feb 00) get_cttf isn't needed -- see bss_get_avedoclen()

******************************************************************************/


#ifdef HAVE_LL
LL_TYPE
get_cttf(Db *db, int ixnum, u_int limit)
{
  LL_TYPE adl = get_avedoclen(db, ixnum, limit) ;
  LL_TYPE N ;
  if ( adl < 0 ) return -1 ;
  N = calc_bign(db, limit) ;
  return N * adl ;
}
#else
double
get_cttf(Db *db, int ixnum, u_int limit)
{
  double adl = get_avedoclen(db, ixnum, limit) ;
  double N ;
  if ( adl < 0 ) return -1 ;
  N = calc_bign(db, limit) ;
  return N * adl ;
}
#endif

/******************************************************************************

  set_avedoclen(db, ixnum, avedoclen)    SW Sep 96

******************************************************************************/

void
set_avedoclen(Db *db, int ixnum, int avedoclen)
{
  Ix *ix = db->ix[ixnum] ;

  ix->current_avedoclen = avedoclen ;
  if ( Dbg & D_DB )
    fprintf(bss_syslog, "Index %d, current avedoclen set to %d\n",
	    ixnum, ix->current_avedoclen) ;
}

void
set_avedoclens(Db *db, u_int limit)
{
  Sg *g = db->groups ;
  int j ;
  int avedoclen ;
  for ( j = 0 ; j < db->num_search_groups ; j++, g++ ) {
    if ( db->ix[g->ixnum] == NULL ) {
      if ( Dbg & D_DB )
	fprintf(bss_syslog,
		"Can't set avedoclen for index %d, index not open\n",
		g->ixnum) ;
    }
    else {
      avedoclen = get_avedoclen(db, g->ixnum, limit) ;
      set_avedoclen(db, g->ixnum, avedoclen) ;
    }
  }
}

/******************************************************************************

  db_availq()  SW May 92

  Check whether database given by name is available to user
  Returns 1 if database is avail else 0

  Needs read_db_avail() first (see bss_startup())

******************************************************************************/

int
db_availq(char *dbname)
{
  int j ;
  char *p ;

  if ( *Username == '\0' ) return 0 ;
  if ( read_db_avail() <= 0 ) {
    return 0 ;
  }

  for ( j = 0, p = Db_names ; j < Db_num ; j++ ) {
    if ( strcmp(p, dbname) == 0 ) return 1 ;
    p += strlen(p) + 1 ;
  }    
  return 0 ;
}

/******************************************************************************

  db_show(various_things)

  Displays info about database

  "attr_names" shows names of and describes the available attributes

******************************************************************************/

char *
db_show(char *what)
{
  int size ;
  FILE *f ;
  char fname[PATHNAME_MAX] ;
  char *p ;

  sprintf(fname, "%s"D_C"%s.", Control_dir, Cdb.name) ;

  if ( strcasecmp(what, "attr_names") == 0 ||
      strcasecmp(what, "attribute_names") == 0 ) {
    strcat(fname, "attr_names") ;
  }

  if ( (f = bss_fopen(fname, "r")) == NULL )
    return NULL ;
  size = (int)ofsize(fileno(f)) ;
  if ( size > 0 ) Info_buf = bss_realloc(Info_buf, size + 128) ;
  p = Info_buf ;
  while ( !(feof(f)) ) *p++ = getc(f) ;
  *--p = '\0' ;
  bss_fclose(f) ;
  return Info_buf ;
}

	
/******************************************************************************

  record_db(db_name, message)

  Nov 99: no longer done. If ever needed something different'll have to
  be written

  Adds utmp-type record to db_acct

  The fields are message, login name, db name, time

  If the file doesn't exist writably for user the function silently fails.

  The file can be displayed using who ~okapi/databases/db_acct

******************************************************************************/

void
record_db(char *db_name, char *message)
{
  return ;
}

/******************************************************************************

  try_paths  SW May 97

  Tries successively TEMP_PATH and the local bibdir before trying the
  path given by <main_stem>/<end_bit>.

  If type is TRY_FOPEN it returns the (int) of a FILE *, if TRY_OPEN
  it returns a descriptor (-1 on failure).

******************************************************************************/

int
try_paths(char *main_stem, char *end_bit, int type, char *rw, int mode,
	  char *ret_path)
{
  FILE *f ;
  int ret ;

  switch (type) {
  case TRY_FOPEN:
    /* Try Localbib_dir first */
    sprintf(ret_path, "%s"D_C"%s", Localbib_dir, end_bit) ;
    f = bss_fopen(ret_path, rw) ;
    if ( f == NULL ) {
      /* Try Temp_dir next */
      sprintf(ret_path, "%s"D_C"%s", Temp_dir, end_bit) ;
      f = bss_fopen(ret_path, rw) ;
      if ( f == NULL ) {
	/* Do the same thing in the proper place */
	sprintf(ret_path, "%s"D_C"%s", main_stem, end_bit) ;
	f = bss_fopen(ret_path, rw) ;
      }
    }
    ret = (int)f ;		/* May be NULL */
    break ;

  default:
    /* Try Localbib_dir first */
    sprintf(ret_path, "%s"D_C"%s", Localbib_dir, end_bit) ;
    ret = bss_open(ret_path, mode) ;
    if ( ret < 0 ) {
      /* Try Temp_dir next */
      sprintf(ret_path, "%s"D_C"%s", Temp_dir, end_bit) ;
      ret = bss_open(ret_path, mode) ;
      if ( ret < 0 ) {
	/* Do the same thing in the proper place */
	sprintf(ret_path, "%s"D_C"%s", main_stem, end_bit) ;
	ret = bss_open(ret_path, mode) ;
      }
    }
  }
  return ret ;
}
