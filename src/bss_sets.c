/******************************************************************************

  bss_sets.c SW Jan 96-

  Gathering set functions together

******************************************************************************/

/******************************************************************************

  bss_pstgs.c  SW May 92

  Functions for handling postings for BSS

  Derived from mergesubs.c and searchsubs.c

******************************************************************************/

#include "bss.h"
#include "bss_errors.h"
#ifndef NO_MMAP
#include <sys/mman.h>
#endif

/******************************************************************************

  make_bss_Gp(maxpos)    SW May 94

  Creates bss_Gp by mallocing to size implied by maxpos, max # of pos
  recs.

  Nov 94: changed malloc to calloc

  Jan 96: note there is a redundant extra byte allocated when maxpos = 0

******************************************************************************/

static bss_Gp *
resize_bss_Gp(bss_Gp *pstg, int newmaxpos)
{
  bss_Gp *res ;
  int oldmaxpos, amount ;
  if ( pstg == NULL ) return NULL ;
  if ( pstg->maxpos == newmaxpos) return pstg ;
  if ( Dbg & DD_PSTGS ) oldmaxpos = pstg->maxpos ;
  amount = sizeof(bss_Gp) + (newmaxpos + 1) * sizeof(U_pos) ;
  res = (bss_Gp *)bss_realloc((char *)pstg, amount) ;
  if ( res != NULL ) {
    res->maxpos = newmaxpos ;
    res->p = (U_pos *)((char *)res + sizeof(bss_Gp)) ;
  }
  else prepare_error(SYS_NO_MEM_1, "make_bss_Gp()", itoascii(amount), 0) ;
  if ( Dbg & DD_PSTGS ) {
    if ( res == NULL ) fprintf(Err_file, "Resize posting failed\n") ;
    else
      fprintf(bss_syslog, "Resized posting from %d to %d\n",
	      oldmaxpos, newmaxpos) ;
  }
  return res ;
}

bss_Gp *
make_bss_Gp(int maxpos)
{
  bss_Gp *res ;
  int amount = sizeof(bss_Gp) + (maxpos + 1) * sizeof(U_pos) ;
  res =
    (bss_Gp *)bss_calloc(amount, 1) ;
  if ( res != NULL ) {
    res->maxpos = maxpos ;
    res->p = (U_pos *)((char *)res + sizeof(bss_Gp)) ;
  }
  else prepare_error(SYS_NO_MEM_1, "make_bss_Gp()", itoascii(amount), 0) ;
  if ( Dbg & DD_PSTGS )
    if ( res != NULL )
      fprintf(bss_syslog, "Made bss_Gp size %d\n", res->maxpos) ;
  return res ;
}

/******************************************************************************

  set_alloc_mem(srec, msize)
  set_get_zeroed_mem(srec, msize, hold)

  Initial allocation of memory for set's raw postings
  Called by get_setrec() and setup_pstgs()

******************************************************************************/

static char *
set_alloc_mem(Setrec *srec, int msize)
{
  srec->membase[0] = srec->pptr[0] = srec->pbase[0] = bss_malloc(msize) ;
  if ( srec->membase[0] != NULL ) {
    srec->output_flags[0] |= (SET_MEM|SET_MEM_MALLOC) ;
    srec->memsize[0] = msize ;
  }
  return srec->membase[0] ;
}

/* Prob no longer used (Dec 98) */

static char *
set_get_zeroed_mem(Setrec *srec, int msize, BOOL hold)
     /* hold ;	Hold memory (nonsequential) if true */
{
  int pages ;
  int ps = Pagesize ;

  pages = iceil(msize, ps) ;
  srec->membase[0] = srec->pptr[0] = srec->pbase[0] =
    get_zeroed_mem(pages, hold) ;
  if ( srec->membase[0] != (char *)-1 ) {
    srec->output_flags[0] |= (SET_MEM|SET_MEM_MAPPED) ;
    srec->memsize[0] = pages * ps ;
    return srec->membase[0] ;
  }
  else return NULL ;
}
  

/******************************************************************************

  get_dummyset(psize)

  Used when no output is required, just np etc. In this case the file
  (assumed open) stdnothing is attached. Flag values are ignored.

  Jan 03: correction: set not alloc'd after first time (side effect
  of change to bss_clear_setrec())

******************************************************************************/

Setrec *
get_dummyset(void)
{
  Setrec *srec ;

  (void)bss_clear_setrec(MAXSETRECS) ; /* Harmless if NULL */
  srec = (Setrec *)bss_calloc(1, sizeof(Setrec)) ;
  if ( srec == NULL ) {
    prepare_error(SYS_NO_MEM_1, "get_dummyset", itoascii(sizeof(Setrec)), 0) ;
    return (Setrec *)NULL ;
  }

  srec->num = MAXSETRECS ;
  srec->pfile[0] = stdnothing ;
  srec->output_flags[0] |= (SET_FOPEN_WRITE|SET_FILE) ;
  return (Setrecs[MAXSETRECS] = srec) ;
}


/******************************************************************************

  get_setrec(psize, msize, flags, pstgtype)

  psize is the initial number of positional fields to allow in the pstg field.
  msize is the initial amount of allocated memory for raw postings.

  flags take the FFL_ bit values (see bss.h)

  Doesn't handle file opening etc, but will allocate memory if msize positive
  and flags & FFL_MEM. Probably, though, this is mostly done in other
  functions.

  See also get_dummyset()

  March 96: made this function set output_flags to either SET_MEM or SET_FILE
  depending on flags value and on the define POSTING_INDEX_FILE (undef for now).

  Apr 00: pstgtype arg now used

******************************************************************************/

Setrec *
get_setrec(int psize, int msize, int flags, int pstgtype)
{
  char *func = "get_setrec" ;
  int i ;
  Setrec *srec = (Setrec *)NULL ;
  int ps = Pagesize ;

  bss_Result = 0 ;
  if ( flags & FFL_NOSAVE) srec = get_dummyset() ;
  else {			/* Find one */
    if ( Num_Setrecs >= MAXSETRECS ) {
      prepare_error(NO_FREE_SETS, 0) ;
      return srec ;
    }
    for ( i = 0 ; i < MAXSETRECS && Setrecs[i] != NULL ; i++ ) ;
    if ( i < MAXSETRECS ) {
      srec = Setrecs[i] = (Setrec *)bss_calloc(1, sizeof(Setrec)) ;
      if ( srec == NULL ) {
	prepare_error(SYS_NO_MEM_1,
		      "get_setrec", itoascii(sizeof(Setrec)),	0) ;
	return srec ;
      }
      if ( Dbg & DD_MEM )
	fprintf(bss_syslog, "%s(): malloc %d for setrec\n",
		func, sizeof(Setrec)) ;
      if ( msize > 0 && (flags & FFL_MEM) ) {
#ifdef HAVE_MMAP_ZERO
	if ( msize < MEM_BREAK_1 * ps ) {
	  if ( set_alloc_mem(srec, msize) == NULL ) {
	    bss_clear_setrec(i) ;
	    prepare_error(SYS_NO_MEM_1, "get_setrec", itoascii(msize), 0) ;
	    return NULL ;
	  }
	  if ( Dbg & DD_MEM )
	    fprintf(bss_syslog, "%s(): malloc %d for set postings memory\n",
		    func, msize) ;
	}
	else {
	  if ( set_get_zeroed_mem(srec, msize, flags & FFL_HOLD_MEM) == NULL ){
	    bss_clear_setrec(i) ;
	    prepare_error(SYS_NO_MEM_1, "get_setrec", itoascii(msize), 0) ;
	    return NULL ;
	  }
	  if ( Dbg & DD_MEM )
	    fprintf(bss_syslog, "%s(): mapped %d for set postings memory\n",
		    func, msize) ;
	}
#else
	if ( set_alloc_mem(srec, msize) == NULL ) {
	  bss_clear_setrec(i) ;
	  prepare_error(SYS_NO_MEM_1, "get_setrec", itoascii(msize), 0) ;
	  return NULL ;
	}
	if ( Dbg & DD_MEM )
	  fprintf(bss_syslog, "%s(): malloc %d for set postings memory\n",
		  func, msize) ;
#endif
      }
      Num_Setrecs++ ;
      srec->num = i ;
    }
    else {
      prepare_error(NO_FREE_SETS, 0) ;
      return srec ;
    }
  }
  srec->pstg = make_bss_Gp(psize) ; 
  if ( srec->pstg == NULL ) {
    bss_clear_setrec(i) ;
    prepare_error(SYS_NO_MEM_1, "make_bss_Gp",
		  itoascii((psize + 1) * 4 + sizeof(bss_Gp)), 0) ;
    return NULL ;
  }
  if ( Dbg & DD_MEM )
    fprintf(bss_syslog, "%s(): malloc %d for set structured posting\n",
	    func, (psize + 1) * 4 + sizeof(bss_Gp)) ;
  if ( flags & FFL_MEM ) srec->output_flags[0] |= SET_MEM ;
  else srec->output_flags[0] |= SET_FILE ;
  /* Should depend on flags in some way? */
#ifndef POSTING_INDEX_FILE
  srec->output_flags[1] |= SET_MEM ;
#else
  srec->output_flags[1] |= SET_FILE ;
#endif
  if ( pstgtype & HASWT ) {
    srec->maxweight = MINWEIGHT ;
    srec->minweight = MAXWEIGHT ;
  }
  srec->pstgtype = pstgtype ;
  srec->ctime = srec->atime = time(NULL) ;
  return srec ;
}

