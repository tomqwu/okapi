#include "bss.h"
#include "bss_errors.h"

#define DELTA_WT .0001

#define CHARS_TO_TOKS (double)10.5

#undef OK
#define OK 1
#undef EMPTY
#define EMPTY 0
#undef FINISHED
#define FINISHED -1

#if HILO
#define p6(x) ((Longpos_hl *)(posptr[(x)]))
#define pt(x) ((Pos_text_hl *)(posptr[(x)]))
#else
#define p6(x) ((Longpos_lh *)(posptr[(x)]))
#define pt(x) ((Pos_text_lh *)(posptr[(x)]))
#endif

/******************************************************************************
  
Following macro used by bss_combine_bm() and bss_combine_pass()

******************************************************************************/


#define DO_MPW(s) \
  if ( k2 != 0 ) has_k2 = TRUE ; \
  /* Next really only if has_k2 */\
  if ( (op == OP_BM40 || op == OP_BM41) ) {\
    s->mpw = s->realmpw = MAXWEIGHT ;\
    s->minpw = MINWEIGHT ;\
  }\
  for ( j = 0 ; j < num ; j++ ) { \
    if ( (srecs[j]->pstgtype & HASWT) ) { \
      hasweights[j] = TRUE ; \
      if ( srecs[j]->maxweight > 0 ) { \
	s->mpw += srecs[j]->maxweight ; \
	s->realmpw += srecs[j]->maxweight ; \
      } \
      if ( srecs[j]->minweight < 0 ) { \
	s->minpw += srecs[j]->minweight ; \
      } \
    } \
    else {		  /* Input stream doesn't have posting weights */\
      hasweights[j] = FALSE ; \
      if ( op != OP_BM40 && op != OP_BM41 && op != OP_BM30 && op != OP_BM3000 ) {\
        streamweights[j] = weights[j] * (1.0 + k1) ; \
        if ( srecs[j]->naw > 0 ) { \
          numfresh++ ; \
          if ( streamweights[j] > 0 ) s->mpw += streamweights[j] ; \
          else s->minpw += streamweights[j] ;\
        } \
        if ( streamweights[j] > 0 ) s->realmpw += streamweights[j] ; \
        if ( has_k2 ) { \
	  s->mpw += fabs(k2) ; \
	  s->realmpw += fabs(k2) ; \
        } \
      } /* != bm30 etc */ \
      else if ( op == OP_BM40 || op == OP_BM41 ) { \
        streamweights[j] = weights[j] ; \
        if ( srecs[j]->naw > 0 ) { \
          numfresh++ ; \
        } \
        s->realmpw = MAXWEIGHT ;\
        s->minpw = MINWEIGHT ;\
      } \
      else /* BM30 etc */ {\
        streamweights[j] = weights[j] ; \
        if ( srecs[j]->naw > 0 ) { \
          numfresh++ ; \
          if ( streamweights[j] > 0 ) s->mpw += streamweights[j] ; \
          else s->minpw += streamweights[j] ;\
        } \
        if ( streamweights[j] > 0 ) s->realmpw += streamweights[j] ; \
      } \
    } /* (no posting weights) */ \
  } /* (input streams loop) */ \
  \
  if ( op == OP_BM30 || op == OP_BM3000 ) { \
    s->mpw *= log((1+alpha)/alpha)/log((1+alpha)/(exp(-1)+alpha)) ;\
    s->realmpw *= log((1+alpha)/alpha)/log((1+alpha)/(exp(-1)+alpha)) ;\
    s->minpw *= log((1+alpha)/alpha)/log((1+alpha)/(exp(-1)+alpha)) ;\
  }

/* Linear only */
/* Strictly should be min, min+range/grad, min+2*range/grad, ..., min+range
   but anything negative goes into bottom box. */

#define SETUP_WEIGHT_DIST(s) \
{ \
  int j ; \
  if ( s->weight_dist_grad > 0 && s->mpw > 0 ) {\
    s->wt_bucket_mult = (float)s->weight_dist_grad/(float)s->mpw ;\
    for ( j = 0 ; j <= outsrec->weight_dist_grad ; j++ ) {\
      if ( s->wt_bucket_mult == 0 ) s->weight_floors[j] = MINWEIGHT ;\
      else s->weight_floors[j] = ((float)j / s->wt_bucket_mult) ; \
    }\
  }\
}

#define DO_WEIGHTS(w,s) \
    if ( w >= s->gw ) { \
      s->ngw++ ; \
      if ( w == s->mpw ) s->nmpw++ ; \
    } \
    if ( w > s->maxweight ) { \
      s->maxweight = w ; \
      s->nmaxweight = 1 ; \
    } \
    else if ( w == s->maxweight ) s->nmaxweight++ ; \
    if ( w < s->minweight ) s->minweight = w ;\
    if ( s->weight_dist_grad > 0 ) {\
      if ( w < 0 ) s->weight_dist[0]++ ; \
      else { \
        int wd = (float)w * s->wt_bucket_mult ;\
        s->weight_dist[wd]++ ; \
        if ( Dbg & D_TARGET ) { \
          if ( s->weight_floors[wd] > (float)w ) \
           fprintf(bss_syslog, "Weight %f, but box %d with floor %f\n",\
                   (float)w, wd, s->weight_floors[wd]) ;\
        }\
      }\
    }

#define CHECK_CUTOFF \
    /* Target stuff, lost when the selection tree was introduced, taken from\
       bss_combine_bm() July 1999 */\
    if ( target > 0 ) {\
      /* Periodically update cutoff_score */\
      /* March 96: can do better here: keep track of next breakpoint,\
	 and increase cutoff when have got this many more (or something\
	 like that) */\
      /* May 02: target must be zero for BM2 etc */\
      if ( outsrec->naw >= target && outsrec->naw % 1000 == 0 ) {\
	/* 1000 arbitrary */\
	for ( lnum = 0, j = outsrec->weight_dist_grad ;\
	      j >= 0 && lnum < target ; j-- )\
	  lnum += outsrec->weight_dist[j] ;\
	if ( lnum >= target && j > 0 ) {\
	  if ( Dbg & D_TARGET ) old_cutoff = cutoff_score ;\
	  cutoff_score = outsrec->weight_floors[j] ;\
	  if ( Dbg & D_TARGET ) \
	    if ( cutoff_score > old_cutoff ) \
	      fprintf(bss_syslog,\
      "Increasing cutoff from %f to %f (box=%d, considered %d) at naw=%d\n",\
		      old_cutoff, cutoff_score, j,\
		      docs_considered, outsrec->naw) ;\
	}\
      }\
    } /* (target > 0) */

#define MERGE_ALL_POS \
      outposptr = (unsigned *)outpstg->p ;\
      outposcount = 0 ;\
\
      for ( k = 0 ; k < numcontribs ; k++ ) {\
	int numtodo ;\
	BOOL posovf = FALSE ;\
	j = streams[k] ;\
	numtodo = pstgs[j]->numpos ;\
	if ( outposcount + numtodo > posmax ) {\
	  numtodo = posmax - outposcount ;\
	  posovf = TRUE ;\
	  if ( Dbg & D_COMBINE )\
	    fprintf(bss_syslog, \
		    "already %d pos recs from IRN %d, truncating to %d\n",\
		    outposcount, pstgs[j]->rec, posmax) ;\
	}\
	memcpy((char *)outposptr, (char *)pstgs[j]->p,\
	       numtodo * sizeof(U_pos));\
	outposptr += numtodo ;\
	outposcount += numtodo ;\
	if ( posovf ) break ;\
      }\
      if ( numcontribs > 1 ) {	/* (Sep 96) */\
	/* gqsort((char *)outpstg->p, outposcount, sizeof(unsigned),\
	   (int (*)(void *, void *))poscmp) ; */\
	gqsort_int((u_int *)outpstg->p, outposcount, INT_SORT_THRESH) ;\
	sortcount++ ;\
      }\
      outpstg->numpos = outposcount ;


#define DO_STATS(w) if ( find_flags & FFL_DO_STATS ) { \
      outsrec->weightsum += w ; \
      outsrec->weightsumsq += w * w ; \
}

#define report_comb \
  if ( Dbg & D_COMBINE ) \
    fprintf(bss_syslog, \
	    "Total contributing postings %d, mean %.2f, sort calls %d\n", \
	    totcontribs, (double)totcontribs/(double)outsrec->naw, sortcount) ;

/******************************************************************************

  poscmp()

  Compare for sorting positional fields

  (May 00) Realised *p -*q rubbish
  (but doubt if using this func any more)

******************************************************************************/

static int
poscmp(u_int *p, u_int *q)
{
  if ( *p > *q ) return 1 ;
  if ( *p < *q ) return -1 ;
  return 0 ;
  /* return *p - *q ; Ha Ha */
}

/*****************************************************************************

  bss_combine.c  SW Apr 92

  Routines for constructing merge records & controlling lookup.

 *****************************************************************************/

/******************************************************************************

  bss_combine_0  SW orig May 92

  Does ordinary boolean and quasi-boolean ops.
  Returns set number or - on error.

  Nov 94: major revision: bss_do_combine() now does all the validation,
  file opening, malloc'ing etc, and tidies up afterwards. Different and
  more args are passed from bss_do_combine().

******************************************************************************/

int maxopc = 0 ;

#define WTGRAN 20

