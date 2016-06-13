/******************************************************************************

  ixsubs.c SW May 92

******************************************************************************/

#include "bss.h"

char *inf = "\377\377\377\377" ;
char *inf_3 = "\377\377\377" ;

/*****************************************************************************
  
  Compitr_4 - index types 4 - 9

  Compare indirect two itrs returning -, 0, +
  
  This version expects record format <term><recnum><pos>
                                              3      4 
  and sorts on term, rec, pos					     

  The norec version does <term><pos> only

*****************************************************************************/
static int
compitr_4(char **s1, char **s2) 
{
  register char *t1 ;
  register char *t2 ;
  int c, diff ;
  u_int i1, i2 ;
  int l1, l2 ;
  t1 = *s1 ; t2 = *s2 ;
  l1 = *(u_char *)t1++ ;
  l2 = *(u_char *)t2++ ;
  c = l1 - l2 ;
  if ( c < 0 ) {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    else return -1 ;		/* t2 > because longer */
  }
  else if ( c > 0 ) {
    diff = memcmp(t1, t2, l2) ;
    if ( diff ) return diff ;
    else return 1 ;		/* t1 > because longer */
  }
  else {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;

    t1 += l1 ; t2 += l2 ;
    diff = cv3(t1) - cv3(t2) ;
    if ( diff ) return diff ;
    t1 += 3 ; t2 += 3 ;
    memcpy((char *)&i1, t1, 4) ; memcpy((char *)&i2, t2, 4) ;
    diff = i1 - i2 ;
    return diff ;
  }
}


static int
compitr_4_norec(char **s1, char **s2) 
{
  register char *t1 ;
  register char *t2 ;
  int c, diff ;
  u_int i1, i2 ;
  int l1, l2 ;
  t1 = *s1 ; t2 = *s2 ;
  l1 = *(u_char *)t1++ ;
  l2 = *(u_char *)t2++ ;
  c = l1 - l2 ;
  if ( c < 0 ) {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    else return -1 ;		/* t2 > because longer */
  }
  else if ( c > 0 ) {
    diff = memcmp(t1, t2, l2) ;
    if ( diff ) return diff ;
    else return 1 ;		/* t1 > because longer */
  }
  else {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;

    t1 += l1 ; t2 += l2 ;
    memcpy((char *)&i1, t1, 4) ; memcpy((char *)&i2, t2, 4) ;
    diff = i1 - i2 ;
    return diff ;
  }
}

/*****************************************************************************
  
  Compitr_12 - index types 12 and 13

  Compare indirect two itrs returning -, 0, +
  
  This version expects record format <term><recnum><pos>
                                              4      4 
  and sorts on term, rec, pos					     

*****************************************************************************/
static int
compitr_12(char **s1, char **s2) 
{
  register char *t1 ;
  register char *t2 ;
  int c, diff ;
  u_int i1, i2 ;
  int l1, l2 ;
  t1 = *s1 ; t2 = *s2 ;
  l1 = *(u_char *)t1++ ;
  l2 = *(u_char *)t2++ ;
  c = l1 - l2 ;
  if ( c < 0 ) {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    else return -1 ;		/* t2 > because longer */
  }
  else if ( c > 0 ) {
    diff = memcmp(t1, t2, l2) ;
    if ( diff ) return diff ;
    else return 1 ;		/* t1 > because longer */
  }
  else {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;

    t1 += l1 ; t2 += l2 ;
    diff = cv4(t1) - cv4(t2) ;
    if ( diff ) return diff ;
    t1 += 4 ; t2 += 4 ;
    /* Wouldn't cv4 work? (see also compitr_4()) */
    memcpy((char *)&i1, t1, 4) ; memcpy((char *)&i2, t2, 4) ;
    diff = i1 - i2 ;
    return diff ;
  }
}

/*****************************************************************************
  
  Compitr_10 - index type 10

  Compare indirect two itrs returning -, 0, +

  Also works for type 20 (<term><recnum><wt><qtf>)
  
  Expects <term><recnum> and orders by recnum within term
            v      3    

  Made memcmp instead of strncmp Aug 97 (also the other sort above),
  might be quicker

  The norec version does <term> only

*****************************************************************************/

static int
compitr_10(char **s1, char **s2) 
{
  register char *t1 ;
  register char *t2 ;
  int l1, l2 ;
  int c, diff ;
  
  t1 = *s1 ; t2 = *s2 ;
  l1 = *(u_char *)t1++ ; l2 = *(u_char *)t2++ ; 
  c = l1 - l2 ;
  if ( c < 0 ) {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    else return -1 ;		/* t2 > because longer */
  }
  else if ( c > 0 ) {
    diff = memcmp(t1, t2, l2) ;
    if ( diff ) return diff ;
    else return 1 ;		/* t1 > because longer */
  }
  else {			/* Length equal */
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    t1 += l1 ; t2 += l2 ;
    diff = cv3(t1) - cv3(t2) ;
    return diff ;
  }
}

