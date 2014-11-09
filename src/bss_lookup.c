/*****************************************************************************

  bss_lookup.c  SW March 92

  Routines for BSS lookup

  Based on searchsubs.c

  May 93: various mods to cope with faulty 1st 2 bytes of chunk in types 1, 2
  and 3 indexes, which couldn't be read backwards

  Jan 96: index types < 4 removed

*****************************************************************************/

#include "bss.h"
#include "bss_errors.h"

/*****************************************************************************

  bss_stype012() Jan 93, June 93

  from bss_stype1() Apr 92  Very slight mods from stype1()
  and bss_stype2() Jan 93

  from stype1 28 Dec 87

  from z80:stype1 July 84

  NB June 93: now does all types of search. Check return values etc.
  Though I think it's still more or less compatible with bss_lkup,
  which is the func that calls it.

  stype                action

  0                    exact match or NULL
  1                    exact match if present otherwise next
 -1                    exact match if present otherwise previous
  2                    next
 -2                    previous

  If there is an exact match it returns the match
  and result 1.  If there is no exact match it backtracks (stype = -1),
  or goes forward (stype = 1), to try to find the first previous
  or first subsequent ITR which matches the limits, and returns
  result 8. Any other result is a failure (inherited from fftge or gprtc
  or gnxtc etc).

  Modified to ?work for index types 1, 2, 3 Jan 93 SW
  and later for types 4, 5, 6 and 7

  Jan 96: index types < 4 removed
  March 96: index types 8 and 9 added & subsequently other types

  May 02: only just noticed bug which caused stype != 0 to fail unpredictably
  as check was not set unless limit non-zero but used in any case.

 *****************************************************************************/

void *
bss_stype012(char *st, int stl, u_int limit, Ix *index,
	     int *result, int stype)
{
  void *itr ;
  int chunk ;
  int length = stl ;
  BOOL check ;

  chunk = bss_pisrch(st, length, index) ;
  if ( chunk < 0 || bss_rdchunk(chunk, index) != 0 ) {
    *result = 0x080 ;
    return(NULL) ;
  }

  itr = bss_fftge(st, length, (u_char *)index->sibuf + 2, index, result) ;
  if ( *result > 0x10 ) return NULL ; /* Index read fail or
					  spurious ITR */

  /* Found term is greater or equal.
     Possibly limits don't match*/

  if ( limit == 0 ) check = 1 ;
  else if ( stype != 0 && limit ) check = sats_limit_itr(itr, index, limit) ;

  switch (stype) {
  case 0:
    /* Search type 0 now gives np=0 if term is found but no postings for
       the limit */
    if ( *result != 1 ) itr = NULL ;
    break ;
  case 1:
    if ( *result == 0x10 ) itr = NULL ;	/* End of index */
    /* If check, we have it already */
    else if ( ! check )	{
      itr = (char *)bss_gnxtc(itr, limit, index, result) ;
      if ( *result >= 0x10 ) itr = NULL ;
    }
    break ;
  case 2:
    if ( *result >= 0x10 ) itr = NULL ;
    else if ( ! check || *result == 1 ) {
      itr = (char *)bss_gnxtc(itr, limit, index, result) ;
      if ( *result >= 0x10 ) itr = NULL ;
    }
    break ;
  case -1:
    if ( ! check || (*result > 1 && *result < 0x08) ||
	( *result == 0x10 && ! check ) ) {
      itr = (char *)bss_gprtc(itr, limit, index, result) ;
      if ( *result >= 0x08 ) itr = NULL ;
    }
    break ;
  case -2:
    if ( ! ( *result == 0x10 && check ) ) {
      itr = (char *)bss_gprtc(itr, limit, index, result) ;
      if ( *result >= 0x08 ) itr = NULL ;
    }
    break ;
  }

  return itr ;
}

/*****************************************************************************

  bss_pisrch()  identical to pisrch() Apr 92

  Based on Pisubs 3 July 87 - Primary index search

  Searches index.pptrs to find the first PI record >= the string term.
  If index.status = 0 or index.numchunks = 0 returns -1, otherwise returns 
  a chunk number (0.. ) in which to start searching.

  Jan 99: maybe speeded slightly

 *****************************************************************************/

