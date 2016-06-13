/******************************************************************************

  bss_show.c

  from bss_recs.c  SW May 92

  Functions for BSS record dispatch

******************************************************************************/

#include "bss.h"
#include "bss_errors.h"
#include "para.h"

/******************************************************************************

  char *bss_getrec_raw(setnum, recnum, reclen, weight, fdnum)
     int setnum ;
     int recnum ;
     int *reclen ;
     WEIGHT *weight ;
     int fdnum ;

  Returns address of bib record in static storage or NULL on error.
  If not NULL, length is written to reclen and weight to weight. If
  no weights known weight is returned as zero.

  Added pstg arg. Feb 96: removed it.
  Apr 93: added fdnum arg, passed to bss_getrec_by_addr()
  Aug 01: in case fdnum = -2 doesn't read record, just gets pstg
  (for formats 253/4/5)

******************************************************************************/

u_char *
bss_getrec_raw(int setnum, int recnum, Db *db, int *reclen, double *weight, int fdnum)
{
  Setrec *srec ;
  int res ;
  int irn ;			/* Internal record number */
  static char *func = "bss_getrec_raw" ;

  if ( fdnum > db->nf ) {
    *reclen = 0 ;
    *weight = 0 ;
    return NULL ;
  }
  if ( setnum == -1 ) {		/* Denotes display by record number */
    irn = recnum ;
  }
  else {
    if ( (res=set_current_displayset(setnum, recnum)) < -1 ) {
      /* (returns number of previous current set; this is -1 if none) */
      sprintf(Err_buf, "can't set display set to %d, rec %d, results %d",
	      setnum, recnum, res) ;
      sys_err(func) ;
      *reclen = -8 ;
      return NULL ;
    }
    srec = Setrecs[setnum] ;
    if ( get_pstg_n(srec, recnum) == NULL ) {
      sprintf(Err_buf, "can't get requested record's posting") ;     
      sys_err(func) ;
      *reclen = -8 ;
      return NULL ;
    }   
    irn = srec->pstg->rec ;
  }
  if ( fdnum == -2 ) {		/* -2 means just the posting wanted */
    *reclen = 0 ;
  }
  else {			/* fdnum != -2 */
    if ( fdnum == -1 ) {
      if ( irn != Cdb.current_recnum ) {
	res = Cdb.current_reclen = rd_recnum(irn, fdnum, db) ;
	if ( res > 0 ) Cdb.current_recnum = irn ;
      }
      else /* res = 0 ;		 Sep 98 */
	res = Cdb.current_reclen ;
    }
    else {
      res = rd_recnum(irn, fdnum, db) ;
      Cdb.current_recnum = 0 ;
    }
    if ( res < 0 ) {
      sprintf(Err_buf,
      "can't read rec %d, set %d, read result=%d", recnum, setnum, res) ;
      sys_err(func) ;
      *reclen = -9 ;
      return NULL ;
    }
    *reclen = res ;
  }
  if ( setnum != -1 && srec->pstgtype & (HASWT|HAS_INDEX_WT) )
    *weight = srec->pstg->score ;
  else *weight = 0 ;
  return (u_char *)db->recbuf ;
}


/******************************************************************************

char *bss_getfield_raw(rec, fieldnum, fieldlen)

See also bss_show(), bss_getrec_raw()

Returns the address of the requested field. The argument rec points to
the return value of a call of bss_getrec_raw(). The length of the field
is written to fieldlen.

Returns NULL for field number > Cdb->nf

Apr 02: see findfld() for purpose of reclen arg

******************************************************************************/

char *
bss_getfield_raw(int fieldnum, u_char *rec, int *fieldlen, Db *db, int reclen)
{
  return findfld(fieldnum, rec, fieldlen, db, reclen) ;
}


/******************************************************************************

  bss_getrec_by_addr()

  All bss record reads should be done through this.
  Simply calls rdr_defaultdb and maintains Current_rec

  Jan 93: note Current_rec is also reset by bss-show() when a highlighted
  record has been despatched, because this rewrites the rec in recbuf

  Apr 93: mod to take fdnum arg. If fdnum in range reads stated field only.

  Feb 98: Obsolete I think. Removed

******************************************************************************/

