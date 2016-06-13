/******************************************************************************

  bss_passages.c  SW June 94

  Experimenting with passage weighting

  For layout of paragraph records see ~okapi/doc/bss/paragraphs

******************************************************************************/

/* #define FASTER */

#include "bss.h"
#include "bss_errors.h"

int f_num ;
int f_oset ;
int f_len ;
int f_numpars ;
int *firstpara ;		/* Filled in by para_get_field */

/******************************************************************************

  read_plain_para_rec(file, offset, place)

  Reads in a document's paragraph records from (file, offset) into an integer
  array at place.

  Returns 0 if OK or - if can't read parafile

  Temporary May 94 for testing paragraph utilities
  Still I guess temporary Sep 98

  Buffer is now (Sep 98) always Db.parabuf (of Db.parabuf_size * sizeof(int)
  bytes). A doc needs 3 + 4 * nf + 2 * totpars integer fields. Start by
  allowing for about 100 paras, then realloc as dictated by the numpars field.

******************************************************************************/
#undef DEBUG

#define Check_parabuf \
      amount_needed = reclen + 4 + numpars * 2 ;\
      if ( amount_needed > Cdb.parabuf_size ) {\
	Cdb.parabuf =\
	  bss_realloc(Cdb.parabuf, amount_needed * sizeof(int)) ;\
	if ( Cdb.parabuf == NULL ) return -5 ;\
	Cdb.parabuf_size = amount_needed ;\
	t = Cdb.parabuf + reclen ;\
	if ( Dbg & (D_PASSAGES|DD_PASSAGES) )\
	  fprintf(bss_syslog, "Reallocating parabuf to %d\n",\
		  amount_needed ) ;\
      }\

