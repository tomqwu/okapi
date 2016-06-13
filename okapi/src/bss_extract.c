/*****************************************************************************

  bss_extract.c  SW June 1992

  Functions for index and search term extraction and storage from text

*****************************************************************************/

#define EXTRACT

#include "bss.h"
#include "bss_errors.h"
#include "para.h"
#include "charclass.h"

#define TOKMAPSIZE 65536		/* Size for the records used for
					   putting source strings into the
					   "superparse" output */
#define WHAT 65

int mymladen()
{
	 fprintf(stderr, "Calling myfunc()");
	 return 0;
}

/******************************************************************************

  Returns number of terms (maybe 0), -1 if storage full, < -1 otherwise

  May 94: for type 4 at least setting positional record all Fs if there
  is overflow on sentence or token number. Note sentences are not allowed to
  reach theoretical max, otherwise we might get a "natural" ffffffff.
  These "dummy" pos recs are then discarded by outrun_4() (ix1_ex).

******************************************************************************/

static int
output_extracted_term(struct outrec *orec)
{
  register char *op ;
  register int tl = orec->tl ;
  int amount ;
  STRINGS *str ;
  int outflag = orec->type & ~EX_DIAG ;
  int diag = orec->type & EX_DIAG ;

  /* tl == 0 changed to tl <= 0 Nov 98 after crash on phrase "&" */
  if ( tl <= 0 ) return 0 ;
  if ( tl > MAXEXITERMLEN ) {
    tl = MAXEXITERMLEN ;
    if ( Dbg & D_EXTRACT )
      fprintf(bss_syslog, "Term %.*s truncated to %d bytes\n",
	      tl, orec->term, MAXEXITERMLEN) ;
  }
  if ( diag ) {
    fprintf(Err_file,
	    "%.*s: f=%d s=%d t=%d nt=%d sw=%d\n",
	    orec->tl, orec->term,
	    orec->fdnum, orec->senum, orec->wdnum,
	    orec->nt, orec->sw) ;
  }
  switch (outflag) {

  case EX_OP_IX4 :
  case EX_OP_IX12 :
    amount = tl + 1 + 3 + 4 ; /* 3-byte recnum, pos */
    if ( outflag & EX_LARGE_RN ) amount++ ;
    str = (STRINGS *) orec->output ;
    if ( str->used + amount > str->maxsize || str->n >= str->maxnum )
      return -1 ;

    /* Temp May 94 til worked out a proper system for coping with oflow */
    if ( orec->senum > TEXTPOS_MAX_S || orec->wdnum > TEXTPOS_MAX_T ) 
      orec->p.pos = 0xffffffff ;
    else {
      if ( orec->nt > TEXTPOS_MAX_NT )
	orec->nt = TEXTPOS_MAX_NT ;
      if ( orec->sw > TEXTPOS_MAX_SW )
	orec->sw = TEXTPOS_MAX_SW ;
#if HILO
      orec->p.pthl.t = orec->wdnum ; 
      orec->p.pthl.nt = orec->nt ;
      orec->p.pthl.sw = orec->sw ;
#else
      orec->p.ptlh.t = orec->wdnum ; 
      orec->p.ptlh.nt = orec->nt ;
      orec->p.ptlh.sw = orec->sw ;
#endif
      /* (the other two fields are constant over one call of extract(),
	 have already been filled in) */
    }
    op = str->data + str->used ;
    str->p[str->n++] = op ;
    *(u_char *)op++ = tl ;
    memcpy(op, orec->term, tl) ; op += tl ;
    if ( outflag & EX_LARGE_RN ) {
      memcpy(op, (void *)&orec->rec, 4) ;
      op += 4 ;
    }
    else {
#if HILO
      memcpy(op, (void *)((u_char *)&orec->rec + 1), 3) ; op += 3 ;
#else
      memcpy(op, (void *)((u_char *)&orec->rec), 3) ; op += 3 ;
#endif
    }
    memcpy(op, (void *)&orec->p, sizeof(U_pos)) ;
    str->used += amount ;
    return 1 ;
    break ;

  case EX_OP_IX5 :		/* Nearly same as IX4 */
  case EX_OP_IX13 :	
    amount = tl + 1 + 3 + 4 ; /* 3-byte recnum, pos */
    if ( outflag & EX_LARGE_RN ) amount++ ;
    str = (STRINGS *) orec->output ;
    if ( str->used + amount > str->maxsize || str->n >= str->maxnum )
      return -1 ;

    /* Temp May 94 til worked out a proper system for coping with oflow */
    if ( orec->senum > LONGPOS_MAX_S || orec->wdnum > LONGPOS_MAX_T ) 
      orec->p.pos = 0xffffffff ;
    else {
      if ( orec->nt > LONGPOS_MAX_NT )
	orec->nt = LONGPOS_MAX_NT ;
      if ( orec->sw > LONGPOS_MAX_SW )
	orec->sw = LONGPOS_MAX_SW ;
#if HILO
      orec->p.lphl.t = orec->wdnum ; 
      orec->p.lphl.nt = orec->nt ;
      orec->p.lphl.sw = orec->sw ;
#else
      orec->p.lplh.t = orec->wdnum ; 
      orec->p.lplh.nt = orec->nt ;
      orec->p.lplh.sw = orec->sw ;
#endif
    }
    op = str->data + str->used ;
    str->p[str->n++] = op ;
    *(u_char *)op++ = tl ;
    memcpy(op, orec->term, tl) ; op += tl ;
    if ( outflag & EX_LARGE_RN ) {
      memcpy(op, (void *)&orec->rec, 4) ;
      op += 4 ;
    }
    else {
#if HILO
      memcpy(op, (void *)((u_char *)&orec->rec + 1), 3) ; op += 3 ;
#else
      memcpy(op, (void *)((u_char *)&orec->rec), 3) ; op += 3 ;
#endif
    }
    memcpy(op, (void *)&orec->p, sizeof(U_pos)) ;
    str->used += amount ;
    return 1 ;
    break ;

  case EX_OP_IX10 :		/* Count tf only, no pos recs */
  case EX_OP_IX11:
    amount = tl + 1 + 3 ; /* 3 or 4-byte recnum, no pos */
    if ( outflag & EX_LARGE_RN ) amount++ ;
    str = (STRINGS *) orec->output ;
    if ( str->used + amount > str->maxsize || str->n >= str->maxnum )
      return -1 ;
    op = str->data + str->used ;
    str->p[str->n++] = op ;
    *(u_char *)op++ = tl ;
    memcpy(op, orec->term, tl) ; op += tl ;
    if ( outflag & EX_LARGE_RN ) {
      memcpy(op, (void *)&orec->rec, 4) ;
      op += 4 ;
    }
    else {
#if HILO
      memcpy(op, (void *)((u_char *)&orec->rec + 1), 3) ; op += 3 ;
#else
      memcpy(op, (void *)&orec->rec, 3) ; op += 3 ;
#endif
    }
    str->used += amount ;
    return 1 ;
    break ;

  case EX_OP_IX20 :		/* <term><wt><qtf><rec> */
    {
      amount = tl + 1 + 3 + sizeof(float) + 1 ; 
      str = (STRINGS *) orec->output ;
      if ( str->used + amount > str->maxsize || str->n >= str->maxnum )
	return -1 ;
      op = str->data + str->used ;
      str->p[str->n++] = op ;
      *(u_char *)op++ = tl ;
      memcpy(op, orec->term, tl) ; op += tl ;
#if HILO
      memcpy(op, (void *)((u_char *)&orec->rec + 1), 3) ; op += 3 ;
#else
      memcpy(op, (void *)&orec->rec, 3) ; op += 3 ;
#endif
      memcpy(op, (void *)&orec->wt, sizeof(float)) ;
      op += sizeof(float) ;
      *op++ = (u_char)orec->tf ;
      str->used += amount ;
      return 1 ;
    }
    break ;

    /* case EX_SUPER :
       Unused for a long time (Jan 02)
       Was for qe term extraction output. Mod Oct 93 to output
       <sgnum+32><term><gsclass> as single terminated string
       followed by <sourceterm> (only if present) as terminated string */
    
  case EX_USER_PARSE :
    op = orec->output ;
    memcpy(op, orec->term, tl) ;
    op += tl ;
    *op++ = '\0' ;
    orec->output = op ;
    orec->outlen += tl + 1 ;
    break ;

  case EX_USER_SUPERPARSE :
#if HILO
    orec->p.ptnthl.t = orec->wdnum ;
    orec->p.ptnthl.nt = orec->nt ;	/* Only interested in these two fields */
#else
    orec->p.ptntlh.t = orec->wdnum ;
    orec->p.ptntlh.nt = orec->nt ;	/* Only interested in these two fields */
#endif
    op = orec->output ;
    *op++ = orec->gsclass ;
    memcpy(op, orec->term, tl) ;
    op += tl ;
    *op++ = '\0' ;
    memcpy(op, (void *)&orec->p, sizeof(U_pos)) ;
    op += 4 ;
    orec->output = op ;
    orec->outlen += tl + 6 ;
    break ;

  case EX_USER_SUPERPARSE_NOSOURCE:
    /* No knowledge of buffer size */
    op = orec->output ;
    *op++ = orec->gsclass ;
    memcpy(op, orec->term, tl) ;
    op += tl ;
    *op++ = '\0' ;
    orec->output = op ;
    orec->outlen += tl + 2 ;
    break ;

  case EX_USER_HASH:
    hash_search((Hashtab *)orec->output, orec->term, tl, H_ADD, FALSE) ;
    break ;

  case 0:	  /* No output to be done (e.g. indexing with no_i true */
    return 1 ;

  default :
    return 0 ;
  }
  return 1 ;
}