/******************************************************************************

  get_rec_addr(setnum, recnum)

  Get record's posting from (setnum, recnum)

  Only the addr field of the posting can be relied upon without investigation
  of the set's posting type.

  Calls set_current_displayset() which does the error checking and most
  of the work (qv)

  Returns 0 if OK and writes record's posting from the set into the supplied
  bss_Gp pointer, or - on error

  Apr 94: if setnum = -1 uses recnum as sequential record number or address.
  This is a messy way of doing it, instead of using the low-level rdrd() etc.
  Some of it is code repeated from rdrd()

  Major change Dec 92: postings with pos fields are variable length, so
  using file1 as an index into file 0.

  May 94: now fetches recnum in the case that postings contain recnum
  instead of address (i.e. DBs of text type). Feb 96: they're all like this
  now.

  Nov 96: probably no longer needed. Should use direct calls of
  set_current_displayset() and get_pstg_n() where required.

  Feb 98: removed

******************************************************************************/


/******************************************************************************

  prepare_set_for_output(setnum, recnum, flags)
  SW May 92, Dec 92, ..Oct 95, debugged again Nov 95

  Was called setup_recs()

  Prepares a set for output of records, by making if necessary 
  pf1 (index by weight). It doesn't make anything unless it's needed,
  but it does leave both pf0 and pf1 open (and rewound) - if files are being
  used.

  Returns 0 if OK and done something, 1 if OK and hasn't needed to do anything,
  otherwise -1 on error (e.g. not being able to open files).

  June 95: Used only to be called by set_current_displayset(). but now
  also used by select_topn. Note that file0 and file1 are left open, so
  calling function may need to close them after.

  Major changes Dec 92 due to many postings now being variable length:
  for simplicity, assume all pstgs are variable length. All sets now have to
  have a record posting file. This now consists simply of a mapping into the
  main posting file, each 4-byte record being the offset of the corresponding
  record in the main posting file. Thus to get record address have to do a seek
  into the main file. Necessitates both files open.

  Added 2nd arg recnum. It is assumed that validity already checked, but
  in the case that pfile[1] already exists and np[1] < recnum + 1 it
  is necessary to resort if necessary and make a larger pfile1. (This
  can only happen at present if pfile[1] is limited to less than naw, as
  may happen in an attempt to speed sorting by weight.)

  March 96: for simplicity assume always use memory, not a file, for the posting
  index, unless POSTING_INDEX_FILE defined (see get_setrec()).

  If raw postings are in a file, it leaves the file open.

  Dec 96: now also called by make_set_from_rec()

  Apr 99: Made minwt float again instead of double. There was sometimes a problem
  comparing srec.pstg.wt with minwt.

******************************************************************************/

struct sbuf {
  int oset ;
  float wt ;
} ;

