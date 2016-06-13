/*****************************************************************************

  Extsubs

  Revision 7 Jan 88 - (phase1()) - modes are now an enum type - see
                      stdeqs.h
  Revision 6 Jan 88 - documentation of Ph1 improved & renamed Phase1.
                      Indexing made BOOL instead of int.

  phase1() funcs removed Jan 98 -- haven't been used for some time

 *****************************************************************************/

#include "bss.h"
#include "charclass.h"
#include "bss_errors.h"
#include <ctype.h>

/******************************************************************************

  num_toks = count_toks(string, length)

  Count the number of text tokens in length of string.

  Uses TOK_START and TOK_FIN (see above).

  See also countchar() (utils.c)

******************************************************************************/

int
count_toks(char *s, int l)
{
  register BOOL intoken = FALSE ;
  register int j ;
  register int count = 0 ;

  for ( j = 1 ; j < l ; j++ ) {
    if ( Toktab[(u_char)(s[j])] & TOK_FIN ) 
      intoken = FALSE ;
    else if ( ! intoken && (Toktab[(u_char)(s[j])] & TOK_START) ) {
      intoken = TRUE ;
      count++ ;
    }
  }
  return count ;
}


/******************************************************************************

  get_next_sentence(field_type)  SW Jan 93

  NB March 2002. This has been unused for some time but the code looks
  possibly useful so is kept here for now (but "defined" out).

  Points to next "sentence" in an input field, setting up a correspondence
  between character positions and token numbers as it goes.

  This is to supersede various versions of phase1() for fully positional
  indexes (type 3 etc)

  char *
  get_next_sentence(ip, &ilength, &op, &olength, field_type, &out_type,
                    tokens, &numtoks)
  
  The return value is the address from which to start looking for the
  following sentence (NULL if input exhausted).

  It is called with ilength = total remaining input length, and on return
  ilength contains the remaining length for the next call.

  op and olength are used to "return" the output (i.e. sentence starts at
  op and is of length olength). Field_type is the type of the input field,
  out_type on exit is the type (possibly different -- a sub-type) of the
  output sentence.

  Tokens is an integer array. On exit tokens[output_offset] is the token number
  of the offset (offsets are 0..olength-1). Numtoks is the number of tokens.

  if tokens[] is NULL, ignores it

  Sentence end definition is difficult..! Added parens 30 March 93 after
  trying to process horrible Federal Register document. Frigged again 1 Apr
  93

  Nov 98: seems only to be used by extract_field(), which itself is only
  used by parse0() in USER_SUPERPARSE or USER_SUPERPARSE_NOSOURCE when
  on non-text databases. I'm pretty sure find_sentence() should be used.

  Dec 99: believe unused

******************************************************************************/

#ifdef OLD_EXTRACT

#define Sentence_end \
   ( \
     il <= 1 || \
    (Toktab[(u_char)(*ip)] & (TOK_QUOTE|TOK_PARENS)) || \
    ( \
    ( il <= 2 || \
      (Toktab[(u_char)(*(ip + 1))] & (TOK_QUOTE|TOK_UPPER))) && \
  ! ( tok_init_cap && toklen < 4 ) && \
      isspace((*ip)) && \
  ! ( tok_alpha(*(ip-2)) && ( *(ip - 3) == ' '  || *(ip - 3) == '.' ) ) && \
     strncmp(ip - 5, "Capt", 4) != 0 \
   ) \
  )

char *
get_next_sentence(u_char *ip, int *ilength, char **op, int *olength,
		  FIELD_TYPE field_type, FIELD_TYPE *out_type,
		  int tokens[], int *numtoks)
{
  register u_char ch ;
  register int il, ol ;
  register BOOL started, halted ;
  int toknum = -1, opos = 0 ;	/* opos counts output chars */
  int toklen ;			/* Doubt if toklen is used */
  BOOL tok_init_cap ;
  BOOL tok_all_caps ;
  BOOL tok_all_dig ;
  BOOL in_token = FALSE ;
  /* BOOL skipping = FALSE ; unused */

  ol = 0 ;
  il = *ilength ;
  *out_type = field_type ;
  if ( tokens != NULL ) *numtoks = 0 ;
  started = FALSE ; halted = FALSE ;

  switch (field_type) {

  case TEXT:
  case PHRASE:
    /* Don't know what PHRASE is meant to be */
    /* split on ; : ? ' - ' '--' and where possible on . */
    
    /* Advance to a start position */
    while ( il > 0 ) {
      if ( Toktab[*ip] & TOK_START ) {
	*op = ip ;
	break ;
      }
      ip++ ; il-- ;
    }
    /* Having found first Tok_start & recorded start address */
    
    while ( il > 0 ) {
      ch = *ip++; il-- ; 
      if ( Toktab[ch] & TOK_START ) {
	if ( ! in_token ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	  toklen = 0 ;
	  if ( tok_upper(ch) ) {
	    tok_init_cap = TRUE ;
	    tok_all_caps = TRUE ;
	  }
	  else if ( tok_digit(ch) ) tok_all_dig = TRUE ;
	  else tok_init_cap = tok_all_caps = tok_all_dig = FALSE ;
	}
      }
      else if ( Toktab[ch] & TOK_FIN ) {
	in_token = FALSE ;
	if ( tok_all_caps ) tok_init_cap = FALSE ;
	if ( Toktab[ch] & TOK_STOP ) {
	  /* Dot: this is the tricky case */
	  if ( ch == '.' ) {
	    if ( Sentence_end )	/* See huge #define above */
	      break ;
	  }			/* . */
	  else /* other TOK_STOP */
	    break ;
	}			/* TOK_STOP */
	else if ( ch == '-' &&
		 (*ip == '-' ||  (isspace(*(ip - 2)) && isspace(*ip) )) )
	  break ;
      }				/* TOK_FIN */
      if ( tokens != NULL ) {
	/* In all cases, having started */
	if ( in_token ) tokens[opos] = toknum ;
	else tokens[opos] = -1 ;
      }
      
      if ( in_token ) {
	toklen++ ;
	if ( ! tok_upper(ch) ) tok_all_caps = FALSE ;
	else if ( ! tok_digit(ch) ) tok_all_dig = FALSE ;
      }
      ol++ ;
      opos++ ;
    }
    break ;

  case TITLE:
  case MAIN_TITLE:
  case SUBTITLE:
    /* Now really only writes token array and stops when required */
    /* Phase2 has to deal with initial articles etc */
    /* Outputs whole field as one sentence except on = (assumed
       parallel title). */

    *op = ip ;

    for ( ; il > 0 ; ol++, opos++ ) {
      ch = *ip++; il-- ; 
      if ( Toktab[ch] & TOK_START ) {
	if ( ! in_token ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	}
      }
      else if ( Toktab[ch] & TOK_FIN ) {
	in_token = FALSE ;
	if ( ch == '=' ) break ;
      }
      if ( tokens != NULL ) {
	/* In all cases, having started */
	if ( in_token ) tokens[opos] = toknum ;
	else tokens[opos] = -1 ;
	/* Is the -1 necessary? */
      }
    }
    break ;

  case NAMES:
  case PERS:
  case CORP:
    /* Split on $ | ;
       If nothing, assume corporate (e.g. following ;)
       $ starts a corporate name
       | starts a personal name */
    /* Expect one of |$ at start. If not will start at first non-white */
    /* A single leading |$ is not output */

    *out_type = CORP ;
    while ( il > 0 ) {
      ch = *ip ;
      if ( ! isspace(ch) ) {
	if ( ch == '|' ) {
	  *out_type = PERS ;
	  ip++ ; il-- ; 
	}
	else if ( ch == '$' ) {
	  *out_type = CORP ;
	  ip++ ; il-- ; 
	}
	/* Any other character will be reread in main loop */
	break ;
      }
      ip++ ; il-- ;
    }

    *op = ip ;

    for ( ; il > 0 ; ol++, opos++ ) {
      ch = *ip++; il-- ; 
      if ( Toktab[ch] & TOK_START ) {
	if ( ! in_token ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	}
      }
      else if ( Toktab[ch] & TOK_FIN ) {
	in_token = FALSE ;
	if ( ch == ';' ) break ;
      }				/* TOK_FIN */
      if ( ch == '$' || ch == '|' ) {
	ip-- ; il++ ;
	break ;			/* Char reread on next call */
      }
      if ( tokens != NULL ) {
	/* In all cases, having started */
	if ( in_token ) tokens[opos] = toknum ;
	else tokens[opos] = -1 ;
      }
    }
    break ;

  case SH:
  case SH_SUBDIV:
    /* Split on ^ $ | (perhaps should also on ;)
       ^ assumed not a name
       $ starts a corporate name
       | starts a personal name
       Removed splitting at subdivisions */
    /* Expect one of ^|$ at start. If not will start at first non-white */
    /* A single leading ^|$ is not output */

    while ( il > 0 ) {
      ch = *ip ;
      if ( ! isspace(ch) ) {
	if ( ch == '|' ) {
	  *out_type = PERS ;
	  ip++ ; il-- ; 
	}
	else if ( ch == '$' ) {
	  *out_type = CORP ;
	  ip++ ; il-- ; 
	}
	else if ( ch == '^' ) {
	  ip++ ; il-- ; 
	}
	/* Any other character will be reread in main loop */
	break ;
      }
      ip++ ; il-- ;
    }

    *op = ip ;

    for ( ; il > 0 ; ol++, opos++ ) {
      ch = *ip++; il-- ; 
      if ( Toktab[ch] & TOK_START ) {
	if ( ! in_token ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	}
      }
      else if ( Toktab[ch] & TOK_FIN ) {
	in_token = FALSE ;
      }				/* TOK_FIN */
      if ( ch == '^' || ch == '$' || ch == '|' ) {
	ip-- ; il++ ;
	break ;			/* Char reread on next call */
      }
      if ( tokens != NULL ) {
	/* In all cases, having started */
	if ( in_token ) tokens[opos] = toknum ;
	else tokens[opos] = -1 ;
      }
    }
    break ;

  case NONE:
    /* non-indexing field_type */
    *olength = 0 ;
    *ilength = 0 ;
    return NULL ;		/* Make sure its not called again */
    break ;

  default:
    /* Output the lot, just tokenize */
    while ( il > 0 && isspace(*ip) ) {
      ip++ ; il-- ;
    }
    *op = ip ;

    for ( ; il > 0 ; ol++, opos++ ) {
      ch = *ip++; il-- ; 
      if ( Toktab[ch] & TOK_START ) {
	if ( ! in_token ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	}
      }
      else if ( Toktab[ch] & TOK_FIN ) {
	in_token = FALSE ;
      }				/* TOK_FIN */
      if ( tokens != NULL ) {
	/* In all cases, having started */
	if ( in_token ) tokens[opos] = toknum ;
	else tokens[opos] = -1 ;
      }
    }
    break ;

  }				/* Switch */

  *ilength = il ; *olength = ol ;
  if ( tokens != NULL ) *numtoks = toknum + 1 ;
  if ( il <= 0 ) return NULL ;	/* This call may have produced something, but
				   there's no more to do */
  else return ip ;
}
    
