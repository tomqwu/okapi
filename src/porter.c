/*****************************************************************************

   Porter - suffix stripping and spelling normalisation routines

   17 May 1987 SW

   Oct 89 - added a line in portspell to replace trailing 'gu' with 'g' -
   'cataloguing' and 'cataloging' were not conflating

   Revision 17 Oct 86 - added Wstem and Sstem which do the complete 
   weak and strong stemming processes. For speed in indexing it is 
   probably better to repeat the code and avoid function calls.
   Added Stem_phrase which will stem a string of words using 
   a stemming function passed as an argument.

   Revision 12 Oct 87 - known length instead of pc strings.
   Revision 8 Oct 87 : Portspell does initial mc and m' -> mac
   Previous revision 2 June 87

   Adapted from Z80 PORTERN of 11 July 86 (orig. Sept 85)


   Port1 does Step 1 of the Porter algorithm with the exception of 'ed'
   endings (which are dealt with in Port2)

   Port2 does Steps 2 - 4 & 'ed' endings.

   Port5 does Step 5.

   Both Port1 and Port2 treat both 'is' and 'iz', although this is redundant
   if Portspell is also done. Portspell is not very useful unless Port1 has 
   been done first.

   To produce a "weak stem", do Port1 then Portspell, move a copy of the 
   result and do Port5 on it.  To produce a "strong stem" do Port2 then 
   Port5 to the result of Port1 + Portspell. 

   Note: Port1 doesn't remove final s if preceded by i or u. This does badly
   with some possessives like Gramsci's. Consider making Phase2 leave all 
   embedded apostrophes, and have Porter deal with them. 

   Nov 92: added wstem_nospell, sstem_nospell and nostem_spell
   (latter prob doesn't work very well as portspell assumes port1 done)

   Nov 01: comparing with Martin Porters own coding (see newport.c) noticed
   several mistakes in ours, and two improvements in his. In particular
   I didn't understand that if the matching part of a rule succeeded but the
   condition part failed the rest of that sequence of rules are to be
   skipped. This doesn't have a big effect on output ("argument", "agreement"
   are exceptions) but it 's possible to write more efficient code. I compared
   running times (our function sstem (which of course contains additions to
   the original Porter) with Martin's newport on a list of 30000 or 40000
   terms. On sanda (dual Xeon or something) Martin's took about 0.13" and ours
   about 0.14" -- not enough for a proper comparison.

   Nov 01: added psstem() which simply calls Martin's English stem function
   (see realport.c)

 *****************************************************************************/

#include "charclass.h"
#include "bss.h"

#define portminl 4 /* Minimum length which will be processed. If less than
		      4 check bounds in various places below! */

/*****************************************************************************
    
Subroutines for Porter

(See also Issub and Replace (in utils))

*****************************************************************************/

static int
cons(char ch, int prev) /* Returns 1 if ch is a consonant otherwise 0.
			   Doesn't care about case. Prev is the value returned
			   by the previous call of cons() */
{
  if ( tok_vowel(ch) || ( ch == 'y' && prev == 1 ) ) return(0) ;
  else return(1) ;
}

static int
cvcq(char *w) /* Returns 1 if three characters at w, w-1, w-2 are CVC, 
	      with 2nd cons. not w or x or y, otherwise 0.
	      (A y at w-1 is a vowel.) Doesn't check bounds. */ 
{
  w -= 2 ;
  if ( tok_vowel(*w++) ) return(0) ;
  if ( ! tok_vowel(*w) && *w != 'y' ) return(0) ;
  w++ ;
  if ( Toktab[(u_char)(*w)] & (TOK_VOWEL|TOK_WXY) ) return(0) ;
  return(1) ;
}

static int
doubq(char *w, int l) /* Returns 0 if last 2 characters of w are not the same
			 or not both consonants otherwise returns last char. */
{
  w += l - 1 ;
  if ( *w != *(w-1) ) return(0);
  if ( tok_vowel(*w) ) return(0) ;
  return(*w) ;
}
 
