/*****************************************************************************

  bss_funcs.c  SW May 92

  BSS functions which are accessible from outside

  Transferred from other files 15 May 92

  Dec 92: added close_db() to bss_exit()

*****************************************************************************/

#define EXTRACT

#include "bss.h"
#ifndef _WIN32
#include <signal.h>
#endif
#ifndef NO_HASRLIMIT
#include <sys/resource.h>
#endif
#include "bss_defaults.h"
#include "bss_errors.h"
#include "charclass.h"
//#include "bss_protos.h" //mko
#include <ctype.h>

/******************************************************************************

  bss_startup  SW May 92

  Returns 0 if OK else - and sets bss_Errno

  bss_exit

******************************************************************************/
void bss_exit() ;
int bss_quitq() ;

int
bss_startup(void)
{
  char *s ;
#ifndef NO_HASRLIMIT
  struct rlimit rlp ;
#endif
#ifndef NO_SETITIMER
  struct itimerval itv ;
#endif

  char buf[512] ;
  static char *func = "bss_startup" ;

  bss_syslog = Err_file = stderr ;
  Pagesize = getpagesize() ;
  Pid = (int)getpid() ;
  Uid = (int)getuid() ;
  (void)cuserid(Username) ;
  Gid = (int)getgid() ;
  N_groups = getgroups(NGROUPS_MAX, (gid_t *)Groups) ;
#ifndef NO_SETITIMER
  memset((char *)&itv, '\0', sizeof(itv)) ;
  itv.it_value.tv_sec = TIMER_LOAD_VAL ;
  start_timers(&itv) ;
#endif
  prepare_error(0, 0) ;
#ifndef _WIN32
  (void) signal(SIGHUP, (void(*)(int))bss_exit) ; 
  (void) signal(SIGQUIT, (void(*)(int))bss_exit) ;
  (void) signal(SIGINT, (void(*)(int))bss_exit) ; 
#endif
  Num_open_files = 3 ;
  stdnothing = bss_fopen("/dev/null", "wb") ;
#ifndef NO_ZERO
  stdzero = bss_open("/dev/zero", O_RDWR) ;
#endif
  if ( ( s = getenv("BSS") ) ) BSS = Okapi = s ;
  if ( ( s = getenv("OKAPI") ) ) BSS = Okapi = s ;
  if ( ( s = getenv("BSS_TEMPPATH") ) ) Temp_dir = s ;
  else Temp_dir = TEMP_DIR ;
  if ( (s = getenv("BSS_PARMPATH")) ) Control_dir = s ;
  else Control_dir = CONTROL_DIR ;
  if ( (s = getenv("BSS_BIBPATH")) ) Bib_dir = s ;
  else Bib_dir = BIB_DIR ;
  if ( (s = getenv("BSS_LOCALBIBPATH")) ) Localbib_dir = s ;
  else Localbib_dir = LOCALBIB_DIR ;
  s = getenv ("BSS_LOGDIR") ;
  if ( s != NULL ) {
    sprintf(buf, "%s"D_C"%s.%s.%s.%d", s, LOG_STEM, version, vdate, Pid) ;
    if ( (bss_syslog = Err_file = bss_fopen(buf, "w")) == NULL ) {
      bss_syslog = Err_file = stderr ;
      fprintf(bss_syslog,
      "Can't open BSS logfile %s, will log to stderr\n", buf) ;
    }
  }

#ifndef NO_HASRLIMIT
  getrlimit(RLIMIT_NOFILE, &rlp) ;
  Maxfiles = rlp.rlim_cur ;
#endif /* NO_HASRLIMIT */
  if ( Maxfiles > MAXFILES ) Maxfiles = MAXFILES ;

  Maxmergestreams = Maxfiles - 17 ; /* Rather arbitrary */

  /* Removed read_db_avail() from here May 98 */

  Info_buf = bss_malloc(8192) ;
  /* So there's always something can be freed */
  return 0 ;
}

void
bss_exit(void)
{
  int i ;

  close_db(&Cdb, 255) ;

  /* Clean up setrecs */
  for ( i = 0 ; i < MAXSETRECS ; i++ ) bss_purge_setrec(i) ;
  combine_stats() ;
#ifndef NO_SETITIMER
  if ( Dbg )
    fprintf(bss_syslog, "u: %.2f s: %.2f\n", get_utime(), get_stime()) ;
#endif
}


/******************************************************************************

  Database functions

******************************************************************************/

/*****************************************************************************

  bss_open_db(db_name, flags)  SW Apr 92

  Opens database given by name. The database is attached to the global
  database struct Db. Returns 0 if OK, else -tve. The name is
  the name of the database parameter file. 

  The flags argument controls the extent to which the database is opened.
  
  
  Closes any current database and deletes Setrecs' posting files first

  If successful sets Current_db_name to Db.name

  Return value -10 means db_name not available to user
  For other -tve values look at code

  (NB: close_db() is called automatically by bss_exit() or
  bss_open_db())

*****************************************************************************/

int
bss_open_db(char *db_name, int flags)
{
  int res ;
  static char *func = "bss_open_db" ;

  bss_Result = 0 ;

  if ( ! db_availq(db_name) ) {
    prepare_error(DB_NOT_AVAIL, db_name, 0) ;
    return -10 ;
  }
  if ( Current_db_name != NULL ) {
    close_db(&Cdb, TRUE) ;
    if ( Num_Setrecs > 0 ) bss_clear_all_setrecs() ;
    Current_db_name = NULL ;
  }
  if ( (res = open_db(db_name, &Cdb, flags)) == 0 ) 
    Current_db_name = Cdb.name ;
  else {
    if ( res == ST_FAIL_FOPEN ) prepare_error(DB_NOPARM, db_name, 0) ;
    else prepare_error(DB_OPEN_FAIL, db_name, 0) ;
    res = ST_FAIL_MISC ;
  }
  return res ;
}

/******************************************************************************

  bss_decode_searchtype()  SW June 92

  Given the name of a search type (e.g. "kw", "au") it writes the beast and
  index number corresponding to the supplied arguments and returns 0, or
  returns a negative value if they can't be found or there is no database
  open.

  The special name "default" always succeeds and returns beast = 1 and
  index number = 0. There is no way of finding the real name of default.

  Dec 96: appears unused (SW)
  Jan 97: reference to beast removed. This func ISN'T used, but could be
  sometime. Now just returns ixnum if this index is open

  Removed March 99. See check_attr() and find_search_group_by_name().

******************************************************************************/


/******************************************************************************
 
  bss_findlimit

  Decodes a sequence of limit names into a limit value and returns the
  limit value

  If a limit name is not found the return value is -(N + 1) where N is the
  offset in bytes of the limit name in the string

  If current database has no limits or no database open returns the appropiate
  error code

******************************************************************************/
u_int
bss_findlimit(char *names)
{
  u_int limit = 0 ;
  char name[128] ;
  int olen, adv ;
  char *ip = names ;
  int res = 0 ;
  int dummy ;

  if ( ! check_db_open(&Cdb) ) 
    return 0 ;
  if ( ! Cdb.has_lims ) {
    prepare_error(NO_LIMITS, Cdb.name, 0) ;
    return 0 ;
  }
  while ( strlen(ip) > 0 ) {
    adv = extwd((u_char *)ip, strlen(ip), (u_char *)name, &olen,
		(u_char *)" ,\t:;|", (u_char *)"", &dummy) ;
    name[olen] = '\0' ;
    res = find_limit(name) ;
    if ( res == 0 ) {		/* Name not found */
      prepare_error(NO_SUCH_LIMIT, name, 0) ;
      return -(ip - names + 1) ;
    }
    limit |= res ;
    ip += adv ;
  }
  return limit ;
}

      
/******************************************************************************

  Lookup functions

******************************************************************************/

/******************************************************************************

  bss_lkup()  SW May 92
  
  Also bss_lkup_a() and bss_lkup_anum()

  Looks up term with given search type, ssearch_group and limits in
  current database.  Returns a set record number (0..MAXSETRECS - 1) if
  save is TRUE unless stype != 0 and reached end/beginning of index.

  or <0 if there are no more free setrecs or anything else goes wrong.

  If stype is 0 it only accepts an exact match
  If stype = -1 it finds the greatest term not greater than the search term
  If stype = -2 it finds the greatest term less than the search term
  If stype = 1  it finds the least term not less than the search term
  If stype = 2  it finds the least term greater than the search term

  Dec 94: now replaced at top level by bss_lkup_a(). Ixnum
  no longer to be known externally. But this still does most of the
  error stuff. NB args no longer compatible with earlier versions.

  Feb 96: It's a bit difficult dividing the set initialising between this
  function and setup_pstgs(). Anything which is read only ought to be
  able to be done here.

  Sept 96: most errors handled by bss_lkup_a()

  March 00: new arg ttf only used in case FFL_NOSAVE, clumsy.
  np arg added for consistency.

******************************************************************************/

int
bss_lkup(char *term, int l, Sg *sg, int limits, int stype, int *result,
	 char **found_term, int flags, int *np, int *ttf)
{
  Ix *ix ;
  int itype ;
  void *itr ;			/* Pointer to ITR in SI buf */
  int res ;			/* Function call results */
  int pstgtype ;		/* Output pstg type for setup_pstgs() */
  Setrec *srec ;
  int maxtf ;

  static char *func = "bss_lkup" ;

  if ( stype < -2 || stype > 2 ) {
    prepare_error(NO_SUCH_SEARCHTYPE, itoascii(stype), 0) ;
    return -4 ;
  }

  ix = sg->ix ;
  itype = ix->type ;
  pstgtype = ix->pstgtype ;

  if ( limits && (u_int)limits != Cdb.current_limit ) {
    if ( prepare_limits(&Cdb) < 0 ) {
      prepare_error(CANT_LIMIT_SET, 0) ;
      return -1 ;
    }
  }
    
  itr = bss_stype012(term, l, limits, ix, result, stype) ;
  /* itr may be NULL, partic if stype=0 and there's no exact match.
     It might also be NULL because of some read error. */

  *found_term = get_term_itr(itr, itype) ; 

  if ( flags & FFL_NOSAVE ) 
    return (*np = bss_getnump(itr, limits, ix, ttf)) ;
  /* else */
  /* Now can decide initial posting size. Even if nopos, a large posting
     struct may be needed during the filtering process. Should be reduced
     by reallocation when finished. */
  {
    int pposguess = 0 ;
    maxtf = get_maxtf_itr(itr, ix) ;
    if ( pstgtype & HASPOS ) {
      if ( maxtf > 0 ) pposguess = maxtf ;
      else if ( pstgtype & LONGPOS ) pposguess = MAXLONGPOS ;
      else if ( pstgtype & TEXTPOS ) pposguess = MAXTEXTPOS ;
    }
    /* Does the set need memory? */
    /* Atomic set doesn't need memory until we actually want to read postings,
       unless there is a nonzero limit or nopos=1, even then this is done by
       setup_pstgs() */
    /* get_setrec() now sets output_flags SET_MEM or SET_FILE */
    /* and (Jan 01) now stores pstgtype */
    if ( (srec = get_setrec(pposguess, 0, flags, pstgtype)) == NULL ) {
      prepare_error(NO_FREE_SETS, 0) ; /* Not quite right */
      return -1 ; 
    }
  }

  srec->set_type = S_P_ATOMIC ;
  if ( limits ) srec->set_type |= S_LIMIT ;
  srec->limits = limits ;
  srec->search_type = stype ;
  /* Both the following allocs need error checking */
  srec->search_term = bss_malloc(l + 1) ;
  memcpy(srec->search_term, term, l) ;
  srec->search_term[l] = '\0' ;
  srec->found_term = bss_strdup(*found_term) ;
  if ( itr == NULL ) srec->search_result = 255 ;
  else {
    if ( *result == 1 )
      srec->search_result = 0 ;
    else if ( (stype == 1 || stype == -1) && *result == 8 )
      srec->search_result = srec->search_type ;
    else
      srec->search_result = 0 ;
  }

  srec->n_streams = 1 ;
  srec->ixnum = sg->ixnum ;
  srec->ix = ix ;
  srec->attr_num = sg->number ;

  if ( srec->search_result != 255 ) {

    srec->ix_chunk = ix->cchunk ;
    srec->ix_offset = (u_char *)itr - (u_char *)ix->sibuf ;
    srec->global_pf_length = get_plength_itr(itr, ix) ;
    srec->totalp = get_tnp_itr(itr, ix->type) ;
    srec->total_tf = get_ttf_itr(itr, ix) ;
    srec->max_tf = maxtf ;	/* This is only provisional if nonzero limit */
    if ( limits == 0 ) {
      srec->naw = srec->totalp ;
      srec->ttf = srec->total_tf ;
    }
    if ( flags & FFL_NOPOS ) pstgtype &= ~HASPOS ;
    if ( flags & FFL_NOTF ) pstgtype &= ~(HASTF|HASPOS) ;
    srec->global_pf_offset = get_ppos_itr(itr, ix->type) ;
    if ( limits == 0 && ! (flags & FFL_NOPOS) ) {
      srec->output_flags[0] |= (SET_FILE0_PF) ;
      srec->pf_offset = srec->global_pf_offset ;
    }
    srec->pf_vol = get_pvol_itr(itr, ix->type) ;
    srec->global_pf = ix->ofp[srec->pf_vol] ;
    srec->op = OP_LKUP ;
    
    srec->pstgtype = pstgtype ;

    if ( limits || (flags & FFL_NOPOS) ) {
      res = setup_pstgs(srec, flags) ;
      if ( res < 0 ) {
	bss_clear_setrec(srec->num) ;
	prepare_error(SYS_ERR, Err_buf, 0) ;
	return res ;
      }
      close_pstgs(srec, 0, flags) ; 
    }
  }
  *np = srec->naw ; *ttf = srec->ttf ;
  return srec->num ;
}

int
bss_lkup_a(char *term, int l, char *attribute, int limits, int stype,
	   int *result, char **found_term, int flags,
	   int *np, int *ttf)
{
  Sg *sg ;

  if ( ! check_db_open(&Cdb) ) return -3 ;
  if ( ! check_limit(limits) ) return -4 ;
  sg = check_attr(&Cdb, attribute, 1) ;
  if ( sg == NULL ) return -6 ;
  return
    bss_lkup(term, l, sg, limits, stype, result, found_term, flags, np, ttf);
}

/******************************************************************************

  bss_lkup_anum(), as bss_lkup except is given attribute number.

  Probably not used except by rf (Dec 94) -- using bss_lkup_a() at top level.
  Argument for this one instead is that it would in theory be slightly quicker.

******************************************************************************/

int
bss_lkup_anum(char *term, int l, int sgnum, int limits, int stype,
	      int *result, char **found_term, int flags, int *np, int *ttf)
{
  Sg *sg ;

  if ( ! check_db_open(&Cdb) ) return -3 ;
  if ( ! check_limit(limits) ) return -4 ;
  if ( sgnum >= Cdb.num_search_groups ) return -3 ;
  /* Above would need an error code and message if used at top level */
  sg = &Cdb.groups[sgnum] ;
  return
    bss_lkup(term, l, sg, limits, stype, result, found_term, flags, np, ttf);
}

/******************************************************************************

  make_set(setnum, recnum)  SW Dec 96

  Uninspired name. Function makes set of size 1 from a document specified
  by IRN or by set and recnum.

  If setnum is -1 the recnum is taken to be an IRN.

  Requested by Mike Gatford.

******************************************************************************/

