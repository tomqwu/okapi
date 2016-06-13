/******************************************************************************

  sentence.c     SW April 94

  Routines for sentence identification

  find_sentence(s, l, rule) will scan up to l chars looking for the
  start of the next sentence (or end of input). It returns the length
  scanned. Typically, it would be called repeatedly, advancing each time by
  the previous call's return value.

  Several rules are supposed to be built in. The rule which is used should
  depend on the nature of the input.

  The most general rule is for data of field_type TEXT. 

  The rule is approximately as follows.

  <PUNCT> = /[.;:?!]/, <CLOSING> = /[\])\'\"], <SPACE> = /[ \n\t]/

  Consider for sentence end (success) at the end of
  <PUNCT><CLOSING>*<SPACE><SPACE>* and at end of input.

  If end of input, success.
  If there has been no alphanumeric content so far, failure.
  If there is only a little left, don't bother (success at end of text).
  If <PUNCT> is other than full stop, success.
  Go forward and look at next text token.
    (a text token is, roughly, any alphanumeric sequence)
  If it doesn't start with capital letter, failure.
  Backtrack and look at text token preceding full stop.
  If length <= 5 and initial capital (or all caps), failure.
    (prevents success after "Ms. ", "James A.P. ")
  If length = 1, failure.
    (prevents success after "b.b.c. ")
  Otherwise success.

  Notes

  A paragraph end (in real life) always ends a sentence. This routine doesn't
  look for paragraph ends. It should not be called across paragraph
  boundaries (see para.c).

  The length returned contains (of course) any white space etc in the input
  before the first text token. It also contains any trailing space after
  the terminating punctuation.

  Obviously it cannot distinguish between ".. U.S. Navy .." and
  ".. in the U.S. The Navy .. " (won't break either). Nor will it start
  a sentence after a full stop when the next token starts with a digit
  (or lower case letter). It will not work well with text which is all caps.

  The boundary conditions haven't been thoroughly tested.

  Obviously, somewhat cleverer rules can be devised, but it's probably not
  worth it. Speed is quite important.

  NB Only the "TEXT" rule has been written (the value of "rule" is
  ignored). Other data-types include TITLE, SUBJECT_HEADINGS, NAMES,
  where special delimiters are used. These cases are much simpler.

  Apr 02: maxlength seems to be unused at present

******************************************************************************/

#undef DEBUG_SENTENCE

#include "defines.h"
#include "bss_errors.h"
#include "charclass.h"

#define LITTLE_BIT_LEFT 4

int
find_sentence(char *s, int l, int rule, int maxtoks, int maxlength)
{
  int retval = 0 ;
  int rretval ;
  u_char c ;
  char *p = s ;
  char *pp, *tok_start ; 
  int ll ;
  BOOL ini_cap ;
  int content = 0 ;		/* Avoid trivial sentences */
  int toklen ;
  int numtoks = 0 ;
  BOOL intoken = FALSE ;

  /* TEXT rule only written */

  switch (rule) {

  default:
    while ( l-- > 0 ) {
      retval++ ;
      if ( Toktab[(c = *p++)] & TOK_START ) {
	content++ ;
	if ( ! intoken ) {
	  numtoks++ ;
	  intoken = TRUE ;
	}
      }
      else if ( intoken && (Toktab[c] & TOK_FIN) ) {
	intoken = FALSE ;
	if ( numtoks == maxtoks ) {
	  /* Should look back for last linefeed or something */
	  ll = l ; pp = p ; rretval = retval ;
	  /* If wanting to return numtoks have to decrement here */
	  while ( --retval > 0 ) if ( *(--pp) == '\n' ) break ;
	  if ( retval == 0 ) {
	    retval = rretval ;
	    if ( Dbg & D_SENTENCE )
	      fprintf(stderr,
		      "Find_sentence(), no linefeed to backtrack to\n") ;
	    break ;
	  }
	  else {
	    if ( Dbg & D_SENTENCE )
	      fprintf(stderr, "find_sentence(): backtracked from %d to %d\n",
		      rretval, retval) ;
	    break ;
	  }
	}
      }
      if ( l <= LITTLE_BIT_LEFT ) { /* Don't bother with the rest */
	retval += l ; l = 0 ;
	continue ;
      }
      /* Scan for punct */
      if ( ! content || ! (Toktab[c] & TOK_STOP) ) continue ;
      /* First look at continuation */
      /* First requirement is that the next bit should be either space or
	 some sort of closing quote or bracket then space. */
      ll = l ; pp = p ;		/* Save l & p in case have to backtrack */
      while ( ll > 0 && (Toktab[(u_char)(*pp)] & TOK_CLOSING) ) {
	ll-- ; pp++ ; 
      }
      if ( ll == 0 || ! tok_space(*pp) ) {
	retval += l - ll ; l = ll ; p = pp ; 
	continue ;
      }
      if ( c == '.' ) {
	/* This is the most likely case, and the most messy */
	/* Then find next TOK_START (now pointing to SPACE after STOP) */
	pp++ ; ll-- ;
	while ( ll > 0 && ! (Toktab[(u_char)(*pp)] & TOK_START) ) {
	  pp++ ; ll-- ;
	}
	if ( ll == 0 || ! tok_upper(*pp) ) continue ;
	/* If here, have got DOT [CLOSING] SPACE [NON-A/N] UC */
	/* Then have to backtrack & look at preceding token */
	pp = p - 2 ; 
	/* Starting with the character before the '.', locate end, and
	   then beginning of previous token */
	while ( pp >= s && ! (Toktab[(u_char)(*pp)] & TOK_START) )
	  pp-- ;
	/* Note the above depends on tokens being identifiable backwards
	   or forwards, indifferently */
	ini_cap = TRUE ;
	toklen = 1 ;
	while ( pp >= s && ! (Toktab[(u_char)(*pp)] & TOK_FIN) ) {
	  toklen++ ;
	  pp-- ;
	}
	/* pp now pointing to char before start of token */
	tok_start = pp + 1 ;	/* Only for debugging I think */
	if ( tok_upper(*tok_start) ) ini_cap = TRUE ;
	else ini_cap = FALSE ;
#ifdef DEBUG_SENTENCE
	fprintf(Err_file, "Prec token '%.*s' ", toklen, tok_start) ;
#endif
	if ( (toklen <= 5 && ini_cap) || toklen == 1 ) continue ;
      }				/* Case '.' */
      /* Now if we've got here there is a sentence */
      while ( l > 0 &&
      (Toktab[(u_char)(*p)] & (TOK_SPACE|TOK_STOP|TOK_CLOSING))) {
	retval++ ;		/* THINK this is right */
	l-- ; p++ ;
      }
      break ;
    }
    return retval ;
  }
}

/******************************************************************************

  count_sentences(s, l, rule)  SW Apr 94
  
  NB "rule" ignored at present. See find_sentence()

******************************************************************************/

int
count_sentences(char *s, int l, int rule, int maxtoks)
{
  int count = 0 ;
  int sl ;

  while ( l > 0 ) {
    sl = find_sentence(s, l, rule, maxtoks, 0) ;
    if ( sl == 0 ) break ;
    count++ ;
    s += sl ; l -= sl ;
  }
  return count ;
}