int
prepare_set_for_output(int setnum, int recnum)
{
  Setrec *srec = Setrecs[setnum] ;
  register FILE *f ;
  register int j ;
  struct sbuf *sbuf = NULL ;	/* malloc'd sort buffer */


  int offset ;
  float minwt ;			/* Changed back from double to float Apr 99
				   Changed from float Jan 98 */
  int num_wanted ;		/*  */
  int num_to_do ;		/* Number to sort (if have weights) */
  int num_out = 0 ;		/* Temp for diagnosis */


  int f1 = srec->output_flags[1] ;
  int mem = (f1 & SET_MEM) ;
  int result ;
  /* Comps on desc weight and asc rec */
  /*int bss_cmp_wts(struct sbuf *, struct sbuf *) ;*/
  int bss_cmp_wts(void *, void *) ;
  static char *func = "prepare_set_for_output" ;

  if ( recnum >= srec->naw ) recnum = srec->naw - 1 ;
  /* Open raw postings */
  if ( open_pstgs(srec, 0, 0) < 0 ) return -1 ;
  /* Set up for postings index */
  /* If already set up and big enough */
  if ( (mem && (f1 & SET_MEM_EXISTS) && srec->np[1] > recnum) ||
      ((f1 & SET_FILE_EXISTS) && srec->np[1] > recnum) ) {
    /* Then just have to open or whatever */
    result = open_pstgs(srec, 1, 0) ;
    if ( result < 0 ) {
      if ( Dbg & D_PSTGS )
	fprintf(bss_syslog, "%s: set %d, can't open pstgs[1] for read\n",
		func, srec->num) ;
      return -1 ;
    }
  } /* Nothing much to do */
  else {			/* Doesn't already exist or not big enough */
    /* Decide how many postings to index (and how much memory needed) */
    /* NOSORT added May 02 for profile sets */
    if ( ! (srec->pstgtype & HASWT) || (srec->set_type & S_NOSORT) ) {
      /* Make complete index */
      num_to_do = srec->naw ;
    } /* (no weights) */
    else { /* Postings have weights and not S_NOSORT, will need sorting */
      if ( Dbg & DD_PSTGS )
	fprintf(bss_syslog,
	"prepare_set_for_output(): resorting (np[1]=%d, naw=%d, rn=%d)\n",
		srec->np[1], srec->naw, recnum) ;
      num_wanted = recnum + 1000 ; /* Do a few extra: 1000 suits TREC eval */
      minwt = MINWEIGHT ;		/* Will be FLT_MIN or something */
      /* Added target_number to condition Aug 02 */
      if ( num_wanted < srec->naw && srec->naw > 10000 &&
	  srec->weight_dist_grad > 0 && srec->target_number > 0 ) {
	for ( j = srec->weight_dist_grad, num_to_do = 0 ; j >= 0 ; j-- ) {
	  num_to_do += srec->weight_dist[j] ;
	  if ( Dbg & D_PSTGS )
	    fprintf(Err_file,
		    "j=%d weight_dist=%d floor=%f num_to_do=%d\n",
		    j, srec->weight_dist[j], srec->weight_floors[j],
		    num_to_do) ; 
	  if ( num_to_do >= num_wanted ) {
	    minwt = srec->weight_floors[j] ;
	    break ;
	  }
	}
	if ( Dbg & D_PSTGS )
	  fprintf(Err_file, "num_to_do=%d, minwt=%f, j=%d, floor=%f\n",
		  num_to_do, minwt, j, srec->weight_floors[j]) ;
      }
      else {
	num_to_do = srec->naw ;
	if ( Dbg & D_PSTGS )
	  fprintf(Err_file, "num_to_do=%d, minwt=%f\n", num_to_do, minwt) ;
      }
      /* Kludge added Aug 02 in case target etc but no weight dist info */
      if ( num_to_do < num_wanted ) num_to_do = srec->naw ;
    } /* Postings have weights */

    if ( mem ) {
      if ( f1 & SET_MEM_MALLOC )
	bss_free(srec->membase[1]) ;
#ifndef NO_MMAP
      else if ( f1 & SET_MEM_MAPPED ) /* But at present it'd never be mapped */
	bss_munmap(srec->membase[1], srec->memsize[1]/Pagesize) ;
#endif
      f1 &= ~SET_MEM_EXISTS ; srec->output_flags[1] = f1 ;
      srec->pbase[1] = srec->membase[1] = srec->pptr[1] =
	bss_malloc(num_to_do * sizeof(int)) ;
      if ( srec->pbase[1] == NULL ) {
	if ( Dbg & DD_PSTGS )
	  fprintf(bss_syslog,
		  "%s: set %d, failed to allocate %d bytes memory\n",
		  func, srec->num, num_to_do * sizeof(int)) ;
	return -1 ;
      }
      srec->memsize[1] = num_to_do * sizeof(int) ;
      srec->next_pstg[1] = 0 ;
      f1 |= SET_MEM_MALLOC ; f1 |= SET_MEM ;
      srec->output_flags[1] = f1 ;
    } /* ( mem ) */
    /* Otherwise have to open the output file */
    else {
      f = set_open_pf(setnum, 1, "w+b") ;
      if ( f == NULL ) return -1 ;
    }
    /* Now have memory or an open file. If no weights or S_NOSORT just output
       otherwise sort first. */
    offset = 0 ;		/* Of raw pstg in mem or file */
    if ( ! (srec->pstgtype & HASWT) || (srec->set_type & S_NOSORT) ) {
      /* then have no weights and don't need to sort */
      for ( j = 0 ; j < srec->naw ; j++ ) {
	if (get_next_pstg(srec) == NULL ) {
	  sprintf(Err_buf, "read fail on posting file, set=%d", setnum) ;
	  sys_err(func) ;
	  goto error ;
	}
	if ( mem ) {
	  memcpy(srec->pptr[1], (char *)&offset, sizeof(int)) ;
	  srec->pptr[1] += sizeof(int) ;
	}
	else /* (file) */ putw(offset, f) ;
	offset += srec->raw_plength ;
      }
      srec->np[1] = srec->naw ;
    } /* (no weights) */
    else {			/* Postings have weights */
      sbuf = (struct sbuf *) bss_malloc(sizeof(struct sbuf) * num_to_do ) ; 
      if ( sbuf == NULL ) {
	mem_err(func, "posting sort buffer", sizeof(struct sbuf) * num_to_do);
	goto error ;
      }
      /* Output pstgs with wt >= minwt for sorting */
      for ( j = 0, offset = 0, num_out = 0 ; j < srec->naw ; j++ ) {
	if ( get_next_pstg(srec) == NULL ) goto error ;
	if ( srec->pstg->score >= minwt ) {
	  if ( num_out >= num_to_do ) {
	    /* This shouldn't happen, but repeatedly has in the past.
	       I think it's OK now (when was this?)
	       Apr 99: it's not OK; decided to disallow more than num_to_do
	       Sep 99: but it's possible (rarely) to lose a high-weighted
	       posting (or more than one?) */
	    if ( Dbg & D_PSTGS ) {
	      fprintf(bss_syslog,
    "%s(): num_out %d >= num_to_do %d, pwt=%f, minwt=%f, j = %d, naw = %d\n",
		    func, num_out, num_to_do,
		      srec->pstg->score, minwt, j, srec->naw) ;
	    }
	    break ;		/* (Apr 99). Next statements never reached */
	  } /* (error case num_out >= num_to_do) */
	  (sbuf + num_out)->wt = srec->pstg->score ;
	  (sbuf + num_out)->oset = offset ;
	  num_out++ ;
	}
	offset += srec->raw_plength ;
      }
      (void) gqsort((char *)sbuf, num_out, sizeof(struct sbuf), bss_cmp_wts) ;
      {
	struct sbuf *nbuf = sbuf;			
	for ( j = 0 ; j < num_out ; j++ ) {
	  if ( mem ) {
	    memcpy(srec->pptr[1], (char *)&nbuf->oset, sizeof(int)) ;
	    srec->pptr[1] += sizeof(int) ;
	  }
	  else /* (file) */ putw(nbuf->oset, f) ;
	  nbuf++ ;
	}
      }
      srec->np[1] = num_out ;
    }
  }
  if ( sbuf ) bss_free(sbuf) ;
  return 0 ;

 error:
  if ( sbuf ) bss_free(sbuf) ;
  return -1 ;
}