int
make_set(int setnum, int recnum)
{
  if ( ! check_db_open(&Cdb) ) return -1 ;
  if ( setnum != -1 && ! check_set_rec(setnum, recnum) ) return -1 ;
  else if ( setnum == -1 && ! check_irn(recnum) ) return -1 ;
  return make_set_from_rec(setnum, recnum, 0) ;
}

/******************************************************************************
  
  bss_stem_term(term, function_name, buf)   SW May 95

  Writes stemmed term to buf

  Feb 99 now returns length written

******************************************************************************/

int
bss_stem_term(char *term, char *fn, char *buf)
{
  int l ;
  PFI func ;

  bss_Result = 0 ;
  func = check_stemfn(fn) ;
  if ( func != (PFI)NULL ) {
    l = (*func)(term, strlen(term), buf) ;
    *(buf + l) = '\n' ;
    *(buf + l + 1) = '\0' ;
    return l + 2 ;
  }
  else return -1 ;
  /* Error dealt with by check_stemfn() */
}

/******************************************************************************

  Record (document) dispatch functions

******************************************************************************/

/******************************************************************************

  record_length = bss_show(setnum, recnum, db, format, weight, buf, code)

  Apr 94: if setnum is -1 it is assumed that <recnum> is a sequential
  record number [1 - Cdb.nr] if Cdb.type = text, or a record address otherwise

  Feb 94: changed field numbers for some formats as docid is always fd 1
  now (except perhaps Bath catalogue)

  Places requested text in supplied buf according to the format.

  May 02: added startstr, finstr

******************************************************************************/

extern char Hion[], Hioff[] ;

int
bss_show(int setnum, int recnum, Db *db, int format, double *weight,
	   char *buf, int code, char *startstr, char *finstr)
{
  u_char *rec ;
  char *hl_buf = NULL ;
  int bufsize ;
  int fdnum = -1 ;		/* Important: dummy value */
  int totlen ;
  int rn ;			/* Internal record number */
  char *p, *fp ;
  Setrec *srec ;
  int fdlen ;
  int pstgtype ;
  BOOL hl, lengthonly ;
  int j ;
  int maxBufSize = I0_OUTBUFSIZE;

  static char *func = "bss_show" ;

  if ( ! check_db_set_rec_irn(db, setnum, recnum) ) return -1 ;
  if ( setnum != -1 ) {
    srec = Setrecs[setnum] ;
    pstgtype = srec->pstgtype ;
  }
  else pstgtype = 0 ;

  hl = format & 256 ; format &= ~256 ;
  lengthonly = format & 512 ; format &= ~512 ;

  switch (format) {
  case 0:			/* : docid only */
  case 2:			/* : seqnum docid weight */
    hl = 0 ;
    fdnum = 1 ;
    break ;
  /* case 4: unused		 Header only */
  case 5:			/* Header and field 1 but needs to read whole
				   record */
    hl = 0 ;
    break ;
  case 100:
  case 101:
  case 197:
  case 198:
    hl = 0 ;
    fdnum = 1 ;
    break ;
  case 255:
  case 254:
  case 253:			/* June 00: IRN and weight only */
    hl = 0 ;
    fdnum = -2 ;		/* In these cases don't use record at all,
				   so don't read it */
    break ;
  default:
    fdnum = -1 ;
    break ;
  }
  if ( ! (pstgtype & HASPOS) ) hl = 0 ;	/* Just ignore it */
  rec = bss_getrec_raw(setnum, recnum, db, &totlen, weight, fdnum) ;
  //printf("calling bss_getrec_raw, %.3f\n", *weight); /* mko */
  if ( totlen < 0 ) {
    if ( totlen == -8 ) {
      prepare_error(RECORD_OUT_OF_RANGE,
		    itoascii(recnum), itoascii(setnum), 0) ;
    }
    else if ( totlen == -9 ) {
      sprintf(Err_buf, "can't read record %d", recnum) ;
      prepare_error(SYS_ERR, Err_buf, 0) ;
      sys_err(func) ;
    }
    return totlen ;
  }
  else if ( hl && format != 3 && format != 4 && format != 5) { 
    bufsize = real_reclen((char *)rec) +
      srec->pstg->numpos * (strlen(Hion) + strlen(Hioff)) ;
    if ( (hl_buf = bss_malloc(bufsize)) == NULL ) {
      prepare_error(SYS_ERR, "no memory", 0) ;
      return -5 ;
    }
  }

  switch (format) {
  case 0:			/* doc-id only, or 1st field if none */
    memcpy(buf, rec, totlen) ;
    break ;
    
  case 1:
  case 10:
  case 36:			/* Whole record */
    p = buf ; 
    sprintf(p, "%s\n", startstr) ; p += strlen(p) ;
    if ( format == 1 )
      sprintf(p, "Record %d\tWeight %.3f\n", recnum, *weight) ;
    else if ( format == 10 )
      sprintf(p, "Set %d Record %d Weight %.3f\n", setnum, recnum, *weight) ;
    else *p = 0 ;		/* format 36 */
    p += strlen(p) ;
    if ( ! hl ) {
      for ( fdnum = 1 ; fdnum <= db->nf ; fdnum++ ) {
	fp = bss_getfield_raw(fdnum, rec, &fdlen, db, totlen) ;
	if ( fdlen <= 0 ) continue ;
	if ( format == 1 ) {
	  sprintf(p, "%2d: %.*s\n", fdnum, fdlen, fp) ;
	  p += fdlen + 5 ;
	}
	else if ( format == 10 ) {
	  sprintf(p, "FIELD %2d\n%.*s\n", fdnum, fdlen, fp) ;
	  p += fdlen + 10 ;
	}
	else {			/* format == 36 */
	  sprintf(p, "%.*s", fdlen, fp) ;
	  p += fdlen ;
	}
      }
      sprintf(p, finstr) ; p += strlen(p) ;
      totlen = p - buf ;
    }
    else {			/* hl */
      for ( fdnum = 1 ; fdnum <= db->nf ; fdnum++ ) {
	fp = bss_getfield_raw(fdnum, rec, &fdlen, db, totlen) ;
	if ( fdlen <= 0 ) continue ;
	if ( hl ) {
	  fdlen =
	    bss_highlight_field(fp, fdlen, fdnum - 1, 0,
				hl_buf, srec->pstg, pstgtype, format, 0) ;
	  /* The 1st '0' is for field_type, TEMP June 94, the 2nd for
	   start_oset */
	  if ( format == 1 )
	    sprintf(p, "%2d: %.*s\n", fdnum, fdlen, hl_buf) ;
	  else if ( format == 10 )
	    sprintf(p, "FIELD %2d\n%.*s\n", fdnum, fdlen, hl_buf) ;
	  else sprintf(p, "%.*s", fdlen, hl_buf) ;
	  p += strlen(p) ;
	}
	totlen = p - buf ;
      }
    }
    break ;
  case 37:			/* Whole record in exchange format */
    p = buf ; 
    for ( fdnum = 1 ; fdnum <= db->nf ; fdnum++ ) {
      fp = bss_getfield_raw(fdnum, rec, &fdlen, db, totlen) ;
      sprintf(p, "%.*s%c", fdlen, fp, FIELD_MARK) ;
      p += fdlen + 1 ;
    }
    *p++ = RECORD_MARK ;
    *p = '\0' ;
    totlen = p - buf ;
    break ;

  case 3:
  case 4:
  case 5:
				/* Record with header and possibly new_type
				   highlighting info held in header*/
    /* Needs to take some additional arguments as to which fields to be done.
       The highlighting procedure may need to be modified. At present (july
       95) only works on whole document. */
    /* -May 96: temp format 4 does header only, 5 header + field 1 */
    /* ??? above comments (Feb 99) ! */
    {
      char temp[128] ;
      char *hptr = buf ;
      char *dirptr = hptr + 8 + 32  + 6 + 10 + 10 + 10 + 10 + 6 ;
      char *hlptr ;
      char *recptr ;
      int field_offset  = 0 ;
      int fd_ls[MAXNUMFIELDS + 1 ] ;
      int field_oset[MAXNUMFIELDS + 1] ;
      int num_hls = 0 ;
      int outlen = 0 ;

      if ( setnum == -1 ) {
	recptr = dirptr + 3 + db->nf * (3 + 8 + 8) + 6 + 0 + 6 ;
	rn = recnum ;
      }
      else {
	recptr =
	  dirptr + 3 + db->nf * (3 + 8 + 8) + 6 +
	  srec->pstg->numpars * (10 + 8 + 8) + 6 ;
	rn = srec->pstg->rec ;
      }
      prepare_limits(db) ;	/* Might as well, as put limit in header */
      hlptr = recptr - 6 ;
      if ( hl ) {			/* Can't be hl if setnum = -1 */
	recptr += srec->pstg->numpos * ( 1 + 8 + 8 ) ;
	sprintf(hlptr, "%06d", srec->pstg->numpos) ;
	hlptr += 6 ;
      }
      else sprintf(hlptr, "%06d", 0) ;
      /* Write header */
      /* Leave space for message_length */
      hptr += 8 ;
      sprintf(hptr, "%27s", "") ; hptr += 27 ;
      sprintf(hptr, "%05d", Getlimit(rn)) ; hptr += 5 ;
      sprintf(hptr, "%06d%010d", setnum, recnum) ; hptr += 16 ;
      sprintf(hptr, "%010d", rn) ; hptr += 10 ;
      sprintf(hptr, "%010.3f", *weight) ; hptr += 10 ;
      if ( setnum == - 1 )
	sprintf(hptr, "%03d", 0) ; 
      else 
	sprintf(hptr, "%03d", srec->pstg->flags) ;
      hptr += 3 ;
      /* Write code (unused) */
      sprintf(hptr, "%7s", "") ; hptr += 7 ;
      /* Write recstart */
      sprintf(hptr, "%06d", recptr - buf) ;
      /* Write number of dir records (always Db.nf) */
      sprintf(dirptr, "%03d", db->nf) ; dirptr += 3 ;
      /* Write the dir records while writing the document text itself */
      for ( fdnum = 1 ; fdnum <= db->nf ; fdnum++ ) {
	fp = bss_getfield_raw(fdnum, rec, &fdlen, db, totlen) ;
	sprintf(temp, "%03d%08d%08d", fdnum, field_offset, fdlen) ;
	memcpy(dirptr, temp, 3 + 8 + 8) ;
	fd_ls[fdnum] = fdlen ;
	field_oset[fdnum] = field_offset ;
	dirptr += 3 + 8 + 8 ; field_offset += fdlen ;
	/* Now do highlighting for this field if required */
	if ( hl ) {
	  outlen =
	    bss_highlight_field(fp, fdlen, fdnum - 1, 0, hlptr, srec->pstg,
				pstgtype, format, field_oset[fdnum]) ;
	  /* The '0' is for field_type, TEMP June 94 */
	  /* It uses format to decide whether to write separate h/l recs */
	  hlptr += outlen ;
	  num_hls += outlen / sizeof(struct hl_rec) ;
	}
	if ( format == 3 || (format == 5 && fdnum == 1) )
	  { memcpy(recptr, fp, fdlen) ; recptr += fdlen ; }
      }
      /* Bodge in case fewer hl records than pstg->numpos */
      if ( hl ) {
	if ( num_hls < srec->pstg->numpos ) 
	  fprintf(Err_file, "Writing %d dummy hl records\n",
		  srec->pstg->numpos - num_hls) ;
	for ( j = num_hls ; j < srec->pstg->numpos ; j++ ) {
	  memset(hlptr, '0', sizeof(struct hl_rec)) ;
	  hlptr += sizeof(struct hl_rec) ;
	}
      }
      /* Write passage recs (can't be more than MAXPARRECS -- currently July
	 95 255 -- of them) */
      if ( setnum == -1 )
	sprintf(temp, "%06d", 0) ;
      else
	sprintf(temp, "%06d", srec->pstg->numpars) ;
      memcpy(dirptr, temp, 6) ; dirptr += 6 ;
      if ( setnum != -1 && srec->pstg->numpars > 0 ) {
	struct para_rec *pr = srec->pstg->pars ;
	int passo ;
	int passl ;
	float passw ;

	(void)get_para_rec(srec->pstg->rec) ;
	for ( j = 0 ; j < srec->pstg->numpars ; j++, pr++ ) {
	  (void)para_get_field(pr->fd) ;
	  passw = pr->wt ;
	  passo = find_para_offset(pr->sp) + field_oset[pr->fd] ; /* I think */
	  if ( pr->fp == 32767 ) passl = fd_ls[pr->fd] ;
	  else passl = find_passage_len(pr->sp, pr->fp) ;
	  /* May be an occasional problem here, ?para file errors, so trying
	     to trap (7/95) */
	  if (passl <= 0 ||
	      passo + passl > field_oset[pr->fd] + fd_ls[pr->fd]) {
	    /* Should be a BSS warning here */
	    if ( Dbg & D_PASSAGES )
	      fprintf(bss_syslog,
      "Can't find best passage, offset=%d, len=%d, indicating whole field\n",
		      passo, passl) ;
	    passo = field_oset[pr->fd] ; passl = fd_ls[pr->fd] ;
	  }
	  sprintf(temp, "%010.3f%08d%08d", passw, passo, passl) ;
	  memcpy(dirptr, temp, 10 + 8 + 8) ; dirptr += 10 + 8 + 8 ;
	}
      }
      
      if ( *(recptr - 1) != '\n' ) *recptr++ = '\n' ;
      *recptr = '\0' ;
      totlen = recptr - buf ;
      sprintf(temp, "%08d", totlen) ;
      memcpy(buf, temp, 8) ;
    }
    break ;
  case 2:			/* old INSPEC seqnum + docid + weight */
    /* Also used for TREC interactive */
    sprintf(buf, "%d\t%.*s\t%.3f", recnum, totlen, rec, *weight) ;
    totlen = strlen(buf) ;
    break ;
  case 197:			/* 1993 TREC docid + weight */
    sprintf(buf, "%.*s", totlen, rec) ;
    /* Kludge to get it all on 1 line */
    while (tok_space(*(buf + strlen(buf) - 1)))
      *(buf + strlen(buf) - 1) = '\0' ;
    sprintf(buf + strlen(buf), " %.3f", *weight) ;
    totlen = strlen(buf) ;
    break ;
  case 198:			/* 1993 TREC docid (identical to format 0) */
    memcpy(buf, rec, totlen) ;
    break ;
  case 199:			/* 1993 TREC text field only for getting
				   weights for routing. Outputs field 2
				   only, which has usually been empty or
				   unindexed. */
    fp = bss_getfield_raw(2, rec, &fdlen, db, totlen) ;
    if ( fp ) sprintf(buf, "%.*s", fdlen, fp) ;
    totlen = fdlen ;
    break ;
  case 200:			/* Last field only */
    /* Used 1994 TREC, text field only for getting weights for routing */
    fp = bss_getfield_raw(db->nf, rec, &fdlen, db, totlen) ;
    sprintf(buf, "%.*s", fdlen, fp) ;
    totlen = fdlen ;
    break ;
  case 100:
    sprintf(buf, "%.*s", totlen, rec) ;
    /* Kludge to get it all on 1 line */
    while (tok_space(*(buf + strlen(buf) - 1)))
      *(buf + strlen(buf) - 1) = '\0' ;
    sprintf(buf + strlen(buf), " %.3f", *weight) ;
    if ( (pstgtype & HASPASSAGES) )
      for ( j = 0 ; j < srec->pstg->numpars ; j++ ) {
	sprintf(buf + strlen(buf), " %.3f %d %d",
		srec->pstg->pars[j].wt,
		(int)srec->pstg->pars[j].sp, (int)srec->pstg->pars[j].fp) ;
      }
    totlen = strlen(buf) ;
    break ;
  case 101:
    p = buf ;
    fp = bss_getfield_raw(1, rec, &fdlen, db, totlen) ;
    sprintf(p, "%.*s\t%.15f", totlen, rec, *weight) ;
      totlen = strlen(p) ;
    break;
  case 102:
  {
    /**
     * mko - Added case 102
     * This method will return the 2nd field in the index,
     * which is assumed to be a raw document text field
     * as opposed to any document number.
     */
     p = buf;  // Set the pointer to point to beginning of buffer
 
     // Obtain contents of the 2nd field.
     fp = bss_getfield_raw (2, rec, &fdlen, db, totlen);

     //fprintf(stderr, "about to copy,fdlen=%d, bufSize=%d", fdlen,maxBufSize);

     // Copy over contents directly into the pointer.
     // For some reason, sprintf does not work very well for large
     // data, so make use of a "safe" memcpy by ensuring that its within
     // the length of the buffer.
     //sprintf (p, "%.*s", fdlen, fp);
     //fprintf(stderr, "...done. Clear the buffer ..");
     //fprintf(stderr, "...done\n");
     if (fdlen < maxBufSize)
     {
       sprintf (p, "%.*s", fdlen, fp);
       // May copy the entire field over.
       //memcpy (p, fp, fdlen);
     }
     else
     {
       // Out buffer field is too small, we will need to truncate it so that
       // we don't get a buffer overflow.
       // We truncate to the length of the buffer that we have, leaving
       // the last byte for null termination.
       sprintf (p, "%.*s", maxBufSize, fp);
       //memcpy (p, fp, maxBufSize);
       fprintf(stderr, "...Truncating output from %d to %d\n", fdlen, maxBufSize);
     }

     // null terminate the field, so that strlen does not read off the
     // end of the buffer, causing seg faults on linux.
     //p[fdlen] = '\0';

     // Commented out the below line. Only used to test the
     // length of the document we're planning on copying.
     //sprintf (stderr, "Doc length : %d", strlen(p));
    
     // Total length is the length of the p string
     totlen = strlen(p);

    break;
  }
  case 255:
    /* Doesn't output rec but dumps posting */
    if ( setnum == -1 ) totlen = 0 ;
    else totlen = dump_unpacked_pstg(srec->pstg, pstgtype, buf, 0) ;
    break ;
  case 254:
    if ( setnum == -1 ) sprintf(buf, "%d\n", recnum) ;
    else sprintf(buf, "%d", srec->pstg->rec) ;
    totlen = strlen(buf) ;
    break ;
  case 253:
    if ( setnum == -1 ) sprintf(buf, "%d ", recnum) ;
    else sprintf(buf, "%d ", srec->pstg->rec) ;
    sprintf(buf + strlen(buf), "%.3f\n", *weight) ;
    totlen = strlen(buf) ;
    break ;
  default:
    if ( hl ) format |= 256 ;
    if ( lengthonly ) format |= 512 ;
    prepare_error(NO_SUCH_FORMAT, itoascii(format), 0) ;
    totlen = -1 ;
  }
  if ( hl && hl_buf != NULL ) bss_free(hl_buf) ;

  return totlen ;
}