static int
compitr_10_norec(char **s1, char **s2) 
{
  register char *t1 ;
  register char *t2 ;
  int l1, l2 ;
  int c, diff ;
  
  t1 = *s1 ; t2 = *s2 ;
  l1 = *(u_char *)t1++ ; l2 = *(u_char *)t2++ ; 
  c = l1 - l2 ;
  if ( c < 0 ) {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    else return -1 ;		/* t2 > because longer */
  }
  else if ( c > 0 ) {
    diff = memcmp(t1, t2, l2) ;
    if ( diff ) return diff ;
    else return 1 ;		/* t1 > because longer */
  }
  else return memcmp(t1, t2, l2) ;
}

/*****************************************************************************
  
  Compitr_11 - index type 11

  Same as 10 but 4-byte recnum

  Compare indirect two itrs returning -, 0, +
  
  Expects <term><recnum>
            v      4    

*****************************************************************************/

static int
compitr_11(char **s1, char **s2) 
{
  register char *t1 ;
  register char *t2 ;
  int c, diff ;
  int l1, l2 ;
  t1 = *s1 ; t2 = *s2 ;
  l1 = *(u_char *)t1++ ;
  l2 = *(u_char *)t2++ ;
  c = l1 - l2 ;
  if ( c < 0 ) {
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;
    else return -1 ;		/* t2 > because longer */
  }
  else if ( c > 0 ) {
    diff = memcmp(t1, t2, l2) ;
    if ( diff ) return diff ;
    else return 1 ;		/* t1 > because longer */
  }
  else {			/* Length equal */
    diff = memcmp(t1, t2, l1) ;
    if ( diff ) return diff ;

    t1 += l1 ; t2 += l2 ;
    diff = cv4(t1) - cv4(t2) ;
    return diff ;
  }
}

/*****************************************************************************
  
  outrun_4() - sort and output a run - index types 4-9
  (1998: only 8 and 9 still used, of these)

  Input: <term><recnum><pos> where <pos> is a Pos_text for type 4/6/8 or a
             v      3      4
         Pos6 for type 5/7/9

  Output: <term>[<recnum>[<pos>]FFFFFFFF]FFFFFF
  
  A posting ends with a <pos> field containing FFFFFFFF
  An entire record ends with a <recnum> field containing FFFFFF

  17 May 94: Now discarding infinite positional records which
  arise from sentence or term number overflow.

  May 97: added some sort of check for term and recnum sequence errors
  in merge() 

*****************************************************************************/

/* WATCH THE 3s and 4s!!! */