/******************************************************************************

  setup_pstgs()

  was bss_storepstgs() SW Apr 92

  (March 96) This is for atomic sets. If set is limited or positional fields
  have to be removed it is called by bss_lkup(), and does just the subfunction
  immediately below this

  If to go in memory
   if not done already
     if atomic
       if no limit map in from the global postings file
       else map in as above then filter it into malloc'd memory
            and unmap
     else {from merge} allocate memory based on the max_tnp estimate.
     Set membase, memsize, pbase, pptr
     Set MEM, MEM_MALLOC or MEM_MAPPED
  else {file}
    if ! FILE_EXISTS
      if atomic
        if no limit {nothing to do}
        else seek in global pf and filter into own file
      else open own file w+ {I think}
      Set FOPEN_READ, FOPEN_WRITE
  Notes

  Restrictions on memory might make it advisable to map only a portion
  at a time, in which case that must be done by (I think) get_next_pstg().
  For now, if mapping fails it assumes file instead.

  Reads if necessary all postings for term given by the itr and writes
  them to the file implied by setrec = *Setrecs[set_num] in the format
  given by pstgtype. Fills in the appropriate fields in the setrec.

  Returns no of pstgs stored or - if anything went wrong.
  Dec 98: returns -3 if memory can't be mapped or allocated. This is a hint
  to the calling function to ask for file input instead. Otherwise -1
  on failure.

  This was originally  based on an inline coding of gnpai() & gnpai_ex()
  (mergesubs.c), but is now (Feb 96) much simpler.

  Jan 96: index types < 4 removed, shortens greatly
  Feb 96: completely rewritten
  Jan 98: shouldn't it check whether has been done already?
          Also there is a problem if NOPOS but index has no pos info
	  (e.g. index is type 10) -- in that case can still use direct
	  mapping of global postings file.

******************************************************************************/

/******************************************************************************

  Subfunction for case where set is atomic but limits are nonzero or
  no positional fields are wanted in the output, so set has to have its
  own raw postings file or memory array.

******************************************************************************/

#if defined(NO_MMAP) || defined(NO_ZERO)

static
setup_pstgs_atomic_own_storage(Setrec *s, int flags)
{
  /* Have to write own raw postings rejecting ones which don't
     match the limit or leaving out the positional fields. */
  int tnp = s->totalp ;
  void *source ;
  int mem_estimate ;
  u_short limits = s->limits ;
  BOOL isfile = (s->output_flags[0] & SET_FILE) ;
  int pstgtype = s->pstgtype ;
  char *result ;
  int res ;
  int l ;

  s->ttf = s->naw = 0 ;		/* Shouldn't be needed but harmless */
  s->max_tf = 0 ;		/* Don't know what it will be (unless
				   nopos = 1). Maintained by write_next_pstg()
				   and used by set_trim_memory(). */
  source = (void *)s->global_pf ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = s->global_pf_offset ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
    res = fsetpos((FILE *)source, &pos) ;
  }
#else
  res =  fseeko((FILE *)source, s->global_pf_offset, 0) ;
#endif
#else
  res = fseek((FILE *)source, s->global_pf_offset, 0) ;
#endif /* LARGEFILES */
  if ( res ) {
    sprintf(Err_buf, "seek to %x in .oi failed, set=%d",
	    s->global_pf, s->num) ;
    sys_err("") ;
    return -1 ;
  }
  
  if ( ! isfile ) {		/* Memory */
    /* Get memory for writing */
    /* (If there's a limit the memory requirement ought to be roughly in
       proportion to the ratio of the big Ns -- originally wrote this to
       use malloc() and then realloc as necessary. Now use malloc() if small
       otherwise mmap() & must estimate on high side). */
    mem_estimate = s->global_pf_length ; /* Can't come out longer than this? */
    if ( ! (pstgtype & HASPOS) && s->total_tf > 0 && s->ix->type >= 8 ) {
      /* March 96: older indexes don't have total_tf */
      mem_estimate -= s->total_tf * 4 ;
      /* Cd reduce further if ! HASTF */
    }
    if ( Dbg & DD_MEM )
      fprintf(bss_syslog, 
	      "Getting %d bytes for set %d's raw postings\n",
	      mem_estimate, s->num) ;
    if ( (result = set_alloc_mem(s, mem_estimate)) == NULL )
      return -1 ;
  }
  else { /* file, not memory */
    /* Open output file if file output */
    /* "w+" in case want to leave it open */
    if ( set_open_pf(s->num, 0, "w+b") == NULL ) 
      /* Error reported by set_open_pf() */
      return -1 ;
  }
  /* Now have input and output set up, read & write the postings */
  while ( tnp-- ) {		/* File or memory */
    /* pstgtype for read is indexes pstgtype, set's may have nopos set. */
    l = read_pstg_unpacked(source, &s->pstg, s->ix->pstgtype, TRUE) ;
    if ( l < 0 ) {
      sprintf(Err_buf,
	      "read_pstg_unpacked failed, remaining pstgs=%d", tnp) ;
      sys_err("") ;
      return -1 ;
    }
    if ( limits != 0 && ! Sats_limit(s->pstg->rec, limits) ) continue ;
    l = write_next_pstg(s) ;
    if ( l < 0 ) {
      sprintf(Err_buf, "write_next_pstg() failed after %d calls",
	      s->naw) ;
      return -1 ;
    }
  } /* (while (tnp--)) */
  return s->naw ;
} /* nonzero limit or nopos */

#else

static int
setup_pstgs_atomic_own_storage(Setrec *s, int flags)
{
  /* Have to write own raw postings rejecting ones which don't
     match the limit or leaving out the positional fields. */
  int tnp = s->totalp ;
  union {
    FILE *f ;
    u_char *m ;
    void *v ;
  } source ;
  /*void *source ;*/
  char *maptop ;		/* This and next 2 only debugging I think */
  int avail ;
  char *base ;
  int maplength ;
  int mem_estimate ;
  u_short limits = s->limits ;
  BOOL isfile = (s->output_flags[0] & SET_FILE) ;
  int pstgtype = s->pstgtype ;
  char *result ;
  int l ;

  s->ttf = s->naw = 0 ;		/* Shouldn't be needed but harmless */
  s->max_tf = 0 ;		/* Don't know what it will be (unless
				   nopos = 1). Maintained by write_next_pstg()
				   and used by set_trim_memory(). */
  /* Map global pf in for reading, assuming can always map in one go */
  /* If NO_ATOMIC_MAP is defined, reads from the file instead. */
#ifndef NO_ATOMIC_MAP
  if ( map_pf(s) < 0 ) return -1 ;
  /* Note this has set membase, pbase, memsize, which will all have be
     altered later. */
  /* source = (void *)s->pptr[0] ; */
  source.m = (u_char *)(s->pptr[0]) ;
  base = s->membase[0] ;
  maplength = s->memsize[0] ;
  maptop = base + maplength ;
  avail = (u_char *)maptop - source.m ;
#else
  /* source = (void *)s->global_pf ; */
  source.f = s->global_pf ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = s->global_pf_offset ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
    res =  fsetpos(source.f, &pos) ;
  }
#else
  res = fseeko(source.f, s->global_pf_offset, 0) ;
#endif
#else
  res = fseek(source.f, s->global_pf_offset, 0) ;
#endif /*LARGEFILES*/
  if ( res ) {
    sprintf(Err_buf, "seek to %x in .oi failed, set=%d",
	    s->global_pf, s->num) ;
    sys_err("") ;
    return -1 ;
  }
#endif /* NO_ATOMIC_MAP */
  if ( ! isfile ) {		/* Memory */
    /* Get memory for writing */
    /* (If there's a limit the memory requirement ought to be roughly in
       proportion to the ratio of the big Ns -- originally wrote this to
       use malloc() and then realloc as necessary. Now use malloc() if small
       otherwise mmap() & must estimate on high side). */
    mem_estimate = s->global_pf_length ; /* Can't come out longer than this? */
    if ( ! (pstgtype & HASPOS) && s->total_tf > 0 && s->ix->type >= 8 ) {
      /* March 96: older indexes don't have total_tf */
      mem_estimate -= s->total_tf * 4 ;
      /* Cd reduce further if ! HASTF */
    }
    if ( Dbg & DD_MEM )
      fprintf(bss_syslog, 
	      "Getting %d bytes for set %d's raw postings\n",
	      mem_estimate, s->num) ;

    if ( mem_estimate < MEM_BREAK_1 * Pagesize ) {
      if ( (result = set_alloc_mem(s, mem_estimate)) == NULL )
	return -1 ;
    }
    else {
      if ( (result =
	    set_get_zeroed_mem(s, mem_estimate, flags & FFL_HOLD_MEM)) == NULL )
	return -1 ;
    }
  }
  else {
    /* Open output file if file output */
    /* "w+" in case want to leave it open */
    if ( set_open_pf(s->num, 0, "w+b") == NULL ) 
      /* Error reported by set_open_pf() */
      return -1 ;
  }
  /* Now have input and output set up, read & write the postings */
  while ( tnp-- ) {		/* File or memory */
    /* pstgtype for read is indexes pstgtype, set's may have nopos set. */
#ifndef NO_ATOMIC_MAP
    l = read_pstg_unpacked(source.v, &s->pstg, s->ix->pstgtype, FALSE) ;
#else
    l = read_pstg_unpacked(source.v, &s->pstg, s->ix->pstgtype, TRUE) ;
#endif
    if ( l < 0 ) {
      sprintf(Err_buf,
 "read_pstg_unpacked failed, remaining pstgs=%d", tnp) ;
      sys_err("") ;
      return -1 ;
    }
#ifndef NO_ATOMIC_MAP
    /* (u_char *)source += l ; */
    source.m += l ;
#endif
    if ( limits != 0 && ! Sats_limit(s->pstg->rec, limits) ) continue ;
    l = write_next_pstg(s) ;
    if ( l < 0 ) {
      sprintf(Err_buf, "write_next_pstg() failed after %d calls",
	      s->naw) ;
      return -1 ;
    }
  } /* (while (tnp--)) */