static int
meas(char *w, int l, int n) /* Return the "measure" - no of VC transitions -
			       in the first n letters of w (measure is
			       0..2 only). The character before the
			       beginning of a word is a vowel. */
{
  register char prev = 0 ;
  register int measure = 0 ;

  if ( n < 2 || n > l ) return(0) ; /* Added 1st cond May 99 following purify */
  while ( n  && ( measure < 2 ) ) {
    /* Find next vowel */
    for ( ; ; ) {
      if ( n-- == 0 ) return(measure) ;
      if ( ! ( prev = cons(*w++, prev) ) ) break ; 
    }
    /* Find next consonant */
    for ( ; ; ) { 
      if ( n-- == 0 ) return(measure) ;
      if ( (prev = cons(*w++, prev)) ) break ;
    }
    measure++ ;
  }
  return(measure) ;
}

static int
vowelq(char *w, int l, int n) /* Returns 1 if 1st n characters of w contain a
				 vowel, otherwise 0 */
{
  register char prev = 0 ;
  if ( n > l ) return(0) ;
  while ( n-- ) if ( ! ( prev = cons(*w++, prev) ) ) return(1) ;
  return(0) ;
}

static int
port1(char *iw, int l, char *w)
     /* Does Porter Step 1 from string iw to w. Returns output 
	length. W can be the same as Iw */
{
  register char ch ;

  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  if ( l < portminl ) return(l) ;
  /* Step 1 : plurals & 'ing' */

  if ( ( ch = *(w + l - 1)) == 's' ) {
    if ( isequal("sse", w+l-4, 3) || isequal("ie", w+l-3, 2) || 
	*(w+l-2) == '\'' ) l -= 2 ;
    else if ( (ch = *(w + l - 2)) != 's' && ch != 'u' && 
	     ( ch != 'i' || ! vowelq(w, l, l - 2 ) ) ) 
      l-- ;
  }
  else if ( isequal("ae", w+l-2, 2) ) l-- ;

  if ( vowelq( w, l, l - 3 ) && isequal("ing", w+l-3, 3) ) {
    l -= 3 ;
    if ( l == 2 ) {
      if ( isequal("us", w, 2) ) {
	*(w + 2) = 'e' ;
	l++ ;
      }
      return l ;		/* & doubtless other cases of length 2 */
    }
    if ( isequal("at",w+l-2,2)||isequal("bl",w+l-2,2)||isequal("is",w+l-2,2)|| 
	isequal("iz", w+l-2, 2) ) 
      { *(w + l ) = 'e' ; l++ ; }
    else if ( ( ch = doubq(w, l) ) &&
	      ! (Toktab[(u_char)ch] & TOK_LSZ) ) l-- ; 
    else if ( l >= 3 && (meas(w, l, l) == 1) && cvcq(w + l - 1) ) {
      /* Added l >= 3 May 99 for cvcq (also in port2) */
      *(w + l) = 'e' ; l++ ;
    }
  }
  if ( vowelq(w, l, l - 1) && *(w + l - 1) == 'y' ) *(w + l - 1) = 'i' ;
  return(l) ;
}