int
outrun_4(STRINGS *sorttab, char *fnames, int dflg,
	 BOOL silent,
	 BOOL dummy		/* If TRUE throws output away */
	 )
{

  FILE *ofp ;
  u_char *term, *lastterm, *recp, *lastrecp, *posp, *lastposp ;

  int l, count ;
  int outterms = 0, outrecs = 0, outposes = 0 ;
  int outchars = 0 ;

  int infposes = 0 ;

  static char *func = "outrun_4()" ;

  if ( ! silent ) fprintf(stderr, "\nSorting %d keys...", sorttab->n) ;
  gqsort_ins_thresh( (char *) sorttab->p, sorttab->n, sizeof(char *),
	  (int (*)(void *, void *))compitr_4, 1) ;
  if ( ! silent ) fprintf(stderr, " sorted") ;
  if ( dummy ) {
    ofp = fopen("/dev/null", "wb") ;
  }
  else {
    ofp = fopen(fnames, "wb") ;
    if ( ofp == NULL ) {
      fprintf(stderr, "\nCan't open temp output file %s", fnames) ;
      goto o_error ;
    }
  }
  putw(MAGIC_MERGEFILE, ofp) ;	/* Sep 98 */

  lastterm = (u_char *) inf ; lastrecp = (u_char *) inf_3 ;
  lastposp = (u_char *) inf ;
  for ( count = 0 ; count < sorttab->n ; count++ ) {
    term = (u_char *) sorttab->p[count] ;
    l = len(term) + 1 ;
    recp = term + l ;
    posp = recp + 3 ;
    if ( memcmp(term, lastterm, l ) == 0 ) { /* (term = lastterm) */
      if ( memcmp(recp, lastrecp, 3) == 0 ) { /* rec = lastrec */
	/* Just write pos field unless it was all Fs */
	if ( memcmp(posp, inf, 4) != 0 ) {
	  if ( fwrite(posp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	    goto w_error ;
	  outposes++ ;
	  outchars += 4 ;
	}
	else infposes++ ;
      }
      else {			/* terms same, recs not, terminate prev pstg */
	if ( fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 4 ;
	/* Start new posting */
	if ( fwrite(recp, (size_t)3, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 3 ; outrecs++ ;
	if ( memcmp(posp, inf, 4) != 0 ) {
	  if ( fwrite(posp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	    goto w_error ;
	  outchars += 4 ;
	  outposes++ ;
	}
	else infposes++ ;
      }
    } /* term = lastterm */
    else {			/* Different term */
      if ( count > 0 ) {	/* Don't do on first term */
	if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;	/* Terminate prev pstg */
	outchars += 4 ;
	if (fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;
				/* Terminate prev record */
	outchars += 3 ;
      } /* count > 0 */
      /* Write out new term and pstg and 1st pos */
      if (fwrite(term, (size_t)(l + 3), (size_t)1, ofp) != (size_t)1)
	goto w_error ;
      outchars += l + 3 ;
      outterms++ ;
      outrecs++ ;
      if ( memcmp(posp, inf, 4) != 0 ) {
	if ( fwrite(posp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 4 ;
	outposes++ ;
      }
      else infposes++ ;
    } /* Different term */    
    lastterm = term ; lastrecp = recp ; lastposp = posp ;
  } /* top for loop */
  /* Finish last record */
  if ( count > 0 ) {	/* Don't do on first term */
    if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
      goto w_error ;
				/* Terminate posting. Other itypes
				   don't seem to need this at end */
    if (fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1)
      goto w_error;		/*Terminate term's postings*/
    outchars += 3 ;
  } /* count > 0 */
  
  fclose(ofp) ;
  if ( ! silent ) {
    if ( ! dummy ) 
      fprintf(stderr,
  "\nWritten output file %s.\nDistinct terms: %d, postings: %d, pos flds: %d",
	    fnames, outterms, outrecs, outposes) ;
    else
      fprintf(stderr,
	      "\nWritten dummy output file.\n\
Distinct terms: %d, postings: %d, pos flds: %d",
	      outterms, outrecs, outposes) ;
    if ( infposes )
      fprintf(stderr, "\n%d infinite positional records discarded", infposes) ;
  }
  return outchars ;

o_error:
  fprintf(stderr, "%s: can't open output file %s\n", func, fnames) ;
  return -2 ;

w_error:
  fprintf(stderr, "%s: write failed on %s after %d chars\n",
	  func, fnames, outchars) ;
  return -1 ;
}

/*****************************************************************************
  
  outrun_12() - sort and output a run - index types 12 and 13

  Same as outrun_4() except recnum is 4-byte instead of 3

  Input: <term><recnum><pos> where <pos> is a Pos_text for type 12 or a
             v      4      4
   Pos6 for type 13

  Output: <term>[<recnum>[<pos>]FFFFFFFF]FFFFFFFF
  
  A posting ends with a <pos> field containing FFFFFFFF
  An entire record ends with a <recnum> field containing FFFFFFFF

*****************************************************************************/

int
outrun_12(STRINGS *sorttab, char *fnames, int dflg,
	 BOOL silent,
	 BOOL dummy		/* If TRUE throws output away */
	 )
{

  FILE *ofp ;
  u_char *term, *lastterm, *recp, *lastrecp, *posp, *lastposp ;

  int l, count ;
  int outterms = 0, outrecs = 0, outposes = 0 ;
  int outchars = 0 ;

  int infposes = 0 ;

  static char *func = "outrun_12()" ;

  if ( ! silent ) fprintf(stderr, "\nSorting %d keys...", sorttab->n) ;
  gqsort( (char *) sorttab->p, sorttab->n, sizeof(char *),
	  (int (*)(void *, void *))compitr_12) ;
  if ( ! silent ) fprintf(stderr, " sorted") ;
  if ( dummy ) {
    ofp = fopen("/dev/null", "wb") ;
  }
  else {
    ofp = fopen(fnames, "wb") ;
    if ( ofp == NULL ) {
      fprintf(stderr, "\nCan't open temp output file %s", fnames) ;
      goto o_error ;
    }
  }
  putw(MAGIC_MERGEFILE, ofp) ;

  lastterm = (u_char *) inf ; lastrecp = (u_char *) inf ;
  lastposp = (u_char *) inf ;
  for ( count = 0 ; count < sorttab->n ; count++ ) {
    term = (u_char *) sorttab->p[count] ;
    l = len(term) + 1 ;
    recp = term + l ;
    posp = recp + 4 ;
    if ( memcmp(term, lastterm, l ) == 0 ) { /* (term = lastterm) */
      if ( memcmp(recp, lastrecp, 4) == 0 ) { /* rec = lastrec */
	/* Just write pos field unless it was all Fs */
	if ( memcmp(posp, inf, 4) != 0 ) {
	  if ( fwrite(posp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	    goto w_error ;
	  outposes++ ;
	  outchars += 4 ;
	}
	else infposes++ ;
      }
      else {			/* terms same, recs not, terminate prev pstg */
	if ( fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 4 ;
	/* Start new posting */
	if ( fwrite(recp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 4 ; outrecs++ ;
	if ( memcmp(posp, inf, 4) != 0 ) {
	  if ( fwrite(posp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	    goto w_error ;
	  outchars += 4 ;
	  outposes++ ;
	}
	else infposes++ ;
      }
    } /* term = lastterm */
    else {			/* Different term */
      if ( count > 0 ) {	/* Don't do on first term */
	if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;	/* Terminate prev pstg */
	outchars += 4 ;
	if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;
				/* Terminate prev record */
	outchars += 4 ;
      } /* count > 0 */
      /* Write out new term and pstg and 1st pos */
      if (fwrite(term, (size_t)(l + 4), (size_t)1, ofp) != (size_t)1)
	goto w_error ;
      outchars += l + 4 ;
      outterms++ ;
      outrecs++ ;
      if ( memcmp(posp, inf, 4) != 0 ) {
	if ( fwrite(posp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 4 ;
	outposes++ ;
      }
      else infposes++ ;
    } /* Different term */    
    lastterm = term ; lastrecp = recp ; lastposp = posp ;
  } /* top for loop */
  /* Finish last record */
  if ( count > 0 ) {	/* Don't do on first term */
    if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
      goto w_error ;
				/* Terminate posting */
    if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
      goto w_error;		/*Terminate term's postings*/
    outchars += 4 ;
  } /* count > 0 */
  
  fclose(ofp) ;
  if ( ! silent ) {
    if ( ! dummy ) 
      fprintf(stderr,
  "\nWritten output file %s.\nDistinct terms: %d, postings: %d, pos flds: %d",
	    fnames, outterms, outrecs, outposes) ;
    else
      fprintf(stderr,
	      "\nWritten dummy output file.\n\
Distinct terms: %d, postings: %d, pos flds: %d",
	      outterms, outrecs, outposes) ;
    if ( infposes )
      fprintf(stderr, "\n%d infinite positional records discarded", infposes) ;
  }
  return outchars ;

o_error:
  fprintf(stderr, "%s: can't open output file %s\n", func, fnames) ;
  return -2 ;

w_error:
  fprintf(stderr, "%s: write failed on %s after %d chars\n",
	  func, fnames, outchars) ;
  return -1 ;
}

/*****************************************************************************
  
  outrun_10() - sort and output a run - index type 10

  Input: <term><recnum>
             v      3  

  Output: <term>[<recnum><tf>]FFFFFF
             v       3    2
  <recnum> = FFFFFF marks end of term record	     

  Aug 97: new for VLC in TREC-6  SW

  Closely based on outrun_4()

*****************************************************************************/

int
outrun_10(STRINGS *sorttab, char *fnames, int dflg,
	  BOOL silent, BOOL dummy)
{

  FILE *ofp ;
  unsigned short tf = 0 ;
  u_char *term, *lastterm, *recp, *lastrecp ;

  int l, count ;
  int outterms = 0, outrecs = 0, outposes = 0 ;
  int outchars = 0 ;


  static char *func = "outrun_10()" ;

  if ( ! silent ) fprintf(stderr, "\nSorting %d keys...", sorttab->n) ;
  gqsort( (char *) sorttab->p, sorttab->n, sizeof(char *),
	  (int (*)(void *, void *))compitr_10) ;
  if ( ! silent ) fprintf(stderr, " sorted") ;
  if ( dummy ) {
    ofp = fopen("/dev/null", "wb") ;
  }
  else {
    ofp = fopen(fnames, "wb") ;
    if ( ofp == NULL ) {
      fprintf(stderr, "\nCan't open temp output file %s", fnames) ;
      goto o_error ;
    }
  }
  putw(MAGIC_MERGEFILE, ofp) ;
  lastterm = (u_char *) inf ; lastrecp = (u_char *) inf_3 ;
  for ( count = 0 ; count < sorttab->n ; count++ ) {
    term = (u_char *) sorttab->p[count] ;
    l = len(term) + 1 ;
    recp = term + l ;
    if ( memcmp(term, lastterm, l ) == 0 ) { /* (term = lastterm) */
      if ( memcmp(recp, lastrecp, 3) == 0 ) { /* rec = lastrec */
	/* Increment tf */
	if ( tf < 32767 ) {
	  tf++ ; outposes++ ;
	}
      }
      else {			/* terms same, recs not, terminate prev pstg */
	/* Write tf */
	if ( fwrite((char *)&tf, (size_t)sizeof(short), (size_t)1, ofp ) !=
	     (size_t)1 )
	  goto w_error ;
	outchars += sizeof(short) ;
	/* Start new posting for current term */
	if ( fwrite(recp, (size_t)3, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 3 ; outrecs++ ;
	tf = 1 ;
	outposes++ ;
      } /* terms same, recs not */
    } /* term = lastterm */
    else {			/* Different term */
      if ( count > 0 ) {	/* Don't do on first term */
	/* Write tf and terminate prev record */
	if ( fwrite((char *)&tf, (size_t)sizeof(short), (size_t)1, ofp ) !=
	     (size_t)1 )
	  goto w_error ;
	outchars += sizeof(short) ;
	if (fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;
	outchars += 3 ;
      } /* count > 0 */
      /* Write out new term and rec */
      if (fwrite(term, (size_t)(l + 3), (size_t)1, ofp) != (size_t)1)
	goto w_error ;
      outchars += l + 3 ;
      outterms++ ; outrecs++ ;
      tf = 1 ;
      outposes++ ;
    } /* Different term */    
    lastterm = term ; lastrecp = recp ;
  } /* top for loop */
  /* Finish last record */
  if ( count > 0 ) {	/* Don't do on first term */
    /* Write tf and terminate record */
    if ( fwrite((char *)&tf, (size_t)sizeof(short), (size_t)1, ofp ) !=
	 (size_t)1 )
      goto w_error ;
    outchars += sizeof(short) ;
    if (fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1)
      goto w_error ;
    outchars += 3 ;
  } /* count > 0 */
  fclose(ofp) ;
  if ( ! silent ) {
    if ( ! dummy ) 
      fprintf(stderr,
  "\nWritten output file %s.\nDistinct terms: %d, postings: %d, term freq: %d",
	    fnames, outterms, outrecs, outposes) ;
    else
      fprintf(stderr,
	      "\nWritten dummy output file.\n\
Distinct terms: %d, postings: %d, term freq: %d",
	      outterms, outrecs, outposes) ;
  }
  return outchars ;

o_error:
  fprintf(stderr, "%s: can't open output file %s\n", func, fnames) ;
  return -2 ;

w_error:
  fprintf(stderr, "%s: write failed on %s after %d chars\n",
	  func, fnames, outchars) ;
  return -1 ;
}

/*****************************************************************************
  
  outrun_20() - sort and output a run - index type 20

  Input: <term><recnum><wt><qtf>
            v     3     4    1

  Output: <term>[<recnum><wt><qtf>]FFFFFF

  <recnum> = FFFFFF marks end of term record	     

*****************************************************************************/

int
outrun_20(STRINGS *sorttab, char *fnames, int dflg,
	  BOOL silent, BOOL dummy)
{

  FILE *ofp ;
  u_char *term, *lastterm ;
  u_char *recp ;
  int l, count ;
  int outterms = 0, outrecs = 0, outchars = 0 ;

  static char *func = "outrun_20()" ;

  if ( ! silent ) fprintf(stderr, "\nSorting %d keys...", sorttab->n) ;
  gqsort( (char *) sorttab->p, sorttab->n, sizeof(char *),
	  (int (*)(void *, void *))compitr_10) ;
  if ( ! silent ) fprintf(stderr, " sorted") ;
  if ( dummy ) {
    ofp = fopen("/dev/null", "wb") ;
  }
  else {
    ofp = fopen(fnames, "wb") ;
    if ( ofp == NULL ) {
      fprintf(stderr, "\nCan't open temp output file %s", fnames) ;
      goto o_error ;
    }
  }
  putw(MAGIC_MERGEFILE, ofp) ;
  lastterm = (u_char *) inf ; 
  for ( count = 0 ; count < sorttab->n ; count++ ) {
    term = (u_char *) sorttab->p[count] ;
    l = len(term) + 1 ;
    recp = term + l ;
    if ( memcmp(term, lastterm, l ) == 0 ) { /* (term = lastterm) */
      /* Write rec + wt + qtf */
      if ( fwrite((void *)recp, (size_t)8, (size_t)1, ofp ) != (size_t)1 )
	goto w_error ;
      outchars += 8 ;
      outrecs++ ;
    } /* term = lastterm */
    else {			/* Different term */
      if ( count > 0 ) {	/* Don't do on first term */
	/* Terminate prev record */
	if (fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;
	outchars += 3 ;
      } /* count > 0 */
      /* Write out new term */
      if (fwrite(term, (size_t)l, (size_t)1, ofp) != (size_t)1)
	goto w_error ;
      outchars += l ;
      outterms++ ;
      /* Write rec + wt + qtf */
      if ( fwrite((void *)recp, (size_t)8, (size_t)1, ofp ) != (size_t)1 )
	goto w_error ;
      outchars += 8 ;
      outrecs++ ;
    } /* Different term */    
    lastterm = term ;
  } /* top for loop */
  /* Finish last record */
  if ( count > 0 ) {	/* Don't do on first term */
    /* Terminate record */
    if (fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1)
      goto w_error ;
    outchars += 3 ;
  } /* count > 0 */
  fclose(ofp) ;
  if ( ! silent ) {
    if ( ! dummy ) 
      fprintf(stderr,
  "\nWritten output file %s.\nDistinct terms: %d, postings: %d",
	    fnames, outterms, outrecs) ;
    else
      fprintf(stderr,
	      "\nWritten dummy output file.\n\
Distinct terms: %d, postings: %d",
	      outterms, outrecs) ;
  }
  return outchars ;

o_error:
  fprintf(stderr, "%s: can't open output file %s\n", func, fnames) ;
  return -2 ;

w_error:
  fprintf(stderr, "%s: write failed on %s after %d chars\n",
	  func, fnames, outchars) ;
  return -1 ;
}
/*****************************************************************************
  
  outrun_11() - sort and output a run - index type 11

  Same as 10 except 4-byte recnum

  Input: <term><recnum>
             v      4  

  Output: <term>[<recnum><tf>]FFFFFFFF
             v       4    2
  <recnum> = FFFFFFFF marks end of term record	     

  July 98: new for VLC in TREC-7  SW

*****************************************************************************/

int
outrun_11(STRINGS *sorttab, char *fnames, int dflg,
	  BOOL silent, BOOL dummy)
{

  FILE *ofp ;
  unsigned short tf = 0 ;
  u_char *term, *lastterm, *recp, *lastrecp ;

  int l, count ;
  int outterms = 0, outrecs = 0, outposes = 0 ;
  int outchars = 0 ;


  static char *func = "outrun_11()" ;

  if ( ! silent ) fprintf(stderr, "\nSorting %d keys...", sorttab->n) ;
  gqsort( (char *) sorttab->p, sorttab->n, sizeof(char *),
	  (int (*)(void *, void *))compitr_11) ;
  if ( ! silent ) fprintf(stderr, " sorted") ;
  if ( dummy ) {
    ofp = fopen("/dev/null", "wb") ;
  }
  else {
    ofp = fopen(fnames, "wb") ;
    if ( ofp == NULL ) {
      fprintf(stderr, "\nCan't open temp output file %s", fnames) ;
      goto o_error ;
    }
  }
  putw(MAGIC_MERGEFILE, ofp) ;
  lastterm = (u_char *) inf ; lastrecp = (u_char *) inf ;
  for ( count = 0 ; count < sorttab->n ; count++ ) {
    term = (u_char *) sorttab->p[count] ;
    l = len(term) + 1 ;
    recp = term + l ;
    if ( memcmp(term, lastterm, l ) == 0 ) { /* (term = lastterm) */
      if ( memcmp(recp, lastrecp, 4) == 0 ) { /* rec = lastrec */
	/* Increment tf */
	if ( tf < 32767 ) {
	  tf++ ; outposes++ ;
	}
      }
      else {			/* terms same, recs not, terminate prev pstg */
	/* Write tf */
	if ( fwrite((char *)&tf, (size_t)sizeof(short), (size_t)1, ofp ) !=
	     (size_t)1 )
	  goto w_error ;
	outchars += sizeof(short) ;
	/* Start new posting for current term */
	if ( fwrite(recp, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	  goto w_error ;
	outchars += 4 ; outrecs++ ;
	tf = 1 ;
	outposes++ ;
      } /* terms same, recs not */
    } /* term = lastterm */
    else {			/* Different term */
      if ( count > 0 ) {	/* Don't do on first term */
	/* Write tf and terminate prev record */
	if ( fwrite((char *)&tf, (size_t)sizeof(short), (size_t)1, ofp ) !=
	     (size_t)1 )
	  goto w_error ;
	outchars += sizeof(short) ;
	if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
	  goto w_error ;
	outchars += 4 ;
      } /* count > 0 */
      /* Write out new term and rec */
      if (fwrite(term, (size_t)(l + 4), (size_t)1, ofp) != (size_t)1)
	goto w_error ;
      outchars += l + 4 ;
      outterms++ ; outrecs++ ;
      tf = 1 ;
      outposes++ ;
    } /* Different term */    
    lastterm = term ; lastrecp = recp ;
  } /* top for loop */
  /* Finish last record */
  if ( count > 0 ) {	/* Don't do on first term */
    /* Write tf and terminate record */
    if ( fwrite((char *)&tf, (size_t)sizeof(short), (size_t)1, ofp ) !=
	 (size_t)1 )
      goto w_error ;
    outchars += sizeof(short) ;
    if (fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1)
      goto w_error ;
    outchars += 4 ;
  } /* count > 0 */
  fclose(ofp) ;
  if ( ! silent ) {
    if ( ! dummy ) 
      fprintf(stderr,
  "\nWritten output file %s.\nDistinct terms: %d, postings: %d, term freq: %d",
	    fnames, outterms, outrecs, outposes) ;
    else
      fprintf(stderr,
	      "\nWritten dummy output file.\n\
Distinct terms: %d, postings: %d, term freq: %d",
	      outterms, outrecs, outposes) ;
  }
  return outchars ;

o_error:
  fprintf(stderr, "%s: can't open output file %s\n", func, fnames) ;
  return -2 ;

w_error:
  fprintf(stderr, "%s: write failed on %s after %d chars\n",
	  func, fnames, outchars) ;
  return -1 ;
}



/*****************************************************************************
  
  Merge - index types 1 - ?

  Perform one merge pass. 
  
  Returns number of characters written or - on error

  July 92: tried to trap write errors (would normally be due to no space).

*****************************************************************************/
#define REFILL 1
#define TERM 2
#define DONE 0

#define NUMFILES 256		/* May well not work over 127 on some O/Ss */

extern int silent ;

OFFSET_TYPE
imerge(int numruns, FILE **ifps, FILE *ofp, int itype)
{
  char *tib ;			/* Term input processing buffer, allocated. */
  char *tbuf[NUMFILES] ;	/* Term input processing buffers */
  char miscbuf[512] ;		/* Read buffer for profile data */
  char *minterm ;		/* Addr of save area for least term in a pass*/
  int minlength ;
  u_int rec = 0 ;		/* Save area for unit posting read.
				   Initialisation essential for itype 4-  */
  u_int prevrec = 0 ;
  u_int pos ;

  unsigned short tf ;
  int st[NUMFILES] ;		/* Current input buffer status (REFILL, TERM,
				   DONE) */

  char prevterm[MAXEXITERMLEN + 2] ;
  int moreterms, minbuf ;
  int i, ch ;
  OFFSET_TYPE outchars = 0 ;
  unsigned magic ;
  int interms=0, outterms=0, outpstgs=0 ;

  int large_rn = (itype == 11)||(itype == 12)||(itype == 13) ;

  *prevterm = '\0' ;
  if ( itype < 4 || (itype > 13 && itype != 20) ) {
    fprintf(stderr,
	    "\nCan't process index type %d, only 4 - 13 and 20", itype) ;
    return (OFFSET_TYPE)-1 ;
  }
  tib = malloc((numruns + 1) * (MAXEXITERMLEN + 2)) ;
  if ( tib == NULL ) {
    fprintf(stderr, "\nimerge(): out of memory creating input buffers") ;
    return (OFFSET_TYPE)-1 ;
  }
    
  for ( i = 0 ; i < numruns ; i++ ) {
    tbuf[i] = tib + i * (MAXEXITERMLEN + 2);
    if ( (magic = getw(ifps[i])) != MAGIC_MERGEFILE ) 
      if ( ! silent )
	fprintf(stderr,
		"Warning: incorrect magic number %08x in stream %d\n",
		magic, i) ;
    st[i] = REFILL ;
  }
  putw(MAGIC_MERGEFILE, ofp) ;
  moreterms = 1 ;
  
  while ( moreterms ) {
    /* Find least term */
    for ( i = 0, moreterms = 0 ; i < numruns ; i++ ) {
      if ( st[i] == DONE ) continue ;
      if (st[i] == REFILL) {
	if ( (ch = getc(ifps[i])) == EOF ) {
	  st[i] = DONE ;
	  continue ;
	}
	else {
	  *tbuf[i] = ch ;
	  fread(tbuf[i] + 1, (size_t)len(tbuf[i]), (size_t)1, ifps[i] ) ;
	  st[i] = TERM ;
	  interms++ ;
	}
      }
      if ( ! moreterms ) {
	minbuf = i ; moreterms = 1 ;
      }
      else if (cpbytes(tbuf[i] + 1, len(tbuf[i]),
		       tbuf[minbuf] + 1, len(tbuf[minbuf]))< 0)
	minbuf = i ;
    }
    if ( ! moreterms ) break ; /* End of an output run */
    minterm = tbuf[minbuf] ;
    minlength = len(minterm) + 1 ;
    if ( strncmp(minterm + 1, prevterm, minlength - 1 < 0 ) ) {
      fprintf(stderr, "merge(): terms %.*s:%s out of sequence\n",
	      minlength - 1, minterm + 1, prevterm) ;
      goto o_error ;
    }
    strncpy(prevterm, minterm + 1, minlength - 1) ;
    *(prevterm + minlength - 1) = '\0' ;
    /* Write out minterm */
    if ( fwrite(minterm, (size_t)minlength, (size_t)1, ofp) != (size_t)1)
      goto m_error ;
    outchars += minlength ;
    outterms++ ;

    /* Check for equal terms in input buffers & output all the postings */
    prevrec = 0 ;
    for ( i = minbuf ; i < numruns ; i++ ) {
      /* Extra cond next line May 99 to avoid UMR */
      if ( st[i] == TERM && len(tbuf[i]) == len(minterm) &&
	   ! memcmp(tbuf[i], minterm, minlength) ) {
	/* Read and write out the postings */
	while ( TRUE ) {
	  if ( ! large_rn ) {
#if HILO
	    fread((char *)&rec + 1, (size_t)3, (size_t)1, ifps[i]) ;
#else
	    fread((char *)&rec, (size_t)3, (size_t)1, ifps[i]) ;
#endif
	    if ( rec == 0x00ffffff ) { /* End of postings for this term
					  in this stream */
	      st[i] = REFILL ;
	      break ;
	    }
	  }
	  else {
	    fread((char *)&rec, (size_t)4, (size_t)1, ifps[i]) ;
	    if ( rec == 0xffffffff ) { /* End of postings for this term
					  in this stream */
	      st[i] = REFILL ;
	      break ;
	    }
	  }
	  if ( rec <= prevrec ) {
	    fprintf(stderr,
		    "merge(): recnums %u:%u out of sequence, stream %d\n",
		    rec, prevrec, i) ;
	    goto o_error ;
	  }
	  if ( ! large_rn ) {
#if HILO
	    if (fwrite((char *)&rec + 1, (size_t)3, (size_t)1, ofp) !=
		(size_t)1)
	      goto m_error ;
#else
	    if (fwrite((char *)&rec, (size_t)3, (size_t)1, ofp) != (size_t)1)
	      goto m_error ;
#endif
	    outchars += 3 ;
	  }
	  else {
	    if (fwrite((char *)&rec, (size_t)4, (size_t)1, ofp) !=
		(size_t)1)
	      goto m_error ;
	    outchars += 4 ;
	  }
	  prevrec = rec ;
	  switch ( itype ) {
	  case 10:
	  case 11:
	    fread((void *)&tf, (size_t)sizeof(short), (size_t)1, ifps[i]) ;
	    if ( fwrite((void *)&tf, (size_t)sizeof(short), (size_t)1, ofp) !=
		 (size_t)1 )
	      goto m_error ;
	    outchars += sizeof(short) ;
	    break ;
	  case 20:
	    fread((void *)miscbuf, sizeof(float) + sizeof(u_char), 1, ifps[i]);
	    if ( fwrite((void *)miscbuf, sizeof(float) + sizeof(u_char),
			1, ofp) != 1 ) 
	      goto m_error ;
	    outchars += sizeof(float) + sizeof(u_char) ;
	    break ;
	  default:
	    do {
	      pos = (u_int) getw(ifps[i]) ;
	      /* Removed error checking as win32 _putw() returns the int
		 but unix putw returns 0 (Sep 00) */
	      putw((int)pos, ofp) ;
	      outchars += 4 ;
	    } while ( pos != 0xffffffff ) ;
	  }
	  outpstgs++ ;
	}
      } /* All pstgs for this term in this stream done */
    } /* All pstgs for this term done */
    /* Terminate this term's postings */
    if ( ! large_rn ) {
      if ( fwrite(inf_3, (size_t)3, (size_t)1, ofp) != (size_t)1 )
	goto m_error ;
      outchars += 3 ;
    }
    else {
      if ( fwrite(inf, (size_t)4, (size_t)1, ofp) != (size_t)1 )
	goto m_error ;
      outchars += 4 ;
    }
  }				/* (while (moreterms)) */
  free(tib) ;
  if ( ! silent ) 
    fprintf(stderr,
	    "\nMerge done - %d terms in; %d terms out, %d postings out",
	    interms, outterms, outpstgs) ;
  return outchars ;

 m_error:
  if ( sizeof(OFFSET_TYPE) == 8 )
#ifdef _WIN32
    fprintf(stderr, "\nMerge failed on output file write after %I64d chars",
#else
    fprintf(stderr, "\nMerge failed on output file write after %lld chars",
#endif
	    (LL_TYPE)outchars) ;
  else
    fprintf(stderr, "\nMerge failed on output file write after %d chars",
	    (int)outchars) ;
  perror(NULL) ;
 o_error:
  free(tib) ;
  return (OFFSET_TYPE)-1 ;
}

OFFSET_TYPE
merge(int first, int num,	/* first suffix, number of runs to do */
      int outsuffix,		/* Suffix for output file */
      char *fnames, char *fnames1,
      BOOL finished,		/* If finished o/p to stdout */
      int itype, int del)	/* If true deletes input files at end */
{
  int suff ;
  FILE **ifps ;		/* Input files */
  FILE *ofp ;
  char fnstem[PATHNAME_MAX] ;		/* File names, suffix on Temp_dir */
  char fnstem1[PATHNAME_MAX] ;		/* File names, suffix on merge_temp_dir */
  char tempb[PATHNAME_MAX] ;

  int l ;
  register int i ;
  OFFSET_TYPE outchars = 0 ;

  if ( itype < 4 || (itype > 13 && itype != 20) ) {
    fprintf(stderr, "\nCan't process index type %d, only 4 - 13 and 20",
	    itype) ;
    return (OFFSET_TYPE)-1 ;
  }
  ifps = (FILE **)malloc(num * sizeof(FILE *)) ;
  if ( ifps == NULL ) {
  }
  for ( i = 0, suff = first ; i < num ; i++, suff++ ) {
    sprintf(fnstem, "%s%06d", fnames, suff) ;
    if ( ( ifps[i] = fopen(fnstem, "rb") ) == NULL ) {
      fprintf(stderr, "\nCan't open merge input file %s", fnstem) ;
      return (OFFSET_TYPE)-1 ;
    }
  }
  if ( ! finished ) {
    sprintf(fnstem1, "%s%06d", fnames1, outsuffix) ;
    ofp = fopen(fnstem1, "wb") ;
    if ( ofp == NULL) {
      fprintf(stderr, "\nCan't open merge output file %s", fnstem1) ;
      return (OFFSET_TYPE)-1 ;
    }
    if ( ! silent ) fprintf(stderr, "\nOpened merge output file %s", fnstem1) ;
  }
  else {
    ofp = stdout ;
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY) ;
#endif
  }
  outchars = imerge(num, ifps, ofp, itype) ;
  if ( outchars < 0 ) goto error ;

  if ( ! finished ) fclose(ofp) ;
  for ( i = 0, suff = first ; i < num ; i++, suff++ ) {
    fclose(ifps[i]) ;
    if ( del ) {
      sprintf(fnstem, "%s%06d", fnames, suff) ;
#ifndef NO_LN
      /* Check whether the filename was a symlink & if so delete the file. */
      if ( (l = readlink(fnstem, tempb, PATHNAME_MAX)) > 0 ) {
	*(tempb + l) = '\0' ;
	if ( unlink(tempb) == 0 ) {
	  if ( ! silent )
	    fprintf(stderr, "\nDeleted linked-to file %s", tempb) ;
	}
	else fprintf(stderr, "\nFailed to delete linked-to file %s", tempb) ;
      }
#endif
      if ( unlink(fnstem) == 0 ) {
	if ( ! silent ) fprintf(stderr, "\nDeleted %s", fnstem) ;
      }
      else fprintf(stderr, "\nFailed to delete %s", fnstem) ;
    }
  }
  free(ifps) ;
  /* If no links fnames1 must be the same as fname */
#ifndef NO_LN
  if ( ! finished ) {
    /* May 95: used to "mv" here, now symlink for speed */
    sprintf(fnstem, "%s%06d", fnames, outsuffix) ;
    if ( strcmp(fnstem, fnstem1) != 0 ) {
      if ( symlink(fnstem1, fnstem) == 0 ) {
	if ( ! silent ) fprintf(stderr, "\nCreated link %s for %s",
				fnstem, fnstem1) ;
      }
      else fprintf(stderr, "\nCan't make link %s to %s", fnstem, fnstem1) ;
    }
  } /* (not finished) */
#endif
  return outchars ;

 error:
  if ( ofp != stdout ) fclose(ofp) ;
  fprintf(stderr, "\nRetaining merge input files unchanged") ;
  for ( i = 0, suff = first ; i < num ; i++, suff++ ) 
    fclose(ifps[i]) ;
  return (OFFSET_TYPE)-1 ;
}