#ifndef NO_ATOMIC_MAP
  /* Free the temporarily mapped pages */
  bss_munmap(base, maplength/Pagesize) ;
#endif
  return s->naw ;
} /* nonzero limit or nopos */

#endif /* NO_MMAP || NO_ZERO */

int
setup_pstgs(Setrec *s, int flags)
{
  int pstgtype = s->pstgtype ;
  int result ;
  static char *func = "setup_pstgs" ;

  if ( s->limits ||
       ( (s->ix->pstgtype & HASPOS) && ! (pstgtype & HASPOS) ) ) {
    /* Have to write own raw postings file rejecting pstgs which don't
       match the limit or leaving out the positional fields. */
    if ( (result = setup_pstgs_atomic_own_storage(s, flags)) < 0 )
      return result ;
  } /* nonzero limit */
  /* In other cases have global pf. If MEM, map it in. If FILE, nothing
     to do. */
  /* Following done not from lkup() but from open_pstgs() */
  else { /* limits 0 and not nopos */
    s->pstgs_size[0] = s->global_pf_length ;
    /* Raw postings to go in memory */
    if ( s->output_flags[0] & SET_MEM ) {
      /* Normally, postings file is mapped in. But if BIGREAD is set, enough
	 memory is allocated and a read() call issued for the whole
	 of the raw postings. */
      /*      if ( s->output_flags & SET_BIGREAD ) result = read_all_pf(s) ;*/
#ifdef SET_BIGREAD
      if ( (result = read_all_pf(s)) >= 0 )
	s->output_flags[0] |= SET_MEM_MALLOC ;
#else
#ifndef NO_MMAP
      if ( (result = map_pf(s)) >= 0 ) s->output_flags[0] |= SET_MEM_MAPPED ;
#else
      s->output_flags[0] &= ~SET_MEM ;
      s->output_flags[0] |= SET_FILE ;
      return -3 ;
#endif /* NO_MMAP */
#endif /* SET_BIGREAD */
      if ( result < 0 ) {	/* Mapping has failed, could try file */
	if ( Dbg & D_PSTGS )
	  fprintf(bss_syslog,
	  "%s(): map_pf failed, set %d, reentering %s to try file instead\n",
		  func, s->num, func) ;
	s->output_flags[0] &= ~(SET_MEM|SET_MEM_MALLOC|SET_MEM_MAPPED) ;
	s->output_flags[0] |= SET_FILE ;
	return -3 ;
      }
    }
  }
  return s->naw ;
}

/******************************************************************************

  open_pstgs(setrec, which, flags)    SW Jan 96

  Prepare set for output of postings, natural order (which=0) or weight order
  (which = 1). If set has a memory mapping or allocated raw postings in memory,
  this is used, otherwise file input is used.

  Don't know whether it's necessary to look for a set which it's not possible
  to output from (no own file, no file0_pf, no postings in memory), but if
  set is empty just returns 0 (OK) ;

  Returns - on error, 0 if OK.

  To replace piecemeal existing stuff.
  
  flags can contain some of the FFL values
  I think they have very little if any effect (see setup_pstgs_atomic..)

  Feb 9?: Now ensures set's posting's rec field is zero. Record numbers start
  with 1, so this flags that no posting has been read.

  Note that writing postings is not dealt with by this function (see
  prepare_set_for_output, setup_pstgs, ..)

  Redundant calls OK.

******************************************************************************/

int
open_pstgs(Setrec *s, int which, int flags)
{
  FILE *f ;
  int sflags = s->output_flags[which] ;
  int result ;
  static char *func = "open_pstgs" ;

  if ( s->naw == 0 ) return 0 ; /* Nothing to do */
  
  if ( which == 0 ) {
    if ( sflags & SET_MEM ) {
      if ( ! (sflags & (SET_MEM_EXISTS|SET_FILE_EXISTS)) ) {
	result = setup_pstgs(s, flags) ; 
	if ( Dbg & DD_PSTGS )
	  fprintf(bss_syslog,
  "%s called setup_pstgs, result %d, because ! SET_MEM_EXISTS, s=%d\n",
		  func, result, s->num) ;
	if ( result == -1 ) return -1 ;
	else if ( result == -3 ) {
	  /* Means setup_pstgs() couldn't use memory (output_flags will have
	     been changed appropriately) */
	  return open_pstgs(s, which, flags) ;
	}
      }
      s->pptr[which] = s->pbase[which] ;
    }
    else { /* which == 0 and file */
      f = set_open_pf(s->num, which, "rb") ;
      /* Doesn't matter if it's already open, open_pf() copes */
      if ( f == NULL ) return -1 ;
    }
    s->next_pstg[which] = 0 ;
  } /* (which = 0 ) */
  s->raw_plength = 0 ;
  s->pstg->rec = 0 ;
  s->atime = time(NULL) ;
  return 0 ;
}

/******************************************************************************

  set_trim_memory()

  After writing set, unmaps any unused pages from top of set's memory,
  or reallocs if memory was malloced.

  Needs to be called at end of setup_pstgs(). May invalidate pptr so
  open_pstgs() needs to be called before reading again.

  Replaces abortive set_realloc_mem()

  March 96: modified so it uses the pstgs_size[] fields; also trims the pstg
  subrecord so long as max_tf has a plausible value.

******************************************************************************/

static void
set_trim_memory(Setrec *s)
{
  int ps = Pagesize ;
  int pages_used, pages_to_unmap ;
  int f0 = s->output_flags[0] ;
  /* int f1 = s->output_flags[1] ; */
  int amount ;
  char *result ;

  /* Unpacked posting memory */
  if ( s->max_tf > 0 || ! (s->pstgtype & HASPOS) ) { 
    int oldmaxpos = s->pstg->maxpos ;
    if ( oldmaxpos > s->max_tf ) {
      if ( (result = (char *)resize_bss_Gp(s->pstg, s->max_tf)) )
	s->pstg = (bss_Gp *)result ;
      if ( Dbg & (D_PSTGS|D_MEM) ) {
	if ( result )
	  fprintf(bss_syslog, "Reduced set %d's posting from %d to %d\n",
		  s->num, oldmaxpos, s->max_tf) ;
	else fprintf(bss_syslog,
		     "Failed to reduce set %d's posting from %d to %d\n",
		     s->num, oldmaxpos, s->max_tf) ;
      }
    }
  }
  else if ( s->max_tf == 0 && (Dbg & D_PSTGS) )
    fprintf(bss_syslog, "Can't reduce set %d's posting, no maxtf info\n",
	    s->num) ;

  /* Raw posting memory */
  if ( f0 & SET_MEM_MAPPED ) {
#ifndef NO_MMAP
    pages_used = iceil((s->pbase[0] + s->pstgs_size[0] - s->membase[0]), ps) ;
    pages_to_unmap = s->memsize[0] / ps - pages_used ;
    if ( pages_to_unmap > 0 &&
       bss_munmap(s->membase[0] + pages_used * ps, pages_to_unmap) == 0) {
      s->memsize[0] -= pages_to_unmap * ps ;
      s->pptr[0] = s->pbase[0] ;
      s->next_pstg[0] = 0 ;
    }
#endif
  }
  else if ( f0 & SET_MEM_MALLOC ) {
    amount = s->memsize[0] - s->pstgs_size[0] ;
    /* Note realloc returns NULL if it fails or if size is zero */
    /* If size is zero the memory will have been freed */
    if ( amount > 0 ) {
      result = bss_realloc(s->membase[0], s->pstgs_size[0]) ;
      s->membase[0] = s->pbase[0] = s->pptr[0] = result ;
      s->memsize[0] = s->pstgs_size[0] ;
      s->next_pstg[0] = 0 ;
    }
  }
}

/******************************************************************************

  close_pstgs(s, which, flags)

  Harmless if called redundantly.
  If flags don't specify FFL_KEEP then (1) mapped (raw) memory will be
  recovered
  from atomic sets and either posting file will be closed if open.
  Flags may also specify FFL_DELETE. This clears the set and returns it to the
  pool. (Not implemented)
  RECOVER_MEM (??) 

  (Mar 01) called by clear_current_displayset(), bss_lkup() (on error),
  bss_do_combine(), bss_do_trec_stats().

******************************************************************************/

int
close_pstgs(Setrec *s, int which, int flags)
{
  int sflags = s->output_flags[which] ;
  int result = 0 ;
  static char *func = "close_pstgs" ;

  if ( flags & FFL_DELETE ) return bss_clear_setrec(s->num) ;
  if ( which == 0 ) {
    if ( ! (flags & FFL_KEEP) ) { /* (if KEEP and file open_pstgs will just
				     rewind if possible) */
      if ( sflags & SET_MEM ) {
	/* If pure atomic unmap pf if mapped or free memory if malloc'd */
	if ( which == 0 && (sflags & SET_FILE0_PF) ) {
	  /* Used to specify S_ATOMIC as well but must be redundant? */
	  if ( sflags & SET_MEM_MAPPED ) result = s_unmap(s, 0) ;
	  else if ( sflags & SET_MEM_MALLOC ) result = s_free(s, 0) ;
	}
      }
      else result = set_close_pf(s->num, 0) ;
    }
    set_trim_memory(s) ;	/* Trims posting field as well as main mem
				   if applicable */
  }
  else if ( (sflags & SET_FOPEN) && ! (flags & FFL_KEEP) ) { /* which = 1 */
    result = set_close_pf(s->num, 1) ;
  }

  s->atime = time(NULL) ;
  return result ;
}