#endif /* OLD_EXTRACT */

/******************************************************************************

  hl_next_sentence()

  This is a version of get_next_sentence() which copies over its input
  while counting tokens and inserting highlighting on/off codes at positions
  corresponding to the posting positional fields pointed to by *posptr.

  For type 4 indexes, and perhaps to supersede this, see
  hl_text() (assumes sentence already identified).

  poscount and posptr always refer to the next unused positional field
  and their values are maintained by this function. fdnum and se_num are
  the current values for matching against positional fields.

  The function is called by bss_highlight_rec().

  ip (in) is the input start address, *ilength (in and out) the remaining
  input length, op (in) the output start address, *olength (out) the amount
  of output written.

  Modified 18th May (MJG) to highlight as much as possible of long recs.

  Parameter 'bytes_left', the current available space in the output
  buffer, is passed (as a pointer) from the function bss_highlight_rec()
  (defined in source '~okapi/src/bss/bss_recs.c').

  The highlighting process is only attempted if bytes_left is greater
  than the sum of strlen(Hion) + strlen(Hioff). Everytime an on or off
  code is added the appropriate value (strlen(Hion) or strlen(Hioff))
  is subtracted from 'bytes_left'.

  The final value of 'bytes_left' is returned to the calling function.

  Oct 99: unused

******************************************************************************/
#ifdef OLD_HIGHLIGHT
extern char Hion[], Hioff[] ;

