/*****************************************************************************

			        bss_readrecs.c May 96			     
			        from dskutils of 21 Apr 87		     

 Jan/Feb 98: simplified a lot, reduced number of functions.
 rd_recnum does for almost everything
 May 96: removed non-specific functions to utils.c, renamed this to
   bss_readrecs.c
 May 92: added rdr_defaultdb and rdrd_defaultdb
 Oct 89 - relaced MAXRECLEN by 8192 in rdr()
 Revision 15 Dec 87 - altered global addr to Addr
 Last revision 6 July 87 - added  Fsize
 Last revision 2 July 87 - added rea and writ
 Last revision 22 June 87 - added Findfld (was in Disp)

 Time: rdrd: read 5000 records in 1.2$ user, 15$ system                    
*****************************************************************************/

#include "bss.h"
#include "bss_errors.h"

/******************************************************************************

  rdr()

  Removed Feb 98

  Read a complete Okapi record at absolute file address <addr> * Db.rec_mult
  into a buffer.

  Returns length of record (inc. directory and fixed field but not including
  padding at end) starting at buf if OK, else -1 if record too long, -2
  if read or seek fails.

  22 June 92: mod to read from multivolume bibfile

  Modified 22 Apr 87 for new record layout with fixed fields first
  Additional argument pdb Feb 1990 to replace globals Rec_mult, Numflds
  and Fixed

  See also rdr_defaultdb(), same on global db Cdb

  See also rdr_n & rdr_n_defaultdb, read field n only without header or
  directory fields (worth using if only want one field of a long rec)

  Mod Oct 96 to always read into Cdb's recbuf, reallocating if needed
  (see also rdr_n) -- buf arg no longer used

  Mod Feb 98: now static function called by rdr_n if and only if whole
  record required. Header has already been read from disk.

******************************************************************************/

/* rdr_extern() will be rewritten when extern type defined */

#ifdef HAVE_EXTERN_DB
#ifdef LARGEFILES
rdr_extern(OFFSET_TYPE addr, Db *db)
#else
rdr_extern(long addr, Db *db)
#endif
{
  int l ;
  FILE *docf ;
#ifdef LARGEFILES
  OFFSET_TYPE offset ;
#else
  long offset ;
#endif
  int length ;
  char ibuf[1024] ;
  char *buf ;
  char *url ;
  char *path ;
  

  if ( open_bibfile(db, 0) < 0 ) return -2 ;
  buf = db->recbuf ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = fpos_t(addr) ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
    fsetpos(db->current_bibf, &pos) ;
  }
#else
  fseeko(db->current_bibf, addr, SEEK_SET) ;
#endif
#else
  fseek(db->current_bibf, addr, SEEK_SET) ;
#endif
  l = readln(db->current_bibf, ibuf, 1024) ; /* Pathname */
  /* Check if already open, need "current file" of some kind */
  docf = bss_fopen(ibuf, "rb") ;
  if ( docf == NULL ) {
    if ( Dbg & D_DB ) fprintf(bss_syslog, "Can't open %s\n", ibuf) ;
    return -2 ;			/* Wants special value */
  }
  l = readln(db->current_bibf, ibuf, 1024) ; /* Offset */
  offset = atoi(ibuf) ;
  l = readln(db->current_bibf, ibuf, 1024) ; /* Doclength */
  length = atoi(ibuf) ;
  l = readln(db->current_bibf, ibuf, 1024) ; /* URL, not needed */
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = fpos_t(offset) ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
    fsetpos(docf, &pos) ;
  }
#else
  fseeko(docf, offset, SEEK_SET) ;
#endif
#else
  fseek(docf, offset, SEEK_SET) ;
#endif
  /* Check first realloc recbuf */
  fread(db->recbuf, (size_t)length, (size_t)1, docf) ;
  fclose(docf) ;		/* For speed we mightn't want to do this
				   in case another doc in same file */
  return length ;
}
#endif
  
/******************************************************************************

  rdr_n()

  Read all or part of an Okapi record at absolute file address
  <addr> * Db.rec_mult into a buffer.
  fdnum = 0 gets fixed field only, fdnum = -1 gets whole record,
  fdnum = -2 gets header only, fdnum 1 - Db.nf gets requested field only.

  Note that if fdnum = 1 and 
  Returns length of field, which starts at buf if OK
  Returns - if something wrong

  Apr 02: added reclen arg. If db.fddir_recsize = 0 (can't be true unless
  db.nf = 1) this has been obtained from the .bibdrl file, otherwise it's
  ignored (will be -1). 

  May 02: just noticed fdnum -2 is here used to obtain header only
  whereas in bss_getrec_raw() it's used to say only want posting, not
  the document itself. I don't think -2 has ever been used here, but this
  should be changed to (e.g.) -3.

******************************************************************************/