int
bss_pisrch(char *term, int l, Ix *index) 
{
  register char **lo = index->pptrs, **hi = index->pptrs + index->numchunks;
  register char **mid ;
  int c ;

#if OBSOLETE
  int nchars ;
  if ( index->numchunks <= 0 ) return -1 ;
  while ( hi >= lo ) {
    mid = lo + ( hi - lo ) / 2  ;
    c = cpasc((*mid)+1, len(*mid), term, l, &nchars) ;
    if ( c & 2 )  /* Term greater */ lo = mid + 1 ;
    else if ( c & 1 ) /* Term less */ hi = mid - 1 ;
    else /* c = 0, equal */ return(mid - index->pptrs) ;
  }
  if ( c & 1 ) return mid - index->pptrs ;
  else return mid - index->pptrs + 1 ;
#else
  if ( index->numchunks <= 0 ) return -1 ;
  while ( hi >= lo ) {
    mid = lo + ( hi - lo ) / 2  ;
    c = cpbytes((*mid)+1, len(*mid), term, l) ;
    if ( c < 0 )  /* Term greater */ lo = mid + 1 ;
    else if ( c > 0 ) /* Term less */ hi = mid - 1 ;
    else /* equal */ return mid - index->pptrs ;
  }
  if ( c > 0 ) return mid - index->pptrs ;
  else return mid - index->pptrs + 1 ;
#endif
}

/*****************************************************************************

  bss_fftge

  Practically the same as old fftge()

  Called by bss_stype012() with entry ITR two bytes after the start of an SI
  chunk.

  Finds first ITR >= search term at or after input ITR

  On success, result is 

    1 : Itr = St
    2 : Itr strictly greater than St
    4 : St left substring of Itr

  Failures include 

    0x10 : reached end of index
    0x40 : spurious Itr
    0x80 : SI read error

  In all cases the return value is the address of the the last (proper)
  Itr considered.

  Very similar to fftge of 7 July 87 (rev up to March 92)

  March 96: added new index types 8 and 9.

*****************************************************************************/
void *
bss_fftge(char *st, int l, void *itr, Ix *ix, int *res)
{
  int c ;
  int offset ;
  int li ;
  int itype = ix->type ;
  
  if ( itype <= 7 ) offset = 2 ;
  else offset = 0 ;

  while ( ( c = cpbytes(st, l, (u_char *)itr + offset + 1,
			len((u_char *)itr + offset) ) ) > 0 ) { 
    /* i.e. st > Itr */
    itr = bss_gnxt(itr, ix, res) ;
    if (*res) return(itr) ; /* Gnxt failed - this may be last term in index */
  }
  /* Now Itr >= St */
  if ( c == 0 ) *res = 1 ;	/* They are equal */
  /* else itr > st, need to know if st is a substring of it */
  else {			/* c < 0 */
    li = len((u_char *)itr + offset) ; 
    if ( l < li && memcmp(st, (u_char *)itr + offset + 1, l) == 0 ) *res = 4 ;	
				/* st left substring of ITR */
    else *res = 2 ;		/* ITR strictly > st */
  }
  return(itr) ;
}

/*****************************************************************************

  Getting the number of postings from an ITR

  getnump - 7 July 87 - old index structure
  
  getnump_ex Oct 1991 - extended index structure

  Rev Dec 1991: take additional args etc

  bss_getnump()

  Returns the number of postings for Itr

  Completely rewritten March 96

  See also sats_limit_itr()

 *****************************************************************************/