char *
hl_next_sentence(char *ip, int *ilength, char *op, int *olength,
		 FIELD_TYPE field_type,
		 struct highlight_table *htab,
		 int *bytes_left)
{
  register char ch ;
  register int il, ol ;
  register BOOL started, halted ;
  BOOL skipping = FALSE ;
  int toknum = -1 ;
  BOOL in_token = FALSE ;
  int toklen ;
  BOOL tok_init_cap ;
  BOOL tok_all_caps ;
  BOOL tok_all_dig ;
  BOOL finished = FALSE ;
  int oncount, offcount ;
  int *ons, *offs ;
  int hi_state = 0 ;
  
  oncount = offcount = htab->size ;
  ons = htab->ons ; offs = htab->offs ;
  ol = 0 ;
  il = *ilength ;

  switch (field_type) {
  case TEXT:
  case PHRASE:
    /* Don't know what PHRASE is meant to be */
    /* split on ; : ? ' - ' '--' and where possible on . */
    
    /* Advance to a start position */
    while ( il > 0 ) {
      if ( Toktab[*ip] & TOK_START ) {
	break ;
      }
      *op++ = *ip++ ; il-- ; ol++ ;
    }
    /* Having found first Tok_start & recorded start address */
    
    for ( ; ! finished && il > 0 ; *op++ = ch, ol++ ) {
      ch = *ip++; il-- ; 
      /*
          Only attempt to add highlights if there is enough space in
          the output buffer for a pair of them. 'bytes_left' will
          be decremented by the appropriate value everytime an 
          on or off code is added.
      */
      if (*bytes_left > (strlen(Hion) + strlen(Hioff))) {
	if ( ! in_token && (Toktab[ch] & TOK_START) ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	  toklen = 0 ;
	  if ( tok_upper(ch) ) {
	    tok_init_cap = TRUE ;
	    tok_all_caps = TRUE ;
	  }
	  else if ( tok_digit(ch) )
	    tok_all_dig = TRUE ;
	  else tok_init_cap = tok_all_caps = tok_all_dig = FALSE ;

	  while ( oncount > 0 && toknum == *ons ) {
	    if ( hi_state == 0 ) {
	      strcpy(op, Hion) ;
              *bytes_left -= strlen(Hion);
	      op += strlen(Hion) ; ol += strlen(Hion) ;
	    }
	    hi_state++ ;
	    oncount-- ;
	    ons++ ;
	  }
	}
	else if ( Toktab[ch] & TOK_FIN ) {
	  if ( tok_all_caps ) tok_init_cap = FALSE ;
	  while ( offcount > 0 && toknum == *offs ) {
	    hi_state-- ;
	    offcount-- ;
	    offs++ ;
	    if ( hi_state == 0 ) {
	      strcpy(op, Hioff) ;
	      *bytes_left -= strlen(Hioff);
	      op += strlen(Hioff) ; ol += strlen(Hioff) ;
	    }
	  }
	  in_token = FALSE ;
	  if ( Toktab[ch] & TOK_STOP ) {
	    if ( ch == '.' ) {
	      /* Dot: this is the tricky case */
	      if ( Sentence_end )
		finished = TRUE ;
	    }
	    else finished = TRUE ;	/* Other "stop" punct */
	  }				/* TOK_STOP */
	  else if ( ch == '-' &&
		 (*ip == '-' ||  (isspace(*(ip - 2)) && isspace(*ip) )) )
	    finished = TRUE ;
	}
	if ( in_token ) {
	  toklen++ ;
	  if ( ! tok_upper(ch) ) tok_all_caps = FALSE ;
	  else if ( ! tok_digit(ch) ) tok_all_dig = FALSE ;
	}
      } /* end if */
    }   /* end for loop */
    break ;

  case TITLE:
  case MAIN_TITLE:
  case SUBTITLE:

    for ( ; ! finished && il > 0 ; *op++ = ch, ol++ ) {
      ch = *ip++; il-- ; 
      if (*bytes_left > (strlen(Hion) + strlen(Hioff))) {
	if ( ! in_token && (Toktab[ch] & TOK_START) ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	  while ( oncount > 0 && toknum == *ons ) {
	    if ( hi_state == 0 ) {
	      strcpy(op, Hion) ;
              *bytes_left -= strlen(Hion);
	      op += strlen(Hion) ; ol += strlen(Hion) ;
	    }
	    hi_state++ ;
	    oncount-- ;
	    ons++ ;
	  }
	}
	else if ( (Toktab[ch] & TOK_FIN) ) {
	  in_token = FALSE ;
	  while ( offcount > 0 && toknum == *offs ) {
	    hi_state-- ;
	    offcount-- ;
	    offs++ ;
	    if ( hi_state == 0 ) {
	      strcpy(op, Hioff) ;
	      *bytes_left -= strlen(Hioff);
	      op += strlen(Hioff) ; ol += strlen(Hioff) ;
	    }
	  }
	  if ( ch == '=' ) {
	    ip-- ; il++ ;
	    break ;			/* Char reread on next call */
	  }
	}
      }
    }
    if ( hi_state ) {
      strcpy(op, Hioff) ;
      *bytes_left -= strlen(Hioff);
      op += strlen(Hioff) ; ol += strlen(Hioff) ;
    }
      
    break ;

  case SH:
  case SH_SUBDIV:

    while ( il > 0 ) {
      ch = *ip ;
      if ( ! isspace(ch) ) {
	if ( ch == '|' || ch == '^' || ch == '$' ) {
	  ip++ ; il-- ; 
	}
	/* Any other character will be reread in main loop */
	break ;
      }
      ip++ ; il-- ;
    }
    for ( ; ! finished && il > 0 ; *op++ = ch, ol++ ) {
      ch = *ip++; il-- ; 
      if (*bytes_left > (strlen(Hion) + strlen(Hioff))) {
	if ( ! in_token && (Toktab[ch] & TOK_START) ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	  while ( oncount > 0 && toknum == *ons ) {
	    if ( hi_state == 0 ) {
	      strcpy(op, Hion) ;
              *bytes_left -= strlen(Hion);
	      op += strlen(Hion) ; ol += strlen(Hion) ;
	    }
	    hi_state++ ;
	    oncount-- ;
	    ons++ ;
	  }
	}
	else if ( (Toktab[ch] & TOK_FIN) ) {
	  in_token = FALSE ;
	  while ( offcount > 0 && toknum == *offs ) {
	    hi_state-- ;
	    offcount-- ;
	    offs++ ;
	    if ( hi_state == 0 ) {
	      strcpy(op, Hioff) ;
	      *bytes_left -= strlen(Hioff);
	      op += strlen(Hioff) ; ol += strlen(Hioff) ;
	    }
	  }
	}
	if ( ch == '^'  || ch == '$' || ch == '|' ) {
	  ip-- ; il++ ;
	  break ;			/* Char reread on next call */
	}
      }
    }
    break ;

  case NAMES:
  case PERS:
  case CORP:

    while ( il > 0 ) {
      ch = *ip ;
      if ( ! isspace(ch) ) {
	if ( ch == '|' || ch == '$' ) {
	  ip++ ; il-- ; 
	}
	/* Any other character will be reread in main loop */
	break ;
      }
      ip++ ; il-- ;
    }
    for ( ; ! finished && il > 0 ; *op++ = ch, ol++ ) {
      ch = *ip++; il-- ; 
      if (*bytes_left > (strlen(Hion) + strlen(Hioff))) {
	if ( ! in_token && (Toktab[ch] & TOK_START) ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	  while ( oncount > 0 && toknum == *ons ) {
	    if ( hi_state == 0 ) {
	      strcpy(op, Hion) ;
              *bytes_left -= strlen(Hion);
	      op += strlen(Hion) ; ol += strlen(Hion) ;
	    }
	    hi_state++ ;
	    oncount-- ;
	    ons++ ;
	  }
	}
	else if ( (Toktab[ch] & TOK_FIN) ) {
	  in_token = FALSE ;
	  while ( offcount > 0 && toknum == *offs ) {
	    hi_state-- ;
	    offcount-- ;
	    offs++ ;
	    if ( hi_state == 0 ) {
	      strcpy(op, Hioff) ;
	      *bytes_left -= strlen(Hioff);
	      op += strlen(Hioff) ; ol += strlen(Hioff) ;
	    }
	  }
	  if ( ch == ';' ) break ;
	}
	if ( ch == '$' || ch == '|' ) {
	  ip-- ; il++ ;
	  break ;			/* Char reread on next call */
	}
      }
    }
    break ;

  default:

    while ( il > 0 && isspace(*ip) ) {
      ip++ ; il-- ;
    }
    for ( ; ! finished && il > 0 ; *op++ = ch, ol++ ) {
      ch = *ip++; il-- ; 
      if (*bytes_left > (strlen(Hion) + strlen(Hioff))) {
	if ( ! in_token && (Toktab[ch] & TOK_START) ) {
	  in_token = TRUE ;
	  toknum++ ;	/* Must start at -1 */
	  while ( oncount > 0 && toknum == *ons ) {
	    if ( hi_state == 0 ) {
	      strcpy(op, Hion) ;
              *bytes_left -= strlen(Hion);
	      op += strlen(Hion) ; ol += strlen(Hion) ;
	    }
	    hi_state++ ;
	    oncount-- ;
	    ons++ ;
	  }
	}
	else if ( (Toktab[ch] & TOK_FIN) ) {
	  in_token = FALSE ;
	  while ( offcount > 0 && toknum == *offs ) {
	    hi_state-- ;
	    offcount-- ;
	    offs++ ;
	    if ( hi_state == 0 ) {
	      strcpy(op, Hioff) ;
	      *bytes_left -= strlen(Hioff);
	      op += strlen(Hioff) ; ol += strlen(Hioff) ;
	    }
	  }
	}
      }
    }
    break ;

  } /* (switch) */

  if ( hi_state ) {
    strcpy(op, Hioff) ;
    *bytes_left -= strlen(Hioff);
    op += strlen(Hioff) ; ol += strlen(Hioff) ;
  }
  *ilength = il ; *olength = ol ;
  
  if ( il <= 0 ) return NULL ;	/* This call may have produced something, but
				   there's no more to do */
  else return ip ;
}
#endif /* OLD_HIGHLIGHT    */
 
/******************************************************************************

  hl_text()    SW May 94

  Highlighter for text-type databases

  Input must be a single "sentence".

  Sketch of algorithm: given a copy of the address of the
  positional fields of the posting (&pos), and start, length etc of the
  text; goes through char by char counting tokens. Every token start,
  it considers turning HL on, and every token finish considers turning it
  off. At token start, while pos.t matches toknum, if off it turns it on and
  sets off_tok to the greatest off position for the pos's matching toknum;
  if on, it updates off_tok if necessary.

  Returns output length and writes the number of pos records used.

  July 95: modifying so that instead of rewriting the input with highlighting
  sequences in, it outputs a sequence of <offset><length> records. Then the
  calling function has to add the cumulative offset to each of these...

******************************************************************************/