/******************************************************************************

  extract()  SW June 92

  This function extracts terms from input and places the
  output in the specified place, together with associated data
  determined by the output_type. The args following output_type are only
  needed if the extraction is for the purposes of indexing (addr, limits),
  for highlighting or query expansion (fdnum).

  The function is for extracting from one "paragraph" of text, not more than
  one field of a record & not more than MAX_TEXT in length, preferably a single
  "sentence".

  Few checks are made, in the interest of speed. gsl, if needed, must be
  non-null;

  Return value: number of items extracted, -1 if output struct becomes or
  is full before processing complete, < -1 on some error

  Output

  

  Output types

  
  Jan 99: added int * arg stopped which is passed to and set/reset
  by phase2_token. Bodge to get round only first "sentence" of <....>
  was stopped.

  99: removed buf1 and old regimes. stem_phrase_token() is now buf2->buf2

  March 2000: Made buf2 and outtokens static malloc'd instead of on stack.
  (Possibly too large stack for some environments.)

  Apr 02: added outlen arg which will "return" the number of chars output
  except in indexing or hash cases.

******************************************************************************/

int
extract(char *input, int i_len,
	Extraction_regime reg, Gsl *gsl, int (*stem_fn)(char *, int, char *),
	void *output, int type,
	int rec, u_int limits, int fdnum, int senum, int tokens[],
	int *stopped, int *outlen) 
{
  int tlen, tlen1, wlen, alen ; /* Temp length */
  static char *buf2 = NULL ;
  static char *buf3 = NULL ;		/* Work buffers */
  char *termptr, *bp, *ip ;
  char *gsres ;
  int matchlen ;
  char *token ;
  char *posBuf;
  BOOL sources ;

  int wdnum = -1 ;

  int ores ;			/* output_extracted_term() result */
  int numkeys = 0 ;

  int start_offset ;
  struct outrec outrec ;

  /* Next fields only for stuff with positional info */

  static int *ott = (int *)NULL ;
  int *outtokens ;
  // mko begin
  // Dynamically allocating memory for the pos[]. 32K is too big often
  // for the stack, so we changed ito to be performed dynamically.
  posBuf = bss_malloc(sizeof(struct pos) * 32768);
  if (posBuf == NULL)
  {
    fprintf(stderr, "Memory shortage, for pos structure");
  }
  struct pos *pos = (void *)posBuf ;
  // struct pos pos[32768],  <-- TOO BIG!!
  struct pos *tok ;
  // mko end
  int nwords = 0 ;
  int nsw = 0 ;
  int nt = 0 ;
  int j ;
      
  static char *func = "extract" ;

  if ( buf2 == NULL ) buf2 = bss_malloc(MAX_TEXT) ;
  if ( buf3 == NULL ) buf3 = bss_malloc(MAX_TEXT) ;

  sources = (type & EX_SOURCE) ;
  if ( sources ) {
    if ( ott == (int *)NULL )
      ott = bss_malloc(MAX_TEXT * sizeof(int)) ;
    outtokens = ott ;
    tok = pos ;
  }
  else {
    outtokens = (int *)NULL ;
    tok = (struct pos *)NULL ;
  }

  if ( i_len > MAX_TEXT - 1 ) {
    sprintf(Err_buf, "truncating input %d to %d", i_len, MAX_TEXT) ;
    sys_err(func) ;
    i_len = MAX_TEXT - 1 ;
  }

  /* If extracting all phrases from text set senum = -1 */

  outrec.type = type ;
  outrec.rec = rec ;
  outrec.output = output ;
  outrec.outlen = 0 ;
  outrec.limits = limits ;
  if ( sources ) {
    outrec.fdnum = fdnum ;
    outrec.senum = senum ;
    outrec.wdnum = wdnum ;
#if HILO
    if ( type & (EX_IX4|EX_IX12) ) {
      outrec.p.pthl.f = fdnum ;
      outrec.p.pthl.s = senum ;
    }
    else if ( type & (EX_IX5|EX_IX13) ) {
      outrec.p.lphl.f = fdnum ;
      outrec.p.lphl.s = senum ;
    }
#else
    if ( type & (EX_IX4|EX_IX12) ) {
      outrec.p.ptlh.f = fdnum ;
      outrec.p.ptlh.s = senum ;
    }
    else if ( type & (EX_IX5|EX_IX13) ) {
      outrec.p.lplh.f = fdnum ;
      outrec.p.lplh.s = senum ;
    }
#endif
  } /* (if sources) */
  outrec.gsclass = ' ' ;	/* Default */

  switch (reg) {
    /* (Nov 99) Here removed the old words, title_words, name_words and
       surnames regimes */
       
  case words3:

    /* I think we want here nt, t etc put into ordinary vars, then tfred to the
       structs in the output function. OK done that (13 May 94) */

    tlen = phase2_token(input, i_len, buf2, TRUE, tokens, outtokens, stopped) ;
    tlen = stem_phrase_token(buf2, tlen, buf2, stem_fn, outtokens, tok) ; 
    /* Note stem_phrase will put a blank on the end */
    termptr = buf2 ;
    
    /* Extract terms  */
    while ( tlen > 0 ) {
      bp = lookup_gsl(termptr, tlen, gsl) ; 
      if ( bp != NULL ) {
	if ( sources ) {
	/* Count words matched so can determine number of tokens */
	/* Words after phase2 & stem_phrase are sep by single blank
	   and string ends with single blank. */
	  nwords = countchar(' ', (u_char *)bp + 1, len(bp)) ;
	  outrec.wdnum = tok->t ;
	  for ( j = 0, nt = 0 ; j < nwords ; j++, tok++) 
	    nt += tok->nt ;
	  outrec.nt = nt ;
	}
	outrec.gsclass = *(bp + len(bp) + 1) ; 
	if ( outrec.gsclass == 'S' ) {
	  ip = bp + len(bp) + 2 ; 
	  outrec.term = ip ; outrec.tl = 5 ; /* gsl token length */
	  if ( sources ) {
	    outrec.sw = nsw ; nsw = 0 ;
	  }
	  if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	  else if ( ores == -1 ) return -1 ;
	} /* gsclass S */
	else if ( outrec.gsclass == 'G' ||
		 (outrec.gsclass == 'I' && ! (type & BLURBY)) ||
		 /* 'I' skipped if field is TEXT (e.g. abstract) */
		 ((type & EX_SUPERPARSE) &&
		  (outrec.gsclass == 'H' || outrec.gsclass == 'F')) ) {
	  ip = bp + 1 ; 
	  outrec.term = ip ; outrec.tl = len(ip - 1) - 1 ;
	  if ( sources ) {
	    outrec.sw = nsw ; nsw = 0 ;
	  }
	  if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	  else if ( ores == -1 ) return -1 ;
	}
	else if ( outrec.gsclass == 'H' || outrec.gsclass == 'F' ) {
	  if ( sources ) nsw += nwords ;
	}
	/* any gsclass except P - Prefixes should already have been dealt
	   with by Dohyphs. (dohyphs() long unused -- Apr 02) */
	
	tlen -= len(bp) ;
	termptr += len(bp) ;
      } /* end of 'found in gsl' */
      else {
	outrec.gsclass = ' ' ;
	alen = extwd((u_char *)termptr, tlen, (u_char *)buf3, &wlen,
		     (u_char *)" ", (u_char *)"-'.", &start_offset) ;
	if (wlen > 0 ) { 
	  if ( sources ) {
	    outrec.wdnum = tok->t ;
	    outrec.nt = tok->nt ;
	    tok++ ;
	    outrec.sw = nsw ; nsw = 0 ;
	  }
	  ip = buf3 ;
	  outrec.term = ip ; outrec.tl = wlen ;
	  if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	  else if ( ores == -1 ) return -1 ;
	}
	tlen -= alen ;
	termptr += alen ;
      } /* not in gsl */
    } /* while something else to extract */
    break ;

  case literal_nc :
  case literal:
    memcpy(buf2, input, i_len) ;
    if ( reg == literal_nc ) cvuls(buf2, i_len) ;
    if ( sources ) {
      /* stem_phrase is simply to map the tokens */
      tlen = stem_phrase_token(buf2, i_len, buf2, nostem, tokens, tok) ; 
      // mko only decrement if tlen > 0, otherwise we set the value
      // below 0 which is unexpected low.
      if (tlen > 0)
      {
        tlen-- ;
      }
    }
    else {
      tlen = stws(buf2, i_len) ;
      /* No need when stem_phrase_token has been done (?) */
    }
    outrec.term = buf2 ;
    outrec.tl = tlen ;
    outrec.gsclass = ' ' ;
    if ( sources ) {
      if (tlen < 0)
      {
        fprintf(stderr, "\ntlen < 0 : this is unexpected!!\n");
      }
      nwords = countchar(' ', (u_char *)buf2, tlen) ;
      outrec.wdnum = tok->t ;
      for ( j = 0, nt = 0 ; j < nwords ; j++, tok++ ) nt += tok->nt ;
      outrec.nt = nt ;
      outrec.sw = 0 ;
    }
    if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
    else if ( ores == -1 ) return -1 ;
    break ;

  case phrase :
  case subject_heading :
  case title_phrase:
  case name_phrase:
  case corp_name_phrase:
  case personal_name:
    /* Produce a single item from the input */
    tlen = phase2_token(input, i_len, buf2, TRUE, tokens, outtokens, stopped) ;
    tlen = stem_phrase_token(buf2, tlen, buf2, stem_fn, outtokens, tok) ; 
    gsres = do_gsl(buf2, tlen, gsl, &outrec.gsclass, &matchlen, &token) ;
    if ( gsres != NULL && tlen == matchlen ) {
      nwords = countchar(' ', (u_char *)gsres + 1, len(gsres)) ;
      if ( sources ) {
	outrec.wdnum = tok->t ;
	for ( j = 0, nt = 0 ; j < nwords ; j++, tok++ ) nt += tok->nt ;
	outrec.nt = nt ;
	outrec.sw = nsw ; nsw = 0 ;
      }
      if ( outrec.gsclass == 'S' ) {
	outrec.term = token ; outrec.tl = 5 ;
	if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	else if ( ores == -1 ) return -1 ;
      }
      else if ( outrec.gsclass == 'H' || outrec.gsclass == 'F' ) {
	nsw += nwords ;
      }
      /* else ignore gsl */
    }
    else {			/* not GSL */
      outrec.term = buf2 ; outrec.tl = tlen - 1 ; outrec.gsclass = ' ' ;
      if ( sources ) {
	nwords = countchar(' ', (u_char *)buf2, tlen) ;
	outrec.wdnum = tok->t ;	
	for ( j = 0, nt = 0 ; j < nwords ; j++, tok++ ) nt += tok->nt ;
	outrec.nt = nt ;
	outrec.sw = nsw ; nsw = 0 ;
      }
      if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
      else if ( ores == -1 ) return -1 ;
    }
    break ;

  case lines:
    /* This simply outputs a line (without the linefeed)
       May 02: can't deal with sources at present (superparse) */
    {
      int ch ;
      while ( i_len > 0 ) {
	tlen = 0 ;
	while ( i_len > 0 && tok_space(*input) ) {
	  i_len-- ;
	  input++ ;
	}
	outrec.term = input ;
	while ( i_len-- > 0 && (ch = *input++) != '\n' ) tlen++ ;
	if ( tlen > 0 ) {
	  outrec.tl = tlen ;
	  if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	  else if ( ores == -1 ) return -1 ;
	}
      }
    }
    break ;

  case profile:
    /* Looks for !<wt>!<qtf> in input */
    {
      int ch ;
      while ( i_len > 0 ) {
	/* Remove leading space */
	while ( i_len > 0 && tok_space(*input) ) {
	  i_len-- ;
	  input++ ;
	}
	tlen = 0 ;
	outrec.term = input ;
	while ( i_len-- > 0 && (ch = *input++) != '!' ) tlen++ ;
	outrec.tl = tlen ;
	sscanf(input, "%f!%d\n", &outrec.wt, &outrec.tf) ;
	if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	else if ( ores == -1 ) return -1 ;
	while ( i_len-- > 0 && (ch = *input++) != '\n' ) ;
      }
    }
    break ;

  case Dewey :
    /* Here we want "words" consisting of 3 digits poss foll by . and more
       digits. A defective word is just skipped. */

    /* See literal case for use of stem_phrase */
    tlen = stem_phrase_token(input, i_len, buf2, nostem, tokens, tok) ;
    ip = buf2 ; 
    while ( tlen > 0 ) {
      tlen1 = extwd((u_char *)ip, tlen, (u_char *)buf3, &wlen, (u_char *)" ",
		    (u_char *)".", &start_offset) ;
      /* If not nnn, nnn. or nnn.n[n] returns <= 0
	 Gets rid of e.g. spine letters (but only if blank in between) */
      if ( isdewey(buf3, wlen) > 0 ) { 
	if ( sources ) {
	  outrec.wdnum = tok->t ;
	  outrec.nt = tok->nt ;
	  outrec.sw = nsw ; nsw = 0 ;
	}
	outrec.term = buf3 ; outrec.tl = wlen ;
	if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	else if ( ores == -1 ) return -1 ;
      }
      tlen -= tlen1 ; ip += tlen1 ;
      tok++ ;
    }
    break ;
    
  case udc :
    /* See literal case for use of stem_phrase */
    tlen = stem_phrase_token(input, i_len, buf2, nostem, tokens, tok) ;
    ip = buf2 ; 
    while ( tlen > 0 ) {
      tlen1 = extwd((u_char *)ip, tlen, (u_char *)buf3, &wlen,
		    (u_char *)" ", (u_char *)".():[]", &start_offset) ;
      /* if ( isudc(buf3, wlen) > 0 ) Not written */ {  
	if ( sources ) {
	  outrec.wdnum = tok->t ;
	  outrec.nt = tok->nt ;
	  outrec.sw = nsw ; nsw = 0 ;
	}
	outrec.term = buf3 ; outrec.tl = wlen ;
	if ((ores = output_extracted_term(&outrec)) > 0 ) numkeys += ores ;
	else if ( ores == -1 ) return -1 ;
      }
      tlen -= tlen1 ; ip += tlen1 ;
      tok++ ;
    }
    break ;
    
  default :
    sprintf(Err_buf, "can't process regime %d", reg) ;
    sys_err(func) ;
    return 0 ;
  }
  *outlen = outrec.outlen ;

  bss_free(posBuf);
  /* fprintf(stderr, "extract() output %d\n", outrec.outlen) ;*/
  return numkeys ;
}