/******************************************************************************

  map_pf(set), s_unmap(set, which)

  Maps postings for an atomic  set (rather, one which has a nonempty
  global_pf field) into pbase[0]. The base address of the mapping
  is stored in membase[0] and the actual amount of memory in memsize[0]

  Only for mapping global pf for atomic sets.

  Called by setup_pstgs()

  Returns 0 if OK and sets membase[0], pbase[0], pptr[0], memsize[0]
  and next_pstg[0], -2 if nonatomic, -1 if mapping fails for e.g. lack of
  system memory.

  Mod Feb 96 to map whole pages only

******************************************************************************/

#ifndef NO_MMAP

int
map_pf(Setrec *s)
{
  int fd ;
  int length ;
  int ps = Pagesize ;
  int pages ;
  OFFSET_TYPE base_offset ;
  if ( s->global_pf == NULL )	/* Think this is the right condition,
				   more or less equiv to nonatomic */
    return -2 ;
  fd = fileno(s->global_pf) ;
  base_offset = s->global_pf_offset / ps * ps ;
  length = s->global_pf_length + s->global_pf_offset % ps ;
  pages = iceil(length, ps) ;
  s->membase[0] =
  (char *)bss_mmap(0, pages, PROT_READ, MAP_SHARED, fd, base_offset, HOLD_ATOMIC_MAPS) ;
  if ( s->membase[0] == (char *)-1 ) return -1 ;
  s->pptr[0] =
    s->pbase[0] = s->membase[0] + s->global_pf_offset % ps ;
  s->next_pstg[0] = 0 ;
  s->memsize[0] = pages * ps ;	/* Or should we keep this in pages? */
  return 0 ;
}
#endif /* NO_MMAP */

int
read_all_pf(Setrec *s)
{
  int fd ;

  if ( s->global_pf == NULL )	/* Think this is the right condition,
				   more or less equiv to nonatomic */
    return -2 ;
  /* Taken out Dec 98, seems little point and some Unix's no /dev/zero */
  /*
  if (set_get_zeroed_mem(s, s->global_pf_length, HOLD_ATOMIC_MAPS) == NULL)
    return -1 ;
  */
  if ( set_alloc_mem(s, s->global_pf_length) == NULL ) return -1 ;
  fd = fileno(s->global_pf) ;
  if ( lseek(fd, s->global_pf_offset, SEEK_SET) != s->global_pf_offset )
    return -3 ;
  if ( read(fd, s->pbase[0], s->global_pf_length) != s->global_pf_length )
    return -3 ;
  if ( Dbg & D_PSTGS ) fprintf(bss_syslog, "Read %d bytes for set %d\n",
			       s->global_pf_length, s->num) ;
  s->next_pstg[0] = 0 ;
  return 0 ;
}

#ifndef NO_MMAP

int
s_unmap(Setrec *s, int which)
{
  int sf = s->output_flags[which] ;
  int result ;
  if ( ! ( sf & SET_MEM_MAPPED ) ) result = 1 ;
  else {
    result = bss_munmap(s->membase[which], s->memsize[which] / Pagesize) ;
    s->membase[which] = s->pbase[which] = s->pptr[which] = NULL ;
    s->memsize[which] = 0 ;
  }
  if ( result == 0 ) s->output_flags[which] &= ~SET_MEM_MAPPED ;
  return result ;
}

#else
/* Dummy */
int
s_unmap(Setrec *s, int which)
{
  return 1 ;
}

#endif
  
int
s_free(Setrec *s, int which)
{
  int sf = s->output_flags[which] ;
  int result = 0 ;
  if ( ! ( sf & SET_MEM_MALLOC ) ) result = 1 ;
  else {
    bss_free(s->membase[which]) ;
    s->membase[which] = s->pbase[which] = s->pptr[which] = NULL ;
    s->memsize[which] = 0 ;
  }
  s->output_flags[which] &= ~SET_MEM_MALLOC ;
  return result ;
}

/******************************************************************************

  get_next_pstg(srec)    SW Jan 96

  Read next natural-order posting into the set's pstg struct and return it.
  Returns NULL if anything fails else address of posting (which must be
  taken note of by the calling function as it may change from call to call).

  Feb 96: changed so that it returns a posting with infinite <rec> field
  when there aren't any more. This is to avoid merge algorithms having to
  count down np themselves, might speed and simplify things a bit.

  Might extend so can use to get next posting in weight order (for now see
  get_pstg_n() for this).

******************************************************************************/

bss_Gp *
get_next_pstg(Setrec *s)
{
  int isfile = (s->output_flags[0] & SET_FOPEN_READ) ;

  if ( ! isfile ) {
    /* In memory */
    if ( s->next_pstg[0] < s->naw ) {
      if ( (s->raw_plength =
	    read_pstg_unpacked(s->pptr[0], &s->pstg, s->pstgtype, 0)) < 0 )
	return NULL ;
      else {
	s->next_pstg[0]++ ;
	s->pptr[0] += s->raw_plength ;
      }
    }
    else s->pstg->rec = INT_MAX ;
  }
  else {
    /* In file */
    if ( s->next_pstg[0] < s->naw ) {
      s->raw_plength = read_pstg_unpacked(s->pfile[0], &s->pstg,
					  s->pstgtype, 1) ;
      if ( s->raw_plength < 0 ) return NULL ;
      else {
	s->next_pstg[0]++ ;	
      }
    }
    else s->pstg->rec = INT_MAX ;
  }
  /* Temp until all indexes have max_tf. Shouldn't now be needed unless pure
     atomic (March 96) */
  /* May 02: doubt if this wanted any more (think it should have been HASTF
     rather than HASPOS anyway) */
  /*
    if ( (s->pstgtype & HASPOS) && s->pstg->numpos > s->max_tf )
      s->max_tf = s->pstg->numpos ;
  */
  return s->pstg ;
}

/******************************************************************************

  write_next_pstg(srec)    SW Jan 96

  Write next natural-order posting from a bss_Gp struct into the set's file
  or memory. It is assumed that the posting has been put there by setup_pstgs()
  or by some bss_combine() function. 

  Returns length written, or - if out of memory or file write failed.

  NB Need to add the weight thing which is at present in combine()

  March 96: doesn't check for memory overflow; it's up to open_pstgs() or
  whatever calls it to see there's enough memory. This is maybe wrong.
  On machines with limited memory should use files only.

******************************************************************************/

int
write_next_pstg(Setrec *s)
{
  /* How do we check it's available, quickly? Check what's available? */
  int result ;
  int sflags = s->output_flags[0] ;
  int isfile = (! (sflags & SET_MEM_EXISTS) ) ;
  /* static char *func = "write_next_pstg" ; */

  /* In memory */
  if ( ! isfile ) {
    s->raw_plength =
      write_unpacked_pstg(s->pptr[0], s->pstg, s->pstgtype, 0) ;
    if ( s->raw_plength > 0 ) {
      s->next_pstg[0]++ ; 
      s->pptr[0] += s->raw_plength ;
    }
    result = s->raw_plength ;
  } /* (memory) */
  else {			/* File */
    s->raw_plength =
      write_unpacked_pstg(s->pfile[0], s->pstg, s->pstgtype, 1) ;
    if ( s->raw_plength > 0 ) s->next_pstg[0]++ ;	
    result = s->raw_plength ;
  }
  if ( result < 0 ) return result ;
  s->pstgs_size[0] += s->raw_plength ;
  if ( s->pstg->numpos > s->max_tf && (s->pstgtype & (HASPOS|HASTF)) )
    s->max_tf = s->pstg->numpos ;
  s->ttf += s->pstg->numpos ;
  s->naw++ ;
  return result ;
}

/******************************************************************************

  get_pstg_n(set)    SW Feb 96
  was part of get_rec_addr(setnum, recnum)

  prepare_set_for_output() must have been done, doesn't check

  get_pstg_offset(set, n)  SW March 96

  Returns the offset in memory or file of the nth weight-order posting,
  -2 if can't read posting index or -1 (EOF) if already had last pstg
  NO IT DOESN'T. Returns NULL or the posting [Noticed Dec 96]

  On return, the length of the raw posting is in s->raw_plength

  Dec 96: should this call prepare_set_for_output() ?

******************************************************************************/

bss_Gp *
get_pstg_n(Setrec *s, int n)
{
#ifdef LARGEFILES
  OFFSET_TYPE offset ;
#else
  long offset ;
#endif
  int plength ;

  offset = get_pstg_offset(s, n) ;
  if ( offset < 0 ) return NULL ;
  /* Now have offset in raw postings, and pointers in the right place */

  /* It's not really worth checking whether the requested posting is a repeat
     of the last one read. This would be a rare occurrence. */
  if ( s->output_flags[0] & SET_MEM_EXISTS ) /* then postings in memory */
    plength =
      read_pstg_unpacked(s->pbase[0] + offset, &s->pstg, s->pstgtype, 0) ;
  else if ( s->output_flags[0] & SET_FILE_EXISTS ) {
    if ( !(s->output_flags[0] & SET_FOPEN_READ) ) {
      if ( open_pstgs(s, 0, 0) < 0 ) {
	fprintf(bss_syslog, "Can't open set %d's raw postings file\n",
		s->num) ;
	return NULL ;
      }
    }
#ifdef LARGEFILES
#ifdef NO_FSEEKO
    {
      fpos_t pos = offset + s->pf_offset ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
      fsetpos(s->pfile[0], &pos) ;
    }
#else
    fseeko(s->pfile[0], offset + s->pf_offset, 0) ;
#endif
#else
    fseek(s->pfile[0], offset + s->pf_offset, 0) ;
#endif
    plength =
      read_pstg_unpacked(s->pfile[0], &s->pstg, s->pstgtype, 1) ;
  }
  else {
    /* Then set isn't set up properly */
    fprintf(bss_syslog,
    "get_pstg_n(): set %d has neither memory nor a file for raw postings\n",
	    s->num) ;
    return NULL ;
  }
  if ( plength < 0 ) return NULL ;
  s->raw_plength = plength ;
  return s->pstg ;
}