static int
port2(char *iw, int l, char *w) /* Does Porter steps 2 - 4 + 'ed' from iw to w.
				   W can be the same as Iw */
{
  register char ch ;
  int m3, m4, m5 ; /* Measures */

  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  if ( l < portminl ) return(l) ;

  if ( isequal("ed", w+l-2, 2) && (*(w+l-3) != 'e' || isequal("agreed",w,l)) &&
      vowelq(w,l,l-2) && !(isequal("united", w, l)) ) {
	l -= 2 ;
	if ( l == 2 ) {
	  if ( isequal("us", w, 2) ) {
	    *(w + 2) = 'e' ;
	    l++ ;
	  }
	  return l ;		/* & doubtless other cases of length 2 */
	}
	  
        if ( isequal("at",w+l-2,2) || isequal("bl",w+l-2,2) || 
	    isequal("is",w+l-2,2) || isequal("iz", w+l-2, 2) ) 
	  { *(w + l ) = 'e' ; l++ ; }
        else if ( ( ch = doubq(w, l) ) &&
		  ! (Toktab[(u_char)ch] & TOK_LSZ) ) l-- ; 
        else if ( l >= 3 && (meas(w, l, l) == 1) && cvcq(w + l - 1) )
	  {  *(w + l) = 'e' ; l++ ;}
  } /* ('ed') */
  ch = *(w + l - 2) ;
  if (meas(w, l, l - 3)) {
    if ( ch == 'a' ) { 
      if ( meas(w, l, l - 7 ) && isequal("ational", w+l-7, 7) )
	{ l -= 4 ; *(w+l-1) = 'e' ; }
      else if (meas(w, l, l - 6) && isequal("tional", w+l-6, 6) ) l -= 2 ; 
    } 
    else if ( ch == 'c' && meas(w, l, l-4)) {
      if ( isequal("enci", w+l-4, 4) || isequal("anci", w+l-4, 4) ) 
	*(w+l-1) = 'e' ;
    }
    else if ( ch == 'e' ) {
      if ( meas(w, l, l-4) && isequal("iser", w+l-4, 4) ) l-- ; 
    }
    else if ( ch == 'l' && meas(w, l, l-4) ) {
      if ( isequal("abli", w+l-4, 4) ) *(w+l-1) = 'e' ;
      else if ( isequal("alli", w+l-4, 4) ) l -= 2 ;
      else if ( meas(w, l, l - 5) &&  
	       ( isequal("entli", w+l-5, 5) ||
		 /* Missed "eli" -> "e" here (noticed Nov 01) */
		isequal("ousli", w+l-5, 5) ) ) l -= 2 ;
    }
    else if ( ch == 'o' && meas(w, l, l-4) ) {
      if ( meas(w, l, l - 7) && 
	  ( isequal("isation", w+l-7, 7) || isequal("ization", w+l-7, 7) ) )
	{ l -= 4 ; *(w+l-1) = 'e' ; }
      else if ( meas(w, l, l - 5) && isequal("ation", w+l-5, 5) ) 
	{ l -= 2 ; *(w+l-1) = 'e' ; }
      else if ( isequal("ator", w+l-4, 4) ) { l-- ; *(w+l-1) = 'e' ; }
    }
    else if ( ch == 's' && meas(w, l, l-5) ) {
      if (meas(w, l, l - 7) && 
	  (isequal("iveness", w+l-7, 7) ||
	   isequal("fulness", w+l-7, 7) ||
	   isequal("ousness", w+l-7, 7) ) )
	l -= 4 ;
      else if (isequal("alist", w+l-5, 5) || isequal("alism", w+l-5, 5) ) 
	l -= 3 ;
    }
    else if ( ch == 't' && meas(w, l, l-5) ) {
      if ( isequal("aliti", w+l-5, 5) ) l -= 3 ;
      else if ( isequal("iviti", w+l-5, 5) ) { l -= 2 ; *(w+l-1) = 'e' ; }
      else if ( meas(w, l, l-6) && isequal("biliti", w+l-6, 6) ) 
	{ l -= 3 ; *(w+l-1) = 'e' ; *(w+l-2) = 'l' ; }
    }

    /* Step 3 of the original algorithm */

    if ( Toktab[(u_char)(*(w + l - 1))] & TOK_EILS ) {

      m5 = meas(w, l, l-5) ; m4 = meas(w, l, l-4) ;
      if (m5 && ( isequal("icate", w+l-5, 5) || isequal("alise", w+l-5, 5) ||
		 isequal("alize", w+l-5, 5) || isequal("iciti", w+l-5, 5) ) ) 
	l -= 3 ;
      else if ( m5 && isequal("ative", w+l-5, 5) ) l -= 5 ;
      else if ( m4 && isequal("ical", w+l-4, 4) ) l -= 2 ;
      else if ( isequal("ness", w+l-4, 4) ) l -= 4 ;
      /* ( meas(l - 3) already tested) */
      else if ( isequal("ful", w+l-3, 3) ) l -= 3 ;
    } /* Step 3 */
  } /*  Measure l - 3 */

  /* Step 4 */

  if ( meas(w, l, l - 2) > 1 ) {
    ch = *(w + l - 2) ;
    m3 = meas(w, l, l-3) ; m4 = meas(w, l, l-4) ; m5 = meas(w, l, l-5) ;
    if ( ( ch == 'a' ) && isequal("al", w+l-2,2) ) l -= 2 ;
    else if ( ( ch == 'c' ) && m4 > 1 && 
	     ( isequal("ance", w+l-4,4) || isequal("ence", w+l-4,4) ) ) l -=4 ;
    else if ( isequal("er", w+l-2, 2) ||  isequal("ic", w+l-2, 2) ) l -=2 ; 
    else if ( ch == 'l' && m4 > 1 && 
	     ( isequal("able", w+l-4, 4) ||
	      isequal("ible", w+l-4, 4) ) )
      l -= 4 ; 
    else if ( ch == 'n' ) {
      if ( m3 > 1 && isequal("ant", w+l-3, 3) ) l -= 3 ; 
      else if ( m5 > 1 && isequal("ement", w+l-5, 5) ) l -= 5 ;
      else if ( m4 > 1 && isequal("ment", w+l-4, 4) ) l -= 4 ;
      else if ( m3 > 1 && isequal("ent", w+l-3, 3) ) l -= 3 ;
    } /* ch == 'n' */
    else if ( ( isequal("tion", w+l-4, 4) || isequal("sion", w+l-4, 4) ) && 
	     m4 > 1 )
      l -=3 ;
    else if ( isequal("ou", w+l-2, 2) ) l -= 2 ;
    else if ( m3 > 1 ) {
      if ( isequal("ism",w+l-3, 3) || 
	  isequal("ise",w+l-3,3) || isequal("ist",w+l-3,3) ) 
	l -= 3 ;
      else if ( ch == 't' &&
	       (isequal("ate",w+l-3,3) || isequal("iti",w+l-3,3)))
	l -= 3 ; 
      else if (isequal("ous",w+l-3,3)||isequal("ive",w+l-3,3) ||
	       isequal("ize",w+l-3,3))
	l -= 3 ; 
    } /* (meas(l - 3) > 1) */
  } /* (meas(l - 2) > 1) */
  return(l) ;
} /* Port2 */

