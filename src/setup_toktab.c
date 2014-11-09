#ifndef lint
static char *setup_toktab_sccsid = "%Z% %M% %I% %E%\n";
#endif  /* lint */

/******************************************************************************

  setup_toktab.c SW Jan 93

  Utility prog needs to be run to construct the Toktab[] array for
  charclass macros etc

  Writes initialised array called Toktab using the character arrays here
  and the definitions in charclass.h. The output is to stdout, and is in
  the form of a .c file.

******************************************************************************/

#include "charclass.h"
#include <stdio.h>

int
isintab(int ch, char *table)
{
  register int i = 1 ; 

  while ( ch != *table ) { 
    if ( *table == '\0' ) return(0) ;
    table++ ;
    i++ ; 
  }
  return(i) ;
}

/******************************************************************************

  Stuff for counting text tokens (Dec 92)

  More generally, character classification (7-bit only)

******************************************************************************/

char Fins[] = " .,'\";:!?/-()[]{}<>=`\n\t\r\f" ;
char Space[] = " \n\t\r\f" ;	/* Should always be subset of Fins */
char Parens[] = "()" ;
char Starts[] =
  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz&" ;
char Closing[] = "'\")]" ;	/* May appear after '.' at end of sentence */
char Special_chars[] = "_@^$|" ; /* Used by phase1 etc */
char Apunct[]  = " .,;:?!" ;	/* Used by phase2() */
char Mpunct[]  = ";:?!" ;	/* Used by phase2() */
char Stop_punct[] = ".;:?!" ;	/* Used by get_next_sentence etc */
char Vowels[] = "aAeEiIoOuU" ;	/* Y is only one if prec by a non-vowel,
				   Should consider U also */
char Cons[] = "bcdfghjklmnpqrstvwxzBCDFGHJKLMNPQRSTVWXZ" ;
char Upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
char Lower[] = "abcdefghijklmnopqrstuvwxyz" ;
char Lsz[] = "lszLSZ" ;
char Eils[] = "eilsEILS" ;
char Wxy[] = "wxyWXY" ;
char Dig[] = "0123456789" ;
char Q[] = "'\"`<>" ;
char Underscore[] = "_" ;
char Ampersand[] = "&" ;

int
main()
{
  int j ;
  int tab[256] ;

  for ( j = 0 ; j < 256 ; j++ ) {
    tab[j] = 0 ;
    if ( j != 0 ) {
      if ( isintab((char)j, Starts) ) tab[j] |= TOK_START ;
      if ( isintab((char)j, Fins) ) tab[j] |= TOK_FIN ;
      if ( isintab((char)j, Space) ) tab[j] |= TOK_SPACE ;
      if ( isintab((char)j, Parens) ) tab[j] |= TOK_PARENS ;
      if ( isintab((char)j, Closing) ) tab[j] |= TOK_CLOSING ;
      if ( isintab((char)j, Special_chars) ) tab[j] |= TOK_SPEC ;
      if ( isintab((char)j, Apunct) ) tab[j] |= TOK_APUNCT ;
      if ( isintab((char)j, Mpunct) ) tab[j] |= TOK_MPUNCT ;
      if ( isintab((char)j, Stop_punct) ) tab[j] |= TOK_STOP ;
      if ( isintab((char)j, Vowels) ) tab[j] |= TOK_VOWEL ;
      else if ( isintab((char)j, Cons) ) tab[j] |= TOK_CONS ;
      if ( isintab((char)j, Upper) ) tab[j] |= TOK_UPPER ;
      else if ( isintab((char)j, Lower) ) tab[j] |= TOK_LOWER ;
      if ( isintab((char)j, Lsz) ) tab[j] |= TOK_LSZ ;
      if ( isintab((char)j, Eils) ) tab[j] |= TOK_EILS ;
      if ( isintab((char)j, Wxy) ) tab[j] |= TOK_WXY ;
      if ( isintab((char)j, Dig) ) tab[j] |= TOK_DIG ;
      if ( isintab((char)j, Q) ) tab[j] |= TOK_QUOTE ;
      if ( isintab((char)j, Underscore) ) tab[j] |= TOK_UNDERSCORE ;
      if ( isintab((char)j, Ampersand) ) tab[j] |= TOK_AMPERSAND ;
    }
  }
  printf("/* Toktab.c */\n\n") ;
  printf("int Toktab[256] = \n{") ;
  for ( j = 0 ; j < 256 ; j++ ) {
    printf("%d", tab[j]) ;
    if ( j < 255 ) printf(",") ;
  }
  printf("} ;\n") ;
  exit(0) ;
}  