extern char Hion[], Hioff[] ;

int
hl_text(char *ip, int il, char *op, U_pos *posp, int fdnum, int senum,
	int *pos_used, int format, int start_oset)
{
  BOOL highlighting = (format != 3 && format != 4 ) ;	/* For testing */
  struct hl_rec *hop = (struct hl_rec *)op ;
  int oset = start_oset - 1 ;		/* So starts from (relative) 0 */
  int on_start_offset ;
  int ol = 0 ;
  int ch ;			/* Nov 99: changed from char */
  int tt ;
  int toknum = -1 ;
  BOOL in_token = FALSE ;
  BOOL match ;
  BOOL on = FALSE ;
  int off_tok = -1 ;
  int pos_end ;
  int posused = 0 ;
  BOOL istext = (Cdb.i_db_type & DB_TEXT_TYPE) ;
  char tempbuf[9] ;

#if HILO
#define pos_t (posp->pthl)
#define pos6 (posp->lphl)
#else
#define pos_t (posp->ptlh)
#define pos6 (posp->lplh)
#endif

  while ( il > 0 ) {
    ch = *ip++; il-- ; oset++ ;
    tt = Toktab[ch] ;
    if ( ! in_token && (tt & TOK_START) ) {
      in_token = TRUE ;
      toknum++ ;	/* Must start at -1 */
      /* Next bit is to cope with overlapping poses (sorry) */
      match = FALSE ;
      /* Next loop repeated for different types of positional record */
      if ( istext ) {
	while ( pos_t.t == toknum && pos_t.s == senum && pos_t.f == fdnum ){
	  match = TRUE ;
	  pos_end = pos_t.t + pos_t.nt - 1 ;
	  if ( pos_end > off_tok ) off_tok = pos_end ;
	  posp++ ;
	  posused++ ;
	}
      }
      else {
	while ( pos6.t == toknum && pos6.s == senum && pos6.f == fdnum ) {
	  match = TRUE ;
	  pos_end = pos6.t + pos6.nt - 1 ;
	  if ( pos_end > off_tok ) off_tok = pos_end ;
	  posp++ ;
	  posused++ ;
	}
      }
      if ( match && ! on ) {
	if ( highlighting ) {
	  memcpy(op, Hion, strlen(Hion)) ;
	  op += strlen(Hion) ;
	  ol += strlen(Hion) ;
	}
	else {
	  hop->code = 'A' ;	/* Dummy */
	  sprintf(tempbuf, "%08d", oset) ;
	  strncpy(hop->oset, tempbuf, 8) ;
	  on_start_offset = oset ;
	}
	on = TRUE ;
      }
    } /* (start of token) */
    else if ( in_token && (tt & TOK_FIN) ) {
      in_token = FALSE ;
      if ( on && toknum == off_tok ) {
	if ( highlighting ) {
	  memcpy(op, Hioff, strlen(Hioff)) ;
	  op += strlen(Hioff) ;
	  ol += strlen(Hioff) ;
	}
	else {
	  sprintf(tempbuf, "%08d", oset - on_start_offset) ;
	  strncpy(hop->l, tempbuf, 8) ;
	  hop++ ;
	  ol += sizeof(struct hl_rec) ;
	}
	on = FALSE ;
	off_tok = -1 ;
      }
    }
    if ( highlighting ) {
      *op++ = ch ;
      ol++ ;
    }
  }
  *pos_used = posused ;
  /* Next bit added March 95 to ensure turned off when no TOK_FIN at end */
  if ( on ) {
    if ( highlighting ) {
      memcpy(op, Hioff, strlen(Hioff)) ;
      ol += strlen(Hioff) ;
    }
    else {
      sprintf(tempbuf, "%08d", oset - on_start_offset) ;
      strncpy(hop->l, tempbuf, 8) ;
      hop++ ;
      ol += sizeof(struct hl_rec) ;
    }
  }
  return ol ;
}

/*****************************************************************************

  Phase2 - 3 June 87

  (Nov 99: has long been effectively superseded by phase2_token.
  This put here from old phase2.c 18 Nov 99.)

  NB 22 Apr 92: phase2() is used by indexing progs and to process user
  input. hl_phase2() is used for term extraction for highlighting and QE.
  There seems to be some discrepancy partic in treatment of numbers

  22 Apr 92: also removed source recording. This is in the highlighting version
  hl_phase2(). This one needs word number recording for Type 1 indexes, but
  not done yet.

  Revised 12 Nov 87 to record sources if Source != NULL - 
  a bit messy.
  Revised 29 Oct 87 - second arg is length of input string.
  Revised 2 Oct 87 - made output terminated string & stripped 
  trailing nonalphanumeric.

  Reduces text to alphanumerics and single blanks and hyphens.
  Recognises and preserves initialisms as far as possible.
  Returns output length. 

  When extracting from bib records, use Phase1 first.  When processing 
  search, don't need phase1.

  Algorithm is similar to that used in LIBERTAS, but has minor 
  improvements.

  Input is string and its length. 
  Output is terminated string, &, if Sources not NULL, the source 
  of each component is recorded in Sources. Sources ends with a NULL 
  source of zero length. This removed Apr 92, see above.

  Output must not overwrite input (because of ampersand)

  Tested, not exhaustively, 3 June 87.
  Time is between 0.2 and 5 ms per call (Sun 3/50).

  Note: treatment of hyphens has been improved by trying to reduce 
  ' - ' to '-' when the 'words' on each side are all digits. (In fact 
  it only takes account of the word on the left, so 1939 - abcd comes
  out as 1939-abcd, whereas abcd - pqrs makes abcd pqrs.
  
  An apostrophe is retained if it is the second, and not the last, 
  character of a word - otherwise removed.

 *****************************************************************************/

#ifdef OLD_REG

#define space 1
#define init 2
#define alldigit 4