/* bss_cmp_wts()
   Comps on desc weight and asc rec addr or offset */

int
/*bss_cmp_wts(struct sbuf *p1, struct sbuf *p2)*/
bss_cmp_wts(void *p1, void *p2)
{
  register double c ;

  c = ((struct sbuf *)p2)->wt - ((struct sbuf *)p1)->wt ;
  if ( c > 0 ) return 1 ;
  else if ( c < 0 ) return -1 ;
  else return ((struct sbuf *)p1)->oset - ((struct sbuf *)p2)->oset ;
}


/******************************************************************************

  set_current_displayset(setnum, recnum)

  Checks db.current_record_srec. If this is not Setrecs[setnum],
  closes it if nec.
  If set.np[1] < recnum does prepare_set_for_output().
  Sets Db.current_record_srec and returns previous set number (-1 if none).

  Returns 1 if nothing to do else 0 if no error else - (Dec 98)

  See also clear_current_displayset

******************************************************************************/

int
set_current_displayset(int setnum, int recnum)
{
  int currentset ;
  int res ;
  static char *func = "set_current_displayset" ;

  if ( Cdb.current_record_srec != NULL )
    currentset = Cdb.current_record_srec->num ; else currentset = -1 ;
  /* Next condition a bit of a bodge */
  if ( setnum == currentset && Setrecs[setnum]->np[1] > recnum )
    return 1 ;		/* Nothing to do */

  clear_current_displayset() ;
  /* Now have to do prepare_set_for_output() (does no harm if already set up */
  res = prepare_set_for_output(setnum, recnum) ;
  /* prepare_set_for_output() leaves the files open and rewound */
  if ( res < 0 ) return -4 ;
  Cdb.current_record_srec = Setrecs[setnum] ;
  return 0 ;
}

/******************************************************************************

  clear_current_displayset()

  Closes postings and sets Cdb.current_record_srec to NULL

  See also set_current_displayset

******************************************************************************/

void
clear_current_displayset(void)
{
  int i ;
  int num ;

  if ( Cdb.current_record_srec == NULL ) num = -2 ;
  else num = Cdb.current_record_srec->num ; 

  if ( num != -2 ) {
    for ( i = 0 ; i < 2 ; i++ ) {
      close_pstgs(Setrecs[num], i, 0) ;	/* 0? */
    }
  }
  Cdb.current_record_srec = NULL ;
}