int
bss_combine_0(int num, int op, u_int op_flags, int np[], int postype,
	      Setrec *outsrec, Setrec *srecs[], int sameas[],
	      int maxgap )
{
  register int j, k ;		/* Loop counters */
  int status[MAXMERGE] ;	/* Each stream is in one of 3 states
				   OK, EMPTY or FINISHED */
  bss_Gp *pstgs[MAXMERGE] ;
  bss_Gp *outpstg = outsrec->pstg ;
  int outpstgtype = outsrec->pstgtype ;
  /* int maxgap = 0 ;		Trial: adj wd accept +1 - +1 + maxgap */
  /* maxgap replaced by arg (see NEAR ops) */
  u_int *outposptr ;
  int outposcount ;
  int this_tf ;			/* tf for a single output posting */
  u_int *posptr[MAXMERGE] ;
  u_int *opp[MAXMERGE] ;	/* Needed for ops which use pos fds: have to
				   rewrite input pos fds */
  int poscount[MAXMERGE] ;
  int oposcount[MAXMERGE] ;
  int posmax = (outpstgtype & TEXTPOS) ? MAXTEXTPOS : MAXLONGPOS ;
  int streams[MAXMERGE] ;
  int numcontribs ;		/* No of contributors to an output posting */

  int  minval = 0 ;		/* Value (i.e. recnum field) of
				   current posting */
  register int jmin ; 	        /* First stream in which current posting 
				   appears */
  int totcontribs = 0, sortcount = 0 ;
  static char *func = "bss_combine_0" ;
  printf("bss_combine_0 is called by Okapi\n");   /*added by James Miao to check which combine function is called 2009.8.25*/
  for ( j = 0 ; j < num ; j++ ) {
    if ( np[j] == 0 ) status[j] = FINISHED ;
    else status[j] = EMPTY ;
  }
  minval = 0 ;
  while ( minval < INT_MAX ) { 
    /* Start of a pass */
    /* Read in any which are empty, then find first stream (jmin) with
       smallest posting */
    /* Should be prepared to use selection tree here, would speed it
       for large enough value of num */
    for ( j = 0, minval = INT_MAX ; j < num ; j++ ) {
      if ( status[j] == FINISHED ) {
	if ( op_flags & ANDLIKE ) {
	  minval = INT_MAX ;
	  break ;
	}
	else continue ;
      }
      if ( status[j] == EMPTY ) {
	/* Prob don't need the np condition */
	if ( (np[j]-- >0 &&  sameas[j] >= 0 && (pstgs[j] = pstgs[sameas[j]]))
	     || (pstgs[j] = get_next_pstg(srecs[j])) ) status[j] = OK ;
	else {
	  status[j] = FINISHED ;
	  if ( op_flags & ANDLIKE ) {
	    minval = INT_MAX ;
	    break ;
	  }
	  else continue ;
	}
      }
      /* (status OK) */
      /* Added streams[] and numcontribs as combine_bm() Jan 96 */
      if ( pstgs[j]->rec < minval ) {
	jmin = j ; minval = pstgs[j]->rec ; 
	numcontribs = 0 ;
	streams[numcontribs++] = jmin ;
      }
      else if ( pstgs[j]->rec == minval ) streams[numcontribs++] = j ;
    }

    if ( minval == INT_MAX ) break ; /* All streams, or one if and_like,
				       finished */

   /* Mark streams with recnum=minval EMPTY, etc */

    for ( k = 0, this_tf = 0 ; k < numcontribs ; k++ ) {
      j = streams[k] ;
      /* (Prob quicker to do next whether or not accept the posting */
      if ( outpstgtype & HASCONTRIBS ) {
	outpstg->t[numcontribs - 1].str = j ;
	outpstg->t[numcontribs - 1].tf = (u_short) pstgs[j]->numpos ;
      }
      status[j] = EMPTY ;
      this_tf += pstgs[j]->numpos ;
    }				/* (contributing streams) */

    /* Now can decide if we accept this posting. The criteria depend
       on whether and_like, not_like, adj_like.
       If or_like accept anything */

    if ( (op_flags & ANDLIKE) && numcontribs != num ) continue ;
    else if ( op == OP_NOT1 && ( jmin != 0 || numcontribs > 1) )
      continue ;
    else if ( op_flags & ADJLIKE ) {

      /* Now what we do depends on the op. The next bit deals with subops of
	 AND. Have to do things with positional fields. In these cases all
	 the postings are the same (at least the address fields are).
	 For each stream after the first, go through its pos fields until
	 (under the appropriate condition for the op) either there aren't any
	 more or one is found which is >= that of the previous stream. If a
	 stream is exhausted finish, (go back to top of outer while loop).
	 If the field is greater, this sequence is no good, so it starts again
	 from the first stream's next pos field (if any). Otherwise, if
	 comparisons return equal right through we have a hit, and the
	 sequence of pos fields is output.

	 Note that, in the case of op=ADJ only, any string of
	 successful pos fields is concatenated to form a single one.

      */

      BOOL got_all = FALSE ;
      int got_one = 0 ;
      BOOL cond ;
      int hi, lo, highest, lowest ; /* For NEAR op */
      /* Get 1st set of pos fields */
      for ( j = 0 ; j < num ; j++ ) {
	posptr[j] = opp[j] = (unsigned *)pstgs[j]->p ;
	poscount[j] = pstgs[j]->numpos ;
	oposcount[j] = 0 ;
      }

      if ( op == OP_NEAR1 ) {
	if ( postype == LONGPOS ) {
	  hi = p6(0)->s + maxgap ;
	  lo = p6(0)->s - maxgap ;
	  highest = lowest = p6(0)->s ;
	}
	else {
	  hi = pt(0)->s + maxgap ;
	  lo = pt(0)->s - maxgap ;
	  highest = lowest = pt(0)->s ;
	}
	if ( lo < 0 ) lo = 0 ;
      }
      for ( j = 1 ; j < num ; j++ ) {
	switch (op) {
	case OP_ADJ1:
	  while ( poscount[j] > 0 &&
		  (postype == LONGPOS &&
		   (
		    (*posptr[j] & 0xffff0000) < (*posptr[j-1] & 0xffff0000) ||
		    ((*posptr[j] & 0xffff0000)==(*posptr[j-1] & 0xffff0000) &&
		     p6(j)->t - p6(j)->sw < p6(j-1)->t + p6(j-1)->nt)
		    ) ||
		   (postype == TEXTPOS &&
		    (
		     (*posptr[j] & 0xffff8000) < (*posptr[j-1] & 0xffff8000) ||
		     ((*posptr[j] & 0xffff8000)==(*posptr[j-1] & 0xffff8000) &&
		      pt(j)->t - pt(j)->sw < pt(j-1)->t + pt(j-1)->nt)
		     )
		    )
		   )
		  )
	    if ( --poscount[j] > 0 ) posptr[j]++ ;
	  break ;
	case OP_ADJ2:
	  while ( poscount[j] > 0 &&
		 (postype == LONGPOS &&
		  ((*posptr[j] & 0xffff0000) < (*posptr[j-1] & 0xffff0000) ||
		   ((*posptr[j] & 0xffff0000)==(*posptr[j-1] & 0xffff0000) &&
		   p6(j)->t < p6(j-1)->t + p6(j-1)->nt)) ||
		  postype == TEXTPOS &&
		  ((*posptr[j] & 0xffff8000) < (*posptr[j-1] & 0xffff8000) ||
		   (*posptr[j] & 0xffff8000)==(*posptr[j-1] & 0xffff8000) &&
		   pt(j)->t < pt(j-1)->t + pt(j-1)->nt)))
	    if ( --poscount[j] > 0 ) posptr[j]++ ;
	  break ;
	case OP_SAMES1:
	  while ( poscount[j] > 0 ) {
	    switch (postype) {
	    case TEXTPOS:
	      cond =
		(((*posptr[j]) & 0xffff8000)<((*posptr[j - 1]) & 0xffff8000)) ;
	      break ;
	    case LONGPOS:
	      cond =
		(((*posptr[j]) & 0xffff0000)<((*posptr[j - 1]) & 0xffff0000)) ;
	      break ;
	    } /* (switch) */
	    if ( cond && --poscount[j] > 0 ) posptr[j]++ ;
	    else break ;
	  } /* (while) */
	  break ; /* (outer case) */
	case OP_NEAR1:
	  while ( poscount[j] > 0 ) {
	    switch (postype) {
	    case TEXTPOS:
	      cond = ( pt(j)->f < pt(j-1)->f || pt(j)->s < lo ) ;
	      break ;
	    case LONGPOS:
	      cond = ( p6(j)->f < p6(j-1)->f || p6(j)->s < lo ) ;
	      break ;
	    } /* (switch) */
	    if ( cond && --poscount[j] > 0 ) posptr[j]++ ;
	    else break ;
	  } /* (while) */
	  break ; /* (outer case) */
	case OP_SAMEF1:
	  while ( poscount[j] > 0 &&
		 (
		  postype == LONGPOS && p6(j)->f < p6(j-1)->f ||
		  postype == TEXTPOS && pt(j)->f < pt(j-1)->f 
		  )
		 ) 
	    if ( --poscount[j] > 0 ) posptr[j]++ ;
	  break ;
	  
	}			/* switch (op) */
	/* Now either it's not less or there aren't any more:
	   still need to check the latter condition */
	if ( poscount[j] <= 0 ) {
	  break ;
	}
	/* If its now too big go back to j = 0 unless none left */
	switch (op) {
	case OP_ADJ1:
	  switch (postype) {
	  case TEXTPOS:
	    cond = (             
              ((*posptr[j]) & 0xffff8000) > ((*posptr[j-1]) & 0xffff8000) ||
              ((*posptr[j]) & 0xffff8000) == ((*posptr[j-1]) & 0xffff8000) &&
              pt(j)->t - pt(j)->sw > pt(j-1)->t + pt(j-1)->nt + maxgap
		    ) ;
	    break ;
	  case LONGPOS:
	    cond = (
              ((*posptr[j]) & 0xffff0000) > ((*posptr[j-1]) & 0xffff0000) ||
              ((*posptr[j]) & 0xffff0000) == ((*posptr[j-1]) & 0xffff0000) &&
              p6(j)->t - p6(j)->sw > p6(j-1)->t + p6(j-1)->nt + maxgap
		    ) ;
	    break ;
	  }
	  break ;
	case OP_ADJ2:
	  switch (postype) {
	  case TEXTPOS:
	    cond = (
              ((*posptr[j]) & 0xffff8000) > ((*posptr[j-1]) & 0xffff8000) ||
              ((*posptr[j]) & 0xffff8000) == ((*posptr[j-1]) & 0xffff8000) &&
             pt(j)->t > pt(j-1)->t + pt(j-1)->nt + maxgap
		    ) ;
	    break ;
	  case LONGPOS:
	    cond = (
              ((*posptr[j]) & 0xffff0000) > ((*posptr[j-1]) & 0xffff0000) ||
              ((*posptr[j]) & 0xffff0000) == ((*posptr[j-1]) & 0xffff0000) &&
              p6(j)->t > p6(j-1)->t + p6(j-1)->nt + maxgap
		    ) ;
	    break ;
	  }
	  break ;
	case OP_SAMES1:
	  switch (postype) {
	  case TEXTPOS:
	    cond =
	      (((*posptr[j]) & 0xffff8000)>((*posptr[j-1]) & 0xffff8000)) ;
	    break ;
	  case LONGPOS:
	    cond = (
		    ((*posptr[j]) & 0xffff0000)>((*posptr[j-1]) & 0xffff0000)
		    ) ;
	    break ;
	  }
	  break ;
	case OP_NEAR1:
	  switch (postype) {
	  case TEXTPOS:
	    cond = (( pt(j)->f > pt(j-1)->f ) || ( pt(j)->s > hi )) ;
	    if ( ! cond ) {
	      if ( pt(j)->s < lowest ) {
		lowest = pt(j)->s ;
		hi = lowest + maxgap ;
	      }
	      else if ( pt(j)->s > highest ) {
		highest = pt(j)->s ;
		lo = highest - maxgap ;
		if ( lo < 0 ) lo = 0 ;
	      }
	    }
	    break ;
	  case LONGPOS:
	    cond = (( p6(j)->f > p6(j-1)->f ) || ( p6(j)->s > hi )) ;
	    if ( ! cond ) {
	      if ( p6(j)->s < lowest ) {
		lowest = p6(j)->s ;
		hi = lowest + maxgap ;
	      }
	      else if ( p6(j)->s > highest ) {
		highest = p6(j)->s ;
		lo = highest - maxgap ;
		if ( lo < 0 ) lo = 0 ;
	      }
	    }
	    break ;
	  }
	  /*if ( ! cond ) fprintf(stderr, "hi=%d, lo=%d\n",hi, lo) ; */
	  break ;
	case OP_SAMEF1:
	  switch (postype) {
	  case TEXTPOS:
	    cond = ( pt(j)->f > pt(j-1)->f ) ;
	    break ;
	  case LONGPOS:
	    cond = ( p6(j)->f > p6(j-1)->f ) ;
	    break ;
	  }
	  break ;
	}
      
	if ( cond ) {
	  /* Start another pass */
	  j = 0 ;
	  poscount[0]-- ;
	  if ( poscount[0] <= 0 ) {
	    break ;
	  }
	  posptr[0]++ ;
	  if ( op == OP_NEAR1 ) {
	    if ( postype == LONGPOS ) {
	      hi = p6(0)->s + maxgap ;
	      lo = p6(0)->s - maxgap ;
	      if ( lo < 0 ) lo = 0 ;
	      highest = lowest = p6(0)->s ;
	    }
	    else {
	      hi = pt(0)->s + maxgap ;
	      lo = pt(0)->s - maxgap ;
	      if ( lo < 0 ) lo = 0 ;
	      highest = lowest = pt(0)->s ;
	    }
	  }
	  continue ;		/* With j = 1 */
 	} /* (if ( cond ) ) */
	/* Then succeeded up to this j */
	if ( j == num - 1 ) {
	  /* Now have a hit - but it may not be the only one */
	  got_one++ ;
	  /* Output & advance them all, then if any finished exit the loop */
	  if ( op != OP_ADJ1 && op != OP_ADJ2 ) {
	    for ( k = 0 ; k < num ; k++ ) {
	      *opp[k]++ = *posptr[k]++ ;
	      oposcount[k]++ ;	
	      if ( --poscount[k] <= 0 ) got_all = TRUE ; 
	    }
	  }
	  else {		/* adj && LONGPOS or TEXTPOS- a single field */
	    if ( postype == LONGPOS ) {
	      int nt = p6(0)->nt ;
	      *opp[0] = *posptr[0] ;
	      for ( k = 1 ; k < num ; k++ ) 
		nt += p6(k)->nt + p6(k)->sw ;
#if HILO
	      ((Pos6_hl *)opp[0]++)->nt = nt ;
#else
	      ((Pos6_lh *)opp[0]++)->nt = nt ;
#endif
	      oposcount[0]++ ;	
	    }
	    else {
	      int nt = pt(0)->nt ;
	      *opp[0] = *posptr[0] ;
	      for ( k = 1 ; k < num ; k++ ) 
		nt += pt(k)->nt + pt(k)->sw ;
#if HILO
	      ((Pos_text_hl *)opp[0]++)->nt = nt ;
#else
	      ((Pos_text_lh *)opp[0]++)->nt = nt ;
#endif
	      oposcount[0]++ ;	
	    }
	    for ( k = 0 ; k < num ; k++ ) {
	      if ( --poscount[k] <= 0 ) got_all = TRUE ; 
	      else posptr[k]++ ;
	    }
	  }
	  if ( got_all ) break ; 
	  j = 0 ;
	  /* Reinitialize NEAR1 */
	  /* Note this block identical to just before start of for loop */
	  if ( op == OP_NEAR1 ) {
	    if ( postype == LONGPOS ) {
	      hi = p6(0)->s + maxgap ;
	      lo = p6(0)->s - maxgap ;
	      highest = lowest = p6(0)->s ;
	    }
	    else {
	      hi = pt(0)->s + maxgap ;
	      lo = pt(0)->s - maxgap ;
	      highest = lowest = pt(0)->s ;
	    }
	    if ( lo < 0 ) lo = 0 ;
	  }
	  continue ;		/* And look for another one */
	} /* if ( j == num - 1 ) -- i.e. got one */
      } /* (for loop) */
      if ( got_one == 0 ) continue ; /* Look for another posting (Back to the
					while (minval..)) */
      else 			/* Success, overwrite the numpos fields */
	for ( j = 0 ; j < num ; j++ ) 
	  pstgs[j]->numpos = oposcount[j] ;
    }			/* adjlike: op = adj, sames, samef */
    /* End of ADJ */

    if ( outpstgtype & HASPOS ) {
      /* Merge positional fields if nec */
      /* This should work for any kind of pos field */
      MERGE_ALL_POS ;
    }	/* HASPOS  */
    else if ( outpstgtype & HASTF ) {
      if ( op_flags & ADJLIKE ) {
	outposcount = 0 ;
	for ( k = 0 ; k < numcontribs ; k++ ) {
	  j = streams[k] ;
	  if ( outposcount + pstgs[j]->numpos > posmax ) break ;
	  outposcount += pstgs[j]->numpos ;
	}
	outpstg->numpos = outposcount ;
      }
      else outpstg->numpos = (this_tf <= posmax) ? this_tf : posmax ;
    }
    if ( outpstgtype & (HASCONTRIBS|HASCOORD) ) outpstg->coord = numcontribs ;
    outpstg->rec = minval ;
    write_next_pstg(outsrec) ;
    totcontribs += numcontribs ;
  }
  report_comb ;
  return outsrec->num ;
}

 