/******************************************************************************

  setup_ixparms  SW June 92

  Fills in global ixparms struct from Search_groups

  Used by ix1 only, should be in ixsubs?

  June 97: added call of setup_gsl_sg(), and return value.
  Returns 0 or 1 if gsl set up OK, otherwise -

******************************************************************************/

/* June 97: no longer needs 2-dim array, and num_ixparms is 1 or 0 */
struct ixparms ixparms[MAXNUMFLDS + 1][1] ;/* The [1] was the obsolete MAXBEASTS */
int num_ixparms[MAXNUMFLDS + 1] ; /* Number of parms for this field */

int
setup_ixparms(int ixnum)
{
  int fd ;
  short *fds ;
  Extraction_regime reg ;
  Gsl *gsl ;
  int (*stem_fn) () ;
  int n ;
  int j, k ;
  int result = 0 ;
  Sg *sg ;
  BOOL found ;

  for ( fd = 1 ; fd <= Cdb.nf ; fd++ ) {
    num_ixparms[fd] = 0 ;
    /* For each search group for this index .. */
    /* (June 97) there's now only one search group per index, but this
       still works, why fiddle with it? */
    for ( sg = Cdb.groups, j = 0 ; j < Cdb.num_search_groups ; sg++, j++ ) {
      if ( sg->ixnum != ixnum ) continue ;
      /* See if this field is in this search group  */
      for ( fds = sg->fields ; *fds > 0 ; fds++ ) {
	/* If it is, .. */
	if ( fd == *fds ) {
	  result = setup_gsl_sg(sg) ; /* Harmless if already done */
	  for ( k = 0, found = FALSE ; k < num_ixparms[fd] ; k++ ) {
	    /* See whether we already have it with this reg, stemfn, gsl */
	    reg = ixparms[fd][k].reg ;
	    stem_fn = ixparms[fd][k].stem_fn ;
	    gsl = ixparms[fd][k].gsl ;
	    /* If we have, just OR the beast field */
	    /* beasts obs < Jan 98, forget it */
	    if ( reg == sg->reg && stem_fn == sg->stem_fn && gsl == sg->gsl ) {
	      /* ixparms[fd][k].beasts |= sg->beast ; */
	      found = TRUE ;
	      break ;
	    }
	  }
	  /* If we don't already have it make a record */
	  if ( ! found ) {
	    n = num_ixparms[fd]++ ;
	    ixparms[fd][n].reg = sg->reg ;
	    ixparms[fd][n].stem_fn = sg->stem_fn ;
	    ixparms[fd][n].gsl = sg->gsl ;
	    /* ixparms[fd][n].beasts = sg->beast ; */
	  }
	  break ;
	} /* (fd == *fds) */
      }
    }
  }
  return result ;
}