int
phase2(char *s, int ls, char *t, BOOL convert)
{
  register int ch, nch ; /* Current and next character */
  register int flags = space ;
  register int l = 0 ; /* Output length */

#define outputchar(x) *t++ = x, l++ 

  if ( ls > 0 ) nch = *s++ ;
  while ( ls-- > 0 ) {
    ch = nch ; nch = ( ls > 0 ) ? *s : '\0' ;
    if ( tok_alpha(ch) ) {
      flags &= ~alldigit ;
      if ( convert ) ch = cvul(ch) ;
      if (flags & init) {
	if ( tok_alnum(nch) || nch == '\'' ) {
	  flags &= ~(init | space | alldigit) ; 
	  outputchar(' ') ;
	}
      }
      else if ( ((flags & space) || *(t-1) == '-') &&
	       (nch == ' ' || nch == '.')) {
	flags |= init ;
      }
      flags &= ~space ; outputchar(ch) ;
    }
    else if ( tok_digit(ch) ) {
      if ( flags & (space|init) ) flags |= alldigit ;
      if ( flags & init ) { 
	flags &= ~init ; *t++ = ' ' ; 
	l++ ; 
      }
      flags &= ~space ; outputchar(ch) ;
    }
    else if ( ch == ' ' && !(flags & (init|space) ) && ! ( *(t-1) == '-' &&
	     (flags & alldigit) ) ) {
	       flags |= space ; *t++ = ch ; l++ ; 
	     } /* otherwise space is simply ignored */ 
    else if (ch == '.') {
      /* If alldigit and next char is digit output unchanged else if next is
	 apunct or initflag or spaceflag ignore, otherwise output space */
      if ( (flags & alldigit) && tok_digit(nch) ) {
	outputchar(ch) ; flags &= ~space ; 
      }
      else if ( !( (Toktab[nch] & TOK_APUNCT) || ( flags & (init|space)))){
	outputchar(' ') ;
	flags |= space ; 
      }
      /* else ignore */
    } /* '.' */
    else if ( ch == ',' ) {
      flags &= ~init ;
      if ( ! ( (Toktab[nch] & TOK_APUNCT) || ( flags & space ) ||
	      ( tok_digit(nch) && tok_digit(*(t-1)) ) ) ) {
	flags |= space ; *t++ = ' ' ; l++ ; 
      }
    }
    else if ( (Toktab[ch] & TOK_MPUNCT) ) {
      flags &= ~init ;
      if ( ! ( (flags & space) || (Toktab[nch] & TOK_APUNCT) ) ) {
	flags |= space ; *t++ = ' ' ; l++ ; 
      }
    }
    else if ( ch == '-' ) {
      if ( flags & alldigit ) {
	if ( flags & space ) { flags &= ~space ; *(t-1) = ch ; }
	else outputchar(ch) ;
      }
      else if ( (flags & space) || (Toktab[nch] & TOK_APUNCT) ) ;
      else if ( nch == '-' ) { 
	flags |= space ; *t++ = ' ' ; l++ ; 
      }
      else { 
	*t++ = ch ; l++ ; 
      }
      flags &= ~init ;
    }

    else if ( ch == '/' ) {
      if ( ! (flags & space) &&  tok_alnum(nch) && tok_alnum(*(t-1)) ) {
	flags &= ~init ; outputchar('-') ;
      }
    }
    /* Apr 92: sincle wordpos has been removed have to detect apos in 2nd
       position in a more clumsy way.. */
    else if ( ch == '\'' && ( *(t - 2) == ' ' || l == 1 ) &&
	     ! ( flags & init ) && tok_alnum(nch) && tok_alpha(*(t - 1)) )
      outputchar(ch) ;

    else if ( ch == '&' && ((flags & space) || (flags & init))
	     && nch == ' ' ) {
      if ( ( flags & init ) ) {
	*t++ = ' ' ; l++ ;
      }
      strcpy(t, "and") ; l += 3 ; t += 3 ; /* Bug corrected 15 Aug 92
					      (t wasn't incremented) */
      flags &= ~(space | init | alldigit) ;
    } /* else ignore */
    
    s++ ;
    
  }
  /* Strip trailing nonalphanumeric */
  while ( l > 0 && !tok_alnum(*(t - 1)) ) {
    t-- ; l-- ;
  }
  *t = '\0' ;
  return(l) ;
}

#endif /* OLD_REG */

/******************************************************************************

  phase2_token()

  See phase2.c:phase2() for process

  This version takes an array of token numbers
  intokens(input char offset) and writes an   array
  outtokens(output char offset). Note both intokens and outtokens can be
  NULL pointers.

  Aug 98: made outputblank reset flags to avoid problem with numbers of the
  form .n when they follow another number.

  Jan 99: added boolean arg `stopped' so effect of e.g. '<' '>' can be carried
  over from one call to the next (previously local, meant <...> over more
  than one "sentence" only worked on first sentence.

  Nov 02: while making a simplified version of this at least two shortcomings
  noticed: (1) initial decimal point not retained (.3 -> 3 but 0.3 -> 0.3)
  and (2) "mixed style" initialisms get concatenated (B.B.C. I T V fails
  but B.B.C. ITV is fine). Second one would rarely happen. Treatment of
  hyphens is not good either (e.g. pre-2.34 -> pre2 34)

******************************************************************************/

#define SPACE 1
#define INIT 2
#define ALLDIGIT 4

/* Strips s into t, returns output length */
int
phase2_token(char *s, int ls, char *t, BOOL convert,
	     int intokens[], int outtokens[],
	     int *stopped)
{
  register u_char ch, nch ; /* Current and next character */
  register int flags = SPACE ;
  register int l = 0 ; /* Output length */
  register int tt ;
  int ipos = -1, opos = 0 ;
  int wordlen = 0 ;
  /* BOOL stopped = FALSE ; */
  int skipped[256] ;
  int nskipped ;
  static char *func = "phase2_token" ;
#define outputchar(x) *t++ = x; l++; wordlen++; \
  if ( intokens != NULL ) outtokens[opos++] = intokens[ipos]

#define outputblank *t++ = ' '; l++; wordlen=0; flags = SPACE; \
  if ( intokens != NULL ) outtokens[opos++] = intokens[ipos]

#define extrablank *t++ = ' '; l++; wordlen=0; flags = SPACE; \
  if ( intokens != NULL ) outtokens[opos++] = -1

  if ( Dbg & DD_TEXT ) {			    
    memset((char *)skipped, '\0', sizeof(skipped)) ;
    nskipped = 0 ;
  }
  if ( ls > 0 ) nch = *s++ ;
  while ( ls-- > 0 ) {
    ch = nch ; nch = ( ls > 0 ) ? *s++ : '\0' ; ipos++ ;

    /* Dreadful bodge put in for TREC3 when I realised it was outputting
       all the '<TEXT>' etc as 'text', and possibly stringing them onto the
       next token, with the '<>' removed. We really want some non-ascii
       char to act as "don't index" brackets, and replace <> with them
       while converting the raw data. */
    /* Further bodged Jan 99 - `stopped' now external to this function */

    if ( *stopped ) {
      if ( ch == '>' ) *stopped = 0 ;
      continue ;
    }

    tt = Toktab[ch] ;
    if ( tt & TOK_ALPH ) {
      if (flags & ALLDIGIT) {
	if ( *(t - 1) == '-' ) *(t - 1) = ' ' ;
	flags &= ~ALLDIGIT ;
      }
      if ( convert ) ch = cvul(ch) ;
      if (flags & INIT) {
	if ( (Toktab[nch] & TOK_ALNUM) || nch == '\'' ) { extrablank ; }
      }
      else if ( ((flags & SPACE) || *(t-1) == '-') &&
	       (nch == ' ' || nch == '.')) {
	flags |= INIT ;
      }
      flags &= ~SPACE ; outputchar(ch) ;
    } /* (alph) */
    else if ( tt & TOK_DIG ) {
      if ( flags & INIT ) { extrablank ; }
      if ( flags & SPACE ) flags |= ALLDIGIT ;
      flags &= ~SPACE ; outputchar(ch) ;
    } /* (dig) */
    else if ( isspace(ch) ) {
      if (!(flags & (INIT|SPACE)) &&  !(*(t-1) == '-' && (flags & ALLDIGIT))) {
	outputblank ;
      } 
    } /* otherwise space is simply ignored */ 
    else if (ch == '.') {
      /* If ALLDIGIT and next char is digit output unchanged else if next is
	 apunct or INITflag or SPACEflag ignore, otherwise output space */
      if ( (flags & ALLDIGIT) && (Toktab[nch] & TOK_DIG) ) {
	outputchar(ch) ; flags &= ~SPACE ; 
      }
      else if ( !( (Toktab[nch] & TOK_APUNCT) || ( flags & (INIT|SPACE)))){
	extrablank ;
      }
      /* else ignore */
    } /* '.' */
    else if ( ch == ',' ) {
      flags &= ~INIT ;
      if ( ! ( (Toktab[nch] & TOK_APUNCT) || ( flags & SPACE ) ||
	      ( tok_digit(nch) && tok_digit(*(t-1)) ) ) ) {
	extrablank ;
      }
    }
    else if ( tt & TOK_MPUNCT ) {
      flags &= ~INIT ;
      if ( ! ( (flags & SPACE) || (Toktab[nch] & TOK_MPUNCT) ) ) {
	extrablank ;
      }
    }
    else if ( ch == '-' ) {
      /* Dec 95: bug in incomplete date ranges e.g. 1914-
	 Added the Toktab[nch) & TOK_DIG
	 but really want lookahead to next A/N.
	 Not sure if the *(t-1)=ch is sound either. */
      if ( (flags & ALLDIGIT) && (Toktab[nch] & TOK_DIG) ) {
	if ( flags & SPACE ) { flags &= ~SPACE ; *(t-1) = ch ; }
	else if ( *(t - 1) != '-' ) { outputchar(ch) ; }
      }
      else if ( (flags & SPACE) || (Toktab[nch] & TOK_APUNCT) ) ;
      else if ( nch == '-' ) { extrablank ; }
      else { 
	/* Inserting here Apr 93 a crude version of the old do_hyphs() */
	if ( wordlen >= 4 ) { extrablank ; }
	/* else squash it */
      }
      flags &= ~INIT ;
    }

    else if ( ch == '/' ) {	/* Cf. hyphen */
      if ( ! (flags & SPACE) &&  tok_alnum(nch) &&
	  tok_alnum(*(t-1)) ) {	extrablank ; }
      /* else squash */
    }
    else if ( ch == '\'' && ( l == 1 || *(t - 2) == ' ' ) &&
	     ! ( flags & INIT ) && tok_alnum(nch) &&
	     tok_alpha(*(t - 1)) ) {
      outputchar(ch) ;
    }
    else if ( ch == '&' && ((flags & SPACE) || (flags & INIT))
	     && nch == ' ' ) {
      if ( ( flags & INIT ) ) {	extrablank ; }
      outputchar('a') ; outputchar('n') ; outputchar('d') ;
      flags &= ~(SPACE | INIT | ALLDIGIT) ;
    }
    else if ( ch == '<' ) { *stopped = TRUE ; continue ; }

    /* else ignore */
    else if ( Dbg & DD_TEXT ) {
      if ( ch >=128  ||
	   (ch < 32 && ch != 10 && ch != 9 && ch != 12 && ch != 13)) {
	skipped[ch]++ ;
	nskipped++ ;
      }
    }
  }
  /* Strip trailing nonalphanumeric */
  while ( l > 0 && ! tok_alnum(*(t - 1)) ) {
    t-- ; l-- ;
  }
  *t = '\0' ;
  if ( Dbg & DD_TEXT ) {
    int j ;
    if ( nskipped ) {
      fprintf(bss_syslog, "%s(): skipped %d chars\n",
	      func, nskipped) ;
      for ( j = 0 ; j < 256 ; j++ )
	if ( skipped[j] ) fprintf(bss_syslog, "%d (%d) ", j, skipped[j]) ;
      fprintf(bss_syslog, "\n") ;
    }
  }
  return(l) ;
}