int
get_pstg_offset(Setrec *s, int n)
{
  int offset ;
  if ( n >= s->np[1] ) {
    fprintf(bss_syslog,
 "Set %d, trying to read pstg %d's offset, %d postings stored\n",
	    s->num, n, s->np[1]) ;
    return -1 ;
  }
  if ( s->output_flags[1] & SET_MEM_EXISTS ) { /* then index in memory */
    s->pptr[1] = s->pbase[1] + n * SET_PF1_RECLEN ;
    offset = *(int *)s->pptr[1] ;
  }
  else if ( s->output_flags[1] & SET_FOPEN_READ ) { /* Index in file */
    if ( s->next_pstg[1] != n ) 	/* If it is, don't need to seek */
#ifdef LARGEFILES
#ifdef NO_FSEEKO
      {
	fpos_t pos = (fpos_t)(n * SET_PF1_RECLEN) ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
	fsetpos(s->pfile[1], &pos) ;
      }
#else
    fseeko(s->pfile[1], (OFFSET_TYPE)(n * SET_PF1_RECLEN), 0) ;
#endif
#else
    fseek(s->pfile[1], (long)(n * SET_PF1_RECLEN), 0) ;
#endif
    offset = getw(s->pfile[1]) ;
  }
  else {
    fprintf(bss_syslog,
	    "Can't read set %d's posting index\n", s->num) ;
    return -2 ;
  }
  s->next_pstg[1] = n + 1 ;
  return offset ;
}
  

/******************************************************************************

  new_set_num = bss_limit(old_set_num, limit_mask)  SW June 1992

  Produces a new set with any postings not matching the limit mask deleted.
  If the input set postings have weights (pstgtype=3 or 4) these are
  retained but must be ignored. The new set's set_type is ored with S_LIMIT.

  See also rf_remove_recs() - much of the code is more or less the same

  Error handling (existence of limits, limit <= 65535, set exists) is
  supposed to be done at higher level.

  June 95: now fills in ttf and log_tf fields in output setrec (previous
  omission).

  July 98: limit stuff not used for some time, probably doesn't work properly.

******************************************************************************/

int
bss_limit(Setrec *old_srec, u_int mask, Setrec *srec, FILE *f_old, FILE *f_new)
{
  u_int lim ;
  int optype = old_srec->pstgtype ;
  int outnum ;
  int ttf = 0 ;
  int i, j ;
  bss_Gp *pstg ;

  int weight_dist_vals[20] ;	/* Weight thresholds for recording weight
				   distn. of output postings */
  static char *func = "bss_limit" ;

  pstg = make_bss_Gp(MAXLONGPOS) ;

  if ( optype & HASWT ) {
    srec->mpw = old_srec->maxweight ;
    srec->gw = old_srec->gw ;	/* ? */
    srec->maxweight = MINWEIGHT ;
    srec->weight_dist_grad = WEIGHT_DIST_GRAD ;
    for ( j = 0 ; j < srec->weight_dist_grad ; j++ ) 
      weight_dist_vals[j] = srec->mpw * (j + 1) / srec->weight_dist_grad ;
  }
  else srec->weight_dist_grad = 0 ;

  for ( i = 0, outnum = 0 ; i < old_srec->naw ; i++ ) {
    if ( read_pstg_unpacked(f_old, &pstg, optype, TRUE) < 0 ) {
      sprintf(Err_buf, "read failed on posting input at rec %d", i) ;
      sys_err(func) ;
      bss_fclose(f_new) ;
      bss_fclose(f_old) ;
      bss_clear_setrec(srec->num) ;
      bss_free(pstg) ;
      return -1 ;
    }
    if ( Cdb.lims != NULL ) lim = Cdb.lims[pstg->rec - 1] ;
    if ( (lim & mask) == mask ) {
      /* TEMP Jan 96 while changing */
      write_unpacked_pstg(f_new, pstg, optype, FFL_FILE) ;

      outnum++ ;
      ttf += pstg->numpos ;
      if ( optype & HASWT ) {
	if ( pstg->score >= srec->gw ) { 
	  srec->ngw++ ;
	  if ( pstg->score >= srec->mpw ) srec->nmpw++ ;
	}
	if ( pstg->score > srec->maxweight ) {
	  srec->maxweight = pstg->score ;
	  srec->nmaxweight = 1 ;
	}
	else if ( pstg->score == srec->maxweight ) srec->nmaxweight++ ;
	for ( j = 0 ; j < srec->weight_dist_grad ; j++ ) {
	  if ( pstg->score <= weight_dist_vals[j] ) {
	    srec->weight_dist[j]++ ;
	    break ;
	  }
	}    
      }
    }
  }
  bss_fclose(f_new) ; bss_fclose(f_old) ;
  srec->output_flags[0] |= SET_FILE_EXISTS ;
  srec->pfile[0] = NULL ;
  srec->naw = outnum ;
  srec->ttf = ttf ;
  bss_free(pstg) ;
  return srec->num ;
}

/******************************************************************************

  length = read_pstg_unpacked(file, &buf, ptype, isfile)

  Reads and unpacks posting of type ptype (excluding length byte if any)
  into bss_Gp struct from file or memory returning length of packed posting
  or - on error.

  June 94: modified to return length instead of numpos

  The buffer must be a bss_Gp struct

  See also write_unpacked_pstg

  Used by various functions in bss_combine and by bss_limit(),
  by get_rec_addr() (bss_recs), get_pstg_matchterm() (bss_funcs),
  and rf_remove_recs() (bss_rf).

  May 94: Added type 4, and arranged that address of buffer pointer is passed
  in case it is necessary to resize the buffer (can't afford the memory
  for e.g. combine() to allocate 256 64K buffers).

  March 96: altered sequence in line with write_unpacked_pstg()

  Note that if posting is in a file the file pointer is advanced, and a
  backward seek must be done to reread it, but if in memory the pointer
  will not have been advanced.

  Note that, for speed,  no field is zeroed if not present, so before
  starting to (re-)use the posting it should be zero-filled.

******************************************************************************/