/******************************************************************************

  extract_text_mladen()

  DO NOT USE THIS FUNCTION

  This function was a direct copy of the actual extract_text() function
  which was being debugged.

  The problem dealt with blowing the stack with extremely large variables
  during runtime which would end up looking like SEGMENTATION FAULTs.

  Very bad!

  Solution: Dynamically allocate and free memory for these large arrays.

  Do extract for one attribute set on a single field or piece of text.
  For all types of database (used to be text only).

  SW June 94

  Jan 99: added var stopped to control behaviour on '<.....>'. See extract()
  and phase2_token()

  Apr 02: originally, this function was supposed to handle output to any
  appropriate structure, not just an array, the actual output being handled
  by extract()'s output function output_extracted_term(). However, since
  it is used to produce output like <term><0><source><0> it cannot be used
  where output is to a hash table or to a STRINGS struct. Hence indexing
  progs (ix1) and bss_parse_hash() cannot use it. See extract_text_nosource().

******************************************************************************/
int
extract_text_mladen(char *ip, int il, FIELD_TYPE field_type, Extraction_regime reg,
	     int (*stemfn)(char *, int, char *), Gsl *gs, void *outptr,
	     int outflag, int rec, u_int lims, int fdnum, int diag)
{

  int l = il ;

  // fprintf(stderr, "extracting_text2:%d\n", l);
  char *p = ip ;
  char *sp ;
  int pl, sl ;
  int paranum = 0 ;
  int senum = 0 ;
  static int *tokens = NULL ;
  int numtoks ;
  int nk ;
  int numterms = 0 ;

  char *opp ;
  int outlen ;
  int stopped ;
  char work[65536] ;
  BOOL sources ;
  opp = outptr ;
  outptr = work ;
  char *tokmapBuf;
	 
  sources = (outflag & EX_SOURCE) ;
/*
 *  mko - Added these just to test out the compiler settings. Not actually
 *  needed for anything else.
 */
/*
#if HILO
fprintf(stderr, "extracting_text2a:%d\n", l); 
#else
fprintf(stderr, "extracting_text2b:%d\n", l); 
#endif
*/

 // while ( l > 0 ) {
    if ( (Cdb.i_db_type & DB_TEXT_TYPE) ) {
      pl = find_para(p, l, INDENT|GAP) ; /* or cd use parafile, because must
 					    be consistent with rule used */
      if ( diag ) fprintf(Err_file, "Para %d\n", paranum) ;
    }
    else pl = l ;
    stopped = 0 ;
    l -= pl ; sp = p ; p += pl ;
 //   while ( pl > 0 ) {
      sl = find_sentence(sp, pl, field_type, TEXTPOS_MAX_T, 0) ;
      if ( diag ) {
 	fprintf(Err_file, "Sentence %d\n", senum) ;
 	fprintf(Err_file, "**%.*s**\n", sl, sp) ;
      }
      if ( sources ) {
 	numtoks = tokenize_text(sp, sl, &tokens) ;
 //	/* Note tokenize_text() creates the tokens array if need be */
 	if ( diag ) fprintf(Err_file, "%d toks\n", numtoks) ;
 	if ( numtoks > TEXTPOS_MAX_T )
 	  fprintf(Err_file,
 		  "rec %d: %d toks in sntce %d, max is %d\n",
 		  rec, numtoks, senum, TEXTPOS_MAX_T) ;
 	nk = extract(sp, sl, reg, gs, stemfn, outptr, outflag,
 		   rec, 0, fdnum - 1, senum, tokens, &stopped, &outlen) ;
      }
      else 
 	nk = extract(sp, sl, reg, gs, stemfn, outptr, outflag,
 	     rec, 0, fdnum - 1, senum, (int *)NULL, &stopped, &outlen) ;
      if ( nk == -1 ) {
 	numterms = nk ;
	//	break ;
      }
      
      if ( sources ) {
 	/*if ( oflag == EX_USER_SUPERPARSE ) {*/
 	/* Put sources in */
 	char *ipp = outptr ;
 	int j, l ;
 	U_pos p ;
 	int start ;
 	int numtoks ;		/* Not really used */

   tokmapBuf = bss_malloc(sizeof(struct tokmap) * 65536);
   if (tokmapBuf == NULL)
	{
		 fprintf(stderr, "out of memory");
	}
 	struct tokmap *tokmap = (void *)tokmapBuf ;
//
// 	numtoks = map_tokens(sp, sl, &tokmap[0]) ;


// 	for ( j = 0 ; j < nk ; j++ ) {
// 	  l = strlen(ipp) + 1 ;
// 	  memcpy(opp, ipp, l) ;
// 	  ipp += l ; opp += l ;
// 	  memcpy((char *)&p, ipp, sizeof(U_pos)) ;
// 	  if ( p.pos == 0xffffffff ) {
// 	    fprintf(Err_file, "Infinite Pos record\n") ;
// 	  }
// 	  ipp += sizeof(U_pos) ;
//#if HILO
// 	  start = tokmap[p.ptnthl.t].s ;
// 	  l = tokmap[p.ptnthl.t + p.ptnthl.nt - 1].f - start + 1 ;
//#else
// 	  start = tokmap[p.ptntlh.t].s ;
// 	  l = tokmap[p.ptntlh.t + p.ptntlh.nt - 1].f - start + 1 ;
//#endif
// 	  memcpy(opp, sp + start, l) ;
// 	  opp += l ;
// 	  *opp++ = '\0' ;
// 	}
      }
 //     else { /* (sources false) */
 //	/* Simply copy over */
 //	char *ipp = outptr ;
 //	int j, l ;
 //
 //	for ( j = 0 ; j < nk ; j++ ) {
 //	  l = strlen(ipp) + 1 ;
 //	  memcpy(opp, ipp, l) ;
 //	  ipp += l ; opp += l ;
 //	}
 //	*opp = '\0' ;
 //     }	
 //
 //     pl -= sl ; sp += sl ;
 //     senum++ ;
 //     numterms += nk ;
 //   }
 // }
  bss_free(tokmapBuf);
  return numterms ;
}