static int
port5(char *iw, int l, char *w) /*Step 5 of the original algorithm. W can be the same as Iw. */
{
  int m ;

  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  if ( l < portminl ) return(l) ;
  if ( *(w + l - 1) == 'e' ) {
    m = meas(w, l, l - 1) ;
    if ( m > 1 || ( m == 1 && ! cvcq(w + l - 2))) l-- ;
  }
  if ( meas(w, l, l) > 1 && ( (char) doubq(w, l) == 'l' ) ) l-- ;
  return(l) ;
} 


static int
portspell(char *iw, int l, char *w)
{
  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  if ( l < portminl ) return(l) ;
  if ( l > 4 ) {
    srchrep("iz", 2, "is", 2, w, &l, 0, 0) ;
    srchrep("ae", 2, "e", 1, w, &l, 2, 0) ; 
    srchrep("ph", 2, "f", 1, w, &l, 0, 0) ;
    srchrep("oe", 2, "e", 1, w, &l, 0, 0) ;
    if ( l > 5 ) {
      if ( isequal(w, "mc", 2) || isequal(w, "m\'", 2) ) {
	(void)memmove(w + 3, w + 2, l - 2) ;
	(void)memcpy(w, "mac", 3) ; l++ ;
      }
      srchrep("our", 3, "or", 2, w, &l, 0, 0) ;
      /* The remainder only apply at the end & if l > 5 */
      if ( isequal("ism", w+l-3, 3) ) {
	if ( vowelq(w, l, l-3) && ( l != 8 || ! isequal("organism", w, l) ) )
	  *(w+l-1) = 't' ;
      }
      else { /* Not 'ism' */
	if ( ! (
		(srchrep("exion", 5, "ection", 6, w, &l, l -4, 1)) || 
		(srchrep("nse", 3, "nce", 3, w, &l, l - 2, 1)) ||
		(srchrep("gue", 3, "g", 1, w, &l, l - 2, 1)) || 
		(srchrep("gu", 2, "g", 1, w, &l, l - 1, 1)) || 
		(srchrep("amme", 4, "am", 2, w, &l, l - 3, 1)) || 
		(srchrep("ism", 3, "ist", 3, w, &l, l - 2, 1)) || 
		(srchrep("ant", 3, "ent", 3, w, &l, l - 2, 1)) || 
		(srchrep("tre", 3, "ter", 3, w, &l, l - 2, 1)) ) 
	    ) /* (if) */
	  if ( l > 6 ) srchrep("anc", 3, "enc", 3, w, &l, l - 3, 1) ;
      } /* Not 'ism' */
    } /* (l > 5) */
  } /* (l > 4) */
  return(l) ;

} /* Portspell */ 

int
wstem(char *iw, int l, char *w) /* Produces weak stem. Port1+Portspell+Port5. */
{
  l = port1(iw, l, w) ;
  l = portspell(w, l, w) ;
  return(l = port5(w, l, w)) ;
}