int
read_pstg_unpacked(void *source, bss_Gp **buf, int pstgtype, int isfile)
{
  register bss_Gp *pstg = *buf ;
  register char *m ;		/* Set to f if postings are in memory */
  register FILE *f ;
  register int length = 0 ;
  int amount ;
  /* int tempr = 0 ; unused */

  if ( isfile ) {
    f = (FILE *)source ;
    if ( pstgtype & HASPASSAGES ) {
      pstg->numpars = (unsigned)getc(f) ; length++ ;
    }
    if ( pstgtype & (HASCONTRIBS|HASCOORD) ) {
      pstg->coord = (unsigned)getc(f) ;
      length++ ;
    }
    if ( pstgtype & HASTF ) {
      pstg->numpos = fget_vshort(f) ;
      if ( pstg->numpos == 0xffff ) {
	fprintf(bss_syslog, "fget_vshort returned %d \n", (int)pstg->numpos) ;
	return -1 ;
      }
      /* NB this is tf only unless HASPOS (Jan 96) */
      if ( pstg->numpos < 128 ) length++ ;
      else length += 2 ;
    }
    else if ( pstgtype & HAS_QTF ) {
      pstg->numpos = (u_short)getc(f) ;
      length++ ;
    }
    if ( pstgtype & LARGE_RN ) {
      fread((char *)&pstg->rec, (size_t)4, (size_t)1, f) ;
      length += 4 ;
    }
    else if ( pstgtype & VARI_RN ) {
      int l ;
      pstg->rec = r1to4(&l, f) ;
      length += l ;
    }
    else {
#if ! HILO
      fread((char *)&pstg->rec, (size_t)3, (size_t)1, f) ;
#else
      fread((char *)&pstg->rec + 1, (size_t)3, (size_t)1, f) ;
#endif
      length += 3 ;
    }
    if ( pstgtype & (HASWT|HAS_INDEX_WT) ) {
      fread((char *)&pstg->score, (size_t)sizeof(float), (size_t)1, f) ;
      length += sizeof(float) ;
      if ( pstgtype & HAS_AUXWT ) {
	fread((char *)&pstg->wt, (size_t)sizeof(float), (size_t)1, f) ;
	fread((char *)&pstg->auxwt, (size_t)sizeof(float), (size_t)1, f) ;
	length += 2 * sizeof(float) ;
      }
    }
    if ( pstgtype & HASMARKS ) {
      pstg->flags = (unsigned)getc(f) ;
      length++ ;
    }
    if ( pstgtype & HASPASSAGES ) {
      int j ;
      Para_rec *p = pstg->pars ;
      for ( j = 0 ; j < pstg->numpars ; j++, p++ ) {
	fread((char *)&p->fd, (size_t)sizeof(short), (size_t)1, f) ;
	fread((char *)&p->sp, (size_t)sizeof(u_short), (size_t)1, f) ;
	fread((char *)&p->fp, (size_t)sizeof(u_short), (size_t)1, f) ;
	fread((char *)&p->wt, (size_t)sizeof(float), (size_t)1, f) ;
      }
      length +=
	(sizeof(short) + 2 * sizeof(u_short) + sizeof(float)) * pstg->numpars ;
    }
    if ( pstgtype & HASCONTRIBS ) {
      fread((char *)&pstg->t, (size_t)sizeof(struct str_tf),
	    (size_t)pstg->coord, f) ;
      length += pstg->coord * sizeof(pstg->coord) ;
    }
    if ( pstgtype & HASPOS ) {
      if ( pstg->numpos > pstg->maxpos ) 
	*buf = pstg = resize_bss_Gp(*buf, pstg->numpos) ;
      amount = pstg->numpos * sizeof(U_pos) ;
      fread((char *)pstg->p, (size_t)amount, (size_t)1, f) ;
      length += amount ;
      /* Added June 94, makes highlighting simpler,
	 is this still needed? (Jan 96) */
      ((u_int *)(pstg->p))[pstg->numpos] = 0xffffffff ; 
    }
  }
  else { /* ( not isfile -- in memory) */
    m = (char *)source ;
    if ( pstgtype & HASPASSAGES ) pstg->numpars = (unsigned)*m++ ;
    if ( pstgtype & (HASCONTRIBS|HASCOORD) ) pstg->coord = (unsigned)*m++ ;
    if ( pstgtype & HASTF ) {
      pstg->numpos = mget_vshort(m) ; /* NB this is tf only unless HASPOS */
      if ( pstg->numpos < 128 ) m++ ;
      else m += 2 ;
    }
    else if ( pstgtype & HAS_QTF ) 
      pstg->numpos = (u_short)*m++ ;
    if ( pstgtype & LARGE_RN ) {
      memcpy((void *)&pstg->rec, (void *)m, (size_t)4) ;
      m += 4 ;
    }
    else {
      pstg->rec = cv3(m) ;
      m += 3 ;
    }
    if ( pstgtype & (HASWT|HAS_INDEX_WT) ) {
      memcpy((char *)&pstg->score, m, sizeof(float)) ;
      m += sizeof(float) ;
      if ( pstgtype & (HAS_AUXWT) ) {
	memcpy((char *)&pstg->wt, m, sizeof(float)) ; 
	m += sizeof(float) ;
	memcpy((char *)&pstg->auxwt, m, sizeof(float)) ;
	m += sizeof(float) ;
      }
    }
    if ( pstgtype & HASMARKS ) {
      pstg->flags = *m++ ;
    }
    if ( pstgtype & HASPASSAGES ) {
      int j ;
      Para_rec *p = pstg->pars ;
      for ( j = 0 ; j < pstg->numpars ; j++, p++ ) {
	memcpy((char *)&p->fd, m, sizeof(short)) ; m += sizeof(short) ;
	memcpy((char *)&p->sp, m, sizeof(u_short)) ; m += sizeof(u_short) ;
	memcpy((char *)&p->fp, m, sizeof(u_short)) ; m += sizeof(u_short) ;
	memcpy((char *)&p->wt, m, sizeof(float)) ; m += sizeof(float) ;
      }
    }
    if ( pstgtype & HASCONTRIBS ) {
      amount = sizeof(struct str_tf) * pstg->coord ;
      memcpy((char *)&pstg->t, m, amount) ;
      m += amount ;
    }
    if ( pstgtype & HASPOS ) {
      if ( pstg->numpos > pstg->maxpos ) 
	*buf = pstg = resize_bss_Gp(*buf, pstg->numpos) ;
      amount = pstg->numpos * sizeof(U_pos) ;
      memcpy((char *)pstg->p, m, amount) ;
      m += amount ;
      /* Added June 94, makes highlighting simpler,
	 is this still needed? (Jan 96) */
      ((u_int *)(pstg->p))[pstg->numpos] = 0xffffffff ; 
    }
    length = m - (char *)source ;
  }
  return length ;
}

/******************************************************************************

  dump_unpacked_pstg(pstg, type, stream) 

  Output bss set posting to e.g. stderr

  JUne 95: doubt if this works for all types, unused I think
  March 96: brought up to date, more or less, kind of works ..
  Revised Feb 99, added "type" line, but new code arg not used yet.

******************************************************************************/

int
dump_unpacked_pstg(bss_Gp *p, int pstgtype, char *buf, int code)
{
  int j ;
  char *ip = buf ;

  sprintf(ip, "pstgtype %d ", pstgtype) ; ip += strlen(ip) ;
  (void)translate_pstgtype(pstgtype, ip) ; ip += strlen(ip) ;
  if ( pstgtype & HASPOS ) {
    if ( pstgtype & HAS_AUXWT )
      sprintf(ip,
	      "maxpos=%d numpos=%d numpars=%d irn=%u score=%.3f wt=%.3f auxwt=%.3f r=%u fl=%d dummy=%d", 
	      (unsigned)p->maxpos, (unsigned)p->numpos, (unsigned)p->numpars,
	      p->rec, (double)p->score, (double)p->wt, (double)p->auxwt,
	      (u_int)p->r, (int)p->flags, (unsigned)p->dummyflags) ;
    else sprintf(ip,
	   "maxpos=%d numpos=%d numpars=%d irn=%u score=%.3f fl=%d dummy=%d", 
	   (unsigned)p->maxpos, (unsigned)p->numpos, (unsigned)p->numpars,
	   p->rec, (double)p->score, 
	   (int)p->flags, (unsigned)p->dummyflags) ;
  }
  else {
    if ( pstgtype & HAS_AUXWT )
      sprintf(ip,
	      "maxpos=%d numtf=%d numpars=%d irn=%u score=%.3f wt=%.3f auxwt=%.3f r=%u fl=%d dummy=%d", 
	      (unsigned)p->maxpos, (unsigned)p->numpos, (unsigned)p->numpars,
	      p->rec, (double)p->score, (double)p->wt, (double)p->auxwt,
	      (u_int)p->r, (int)p->flags, (unsigned)p->dummyflags) ;
    else sprintf(ip,
       "maxpos=%d numtf=%d numpars=%d irn=%u score=%.3f r=%u fl=%d dummy=%d", 
	(unsigned)p->maxpos, (unsigned)p->numpos, (unsigned)p->numpars,
	p->rec, (double)p->score, 
	(u_int)p->r, (int)p->flags, (unsigned)p->dummyflags) ;
  }
  if ( pstgtype & (HASCOORD|HASCONTRIBS) ) {
    ip += strlen(ip) ;
    sprintf(ip, " coord=%d", (int)p->coord) ;
  }
  ip += strlen(ip) ;
  sprintf(ip, "\n") ;
  /* contribs wouldn't be right, but we've not used it yet (Jan 01) */
  if ( pstgtype & HASCONTRIBS ) {
    for ( j = 0 ; j < p->coord ; j++ ) {
      ip += strlen(ip) ;
      sprintf(ip, ":%d:%d", (unsigned)p->t[j].str, (unsigned)p->t[j].tf) ;
    }
  }
  /* Skip para_recs for now, can't be bothered */
  if ( pstgtype & HASPOS ) {
    ip += strlen(ip) ;
    for ( j = 0 ; j < p->numpos ; j++ ) {
      if ( pstgtype & TEXTPOS )
	sprintf(ip, "%s ", expand_pos_text(&p->p[j])) ;
      else if ( pstgtype & LONGPOS )
	sprintf(ip, "%s ", expand_pos6(&p->p[j])) ;
      ip += strlen(ip) ;
    }
  }
  return ip - buf ;
}


/******************************************************************************

  write_unpacked_pstg

  Writes out a posting from a bss_Gp struct into a file, or memory.
  Used to return zero, but now returns number of bytes written or
  copied.

  March 96: modified the sequence in which a posting is written out to file
  or memory.

******************************************************************************/

