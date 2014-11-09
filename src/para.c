/******************************************************************************

  para.c     SW March 94

  Routines for paragraph identification

  find_para(s, l, rule) called at start of line in s will scan up to l chars
  looking for the first line which starts a paragraph. It returns the length
  scanned up to the start of the paragraph (or l if it hasn't found one).
  It will not return a paragraph of length 0 unless it is called with
  l = 0 ( i.e. it is assumed that the start is known to be the beginning of a
  para). Typically, it would be called repeatedly, advancing each time by
  the previous call's return value.

  Several rules are built in. If rule & INDENT a paragraph starts at
  the beginning of a line which has some alphanumeric content and is
  indented by more than the previous content-bearing line. If rule &
  GAP a paragraph starts at a content bearing  line preceded by one or
  more "empty" lines. Other rules may be coded in as required. They should
  be represented by single bits. Some rules may need too much context
  to be implemented in this function.

  Different rules are needed for some texts. For example, for plays the
  indent criterion is wrong and should be replaced by first word upper case.

  Notes.

  The INDENT rule is not clever. If fails to detect one-line paras. A concept
  of "prevailing indentation" may be useful, but beware.

******************************************************************************/
#include "bss.h"
#include "charclass.h"
#include "para.h"

#define TABSIZE 8

int
find_para(char *s, int l, int rule)
{
  int retval = 0 ;
  int ll = 0 ;			/* Linelength */
  int indent = 0 ;
  int newindent ;
  int newlines = 0 ;
  int interline = 1 ;		/* Conceivably this could be passed or
				   determined within this function */
  BOOL firstline = TRUE ;	/* Never want para before 1st line */
  BOOL content ;
  char ch ;
  char *p = s ;
  char *linestart, *content_start, *pp ;
  int outlength ;
  int offset ;
  int i ;

  while ( l > 0 ) {
    /* Find next line */
    ll = ch = 0 ;
    linestart = p ;
    while ( l-- > 0 && (ch = *p++) != '\n' ) ll++ ;
    if ( ch == '\n' ) ll++ ;
    content = FALSE ;
    newindent = 0 ;
    /* Examine line */
    for ( i = 0, pp = linestart ; i < ll ; i++, pp++ ) {
      ch = *pp ;
      if ( content == FALSE ) {
	if ( tok_alnum(ch) ) { /* ? */
	  content = TRUE ;
	  content_start = pp ;
	}
	else if ( ch == ' ' ) newindent++ ;
	else if ( ch == TAB ) newindent += TABSIZE ;
      }
    }
#ifdef DEBUG_PARA
    fprintf(Err_file, "Line length %d starts with '%c', content=%d, indent=%d, newindent=%d\n", ll, *linestart, content, indent, newindent) ;
    sleep(1) ;
#endif
    if ( content == FALSE ) newlines++ ;
    /* Check rules */
    else {
      if ( ! firstline ) {
	if ((rule & GAP) && newlines >= interline ||
	    (rule & INDENT) && newindent > indent )
	  break ;
	if ( rule & UC ) {
	   offset = find_tok(linestart, ll, &outlength, TOK_START, TOK_FIN) ;
	   if ( outlength >= 2 && offset > -1 &&
	       is_upper_tok(linestart + offset, outlength) ) {
#ifdef DEBUG_PARA
	     fprintf(Err_file, "Tok is '%.*s'\n", outlength, linestart + offset);
	     fflush(Err_file) ;
#endif
	     break ;
	   }
	 }
      }
	  
      newlines = 0 ;
      indent = newindent ;
      firstline = FALSE ;
    }
    retval += ll ;
  }
  return retval ;
}

/******************************************************************************

  count_paras(s, l, rule)     SW Apr 94

  Cf count_sentences()
  
******************************************************************************/

int
count_paras(char *s, int l, int rule)
{
  int count = 0 ;
  int sl ;

  while ( l > 0 ) {
    sl = find_para(s, l, rule) ;
    if ( sl == 0 ) break ;
    count++ ;
    s += sl ; l -= sl ;
  }
  return count ;
}