/******************************************************************************

  bss_parse(input, length_input, attr, parsed_terms)

  etc (many others added since the original)

  Note Apr 02: some of these could better call extract_text_nosource()
  as bss_parse_hash() does.

******************************************************************************/

int
bss_parse(char *input, int length_input, char *attr, char *parsed_terms)
{
  char *func = "bss_parse" ;
  Sg *sg ;
  int number_words ;

  if ( ! check_db_open(&Cdb) ) return -1 ;

  sg = check_attr(&Cdb, attr, 0) ;
  if ( sg == NULL ) return -1 ;
  number_words =
    extract_text(input, length_input, ANY, sg->reg, sg->stem_fn,
		 sg->gsl, parsed_terms, EX_USER_PARSE, 0, 0, 0, 0) ;
  return number_words ;
}

int
bss_parse_rgf(char *input, int length,
	      char *r, char *g, char *sf,
	      char *buffer)
{
  Extraction_regime reg ;
  Gsl *gsl ;
  int (*stemfn)() ;

  if ( ! (reg = check_ext_reg(r)) || ! (stemfn = check_stemfn(sf)) ||
       ! (gsl = check_gsl(g, sf, TRUE)) )
    return -1 ;
  return extract_text(input, length, ANY, reg, stemfn, gsl,
		      buffer, EX_USER_PARSE, 0, 0, 0, 0) ;
}

Hashtab *
bss_parse_hash(char *input, int length, Hash_type hash_type,
	       char *r, char *sf, char *g)
{
  Extraction_regime reg ;
  Gsl *gsl ;
  int (*stemfn)() ;
  Hashtab *ht ;
  int hashlen ;
  int numterms ;
  char *hashfuncname = "bwhashpwr2_kl" ;

  if ( ! (reg = check_ext_reg(r)) || ! (stemfn = check_stemfn(sf)) ||
       ! (gsl = check_gsl(g, sf, TRUE)) )
    return (Hashtab *)NULL ; ;
  /* hashlen = estimate_hashlen(length) ; */
  /*hashlen = 32768 ;*/
  /* hashlen = 8192 ;*/
  hashlen = 1024 ;
  ht = make_hashtab(hash_type, hashlen, hashfuncname, HASHSEED, 0) ;
  numterms =  extract_text_nosource(input, length, ANY, reg, stemfn, gsl,
		      (void *)ht, EX_USER_HASH, 0, 0) ;
  if ( numterms < 0 ) {
    free_tab(ht) ;
    return NULL ;
  }
  else return ht ;
}

int
bss_superparse(char *input, int length_input, char *attr, char *buf)
{
  Sg *sg = NULL;

  int number_words = 0;
  // mko - Added all these fprintf statements in here debuggin
  // memory corruption issues. The problem actually turned out to
  // be an issue with blowing the stack. To avoid this, extremely
  // large array (ie. > 64K) MUST be avoided - instead, this memory
  // should be allocated dynamically when needed using the bss_malloc
  // and bss_free() memory allocation/deallocation calls.

  // fprintf(stderr, "About to call myfunc()");
  //mymladen();
  //fprintf(stderr, "Finished called myfunc()");
  //number_words = 
		//extract_text(input, length_input, ANY, sg->reg, sg->stem_fn, sg->gsl,
		//buf, EX_USER_SUPERPARSE, 0, 0, 0, 0) ;
      //extract_text(ip, i1, field_type, reg, stemfn, gs, outptr, outflag,
		//				 0, 0, 0, 0);
      //extract_text_mladen(NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  if ( ! check_db_open(&Cdb) ) return -1 ;
  sg = check_attr(&Cdb, attr, 0) ;
  if ( sg == NULL ) return -1 ;
  if ( sg->reg == lines || sg->reg == profile ) {
    prepare_error(NO_SUPERPARSE, 0) ;
    return -1 ;
  }
  //fprintf(stderr, "reg:%x\nstem_fn:%x\ngsl:%x\n",sg->reg,sg->stem_fn,sg->gsl);
  // Input parms
  //fprintf (stderr, "Dereferencing\n");
  char *ip = input;
  int i1 = length_input;
  FIELD_TYPE field_type = ANY;
  Extraction_regime reg = sg->reg;
  int (*stemfn)(char *, int, char *) = sg->stem_fn;
  Gsl *gs = sg->gsl;
  void *outptr = buf;
  int outflag = EX_USER_SUPERPARSE;
  //fprintf (stderr, "Dereferencing2\n");
  //fprintf(stderr, "Input         :%s\n"
			         //"Length input  :%d\n"
                  //"field_type    :%d\n"
                  //"Extraction reg:%d\n"
                  //"stemfn        :0x%X\n"
                  //"Gsl           :0x%X\n"
                  //"Outptr (buf)  :0x%X\n"
                  //"Outptr length :%d\n"
                  //"Outflag       :0x%08X\n"
                  //"number_words  :%d\n" , ip, i1, field_type, reg, stemfn, gs, outptr,
                                          //strlen((char *)outptr), outflag, number_words);
                 
                  
  number_words = 
		extract_text(input, length_input, ANY, sg->reg, sg->stem_fn, sg->gsl,
		buf, EX_USER_SUPERPARSE, 0, 0, 0, 0) ;
      //extract_text(ip, i1, field_type, reg, stemfn, gs, outptr, outflag,
		//				 0, 0, 0, 0);
      //extract_text(NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  //fprintf (stderr, "Dereferencing3\n");
  return number_words ;
}

int
bss_superparse_rgf(char *input, int length,
		   char *r, char *g, char *sf,
		   char *buffer)
{
  Extraction_regime reg ;
  Gsl *gsl ;
  int (*stemfn)() ;

  if ( ! (reg = check_ext_reg(r)) || ! (stemfn = check_stemfn(sf)) ||
       ! (gsl = check_gsl(g, sf, TRUE)) )
    return -1 ;
  if ( reg == lines || reg == profile ) {
    prepare_error(NO_SUPERPARSE, 0) ;
    return -1 ;
  }
  return extract_text(input, length, ANY, reg, stemfn, gsl,
		      buffer, EX_USER_SUPERPARSE, 0, 0, 0, 0) ;
}


int
bss_superparse_nosource(char *input, int length_input, char *attr, char *buf)
     /* Same as bss_superparse except for type */
{
  Sg *sg ;
  char *func = "bss_superparse_nosource" ;
  int number_words ;

  if ( ! check_db_open(&Cdb) ) return -1 ;
  sg = check_attr(&Cdb, attr, 0) ;
  if ( sg == NULL ) return -1 ;
  number_words =
    extract_text(input, length_input, ANY, sg->reg, sg->stem_fn, sg->gsl,
		 buf, EX_USER_SUPERPARSE_NOSOURCE, 0, 0, 0, 0) ;
  return number_words ;
}


int
bss_superparse_doc(int setnum, int recnum, Db *db, int field, int offset,
		   int length, char *attr, char **buf, BOOL sources)
{
  /* Read doc if necessary, point to the required portion and do it. */
  int l ;
  int nt ;			/* Number of terms */
  char smallbuf[1024] ;		/*  I don't think header alone can be more than
				    about 700 bytes, without highlight info */
  char *largebuf, *p ;
  int sod, osod ;
  int field_length ;
  int length_to_do, max_length_to_do ;
  double dummy ;
  static char *func = "bss_superparse_doc" ;

  if ( ! check_db_open(&Cdb) ) return -1 ;
  if ( setnum != -1 && ! check_set_rec(setnum, recnum) ) return -1 ;
  else if ( setnum == -1 && ! check_irn(recnum) ) return -1 ;
  if ( check_attr(&Cdb, attr, 0) == (Sg *)NULL ) return -1 ;
  if ( ! check_fdnum(&Cdb, field) ) return -1 ;
  /* Show header only (format 4) */
  l = bss_show(setnum, recnum, db, 4, &dummy, smallbuf, 0, "", "");
  if ( l < 0 ) {
    nt = -1 ;
    goto err ;
  }
  l = length_from_header(smallbuf) ;
  /* Now transfer doc to local buf */
  largebuf = bss_malloc(l + 1024) ;
  if ( largebuf == NULL ) {
    prepare_error(SYS_NO_MEM_1, func, itoascii(l + 1024), 0) ;
    nt = -1 ;
    goto err ;
  }
  l = bss_show(setnum, recnum, db, 3, &dummy, largebuf, 0, "", "") ;
  if ( l < 0 ) {
    nt = -1 ;
    goto err ;
  }
  p = largebuf + 86 ; sscanf(p, "%6d", &sod) ; /* Offset of start of data */
  p = largebuf + 95 + (field - 1) * 19 + 3 ;
  sscanf(p, "%8d", &osod) ;	/* Offset of field start from sod */
  sscanf(p + 8, "%8d", &field_length) ;
  max_length_to_do = field_length - offset ;
  if ( max_length_to_do < 0 ) max_length_to_do = 0 ;
  length_to_do = (length <= 0 || length > max_length_to_do) ?
    max_length_to_do : length ;
  *buf = bss_malloc(length_to_do * 8) ;	/* 8 rather arbitrary */
  if ( *buf == NULL ) {
    prepare_error(SYS_NO_MEM_1, func, itoascii(length_to_do * 8), 0 ) ;
    goto err ;
  }
  p = largebuf + sod + osod + offset ;
  if ( sources ) nt = bss_superparse(p, length_to_do, attr, *buf) ;
  else nt = bss_superparse_nosource(p, length_to_do, attr, *buf) ;
 err:
  bss_free(largebuf) ;		/* (now OK if NULL) */
  return nt ;
}

/******************************************************************************

  Set combination (merge) functions

******************************************************************************/

/******************************************************************************

  bss_do_combine()

  originally bss_combine  SW May 92

  This is supposed to do all, or almost all, of the input checking, as well
  as setting up much of the stuff which is used as input to the various
  functions bss_combine_*() which actually do the work. It then calls the
  appropriate function and cleans up afterwards (Nov 94).

  Returns set number or - on error

  Flags values (FFL_...) are defined in bss.h
  NOPOS means no positional data output (to speed batch processing),
  NOSAVE means no output set produced, only statistics. Other bits
  undefined yet.

  May 95: If op is BMLIKE unless BM1 need doclens and avedoclens
          for the index (if index type > 3 (resp 2)) of every atomic set 

  May 96: for wt_frig_factor see bss_combine_bm() 
  May 98: avedoclen may now be passed as arg. If zero (normal case) the true
  value for the database will be used (as always previously).

  Oct 2000: added score function

  Mar 2001: added args qtf, k3, (k7), (k8)
  If there is a single input set (num=1) which is quasi-atomic and the op
  is bmlike, (1) k1, b and k3 are recorded in the output set; (2) the query
  term weight and frequency are recorded in the appropriate fields; (3) the
  given weight is multiplied by the output from k3mult() (NB no effect unless
  qtf > 1 and k3 > 0)

******************************************************************************/