int
write_unpacked_pstg(void *target, bss_Gp *buf, int pstgtype, int isfile)
{
  FILE *f ;			/* If to file */
  register char *m ;		/* If to memory */
  Para_rec *p ;
  register int l ;
  /* unsigned numpos ; unused */
  int amount ;
  register int j ;

  if ( isfile ) {
    f = (FILE *)target ;
    l = 0 ;
    if ( pstgtype & HASPASSAGES ) {
      putc((u_char)buf->numpars, f) ; l++ ;
    }
    if ( pstgtype & (HASCONTRIBS|HASCOORD) ) {
      putc((u_char)buf->coord, f) ; l++ ;
    }
    if ( pstgtype & (HASPOS|HASTF) ) l += fput_vshort(buf->numpos, f) ;
    else if ( pstgtype & HAS_QTF ) {
      putc((int)buf->numpos, f) ;
      l++ ;
    }
    if ( pstgtype & LARGE_RN ) {
      fwrite((char *)&buf->rec, (size_t)4, (size_t)1, f) ;
      l += 4 ;
    }
    else {
#if HILO
      fwrite((char *)&buf->rec + 1, (size_t)3, (size_t)1, f) ; l += 3 ;
#else
      fwrite((char *)&buf->rec, (size_t)3, (size_t)1, f) ; l += 3 ;
#endif
    }
    if ( pstgtype & (HASWT|HAS_INDEX_WT) ) {
      fwrite((char *)&buf->score, (size_t)sizeof(float), (size_t)1, f) ;
      l += sizeof(float) ;
      if ( pstgtype & HAS_AUXWT ) {
	fwrite((char *)&buf->wt, (size_t)sizeof(float), (size_t)1, f) ;
	l += sizeof(float) ;
	fwrite((char *)&buf->auxwt, (size_t)sizeof(float), (size_t)1, f) ;
	l += sizeof(float) ;
      }
    }
    if ( pstgtype & HASMARKS ) {
      putc(buf->flags, f) ; l++ ;
    }
    if ( pstgtype & HASPASSAGES ) {
      for ( j = 0, p = buf->pars ; j < buf->numpars ; j++, p++ ) {
	fwrite((char *)&p->fd, (size_t)sizeof(short), (size_t)1, f) ;
	fwrite((char *)&p->sp, (size_t)sizeof(u_short), (size_t)1, f) ;
	fwrite((char *)&p->fp, (size_t)sizeof(u_short), (size_t)1, f) ;
	fwrite((char *)&p->wt, (size_t)sizeof(float), (size_t)1, f) ;
      }
      l += (sizeof(short) + 2 * sizeof(u_short) + sizeof(float)) * buf->numpars ;
    }
    if ( pstgtype & HASCONTRIBS ) {
      fwrite((char *)buf->t, (size_t)sizeof(struct str_tf), (size_t)buf->coord, f) ;
      l += sizeof(struct str_tf) * buf->coord ;
    }
    if ( pstgtype & HASPOS ) {
      fwrite((char *)buf->p, (size_t)(buf->numpos * sizeof(U_pos)), (size_t)1, f) ;
      l += buf->numpos * 4 ;
    }
  }
  else { /* MEMTYPE */
    char *start = target ;
    register int M ;
    m = (char *)target ;
    /* May be worth inlining */
    if ( pstgtype & HASPASSAGES ) *m++ = (u_char)buf->numpars ;
    if ( pstgtype & (HASCONTRIBS|HASCOORD) ) *m++ = (u_char)buf->coord ;
    if ( pstgtype & (HASPOS|HASTF) ) {
      m += (M = mput_vshort((unsigned)buf->numpos, m)) ;
      if ( M <= 0 ) fprintf(bss_syslog, "mput_vshort() returned 0\n") ;
    }
    else if ( pstgtype & HAS_QTF ) *m++ = (u_char)buf->numpos ;
    if ( pstgtype & LARGE_RN ) {
      memcpy(m, (void *)&buf->rec, 4) ;
      m += 4 ;
    }
    else {
#if HILO
    memcpy(m, (u_char *)&buf->rec + 1, 3) ; m += 3 ;
#else
    memcpy(m, (u_char *)&buf->rec, 3) ; m += 3 ;
#endif
    }
    if ( pstgtype & (HASWT|HAS_INDEX_WT) ) {
      memcpy(m, (void *)&buf->score, sizeof(float)) ;
      m += sizeof(float) ;
      if ( pstgtype & HAS_AUXWT ) {
	memcpy(m, (void *)&buf->wt, sizeof(float)) ;
	m += sizeof(float) ;
	memcpy(m, (void *)&buf->auxwt, sizeof(float)) ;
	m += sizeof(float) ;
      }
    }
    if ( pstgtype & HASMARKS ) {
      *m++ = buf->flags ;
    }
    if ( pstgtype & HASPASSAGES ) 
      for ( j = 0, p = buf->pars ; j < buf->numpars ; j++, p++ ) {
	memcpy(m, (void *)&p->fd, sizeof(short)) ; m += sizeof(short) ;
	memcpy(m, (void *)&p->sp, sizeof(u_short)) ; m += sizeof(u_short) ;
	memcpy(m, (void *)&p->fp, sizeof(u_short)) ; m += sizeof(u_short) ;
	memcpy(m, (void *)&p->wt, sizeof(float)) ; m += sizeof(float) ;
      }
    if ( pstgtype & HASCONTRIBS ) {
      amount = sizeof(struct str_tf) * buf->coord ;
      memcpy(m, (void *)buf->t, amount) ;
      m += amount ;
    }
    if ( pstgtype & HASPOS ) {
      amount = buf->numpos * sizeof(U_pos) ;
      memcpy(m, (void *)buf->p, amount) ;
      m += amount ;
    }
    l = m - start ;
  } /* (written to memory) */
  return l ;
}

#ifdef GET_PACKED_PLENGTH

/******************************************************************************

  get_packed_plength(p, type, isfile)

  Returns length of a packed posting in a file or in memory

  Incomplete and incorrect, check against write_unpacked_pstg()

  July 98: appears unused

******************************************************************************/

int
get_packed_plength(void *p, int pstgtype, BOOL isfile)
{
  register FILE *f ;
  register char *mem ;
  register int l = 0 ;
  int numpos, numpassages = 0  ;
  
  if ( isfile ) {
    f = (FILE *)p ;
    numpos = fget_vshort(f) ;
    if ( numpos < 128 ) {
      if ( pstgtype & HASPASSAGES ) {
	numpassages = (u_char)getc(f) ;
	l = 1 + 1 + 3 + numpassages * sizeof(Para_rec) ;
#ifdef LARGEFILES
	fseeko(f, (OFFSET_TYPE)-2, 1) ;
#else
	fseek(f, (long)-2, 1) ;
#endif
      }
      else {
	l = 1 + 3 ;
	ungetc((u_char)numpos, f) ;
      }
    }
    else {
      if ( pstgtype & HASPASSAGES ) {
	numpassages = (u_char)getc(f) ;
	l = 2 + 1 + 3 + numpassages * sizeof(Para_rec) ;
#ifdef LARGEFILES
	fseeko(f, (OFFSET_TYPE)-3, 1) ;
#else
	fseek(f, (long)-3, 1) ;
#endif
      }
      else {
	l = 2 + 3 ;
#ifdef LARGEFILES
	fseeko(f, (OFFSET_TYPE)-2, 1) ;
#else
	fseek(f, (long)-2, 1) ;
#endif
      }
    }
  }
  else {
    mem = (char *)p ;
    numpos = mget_vshort(mem) ;
    if ( pstgtype & HASPASSAGES ) {
      if ( numpos < 128 ) {
	numpassages = *(u_char *)(++mem) ;
	l = 1 + 1 + 3 + numpassages * sizeof(Para_rec) ;
      }
      else {
	mem += 2 ;
	numpassages = *(u_char *)(mem) ;
	l = 2 + 1 + 3 + numpassages * sizeof(Para_rec) ;
      }
    }
    else l = (numpos < 128) ? 1 + 3 : 2 + 3 ;
  }
  if ( pstgtype & HASPOS ) l += numpos * sizeof(u_int) ;
  if ( pstgtype & (HASWT|HAS_INDEX_WT) ) l += sizeof(float) ;
  /*if ( pstgtype & HASCONTRIBS ) l +=*/
  return l ;
}
#endif /* GET_PACKED_PLENGTH */

#ifdef NOT_DONE_YET

/******************************************************************************

  copy_packed_pstg(t, s, isfile)

  Utility function to speed e.g. select_topn()

  Copies posting of 

  SW March 96

  Incomplete

******************************************************************************/

void
copy_packed_pstg(void *t, void *s, BOOL isfile_t, BOOL isfile_s)
{
  FILE *f ;
  char *mem ;
  int isfile ;
  if ( isfile ) ;
}

#endif

/******************************************************************************

  Various properties of an unpacked posting

  Normally preceded by read_pstg_unpacked

******************************************************************************/

int
get_numpos_pstg(bss_Gp *pstg)
{
  return pstg->numpos ;
}

int
get_tf_pstg(bss_Gp *pstg)	/* Same as numpos if pstg from term lkup */
{
  return pstg->numpos ;
}

int
get_rec_pstg(bss_Gp *pstg)
{
  return pstg->rec ;
}

int
get_doclen_pstg(bss_Gp *pstg, Ix *index)
{
  if ( index->doclens == NULL ) return -1 ;
  else return index->doclens[pstg->rec - 1] ;
}

double
get_wt_pstg(bss_Gp *pstg)	
{
  return (double)pstg->score ;
}

double
get_auxwt_pstg(bss_Gp *pstg)	
{
  return (double)pstg->auxwt ;
}

/******************************************************************************
 
  Translating various flags, mainly for debugging

  translate_pstgtype(pstgtype, p)  SW Dec 94
 
  For the #defines see bss.h
 
******************************************************************************/

int
translate_pstgtype(int t, char *p)
{
  char *sp = p ;
  if ( t & HASWT ) sprintf(p, "HASWT") ; p += strlen(p) ;
  if ( t & HAS_INDEX_WT ) sprintf(p, "HAS_INDEX_WT") ; p += strlen(p) ;
  if ( t & HAS_AUXWT ) sprintf(p, " HAS_AUXWT") ; p += strlen(p) ;
  if ( t & HASTF ) sprintf(p, " HASTF") ; p += strlen(p) ;
  if ( t & HASPOS ) sprintf(p, " HASPOS") ; p += strlen(p) ;
  if ( t & HAS_QTF ) sprintf(p, " HAS_QTF") ; p += strlen(p) ;
  if ( t & LONGPOS ) sprintf(p, " LONGPOS") ; p += strlen(p) ;
  if ( t & TEXTPOS ) sprintf(p, " TEXTPOS") ; p += strlen(p) ;
  if ( t & HASCOORD ) sprintf(p, " HASCOORD") ; p += strlen(p) ;
  if ( t & HASCONTRIBS ) sprintf(p, " HASCONTRIBS") ; p += strlen(p) ;
  if ( t & HASPASSAGES ) sprintf(p, " HASPASSAGES") ; p += strlen(p) ;
  if ( t & HASMARKS ) sprintf(p, " HASMARKS") ; p += strlen(p) ;
  if ( t & LARGE_RN ) sprintf(p, " LARGE_RN") ; p += strlen(p) ;
  sprintf(p, "\n") ;
  return p - sp ;
}