#if NEW
/* Strips s into t, returns output length */
phase2_token_plainwords(char *s, int ls, char *t, BOOL convert,
	     int intokens[], int outtokens[],
	     int *stopped)
{
  register u_char ch, nch ; /* Current and next character */
  register int flags = SPACE ;
  register int l = 0 ; /* Output length */
  register int tt ;
  int ipos = -1, opos = 0 ;
  int wordlen = 0 ;
  /* BOOL stopped = FALSE ; */
  int skipped[256] ;
  int nskipped ;
  static char *func = "phase2_token" ;

  if ( Dbg & DD_TEXT ) {			    
    memset((char *)skipped, '\0', sizeof(skipped)) ;
    nskipped = 0 ;
  }
  if ( ls > 0 ) nch = *s++ ;
  while ( ls-- > 0 ) {
    ch = nch ; nch = ( ls > 0 ) ? *s++ : '\0' ; ipos++ ;

    if ( *stopped ) {
      if ( ch == '>' ) *stopped = 0 ;
      continue ;
    }

    tt = Toktab[ch] ;
    if ( tt & TOK_ALPH ) {
      if (flags & ALLDIGIT) {
	if ( *(t - 1) == '-' ) *(t - 1) = ' ' ;
	flags &= ~ALLDIGIT ;
      }
      if ( convert ) ch = cvul(ch) ;
      if (flags & INIT) {
	if ( (Toktab[nch] & TOK_ALNUM) || nch == '\'' ) { extrablank ; }
      }
      else if ( ((flags & SPACE) || *(t-1) == '-') &&
	       (nch == ' ' || nch == '.')) {
	flags |= INIT ;
      }
      flags &= ~SPACE ; outputchar(ch) ;
    } /* (alph) */
    else if ( tt & TOK_DIG ) {
      if ( flags & INIT ) { extrablank ; }
      if ( flags & SPACE ) flags |= ALLDIGIT ;
      flags &= ~SPACE ; outputchar(ch) ;
    } /* (dig) */
    else if ( isspace(ch) ) {
      if (!(flags & (INIT|SPACE)) &&  !(*(t-1) == '-' && (flags & ALLDIGIT))) {
	outputblank ;
      } 
    } /* otherwise space is simply ignored */ 
    else if (ch == '.') {
      /* If ALLDIGIT and next char is digit output unchanged else if next is
	 apunct or INITflag or SPACEflag ignore, otherwise output space */
      if ( (flags & ALLDIGIT) && (Toktab[nch] & TOK_DIG) ) {
	outputchar(ch) ; flags &= ~SPACE ; 
      }
      else if ( !( (Toktab[nch] & TOK_APUNCT) || ( flags & (INIT|SPACE)))){
	extrablank ;
      }
      /* else ignore */
    } /* '.' */
    else if ( ch == ',' ) {
      flags &= ~INIT ;
      if ( ! ( (Toktab[nch] & TOK_APUNCT) || ( flags & SPACE ) ||
	      ( tok_digit(nch) && tok_digit(*(t-1)) ) ) ) {
	extrablank ;
      }
    }
    else if ( tt & TOK_MPUNCT ) {
      flags &= ~INIT ;
      if ( ! ( (flags & SPACE) || (Toktab[nch] & TOK_MPUNCT) ) ) {
	extrablank ;
      }
    }
    else if ( ch == '-' ) {
      /* Dec 95: bug in incomplete date ranges e.g. 1914-
	 Added the Toktab[nch) & TOK_DIG
	 but really want lookahead to next A/N.
	 Not sure if the *(t-1)=ch is sound either. */
      if ( (flags & ALLDIGIT) && (Toktab[nch] & TOK_DIG) ) {
	if ( flags & SPACE ) { flags &= ~SPACE ; *(t-1) = ch ; }
	else if ( *(t - 1) != '-' ) { outputchar(ch) ; }
      }
      else if ( (flags & SPACE) || (Toktab[nch] & TOK_APUNCT) ) ;
      else if ( nch == '-' ) { extrablank ; }
      else { 
	/* Inserting here Apr 93 a crude version of the old do_hyphs() */
	if ( wordlen >= 4 ) { extrablank ; }
	/* else squash it */
      }
      flags &= ~INIT ;
    }

    else if ( ch == '/' ) {	/* Cf. hyphen */
      if ( ! (flags & SPACE) &&  tok_alnum(nch) &&
	  tok_alnum(*(t-1)) ) {	extrablank ; }
      /* else squash */
    }
    else if ( ch == '\'' && ( *(t - 2) == ' ' || l == 1 ) &&
	     ! ( flags & INIT ) && tok_alnum(nch) &&
	     tok_alpha(*(t - 1)) ) {
      outputchar(ch) ;
    }
    else if ( ch == '&' && ((flags & SPACE) || (flags & INIT))
	     && nch == ' ' ) {
      if ( ( flags & INIT ) ) {	extrablank ; }
      outputchar('a') ; outputchar('n') ; outputchar('d') ;
      flags &= ~(SPACE | INIT | ALLDIGIT) ;
    }
    else if ( ch == '<' ) { *stopped = TRUE ; continue ; }

    /* else ignore */
    else if ( Dbg & DD_TEXT ) {
      if ( ch >=128  ||
	   (ch < 32 && ch != 10 && ch != 9 && ch != 12 && ch != 13)) {
	skipped[ch]++ ;
	nskipped++ ;
      }
    }
  }
  /* Strip trailing nonalphanumeric */
  while ( l > 0 && ! tok_alnum(*(t - 1)) ) {
    t-- ; l-- ;
  }
  *t = '\0' ;
  if ( Dbg & DD_TEXT ) {
    int j ;
    if ( nskipped ) {
      fprintf(bss_syslog, "%s(): skipped %d chars\n",
	      func, nskipped) ;
      for ( j = 0 ; j < 256 ; j++ )
	if ( skipped[j] ) fprintf(bss_syslog, "%d (%d) ", j, skipped[j]) ;
      fprintf(bss_syslog, "\n") ;
    }
  }
  return(l) ;
}