int
bss_getnump(void *itr, u_int limit, Ix *index, int *ttf) 
{
  int np, tnp, tf ;
  u_short l = limit ;
  FILE *f ;
  bss_Gp *pstg ;

  tnp = get_tnp_itr(itr, index->type) ;
  tf = get_ttf_itr(itr, index) ;
  if ( limit == 0 ) {
    *ttf = tf ;
    return tnp ;
  }
  f = index->ofp[get_pvol_itr(itr, index->type)] ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = get_ppos_itr(itr, index->type) ;
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
  fseeko(f, get_ppos_itr(itr, index->type), 0) ;
#endif
#else
  fseek(f, (long)get_ppos_itr(itr, index->type), 0) ;
#endif
  /* Read <tnp> pstgs & count the number that satisfy the criteria */
  pstg = make_bss_Gp(MAXTEXTPOS) ;/* Biggest possible */
  if ( pstg == NULL ) return -3 ; /* ? */
  for ( np = 0, tf = 0 ; tnp > 0 ; tnp-- ) {
    read_pstg_unpacked(f, &pstg, index->pstgtype, TRUE);
    if ( (l & Cdb.lims[pstg->rec - 1]) == l ) {
      np++ ;
      tf += pstg->numpos ;
    }
  }
  bss_free((void *)pstg) ;
  *ttf = tf ;
  return np ;
}

/*****************************************************************************

  bss_gprt() Apr 92  Identical to gprt()
  Has been modified for types 0, 1, 2 and 3 indexes

  gprt() 6 July 87

  Given an ITR in index.sibuf, reads backwards to find previous itr, if
  possible. If OK, result = 0, and gprt is address of prev. itr. Otherwise 
  gprt returns itr, and result is one of:

    0x08 (was 20 - June 93) - have already got first itr in index;
    0x40 - entry itr implausible. (itype 0 only I think)
    0x80 - read failed.

*****************************************************************************/

void *
bss_gprt(void *itr, Ix *index, int *result) 
{
  u_short l ;
  u_char *p = itr ;
  int itype = index->type ;

  if ( itype < 8 ) {			/* itype 4 - 7 */
    p -= 2 ; /* p at backpointer = length of previous record */
    l = cv2(p) ;
    if ( l == 0x8000 ) { /* Beginning of index */
      *result = 0x8 ; return(itr) ;
    }
    /*if (l & 0x8000) { Beginning of chunk */
    /* Bodge for faulty indexes follows */
    if ( p == index->sibuf ) {
#ifdef DEBUG_LKUP
      fprintf(Err_file, "First in chk: %.*s\n", *(p+4), p+5);
#endif
      *result = bss_rdchunk(index->cchunk - 1, index) ;

      if (*result != 0) { *result = 0x80 ; return(itr) ; }
      /* NB cannot rely on buffer unchanged in above case - calling 
	 program should give up, or retry or something */
      l &= 0x7fff ;
      p += index->schunksize - 2 ; /* last but one byte of chunk */
      /* Dreadful bodge to cope with what seem to be defective indexes 5/93 */
      /* NBB This could fail if ITR was exactly 256 bytes long */
      while ( *p == 0 ) p-- ;	/* Nonzero byte is last byte of ITR */
      p-- ; l = cv2(p) ;
    }
    *result = 0 ; return p - l + 2 ;
  }
  else  {			/* itype >= 8 */
    p -= 1 ; /* p at length of previous record's term field
	        unless the entry ITR was first in a chunk */
    if ( p == (u_char *)index->sibuf + 1 ) {
      p-- ; l = cv2(p) ;
      if ( l == 0x8000 ) {	/* Beginning of index */
	*result = 0x8 ; return(itr) ;
      }
      *result = bss_rdchunk(index->cchunk - 1, index) ;
      if (*result != 0) {
	*result = 0x80 ; return(itr) ;
      }	/* Shouldn't happen anyway */
      l &= 0x7fff ;
      p += index->schunksize - l ;
    }
    else {			/* Not 1st in chunk */
      p -= (*(u_char *)p + 22) ;
      if ( itype == 11 || itype == 12 || itype == 13 ) p-- ;
    }
    *result = 0 ; return p ;
  }
}

/******************************************************************************

  bss_gprtc(), bss_gnxtc()

  Finds first previous/subsequent itr satisfying limit. Returns
  result as gprt()/gnxt()
  + 0x20 if the index is too sparse -  i.e. MAXIXREAD itrs were read without 
  finding one which matches.

  In all failure (*result != 0) cases, the index is reread if necessary and 
  the entry itr is supposed to be returned.

  These two functions are almost identical

******************************************************************************/