/******************************************************************************

  Weighted combine with selection tree. Orig ? 1994 SW

  June 97: frig factor now an array and applies to any stream except
  perhaps the first (also in bss_combine_pass())

  Sep 96: removed effect of sameas[]. This function won't work properly if
  a set is repeated in the input. There's no very obvious way to cure this.
  (If any element of sameas[] is nonzero this function shouldn't be called --
  see bss_do_combine())

  May 96: added wt_frig_factor for TREC routing term selection. This multiplies
  the weight of postings in stream 1 (2nd stream) only, provided stream 1
  has weights and there are only two streams.

  Nov 94: this now does all available non-log regr BM ops. It needs more
  tidying, but seems to work.

  There is a problem in that both avdl and dl may depend on the index in
  which a given term was looked up. This is handled for type 4 indexes
  but not others.

  Added BM25/250/2500 Jul 94
  These are tf^c/(K^c + tf^c) where K = k1(1 - b + b * dl/avedl),
  c = 1 + m * K)
  (scale factor might as well be k1 + 1 again)

  This version of combine() is set up ONLY to do two of the new probabilistic
  models: BM1100 and BM1500

  NB: June 94: this func now does all weighted ops
      June 94: new op bm110 does passages

  BM1100 is sigma(tf * k1/(k1*dl/avdl + tf) * w) + doclength_correction
  BM1500 is sigma(tf * k1/(k1+tf) * w) + doclength_correction

  (the multiplier k1 in the numerator is only used if k1 > 1; the
  purpose of this correction is to keep weights in a reasonable range,
  and to reduce the interaction of k1 and k2 )

  (June 94: multiplier now k1 + 1 for all k1)

  where doclength_correction is k2 * nterms * (avdl - dl)/(avedl + dl)
  and nterms is the number of atomic sets in the query

******************************************************************************/