int
bss_do_combine(int num, int sets[], double weights[], int qtf[],
	       int op, int scorefunc,
	       int maxgap, double aw, double gw,
	       int target_number, u_int find_flags, u_int limit,
	       double k1, double k2, double k3, double k7, double k8,
	       double b, double b3,
	       int avedoclen, int querylen, int avequerylen,
	       int passage_unit, int passage_step, int passage_maxlen,
	       double *wt_frig_factor)
{
  //printf ("do some more combining\n"); // mko

  static char *func = "bss_do_combine" ;

  enum OP_TYPE op_flags = NO_TYPE ;

  /* outpstgtype is set in various places: but note that for limit-like
     ops it is always overwritten as the outpstgtype of the 1st input
     stream */
  int outpstgtype = 0 ;
  enum SET_TYPE input_set_types = S_NO_TYPE ;	/* Mar 01 */
  int postype = 0 ;
  int longposcount = 0, textposcount = 0 ;
  int tfcount = 0 ;		/* Count of sets with TF */
  int totalp = 0 ;		/* Sum of the np[] (Feb 96) */
  int total_tf = 0 ;		/* Sum of the input term frequencies */
  int maxoutp = 0 ;		/* Upper bound on number of output postings */
  int maxouttf = 0 ;
  int min_np ;
  int instreams = 0 ;
  int empties = 0 ;		/* # of empty input sets */
  Setrec *outs = NULL ;
  int some_operand_has_marks = FALSE ;
  int large_rn = FALSE ;
  double wt_pwr = 0 ;

  int j ;
  int np[MAXMERGE] ;
  Setrec *srecs[MAXMERGE] ;	/* Pointers to sets for BM ops */
  int iptypes[MAXMERGE] ;       /* Type of input postings (each stream) */
  int sameas[MAXMERGE] ;	/* Jan 96:  */
  int *doclens ;		/* For non-BM1 BM ops */
  int attr_num ;		/* For BM ops */
  int last_attr ;
  int last_unweighted_attr ;
  BOOL mixed_attr ;
  int no_posting_weight_streams ;
  double qtw = weights[0] ;	/* Saved copy for single Q_ATOMIC weighting
				   in case weight modified by k3/qtf */
  int result ;
  char *envp ;

  if ( ! check_db_open(&Cdb) ) goto d_error ;
  if ( ! check_limit(limit) ) goto d_error ;

  if ( num <= 0 ) {
    prepare_error(NO_SETS, 0) ;
    return -1 ;
  }
  if ( num > MAXMERGESTREAMS ) {
    prepare_error(TOO_MANY_SETS, itoascii(MAXMERGESTREAMS), 0) ;
    goto d_error ;
  }

  min_np = INT_MAX ;		/* Moved from above */

  if ( ! (find_flags & FFL_DO_STATS) ) {
    envp = getenv("BSS_COMBINE_DO_STATS") ;
    if ( envp != NULL ) {
      find_flags |= FFL_DO_STATS ;
    }
  }
#ifdef BSS_WT_PWR
  if ( (envp = getenv("BSS_WT_PWR")) ) {
    wt_pwr = atof(envp) ;
    if ( wt_pwr < 0 ) wt_pwr = 0 ;
    if ( wt_pwr > 0 && (Dbg & D_COMBINE) ) {
      fprintf(bss_syslog, "Using weight power %f\n", wt_pwr) ;
    }
  }
#endif
  switch (op) {
  case OP_BM1:
  case OP_BM25:
  case OP_BM11:
  case OP_BM1100:
  case OP_BM15:
  case OP_BM1500:
  case OP_BM25R:
  case OP_BM25S:
  case OP_BM26:
  case OP_BM2600:
    outpstgtype |= HASWT ;
    /* HAS_COORD added Jan 01. But really should validate first (e.g.
       are all sets atomic and with weighted postings?) */
    if ( scorefunc > 0 ) outpstgtype |= (HAS_AUXWT|HASCOORD) ;
    op_flags |= BMLIKE ;
    break ;
  case OP_BM2:
    outpstgtype |= HASWT ;
    if ( scorefunc > 0 ) outpstgtype |= (HAS_AUXWT|HASCOORD) ;
    k1 = k2 = k3 = b = 0 ;
    op_flags |= BMLIKE ;
    break ;
  case OP_BM30:
  case OP_BM3000:
    outpstgtype |= HASWT ;
    op_flags |= BMLIKE ;
    break ;
  case OP_BM40:
  case OP_BM41:
    outpstgtype |= HASWT ;
    op_flags |= BMLIKE ;
    break ;
  case OP_BM250:
  case OP_BM251:
    result = prepare_paras(&Cdb, 1) ;
    if ( result == -2 ) {
      prepare_error(NOPASSAGES, 0) ;
      goto d_error ;
    }
    if ( result == -1 ) {
      prepare_error(DEFECTIVE_PASSAGES, 0) ;
      goto d_error ;
    }
    if (passage_unit < 1 || passage_step < 1 || passage_step > passage_unit ||
	passage_maxlen < passage_unit) {
      prepare_error(BAD_PASSAGE_PARMS,
   itoascii(passage_unit), itoascii(passage_step), itoascii(passage_maxlen), 0) ;
      goto d_error ;
    }
    op_flags |= BMLIKE ;
    outpstgtype |= HASPASSAGES ;
    outpstgtype |= HASWT ;
    if ( scorefunc > 0 ) outpstgtype |= HAS_AUXWT ;
    break ;
  case OP_ADJ1:
  case OP_ADJ2:
  case OP_SAMES1:
  case OP_NEAR1:
  case OP_SAMEF1:
    op_flags |= ADJLIKE ;
    op_flags |= ANDLIKE ;
    break ;
  case OP_AND1:
    op_flags |= ANDLIKE ;
    break ;
  case OP_AND2:
    op_flags |= ANDLIKE ;
    op_flags |= LIMITLIKE ;
    break ;
  case OP_MARK:
    op_flags |= LIMITLIKE ;
    break ;
  case OP_OR1:
    op_flags |= ORLIKE ;
    break ;
  case OP_NOT1:
    op_flags |= NOTLIKE ;
    break ;
  case OP_NOT2:
    op_flags |= LIMITLIKE ;
    op_flags |= NOTLIKE ;
    break ;
  case OP_NOT3:
    op_flags |= LIMITLIKE ;
    op_flags |= NOTLIKE ;
    break ;
  case OP_TOPN:
    if ( num > 1 ) {
      prepare_error(ONLY_ONE_SET, translate_opcode(op), 0) ;
      return -1 ;
    }
    op_flags |= TOPN ;
    op_flags |= LIMITLIKE ;
    break ;
  case OP_BM42:
    prepare_error(UNSUPPORTED_OP, 0) ;
    break ;
  case OP_COPY:
    if ( num > 1 ) {
      prepare_error(ONLY_ONE_SET, translate_opcode(op), 0) ;
      return -1 ;
    }
    op_flags |= LIMITLIKE ;
    break ;
  default:
    prepare_error(NO_SUCH_OP, 0) ;
    return -1 ; 
    break ;
  }

  /* Set and output type checking and determination. Moved here from the
     various functions which actually do the merge (1995). */

  /* Check input sets, get np's, check attributes */
  attr_num = NO_ATTRIBUTE ;
  no_posting_weight_streams = 0 ;
  mixed_attr = FALSE ;
  for ( j = 0 ; j < num ; j++ ) {
    if ( ! check_set(sets[j]) )	/* Checks set's existence,
				   does prepare_error() if absent */
      goto d_error ;
    srecs[j] = Setrecs[sets[j]] ; 
    np[j] = srecs[j]->naw ;
    if ( np[j] == 0 ) {
      empties++ ;
      iptypes[j] = 0 ;		/* ?? */
    }
    else {
      iptypes[j] = srecs[j]->pstgtype ;
      if ( iptypes[j] & HASMARKS ) some_operand_has_marks = TRUE ;
      if ( iptypes[j] & LARGE_RN ) large_rn = TRUE ;
      if ( srecs[j]->set_type & S_Q_ATOMIC )
	input_set_types |= srecs[j]->set_type ;
      else input_set_types &= ~(S_Q_ATOMIC) ;
    }
    instreams += srecs[j]->n_streams ;
    /* For any non-NOT and non-LIMIT op involving more than one set
       the attr_num and index fields of the output set must be
       invalidated */
    if ( j > 0 &&
	 !(op_flags & LIMITLIKE) && !(op_flags & NOTLIKE) &&
	 srecs[j]->attr_num != last_attr )
      mixed_attr = TRUE ;

    if ( (op_flags & (BMLIKE)) &&
	! (iptypes[j] & HASWT) && op != OP_BM1 && op != OP_BM2 ) {
      /* BM ops exc BM1/2 con't be done on sets from different attributes */
      if ( no_posting_weight_streams > 0 &&
	   srecs[j]->attr_num != last_unweighted_attr ) {
	prepare_error(MORE_THAN_ONE_ATTRIBUTE, translate_opcode(op), 0) ;
	goto d_error ;
      }
      if ( srecs[j]->attr_num == MIXED_ATTRIBUTE ) {
	prepare_error(IMPURE_SET,
		      itoascii(srecs[j]->num), translate_opcode(op), 0) ;
	goto d_error ;
      }
      last_unweighted_attr = srecs[j]->attr_num ;
    }
    last_attr = srecs[j]->attr_num ;
    if ( ! (iptypes[j] & (HASWT|HAS_INDEX_WT)) ) {
      no_posting_weight_streams++ ;
      if ( (op_flags & BMLIKE) && k3 > 0 && qtf[j] > 1 ) {
	/* Calculate weight to pass to the combine function (Mar 01) */
	weights[j] *= k3mult(k3, qtf[j]) ;
      }
    }
  }

  /* Process BM ops */
  if ( op_flags & BMLIKE ) {
    /* Read doclens if necessary */
    if ( op != OP_BM1 && op != OP_BM2
 && ( no_posting_weight_streams > 0 || op == OP_BM40 || op == OP_BM41 ) ) {
      struct index *ix ;
      /* ix = Cdb.ix[Cdb.groups[last_unweighted_attr].ixnum] ;*/
      /* Changed to last_attr, really for bm40/1 Apr 00 */
      
      if ( mixed_attr ) {
	prepare_error(DOCLENS_MIXED_ATTRIBUTE, 0) ;
	goto d_error ;
      }
      else {
	ix = Cdb.ix[Cdb.groups[last_attr].ixnum] ;
	result = read_doclens(&Cdb, ix->num) ;
	if ( avedoclen == 0 ) avedoclen = get_avedoclen(&Cdb, ix->num, limit) ;
      }
      doclens = ix->doclens ;
      if ( result < 0 || avedoclen < 0 || doclens == NULL ) {
	prepare_error(CANT_GET_DOCLENS, Cdb.groups[last_attr].search_names, 0);
	goto d_error ;
      }
    } /* (read doclens) */
  
    /* Maybe only BM ops with at least one unweighted stream but m.a.w. do for
       all */
    switch (op) {
    case OP_BM25:
    case OP_BM26:
    case OP_BM250:
    case OP_BM251:
    case OP_BM2600:
    case OP_BM25R:
    case OP_BM25S:
    case OP_BM30:
    case OP_BM3000:
      if ( k1 == NONE_ASSIGNED ) k1 = DEF_BM25_K1 ;
      if ( k2 == NONE_ASSIGNED ) k2 = DEF_BM25_K2 ;
      if ( k3 == NONE_ASSIGNED ) k3 = DEF_BM25_K3 ;
      if ( b == NONE_ASSIGNED ) b = DEF_BM25_B ;
      if ( b3 == NONE_ASSIGNED ) b3 = DEF_BM25_B3 ;
      break ;
    case OP_BM2:
      k1 = k2 = k3 = b = 0 ;
      op_flags |= NO_MPW ;
      break ;
    case OP_BM40:
    case OP_BM41:
      if ( k2 == NONE_ASSIGNED ) k2 = DEF_K2_BM40 ;
      if ( k2 != 0 ) op_flags |= NO_MPW ;
      /* Forces wt_dist_grad 0 */
      break ;
    case OP_BM1100:
      if ( k1 == NONE_ASSIGNED ) k1 = DEF_K1_BM11 ;
      if ( k2 == NONE_ASSIGNED ) k2 = DEF_K2_BM11 ;
      break ;
    case OP_BM11:
      k1 = DEF_K1_BM11 ;
      k2 = DEF_K2_BM11 ;
      break ;
    case OP_BM1500:
      if ( k1 == NONE_ASSIGNED ) k1 = DEF_K1_BM15 ;
      if ( k2 == NONE_ASSIGNED ) k2 = DEF_K2_BM15 ;
      break ;
    case OP_BM15:
      k1 = DEF_K1_BM15 ;
      k2 = DEF_K2_BM15 ;
      break ;
    case OP_BM1:
    default:   
      /* Defaults altered Jan 01 as k2 used in score function 2 & 3 */
      /* Now (Feb 01) k7 so didn't matter, but m.a.w. leave it */
      if ( k1 == NONE_ASSIGNED ) k1 = 0 ;
      if ( k2 == NONE_ASSIGNED ) k2 = 0 ;
      if ( b == NONE_ASSIGNED ) b = 0 ;
    } /* ( switch(op)) */
  } /* (BMLIKE) */

  /* Check positional info. Don't output positional info unless all
     nonempty streams have pos info and of same type */
  for ( j = 0 ; j < num ; j++ ) {
    if ( np[j] == 0 ) continue ;
    /* All sets must be quasi-atomic for ADJ to work */
    if ( ( op_flags & ADJLIKE ) &&
	 ! (srecs[j]->set_type & S_Q_ATOMIC) ) {
      prepare_error(NO_PROX_2, itoascii(sets[j]), 0) ;
      goto c_error ;
    }
    if ( (iptypes[j] & TEXTPOS) ) textposcount++ ;
    else if ( (iptypes[j] & LONGPOS) ) longposcount++ ;
    else { /* No positional info */
      if ( op_flags & ADJLIKE ) {
	prepare_error(NO_PROX_3, itoascii(sets[j]), 0) ;
	goto c_error ;
      }
    }
    if ( iptypes[j] & HASTF ) tfcount++ ;
    /* Feb 96: temp condition to avoid testing */
    if ( (op_flags & BMLIKE) && ! (iptypes[j] & HASWT)) {
      if ( ! Legalweight(weights[j]) ) {
	prepare_error(SET_NO_WEIGHT, itoascii(sets[j]), 0) ;
	goto c_error ;
      }
      if ( wt_pwr > 0 && weights[j] > 0 )
	weights[j] = pow(weights[j], wt_pwr) ;
    }
    if ( op == OP_BM2 && ! (iptypes[j] & HAS_INDEX_WT) ) {
      prepare_error(NO_INDEX_WEIGHT, itoascii(sets[j]), 0) ;
      goto c_error ;
    }
  }

  /* Set flags appropriately with regard to positional info */
  if ( textposcount < num - empties &&
      longposcount < num - empties ) {
    /* Not a complete set of positional records */
    if ( op_flags & ADJLIKE ) {
      prepare_error(NO_PROX_1, 0) ;
      goto c_error ;
    }
    /* and can't output any positional info */
    find_flags |= FFL_NOPOS ;
    if ( tfcount < num - empties ) {
      find_flags |= FFL_NOTF ;
      if ( (op_flags & BMLIKE) && op != OP_BM1 && op != OP_BM2 ) {
	prepare_warning(NO_TF_WARNING) ;

      }
    }
  }
  else if ( textposcount >= num - empties ) postype = TEXTPOS ;
  else /* longposcount >= num - empties */ postype = LONGPOS ;

  if ( ! (find_flags & FFL_NOTF) ) {
    outpstgtype |= HASTF ;
    if ( ! (find_flags & FFL_NOPOS) ) outpstgtype |= postype ;
  }
  /* but */
  if ( find_flags & FFL_QUICK_COMBINE ) outpstgtype &= ~(HASPOS|HASTF) ;

  /* Prepare sets for output */

  {
    int match, k ;
    for ( j = 0 ; j < num ; j++ ) {
      for ( k = 0, match = 0 ; k < j ; k++ ) {
	if ( sets[j] == sets[k] ) {
	  match = 1 ;
	  break ;
	}
      }
      if ( match ) {
	sameas[j] = k ;
	find_flags |= FFL_NO_SELECTION_TREE ; /* Because it can't cope with
					         repeated sets. */
      }
      else sameas[j] = -1 ;
      if ( np[j] > 0 && ! match ) {
	if ( ! (find_flags & FFL_QUICK_COMBINE ) )
	  /* (if QUICK_COMBINE sets' postings are opened one at a time
	     when wanted) */
	  if ( open_pstgs(srecs[j], 0, find_flags ) < 0 ) {
	    prepare_error(CANT_READ_SET, itoascii(sets[j]), 0) ;
	    goto c_error ;
	  }
	totalp += np[j] ;
	if ( np[j] < min_np ) min_np = np[j] ;
	total_tf += srecs[j]->ttf ;
      }
    }
  }

  if ( op_flags & LIMITLIKE ) {
    maxoutp = np[0] ;
    maxouttf = srecs[0]->ttf ;
  }
  else {
    maxoutp = totalp ;
    maxouttf = total_tf ;
  }
  if ( totalp > Cdb.nr ) totalp = Cdb.nr ; /* (July 98) */

  /* Finalize outpstgtype */
  if ( op_flags & LIMITLIKE ) outpstgtype = iptypes[0] ;
  else if ( some_operand_has_marks &&
      ! (op_flags & (LIMITLIKE|NOTLIKE)) )
    outpstgtype |= HASMARKS ;
  if ( (iptypes[0] & HASMARKS) || op == OP_MARK ) outpstgtype |= HASMARKS ;
  if ( large_rn ) outpstgtype |= LARGE_RN ;
  /* Get and set up output setrec */
  /* This ought to be a separate function, cf setup_pstgs() */
  {
    int psize ;
    int msize ;
    if ( ! (outpstgtype & HASPOS) ) psize = 0 ;
    else psize = MAXTEXTPOS ;	/* This is trimmed at end */
    /* Estimate output memory requirement as upper bound: assume all sets
       disjoint and all output postings have > 127 positional fields. */
    /* An alternative would be just to add up the input posting lengths and
       add 4 * totalp for weights if necessary. */
    msize = totalp * 3 ;	/* For IRNs */
    if ( large_rn ) msize += totalp ; /* 4-byte IRN instead of 3 */
    /* Note can't have both TF and QTF */
    if ( outpstgtype & HASTF )
      msize += totalp * 2 ;    /* Have to assume all have 2-byte numpos field*/
    else if ( outpstgtype & HAS_QTF ) msize += totalp ;
    /* msize += totalp * 2 ; Have to assume all have 2-byte numpos field*/
    if ( outpstgtype & HASPOS ) msize += sizeof(U_pos) * maxouttf ;
    if ( outpstgtype & (HASWT|HAS_INDEX_WT) ) {
      msize += sizeof(float) * maxoutp ;
      if ( outpstgtype & HAS_AUXWT )
	msize += 2 * sizeof(float) * maxoutp ;
    }
    if ( outpstgtype & HASMARKS ) msize += maxoutp ;
    if ( outpstgtype & HASPASSAGES )
      msize += maxoutp * MAXPARRECS * sizeof(Para_rec) ;
    /* outpstgtype was given as 0 (and ignored by get_setrec()),
       changed Apr 00. */
    outs = get_setrec(psize, msize, find_flags, outpstgtype) ;
    /* (msize ignored unless flags say mem) */
    if ( outs == NULL ) {
      /* prepare_error() done by get_setrec() */
      goto c_error ;
    }

    if ( ! (find_flags & (FFL_MEM|FFL_NOSAVE)) ) {
      outs->pfile[0] = set_open_pf(outs->num, 0, "w+b") ;
      /* "w+" in case want to leave it open */
      if ( outs->pfile[0] == NULL ) {
	prepare_error(CANT_WRITE_SET, itoascii(outs->num), 0) ;
	goto c_error ;
      }
    }
  }
  
  outs->totalp = maxoutp ;
  outs->total_tf = maxouttf ;
    
  if ( outpstgtype & (HASWT|HAS_INDEX_WT) ) {
    if ( op_flags & NO_MPW ) outs->weight_dist_grad = 0 ;
    else outs->weight_dist_grad = WEIGHT_DIST_GRAD ;
    outs->set_type |= S_WEIGHTED ;
    /* May 02: for profile sets */
    if ( op == OP_BM2 ) outs->set_type |= S_NOSORT ;
  }

  /* Finalise outs->set_type: S_Q_ATOMIC holds iff all input sets are
     some kind of atomic. */
  if ( input_set_types & S_Q_ATOMIC ) {
    enum SET_TYPE ist = input_set_types ;
    if ( op == OP_ADJ1 || op == OP_ADJ2 )
      outs->set_type |= (ist | S_A_ATOMIC) ;
    else if ( op == OP_OR1 ) outs->set_type |= (ist | S_O_ATOMIC) ;
    else if ( num == 1 && (op_flags & BMLIKE) ) outs->set_type |= ist ;
    /* and there must be other cases ... (Mar 01) */
    if ( outs->set_type & (S_A_ATOMIC | S_O_ATOMIC) )
      outs->set_type &= ~(S_P_ATOMIC) ;
    /* Don't want to show pure atomic if one of the inputs wasn't */
  }
  outs->set_type |= S_MERGE ; /* Is this useful? Only tells us it's not
				 just a lookup */
  if ( op == OP_TOPN || target_number > 0 ) outs->set_type |= S_PARTIAL ;
  /* But could have a nonzero target and still be complete */
  outs->op = op ;		/* Is this right for limit-like? */
  if ( op_flags & BMLIKE ) {
    if ( no_posting_weight_streams > 0 ) {
      /* Otherwise these are ignored & will be recorded as 0 */
      outs->k1 = k1 ;
      outs->b = b ;
      outs->k2 = k2 ;
      outs->k3 = k3 ;
      if ( num == 1 ) {
	/* qtf[] ignored unless k3 > 0. If num > 1 qtf[] & weights[] are used
	   to calculate the weights passed to combine function if k3 > 0
	   but can't be recorded in the set record. */
	outs->qtweight = qtw ;	/* Copy of input weights[0] (bodge, Mar 01) */
	if ( k3 > 0 ) outs->qtf = qtf[0] ;
      }
    }
    outs->score_function = scorefunc ;
    outs->k7 = k7 ;
    outs->k8 = k8 ;
    outs->target_number = target_number ; /* (Aug 02) */
    //printf ("continuing!!\n"); //mko
  }
  if ( mixed_attr ) {		/* See above */
    outs->ixnum = MIXED_ATTRIBUTE ; 
    outs->ix = NULL ;
    outs->attr_num = MIXED_ATTRIBUTE ;
  }
  else {
    outs->ixnum = srecs[0]->ixnum ;
    outs->ix = srecs[0]->ix ;
    outs->attr_num = srecs[0]->attr_num ;
  }
  outs->aw = aw ;		/* Overridden in case OP_NOT3 */
  outs->gw = gw ;		/* Overridden in case OP_NOT3 */

  if ( op_flags & BMLIKE ) {
    if ( Dbg & D_COMBINE ) 
      fprintf(bss_syslog,
"k1=%.3f, k2=%.3f, k3=%.3f, k7=%.3f, k8=%.3f, b=%.3f, b3=%.3f, ql=%d, aql=%d\n",
	      k1, k2, k3, k7, k8, b, b3, querylen, avequerylen) ;

    if ( (find_flags & FFL_QUICK_COMBINE) &&
	 (op == OP_BM25 || op == OP_BM2 || op == OP_BM1) ) {
      result = bss_quick_combine(num, op, outs, find_flags, srecs, weights,
				 doclens, avedoclen, target_number,
				 k1, b, Cdb.big_n) ;
    }
    else if ( ( op == OP_BM250
	 || op == OP_BM251
	 || (num < SELECTION_TREE_CROSSOVER && !
	     (find_flags & FFL_SELECTION_TREE))
	 || (find_flags & FFL_NO_SELECTION_TREE) )
	 && op != OP_BM2 ) {
      if ( Dbg & D_COMBINE ) fprintf(bss_syslog,
				     "Combine: not using selection tree\n") ;
      //printf ("try 1\n"); /* mko */
      result = bss_combine_pass(num, op, scorefunc, outs, iptypes, 
				find_flags, limit, srecs, weights,
				doclens, avedoclen, sameas, target_number,
				 k1, k2, k7, b, wt_frig_factor,
				 passage_unit, passage_step, passage_maxlen) ;
    }
    else {
      if ( Dbg & D_COMBINE )
	fprintf(bss_syslog, "Combine: using selection tree\n") ;
      //printf ("try 2\n"); // mko
      result =
	  bss_combine_bm(num, op, scorefunc, outs, find_flags, limit,
			 srecs, weights, doclens, avedoclen,
			 sameas, target_number,
			 k1, k2, k7, b, wt_frig_factor) ;
    }
    if ( result < 0 ) goto c_error ;

    outs->n_streams = instreams ;
    if ( outs->n_streams > 1 ) outs->adj_no_good = TRUE ;
    /* But you should be able to do adj if n_streams is only one. */
  }
  else if ( op_flags & TOPN ) {
    /* This has to come before LIMITLIKE because topn has both set */
    outs->mpw = srecs[0]->maxweight ;
    outs->realmpw = srecs[0]->maxweight ;
    outs->minpw = srecs[0]->minweight ;
    outs->gw = srecs[0]->gw ;
    outs->aw = srecs[0]->aw ;
    if ( outs->pstgtype & HASWT )
      outs->weight_dist_grad = WEIGHT_DIST_GRAD ;
    if ( (result =
	  select_topn(sets[0], outs, target_number, find_flags)) < 0 )
      goto c_error ;
  }
  else if ( op_flags & LIMITLIKE ) {
    outs->set_type = (srecs[0]->set_type | S_RLIMIT) ;
    outs->search_type = srecs[0]->search_type ;
    outs->search_result = srecs[0]->search_result ;
    outs->attr_num = srecs[0]->attr_num ;
    outs->mpw = srecs[0]->maxweight ;
    outs->realmpw = srecs[0]->maxweight ;
    outs->minpw = srecs[0]->minweight ;
    outs->n_streams = srecs[0]->n_streams ;
    if ( op == OP_NOT3 ) outs->n_streams -= num - 1 ;
    outs->adj_no_good = srecs[0]->adj_no_good ;
    if ( srecs[0]->weight_dist_grad )
      outs->weight_dist_grad = WEIGHT_DIST_GRAD ;
    if ( op == OP_NOT3 ) 
      outs->aw = outs->gw = 0 ;	/* Not sure about this */
    if ( (result =
	  bss_combine_limit(num, op, op_flags, np, srecs,
			    sameas, target_number, outs, find_flags)) < 0 )
      goto c_error ;
  }
#ifdef COPY_OK
  else if ( op == OP_COPY )
    result = bss_copy_set(np, postype, outs, srecs) ;
#endif
  else {
    if ( (result =
	  bss_combine_0(num, op, op_flags, np, postype, 
			outs, srecs, sameas, maxgap)) < 0 )
      goto c_error ;
    if ( op == OP_NOT1 ) {
      outs->n_streams = srecs[0]->n_streams ; /* Doubt it! */
      outs->adj_no_good = srecs[0]->adj_no_good ;
    }
    else {
      outs->n_streams = instreams ;
      if ( ! (op_flags & ADJLIKE) &&
	     ! (op_flags & ORLIKE) )
      outs->adj_no_good = TRUE ;
    }
  }

  for ( j = 0 ; j < num ; j++ ) close_pstgs(srecs[j], 0, find_flags) ;
  close_pstgs(outs, 0, 0) ; 
  return outs->num ;

 c_error:
  
  for ( j = 0 ; j < num ; j++ ) close_pstgs(srecs[j], 0, 0) ;
  if ( outs != NULL ) bss_clear_setrec(outs->num) ;

 d_error:

  return -1 ;
}