void *
bss_gprtc(void *itr, u_int limit, Ix *index, int *result) 
{
  int savchk = index->cchunk ;
  void * savptr = itr ;
  int termcount = 0 ;

  do {
    itr = bss_gprt(itr, index, result) ;
    termcount++ ;
    if ( termcount > MAXIXREAD ) *result = 0x020 ;
    if ( *result ) {
      bss_rdchunk(savchk, index) ;
      return(savptr) ;
    }
  }
  while ( ! sats_limit_itr(itr, index, limit) ) ;
  if ( Dbg & D_SEARCH ) fprintf(bss_syslog, "termcount %d\n", termcount) ;
  *result = 0 ; return itr ;
}

void *
bss_gnxtc(void *itr, u_int limit, Ix *index, int *result) 
{
  int savchk = index->cchunk ;
  void * savptr = itr ;
  int termcount = 0 ;

  do {
    itr = bss_gnxt(itr, index, result) ;
    termcount++ ;
    if ( termcount > MAXIXREAD ) *result = 0x020 ;
    if ( *result ) {
      bss_rdchunk(savchk, index) ;
      if ( Dbg & D_SEARCH ) fprintf(bss_syslog, "termcount %d\n", termcount) ;
      return savptr ;
    }
  }
  while ( ! sats_limit_itr(itr, index, limit) );
  if ( Dbg & D_SEARCH ) fprintf(bss_syslog, "termcount %d\n", termcount) ;
  *result = 0 ; return itr ;
}

/*****************************************************************************

  bss_gnxt() Apr 92 Identical to gnxt

  gnxt

  Given an ITR in index.sibuf, reads forward to find next itr, if
  possible. If OK, result = 0, and gnxt is address of next itr. Otherwise
  gnxt returns itr, and result is one of:
    0x10 - have already got last itr in index;
    0x40 - entry itr implausible, or dummy eof rec
    0x80 - read failed.  

    Dec 1991: new function for type 1 index, old for type 0 only

  Jan 96: now index types >= 4 only

  See also skip_itr(), doesn't do rdchunk etc (Dec 98, but we had this
  in the old Z80 system I think).

*****************************************************************************/