#endif /* NEW */

/******************************************************************************

  tokenize_text(s, l, tokens)  SW May 94

  Does the "tokenizing" part of get_next_sentence, assumes the sentence has
  already been got. Simpler if theoretically less efficient, we should
  probably change over to this, but at present only used for type 4
  indexing. (Nov 99) is used for all now -- type 4 long obsolete.

  Writes -1 if not in a token or <toknum> if in a token to tokens[offset],
  offset goes from 0.

  Returns the number of tokens (not the number of live elements in tokens[])

  It creates the tokens array and places its address in the supplied pointer,
  resizing if necessary.

******************************************************************************/

int
tokenize_text(char *s, int l, int **tokens)
{
  int opos = 0 ;
  int toknum = -1 ;
  /* int numtoks = 0 ; unused */
  int ch ;
  BOOL in_token = FALSE ;
  u_int t ;
  int *toks ;

  static int array_size = 0 ;

  if ( array_size < l ) {
    if ( array_size == 0 ) *tokens = (int *)bss_malloc(l * sizeof(int)) ;
    else *tokens = (int *)bss_realloc(*tokens, l * sizeof(int)) ;
    /* Never freed I think, shouldn't matter */
    if (Dbg & D_EXTRACT)
      fprintf(bss_syslog, "tokens array resized from %d to %d\n",
	    array_size, l) ;
    array_size = l ;
  }
  toks = *tokens ;
  while ( l > 0 ) {
    ch = *s++ ;
    t = Toktab[ch] ; l-- ;
    if ( t & TOK_START ) {
      if ( ! in_token ) {
	in_token = TRUE ;
	toknum++ ;
      }
    }
    else if ( t & TOK_FIN ) in_token = FALSE ;

    if ( in_token ) toks[opos] = toknum ;
    else toks[opos] = -1 ;
    opos++ ;
  }
  return toknum + 1 ;
}


/******************************************************************************

  map_tokens  SW Aug 93

  Function for writing start offset (0- ) and finish offset onto an array
  of (int, int). Uses character classes TOK_START and TOK_FIN
  (see Toktab).

  Stops when has processed given length or reads a null character.
  (If length unknown but string terminated give tl a large value.)

  Returns number of tokens mapped.

  Used by i0(superparse)

******************************************************************************/

int
map_tokens(char *text, int tl, struct tokmap *map)
{
  int num = 0 ;			/* Number of toks output */
  BOOL in_tok = FALSE ;
  int offset = 0 ;
  char *p = text ;
  u_char ch ;

  for ( ; tl && (ch = *p) ; p++, offset++, tl-- ) {
    if ( ! in_tok && (Toktab[ch] & TOK_START) ) {
      in_tok = TRUE ;
      map->s = offset ;
    }
    else if ( in_tok && (Toktab[ch] & TOK_FIN) ) {
      in_tok = FALSE ;
      map->f = offset - 1 ;
      num++ ;
      map++ ;
    }
  }
  if ( in_tok ) {
    map->f = offset - 1 ;
    num++ ;
    map++ ;
  }
  return num ;
}


/*****************************************************************************

   Dohyphs_srch 6 Oct 87

   Process hyphens in a search string

   Process a string produced by Phase2 so that it becomes a sequence of 
   blank-separated 'words'.  Hyphens are treated as follows:

      if there is one hyphen in a 'word', if the portion up to and 
      including the hyphen is in Gsl as a prefix (gsclass P), the hyphen 
      is squashed.  If the portion is not in gsl, or gsl is NULL, then 
      the hyphen is squashed if it is in positions 2..4 of the word.
      If the segment on both sides of the hyphen is all digits (probably 
      a range of dates) or the prefix is in the gsl with gsclass H (e.g. 'al-') 
      the hyphen is retained.

      In all other cases hyphens are replaced by blanks.

      This routine should arguably handle apostrophes too.

      1 June 92: can crash by il running negative, not clear how. Altered top
      loop so checks il > 0 instead of il != 0. Altered hl_dohyphs also.

      There are problems with / -> - when used in fractions (e.g. 1/2). Need
      to think more about treatment of numbers etc.

      Feb 98: don't think this has been used for ages
 *****************************************************************************/

#ifdef DOHYPHS

int
dohyphs_srch(u_char *s, int il, Gsl *gsl) 
#define HDIAG 0
{
  register int wl = 0 ;		/* Length of current word */
  register BOOL alldig = TRUE, hyphsq = FALSE ;
  int ol = il ;	      /* Output length (occasionally less than input length) */
  int pos ;
  char *gsres, gsclass, *tok ;
  int gsml ;

  for ( ; il > 0 ; il-- ) {
    if (tok_alpha(*s)) { alldig = 0 ; wl++ ; }
    else if (*s == ' ') { 
      wl = 0 ; alldig = TRUE ; hyphsq = FALSE ;
    }
    else if (*s == '-') {	/* This is the messy one */
      if ( hyphsq ) { *s = ' ' ; wl++ ; alldig = FALSE ; }
      else {			/* Go and see if there's another one */
	pos = findchars((u_char *)" -", s+1, il) ;
	/* Then pos is 0 or the position (1..) of the next blank or hyph. */
	if ( pos && *(s+pos) == '-' ) {
	  hyphsq = TRUE ; *s = ' ' ; wl++ ; alldig = FALSE ;
	}
	else {			/* There's only one hyphen */
	  if ( alldig && ((pos = posnondig(s+1, il))==0 || *(s+pos)==' ')) ;
	  /* Just leave the hyphen alone */

	  /* Check if prefix in Gsl */
	  else {
	    /* Should do a check here for 'special' hyphenated words
	       such as 'gilt-edge', 'a-level', .. */
	    gsres = do_gsl((char *)s-wl, wl+1, gsl, &gsclass, &gsml, &tok) ;
	    if ( gsres != NULL && (gsclass == 'P' || gsclass == 'H' ) ) {
	      if ( gsclass == 'P' ) { /* Squash it */
		memcpy(s, s+1, il) ; il-- ; ol-- ;
	      }
	      /* If gsclass = H leave it alone -it'll be stopped during 
		 lookup */
	    }			/* end of gsclass = H or P */
	    else if (wl >= 1 && wl <= 3) { /* Squash it */
	      memcpy(s, s+1, il) ; il-- ; ol-- ;
	    }
	    else {		/* Replace it by a blank */
	      *s = ' ' ; wl++ ; alldig = 0 ;
	    }
	  }
	} /* end of 'one hyphen' */
      }
    }
    /* Any nonalph, nonhyphen, nonblank */
    else {
      if ( ! isdigit(*s) ) alldig = 0 ;
      wl++ ;
    }
    /* All cases */
    s++ ;
  }
  return(ol) ;
}

#endif /* DOHYPHS */