void
translate_output_flags(int t, char *p)
{
  if ( t & SET_FILE0_PF ) sprintf(p, "PF ") ; p += strlen(p) ;
  if ( t & SET_FILE ) sprintf(p, "FILE ") ; p += strlen(p) ;
  if ( t & SET_FILE_EXISTS ) sprintf(p, "EXISTS ") ; p += strlen(p) ;
  if ( t & SET_FOPEN_READ ) sprintf(p, "READ ") ; p += strlen(p) ;
  if ( t & SET_FOPEN_WRITE ) sprintf(p, "WRITE ") ; p += strlen(p) ;
  if ( t & SET_MEM ) sprintf(p, "MEM ") ; p += strlen(p) ;
  if ( t & SET_MEM_MALLOC ) sprintf(p, "MALLOC ") ; p += strlen(p) ;
  if ( t & SET_MEM_MAPPED ) sprintf(p, "MAPPED ") ;
  *(p + strlen(p) - 1) = '\n' ;
}

void
translate_set_type(int t, char *p)
{
  if ( t == S_NO_TYPE ) sprintf(p, "NO-TYPE") ; p += strlen(p) ;
  if ( t & S_Q_ATOMIC ) sprintf(p, "QUASI-ATOMIC ") ; p += strlen(p) ;
  if ( t & S_P_ATOMIC ) sprintf(p, "PURE-ATOMIC ") ; p += strlen(p) ;
  if ( t & S_A_ATOMIC ) sprintf(p, "ADJ-ATOMIC ") ; p += strlen(p) ;
  if ( t & S_O_ATOMIC ) sprintf(p, "OR-ATOMIC ") ; p += strlen(p) ;
  if ( t & S_WEIGHTED ) sprintf(p, "WEIGHTED ") ; p += strlen(p) ;
  if ( t & S_MERGE ) sprintf(p, "MERGE ") ; p += strlen(p) ;
  if ( t & S_LIMIT ) sprintf(p, "LIMIT ") ; p += strlen(p) ;
  if ( t & S_REMOVE ) sprintf(p, "REMOVE ") ; p += strlen(p) ;
  if ( t & S_RLIMIT ) sprintf(p, "ROBERTSON_LIMIT ") ; p += strlen(p) ;
  if ( t & S_PARTIAL ) sprintf(p, "PARTIAL ") ; p += strlen(p) ;
  if ( t & S_IRN ) sprintf(p, "IRN ") ; p += strlen(p) ;
  if ( t & S_NOSORT ) sprintf(p, "NOSORT ") ; p += strlen(p) ;
  /* Ephemeral sets hold simple rn/score pstgs in approx weight order
     without sorting. Can't be recombined except with bss-quick_combine() */
  if ( t & S_EPHEMERAL ) sprintf(p, "EPHEMERAL ") ; p += strlen(p) ;
  *(p - 1) = '\n' ;
}  

/******************************************************************************

  bss set posting file functions

******************************************************************************/

/******************************************************************************

  set_open_pf(setnum, which, mode)   SW

  Opens a set's file 0 or 1 in the given mode (r, r+, w, w+).
  Returns NULL if can't open it otherwise returns the file pointer.

  CHecks whether exists and is open already in an appropriate mode.
  If it does, it seeks or truncates in accordance with the mode 

  Doesn't check whether it exists already, so would truncate if w or w+
  & fail if it doesn't exist if r.

  On success sets the set's output_flags fields to "exists", pfile[] to the file
  pointer and nextrec to zero (NB if mode was a or a+ this would be wrong,
  but it never is, at least at present -- Oct 95).

******************************************************************************/

FILE *
set_open_pf(int setnum, int which, char *rw)
     /* which ;	0, 1 or 2 */
     /* *rw ;	"rb" or "wb" or "w+b" */
{
  FILE *f ;
  BOOL pf ;			/* TRUE if file is Db's pstg file */
  char fname[PATHNAME_MAX] ;
  Setrec *s = Setrecs[setnum] ;
  int flags = s->output_flags[which] ;
  int result ;
  static char *func = "set_open_pf" ;
  bss_Result = 0 ;

  if ( Dbg & D_FILES )
    fprintf(bss_syslog, "%s(s=%d, wh=%d, mode=%s)\n",
	    func, setnum, which, rw) ;
  /* Feb 96: check if it's already open */
  if ( flags & SET_FOPEN ) {
    if ( strcmp(rw, "rb") == 0 && ( flags & SET_FOPEN_READ ) ) {
      f = s->pfile[which] ;
      if ( which == 0 ) {
#ifdef LARGEFILES
#ifdef NO_FSEEKO
	{
	  fpos_t pos = s->pf_offset ;
	  if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				       "fsetpos(%I64d)\n",
#else
				       "fsetpos(%lld)\n",
#endif
				       pos) ;
	  fsetpos(f, &pos) ;
	}
#else
	fseeko(f, s->pf_offset, 0) ;
#endif
#else
	fseek(f, s->pf_offset, 0) ;
#endif
      }
      else rewind(f) ;
      return f ;
    }
    else if ( strcmp(rw, "wb") == 0 && ( flags & SET_FOPEN_WRITE ) ) {
      f = s->pfile[which] ;
      rewind(f) ;
#ifndef NO_TRUNCATE
      result = ftruncate(fileno(f), 0) ;
      if ( Dbg & DD_PSTGS )
	fprintf(bss_syslog,
		"Truncated file %d set %d open for write, result %d\n",
		which, s->num, result) ;
#endif /* NO_TRUNCATE */
      return f ;
    }
    else if ( strcmp(rw, "r+b") == 0 && (flags & SET_FOPEN) == SET_FOPEN ) {
      /* FOPEN is READ|WRITE */
      f = s->pfile[which] ;
      rewind(f) ;
      return f ;
    }
    else if ( strcmp(rw, "w+b") == 0 && (flags & SET_FOPEN) == SET_FOPEN ) {
      /* FOPEN is READ|WRITE */
      f = s->pfile[which] ;
      rewind(f) ;
#ifndef NO_TRUNCATE
      result = ftruncate(fileno(f), 0) ;
      if ( Dbg & DD_PSTGS )
	fprintf(bss_syslog,
		"Truncated file %d set %d open w+, result %d\n",
		which, s->num, result) ;
#endif /* NO_TRUNCATE */
      return f ;
    }
    else {
      set_close_pf(setnum, which) ;
    }
  } /* (already open) */

  if ( which == 0 && strcmp(rw, "rb") == 0 &&
      (s->output_flags[0] & SET_FILE0_PF) ) {
    f = bss_fopen(s->ix->pfname[s->pf_vol], rw) ;
    pf = TRUE ;
  }
  else {
    bss_pfname(fname, setnum, which) ;
    pf = FALSE ;
    f = bss_fopen(fname, rw) ;
  }
  if ( f == NULL ) {
    if ( Dbg & D_PSTGS ) {
      perror(NULL) ;
      sprintf(Err_buf, "set %d: can't open pstgs file for %s", setnum, rw) ;
      sys_err(func) ;
    }
    /* Sep 00: deleted prepare_error() here: should be handled by calling
       function (2.31) */
    return f ;
  }
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  if ( pf ) {
    fpos_t pos = s->pf_offset ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
    fsetpos(f, &pos) ;
  }
#else
  if ( pf ) fseeko(f, s->pf_offset, 0) ;
#endif
#else
  if ( pf ) fseek(f, s->pf_offset, 0) ;
#endif
  s->pfile[which] = f ;
  s->next_pstg[which] = 0 ;
  s->output_flags[which] |= (SET_FILE_EXISTS) ;
  if ( strcmp(rw, "rb") == 0 || strcmp(rw, "w+b") == 0 ||
       strcmp(rw, "r+b") == 0 )
    s->output_flags[which] |= (SET_FOPEN_READ) ;
  if ( strcmp(rw, "wb") == 0 || strcmp(rw, "w+b") == 0 ||
       strcmp(rw, "r+b") == 0 )
    s->output_flags[which] |= (SET_FOPEN_WRITE) ;
  return f ;
}


int
set_close_pf(int setnum, int which)
{
  Setrec *s = Setrecs[setnum] ;
  int result = 0 ;
  /* Don't close dummyset's file 0 (/dev/null) */
  if ( setnum == MAXSETRECS || s == NULL ) return result ;
  if ( s->pfile[which] != NULL && (s->output_flags[which] & SET_FOPEN)) {
    result = bss_fclose(s->pfile[which]) ;
    s->output_flags[which] &= ~(SET_FOPEN) ;
    s->pfile[which] = NULL ;
  }
  return result ;
}

int
set_close_pfs(int setnum)
{
  int j ;
  int result = 0 ;
  for ( j = 0 ; j < 2 ; j++ ) 
    result |= set_close_pf(setnum, j) ;
  return result ;
}

void
bss_pfname(char *buf, int setnum, int ch)
{
  sprintf(buf, "%s"D_C"%s%1d_%04d.%d",
	  Temp_dir, "bss_pf", ch, setnum, Pid) ;
}


/******************************************************************************

  make_set_from_irn()  SW Nov 96

  Given an internal record number, creates a set consisting of this document
  only

******************************************************************************/

int
make_set_from_irn(int irn, int code)
     /* code ;	Unused */
{
  int l ;
  int pstgtype = 0 ;
  int flags = FFL_MEM ;
  Setrec *s = get_setrec(0, 4, flags, pstgtype) ;
  if ( s == NULL ) return -1 ;
  s->pstg->rec = irn ;
  s->set_type = S_IRN ;
  s->adj_no_good = TRUE ;
  l = write_next_pstg(s) ;
  return s->num ;
}

int
make_set_from_rec(int setnum, int recnum, int code)
     /* code ;	Unused */
{
  int irn ;

  if ( setnum != -1 ) {
    if ( prepare_set_for_output(setnum, recnum) < 0 )
      return -1 ;
    if ( get_pstg_n(Setrecs[setnum], recnum) == NULL )
      return -1 ;
    else irn = Setrecs[setnum]->pstg->rec ;
  }
  else irn = recnum ;
  return make_set_from_irn(irn, code) ;
}