int
bss_combine_bm(int num, int op, int scorefunc, Setrec *outsrec,
	       u_int find_flags,
	       u_int limit, Setrec *srecs[], double weights[], int doclens[],
	       int i_avedoclen, int sameas[], int target,
	       double k1, double k2, double k7, double b,
	       double wt_frig_factor[] )
{
  /* scorefunc unused yet (Jan 01) */
  register int j ;		/* Loop counter */
  bss_Gp *pstgs[MAXMERGE] ;

  bss_Gp *outpstg = outsrec->pstg ;
  int outpstgtype = outsrec->pstgtype ;
  u_int *outposptr ;
  int outposcount ;
  int posmax = (outpstgtype & TEXTPOS) ? MAXTEXTPOS : MAXLONGPOS ;
  int this_tf ;			/* Accumulate tf for an output posting */
  u_short *lims = Cdb.lims ;
  int mark ;
  int docs_considered = 0 ;

  double streamweights[MAXMERGE] ;
				/* Local copy of the weights array */
  double avedoclen = i_avedoclen ;
  double n_k1 = k1 / avedoclen ; /* For bm11 */
  double nn_k1 = k1 / avedoclen * b ; /* For bm25 */
  double c_b = (1 - b) * k1 ; /* For bm25 */
  /* For new BM30 etc */
  double alpha = exp(-k1) ;
  double ntf, B, entf ;

  BOOL has_k2 = FALSE ;

  double doclen ;
  BOOL got_doclen ;
  double aw = outsrec->aw ;
  double cutoff_score = aw ;
  double old_cutoff ;		/* Only used if D_TARGET */
  double tfp ;

  double t ;

  BOOL hasweights[MAXMERGE] ; /* True if input stream has individual
				 weights (e.g. from a best match) */
  int numfresh = 0 ;		/* Number of streams without posting wts  */

  double weight ;
  int minval = 0 ;		/* Value (i.e. address field) of current 
				   posting */
  register int jmin ; 	        /* First stream in which current posting 
				   appears */
  int lnum ;
  int numcontribs ;

  BOOL posovf = FALSE ;

  int totcontribs = 0, sortcount = 0 ;

  static char *func = "bss_combine_bm" ;

 printf("bss_combine_bm is called by Okapi\n");   /*added by James Miao to check which combine function is called 2009.8.25*/

  outpstg->numpos = outpstg->numpars = 0 ;

  if ( op != OP_BM2 ) {
    DO_MPW(outsrec) ;
  }
  else outsrec->mpw = MAXWEIGHT ;

  SETUP_WEIGHT_DIST(outsrec) ;

  /* Initialise */
  /* Added empty set check Jan 96 */

  {
    /* Set up selection tree and read in first batch */
    int val[MAXMEMMERGE] ;
    int nodes[2 * MAXMEMMERGE] ;
    /* register int *nodes = NULL ; */
    int L2 = lg2(num) ;
    int ltr = 1 << L2 ;		/* Notional number of nodes in top row */
    int nrows = L2 + 1 ;	/* Number of rows */
    int st = ltr ;		/* Start of top row, used for locating leaf
				   node belonging to given stream */
    register int ii, jj ;
    int r ;
    for ( ii = 0 ; ii < MAXMEMMERGE ; ii++ ) val[ii] = INT_MAX ;
    /* Set up top row (not really needed) */
    for ( jj = 0 ; jj < num ; jj++ ) {
      /* Don't know how to deal with sameas[] (see bss_combine_pass()) */
      if ( limit == 0 ) pstgs[jj] = get_next_pstg(srecs[jj]) ;
      else 
	do pstgs[jj] = get_next_pstg(srecs[jj]);
	while ( pstgs[jj]->rec != INT_MAX &&
		(lims[pstgs[jj]->rec - 1] & limit) != limit ) ;
      val[jj] = pstgs[jj]->rec ;
    }
    /* All top row (leaf) nodes contain corresponding stream numbers,
       even if streams nonexistent */
    for ( jj = 0, ii = 1 << (nrows - 1) ; ii < (1 << nrows) ; jj++, ii++ )
      nodes[ii] = jj ;
    for ( r = nrows - 2 ; r >= 0 ; r-- ) {
      /* Set up each row below top (top row is row nrows - 1) */
      for ( jj = 0, ii = (1 << r) ; jj < (1 << r) ; jj++, ii++ ) {
	if ( val[nodes[ii * 2]] <= val[nodes[ii * 2 + 1]] )
	  nodes[ii] = nodes[ii * 2] ;
	else nodes[ii] = nodes[ii * 2 + 1] ;
      }
    }
    /* Now nodes[1] is stream with min val */
    /* Initialise */
    jmin = nodes[1] ;
    minval = val[jmin] ;
    weight = 0.0 ; numcontribs = 0 ;
    outposptr = (unsigned *)outpstg->p ; outposcount = 0 ;
    this_tf = 0 ;
    mark = 0 ;
    got_doclen = FALSE ;

    /* Now start */
    while ( minval < INT_MAX ) { 
      /* Start of a pass */
      if ( val[jmin] != minval ) {/* (initially they're equal) */
	/* Have to finish-weight and check and if necessary output posting
	   for minval, before starting to process the new val[jmin]. */
	
	/* Do the "global" doclength correction */
	/* I think nonzero k2 shd be checked for 1st */
	/* (Feb 97: includes ! BM1 condition) */
	/* May 02: can we get rid of k2? */
	switch (op) {
	case OP_BM40:
	case OP_BM41:
	  if ( ! got_doclen ) doclen = doclens[minval - 1] ;
	  weight += k2 * log(doclen/avedoclen) ;
	  break ;
	default:
	  if ( has_k2 && numfresh > 0 ) 
	    weight += k2 * numfresh *
	      (avedoclen - doclen) / (avedoclen + doclen) ;
	  break ;
	}
	if ( weight >= cutoff_score ) {

	  /* Here should come passage stuff, difficult to fit into this way
	     of combining. See bss_combine_pass */

	  docs_considered++ ;	/* Any need? */

	  CHECK_CUTOFF ;

	  /* Deal with positional fields. These have been written haphazardly
	     to outpstg's pos area, and just need sorting. Note there's nothing
	     here to remove duplicates. The sorting here is a big time-waster.
	     It would probably be quicker to merge. Is it worth the overhead
	     of setting up a merge tree for each output posting? */

	  if ( outpstgtype & HASPOS ) {
	    if ( numcontribs > 1 ) {
	      /*gqsort((char *)outpstg->p, outposcount,
		sizeof(unsigned), (int (*)(void *, void *))poscmp) ;*/
	      gqsort_int((u_int *)outpstg->p, outposcount, INT_SORT_THRESH) ;
	      sortcount++ ;
	    }
	    outpstg->numpos = outposcount ;
	  }
	  else if ( outpstgtype & HASTF )
	    outpstg->numpos = (this_tf <= posmax) ? this_tf : posmax ;
	
	  /* Output the posting */
	  if ( outpstgtype & (HASCONTRIBS|HASCOORD) )
	    outpstg->coord = numcontribs ;
	  if ( outpstgtype & HASMARKS ) outpstg->flags = mark ;
	  outpstg->rec = minval ;
	  outpstg->score = weight ;
	  DO_WEIGHTS(weight,outsrec) 
          write_next_pstg(outsrec) ;
	  DO_STATS(weight) ;
	  totcontribs += numcontribs ;
	}
	if ( val[jmin] == INT_MAX ) break ; /* All finished */
	
	numcontribs = 0 ;
	weight = 0.0 ;
	this_tf = 0 ;
	mark = 0 ;
	got_doclen = FALSE ;
	posovf = FALSE ;
	if ( outpstgtype & HASPOS ) {
	  outposptr = (unsigned *)outpstg->p ; outposcount = 0 ;
	}
	minval = val[jmin] = pstgs[jmin]->rec ;
	numcontribs = 0 ;
      } /* val[jmin] != minval */

      /* Accumulate weight */
      this_tf += pstgs[jmin]->numpos ;
      if ( op == OP_BM2 )
	weight += pstgs[jmin]->score * weights[jmin] ;
      else {
	if ( ! hasweights[jmin] ) {
	  if ( op != OP_BM1 && op != OP_BM2 ) {
	    if ( ! got_doclen ) {
	      doclen = doclens[minval - 1] ;	/* & avedoclen is known */
	      /* -1 because recs go from 1, doclens from 0 */
	      got_doclen = TRUE ;
	    }
	    tfp = pstgs[jmin]->numpos ;	/* Needed for most ops .. */
	    /* Some postings have zero term freq because of overflow,
	       assume 1 in this case. */
	  }
	  switch (op) {
	  case OP_BM25:
	  weight += tfp * streamweights[jmin] / (nn_k1 * doclen + c_b + tfp) ;
	  break ;
	  case OP_BM25R:
	    t = tfp * srecs[jmin]->naw / srecs[jmin]->ttf ;
	    weight += t * streamweights[jmin] / (nn_k1 * doclen + c_b + t) ;
	    break ;
	  case OP_BM25S:
	    weight += tfp * streamweights[jmin] /
      (nn_k1 * doclen * srecs[jmin]->ttf / srecs[jmin]->naw + c_b + tfp) ;
	    break ;
	  case OP_BM2600:
	  case OP_BM26:
	    weight += tfp * streamweights[jmin] *
	      (b / (tfp + n_k1 * doclen) + (1 - b) / (tfp + k1)) ;
	    break ;
	  case OP_BM40:
	  case OP_BM41:
	    {
	      double t = tfp / (doclen / CHARS_TO_TOKS) ;
	      weight += log(1 + streamweights[jmin] * t) ;
	    }
	    break ;
	  case OP_BM3000:
	  case OP_BM30:
	    B = 1 - b * (1 - doclen / avedoclen) ;
	    ntf = tfp / B ;
	    entf = exp(-ntf) ;
	    weight += streamweights[jmin] *
	      log((alpha+1)/(alpha+entf))/log((alpha+1)/(alpha+exp(-1))) ;
	    break ;
	  case OP_BM1100:
	  case OP_BM11:
	    weight += tfp * streamweights[jmin] / (tfp + n_k1 * doclen) ;
	    break ;
	  case OP_BM1500:
	  case OP_BM15:
	    weight += tfp * streamweights[jmin] / ( tfp + k1 ) ;
	    break ;
	  default:		/* i.e. BM1 */
	    weight += streamweights[jmin] ;
	    break ;
	  }
	}			/* ! (hasweights[jmin])  */
	else weight += pstgs[jmin]->score * wt_frig_factor[jmin] ;
      }
      /* Do marks */
      if ( outpstgtype & HASMARKS ) mark |= pstgs[jmin]->flags ;
      /* Copy over positional info */
      /* Shd check sameas[] */
      if ( (outpstgtype & HASTF) ) {
	if ( (outpstgtype & HASPOS) && ! posovf ) {
	  int numtodo = pstgs[jmin]->numpos ;
	  if ( outposcount + numtodo > posmax ) {
	    numtodo = posmax - outposcount ;
	    posovf = TRUE ;
	    if ( Dbg & D_COMBINE )
	      fprintf(bss_syslog, 
      "already %d pos recs from IRN %d, truncating to %d\n",
		    outposcount, pstgs[jmin]->rec, posmax) ;
	  }
	  /* The memcpy is about as quick as the integer copy below */
	  memcpy((char *)outposptr, (char *)pstgs[jmin]->p,
		 numtodo * sizeof(U_pos));
	  outposptr += numtodo ;
	  /* {
	    register i ;
	    register u_int *ip = (u_int *)pstgs[jmin]->p ;
	    for ( i = 0 ; i < numtodo ; i++ ) {
	      *outposptr++ = *ip++ ;
	    }
	    } */
	  outposcount += numtodo ;
	}
      }
      /* Got weight contribution from this stream. Now refill stream jmin
	 and update the selection tree. */
      /* ****** */
      numcontribs++ ;
      if ( limit == 0 )
	pstgs[jmin] =
	  /* (sameas[jmin] >= 0) ? pstgs[sameas[jmin]] : */
	    get_next_pstg(srecs[jmin]) ;
      else 
	do pstgs[jmin] =
	     /* (sameas[jmin] >= 0) ? pstgs[sameas[jmin]] : */
	       get_next_pstg(srecs[jmin]) ;
	while ( pstgs[jmin]->rec != INT_MAX &&
		(lims[pstgs[jmin]->rec - 1] & limit) != limit );
      val[jmin] = pstgs[jmin]->rec ;
      
      /* and get new jmin */
      for ( jj = (st + jmin) / 2 ; jj > 0 ; jj /= 2 ) {
	if ( val[nodes[jj * 2]] <= val[nodes[jj * 2 + 1]] )
	  nodes[jj] = nodes[jj * 2] ;
	else nodes[jj] = nodes[jj * 2 + 1] ;
      }
      jmin = nodes[1] ;
    } /* End of main selection and weighting loop */
    /* if ( nodes != NULL ) bss_free(nodes) ; */
  } /* Selection and weighting section */

  report_comb ;
  return outsrec->num ;
}