void *
bss_gnxt(void *itr, Ix *index, int *result)
{
  int reclen, newlen ;
  int termlen, newtermlen ;
  u_char *p ;
  int itype = index->type ;

  switch (itype) {
  case 4:
  case 5:
  case 6:
  case 7:
    reclen = cv2(itr) ;
    if ( reclen == 0x8000 ) {	/* This would be the case if (1) the "ITR" was
				   the dummy one after the last record in the
				   index, or (2) this is the beginning of the
				   first chunk in the index. */
      *result = 0x40 ; return itr ; /* Neither (1) nor (2) should happen */
    }
    if ( reclen != 0 ) {
      p = (u_char *)itr + reclen ;
      newlen = cv2(p) ;		/* Next ITR */
      if ( newlen == 0x8000 ) {
	*result = 0x10 ;		/* Already had last ITR in index */
	return itr ;
      }
    }
    if ( reclen == 0 || newlen == 0 ) {
      /* Either called at end of chunk or entry term was last in chunk */
      *result = bss_rdchunk(index->cchunk + 1, index) ;
      if (*result != 0) {
	*result = 0x80 ;
	return itr ;
      }
      /* NB in above case entry itr may be lost, calling prog shouldn't rely */
      
      else {
	p = (u_char *)index->sibuf + 2 ;	/* Skip start of chunk word */
	newlen = cv2(p) ;
	if ( newlen == 0x8000 ) {	/* End of index at chunk start */
	  *result = bss_rdchunk(index->cchunk - 1, index) ;
	  if ( *result != 0 ) {
	    *result = 0x80 ;
	    return itr ;
	  }
	  else {
	    *result = 0x10 ;
	    return itr ;
	  }
	}
      }
    }
    break ;

  case 11:
  case 12:
  case 13:
    termlen = *(u_char *)(itr) ;
    if ( termlen == 0xff ) {	/* This corresponds to case (1) above for the
				   other index types. */
      *result = 0x40 ; return itr ;
    }
    if ( termlen != 0 ) {	/* If it was, that would be end of chunk */
      reclen = termlen + 24 ;	/* See SI layout in e.g. ixf.c */
      p = (u_char *)itr + reclen ;	/* Start of next ITR or dummy end of
					   chunk */
      newtermlen = (u_char)(*p) ;
      if ( newtermlen == 0xff ) { /* End of index */
	*result = 0x10 ;
	return itr ;		/* i.e. entry ITR */
      }
    }
    if ( termlen == 0 || newtermlen == 0 ) {
    /* Surely termlen can only be 0 if called with dummy ITR, can this happen?*/
      *result = bss_rdchunk(index->cchunk + 1, index) ;
      if ( *result != 0 ) {	/* Read error on SI */
	*result = 0x80 ; return itr ;
      }
      else {
	p = (u_char *)index->sibuf + 2 ;
	newtermlen = (u_char)(*p) ;
	if ( newtermlen == 0xff ) {
	  *result = bss_rdchunk(index->cchunk - 1, index) ;
	  if ( *result != 0 ) {
	    *result = 0x80 ; return itr ;
	  }
	  else {
	    *result = 0x10 ;
	    return itr ;
	  }
	}
      }
    }
    break ;

  default:			/* i.e. 8 - 10  and 20 */
    termlen = *(u_char *)(itr) ;
    if ( termlen == 0xff ) {	/* This corresponds to case (1) above for the
				   other index types. */
      *result = 0x40 ; return itr ;
    }
    if ( termlen != 0 ) {	/* If it was, that would be end of chunk */
      reclen = termlen + 23 ;	/* See SI layout */
      p = (u_char *)itr + reclen ;	/* Start of next ITR or dummy end of
					   chunk */
      newtermlen = (u_char)(*p) ;
      if ( newtermlen == 0xff ) { /* End of index */
	*result = 0x10 ;
	return itr ;		/* i.e. entry ITR */
      }
    }
    if ( termlen == 0 || newtermlen == 0 ) {
    /* Surely termlen can only be 0 if called with dummy ITR, can this happen?*/
      *result = bss_rdchunk(index->cchunk + 1, index) ;
      if ( *result != 0 ) {	/* Read error on SI */
	*result = 0x80 ; return itr ;
      }
      else {
	p = (u_char *)index->sibuf + 2 ;
	newtermlen = (u_char)(*p) ;
	if ( newtermlen == 0xff ) {
	  *result = bss_rdchunk(index->cchunk - 1, index) ;
	  if ( *result != 0 ) {
	    *result = 0x80 ; return itr ;
	  }
	  else {
	    *result = 0x10 ;
	    return itr ;
	  }
	}
      }
    }
    break ;
  }
  /* Normal case */
  *result = 0 ;
  return p ;
}
  
/******************************************************************************

  skip_itr(itr, index_type) SW (Dec 98)

  Returns address of byte after end of entry ITR, or NULL if apparently
  called with end of chunk or end of index.

  Only works for index types 8 - 13

******************************************************************************/

void *
skip_itr(void *itr, int itype)
{
  int l ;
  /* int itrlen ; unused */
  l = *(unsigned char *)itr ;
  if ( l == 0 || l == 0xff ) return NULL ;
  switch(itype) {
  case 8:
  case 9:
  case 10:
  case 20:
    return (u_char *)itr + l + 23 ;
  case 11:
  case 12:
  case 13:
  default:
    return (u_char *)itr + l + 24 ;
  }
}

/*****************************************************************************

  Getitr 28 Feb 88
  Read secondary index chunk and return itr at specified offset.
  Used by gnpcont, gprpcont

  March 2002: unused. Renamed from bss_getitr to bss_getitr_by_addr

*****************************************************************************/

void *
bss_getitr_by_addr(int chunk, int offset, Ix *index)
{
  if ( bss_rdchunk(chunk, index) != 0 ) return(NULL) ;
  return (u_char *)index->sibuf + offset ;
}
    