/******************************************************************************

  Weight functions

  int
  bss_assign_weight(weight_function, limit, n, R, r, Rloading, rloading, bign)

  double
  bss_assign_weight(weight_function, limit, n, R, r, Rloading, rloading, bign)

  function 0 does f4 with R = r = 0 (passed R and r are ignored)
  function 1 does normal f4 predictive
  function 2 does f4 predictive with a loading on R and r which is applied
  to the p component only, using args Rloading and rloading

  Big N is determined by the limits (except see below)

  Function 2 added 11 Mar 93, takes 2 extra args Rloading and rloading

  NB Sep 93: index_num no longer used

  Returns a weight, or 0x0fffffff on error

  May 95: Now does bss_set_limit if required by the limit argument and restores
          the old one before returning.

  March 96: new arg bign may be passed. Used instead of N if positive.

  The integer-returning version is now obsolete & has been removed. The double
  version probably returns 0 on error.

  March 00: added functions 5 and 6. Both need lamda. 5 needs tnp and n,
  6 needs cttf and ttf. (tnp = total # of postings in the index, cttf = total
  # indexed tokens in the index, ttf total # of tokens for the term.) These
  are used with bm40 (5) and bm41 (6) -- although 40 and 41 only differ in the
  weights, not in the search-time processing -- they're identical as far as the
  combine functions are concerned.

  June 00: This function should take care of getting tnp or cttf for functions
  5 or 6. At present there's no way of getting tnp, so I haven't bothered about
  cttf either. Note that they both depend on the attribute, so maybe this
  should become an argument of the function.

  Nov 00: added factor which if > 0 multiplies returned weight

******************************************************************************/

double
bss_assign_weight_ln(int f, int limit, int n, int ttf, int R, int r,
		     int Rloading, int rloading, int bign,
		     int S, int s, int qtf,
		     double k3, double k4, double k5, double k6,
		     int theta, double lamda,
#ifdef HAVE_LL
		     LL_TYPE tnp,
		     LL_TYPE cttf,
#else
		     double tnp,
		     double cttf,
#endif
		     double factor
		     ) 
{
  int N ;
  double weight ;

  /* If have value for bign don't need database open.
     n < 0 and N < 0 are used to denote "unassigned" */

  /* For function 5 don't need bign, so don't need Db open,
     but never mind for now. (Can do func 5 without db open
     if give value for bign, provided it's bigger than n.) */

  if ( bign > 0 ) N = bign ;
  else { /* must have database open */
    if ( ! check_db_open(&Cdb) ) return 0 ;
    if ( ! check_limit(limit) ) return 0 ;
    if ( (u_int)limit == Cdb.current_limit ) N = Cdb.big_n ;
    else N = calc_bign(&Cdb, limit) ;
  }
  if ( n < 0 || N <= 0 || N < n ) {
    prepare_error(PHONEY_BIGN_OR_N, itoascii(N), itoascii(n), 0) ;
    return 0 ;
  }
  if ( f != 6 && n == 0 ) return 0 ; 
  if ( f == 6 && ttf == 0 ) return 0 ; 
//printf ("f value : %d, in hex : %x", f, f); // mko
  switch(f) {
  case 0:
    weight = k4 + f4_p_loaded_ln((double)N, (double)n,
	  (double)0, (double)0, (double)0.5, (double)0, (double)0) ;
    break;

  case 1:
    weight = k4 + f4_p_loaded_ln((double)N, (double)n,
	  (double)R, (double)r, (double)0.5, (double)0, (double)0) ;
    break;

  case 2:
    //printf ("calculating weight\n"); // mko
    weight = k4 + f4_p_loaded_ln((double)N, (double)n,
      (double)R, (double)r, (double)0.5, (double)Rloading, (double)rloading) ;
    //printf ("weight: %10.3f\n", weight); //mko
    break;

  case 3:
    weight = nr_wt((double)N, (double)n, (double)R, (double)r,
		   (double)S, (double)s,
		   k4, k5, k6) ;
    break ;

  case 4:
    weight = k4 + f4_p_theta_ln((double)N, (double)n, (double)R, (double)r,
				(double)theta) ;
    break;
  case 5:
    if ( lamda <= 0 || lamda >= 1 || tnp <= 0 ) {
      prepare_error(PHONEY_WEIGHT_ARGS, itoascii(f), 0) ;
      return 0 ;
    }
    fprintf (stdout, "CASE 5!!! ");
    weight = (double)tnp / (double)n * lamda / (1 - lamda) ;
    k3 = 0 ;
    break ;
  case 6:
    if ( lamda <= 0 || lamda >= 1 || cttf <= 0 ) {
      prepare_error(PHONEY_WEIGHT_ARGS, itoascii(f), 0) ;
      return 0 ;
    }
    weight = (double)cttf / (double)ttf * lamda / (1 - lamda) ;
    k3 = 0 ;
    break ;
  default:
    printf ("just default used\n");
    prepare_error(NO_SUCH_WEIGHTFUNC, itoascii(f), 0) ;
    return -1.0 ;
  }
  if ( isnan(weight) ) {
    prepare_error(PHONEY_WEIGHT_ARGS, itoascii(f), 0) ;
    return 0 ;
  }
  /* Mustn't use k3 here for bm40/41 as subsequently logged */
  if ( qtf > 1 ) weight *= ( k3 + 1 ) * (double)qtf / ((double)qtf + k3) ;
  if ( factor > 0 ) weight *= factor ;
  return weight ;
}