int 
rdr_n(DIR_ADDR_TYPE addr, Db *pdb, int fdnum, int reclen)
{
  char *p ;  
  int length, i, volno ;
  OFFSET_TYPE volsize ;		/* OFFSET_TYPE OK because if not largefiles
				   can't overflow an int */
  int fd ;
  int headerlen ;
  int pad ;			/* Number of pad chars after rec */
  char *buf ;			/* (Oct 96) */

  if ( fdnum < -2 || fdnum > pdb->nf ) return -3 ; /* -2: header only */
  headerlen = pdb->fixed + pdb->nf * pdb->fddir_recsize ;
  volsize = pdb->real_bibsize[0] ;
  for ( volno = 0 ; volno <= pdb->lastbibvol && addr >= volsize ;
       addr -= volsize, volsize = pdb->real_bibsize[++volno] ) ;
  if ( open_bibfile(pdb, volno) < 0 ) return -2 ;
  buf = pdb->recbuf ;
  fd = pdb->m_sfd[volno] ;
  pdb->current_sfd = fd ;	/* (June 92, only for read_next_rec()) */
  addr *= pdb->rec_mult ;	/* no. of bytes spanned by a record address */
  if (get(fd, (OFFSET_TYPE)addr, buf, headerlen ) != (ssize_t)headerlen )
    return -2 ;
  if ( fdnum == 0 )
    return pdb->fixed ;	/* Fixed field if any (tho whole header read) */
  if ( fdnum == -2 ) return headerlen ;
  if ( fdnum == -1 ) {		/* Whole record */
    /* work out record length */
    p = pdb->recbuf + pdb->fixed ;  /* skip over fixed fields */
    if ( pdb->fddir_recsize == 0 ) {
      /* Then already know record length */
      length = reclen ;
    }
    else if ( pdb->fddir_recsize == 2 )
      for ( i = 1, length = 0 ; i <= pdb->nf ; i++, p+=2 )
	length += cv2hl(p) ;
    else
      for ( i = 1, length = 0 ; i <= pdb->nf ; i++, p+=3 ) length += cv3hl(p) ;
    length += headerlen ;
    pad = length % pdb->rec_mult ;
    if ( pad ) pad = pdb->rec_mult - pad ; /* Ensure total amount read include
					    padding at end of rec. This is to
					    allow reading "next" rec
					    sequentially. */
    /* New, Oct 96 */
    if ( length + pad > pdb->recbufsize ) {
      if ( (p = bss_realloc(pdb->recbuf, length + pad)) == NULL ) {
	if ( Dbg & D_DB )
	  fprintf(bss_syslog, "Can't reallocate memory for recbuf\n") ;
	return -5 ;
      }
      else {
	if ( Dbg & D_DB )
	  fprintf(bss_syslog, "recbuf now %d, was %d\n",
		  length + pad, pdb->recbufsize) ;
	buf = pdb->recbuf = p ;
	pdb->recbufsize = length + pad ;
      }
    }
    buf += headerlen ;
    if ( read(fd, buf, (size_t)(length - headerlen + pad) ) !=
	 (ssize_t)(length - headerlen + pad))
      return -2 ;
    return length ;
  }
  else {			/* One field only */
    int skip ;
    /* get field length & start */
    p = buf + pdb->fixed ;  /* skip over fixed fields */
    if ( pdb->fddir_recsize == 3 ) {
      for ( skip = 0, i = 1 ; i < fdnum ; i++, p+=pdb->fddir_recsize )
	skip += cv3hl(p) ;
      length = cv3hl(p) ;
    }
    else {
      for ( skip = 0, i = 1 ; i < fdnum ; i++, p+=pdb->fddir_recsize )
	skip += cv2hl(p) ;
      length = cv2hl(p) ;
    }
    if ( skip > 0 && lseek(fd, (OFFSET_TYPE)skip, SEEK_CUR) < 0 ) return -2 ;
    /* New, Oct 96 */
    if ( length > pdb->recbufsize ) {
      if ( (p = bss_realloc(pdb->recbuf, length)) == NULL ) {
	if ( Dbg & D_DB )
	  fprintf(bss_syslog, "Can't reallocate memory for recbuf\n") ;
	return -5 ;
      }
      else {
	if ( Dbg & D_DB )
	  fprintf(bss_syslog, "recbuf now %d, was %d\n",
		  length, pdb->recbufsize) ;
	buf = pdb->recbuf = p ;
	pdb->recbufsize = length ;
      }
    }
    if ( read(fd, buf, (size_t)length) != (ssize_t)length ) return -2 ;
    return length ;
  }
} /* rdr_n */