/*****************************************************************************

  Rdchunk 6 July 87

  Read given chunk number (0.. ) into the given INDEX structure. Returns 0
  if OK. If OK, index.cchunk contains the chunk number and the chunk has
  been read into index.sibuf. Errors include: 1 if index.status != 1, 
  2 if chunk number is too large, 4 if SI read failed.

*****************************************************************************/

int
bss_rdchunk(int chunknumber, Ix *index) 
{
  size_t chunksize = index->schunksize ;

  if (index->status != 1) return 1 ;
  if ( chunknumber >= index->numchunks ) return 2 ;
  if ( chunknumber == index->cchunk ) return 0 ; /* Nothing to do */
  if (get(index->sifd, (OFFSET_TYPE) ((OFFSET_TYPE)chunknumber * chunksize),
	  index->sibuf, chunksize) != (ssize_t)chunksize)
    return 4 ; 
  index->cchunk = chunknumber ;
  return 0 ;
}

/******************************************************************************

  parse_itr(itr, itype, Transfer_itr)   SW March 2002 in connection with
  hash storage of SI
  
  Copies fields from an ITR (index types >= 8 only) into a Transfer_itr
  (hash.h)

  Returns address of termrec
  
  Note that the term is just a pointer to the term in the ITR

******************************************************************************/

Transfer_itr *
parse_itr(void *itr, int itype, Transfer_itr *tr)
{
  u_int offset ;
  char *p = itr ;
  if ( itype < 8 || (itype > 13 && itype != 20) ) return NULL ;

  tr->itype = itype ;
  tr->termlen = *(u_char *)p++ ;
  tr->term = p ; p += tr->termlen ;
  memcpy((void *)&(tr->limits), p, 16) ; p += 16 ;
  tr->pstg_vol = *(u_char *)p++ ;
  switch (itype) {
  case 8:
  case 9:
  case 10:
  case 20:
    /* ITR's pstg offset length 4 */
    memcpy((void *)&offset, p, 4) ;
    tr->pstg_offset = offset ;
    break ;
  default:
    /* Otherwise 5 (but watch it if other index types in future */
    if ( sizeof(off_t) != 8 ) return NULL ;
    tr->pstg_offset = 0 ;
#if ! HILO
    memcpy((void *)&(tr->pstg_offset), p, 5) ;
#else
    memcpy((u_char *)&(tr->pstg_offset) + 3, p, 5) ;
#endif
  }
  return tr ;
}

/******************************************************************************

  read_itr_seq(buf, itype, file)   SW March 2002

  Assuming a secondary index file is positioned at the length byte at the
  start of an ITR (this may be the "empty" ITR at the end of a chunk or the
  dummy ITR at the end of the index), reads the complete ITR into buf (unless
  end of index) and leaves the file positioned at the length byte of the next
  ITR.
  
  It is assumed that buf is large enough (max length for index types 7-13 is
  254 + 24 bytes (MAXEXITERMLEN + 24)).

  Returns 1 if OK, 0 if end of index, -1 read error, -2 unsupported index type.

  Only works for index types 7-13.

******************************************************************************/

int
read_itr_seq(void *buf, int itype, FILE *f)
{
  int l, ch ;
  void *p = buf ;
  if ( itype < 7 || (itype > 13 && itype != 20) ) return -2 ;
  l = (ch = getc(f)) ;
  if ( l == 0 ) {
    while ( (ch = getc(f)) == 0 ) ;
    ch = getc(f) ;		/* Second byte of chunk word */
    l = (ch = getc(f)) ;
  }
  else if ( ch == 0xff ) return 0 ;
  
  /* mko - change to separate lines */
  (u_char *)p++;   /* advance the pointer */
  *(u_char *)p = l;/* assign contents of pointer p to l */
  /*  (*((u_char *)p)++) = l ; */
  if ( itype < 11 || itype == 20 ) fread(p, (size_t)(l + 22), (size_t)1, f) ;
  else fread(p, (size_t)(l + 23), (size_t)1, f) ;
  if ( ferror(f) ) return -1 ;
  return 1 ;
}
/******************************************************************************

  get_term_itr(itr, itype)  SW May 93

  Gets term from ITR into static storage. Empty string if itr NULL etc.

******************************************************************************/