/******************************************************************************

  bss_set_limit(limit)  SW Nov 94

  The limit which is set is stored in Cdb.current_limit, initially zero.
  
  Returns - on error, or previous limit value (starts as 0 when Db opened).

  May 95: now also opens the N_s file if not open & sets current_avedoclen
  for each index.

  Sept 96: N_s file obsolescent. Calcs bign instead, and sets it.

******************************************************************************/

u_int
bss_set_limit(u_int limit)
{
  u_int old_limit ;

  if ( ! check_db_open(&Cdb) ) return (u_int)-1 ;
  if ( ! check_limit(limit) ) return (u_int)-1 ;
  if ( (old_limit = Cdb.current_limit) != limit ) {
    set_bign(&Cdb, calc_bign(&Cdb, limit)) ;
    set_avedoclens(&Cdb, limit) ; /* (Oct 96) only does it for open indexes
				    with doclens set up */
    Cdb.current_limit = limit ;
  }
  return old_limit ;
}

/******************************************************************************

  bss_dolimit()  SW Dec 94

  Does little more than call bss_limit()

******************************************************************************/

u_int
bss_dolimit(int setnum, u_int mask, int flags)
{
  Setrec *old_srec, *srec ;
  FILE *f_old, *f_new ;
  int optype ;
  
  if ( ! check_db_open(&Cdb) || ! check_limit(mask) || ! check_set(setnum) )
    return (u_int)-1 ;
  
  old_srec = Setrecs[setnum] ;
  optype = old_srec->pstgtype ;
  if ((f_old = set_open_pf(old_srec->num, 0, "rb")) == NULL) return (u_int)-1 ;
  /* 0, 0, 0, 0 is temp Feb 96 til this func rewritten */
  if ((srec = get_setrec(0, 0, 0, optype)) == NULL ) return (u_int)-1 ;
  srec->set_type |= S_LIMIT ; /* limit */
  srec->op = old_srec->op ;
  srec->limits = mask ;
  srec->n_streams = old_srec->n_streams ;
  /* srec->pstgtype = optype ; See above */
  if ((f_new = set_open_pf(srec->num, 0, "wb")) == NULL) {
    bss_clear_setrec(srec->num) ;
    return (u_int)-1 ;
  }
  return bss_limit(old_srec, mask, srec, f_old, f_new) ;
}


/******************************************************************************

  Various functions for validity checking
  and, for what it's worth, return 1 if OK, 0 otherwise.
  If not OK they all call prepare_error() with the appropriate code.
  If OK they reset bss_Result.
  Most of them assume check_db_open() has been done.

******************************************************************************/

int
check_db_open(Db *db)
{
  bss_Result = 0 ;
  if ( ! is_db_open(db) ) {
    prepare_error(NO_DB_OPEN, 0) ;
    return 0 ;
  }
  else return 1 ;
}

int
check_set(int num)
{
  bss_Result = 0 ;		/* Should it do this? */
  if ( num < 0 ) {
    prepare_error(NO_SET_ASSIGNED, 0) ;
    return 0 ;
  }
  else if ( ! check_setnum(num) ) {
    prepare_error(NO_SUCH_SET, itoascii(num), 0) ;
    return 0 ;
  }
  else return 1 ;
}

int
check_set_rec(int setnum, int recnum)
{
  if ( ! check_set(setnum) ) return 0 ;
  if ( recnum < 0 ) {
    prepare_error(NO_RECORD_ASSIGNED) ;
    return 0 ;
  }
  if ( ! check_set_and_rec(setnum, recnum) ) {
    prepare_error(NO_SUCH_RECORD, itoascii(recnum), itoascii(setnum), 0) ;
    return 0 ;
  }
  else return 1 ;
}

int
check_irn(int irn)
{
  bss_Result = 0 ;
  if ( Cdb.nr == 0 ) {
    prepare_error(EMPTY_DB, Cdb.name, 0) ;
    return 0 ;
  }
    
  if ( irn < 1 || irn > Cdb.nr ) {
    prepare_error(RECORD_OUT_OF_RANGE_DB, itoascii(1), itoascii(Cdb.nr), 0) ;
    return 0 ;
  }
  else return 1 ;
}

int
check_db_set_rec_irn(Db *db, int setnum, int recnum)
{
  if ( ! check_db_open(db) ) return 0 ;
  if ( setnum != -1 ) {
    if ( ! check_set(setnum) ) return 0 ;
    if ( ! check_set_rec(setnum, recnum) ) return 0 ;
  }
  else {
    if ( ! check_irn(recnum) ) return 0 ;
  }
  return 1 ;
}

int
check_limit(u_int l)
{
  bss_Result = 0 ;
  if ( l == 0 ) return 1 ;
  if ( ! Cdb.has_lims ) {
    prepare_error(NO_LIMITS, Cdb.name, 0) ;
    return 0 ;
  }
  if ( l > Cdb.highest_lim ) {
    prepare_error(SILLY_LIMIT, itoascii((int)l), 0) ;
    return 0 ;
  }
  if ( prepare_limits(&Cdb) < 0 ) {
    prepare_error(CANT_SET_LIMIT, 0) ;
    return 0 ;
  }
  return 1 ;
}


/* check_attr() checks and partially sets up attribute given by name (OK for
   parsing but not lookup). Returns address of group struct or NULL on
   failure. See also check_index.

   code=0 means check or setup GSL, code=1 means do ixinit as well.

   If this function has been called before with these args, (and the database
   has not been closed), it's only necessary to look at the status field.

   If the Db is open then (as at 2.26) search_groups will have been read,
   stem function name validated and function address obtained, but GSL will
   not have been looked at.

*/

Sg *
check_attr(Db *db, char *attr_name, int code)
{
  Sg *sg = NULL ;
  Ix *ix ;
  /* int result ; unused */
  if ( ! check_db_open(db) ) return sg ;
  /* (Feb 99) If DB is open search group parms have been read, so next line OK,
     but this may change in future. */
  sg = find_search_group_by_name(db, attr_name) ;
  if ( sg == NULL ) {
    prepare_error(NO_SUCH_ATTRIBUTE, attr_name, db->name, 0) ;
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "find_search_group_by_name(%s) returned NULL\n",
	      attr_name) ;
    return NULL ;
  }
  if ( sg->status == SG_COMPLETE ) {  return sg ;}
  if ( sg->status & (SG_GSL_FAIL|SG_INDEX_FAIL) ) {
    prepare_error(CANT_SETUP_ATTRIBUTE, attr_name, 0) ;
    if ( Dbg & D_DB )
      fprintf(bss_syslog, "check_attr(..%s..) status is %d\n",
	      attr_name, sg->status) ;
    return NULL ;
  }
  if ( ! (sg->status & SG_GSL_OK) ) {
    sg->gsl = check_gsl(sg->gsl_name, sg->stem_name, FALSE) ;
    if ( sg->gsl == NULL ) {
      prepare_error(CANT_SETUP_ATTRIBUTE, attr_name, 0) ;
      if ( Dbg & D_DB )
	fprintf(bss_syslog, "check_gsl(%s,%s,0) returned NULL\n",
		sg->gsl_name, sg->stem_name) ;
      sg->status |= SG_GSL_FAIL ;
      return sg ;
    }
    sg->status |= SG_GSL_OK ;
  }
  if ( code == 0 || (code == 1 && (sg->status & SG_INDEX_OK)) ) 
  {
    return sg ;
  }
  ix = check_index(db, sg) ;
  if ( ix == NULL ) {
    sg->status |= SG_INDEX_FAIL ;
    /* DEFECTIVE_INDEX will have been set */
    return NULL ;
  }
  sg->ix = ix ;			/* (already done by check_ix) */
  sg->status |= SG_INDEX_OK ;
  return sg ;
}

/* check_ext_reg_name() checks existence and validity of extraction
   regime. Returns extraction regime if found & valid else sets
   error and returns (Extraction_regime)0. */

Extraction_regime
check_ext_reg(char *name)
{
  struct ext_reg_name *ern = Ext_reg_names ;
  int found = FALSE, valid = FALSE ;

  while ( ern->name ) {
    if ( strcasecmp(name, ern->name) == 0 ) {
      found = TRUE ;
      if ( ern->valid ) valid = TRUE ;
      break ;
    }
    ern++ ;
  }
  if ( found && valid ) return ern->reg ;
  if ( ! found ) prepare_error(NO_SUCH_REGIME, name, 0) ;
  else prepare_error(INVALID_REGIME, name, 0) ;
  return (Extraction_regime)0 ;
}

/* check_stemfn(name)  SW March 99
   Returns pointer to function if found otherwise NULL.
   Note don't need this func if doing check_gsl: if OK stemfunc is
   a member of the GSL struct. */

PFI
check_stemfn(char *name)
{
  int k ;

  for ( k = 0 ; Stem_fns[k].name != NULL ; k++ ) 
    if ( strcasecmp(Stem_fns[k].name, name) == 0 )
      return Stem_fns[k].func ;
  prepare_error(NO_SUCH_STEMFUNCTION, name, 0) ;
  return NULL ;
}
 

/* check_gsl() looks for existing already set up GSL with the given filename
   and stem function name, and tries to set it up if not found. It returns
   a pointer to the setup Gsl if successful (or already set up) or NULL
   and sets errors if GSL file or function not found or can't be set up.
   Call is relatively expensive. */

Gsl *
check_gsl(char *name, char *stemname, BOOL no_db)
{
  PFI stem_fn ;
  Gsl *g ;
  int result ;
  if ( ! (stem_fn = check_stemfn(stemname)) )
  /* NO_SUCH_STEMFUNCTION will be set */
    return NULL ;

  if ( (g = setup_gsl(name, stemname, stem_fn, &result)) == NULL ||
       result < 0 )
    prepare_error(CANT_SETUP_GSL, name, stemname, 0) ;
  else g->no_db = no_db ;
  return g ;
}

/* check_index() appears (Feb 99) only used by stype012()
   See also check_attr(). The search group must have been read OK.
*/

Ix *
check_index(Db *db, Sg *sg)
{
  Ix *index ;

  if ( sg->ix != NULL ) return sg->ix ;
  index = sg->ix = ixinit(db, sg) ;	/* although OK even if done already */
  if ( index == NULL || db->index_status[sg->ixnum] != ST_OK ) {
    prepare_error(DEFECTIVE_INDEX, sg->search_names, 0) ;
    return NULL ;
  }
  return index ;
}

int
check_fdnum(Db *db, int fdnum)
{
  if ( fdnum < 1 || fdnum > db->nf ) {
    prepare_error(NO_SUCH_FIELDNUM, db->name, 0) ;
    return 0 ;
  }
  return fdnum ;
}

/******************************************************************************

  bss_dump_setrec()  SW Nov 94

  Diagnostic function, unlikely to be used by front ends.

  March 00: 1st two cases no longer used, I think

******************************************************************************/

char *null = "(null)" ;