/******************************************************************************

  read_record_header  SW May 94

  Reads database record header (= fixed field + field directory) into buf,
  returning length read or -

  Intended for the info rec command for non-text databases

  NB Might consider modifying rdr_n so that it does this if called with
  field = 0

  Feb 98: done above but with field = -2

  No longer needed. Use rd_recnum(., -2, .)


  rd_recnum(recnum, fdnum, db)

  Gets address in textfile (which returns length as well for certain 1-field
  databases), then calls rdr_n which reads the requested text into db.recbuf.

  Field numbers: -2 is header only, -1 entire record, 0 fixed field only
  (obsolete), otherwise field.

******************************************************************************/


int
rd_recnum(int recnum, int fdnum, Db *db)
{
  DIR_ADDR_TYPE addr ;
  int reclen ;

  if ( fdnum == 1 && db->nf == 1 ) fdnum = -1 ;	/* May 02 */
  addr = recnum_to_addr(recnum, db, 0, &reclen) ;
  if ( addr == (DIR_ADDR_TYPE)-1 ) return -2 ;
  return rdr_n(addr, db, fdnum, reclen) ;
}

/******************************************************************************

  real_reclen(rec)    SW June 94

  Return length of Okapi record without header, dir fields or padding

  Record has been read complete, or at least as far as the end of the dir
  fields, into rec

******************************************************************************/

int
real_reclen(char *rec)
{
  u_char *p ;
  int i, length = 0 ;
  
  p = (u_char *) rec + Cdb.fixed ;		/* skip over fixed fields */
  if ( Cdb.fddir_recsize == 2 )
    for ( i = 1 ; i <= Cdb.nf ; i++, p+=2 ) length += cv2hl(p) ;
  else
    for ( i = 1 ; i <= Cdb.nf ; i++, p+=3 ) length += cv3hl(p) ;
  return length ;
}

/******************************************************************************
  
  recnum_to_addr(recnum), using directory file if recptrs empty

  Returns (DIR_ADDR_TYPE) total offset in bibfiles divided by rec_mult
  or -1 if bibdir read fails and no recptrs

  Now recnum_to_addr(recnum, Db, auxnum) where an auxnum > 0 refers to
  an auxiliary textfile (e.g. single field only). Up to Apr 02 no non-zero
  auxnum has been used.

  Mod April 02 so that don't need the HILO def here, and now if db.has_drl
  "returns" record length also, otherwise -1.


******************************************************************************/


DIR_ADDR_TYPE
recnum_to_addr(int recnum, Db *db, int auxnum, int *reclen)
{
  DIR_ADDR_TYPE addr ;
  OFFSET_TYPE pos ;
  int offset_size ;
#ifdef OLD_recnum_to_addr
  int offset ;
#endif
  addr = 0 ;
  if ( auxnum > 0 ) {
#ifdef OLD_recnum_to_addr
    offset = sizeof(addr) - 4 ;	/* Unused unless OLD... */
#endif
    offset_size = 4 ;
  }
  else {
#ifdef OLD_recnum_to_addr
    offset = sizeof(addr) - db->dir_recsize ;
#endif
    offset_size = db->dir_recsize ;
  }
#ifdef OLD_recnum_to_addr
  if ( offset < 0 ) return (DIR_ADDR_TYPE)-1 ;
#endif
  if ( open_bibdir(db, auxnum) == FD_CANT_OPEN ) return (DIR_ADDR_TYPE)-1 ;
  pos = (recnum - 1) * ( db->dir_recsize + db->dir_lensize) ;
#if OLD_recnum_to_addr
  if (get(db->dirfd[auxnum], pos, (char *)&addr + offset, (size_t)nbytes)
      != (ssize_t)nbytes)
    return (DIR_ADDR_TYPE)-1 ;
#if ! HILO
  /* Because bibdir records are always HILO (overhead not great) */
  if ( sizeof(addr) == 8 ) addr = reverse_64(addr) ;
  else addr = reverse(addr) ;
#endif /* ! HILO */
#else /* not OLD_... */
  {
    char buf[8] ;		/* Max drl recsize is 5 + 3 */
    if ( get(db->dirfd[auxnum], pos, buf,
	     (size_t)(offset_size + db->dir_lensize)) !=
	 offset_size + db->dir_lensize ) 
      return (DIR_ADDR_TYPE)-1 ;
    if ( offset_size == 5 ) addr = cv5hl(buf) ;
    else addr = cv4hl(buf) ;
    if ( db->nf > 1 || db->no_drl ) *reclen = -1 ;
    else {
      if ( db->dir_lensize == 2 ) *reclen = cv2hl(buf + db->dir_recsize) ;
      else *reclen = cv3hl(buf + db->dir_recsize) ;
    }
  }    
#endif /* OLD_recnum_to_addr */
#if defined(HAVE_LL)
  return (addr & (LL_TYPE)0x3fffffffffffffff) ;
#else
  return (addr & 0x7fffffff) ;
#endif
}