char *
get_term_itr(void *itr, int itype)
{
  static char *term = NULL ;
  static int space = 0 ;
  int leng ;
  u_char *p ;
  int offset = (itype < 8) ? 2 : 0 ;

  if ( itr == NULL ) leng = 0 ;
  else {
    p = (u_char *)itr + offset ;
    leng = *(u_char *)p++ ;
  }
  if ( space < leng + 1 ) {
    if ( term == NULL ) term = bss_malloc(leng + 1) ;
    else term = bss_realloc(term, leng + 1) ;
    if ( term == NULL ) return NULL ;
    space = leng + 1 ;
  }
  if ( itr != NULL ) strncpy(term, (char *)p, leng) ;
  term[leng] = '\0' ;
  return term ;
}

/******************************************************************************

  Various things from an ITR    SW Jan 96

******************************************************************************/

int
get_tnp_itr(void *itr, int itype)
{
  int tnp = 0 ;
  void *p ;

  if ( itr == NULL ) return 0 ;
  switch (itype) {
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 20:
    p = (u_char *)itr + 9 + *((u_char *)itr) ;
    tnp = cv4(p) ;
    break ;
  default:
    p = (u_char *)itr + 7 + *((u_char *)((u_char *)itr + 2)) ;
    tnp = cv3(p) ;
    break ;
  }
  return tnp ;
}

int
get_ttf_itr(void *itr, Ix *index) /* If all ttf recorded exactly (itype > 7)
					would only need itype here */
{
  int itype = index->type ;
  int ttf ;
  void *p ;
  int l, tnp ;

  if ( itr == NULL ) return 0 ;

  switch (itype) {
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 20:
    p = (u_char *)itr + 5 + *(u_char *)itr ;
    ttf = cv4(p) ;
    break ;
  default:
    /* Note this is inaccurate for indexes of types < 8 where ttf is stored as
       a log, see hack below for upper bound */
    /* p = itr + 5 + *((u_char *)(itr + 2)) ;
       memcpy((char *)&lttf, p, 2) ;
       ttf = alg(lttf) ; */
    l = get_plength_itr(itr, index) ;
    tnp = get_tnp_itr(itr, index->type) ;
    /* This gives an upper bound (assume all numpos fields are 1 byte,
       some may really be 2. */
    ttf = (double)(l - tnp * 4) / sizeof(U_pos) ;
  }
  return ttf ;
}

int
get_maxtf_itr(void *itr, Ix *index)
{
  int maxtf ;
  void *p ;

  if ( itr == NULL ) return 0 ;

  switch (index->type) {
  case 8:
  case 9:
  case 10:
  case 20:
  case 11:
  case 12:
  case 13:
    p = (u_char *)itr + 3 + *(u_char *)itr ;
    maxtf = cv2(p) ;
    break ;
  default:
    maxtf = 0 ;			/* Not known for itypes < 8 */
  }
  return maxtf ;
}

OFFSET_TYPE
get_ppos_itr(void *itr, int itype)
{
  void *p ;
  OFFSET_TYPE ppos ;

  if ( itr == NULL ) return 0xffffffff ;

  switch (itype) {
  case 8:
  case 9:
  case 10:
  case 20:
    p = (u_char *)itr + *(u_char *)itr + 18 ;
    ppos = (OFFSET_TYPE)cv4(p) ;
    break ;
  case 11:
  case 12:
  case 13:
    p = (u_char *)itr + *(u_char *)itr + 18 ;
    ppos = cv5(p) ;
    break ;
  default:
    p = (u_char *)itr + cv2(itr) - 6 ;
    ppos = (OFFSET_TYPE)cv4(p) ;
  }
  return ppos ;
}

int
get_pvol_itr(void *itr, int itype)
{
  int vol ;

  if ( itr == NULL ) return 0 ;

  switch(itype) {
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 20:
    vol = *(u_char *)((u_char *)itr + *(u_char *)itr + 17) ;
    break ;
  case 6:
  case 7:
    vol = *(u_char *)((u_char *)itr + cv2(itr) - 7) ;
    break ;
  case 4:
  case 5:
  default:
    vol = 0 ;
  }
  return vol ;
}