/******************************************************************************

  bss_combine_pass()

  Feb 97: this is only used if op is bm250/1 or there are fewer streams than
  it's worth using a selection tree for. It may not be quite consistent with
  the other bm function.

  Taken from the old bss_combine_bm() July 96 so as to recover the passage
  retrieval stuff.

  Added BM25/250/2500 Jul 94
  These are tf^c/(K^c + tf^c) where K = k1(1 - b + b * dl/avedl),
  c = 1 + m * K)
  (scale factor might as well be k1 + 1 again)

  BM1100 is sigma(tf * k1/(k1*dl/avdl + tf) * w) + doclength_correction
  BM1500 is sigma(tf * k1/(k1+tf) * w) + doclength_correction

  (June 94: multiplier now k1 + 1 for all k1)

  where doclength_correction is k2 * nterms * (avdl - dl)/(avedl + dl)
  where nterms is the number of atomic sets in the query

  There is a problem in that both avdl and dl may depend on the index in
  which a given term was looked up. This is handled for type 4 indexes
  but not others. But (Feb 96) there are now only indexes types 4 - 7.

  Nov 94: this now does all available non-log regr BM ops. It needs more
  tidying, but seems to work.

  Mar 01: bss_combine_atomic() is same but single input set.

******************************************************************************/