/* Findfld: return position and length of specified field of Okapi record   */
/* Fixed fields are fldno = 0, others 1 - pdb->nf			    */
/* Returns address of field start, NULL if fldno too high, and length in lp */
/* Apr 02: had to add reclen arg which is the return value if .bibdrl-type db*/
char *
findfld(int fldno, u_char *rec, int *lp, Db *pdb, int reclen)
{
  u_char *dp ;			/* for pointing at record directory */

  if ( fldno > pdb->nf ) { *lp = 0 ; return NULL ; }
  if ( fldno == 0 ) { *lp = pdb->fixed ; return((char *) rec) ; }
  if ( pdb->nf == 1 && ! pdb->no_drl ) {
    *lp = reclen ;
    return (char *)rec ;
  }
  dp = rec + pdb->fixed ;		/* directory portion of record */
  rec += pdb->fixed + pdb->fddir_recsize * pdb->nf ;	/* Point to field 1 */
  while ( --fldno > 0 ) {	/* fields go from 1 to db->nf    */
    if ( pdb->fddir_recsize == 3 ) rec += cv3hl(dp) ;
    else rec += cv2hl(dp) ;
    dp += pdb->fddir_recsize ; 
  }
  if ( pdb->fddir_recsize == 3 )
    *lp = cv3hl(dp);		/* returned length */
  else *lp = cv2hl(dp) ;
  return (char *) rec ; 

}

/* findfld_defaultdb unused Apr 02 */
char *
findfld_defaultdb(int fldno, u_char * rec, int *lp, int reclen)
{
  return findfld(fldno, rec, lp, &Cdb, reclen) ;
}

#ifdef WRITEREC

/******************************************************************************

  writerec()    SW May 96

  Writes out assembled runtime document record, together with directory
  record and paragraph record. Record padding is added in accordance with
  recmult. The directory record is written at offset / rec_mult (not
  checked for divisibility), and the length written (including padding) is
  returned.

  Doesn't really belong here, but never mind. May only be used by frig_recs
  (~okapi/lib). May 1996.

  Feb 98: look at this sometime

******************************************************************************/

int
writerec(char *buf, int l, int rec_mult, int fixed,
	 FILE *bf, FILE *df, FILE *pp, BOOL istext)
{
  int pad, i ;
  char *bp ;
#ifdef LARGEFILES
  OFFSET_TYPE offset = ftello(bf) / rec_mult ;
#else
  long offset = ftell(bf) / rec_mult ;
#endif
  int outl = l ;
  {
    int r = (l + fixed)  % rec_mult ;
    pad = r ? rec_mult - r : 0 ;
  }
  bp = buf ;
  if ( fixed > 0 ) 		/* Have to add this number of bytes */
    for ( i = 0 ; i < fixed ; i++ ) putc('\0', bf) ;
  else {
    bp -= fixed ;		/* Skip this number */
    outl += fixed ;
  }
  if (fwrite(bp, (size_t)outl, (size_t)1, bf) != 1 && outl > 0) return -1 ;
  for ( i = 0 ; i < pad ; i++ ) putc('+', bf) ;
  if ( fwrite((char *)&offset, (size_t)4, (size_t)1, df) != 1 ) return -1 ;
  /* Temp til do para stuff */
  /* if ( pp ) */
  if ( istext ) {
    if ( fwrite("    ", (size_t)4, (size_t)1, df) != 1 ) return -1 ;
  }
  return l + fixed + pad ;
}

#endif /* WRITEREC */