int
wstem_nospell(char *iw, int l, char *w) /* Produces weak stem. Port1+Portspell+Port5. */
{
  l = port1(iw, l, w) ;
  return(l = port5(w, l, w)) ;
}

int
sstem(char *iw, int l, char *w) /* Strong stem. Port1+Portspell+Port2+Port5. */
{
  l = port1(iw, l, w) ;
  l = portspell(w, l, w) ;
  l = port2(w, l, w) ;
  return(l = port5(w, l, w)) ;
}

int
sstem_nospell(char *iw, int l, char *w) /* Strong stem. Port1+Portspell+Port2+Port5. */
{
  l = port1(iw, l, w) ;
  l = port2(w, l, w) ;
  return(l = port5(w, l, w)) ;
}


int
nostem(char *iw, int l, char *w)
{
  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  return(l) ;
}

int
nostem_spell(char *iw, int l, char *w)
{
  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  l = portspell(w, l, w) ;
  return(l) ;
}

int
psstem(char *iw, int l, char *w)
{
  if ( w != iw ) (void)memmove((void *)w, (void *)iw, l) ;
  if ( l < portminl ) return l ;
  return portrealstem(w, 0, l-1) + 1 ;
}

/******************************************************************************
  stem_phrase()

  Stems and reassembles a string. Each output token is followed by a blank.
  For token identification rules see the code (call of extwd())

  stem_phrase() is now only used for processing user input in the old
  interactive system.

  stem_phrase_token()

  Version of stem_phrase() which records start token number
  and number of tokens from array intokens into array of t, nt

******************************************************************************/

#ifdef OLD_EXTRACT
stem_phrase(char *ip, int length, char *op, int (*stemfn)(char *, int, char *),
	    KLS *sources) 
/* sources is array of p, l pairs to record start & length
   of the source for each item extracted, terminated by a NULL p */
{
  int totl = 0 ;
  register int l ;
  int wlength ;			/* Wordlengths returned by extwd */
  register char * source = ip ;
  int start_offset ;

  while ( (l = extwd(ip, length, op, &wlength, " ", "-'.", &start_offset))
	 > 0 ) {
    if (sources != NULL) {
      sources->l = (u_short) wlength ;
      sources->p = source ;
      sources++ ;
      source += l ;
    }
    ip += l ; length -= l  ;
    wlength = (*stemfn)(op, wlength, op) ; 
    op += wlength ; totl += wlength + 1 ;
    *op++ = ' ' ; 
  }
  if ( sources != NULL ) sources->p = NULL ;
  /* Remove trailing space if output ends with non-a/n */
  if ( totl > 0 && ! tok_alnum(*(op - 2)) ) totl-- ;
  return(totl) ; 
}
#endif /* OLD_EXTRACT */
int
stem_phrase_token(char *ip, int length, char *op,
		  int (*stemfn)(char *, int, char *),
		  int intokens[], struct pos *pos)
{

  int totl = 0 ;
  register int l ;
  int wlength ;			/* Wordlengths returned by extwd */
  int start_offset ;
  int beg_offset = 0 ;
  int end_offset ;
 /* mko 
  const char *matchString = "//-->\n-->+";
  if (strncmp (ip, matchString, 10) == 0 && length == 10)
  {
    fprintf(stderr, "\nmatchString found\n");//mko
  } */
  while ( (l = extwd((u_char *)ip, length, (u_char *)op, &wlength,
		     (u_char *)" ", (u_char *)"-'.", &start_offset))
	 > 0 ) {
    /* Have to find 1st and last char of source word to get tokens.
       Problem unless output word starts at ip, so have introduced new
       start_offset, "returned" by extwd() */
    if ( intokens != NULL ) {
      pos->t = intokens[beg_offset + start_offset] ;
      end_offset = beg_offset + start_offset + wlength - 1 ;
      pos->nt = intokens[end_offset] - pos->t + 1 ;
      beg_offset += l ;
    }
    ip += l ; length -= l  ;
    /* NB. Stem function overwriting input */
    wlength = (*stemfn)(op, wlength, op) ; 
    op += wlength ; totl += wlength + 1 ;
    if ( wlength > 0 ) {
      *op++ = ' ' ; 
      if ( intokens != NULL ) pos++ ;
    }
  }
  /* Remove trailing space if output ends with non-a/n */
  if ( totl > 0 && ! tok_alnum(*(op - 2)) )
  {
    totl-- ;
  }
  return(totl) ; 
}