int
bss_combine_pass(int num, int op, int scorefunc, Setrec *outsrec,
		 int iptypes[], u_int find_flags, u_int limit, Setrec *srecs[],
		 double weights[], int doclens[], int i_avedoclen,
		 int sameas[], int target, double k1, double k2, double k7,
		 double b,
		 double wt_frig_factor[], int passage_unit,
		 int passage_step, int passage_maxlen)
{
  //printf ("working in here now\n"); /* mko */
  register int j ;		/* Loop counter */
  int status[MAXMERGE] ;	/* Each stream is in one of 3 states
				   OK, EMPTY or FINISHED */
  bss_Gp *pstgs[MAXMERGE] ;
  bss_Gp *outpstg = outsrec->pstg ; /* Not sure this is safe, may change? */
  int outpstgtype = outsrec->pstgtype ;
  u_int *outposptr ;
  int outposcount ;
  int posmax = (outpstgtype & TEXTPOS) ? MAXTEXTPOS : MAXLONGPOS ;
  int this_tf ;			/* Accumulate tf for an output posting */
  u_short *lims = Cdb.lims ;
  int mark ;
  int docs_considered = 0 ;
  double streamweights[MAXMERGE] ;
				/* Local copy of the weights array */
  double doclen ;
  BOOL has_k2 = FALSE ;
  double avedoclen = i_avedoclen ;
  double aw = outsrec->aw ;
  /* double topmult[MAXMERGE] ;*/
  double bm25_K ;
  double tfp ;			/* Float version of a numpos field */

  /* For new BM30 etc */
  double alpha = exp(-k1) ;
  double ntf, B, entf ;

  double t ;

  BOOL hasweights[MAXMERGE] ;	/* True if input stream has individual
				   weights (e.g. from a best match) */
  int numfresh = 0 ;		/* Number of streams without posting wts  */
  BOOL got_doclen ;

  double cutoff_score = aw ;
  double old_cutoff ;		/* Only used if D_TARGET */
  int lnum ;

  double weight, score ;
  double auxwt ;
  BOOL do_auxwt = (scorefunc > 0) ;

  double minwt, wt_range ;
  double rmpw = 0.0 ;
  int minval = 0 ;		/* Value (i.e. address field) of current 
				   posting */
  register int jmin ; 	        /* First stream in which current posting 
				   appears */
  int k ;
  int streams[MAXMERGE] ;
  int numcontribs ;
  char *envp ;
  /* Passage stuff */
  int p_wts_gtr = 0 ;
  Para_rec *p_rec ;
  double passage_avedoclen = 0.0 ;
  double passage_thresh_doclen = 0.0 ;
  int passage_maxiters = 32768 * 32768 ;
  int maxoutposcount = 0 ;
  int totcontribs = 0, sortcount = 0 ;

  static char *func = "bss_combine_pass" ;

 printf("bss_combine_pass is called by Okapi\n");   /*added by James Miao to check which combine function is called 2009.8.25*/

  if ( op == OP_BM250 || op == OP_BM251 ) {	/* passages */
    if ( passage_avedoclen == 0 ) {
      envp = getenv("BSS_PASSAGE_AVEDOCLEN") ;
      if ( envp != NULL ) {
	passage_avedoclen = atof(envp) ;
	if ( Dbg &(D_COMBINE|D_PASSAGES) )
	  fprintf(Err_file, "passage_avedoclen=%f\n", passage_avedoclen) ;
      }
    }
    if ( passage_thresh_doclen == 0 ) {
      envp = getenv("BSS_PASSAGE_THRESH_DOCLEN") ;
      if ( envp != NULL ) {
	passage_thresh_doclen = atof(envp) ;
	if ( Dbg & (D_COMBINE|D_PASSAGES) )
	  fprintf(Err_file, "passage_thresh_doclen=%f\n",
		  passage_thresh_doclen) ;
      }
    }
    if ( passage_maxiters == 32768 * 32768 ) {
      envp = getenv("BSS_PASSAGE_MAXITERS") ;
      if ( envp != NULL ) {
	passage_maxiters = atoi(envp) ;
	if ( Dbg & (D_COMBINE|D_PASSAGES) )
	  fprintf(Err_file, "passage_maxiters=%d\n", passage_maxiters) ;
      }
    }
    if ( Dbg & D_PASSAGES ) 
      do_passages(0xfffffffe, 0, 0, 0, 0, 0, 0, NULL, NULL,
	  NULL, 0.0, 0.0, 0.0, 0.0, 0) ; /* Zeroes statistical variables */
  } /* (bm250/1) */

  outpstg->numpos = outpstg->numpars = 0 ;
  maxoutposcount = 0 ;

  DO_MPW(outsrec) ;

  minwt = 0 ;			/* Bodge */
  wt_range = rmpw + DELTA_WT - minwt ;

  SETUP_WEIGHT_DIST(outsrec) ;

  for ( j = 0 ; j < num ; j++ ) status[j] = EMPTY ;

  minval = 0 ;

  /* Now start */

  while ( minval < INT_MAX ) { 
    /* Start of a pass */
    /* Read in any which are empty, then find first stream (jmin) with
       smallest posting */
    /* Should be prepared to use selection tree here, would speed it
       for large enough value of num */

    for ( j = 0, minval = INT_MAX ; j < num ; j++ ) {
      if ( status[j] == FINISHED ) continue ; /* Added June 94, might speed it
						 slightly */
      if ( status[j] == EMPTY ) {
	if ( limit != 0 ) {
	  do {
	    pstgs[j] = (sameas[j] >= 0) ? pstgs[sameas[j]] :
	      get_next_pstg(srecs[j]) ;
	  }
	  while ( pstgs[j]->rec != INT_MAX &&
		  (lims[pstgs[j]->rec - 1] & limit) != limit );
	}
	else {
	  pstgs[j] = (sameas[j] >= 0) ? pstgs[sameas[j]] :
	    get_next_pstg(srecs[j]) ;
	}
	if ( pstgs[j]->rec != INT_MAX ) status[j] = OK ;
	else {
	  status[j] = FINISHED ;
	  continue ;
	}
      } /* (empty) */
      /* (status OK) */
      if ( pstgs[j]->rec < minval ) {
	numcontribs = 0 ;
	jmin = j ; minval = pstgs[j]->rec ; 
	streams[numcontribs++] = jmin ;	/* June 94 */
      }
      else if ( pstgs[j]->rec == minval ) {
	streams[numcontribs++] = j ;
      }
    } /* (read pstgs loop) */
    if ( minval == INT_MAX ) break ; /* All streams finished */

   /* Accumulate weight over postings with addr=minval & mark them for refill*/
    for ( k = 0, weight = 0.0, auxwt = 0.0, 
	    got_doclen = FALSE, this_tf = 0, mark = 0 ;
	  k < numcontribs ; k++ ) {
      j = streams[k] ;
      if ( sameas[j] < 0 ) this_tf += pstgs[j]->numpos ; ;
      status[j] = EMPTY ;

      if ( ! hasweights[j] ) {
	if ( op != OP_BM1 ) {
	  if ( ! got_doclen ) {
	    doclen = doclens[minval - 1] ;
	    got_doclen = TRUE ;
	  }
	  tfp = pstgs[j]->numpos ;	/* Needed for most ops .. */
	}
	switch (op) {
	case OP_BM25:
	case OP_BM250:
	case OP_BM251:
          bm25_K = k1 * (1.0 - b + b * doclen / avedoclen);
	  weight += tfp * streamweights[j] / (bm25_K + tfp) ;
         // printf ("\nbm25_K : %10.3f \n\
                   k1     : %10.3f \n\
                   b      : %10.3f \n\
                   doclen      : %10.3f \n\
                   avedoclen      : %10.3f \n\
                   weight      : %10.3f\n ", bm25_K, k1, b, doclen, avedoclen, weight);
	  break ;
	case OP_BM25R:
	  /* Mar 2000: Noticed BM25R and S had srecs[jmin]. I think this
	     should be srecs[j]. */
          t = tfp * srecs[j]->naw / srecs[j]->ttf ;
          bm25_K = k1 * (1.0 - b + b * doclen / avedoclen);
	  weight += t * streamweights[j] / (bm25_K + t) ;
	  break ;
	case OP_BM25S:
          bm25_K = k1 * (1.0 - b + b * doclen * srecs[j]->ttf /
			 (avedoclen * srecs[j]->naw));
	  weight += tfp * streamweights[j] / (bm25_K + tfp) ;
	  break ;
	case OP_BM2600:
	case OP_BM26:
	  weight += tfp * streamweights[j] *
	    (b / (tfp + k1 * doclen / avedoclen) + (1 - b) / (tfp + k1)) ;
	  break ;
	case OP_BM40:
	case OP_BM41:
	  {
	    double t = tfp / (doclen / CHARS_TO_TOKS) ;
	    weight += log(1 + streamweights[j] * t) ;
	  }
	  break ;
	case OP_BM3000:
	case OP_BM30:
	  B = 1 - b * (1 - doclen / avedoclen) ;
	  ntf = tfp / B ;
	  entf = exp(-ntf) ;
	  /* (Mar 2000: noticed I had jmin not j in line below, assume
	     this was wrong? (We didn't do any good with BM30, was this why?!)
	  */
	  weight += streamweights[j] *
	    log((alpha+1)/(alpha+entf))/log((alpha+1)/(alpha+exp(-1))) ;
	  break ;
	case OP_BM1100:
	case OP_BM11:
	  weight += tfp * streamweights[j] /
	    (tfp + k1 * doclen / avedoclen) ;
	  break ;
	case OP_BM1500:
	case OP_BM15:
	  weight += tfp * streamweights[j] /
	    ( tfp + k1 ) ;
	  break ;
	default:		/* i.e. BM1 */
	  weight += streamweights[j] ;
	  break ;
	} /* (switch(op)) */
      }	/* ! (hasweights[j])  */
      else {
	if ( do_auxwt ) {
	  switch (scorefunc) {
	  case 4:
	    auxwt += srecs[j]->qtweight * pstgs[j]->score ;
	    break ;
	  default:
	    auxwt += weight * pstgs[j]->score ;
	  }
	}
	weight += pstgs[j]->score * wt_frig_factor[j] ;
      }
      /* Do marks */
      if ( outpstgtype & HASMARKS ) mark |= pstgs[jmin]->flags ;
    }				/* (contributing stream) */
    /* Do any required "global" corrections */
    /* Using k2 also for Djoerd Hiemstra's doclength correction to BM40/41 */
    switch (op) {
    case OP_BM40:
    case OP_BM41:
      if ( ! got_doclen ) doclen = doclens[minval - 1] ;
      weight += k2 * log(doclen/avedoclen) ;
      break ;
    default:
      if ( k2 > 0 && op != OP_BM1 && numfresh > 0 ) 
	weight += k2 * numfresh * (avedoclen - doclen) / (avedoclen + doclen) ;
      break ;
    }
    switch (scorefunc) {
    case 0:
      score = weight ;
      break ;
    case 1:
    case 4:
      score = weight - k7 * auxwt / outsrec->mpw ;
      break ;
    case 2:
      score = weight - k7 * auxwt / outsrec->mpw / (double)num ;
      break ;
    case 3:
      score = weight - k7 * auxwt / outsrec->mpw / (double)numcontribs ;
      break ;
    default:
      score = weight ;
    }

    /* Now can decide if we accept this posting. */
    /* if ( weight < aw ) continue ; */
    if ( score < cutoff_score ) continue ;

    if ( op == OP_BM250 || op == OP_BM251 ) {
      /* July 94: simply returns best passage at present */
      /* Dec 94: POWER removed. Even ifdef POWER it isn't used. */
      /* Store result in pars[0], original in pars[1], and the best
	 weight will go in wt if bm250 */
      /* unit and max are obtained from the environment */
      if ( doclen >= passage_thresh_doclen ) {
	if ( passage_avedoclen < 0.1 ) passage_avedoclen = avedoclen ;
	p_rec = do_passages(minval, Cdb.nf, passage_unit, passage_maxlen,
			    passage_step, 
			    numfresh, numcontribs, streams, pstgs,
			    streamweights,
			    k1, k2, b,
			    passage_avedoclen, passage_maxiters);
	if ( p_rec == NULL ) return -9 ; /* Just in case */
	memcpy((char *)&outpstg->pars[0], (char *)p_rec, sizeof(Para_rec)) ;
	outpstg->numpars = 2 ;
	outpstg->pars[1].wt = score ; /* Jul 01: was wt, forgot to do this
					 when doing the new score functions */
	outpstg->pars[1].sp = 0 ;
	outpstg->pars[1].fp = 32767 ;
	outpstg->pars[1].fd = Cdb.nf ; /* Bodge. Changed Jan 00 from 3 to nf */
   				       /* And same below */
	if ( p_rec->fd == 0 ) {	       /* Passage not done because e.g. not
					  enough paras or too many iterations*/
	  memcpy((char *)&outpstg->pars[0], (char *)&outpstg->pars[1],
		 sizeof(Para_rec)) ;
	}
	  
	/* weight -> score (see above comment) */
	if ( p_rec->wt > score ) p_wts_gtr++ ;
	if ( op == OP_BM250 && p_rec->wt >= score ||
	     op == OP_BM251 && p_rec->wt > 0 )
	  score = p_rec->wt ;
	/* Regardless (BM251, May 97, first is passage, 2nd doc) */
      }
      else { /* (doc too short or whatever, not done at all) */
	outpstg->pars[1].wt = score ;
	outpstg->pars[1].sp = 0 ;
	outpstg->pars[1].fp = 32767 ;
	outpstg->pars[1].fd = Cdb.nf ; /* See above */
	/* Made same as [1] (May 97), used to be zero weight and rubbish
	   values */
	memcpy((char *)&outpstg->pars[0], (char *)&outpstg->pars[1],
	       sizeof(Para_rec)) ;
      }
    } /* (bm250/1) */

    docs_considered++ ;
    CHECK_CUTOFF ;
    if ( outpstgtype & HASPOS ) {
      /* Merge positional fields */
    
      MERGE_ALL_POS ;

    }	/* HASPOS  */
    else if ( outpstgtype & HASTF )
      outpstg->numpos = (this_tf <= posmax) ? this_tf : posmax ;

    if ( outpstgtype & (HASCONTRIBS|HASCOORD) ) outpstg->coord = numcontribs ;
    if ( outpstgtype & HASMARKS ) outpstg->flags = mark ;
    outpstg->rec = minval ;
    outpstg->score = score ;
    DO_WEIGHTS(score,outsrec) 
    if ( do_auxwt ) {
      outpstg->wt = weight ;
      outpstg->auxwt = auxwt ;
    }

    write_next_pstg(outsrec) ;
    DO_STATS(score) ;
  }

  if ( Dbg & D_PASSAGES ) {
    /* Stats */
    if ( op == OP_BM250 || op == OP_BM251 ) {
      do_passages(0xffffffff,  0, 0, 0, 0, 0, 0, NULL, NULL,
		  NULL, 0.0, 0.0, 0.0, 0.0, 0) ;
      fprintf(bss_syslog, "Psge > whole %d\n", p_wts_gtr) ;
    }
  }

  return outsrec->num ;
}

/* bss_willett(): accumulate scores for one stream */

int
bss_willett(int op, Setrec *srec, float *scores, double weight,
	    int *doclens, int i_avedoclen,
	    double k1, double b, double *min, double *max)
{
  bss_Gp *inpstg ;
  int i ;
  double reldoclen ;
  double score ;

 printf("bss_willett is called by Okapi\n");   /*added by James Miao to check which combine function is called 2009.8.25*/

  for ( i = 0 ; i < srec->naw ; i++ ) {
    inpstg = get_next_pstg(srec) ;
    if ( op == OP_BM25 ) {
      reldoclen = (double)doclens[inpstg->rec - 1] / (double)i_avedoclen ;
      score = weight * ((k1 + 1) * inpstg->numpos) /
			   (k1 * (b * reldoclen + 1 - b) + inpstg->numpos) ;
    }
    else if ( op == OP_BM2 ) score = inpstg->score * weight ;
    else score = weight ;
    scores[inpstg->rec] += score ;
    if (scores[inpstg->rec] < *min ) *min = scores[inpstg->rec] ;
    if (scores[inpstg->rec] > *max ) *max = scores[inpstg->rec] ;
  }
  return 0 ;
}

int
bss_quick_merge(int num, int op, u_int flags, Setrec **srecs, float *scores,
		double *weights, int *doclens, int i_avedoclen,
		double k1, double b, double *min, double *max)
{
  int i ;
  int ires ;
  for ( i = 0 ; i < num ; i++ ) {
    if ( open_pstgs(srecs[i], 0, flags) < 0 ) return -1 ;
    ires = bss_willett(op, srecs[i], scores, weights[i], doclens,
		       i_avedoclen, k1, b, min, max) ;
    (void)close_pstgs(srecs[i], 0, flags) ;
  }
  return 0 ;
}

#define GRAN 5