/*********************************************************************

Extdewey

Extract a sequence of Dewey numbers and truncated DNs from an Okapi 
bib. record class number field.  It returns the number of valid 
items extracted (may be zero).

The output is placed as a sequence of precount strings in obuf, and each
string is followed by an fbyte with the 'stemming' bits set
according to whether the number is truncated or entire and with the index
number bits set to 1. See Fbytes.doc for the rules for constructing
fbytes. The fbyte is not counted in the precount.

A DN starts at the first digit and terminates with the character before the
next character which is not a digit or a '$' or a '.', or when there is no
more input or on error (but see Truncated DNs below).

The following conditions constitute an error: a '.' occurs other than in
position 4 of the current item 	or a '$' occurs in positions 2 or 3 (this
is unlikely).  If an error occurs the current item is not output (i.e. is
not added to the count which will be returned as result), and the function
returns.

Whenever a (valid) '$' is read, the portion of the current item up to but
not including the dollar is output as a truncated key; input resumes with
the character following the dollar.

Any output ending with a non-digit (could only be a '.') has it removed.

Examples: 
    123.456 returns 1 and outputs <7>123.456<CLB>
    123$.45$6 returns 3 and outputs <3>123<TCLB><6>123.45<TCLB><7>123.456<CLB>
    12$5.6 returns 0
    123. or 123 return <3>123<CLB>
    123.45.6 or 12.345 return 0
    123.4$56ARP   299$.56 returns 4 and outputs:
        <5>123.4<TCLB><7>123.456<CLB><3>299<TCLB><6>299.56<CLB>
    123 25PQR returns 1 and outputs <3>123<CLB>


************************************************************************/

#ifdef EXTDEWEY

extdewey(u_char *ibuf, int ilength, u_char *obuf)
{
  u_char precount, tbuf[BLOCKSIZE];
  int a, i, j, item_count;

  precount = 0; item_count = 0;
  j = 1;  /* used as subscript for tbuf ... precount in j = 0 */

  while (ilength > 0 && ! isdigit(*ibuf)) {
    ilength--; ibuf++;		/* get start of first d.n. */
  }

  for ( i = 0; i < ilength; i++) {
    if (isdigit(*ibuf)) {
      if (precount == 3) return(item_count); 
      tbuf[j++] = *ibuf++;
      precount++; 
    }
    else if (*ibuf == '.') {
      if (precount != 3) return(item_count);
      tbuf[j++] = *ibuf++;
      precount++;
    }
    else {
      if (precount <= 2) return(item_count);
      tbuf[0] = precount;
      for (a = 0; a <= precount; a++)
	*obuf++ = tbuf[a];	/* temp buffer into output */
      ++item_count;
      if (*ibuf == '$') {	/* truncated d.n. */
	*obuf++ = ((1 << 3) | TCLB) ; /* Index number 1, stemmed class
					 number (TCLB) */
	ibuf++;
      }
      else {			/* complete d.n. */
	*obuf++ = ((1 << 3) | CLB) ; /* Unstemmed class number */
	j = 1; precount = 0;	/* re-initialise counts */
	ibuf++;
	while (ilength > 0 && ! isdigit(*ibuf)) {
	  ilength--; ibuf++; } /* get start of next d.n. */
      }
    } /* end of if ... else  */
  }   /* end of loop */

  if (precount <= 2) return(item_count);
  if (precount == 4 && tbuf[--j] == '.') /* nnn. taken to be nnn */
    --precount;

  tbuf[0] = precount;
  for (a = 0; a <= precount; a++) *obuf++ = tbuf[a];
  *obuf++ = (1 << 3) | CLB ;	/* Unstemmed */

  return(++item_count);
}
#endif /* EXTDEWEY       */
#ifdef dohyphs_ind

/*****************************************************************************

    Dohyphs_ind - 16 June 87

    NB 13 Oct 87 - this routine will no longer work because of changes to 
    Extwd. It may not be needed - delete or rewrite.

    Deal with hyphens in a word while indexing

    The input is a word as precount string, produced by Extwd (Utils).
    Returns the number of hyphens in the word.  If there are no hyphens 
    or more than one hyphen that is all it does.  If there is one hyphen, the 
    output array will contain 1st half, 2nd half, concatenated pair as 
    precount strings, in that order. (If there is more than one, the calling 
    program can use extwd with Delim = "-" to extract the individual portions.

    The output array should be in theory around 512 bytes as everything may 
    be output twice.

    Bug: if the input ended with a hyphen the routine would run off the end.

*****************************************************************************/

dohyphs_ind(char *s, char *t)
{
  int result, l1, l2, extwd() ;
  char *ip, *op, *half_1, *half_2, *conc_pair ;

  result = countchar( '-', s+1, len(s) ) ;
  
  if ( result == 1 ) {
    ip = s + 1 ; op = t ;
    l1 = extwd(ip, len(s), op, "-", "-'.") ;
    half_1 = t ;
    ip += l1 ;
    op = half_1 + len(half_1) + 1 ;
    l2 = extwd(ip, len(s) - l1, op, "-", "-'.") ;
    half_2 = op ;
    op = half_2 + len(half_2) + 2 ;
    conc_pair = op - 1 ;
    *conc_pair = len(half_1) + len(half_2) ; 
    memcpy(op, half_1+1, len(half_1)) ;
    op = conc_pair + len(half_1) + 1 ;
    memcpy(op, half_2 + 1, len(half_2)) ;
    return(result) ;
  }

  return(result) ; 
}
 
#endif /* dohyphs_ind */

/*****************************************************************************

  is_a_year ... 22 Jan 88

  Routine to identify a year. Must consist of four digits and be between the
  year 1000 and NEXT_YEAR + 5 (defined in Makefile).

  Returns TRUE if satisfies the above else returns FALSE.

 *****************************************************************************/

#ifndef NEXT_YEAR
#define NEXT_YEAR 2000
#endif

BOOL
is_a_year(char *s, int l)
{
  char *date = s ;
  int idate ;

  sscanf(date, "%4d", &idate) ;

  if (l != 4)  return(FALSE) ;	            /* must have length of four */
  
  if (! is_a_num(date, l) ) return(FALSE) ; /* invalid chars in num */

  if ((strncmp(date, "1000", 4)) <= 0)      /* ...  > 1000 */ 
    return(FALSE) ;

  if ( idate > NEXT_YEAR + 5 ) 
    return(FALSE) ;

  return(TRUE) ;
}




/*****************************************************************************

  expand_date_range

  Returns 0 and writes nothing if not a date range, else writes expanded
  date to t and returns nine, the length of expanded date range.
  
 *****************************************************************************/

int
expand_date_range(char *s, int l, char *t)
{
  char *year, expand_year[4], *output ;
  int i, no_expand, num ;
  
  year   = s ;
  output = t ;

  if (l > 9) return(0) ;	         /* ... too long */

  if ( ! is_a_year(year, 4)) return(0) ; /* .. is it a year */
  
  year += 4 ;
  if ( *year++ != '-')  return(0) ;      /* ... is hyphen fifth char  */

  if (l < 9) {		         /* ... EXPAND ... */
    if ( (no_expand = l - 5) == 3) return(0) ;
    if (! is_a_num(year, no_expand) ) return(FALSE) ;

    num = 4 - no_expand ;
    year = s ;
    for (i = 0 ; i < num ; i++ ) expand_year[i] = *year++ ;
    year = s + 5 ;
    for (i = num ; i < num + no_expand ; i++ )
      expand_year[i] = *year++ ;
    if (! is_a_year(expand_year, 4)) return(0) ;
    year = s ;
    for (i = 0 ; i < 5 ; i++ ) *output++ = *year++ ;
    for (i = 0 ; i < 4 ; i++ ) *output++ = expand_year[i] ;
  }
  else {			/* ... DON'T EXPAND ... */
    year = s + 5 ;
    if (! is_a_year(year, 4)) return(0) ;
    year = s ;			/* ... start of date range for outputting  */
    for (i = 0 ; i < l ; i++ )
      *output++ = *year++ ;
  }

  output = t ;
  if ( (cpbytes(output, 4, (output + 5), 4)) >= 0) return(0) ;
  
  return(9) ;			/* ... the length of the string */
}