int
bss_dump_setrec(int setnum, char *response, int code)
{
  Setrec *s = Setrecs[setnum] ;
  char *p = response ;
  char *item ;
  if ( ! check_set(setnum) ) return -1 ;
  s = Setrecs[setnum] ;
  
  switch (code) {
  case D_TTF:
    sprintf(p, "ttf %d\n", s->ttf) ; 
    p += strlen(p) ;
    break ;
  case D_NP:
    sprintf(p, "naw %d\n", s->naw) ;
    p += strlen(p) ;
    break ;
  case D_ATTR:
    sprintf(p, "attr_num %d\n", s->attr_num) ;
    p += strlen(p) ;
    break ;
  case D_ALL:
    sprintf(p, "num %d\n", s->num) ; p += strlen(p) ;
    sprintf(p, "ctime %s", ctime((time_t *)&s->ctime)) ; p += strlen(p) ;
    sprintf(p, "atime %s", ctime((time_t *)&s->atime)) ; p += strlen(p) ;
    sprintf(p, "set_type %d ", s->set_type) ; p += strlen(p) ;
    translate_set_type(s->set_type, p) ; p += strlen(p) ;
    if ( s->search_term == NULL || strlen(s->search_term) == 0 ) item = null ;
    else item = s->search_term ;
    sprintf(p, "search_term %s\n", item) ; p += strlen(p) ;
    if ( s->found_term == NULL || strlen(s->found_term) == 0 ) item = null ;
    else item = s->found_term ;
    sprintf(p, "found_term %s\n", item) ; p += strlen(p) ;
    sprintf(p, "search_type %d\n", s->search_type) ; p += strlen(p) ;
    sprintf(p, "search_result %d\n", s->search_result) ; p += strlen(p) ;
    sprintf(p, "attr_num %d\n", s->attr_num) ; p += strlen(p) ;
    sprintf(p, "ixnum %d\n", s->ixnum) ; p += strlen(p) ;
    sprintf(p, "ix %x\n", (u_int)s->ix) ; p += strlen(p) ;
    sprintf(p, "ix_chunk %d\n", s->ix_chunk) ; p += strlen(p) ;
    sprintf(p, "ix_offset %d\n", s->ix_offset) ; p += strlen(p) ;
    sprintf(p, "limits %d\n", s->limits) ; p += strlen(p) ;
    sprintf(p, "adj_no_good %d\n", s->adj_no_good) ; p += strlen(p) ;
    sprintf(p, "op %d %s\n", s->op, translate_opcode(s->op)) ; p += strlen(p) ;
    sprintf(p, "score function %d\n", s->score_function) ; p += strlen(p) ;
    sprintf(p, "n_streams %d\n", s->n_streams) ; p += strlen(p) ;
    sprintf(p, "pstgtype %d ", s->pstgtype) ; p += strlen(p) ;
    translate_pstgtype(s->pstgtype, p) ; p += strlen(p) ;
    sprintf(p, "qtweight %.3f\n", s->qtweight) ; p += strlen(p) ;
    sprintf(p, "qtf %d\n", s->qtf) ; p += strlen(p) ;
    sprintf(p, "k1 %.3f\nk2 %.3f\nk3 %.3f\nk7 %.3f\nk8 %.3f\nb %.3f\n",
	    s->k1, s->k2, s->k3, s->k7, s->k8, s->b) ; p += strlen(p) ;
    sprintf(p, "aw %.3f\ngw %.3f\n", s->aw, s->gw) ; p += strlen(p) ;
    if ( s->mpw >= MAXWEIGHT ) 
      sprintf(p, "mpw INF\nrealmpw INF\n") ;
    else sprintf(p, "mpw %.3f\nrealmpw %.3f\n", s->mpw, s->realmpw) ;
    p += strlen(p) ;
    if ( s->minpw <= MINWEIGHT ) 
      sprintf(p, "minpw -INF\n") ;
    else sprintf(p, "minpw %.3f\n", s->minpw) ;
    p += strlen(p) ;
    sprintf(p, "nmpw %d\n", s->nmpw) ; p += strlen(p) ;
    sprintf(p, "maxweight %.3f\n", s->maxweight) ; p += strlen(p) ;
    sprintf(p, "nmaxweight %d\n", s->nmaxweight) ; p += strlen(p) ;
    sprintf(p, "target number %d\n", s->target_number) ; p += strlen(p) ;
    sprintf(p, "minweight %.3f\n", s->minweight) ; p += strlen(p) ;
    sprintf(p, "ngw %d\n", s->ngw) ; p += strlen(p) ;
    sprintf(p, "naw %d\n", s->naw) ; p += strlen(p) ;
    sprintf(p, "ttf %d\n", s->ttf) ; p += strlen(p) ;
    sprintf(p, "max_tf %d\n", s->max_tf) ; p += strlen(p) ;
    sprintf(p, "weight_dist_grad %d\n", s->weight_dist_grad) ; p += strlen(p) ;
    /* if ( s->weight_dist_grad != 0 ) {*/
    /* Changed March 2000 as the array always has at least one element,
       so might as well display it */
    if ( s->pstgtype & HASWT ) {
      int j ;
      sprintf(p, "weight_dist") ; p += strlen(p) ;
      for ( j = 0 ; j <= s->weight_dist_grad ; j++ ) {
	sprintf(p, " %d", s->weight_dist[j]) ; p += strlen(p) ;
      }
      sprintf(p, "\n") ; p++ ;
      sprintf(p, "weight_floors") ; p += strlen(p) ;
      for ( j = 0 ; j <= s->weight_dist_grad ; j++ ) {
	sprintf(p, " %.3f", s->weight_floors[j]) ; p += strlen(p) ;
      }
      sprintf(p, "\n") ; p++ ;
      sprintf(p, "gran %d\n", s->gran) ; p += strlen(p) ;
    }
    sprintf(p, "totalp %d\n", s->totalp) ; p += strlen(p) ;
    sprintf(p, "total_tf %d\n", s->total_tf) ; p += strlen(p) ;
    sprintf(p, "weightsum %.1f\n", s->weightsum) ; p += strlen(p) ;
    sprintf(p, "weightsumsq %.1f\n", s->weightsumsq) ; p += strlen(p) ;
    sprintf(p, "pf_vol %d\n", s->pf_vol) ; p += strlen(p) ;
#ifdef LARGEFILES
    sprintf(p, 
#ifdef _WIN32
	    "pf_offset %I64d\n",
#else
	    "pf_offset %lld\n", 
#endif
	    (LL_TYPE)s->pf_offset) ;
#else
    sprintf(p, "pf_offset %u\n", (unsigned int)s->pf_offset) ;
#endif
    p += strlen(p) ;
    sprintf(p, "global_pf %x\n", (unsigned int)s->global_pf) ;
    p += strlen(p) ;
#ifdef LARGEFILES
    sprintf(p,
#ifdef _WIN32
	    "global_pf_offset %I64d\n",
#else
	    "global_pf_offset %lld\n",
#endif
	    (LL_TYPE)s->global_pf_offset) ;
#else
    sprintf(p, "global_pf_offset %u\n", (unsigned int)s->global_pf_offset) ;
#endif
    p += strlen(p) ;
    sprintf(p, "output_flags %d %d\n", s->output_flags[0], s->output_flags[1]) ;
    p += strlen(p) ;
    translate_output_flags(s->output_flags[0], p) ; p+= strlen(p) ;
    translate_output_flags(s->output_flags[1], p) ; p+= strlen(p) ;
    sprintf(p, "pfiles %x %x\n",
	    (unsigned int)s->pfile[0], (unsigned int)s->pfile[1]) ;
    p += strlen(p) ;
    sprintf(p, "np %d %d\n", s->np[0], s->np[1]) ;
    p += strlen(p) ;
    sprintf(p, "global_pf_length %d\n",
	    s->global_pf_length) ;
    p += strlen(p) ;
    sprintf(p, "pstgs_sizes %u %u\n",
	    s->pstgs_size[0], s->pstgs_size[1]) ;
    p += strlen(p) ;
    sprintf(p, "pbase %x %x\n",
	    (unsigned int)s->pbase[0], (unsigned int)s->pbase[1]) ;
    p += strlen(p) ;
    sprintf(p, "pptr %x %x\n",
	    (unsigned int)s->pptr[0], (unsigned int)s->pptr[1]) ;
    p += strlen(p) ;
    sprintf(p, "next_pstgs %d %d\n",
	    s->next_pstg[0], s->next_pstg[1]) ;
    p += strlen(p) ;
    sprintf(p, "membase %x %x\n",
	    (unsigned int)s->membase[0], (unsigned int)s->membase[1]) ;
    p += strlen(p) ;
    sprintf(p, "memsize %u %u\n", s->memsize[0], s->memsize[1]) ;
    p += strlen(p) ;
    sprintf(p, "pstg %x\n", (unsigned int)s->pstg) ;
    p += strlen(p) ;
    dump_unpacked_pstg(s->pstg, s->pstgtype, p, 0) ; p += strlen(p) ;
    sprintf(p, "raw_plength %d\n", s->raw_plength) ;
    p += strlen(p) ;

    break ;
  default:
    break ;
  }
  return 0 ;
}

/******************************************************************************

  bss_display_setstats()  SW June 95

  Formats weightsum and weightsumsq into supplied buf

******************************************************************************/

int
bss_display_setstats(int setnum, char *buf)
{
  if ( ! check_set(setnum) ) return -1 ;
  sprintf(buf, "sum=%.1f sumsq=%.1f\n", Setrecs[setnum]->weightsum,
	  Setrecs[setnum]->weightsumsq) ;
  return 0 ;
}

/******************************************************************************

  bss_describe_database(name, buf, code)  SW Oct 96
  Returns 1 if OK, otherwise value from read_db_desc()
  (but ignored by bss_describe_databases())

  bss_describe_databases(buf, code)

  bss_describe_attribute(name, buf)

  bss_describe_attributes(buf, code)

  Unfinished. The "code" arg should allow degrees of fullness
  Last func not done yet.

******************************************************************************/

int
bss_describe_database(char *db_name, char *buf, int code)
{
  char *p = buf ;
  Db *tempdb ;
  int result = 1 ;
  int mallocd = 0 ;

  if ( db_name == NULL ) {
    if ( ! check_db_open(&Cdb) ) return -1 ;
    tempdb = &Cdb ;
  }
  else {
    tempdb = (Db *)bss_calloc(1, sizeof(Db)) ;
    if ( tempdb == NULL ) return ST_FAIL_MEM ;
    mallocd = sizeof(Db) ;

    bss_Result = 0 ;
    if ( ! db_availq(db_name) ) {
      /* This can't be the right level? */
      prepare_error(DB_NOT_AVAIL, db_name) ;
      return ST_FAIL_MISC ;
    }
    result = read_db_desc(db_name, tempdb) ;
  }
  if ( result == 1 ) {
    sprintf(p, "name=%s desc=\"%s\" nrecs=%d nfields=%d",
	    tempdb->name, tempdb->display_name, tempdb->nr, tempdb->nf) ;
    p += strlen(p) ;
    sprintf(p, "\n") ;
  }
  if ( mallocd ) {
    free_buf(tempdb) ;
    bss_free(tempdb) ;
  }
  return result ;
}

void
bss_describe_databases(char *buf, int code)
{
  int i ;
  char *ip ;
  char *op = buf ;
  if ( read_db_avail() <= 0 ) sprintf(op, "No databases\n") ;
  else if ( read_db_avail() == 1 ) sprintf(op, "1 database\n") ;
  else sprintf(op, "%d databases\n", Db_num) ;
  op += strlen(op) ;
  for ( i = 0, ip = Db_names ; i < Db_num ; i++ ) {
    bss_describe_database(ip, op, 0) ;
    ip += strlen(ip) + 1 ;
    op += strlen(op) ;
  }
}

static void
describe_attribute(Sg *sg, char *buf)
{
  char *p = buf ;
  short *i = sg->fields ;
  sprintf(p,
  "primary-name=%s index-num=%d index-type=%d ext-reg=%s stem-func=%s gsl=%s",
  sg->search_names, sg->ixnum, sg->itype,
	  sg->reg_name, sg->stem_name, sg->gsl_name ) ;
  p += strlen(p) ;
  sprintf(p, " fields=") ;
  p += strlen(p) ;
  while ( *i > 0 ) {
    sprintf(p, "%hd,", *i++) ;
    p += strlen(p) ;
  }
  sprintf(--p, "\n") ;
}

/* Note search_groups are read when and only when database is opened */

int
bss_describe_attribute(char *attr_name, char *buf)
{
  Sg *sg ;
  if ( ! check_db_open(&Cdb) ) return -1 ;
  sg = find_search_name(&Cdb, attr_name) ;
  if ( sg == NULL ) return -1 ;	/* Error handled by find_search_name() */
  describe_attribute(sg, buf) ;
  return 0 ;
}

/******************************************************************************

  find_search_name()

  Put here from bss_db Dec 94. Top level version of find_search_group_by_name()

******************************************************************************/

Sg *
find_search_name(Db *db, char *attr)
{
  Sg *result = NULL ;
  if ( check_db_open(db) ) {
    result = find_search_group_by_name(db, attr) ;
    if ( result == NULL ) 
      prepare_error(NO_SUCH_ATTRIBUTE, attr, db->name, 0) ;
  }
  return result ;
}

/******************************************************************************

  Freeing sets

  Moved here from bss_misc.c Dec 94  SW

  No errors set, but bss_clear_setrec() returns 1 if try to free an unused set.

  bss_clear_all_setrecs() returns the number of sets freed.

  Note take care not to close file 0 if the set is the "dummy" setrec
  Setrecs[MAXSETRECS]. This is /dev/null, or stdnothing, and always left
  open.

******************************************************************************/

int
bss_clear_setrec(int num)
{
  int i ;

  Setrec *srec = Setrecs[num] ;
  char fname[PATHNAME_MAX] ;
  
  /* Sep 00: no longer resets bss_Result as this can nullify the effect
     of a previous error (2.31) */
  /* bss_Result = 0 ; */
  if ( srec == NULL ) return 1 ; /* Nothing to do */
  if ( srec == Cdb.current_record_srec ) clear_current_displayset() ;

  for ( i = 0 ; i < 2 ; i++ ) {
    if ( num != MAXSETRECS ) {
      if ( srec->output_flags[i] & SET_FOPEN) {
	if ( srec->pfile[i] ) set_close_pfs(num) ; /* Condition redundant */
      }
      if ( (srec->output_flags[i] & SET_FILE_EXISTS) &&
	  ! (srec->output_flags[i] & SET_FILE0_PF) ) {
	bss_pfname(fname, srec->num, i) ;
	unlink(fname) ;
      }
    }
    if ( srec->output_flags[i] & SET_MEM_MALLOC ) {
      bss_free(srec->membase[i]) ;
      if ( Dbg & DD_MEM )
	fprintf(bss_syslog, "Freed %u bytes set postings memory\n",
		srec->memsize[i]) ;
    }
#ifndef NO_MMAP
    else if ( srec->output_flags[i] & SET_MEM_MAPPED ) {
      bss_munmap(srec->membase[i], srec->memsize[i]/Pagesize) ;
      if ( Dbg & DD_MEM )
	fprintf(bss_syslog, "Unmapped %u bytes set postings memory\n",
		srec->memsize[i]) ;
    }
#endif
  }

  if ( srec->search_term != NULL ) bss_free((void *)srec->search_term) ;
  if ( srec->found_term != NULL ) bss_free((void *)srec->found_term) ;
  if ( srec->scores != NULL ) bss_free((void *)srec->scores) ;
  if ( srec->bkts != NULL ) {
    int i ;
    struct rbkt *bkt ;
    struct rrec *r, *tr ;
    for ( i = 0, bkt = srec->bkts ; i <= srec->gran ; i++, bkt++ ) {
      r = bkt->rlist ;
      while ( r != NULL ) {
	tr = r ;
	r = r->next ;
	bss_free((void *)tr) ;
      }
    }
    bss_free((void *)srec->bkts) ;
  }
  if ( srec->pstg != NULL ) bss_free((void *)srec->pstg) ;
  bss_free(srec) ;
  Setrecs[num] = (Setrec *)NULL ;
  if ( num < MAXSETRECS ) Num_Setrecs-- ;
  return 0 ;
}

/* Obsolescent. Used to free memory but now all done by clear_setrec() */

int
bss_purge_setrec(int num)
{
  return bss_clear_setrec(num) ;
}

int
bss_clear_all_setrecs(void)
{
  int j, count = 0, res ;

  bss_Result = 0 ;
  for ( j = 0 ; j < MAXSETRECS ; j++ )
    if ( (res = bss_clear_setrec(j)) == 0 ) count++ ;
  return count ;
}

/******************************************************************************

  get_rec_info_text(recnum)

  Make document info string using paragraph records if available

  Was taking < 1sec / 1000 in issun6 on trec12_94

  Transferred from bss_recs.c May 95

******************************************************************************/

int
get_rec_info_text(int irn, char *outstring)
{
  int *p ;
  char *pp ;
  int reclen ;
  int lims ;
  int fdlen, numpars ;
  int i ;
  char *op = outstring ;
  int result ;
  OFFSET_TYPE bib_offset ;
  int para_offset = -1 ;	/* Dummy for non-text databases */

  if ( ! check_db_open(&Cdb) ) return -1 ;
  if ( ! check_irn(irn) ) return -1 ;
  prepare_limits(&Cdb) ;
  bib_offset = recnum_to_addr(irn, &Cdb, 0, &reclen) ;
  if ( sizeof(bib_offset) == 8 )
    sprintf(op, "irn=%d bib_offset=%llu",
	  irn, bib_offset * (OFFSET_TYPE)Cdb.rec_mult) ;
  else sprintf(op, "irn=%d bib_offset=%.0f",
	  irn, (double)bib_offset * (double)Cdb.rec_mult) ;
  op += strlen(op) ;
  if ( Cdb.nf > 1 || Cdb.no_drl ) {
    result = rd_recnum(irn, -2, &Cdb) ; /* -2 makes it read header only */
    pp = Cdb.recbuf + Cdb.fixed ;
    if ( Cdb.fddir_recsize == 2 ) {
      for ( i = 1, reclen = 0 ; i <= Cdb.nf ;
	    i++, reclen += cv2hl(pp), pp += Cdb.fddir_recsize ) ;
    }
    else 
      for ( i = 1, reclen = 0 ; i <= Cdb.nf ;
	    reclen += cv3hl(pp), i++, pp += Cdb.fddir_recsize ) ;
  }
  /* else already have record length */
  lims = Getlimit(irn) ;
  sprintf(op, " len=%d lims=%d", reclen, lims) ;
  op += strlen(op) ;

  if ( prepare_paras(&Cdb, 1) > 0 ) {
    /* Changed prepare_paras() code from 0 to 1 Sep 01 to avoid coredump
       if pardir is short */
    para_offset = get_para_offset(irn) ;
    p = get_para_rec(irn) ;
    p += 3 ;			/* 1st 3 fields no longer used (were only
				   in parafile type 0 -- obsolete) */
    for ( i = 1 ; i <= Cdb.nf ; i++ ) {
      p++ ; p++ ;			/* Skip fdnum, oset */
      fdlen = *p++ ;
      numpars = *p++ ;
      sprintf(op, " fd=%d len=%d paras=%d", i, fdlen, numpars) ;
      op += strlen(op) ;
      p += numpars * 2 ;
    }
  }
  else {
    if ( Cdb.nf == 1 && ! Cdb.no_drl ) {
      sprintf(op, " fd=1 len=%d", fdlen = reclen) ;
      op += strlen(op) ;
      if ( fdlen == 0 ) sprintf(op, " paras=0") ;
      else sprintf(op, " paras=1") ;
      op += strlen(op) ;
    }
    else { 
      pp = Cdb.recbuf + Cdb.fixed ;
      for ( i = 1 ; i <= Cdb.nf ; i++, pp += Cdb.fddir_recsize ) {
	if ( Cdb.fddir_recsize == 2 ) 
	  sprintf(op, " fd=%d len=%d", i, fdlen = cv2hl(pp)) ;
	else
	  sprintf(op, " fd=%d len=%d", i, fdlen = cv3hl(pp)) ;
	op += strlen(op) ;
	if ( fdlen == 0 ) sprintf(op, " paras=0") ;
	else sprintf(op, " paras=1") ;
	op += strlen(op) ;
      }
    }
  }
  sprintf(op, " para_offset=%d", para_offset) ;
  op += strlen(op) ;
  sprintf(op ,"\n") ;
  if ( Dbg & D_PARAS ) fprintf(bss_syslog, "%s", outstring) ;
  return 0 ;
}