int
bss_quick_combine(int num, int op, Setrec *outsrec, u_int flags,
		  Setrec **insrecs, double *weights,
		  int *doclens, int i_avedoclen,
		  int target, double k1, double b,
		  int N) 
{
  printf ("Should be here with N = %d\n", N);
  char *func = "bss_quick_combine" ;
  bss_Gp *outpstg = outsrec->pstg ;
  double range ;
  double cutoff_score = outsrec->aw ;
  int i ;

  outsrec->minweight = FLT_MAX ;
  outsrec->maxweight = FLT_MIN ;
  outsrec->scores = (float *)bss_calloc(N + 1, sizeof(float)) ;
  if ( Dbg & DD_MEM )
    fprintf(bss_syslog, "%s(): alloc %d for scores array\n",
	    func, (N + 1) * sizeof(float)) ;
  bss_quick_merge(num, op, flags, insrecs, outsrec->scores, weights,
		  doclens, i_avedoclen, k1, b,
		  &outsrec->minweight, &outsrec->maxweight) ;
  outsrec->mpw = outsrec->maxweight ;
  outsrec->gran = GRAN ;
  outsrec->bkts = bss_calloc(outsrec->gran + 1, sizeof(struct rbkt)) ;
  if ( Dbg & DD_MEM )
    fprintf(bss_syslog, "%s(): alloc %d for buckets array\n",
	    func, (outsrec->gran + 1) * sizeof(struct rbkt))  ;
  range = outsrec->maxweight - outsrec->minweight ;
  /* Split here according as ephemeral or normal.
     If normal, use bkts just for count.
     Otherwise, store IRNs as well. */
  /*outsrec->set_type |= S_EPHEMERAL ;*/
  if ( outsrec->set_type & S_EPHEMERAL ) {
    struct rbkt *b ;
    struct rrec *r ;
    double score ;
    double gran = outsrec->gran ;
    int bucketnum, bnum ;
    int tcount ;
    struct rbkt *tb ;
    for ( i = 1 ; i <= N ; i++ ) {
      if ( (score = outsrec->scores[i]) == 0 ) continue ;
      if ( score < cutoff_score ) continue ;
      bucketnum = (gran * (score - outsrec->minweight))/range ;
      b = &outsrec->bkts[bucketnum] ;
      /*r = bss_calloc(1, sizeof(struct rrec)) ;
      r->rn = i ;
      r->next = b->rlist ;
      b->rlist = r ;*/
      b->count++ ;
      outsrec->naw++ ;
      if ( target && outsrec->naw >= target && outsrec->naw % 1000 == 0 ) {
	/* Count down buckets until have target num */
	for ( tcount = 0, bnum = outsrec->gran, tb = &outsrec->bkts[bnum] ;
	      tcount < target && bnum >= 0 ;
	      tcount += tb->count, tb = &outsrec->bkts[--bnum]) ;
	fprintf(stderr, "old cutoff %.3f, ", cutoff_score) ;
	cutoff_score = (range * (double)bnum / gran) + outsrec->minweight ;
	fprintf(stderr, "new cutoff %.3f\n", cutoff_score) ;
      }
    }
  }
  else {			/* Normal */
    float score ;
    for ( i = 1 ; i <= N ; i++ ) {
      if ( (score = outsrec->scores[i]) == 0 ) continue ;
      if ( score < outsrec->aw ) continue ;
      outpstg->rec = i ;
      outpstg->score = outsrec->scores[i] ;
      write_next_pstg(outsrec) ;
    }
  }
  return outsrec->num ;
}





#ifdef NEW_ATOMIC_COMBINE

int
bss_combine_atomic(int op, int scorefunc, Setrec *outsrec,
		 int iptype, u_int find_flags, u_int limit, Setrec *srec,
		 double weight, int doclens[], int i_avedoclen,
		 int target, double k1, double k2, double k7,
		 double b,
		 double wt_frig_factor, int passage_unit,
		 int passage_step, int passage_maxlen)
{
  register int j ;		/* Loop counter */
  int status ;			/* Stream is in one of 3 states
				   OK, EMPTY or FINISHED */
  bss_Gp *pstg ;
  bss_Gp *outpstg = outsrec->pstg ; /* Not sure this is safe, may change? */
  int outpstgtype = outsrec->pstgtype ;
  u_int *outposptr ;
  int outposcount ;
  int posmax = (outpstgtype & TEXTPOS) ? MAXTEXTPOS : MAXLONGPOS ;
  int this_tf ;			/* Accumulate tf for an output posting */
  u_short *lims = Cdb.lims ;
  int mark ;
  int docs_considered = 0 ;
  double streamweight ;
				/* Local copy of the weights array */
  double doclen ;
  BOOL has_k2 = FALSE ;
  double avedoclen = i_avedoclen ;
  double aw = outsrec->aw ;

  double bm25_K ;
  double tfp ;			/* Float version of a numpos field */

  /* For new BM30 etc */
  double alpha = exp(-k1) ;
  double ntf, B, entf ;

  double t ;

  BOOL hasweight ;	/* True if input stream has individual
				   weights (e.g. from a best match) */
  int numfresh = 0 ;		/* Number of streams without posting wts  */
  BOOL got_doclen ;

  double cutoff_score = aw ;
  double old_cutoff ;		/* Only used if D_TARGET */
  int lnum ;

  double weight, score ;
  double auxwt ;
  BOOL do_auxwt = (scorefunc > 0) ;

  double minwt, wt_range ;
  double rmpw = 0.0 ;
  int val = 0 ;			/* Value (i.e. address field) of current 
				   posting */
  int k ;
  int stream ;
  char *envp ;
  int maxoutposcount = 0 ;
  int totcontribs = 0, sortcount = 0 ;

  static char *func = "bss_combine_atomic" ;

  outpstg->numpos = outpstg->numpars = 0 ;
  maxoutposcount = 0 ;

  DO_MPW(outsrec) ;

  minwt = 0 ;			/* Bodge */
  wt_range = rmpw + DELTA_WT - minwt ;

  SETUP_WEIGHT_DIST(outsrec) ;

  status = EMPTY ;

  while ( status != FINISHED ) { 
    if ( status == FINISHED ) continue ;
    if ( status == EMPTY ) {
      if ( limit != 0 ) {
	do pstg = get_next_pstg(srec) ;
	while ( pstg->rec != INT_MAX &&
		(lims[pstg->rec - 1] & limit) != limit );
      }
      else pstg = get_next_pstg(srec) ;
      val = pstg->rec ;
      if ( val != INT_MAX ) status = OK ;
      else {
	status = FINISHED ;
	continue ;
      }
    } /* (empty) */
    /* (status OK) */
    status = EMPTY ;
    if ( ! hasweights ) {
      if ( op != OP_BM1 ) {
	if ( ! got_doclen ) {
	  doclen = doclens[val - 1] ;
	  got_doclen = TRUE ;
	}
	tfp = pstg->numpos ;	/* Needed for most ops .. */
      }
      switch (op) {
      case OP_BM25:
	bm25_K = k1 * (1.0 - b + b * doclen / avedoclen);
	weight = tfp * streamweight / (bm25_K + tfp) ;
	break ;
      case OP_BM25R:
	t = tfp * srecs[j]->naw / srecs[j]->ttf ;
	bm25_K = k1 * (1.0 - b + b * doclen / avedoclen);
	weight = t * streamweight / (bm25_K + t) ;
	break ;
      case OP_BM25S:
	bm25_K = k1 * (1.0 - b + b * doclen * srec->ttf /
		       (avedoclen * srec->naw));
	weight = tfp * streamweight / (bm25_K + tfp) ;
	break ;
      case OP_BM2600:
      case OP_BM26:
	weight = tfp * streamweight *
	  (b / (tfp + k1 * doclen / avedoclen) + (1 - b) / (tfp + k1)) ;
	break ;
      case OP_BM40:
      case OP_BM41:
	{
	  double t = tfp / (doclen / CHARS_TO_TOKS) ;
	  weight += log(1 + streamweight * t) ;
	}
	break ;
      case OP_BM3000:
      case OP_BM30:
	B = 1 - b * (1 - doclen / avedoclen) ;
	ntf = tfp / B ;
	entf = exp(-ntf) ;
	weight = streamweight *
	  log((alpha+1)/(alpha+entf))/log((alpha+1)/(alpha+exp(-1))) ;
	break ;
      default:		/* i.e. BM1 */
	weight += streamweight ;
	break ;
      } /* (switch(op)) */
    }	/* ! (hasweights[j])  */
    else {
      if ( do_auxwt ) auxwt += weight * pstg->score ;
      weight += pstg->score * wt_frig_factor ;
    }
    /* Do marks */
    if ( outpstgtype & HASMARKS ) mark |= pstg->flags ;
    /* Do any required "global" corrections */
    /* Using k2 also for Djoerd Hiemstra's doclength correction to BM40/41 */
    switch (op) {
    case OP_BM40:
    case OP_BM41:
      if ( ! got_doclen ) doclen = doclens[val - 1] ;
      weight += k2 * log(doclen/avedoclen) ;
      break ;
    default:
      if ( k2 > 0 && op != OP_BM1 && numfresh > 0 ) 
	weight += k2 * numfresh * (avedoclen - doclen) / (avedoclen + doclen) ;
      break ;
    }

    score = weight ;

    /* Now can decide if we accept this posting. */
    if ( score < cutoff_score ) continue ;

    docs_considered++ ;
    CHECK_CUTOFF ;

    if ( outpstgtype & HASTF ) {
      outpstg->numpos = (this_tf <= posmax) ? this_tf : posmax ;
      if ( outpstgtype & HASPOS ) 
	memcpy((char *)outpstg->p, (char *)pstg->p,
	       outpstg->numpos * sizeof(U_pos));
    }

    if ( outpstgtype & (HASCONTRIBS|HASCOORD) ) outpstg->coord = numcontribs ;
    if ( outpstgtype & HASMARKS ) outpstg->flags = mark ;
    outpstg->rec = val ;
    outpstg->score = score ;
    DO_WEIGHTS(score,outsrec) 
    outsrec->qtweight = streamweight ;
    write_next_pstg(outsrec) ;
    DO_STATS(score) ;
  }

  return outsrec->num ;
}

#endif /* NEW_ATOMIC_COMBINE */

/******************************************************************************

  bss_combine_limit()
  Special combine for "Robertson limit"

  Does AND2 and NOT2 where records not in all the others/in any of the others
  (resp) are removed from the first set. Otherwise postings are output
  unchanged.

  Oct 95: added AND3 (MARK), which copies its first input set to its output
  with the requested flag bit(s) set or unset in any posting which occurs in
  any input set other than the first.

  July 95: Added OP_NOT3 which tries to remove the effect of a single set
  from a set containing it. For every posting which matches it removes any
  positional records matching those in the set being removed, and reduces the
  weight if any by the weight in the set being removed. Any postings not
  coming up to aw are not output. No effect on sets which have no weight
  information and no positional information.

******************************************************************************/