int
#ifdef LARGEFILES
read_plain_para_rec(OFFSET_TYPE oset, int parafile_type)
#else
read_plain_para_rec(long oset, int parafile_type)
#endif
{
  FILE *f = Cdb.parafile ;
  int *t ;
  int fdnum, numpars, j ;
  int reclen = 0 ;
  int amount_needed ;

  if ( f == NULL ) return -1 ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
  {
    fpos_t pos = oset ;
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
  fseeko(f, oset, 0) ;
#endif
#else
  fseek(f, oset, 0) ;
#endif
  if ( Cdb.parabuf == NULL ) {
    /* Start by allocating an arbitrary amount */
    Cdb.parabuf = (int *)bss_malloc(220 * sizeof(int)) ;
    if ( Cdb.parabuf == NULL ) return -5 ;
    else Cdb.parabuf_size = 220 ;
  }
  t = Cdb.parabuf ;
  if ( parafile_type == 0 ) {
    fscanf(f, "%d %*s %d %d ", t, t + 1, t + 2) ; t += 3 ;
    reclen += 3 ;
    for ( fdnum = 1 ; fdnum <= Cdb.nf ; fdnum++ ) {
      fscanf(f, "%d%d%d ", t, t + 1, t + 2) ; t += 3 ;
      fscanf(f, "%d", &numpars) ; *t++ = numpars ;
      reclen += 4 ;
      Check_parabuf 
      for ( j = 0 ; j < numpars ; j++ ) {
	fscanf(f, "%d%d", t, t + 1) ; t += 2 ;
      }
      reclen += numpars * 2 ;
    }
  }
  else if ( parafile_type == 1 ) {
    int fdnum, offset, fdlen, numpars ;
    t += 3 ;			/* Fields not present in type 1 */
    reclen += 3 ;
    for ( fdnum = 1 ; fdnum <= Cdb.nf ; fdnum++ ) {
      fdnum = getc(f) ;
      offset = r3hl(f) ;
      fdlen = r3hl(f) ;
      numpars = r2hl(f) ;
#ifdef DEBUG
      fprintf(stderr, "fd %d oset %d len %d pars %d\n",
	      fdnum, offset, fdlen, numpars) ;
#endif
      *t++ = fdnum ;
      *t++ = offset ;
      *t++ = fdlen ;
      *t++ = numpars ;
      reclen += 4 ;
      Check_parabuf
      for ( j = 0 ; j < numpars ; j++ ) {
	*t++ = r3hl(f) ;
	*t++ = r2hl(f) ;
#ifdef DEBUG
	fprintf(stderr, "End oset + 1 %d last senum %d\n",
		*(t-2), *(t-1)) ;
#endif
      }
      reclen += numpars * 2 ;
    }
  }
  else if ( parafile_type == 2 ) {
    int fdnum, offset, fdlen, numpars ;
    int l ;
    t += 3 ;			/* Fields not present in type 2 */
    reclen += 3 ;
    for ( fdnum = 1 ; fdnum <= Cdb.nf ; fdnum++ ) {
      fdnum = getc(f) ;		/* Well... */
      offset = r1to4(&l, f) ;
      fdlen = r1to4(&l, f) ;
      numpars = r1to4(&l, f) ;
#ifdef DEBUG
      fprintf(stderr, "fd %d oset %d len %d pars %d\n",
	      fdnum, offset, fdlen, numpars) ;
#endif
      *t++ = fdnum ;
      *t++ = offset ;
      *t++ = fdlen ;
      *t++ = numpars ;
      reclen += 4 ;
      Check_parabuf
      for ( j = 0 ; j < numpars ; j++ ) {
	*t++ = r1to4(&l, f) ;
	*t++ = r1to4(&l, f) ;
#ifdef DEBUG
	fprintf(stderr, "End oset + 1 %d last senum %d\n",
		*(t-2), *(t-1)) ;
#endif
      }
      reclen += numpars * 2 ;
    }
  }
  return 0 ;
}
#undef DEBUG

/******************************************************************************

  get_para_offset(IRN)

  Offset of IRN's paragraph data in database's paragraph file.

******************************************************************************/


OFFSET_TYPE
get_para_offset(int recnum)
{
  OFFSET_TYPE offset ;
  if ( Cdb.paraptrs == NULL ) {
#ifdef LARGEFILES
#ifdef NO_FSEEKO
    {
      fpos_t pos = (fpos_t)((recnum - 1) * 4) ;
    if ( Dbg & D_FILES ) fprintf(bss_syslog, 
#ifdef _WIN32
				 "fsetpos(%I64d)\n",
#else
				 "fsetpos(%lld)\n",
#endif
				 pos) ;
      fsetpos(Cdb.pardirf, &pos) ;
    }
#else
    fseeko(Cdb.pardirf, (OFFSET_TYPE)((recnum - 1) * 4), 0) ;
#endif
#else
    fseek(Cdb.pardirf, (long)((recnum - 1) * 4), 0) ;
#endif
    offset = getw(Cdb.pardirf) ;
#if ! HILO
    offset = reverse(offset) ;
#endif
  }
  else offset = Cdb.paraptrs[recnum - 1] ;
  return offset ;
}

int *
get_para_rec(int recnum)
{
  int res ;
  res = read_plain_para_rec(get_para_offset(recnum), Cdb.parafile_type) ;
  if ( res < 0 ) return NULL ;
  return Cdb.parabuf ;
}


int *
para_get_field(int fdnum)
{
  int num_paras ;
  int j ;
  int *ip = Cdb.parabuf ;
  ip += 3 ;
  for ( j = 0 ; j < fdnum - 1 ; j++ ) {
    ip += 3 ;
    num_paras = *ip++ ;
    ip += num_paras * 2 ;
  }
  f_num = *ip++ ; f_oset = *ip++ ; f_len = *ip++ ; f_numpars = *ip++ ;
  firstpara = ip ;
  return firstpara ;
}

int
find_para_len(int paranum)
{
  int *ip = firstpara ;
  if ( paranum == 0 ) return *ip ;
  ip += paranum * 2 ;
  return *ip - *(ip - 2) ;
}

int
find_passage_len(int spara, int fpara)
{
  int *ip = firstpara ;
  ip += fpara * 2 ;
  if ( spara == 0 ) return *ip ;
  else return *ip - *(ip - (fpara - spara + 1) * 2) ;
}

int
find_para_offset(int paranum)
{
  int *ip = firstpara ;
  if ( paranum == 0 ) return 0 ;
  else return *(ip + (paranum - 1) * 2) ;
}

/******************************************************************************

  setup_tfs

  Attempt at improving speed for do_passage()

  int array of numstreams * numpars is set up and filled with tfs

  NB Can use 32 meg or more of memory

******************************************************************************/

#ifdef FASTER

int *
setup_tfs(int fdnum, int numstreams, int streams[], bss_Gp *pstgs[])
{
  int *tfs, *tfptr ;
  int tf ;
#if HILO
  Pos_text_hl *pos ; 
#else
  Pos_text_lh *pos ; 
#endif
  int *ip = firstpara ;
  int f_sent ;
  int j, k ;
  static char *func = "setup_tfs" ;

  tfs = (int *)calloc(numstreams * f_numpars, sizeof(int)) ;
  if ( tfs == NULL ) {
    mem_err(func, "tfs", numstreams * f_numpars * sizeof(int)) ;
    exit(1) ;			/* NO */
  }
  fdnum-- ;
  for ( j = 0 ; j < numstreams ; j++ ) {
    pos = pstgs[streams[j]]->p ;
    while ( *(u_int *)pos != 0xffffffff && pos->f < fdnum ) pos++ ;
    for ( k = 0 ; k < f_numpars ; k++ ) {
      tf = 0 ;
      f_sent = *(ip + k * 2 + 1) ;
      while ( *(u_int *)pos != 0xffffffff && pos->f == fdnum &&
	     pos->s <= f_sent) {
	tf++ ;
	pos++ ;
      }
      *(tfs + j * f_numpars + k) = tf ;
      if ( *(u_int *)pos == 0xffffffff || pos->f > fdnum ) break ;
    }
  }

  return tfs ;
}

#endif

/******************************************************************************

  setup_tot_tfs

  Attempt at improving speed for do_passage()

  int array of size numpars is set up and filled with total tf for each
  paragraph (all streams)

******************************************************************************/

static int *tot_tfs ;
static int num_tot_tfs ;

int *
setup_tot_tfs(int fdnum, int numstreams, int streams[], bss_Gp *pstgs[])
{
  U_pos *pos ; 
  int *ip = firstpara ;
  int f_sent ;
  int j, k ;

  if ( f_numpars > num_tot_tfs ) {
    if ( tot_tfs != NULL ) bss_free(tot_tfs) ;
    tot_tfs = (int *)bss_malloc(f_numpars * sizeof(int)) ;
    num_tot_tfs = f_numpars ;
    if ( Dbg & (D_PASSAGES|DD_PASSAGES) )
      fprintf(bss_syslog, "Reallocating tot_tfs buffer to %d\n",
	      f_numpars) ;
  }
  if ( tot_tfs == NULL ) {
    num_tot_tfs = 0 ;
    return NULL ;
  }
  memset((char *)tot_tfs, 0, f_numpars * sizeof(int)) ;
  fdnum-- ;
  for ( j = 0 ; j < numstreams ; j++ ) {
    /* should be pstgs[streams[j]] you stupid oaf */
    /* pos = pstgs[j]->p ; */
    pos = pstgs[streams[j]]->p ;
#if HILO
    while ( *(u_int *)pos != 0xffffffff && pos->pthl.f < fdnum ) pos++ ;
    for ( k = 0 ; k < f_numpars ; k++ ) {
      f_sent = *(ip + k * 2 + 1) ;
      while ( *(u_int *)pos != 0xffffffff && pos->pthl.f == fdnum &&
	     pos->pthl.s <= f_sent) {
	tot_tfs[k]++ ;
	pos++ ;
      }
      if ( *(u_int *)pos == 0xffffffff || pos->pthl.f > fdnum ) break ;
    }
#else
    while ( *(u_int *)pos != 0xffffffff && pos->ptlh.f < fdnum ) pos++ ;
    for ( k = 0 ; k < f_numpars ; k++ ) {
      f_sent = *(ip + k * 2 + 1) ;
      while ( *(u_int *)pos != 0xffffffff && pos->ptlh.f == fdnum &&
	     pos->ptlh.s <= f_sent) {
	tot_tfs[k]++ ;
	pos++ ;
      }
      if ( *(u_int *)pos == 0xffffffff || pos->ptlh.f > fdnum ) break ;
    }
#endif
  }
  return tot_tfs ;
}

/******************************************************************************

  find_passage_tf

  Counts number of pos recs within the passage. Obviously they must all
  belong to the same "term". Pos recs terminated by 0xffffffff.

  Must have done para_get_field() to find ip

******************************************************************************/

int
find_passage_tf(int fdnum, int spara, int fpara, U_pos *pos)
{
  int *ip = firstpara ;
  int s_sent, f_sent ;		/* 1st and last sentences in passage */
  int tf = 0 ;

  if ( spara == 0 ) s_sent = 0 ;
  else s_sent = *(ip + (spara - 1) * 2 + 1) + 1 ;
  f_sent = *(ip + fpara * 2 + 1) ;

  fdnum-- ;			/* Because 1- in most places but 0.. in pos */
#if HILO
  while ( *(u_int *)pos != 0xffffffff && pos->pthl.f < fdnum ) pos++ ;

  while ( *(u_int *)pos != 0xffffffff &&
	 pos->pthl.f == fdnum && pos->pthl.s <= f_sent ) {
    if ( pos->pthl.s >= s_sent ) tf++ ;
    pos++ ;
  }
#else
  while ( *(u_int *)pos != 0xffffffff && pos->ptlh.f < fdnum ) pos++ ;

  while ( *(u_int *)pos != 0xffffffff &&
	 pos->ptlh.f == fdnum && pos->ptlh.s <= f_sent ) {
    if ( pos->ptlh.s >= s_sent ) tf++ ;
    pos++ ;
  }
#endif
  return tf ;
}

  
/******************************************************************************

  find_passage_wt_component

  returns the (double) weight component due to a single stream from a
  single passage, ops bm11/15/1 only

******************************************************************************/

double
find_passage_wt_component(int fdnum, int spara, int fpara, int pl,
			  U_pos *pos,
			  double streamweight, double k1, double bm25_b,
			  double avedl)
{
  double w = 0.0 ;
  int tf ;
  double bm25_K, tfp ;
  tf = find_passage_tf(fdnum, spara, fpara, pos) ;
  if ( Dbg & DD_PASSAGES )
    fprintf(bss_syslog, "%d,%d: pl=%d tf=%d", spara, fpara, pl, tf) ;
  if ( tf > 0 )  {
    bm25_K = k1 * (1.0 - bm25_b + bm25_b * (double)pl / avedl);
    tfp = (double)tf ;
    w += streamweight * tfp / (tfp + bm25_K ) ;
  }
  if ( Dbg & DD_PASSAGES )
    fprintf(bss_syslog, ", w=%.1f\n", w) ;

  return w ;
}

/******************************************************************************

  do_passage

******************************************************************************/

double
do_passage(int fdnum, int spara, int fpara, int pl, int numstreams,
	   int streams[], bss_Gp *pstgs[], double streamweights[],
	   double k1, double bm25_b, double avdl)
{
  double w = 0.0 ;		/* Weight */
  int j, stream ;
  for ( j = 0 ; j < numstreams ; j++ ) {
    stream = streams[j]  ;
    w += find_passage_wt_component(fdnum, spara, fpara, pl,
				  pstgs[stream]->p, streamweights[stream],
				  k1, bm25_b, avdl) ;
  }
  return w ;
}

/******************************************************************************

  faster_do_passage()

  Using array of tfs[stream][par]

  Feb 98: note weighting now out of date, no b

******************************************************************************/
#ifdef FASTER

double
faster_do_passage(*tfs, int spara, int fpara, int pl, int numstreams,
		  int streams[], double streamweights[],
		  double k1, double avedl)
{
  double w = 0.0 ;
  int stream, j, k ;
  int tf ;
  int *tfp ;

  for ( j = 0 ; j < numstreams ; j++ ) {
    tf = 0 ;
    stream = streams[j] ;
    tfp = tfs + j * f_numpars + spara ;
    for ( k = spara ; k <= fpara ; k++ ) tf += (int)(*tfp++) ;
    if ( tf > 0 ) 
      w += streamweights[stream] *
	(double)tf / ((double)tf + k1 * (double)pl / avedl) ;
  }
  return w ;
}

#endif

/******************************************************************************

  do_passages

******************************************************************************/
/* For some reason FASTER is slower by a factor of about 3 */
Para_rec *			/* Temporary, retains best only */
do_passages(int rec, int field, int unit, int max, int step,
	    int nk, int numstreams, int streams[], bss_Gp *pstgs[],
	    double wts[], double k1, double k2, double bm25_b,
	    double avedl, int maxiters)
{
  static Para_rec p_rec ;
  
  static int calls = 0 ;
  static int do_passage_calls = 0 ;
  static int skipped_starts = 0 ;
  static int skipped_ends = 0 ;
  static int aborted = 0 ;

  int pl ;			/* passage length */
  double wt = 0.0, tempwt ;
  int spara, fpara ;
  int iters = 0 ;
  int *tot_tfs ;
#ifdef FASTER
  int *tfs ;
#endif

  if ( Dbg & D_PASSAGES ) {
    if ( rec == (int)0xffffffff ) {
      fprintf(bss_syslog,
 "do_passages(): %d calls %d do_passage() %d skipped starts %d skipped ends %d aborted\n",
	      calls, do_passage_calls, skipped_starts, skipped_ends, aborted) ;
      return NULL ;
    }
    else if ( rec == (int)0xfffffffe ) {
      calls = do_passage_calls = skipped_starts = skipped_ends = aborted = 0 ;
      return NULL ;
    }
  }
  /* Arg error check. Should be done at much higher level, but still.. */
  if ( unit < 1 || step < 1 || step > unit || max < unit ) return NULL ;
  calls++ ;
  get_para_rec(rec) ;
  para_get_field(field) ;
  memset((char *)&p_rec, 0, sizeof(Para_rec)) ; /* Just for luck */
  if ( f_numpars <= unit ) return &p_rec ; 
#ifdef FASTER
  tfs = setup_tfs(field, numstreams, streams, pstgs) ;
#endif
  tot_tfs = setup_tot_tfs(field, numstreams, streams, pstgs) ; 
  if ( Dbg & DD_PASSAGES )
    fprintf(bss_syslog, "numpars=%d unit=%d step=%d max=%d\n", f_numpars,
	  unit, step, max) ;
  for ( spara = 0 ; spara < f_numpars ; spara += step ) {
    while ( spara < f_numpars && tot_tfs[spara] == 0 ) {
      skipped_starts++ ;
      if ( Dbg & DD_PASSAGES )
	fprintf(bss_syslog, "tot_tfs[%d] = 0, skipping spara\n", spara) ;
      spara++ ;
    }
    if ( spara >= f_numpars ) break ;
    for ( fpara = (spara >= f_numpars - unit) ?
	 f_numpars - 1 : spara + unit - 1 ;
	 fpara < f_numpars && fpara < spara + max  ;
	 fpara = (fpara < f_numpars - 1 && fpara >= f_numpars - unit) ?
	 f_numpars - 1 : fpara + unit ) {
      while ( fpara < f_numpars - 1 && tot_tfs[fpara] == 0 ) {
	skipped_ends++ ;
	if ( Dbg & DD_PASSAGES )
	  fprintf(bss_syslog, "tot_tfs[%d] = 0, skipping fpara\n", fpara) ;
	fpara++ ;
      }
      if ( fpara >= f_numpars ) break ;
      if ( Dbg & DD_PASSAGES ) 
	fprintf(bss_syslog, "Doing (%d,%d)\n", spara, fpara) ;
      pl = find_passage_len(spara, fpara) ;
#ifdef FASTER
      tempwt = faster_do_passage(tfs, spara, fpara, pl, numstreams, streams,
				 wts, k1, avedl) ;
#else
      tempwt = do_passage(field, spara, fpara, pl, numstreams, streams,
			  pstgs, wts, k1, bm25_b, avedl) ;
#endif
      do_passage_calls++ ;
      iters++ ;
      if ( iters > maxiters ) {
	aborted++ ;
	return &p_rec ;
      }
      tempwt += (double)nk * k2 * (avedl - (double)pl) / (avedl + (double)pl) ;
      if ( Dbg & DD_PASSAGES )
	fprintf(bss_syslog,
		"w(%d, %d, %d) = %4.1f\n", rec, spara, fpara, tempwt) ;
      if ( tempwt > wt ) {
	wt = tempwt ;
	p_rec.wt = wt ;
	p_rec.sp = spara ;
	p_rec.fp = fpara ;
      }
    }
  }
  p_rec.fd = field ;
  if ( Dbg & DD_PASSAGES )
    fprintf(bss_syslog, "pass: %.3f %d %d\n", p_rec.wt, p_rec.sp, p_rec.fp) ;
#ifdef FASTER
  bss_free(tfs) ;
#endif
  return &p_rec ;
}