int
extract_text(char *ip, int il, FIELD_TYPE field_type, Extraction_regime reg,
	     int (*stemfn)(char *, int, char *), Gsl *gs, void *outptr,
	     int outflag, int rec, u_int lims, int fdnum, int diag)
{

  int l = il ;
  char *p = ip ;
  char *sp ;
  int pl, sl ;
  int paranum = 0 ;
  int senum = 0 ;
  static int *tokens = NULL ;
  int numtoks ;
  int nk ;
  int numterms = 0 ;
  char *tokmapBuf;

  char *opp ;
  int outlen ;
  int stopped ;
  char work[65536] ;
  BOOL sources ;
  opp = outptr ;
  outptr = work ;
  //fprintf(stderr, "extracting_text\n");
  sources = (outflag & EX_SOURCE) ;
  while ( l > 0 ) {

    if ( (Cdb.i_db_type & DB_TEXT_TYPE) ) {
      pl = find_para(p, l, INDENT|GAP) ; /* or cd use parafile, because must
					    be consistent with rule used */
      if ( diag ) fprintf(Err_file, "Para %d\n", paranum) ;
    }
    else pl = l ;
    stopped = 0 ;

    l -= pl ; sp = p ; p += pl ;
    while ( pl > 0 ) {
      sl = find_sentence(sp, pl, field_type, TEXTPOS_MAX_T, 0) ;
      if ( diag ) {
        //fprintf(stderr, "Sentence %d\n", senum) ; // mko
        // fprintf(stderr, "**%.*s**\n", sl, sp) ; //mko
      }
      if ( sources ) {
        numtoks = tokenize_text(sp, sl, &tokens) ;
        /* Note tokenize_text() creates the tokens array if need be */
        if ( diag )  fprintf(stderr, "%d toks\n", numtoks) ;
        if ( numtoks > TEXTPOS_MAX_T )
            fprintf(stderr,
                    "rec %d: %d toks in sntce %d, max is %d\n",
                    rec, numtoks, senum, TEXTPOS_MAX_T) ;
            //fprintf(stderr, "extracting_text2\n"); 
         nk = extract(sp, sl, reg, gs, stemfn, outptr, outflag,
                      rec, 0, fdnum - 1, senum, tokens, &stopped, &outlen) ;
      }
      else 
        nk = extract(sp, sl, reg, gs, stemfn, outptr, outflag,
                     rec, 0, fdnum - 1, senum, (int *)NULL, &stopped, &outlen) ;
      if ( nk == -1 ) {
        numterms = nk ;
        break ;
      }
      // fprintf(stderr, "extracting_text3\n");  // mko
     
      if ( sources ) {
	/*if ( oflag == EX_USER_SUPERPARSE ) {*/
	/* Put sources in */
	char *ipp = outptr ;
	int j, l ;
	U_pos p ;
	int start ;
	int numtoks ;		/* Not really used */

   // Begin mko
   // Dynamically allocat the tokmapBuf array.
   tokmapBuf = bss_malloc(sizeof(struct tokmap) * TOKMAPSIZE);
   if (tokmapBuf == NULL)
   {
     fprintf(stderr, "Memory shortage, for tokmap structure");
   }
   struct tokmap *tokmap = (void *)tokmapBuf ;
   //struct tokmap tokmap[TOKMAPSIZE] ;
   // End mko

	numtoks = map_tokens(sp, sl, &tokmap[0]) ;
	for ( j = 0 ; j < nk ; j++ ) {
	  l = strlen(ipp) + 1 ;
	  memcpy(opp, ipp, l) ;
	  ipp += l ; opp += l ;
	  memcpy((char *)&p, ipp, sizeof(U_pos)) ;
	  if ( p.pos == 0xffffffff ) {
	    fprintf(Err_file, "Infinite Pos record\n") ;
	  }
	  ipp += sizeof(U_pos) ;
#if HILO
	  start = tokmap[p.ptnthl.t].s ;
	  l = tokmap[p.ptnthl.t + p.ptnthl.nt - 1].f - start + 1 ;
#else
	  start = tokmap[p.ptntlh.t].s ;
	  l = tokmap[p.ptntlh.t + p.ptntlh.nt - 1].f - start + 1 ;
#endif
	  memcpy(opp, sp + start, l) ;
	  opp += l ;
	  *opp++ = '\0' ;
	}
      }
      else { /* (sources false) */
	/* Simply copy over */
	char *ipp = outptr ;
	int j, l ;

	for ( j = 0 ; j < nk ; j++ ) {
	  l = strlen(ipp) + 1 ;
	  memcpy(opp, ipp, l) ;
	  ipp += l ; opp += l ;
	}
	*opp = '\0' ;
      }	

      pl -= sl ; sp += sl ;
      senum++ ;
      numterms += nk ;
    }
  }
  bss_free(tokmapBuf); //mko
  return numterms ;
}