int
bss_combine_limit(int num, int op, u_int op_flags, int np[],
		  Setrec *srecs[], int sameas[], int target, Setrec *outsrec,
		  u_int find_flags) 
{
  register int j, k ;		/* Loop counter */
  int status[MAXMERGE] ;	/* Each stream is in one of 3 states
				   OK, EMPTY or FINISHED */
  bss_Gp *pstgs[MAXMERGE] ;
  bss_Gp *outpstg  ;
  int outpstgtype = outsrec->pstgtype ;
  int streams[MAXMERGE] ;
  int numcontribs ;		/* No of contributors to an output posting */
  int minval = 0 ;		/* Value (i.e. address field) of current 
				   posting */
  register int jmin ; 	        /* First stream in which current posting 
				   appears */
  double wt = 0 ;		/* Initialization important */
  /* double minwt, wt_range ;	  NOT3 only */
  double aw = outsrec->aw ;



  char mark = FFL_GETMARK(find_flags) ;
  bss_Gp *savp = outsrec->pstg ;

  static char *func = "bss_combine_limit" ;

 printf("bss_combine_limit is called by Okapi\n");   /*added by James Miao to check which combine function is called 2009.8.25*/


  outpstg = outsrec->pstg = srecs[0]->pstg ;
  SETUP_WEIGHT_DIST(outsrec)

  /* Now start */

  for ( j = 0 ; j < num ; j++ ) {
    if ( np[j] > 0 ) status[j] = EMPTY ;
    else status[j] = FINISHED ;
  }
  minval = 0 ;
  while ( minval < INT_MAX ) { 
    /* Start of a pass */
    /* Read in any which are empty, then find first stream (jmin) with
       smallest posting */
    for ( j = 0, minval = INT_MAX ; j < num ; j++ ) {
      if ( status[j] == FINISHED ) {
	if ( op_flags & ANDLIKE ) {
	  minval = INT_MAX ;
	  break ; 
	}
	else continue ;
      }
      if ( status[j] == EMPTY ) {
	if (
	    np[j]-- >0
	    && ( sameas[j] >= 0 && (pstgs[j] = pstgs[sameas[j]])
		|| (pstgs[j] = get_next_pstg(srecs[j])) )
	    )
	  status[j] = OK ;
	else {
	  status[j] = FINISHED ; 
	  if ( op == OP_AND2 ) { /* and any other and-like wh might be added
				    anytime */
	    minval = INT_MAX ;
	    break ;
	  }
	  else continue ;
	}
      }
      /* (status OK) */
      if ( pstgs[j]->rec < minval ) {
	jmin = j ; minval = pstgs[j]->rec ; 
	numcontribs = 0 ;	/* June 94 */
	streams[numcontribs++] = jmin ;	/* June 94 */
      }
      else if ( pstgs[j]->rec == minval ) /* June 94 */
	streams[numcontribs++] = j ; /* June 94 */
    }

    if ( minval == INT_MAX ) break ; /* All streams, or one if and_like,
				       finished */

    for ( k = 0 ; k < numcontribs ; k++ ) status[streams[k]] = EMPTY ;

    /* Now can decide if we accept this posting. */

    if ( jmin != 0 ) continue ;	/* Must be in left operand */ 
    else if ( op == OP_AND2 && numcontribs != num ) continue ;
    else if ( op == OP_NOT2 && numcontribs > 1 ) continue ;
/*  else if ( op == OP_NOT3 ) { Remove positional recs, not done yet */
      
    if ( outpstgtype & HASWT ) {
      wt = pstgs[0]->score ;
      if ( op == OP_NOT3 && numcontribs == 2 ) {
	wt -= pstgs[1]->score ;
	if ( wt <= 0 ) continue ;
	outpstg->score = wt ;
	/* Here remove some pos fields, copying the others into outpstg */
      }
      if ( wt < aw ) continue ;
    } /* (HASWT) */
    /* Anything that gets here will be output. */
    if ( op == OP_MARK ) {
      /* This isn't right. Might be carrying over some marks from an already
	 marked srecs[0]. Have to think about it. */
      if ( numcontribs > 1 || num == 1 ) {
	if ( mark >= 0 ) outpstg->flags |= mark ;
	else outpstg->flags &= ~(-mark) ;
      }
      else outpstg->flags = 0 ;
    }
    /* When NOT3 properly implemented, positional fields in pstgs[1]
       must be removed from pstgs[0] (=outpstg) */
    DO_WEIGHTS(wt,outsrec) ;
    DO_STATS(wt) ;
    write_next_pstg(outsrec) ;
  }
  outsrec->pstg = savp ;
  return outsrec->num ;
}

#ifndef LR
int
bss_combine_lr(void)
{
  return -1 ;
}
#endif

/******************************************************************************

  new_setnum = select_topn(..., n, ...)  SW June 95

  Makes set from top-weighted n records of the given set.

  Not strictly in BSS yet, used in routing_select 1995

  March 96: rewritten. Sets up postings index, reads off offsets of top n and
  stores them with sequence number in temporary memory, sorts by
  ascending offset, then does get_pstg_n in offset sequence and writes them to
  the output set.

******************************************************************************/

struct abcz {
  int offset ;
  int seqnum ;
} ;

static int
cmp(struct abcz *s1, struct abcz *s2)
{
  return s1->offset - s2->offset ;
}

int
select_topn(int setnum, Setrec *outsrec, int n, u_int find_flags)
{
  Setrec *insrec = Setrecs[setnum] ;
  int num_to_do ;
  struct abcz *sortbuf = (struct abcz *)NULL, *p ;

  int j ;
  bss_Gp *savp = outsrec->pstg ;

  if ( insrec->naw == 0 ) return outsrec->num ; /* Nothing to do */
  num_to_do = insrec->naw < n ? insrec->naw : n ;
  if ( prepare_set_for_output(setnum, num_to_do) < 0 ) return -1 ;
  /* (leaves open) */
  if ( (sortbuf = (struct abcz *)bss_malloc(sizeof(struct abcz) * num_to_do))
      == NULL ) {
    prepare_error(SYS_NO_MEM_1, "", "?", 0) ; 
    goto s_error ;
  }
  savp = outsrec->pstg ;
  outsrec->pstg = insrec->pstg ;

  SETUP_WEIGHT_DIST(outsrec)

  /* Get offsets and sequence numbers */
  for ( j = 0, p = sortbuf ; j < num_to_do ; j++, p++ ) {
    p->offset = get_pstg_offset(insrec, j) ;
    p->seqnum = j ;
  }
  (void)gqsort((char *)sortbuf, num_to_do, sizeof(struct abcz),
	       (int (*)(void *, void *))cmp) ;
  /* Now have the numbers of the top pstgs in ascending offset order */
  for ( j = 0, p = sortbuf ; j < num_to_do ; j++, p++ ) {
    get_pstg_n(insrec, p->seqnum) ;
    DO_STATS(outsrec->pstg->score) ;
    DO_WEIGHTS(outsrec->pstg->score,outsrec) 
    write_next_pstg(outsrec) ;
  }
  outsrec->pstg = savp ;
  bss_free(sortbuf) ;
  return outsrec->num ;

 s_error:
  if ( sortbuf != NULL ) bss_free(sortbuf) ;
  return -1 ;
}

/******************************************************************************

  char *
  translate_opcode(int code)  SW Dec94
  
  Returns static string containing opcode

******************************************************************************/

char *
translate_opcode(int code)
{
  static char *s ;
  switch (code) {
  case OP_TOPN:
    s = "top_n" ;
    break ;
  case OP_MARK:
    s = "mark" ;
    break ;
  case OP_NOOP:
    s = "noop" ;
    break ;
  case OP_LKUP:
    s = "lkup" ;
    break ;
  case OP_BM1:
    s = "bm1" ;
    break ;
  case OP_BM2:
    s = "bm2" ;
    break ;
  case OP_BM11:
    s = "bm11" ;
    break ;
  case OP_BM15:
    s = "bm15" ;
    break ;
  case OP_BM1100:
    s = "bm1100" ;
    break ;
  case OP_BM1500:
    s = "bm1500" ;
    break ;
  case OP_BM25:
    s = "bm25" ;
    break ;
  case OP_BM25R:
    s = "bm25r" ;
    break ;
  case OP_BM25S:
    s = "bm25s" ;
    break ;
  case OP_BM26:
    s = "bm26" ;
    break ;
  case OP_BM250:
    s = "bm250" ;
    break ;
  case OP_BM251:
    s = "bm251" ;
    break ;
  case OP_BM30:
    s = "bm30" ;
    break ;
  case OP_BM3000:
    s = "bm3000" ;
    break ;
  case OP_BM40:
    s = "bm40" ;
    break ;
  case OP_BM41:
    s = "bm41" ;
    break ;
  case OP_BM42:
    s = "bm42" ;
    break ;
  case OP_AND1:
    s = "and1" ;
    break ;
  case OP_AND2:
    s = "and2" ;
    break ;
  case OP_OR1:
    s = "or1" ;
    break ;
  case OP_NOT1:
    s = "not1" ;
    break ;
  case OP_NOT2:
    s = "not2" ;
    break ;
  case OP_NOT3:
    s = "not3" ;
    break ;
  case OP_ADJ1:
    s = "adj1" ;
    break ;
  case OP_ADJ2:
    s = "adj2" ;
    break ;
  case OP_SAMEF1:
    s = "samef1" ;
    break ;
  case OP_SAMES1:
    s = "sames1" ;
    break ;
  case OP_LIMIT:
    s = "limit" ;
    break ;
  case OP_COPY:
    s = "copy" ;
    break ;
  default:
    s = "unknown" ;
  }
  return s ;
}

/******************************************************************************

  combine_stats

******************************************************************************/

void
combine_stats(void)
{
}

int
bss_copy_set(void)
{
  return 0 ;
}