/* Mod Feb 96 so restores current index chunk if necessary. */

int
get_plength_itr(void *itr, Ix *index)
{
  int itype = index->type ;
  int plength ;
  int result ;
  int next_vol ;
  int chunknum = index->cchunk ;
  void *next_itr ;
  OFFSET_TYPE start = get_ppos_itr(itr, index->type) ;
  OFFSET_TYPE next_start ;
  int vol = get_pvol_itr(itr, index->type) ;

  if ( itr == NULL ) return 0 ;

  switch (itype) {
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 20:
    (void)memcpy((void *)&plength, (void *)((u_char *)itr + *(u_char *)itr + 13), 4) ;
    break ;
  default:
    /* Have to work it out */
    next_itr = bss_gnxt(itr, index, &result) ;
    /* Now itr may no longer be valid */
    if ( result == 0 ) {
      next_start = get_ppos_itr(next_itr, index->type) ;
      next_vol = get_pvol_itr(next_itr, index->type) ;
    }
    
    if ( result == 0 && vol == next_vol ) plength = next_start - start ;
    else if ( result == 0x10 || ( result == 0 && vol != next_vol ) )
      plength = index->pfsize[vol] - start ;
    /* 0x40 is end of index */
    else plength = -1 ;			/* there's something odd */
    if ( result == 0x10 )
      fprintf(stderr, "gnxt reached end of index\n") ;
    bss_rdchunk(chunknum, index) ;
    break ;
  }
  return plength ;
}


u_int
get_limits_itr(void *itr, Ix *index)
{
  int offset ;
  u_short l ;
  u_char *p ;

  if ( itr == NULL ) return 0 ;

  switch (index->type) {
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
    offset = 0 ;
    break ;
  default:
    offset = 2 ;
    break ;
  }
  p = (u_char *)itr + offset ;
  p += *(u_char *)p + 1 ;
  (void)memcpy((void *)&l, (void *)p, 2) ;
  l = reverse_16(l) ;
  return (u_int)l ;
}

/******************************************************************************

  sats_limit_itr   SW Oct 96

  If limit = 0 or index.lims_invalid is false, simple;
  otherwise have to look at postings to get recnum, and use the lims array

  This becomes necessary if we allow secondary indexes without limits info
  or more than one limits array.

  Note similarity to bss_getnump when index->lims_invalid

  An extension of this would be a function which finds all the limits satisfied
  by an index term (e.g. for inserting limits into an SI which hasn't got them)

******************************************************************************/

int
sats_limit_itr(void *itr, Ix *index, u_int limit)
{
  int tnp ;
  FILE *f ;
  u_short l = limit ;
  u_short i_limit ;
  bss_Gp *pstg = NULL ;
  int success = 0 ;
  if ( limit == 0 ) return 1 ;
  i_limit = get_limits_itr(itr, index) ;
  if ( i_limit && ! index->lims_invalid ) return (i_limit & l) == l ;
  /* Otherwise either SI has non-zero limit fields which are invalid, or it
     has zero limit fields. Then we have to check each posting. */
  pstg = make_bss_Gp(MAXTEXTPOS) ;/* Biggest possible */
  if ( pstg == NULL ) return -3 ; /* ? */
  tnp = get_tnp_itr(itr, index->type) ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = get_ppos_itr(itr, index->type) ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
    fsetpos((f = index->ofp[get_pvol_itr(itr, index->type)]), &pos) ;
  }
#else
  fseeko((f = index->ofp[get_pvol_itr(itr, index->type)]),
	 get_ppos_itr(itr, index->type), 0) ;
#endif
#else
  fseek((f = index->ofp[get_pvol_itr(itr, index->type)]),
	(long)get_ppos_itr(itr, index->type), 0) ;
#endif
  while ( tnp-- ) {
    read_pstg_unpacked(f, &pstg, index->pstgtype, TRUE) ;
    if ( (l & Cdb.lims[pstg->rec - 1]) == l ) {
      success = 1 ;
      break ;
    }
  }
  if ( pstg != NULL ) bss_free(pstg) ;
  return success ;
}