/******************************************************************************

  bss_get_big_n(limit)   SW May 95

  Returns the big N value corresponding to limit mask or - on error
  Assumes that Cdb.bign corresponds to Cdb.current_limit (see bss_set_limit()).

  (Feb 2000) Don't think we need the function get_cttf, can be done here.
  Snag is doclen is chars (includes stopwords etc) not number of indexed
  tokens. Assume # tokens is doclen/11 generally near enough. This'll
  need changing when lengths in tokens are available (see note in
  "notes_and_reminders").

******************************************************************************/

#define LEN_TO_TOK_FACTOR 11
#define CH8K_TO_TERMS_FACTOR 270 /* Typical kwds per 8K chunk
				    (temp, for bss_get_tnt) */

int
bss_get_big_n(u_int limit)
{
  if ( ! check_db_open(&Cdb) || ! check_limit(limit) ) return -1 ;
  if ( limit == Cdb.current_limit ) return Cdb.big_n ;
  return get_bign(&Cdb, limit) ;
}

int
#ifdef HAVE_LL
bss_get_avedoclen(char *attr, u_int limit, LL_TYPE *cttf)
#else
bss_get_avedoclen(char *attr, u_int limit, double *cttf)
#endif
{
  Sg *g ;
  int N ;
  int res ;
  g = check_attr(&Cdb, attr, 1) ;
  if ( g == NULL ) return -1 ;
  if (! check_limit(limit) ) return -1 ;

  if (read_doclens(&Cdb, g->ixnum) >= 0 ) {
    res = get_avedoclen(&Cdb, g->ixnum, limit) ;
    if ( res >= 0 ) {
      N = bss_get_big_n(limit) ;
      *cttf = (double)res / LEN_TO_TOK_FACTOR * N ;
    }
    else *cttf = -1 ;
    return res ;
  }
  else {
    prepare_error(CANT_GET_DOCLENS, attr) ;
    *cttf = -1 ;
    return -1 ;
  }
}

int
bss_get_tnt(char *attr, u_int limit)
/* NB limit ignored */
{
  Sg *sg ;
  int numchunks, tnt ;

  /* check_attr with code 1 will setup index if not already done */
  if ( (sg = check_attr(&Cdb, attr, 1)) == (Sg *)NULL ) return -1 ;
  if ( sg->ix->tnt > 0 ) return sg->ix->tnt ;
  /* else estimate it (old index) */
  numchunks = ofsize(sg->ix->sifd) / sg->ix->schunksize ;
  /* fprintf(stderr, "Numchunks for attr %s = %d\n", attr, numchunks) ; */
  tnt = numchunks * CH8K_TO_TERMS_FACTOR * sg->ix->schunksize / SCHUNKSIZE ;
  return tnt ;
}

int
bss_get_doclen(Db *db, char *attr, int setnum, int recnum)
{
  int irn ;
  struct search_group *sg ;

  if ( ! check_db_set_rec_irn(db, setnum, recnum) ) return -1 ;
  if ( ! (sg = check_attr(db, attr, 1)) ) return -1 ;
  if (read_doclens(db, sg->ixnum) < 0 ) {
    prepare_error(CANT_GET_DOCLENS, attr) ;
    return -1 ;
  }
  if ( setnum != -1 ) {
    if ( prepare_set_for_output(setnum, recnum) < 0 ) return -1 ;
    if ( get_pstg_n(Setrecs[setnum], recnum) == NULL ) return -1 ;
    else irn = Setrecs[setnum]->pstg->rec ;
  }
  else irn = recnum ;
  return get_doclen(db, sg->ixnum, irn) ;
} 

double
bss_get_rscore(Db *db, char *attr, u_int limit, int n, int R, int r)
/* NB limit ignored */
{
  int tnt ;
  int N ;
  double score ;
  tnt = bss_get_tnt(attr, limit) ;
  if ( tnt < 0 ) return tnt ;	/* check_attr will have prepared the error */
  if ( limit == 0 ) N = db->nr ;
  else if ( limit == db->current_limit ) N = db->big_n ;
  else N = calc_bign(db, limit) ;
  score = rscore(N, n, R, r, tnt) ;
  if ( score <= -400 ) 
    prepare_error(PHONEY_RSCORE, itoascii(R), itoascii(r),
		  itoascii(N), itoascii(n), 0) ;
  return score ;
}

int
bss_get_db_name(char *buf)
{
  bss_Result = 0 ;
  if ( Cdb.name == NULL ) sprintf(buf, "No database\n") ;
  else sprintf(buf, "%s\n", Cdb.name) ;
  return 0 ;
}

int
bss_get_stemfunctions(char *buf)
{
  int j, k ;
  char *p ;
  bss_Result = 0 ;
  /* Count the functions */
  for ( j = 0 ; Stem_fns[j].func != NULL ; j++ ) ;
  sprintf(buf, "%d function(s)\n", j) ;
  for ( k = 0, p = buf + strlen(buf) ; k < j ; k++ ) {
    sprintf(p, "%s\n", Stem_fns[k].name) ;
    p += strlen(p) ;
  }
  return j ;
}

int
bss_get_regimes(char *buf)
{
  struct ext_reg_name *ern = Ext_reg_names ;
  int count = 0 ;
  char *p ;

  while ( ern->name ) {
    if ( ern->valid ) count++ ;
    ern++ ;
  }
  sprintf(buf, "%d regime(s)\n", count) ;
  ern = Ext_reg_names ;
  p = buf + strlen(buf) ;
  while ( ern->name ) {
    if ( ern->valid ) {
      sprintf(p, "%s\n", ern->name) ;
      p += strlen(p) ;
    }
    ern++ ;
  }
  return count ;
}

void
bss_get_version(char *buf) 
{
  sprintf(buf, "%s %s\n", version, vdate) ;
}

/******************************************************************************

  bss_do_trec_stats(setnum, R)   SW Oct 95

  Writes TREC evaluation statistics to supplied buffer

  Goes through a set which has had a MARK done on it so it has flags fields
  in the postings, and calculates various statistics

  May 96: argument n added, does it on n instead of setsize; if n = 0 uses
  setsize

******************************************************************************/

int
bss_do_trec_stats(char *buf, int setnum, int mark, int R, int flags,
		  int n, int code)
     /* mark ;  Really char */
     /* flags ; Can take the FFL_MEM bit and possibly others in the future */
     /* n ;	Number off top to do stats on */
     /* code ;	June 97: 	   0 gives avep, 5, 30, 100, 200, rprec, rcl;
				   1 adds P10, P15 and P20
				   2 gives avep and recall only
		June 99:         101-103 and 201-202 give filtering function values
		+ avep and recall. See the code for details. */
{
  Setrec *srec ;
  int j ;
  double numrel = 0 ;
  int num_to_do ;
  bss_Gp *pstg ;
  int setsize ;
  double p_sum = 0.0 ;
  double avep = 0.0, rprec = 0.0, rcl = 0.0 ;
  double P5 = 0.0, P10 = 0.0, P15 = 0.0, P20 = 0.0, P30 = 0.0,
  P100 = 0.0, P200 = 0.0, P1000 = 0.0 ;
  double funcval = 0 ;
  double maxval = 0 ;
  int maxn = 0 ;
  double maxw ;
  double nrmult = 0, nnrmult = 0 ;
  double nlpower = 1 ;
  BOOL functype = FALSE ;
  static char *func = "bss_do_trec_stats" ;

  if ( ! check_db_open(&Cdb) ) return -1 ;
  if ( ! check_set(setnum) ) return -1 ;
  if ( buf == NULL ) {
    internal_err(func, "null buffer") ;
    return -1 ;
  }
  switch (code) {
  case TS_LF1:
    functype = TRUE ;
    nrmult = 3 ;
    nnrmult = 2 ;
    break ;
  case TS_LF2:
    functype = TRUE ;
    nrmult = 3 ;
    nnrmult = 1 ;
    break ;
  case TS_LF3:
    functype = TRUE ;
    nrmult = 4 ;
    nnrmult = 1 ;
    break ;
  case TS_NF1:
    functype = TRUE ;
    nrmult = 6 ;
    nnrmult = 1 ;
    nlpower = 0.5 ;
    break ;
  case TS_NF2:
    functype = TRUE ;
    nrmult = 6 ;
    nnrmult = 1 ;
    nlpower = 0.8 ;
    break ;
  }
  srec = Setrecs[setnum] ;
  setsize = srec->naw ;
  num_to_do = (n <= setsize && n > 0) ? n : setsize ;
  if ( num_to_do > 0 ) {
    if ( prepare_set_for_output(setnum, num_to_do - 1) < 0 )
      return -1 ;
    if ( functype ) maxw = (get_pstg_n(srec, 0))->score ;
    for ( j = 1 ; j <= num_to_do ; j++ ) {
      pstg = get_pstg_n(srec, j - 1) ;
      if ( (pstg->flags & mark) ) {
	numrel++ ;
	p_sum += numrel / (double)j ;
	if ( functype ) {
	  funcval = pow(numrel, nlpower) * nrmult - (j - numrel) * nnrmult ;
	  if ( funcval >= maxval ) { /* >= because want greatest n which gives max */
	    maxval = funcval ;
	    maxn = j ;
	    maxw = pstg->score ;
	  }
	}
      }
      if ( j == R ) rprec = numrel ;
      switch (j) {
      case 5:
        P5 = numrel ;
        break ;
      case 10:
        P10 = numrel ;
        break ;
      case 15:
        P15 = numrel ;
	break ;
      case 20:
	P20 = numrel ;
	break ;
      case 30:
	P30 = numrel ;
	break ;
      case 100:
	P100 = numrel ;
	break ;
      case 200:
	P200 = numrel ;
	break ;
      case 1000:
	P1000 = numrel ;
      }
    }
    if ( functype ) {		/* Calc final func vals  */
      funcval = pow(numrel, nlpower) * nrmult - (num_to_do - numrel) * nnrmult ;
      if ( funcval >= maxval ) {
	maxval = funcval ;
	maxn = num_to_do ;
	maxw = pstg->score ;
      }
    }
    if ( R < numrel ) {
      /* This is an error & at least a warning should be issued */
      fprintf(bss_syslog,
 "Warning: do_trec_stats(): bigr=%d < numrel=%d, setting bigr=numrel\n",
	      R, (int)numrel) ;
      R = rprec = /* (the final value of) */ numrel ;
    }
    if ( num_to_do < R ) rprec = numrel ; /* This was setsize, a mistake */
    if ( R > 0 ) {
      avep = p_sum / (double)R ;
      rcl = /* final */ numrel / (double)R ;
      rprec /= (double)R ;
    }
    if ( num_to_do < 1000 ) {
      P1000 = numrel ;
      if ( num_to_do < 200 ) {
	P200 = numrel ;
	if ( num_to_do < 100 ) {
	  P100 = numrel ;
	  if ( num_to_do < 30 ) {
	    P30 = numrel ;
	    if ( num_to_do < 20 ) {
	      P20 = numrel ;
	      if ( num_to_do < 15 ) {
		P15 = numrel ;
		if ( num_to_do < 10 ) {
		  P10 = numrel ;
		  if ( num_to_do < 5 ) P5 = numrel ;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  if ( functype ) {
    sprintf(buf, "Val=%.3f Maxval=%.3f Wt=%.3f Maxn=%d Avep=%.3f Rcl=%.3f\n",
	    funcval, maxval, maxw, maxn, avep, rcl) ;
  }
  else {
    switch (code) {
    case TS_STANDARD:
      sprintf(buf,
	      "Avep=%.3f P5=%.3f P30=%.3f P100=%.3f P200=%.3f P1000=%.3f Rprec=%.3f Rcl=%.3f\n",
	      avep, P5/5, P30/30, P100/100, P200/200, P1000/1000, rprec, rcl) ;
      break ;
    case TS_LONG:
      sprintf(buf,
	      "Avep=%.3f P5=%.3f P10=%.3f P15=%.3f P20=%.3f P30=%.3f \
P100=%.3f P200=%.3f P1000=%.3f Rprec=%.3f Rcl=%.3f\n",
	      avep, P5/5, P10/10, P15/15, P20/20, P30/30,
	      P100/100, P200/200, P1000/1000, rprec, rcl) ;
      break ;
    case TS_SHORT:
      sprintf(buf, "Avep=%.3f Rcl=%.3f\n", avep, rcl) ;
      break ;
    }
  }
  close_pstgs(srec, 0, 0) ;
  return 0 ;
}

int
bss_set_debug(int v)
{
  int old_debug = Dbg ;
  if ( old_debug != v ) {
    if ( old_debug == 0 ) 
      fprintf(bss_syslog, "BSS version %s vdate %s LOG, PID %d\n",
	      version, vdate, Pid) ;
    Dbg = v ;
    fprintf(bss_syslog, "Debug changed from %d to %d\n",
	    old_debug, Dbg) ;
  }
  return old_debug ;
}

/******************************************************************************
 
  bss_calc_dbg

  Decodes a sequence of debug names into a value and returns the value

  If a name is not found the return value is -(N + 1) where N is the
  offset in bytes of the name in the string

  Changed July 2000 to allow any of -_%$ in debug name

******************************************************************************/
int
bss_calc_debug(char *names)
{
  int code = 0 ;
  char name[NNAME_MAX] ;
  int olen, adv ;
  char *ip = names ;
  int res = 0 ;
  int dummy ;

  while ( strlen(ip) > 0 ) {
    adv = extwd((u_char *)ip, strlen(ip), (u_char *)name, &olen,
		(u_char *)" ,\t:;|", (u_char *)"_-$%", &dummy) ;
    name[olen] = '\0' ;
    res = find_name(name, debug_names) ;
    if ( res == 0 ) {		/* Name not found */
      prepare_error(NO_SUCH_DEBUG_NAME, name, 0) ;
      return -(ip - names + 1) ;
    }
    code |= res ;
    ip += adv ;
  }
  return code ;
}