/******************************************************************************

  bss_highlight_field()  SW Jan 93

  Given a posting with (text) positional info and a raw database field
  corresponding to the posting, put in highlighting on/off strings, rewriting
  to the buffer provided.

  (July 95) If the show format is 3, it doesn't write the text out, but
  just writes highlighting records to the output pointer. The offset field,
  at this stage, contains offset relative to start of field. 

  Not only text.

******************************************************************************/

extern char Hion[], Hioff[];

int
bss_highlight_field(char *ip, int fdlen, int fdnum,
		    FIELD_TYPE field_type, char *op, bss_Gp *pstg,
		    int pstgtype, int format, int start_oset)
{
  int senum ;
  int paralen ;
  int ol ;
  int outlen = 0 ;
  U_pos *posptr = pstg->p ;
  int pos_used ;
  BOOL istext = (Cdb.i_db_type & DB_TEXT_TYPE) ;

  /* Advance posp to this field */
#if HILO
  if ( istext ) while ( posptr->pthl.f < fdnum ) posptr++ ;
  else while ( posptr->lphl.f < fdnum ) posptr++ ;
#else
  if ( istext ) while ( posptr->ptlh.f < fdnum ) posptr++ ;
  else while ( posptr->lplh.f < fdnum ) posptr++ ;
#endif
  senum = 0 ;
  if ( istext ) 
    while ( fdlen > 0 ) {
      paralen = find_para(ip, fdlen, INDENT|GAP) ;
      ol = bss_highlight_para(ip, paralen, op, field_type, posptr,
			      fdnum, &senum, &pos_used, format, start_oset) ;
      ip += paralen ; fdlen -= paralen ; start_oset += paralen ;
      op += ol ; outlen += ol ;
      posptr += pos_used ;
    }
  else
    outlen = bss_highlight_para(ip, fdlen, op, field_type, posptr,
				fdnum, &senum, &pos_used, format, start_oset) ; 
  return outlen ;
}

/******************************************************************************

  bss_highlight_para()    SW June 1994

  Provisional version. Supersedes bss_highlight_rec() for text databases
  and later I hope for all databases.

  Copies over input text inserting highlighting sequences in accordance with
  an array of positional records. Assumes input is a single paragraph and the
  positional records are of type Pos_text (this will be modified later, then
  it should work for units of up to a single field on any database with pos
  recs of type Pos_text or Pos6).

  Repeatedly calls find_sentence() and hl_text(). Returns length output and
  writes number of pos recs used.

  fdnum and senum are the field number and (start) sentence number in the
  field resp. pos, on entry, must be the 1st pos rec with f and s fields
  >= fdnum and senum.

******************************************************************************/

int
bss_highlight_para(char *ip, int il, char *op,
		   FIELD_TYPE field_type, U_pos *posp,
		   int fdnum, int *senum, int *pos_used,
		   int format, int start_oset)
{
  U_pos *pos = posp ;
  int posused ;
  int olength = 0, ol ;
  int sl ;

  *pos_used = 0 ;
  
  while ( il > 0 ) {
    sl = find_sentence(ip, il, field_type, TEXTPOS_MAX_T, 0) ;
    ol = hl_text(ip, sl, op, pos, fdnum, *senum, &posused, format, start_oset) ;
    pos += posused ;
    (*pos_used) += posused ;
    il -= sl ; ip += sl ; start_oset += sl ;
    olength += ol ; op += ol ;
    (*senum)++ ;
  }
  return olength ;
}


/******************************************************************************

  Document show utility functions SW Dec 96

  length_from_header(buf)

  Given (format 4) document header in buf returns document length (raw format
  but without directory records). May be useful for determining bufsize for
  output document. But could more economically do at lower level straight
  from raw doc in bibfile.
  
******************************************************************************/

int
length_from_header(char *doc_header)
{
  int length = 0, l ;
  int ndir ;
  int j ;
  char *p = doc_header + 92 ;
  sscanf(p, "%3d", &ndir) ;
  p += 3 ;
  for ( j = 0 ; j < ndir ; j++ ) {
    p += 11 ;
    sscanf(p, "%8d", &l) ;
    length += l ;
    p += 8 ;			/* Correction Apr 6 98 */
  }
  return length ;
}