int
extract_text_nosource(char *ip, int il, FIELD_TYPE field_type,
		      Extraction_regime reg, int (*stemfn)(char *, int, char *),
		      Gsl *gs, void *outptr, int outflag, int rec, int diag)
{
  int l = il ;
  char *p = ip ;
  char *sp ;
  int pl, sl ;
  int nk ;
  int numterms = 0 ;

  int stopped ;
  int outlen ;
  void *opp = outptr ;
  while ( l > 0 ) {
    pl = find_para(p, l, INDENT|GAP) ; 
    stopped = 0 ;
    l -= pl ; sp = p ; p += pl ;
    while ( pl > 0 ) {
      sl = find_sentence(sp, pl, field_type, TEXTPOS_MAX_T, 0) ;
      nk = extract(sp, sl, reg, gs, stemfn, opp, outflag,
		   rec, 0, 0, 0, (int *)NULL, &stopped, &outlen) ;
      if ( nk == -1 ) {
	numterms = nk ;
	break ;
      }
      pl -= sl ;
      sp += sl ;
      numterms += nk ;
      /*(char *)opp += outlen ; */
      /* mko Fix the pointer arithmetic here */
      char * opp_temp = (char *) opp;
      opp_temp += outlen;
      opp = (void *) opp_temp;
    }
  }
  return numterms ;
}
