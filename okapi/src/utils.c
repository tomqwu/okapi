/* utils.c  10th Apr 1987

Miscellaneous utility functions used throughout Okapi systems

May 96: transferred some funcs from dskutils.c

March 94: Added find_tok

26 March 92: tidied a bit & removed move()

19 Nov 89 - added dedup()
Revision 19 Feb 88 - altered time functions a little
Revision 4 Feb 88 - added Cptspcs() (compare terminated and pc strings 
Revision 28 Jan 88 - added pcit() (Put Character Into
Table), psit() (Put string into table), search_for_command()

Revision 12 Jan 88 - added two comparison functions Cpbytes & Cpssi
Rev 16 Oct 87 - added Findwd(), deleted Fnw()

Last revision 7 Oct 87, ... 16 June 87, 14 May 87, 19 Apr 87, ... etc
*/

#include "bss.h"
#include "charclass.h"
#include <sys/stat.h>
#ifndef NO_DF
#include <sys/vfs.h>
#endif

/***********************************************************************/

/*                      Low level routines		               */

/***********************************************************************/

void
nullfunction(void)		/* A function which does nothing */
{
}

void
movepcs(char *s, char *t)	/* Copy pc string from s to t */
{
  register int n = *(u_char *)s + 1 ;
  while ( n-- ) *t++ = *s++ ;
}

/* Make pcs at t from string of length l at s.
   T = S is OK provided there is a spare byte at the end.
   If l > 255 output is truncated at 255.
   Returns l (or 255) */
/* Corrected (?) 14 Jan 88, 21 Apr 88 */
int
makepcs(char *s, int l, char *t) 
{
  register int length ;
  char *t0 = t ;

  if ( l > 255 ) l = 255 ;
  length = l ;
  t += l ; s += l - 1 ;
  while ( l-- ) *t-- = *s-- ;
  *t0 = length & 0xff ;
  return(length) ;
}

/* T becomes a terminated version of the pcs S.
   S = T is all right */ 
int
makets(char *s, char *t)
{
  register int l = len(s), i = l ;
  s++ ;
  while( i-- ) *t++ = *s++ ;
  *t = '\0' ;
  return l ;
}

/* strrncpy() SW Feb 99
   As strncpy but doesn't null-fill destination
   Copies src to dst until n chars or null whichever first
*/

void *
strrncpy(void *dst, void *src, size_t n)
{
  char *ds = dst, *sr = src ;
  while ( n-- ) {
    *ds++ = *sr ;
    if ( ! *sr++ ) break ;
  }
  return dst ;
}

/*****************************************************************************

  Routines for conversion of possibly misaligned binary values to int, etc

  See also macros with similar names in defines.h

  w5hl() writes a long long value < 0x10000000000 to a 5-byte u_char buffer
  in high-low order.

*****************************************************************************/

/**HILO**/
#ifdef CV2_AND_3_FUNCS

cv2(u_char *p)			/* Convert 2-byte to (unsigned) int */
{
  int result = 0 ;

  *((u_char *)&result + 2) = *p++ ; *((u_char *)&result + 3) = *p ;
  return result ;
}

cv3(u_char *p)
{
  int u = 0 ;
  memcpy((char *)&u + 1, (char *)p, 3) ;
  return u ;
}

#endif

#ifdef HAVE_LL

int
w5hl(u_char *p, long long v)
{
  u_int i ;
  if ( v >> 40 ) return -1 ;
  *p++ = v >> 32 ;
  i = (u_int)(v & 0xffffffff) ;
  *p++ = i >> 24 ;
  i &= 0xffffff ;
  *p++ = i >> 16 ;
  i &= 0xffff ;
  *p++ = i >> 8 ;
  i &= 0xff ;
  *p = i ;
  return 0 ;
}

#endif /* HAVE_LL */

/*****************************************************************************

  reverse() byte order of 32-bit value
  reverse_bits() within byte

  getw_reverse()

  Like getw() but reverses byte order

*****************************************************************************/

short
reverse_16(short u)
{
  short result ;
  char *s = ((char *)&u) + 1 ;
  char *t = ((char *)&result) ;
  *t++ = *s-- ;
  *t = *s ;
  return result ;
}

int
reverse(int u)
{
  int result ;
  char *s = ((char *)&u) + 3 ;
  char *t = (char *)&result ;

  *t++ = *s-- ;
  *t++ = *s-- ;
  *t++ = *s-- ;
  *t = *s ;
  return result ;
}

#ifdef HAVE_LL
LL_TYPE
reverse_64(LL_TYPE ll)
{
  LL_TYPE result ;
  char *s = ((char *)&ll) + 7 ;
  char *t = (char *)&result ;

  *t++ = *s-- ;
  *t++ = *s-- ;
  *t++ = *s-- ;
  *t++ = *s-- ;
  *t++ = *s-- ;
  *t++ = *s-- ;
  *t++ = *s-- ;
  *t = *s ;
  return result ;
}
#endif

u_int
getw_reverse(FILE *f)
{
  unsigned u ;
  u_char *s = ((u_char *)&u) + 3 ;
  *s-- = getc(f) ;
  *s-- = getc(f) ;
  *s-- = getc(f) ;
  *s = getc(f) ;
  return u ;
}

u_int
reverse_bits(u_int u)
{
  unsigned o = 0 ;
  int j, k ;
  for ( j = 1, k = 128 ; k >= 1 ; j += j, k /= 2 )
    if ( u & j ) o |= k ;
  return o ;
}

u_int
reverse_int(u_int u)
{
  unsigned o = 0 ;
  unsigned j, k ;
  for ( j = 1, k = 0x80000000 ; k >= 1 ; j = j << 1, k = k >> 1 )
    if ( u & j ) o |= k ;
  return o ;
}

/*****************************************************************************

  cv_pos_text_hllh()

  Arg is assumed to be Pos_text record written on HILO machine.

  Output is the corresponding Pos_text_lh record

  (Only makes sense run on LOHI machine)

*****************************************************************************/

u_int
cv_pos_text_hllh(u_int u)
{
  unsigned rv ;
  Pos_text_lh *p = (Pos_text_lh *)&rv ;
  unsigned f, s, t, nt, sw ;
  u_char *b = (u_char *)&u ;

  f = (b[0] & 0xc0) >> 6 ;
  s = ((b[0] & 0x3f) << 9) + (b[1] << 1) + ((b[2] & 0x80) >> 7) ;
  t = ((b[2] & 0x7f) << 1) + ((b[3] & 0x80) >> 7) ;
  nt = ((b[3] & 0x78) >> 3) ;
  sw = (b[3] & 0x7) ;

  p->f = f ;
  p->s = s ;
  p->t = t ;
  p->nt = nt ;
  p->sw = sw ;
  return rv ;
}


/****************************************************************************/


void
cvint(char *s, u_int i)		/* Convert unsigned to terminated string */
{
  sprintf(s, "%u", i) ;
}

/* Convert int to string in static buf */
/* Will hold 30 or more before overwriting */
char *
itoascii(int i)
{
  static char buf[256] = "" ;
  static char *this_one = buf ;

  this_one += strlen(this_one) + 1 ; 
  if ( this_one > buf + 224 ) {
    *buf = '\0' ;
    this_one = buf ;
  }
  sprintf(this_one, "%d", i) ;
  return this_one ;
}

/* Remove leading and trailing nonalphanumeric
   from s. Returns new length.
   Exception is leading '-'. */
int
striplts(char *s, int l)
{
  register char *t = s ;
  while ( l > 0 && !(tok_alnum(*t)) && *t != '-' ) { t++ ; l-- ; }
  if ( *t != '-' || l < 2 || ! tok_digit(*(t + 1)) )
    while ( l > 0 && !tok_alnum(*t) ) { t++ ; l-- ; }
  if ( t != s ) memcpy(s, t, l) ; /* Move down */
  t = s + l - 1 ;
  while ( l > 0 && !(tok_alnum(*t)) ) { t-- ; l-- ; }
  return l ;
}

/* Strip leading white space from terminated string */
char *
slws(char *s)
{
  while ( tok_space(*s) ) s++ ;
  return s ;
}

/* Strip trailing white space from known length string */
int
stws(char *s, int l)
{
  char *t = s + l - 1 ;
  while ( l > 0 && tok_space(*t) ) {
    l-- ;
    t-- ;
  }
  return l ;
}


/* Count number of displayable characters up
   to NL or 0 or l chars read */
int
real_length(char *s, int l, int *nchars)
     /* nchars 'returns' no of chars considered including a newline */
{
  register int result = 0 ;
  register char ch ;

  *nchars = 0 ;
  while ( l-- && (ch = *s++) != '\0' && ch != '\n' ) {
    (*nchars)++ ;
    if (isprint((int)ch)) result++ ;
  }
  if ( ch == '\n' ) (*nchars)++ ;
  return(result) ;
}
  
/* Count number of displayable characters in
   null-terminated string */
int
r_len(char *s)
{
  int nchars ;
  return(real_length(s, 65536, &nchars)) ;
}

/* Find max line length & no of lines in
  a terminated string */
void
get_message_size(char *s, int *length, int *nlines) 
{
  int maxl = 0, lines = 0, lengthleft = strlen(s) ;
  int l, nchars ;

  while ( lengthleft > 0 ) {
    l = real_length(s, lengthleft, &nchars) ;
    if ( l > maxl ) maxl = l ;
    lines++ ;
    s += nchars ;
    lengthleft -= nchars ;
  }
  *length = maxl ;
  *nlines = lines ;
}

/* Interchange two known length string structs */  
void
swapkls(KLS *k1, KLS *k2) 
{
  char * p ;
  u_short l ;
  p = k1->p ; k1->p = k2->p ; k2->p = p ;
  l = k1->l ; k1->l = k2->l ; k2->l = l ;
}


/***********************************************************************/

/*                      String comparison                              */

/***********************************************************************/

/* cpbytes 12 Jan 88 - compare strings of given length, return -, 0, +
   according as first is <, =, >

   Rewritten using memcmp Jan 99, may be quicker
*/

int
cpbytes(void *s1, int l1, void *s2, int l2)
{
#ifdef OBSOLETE
  register int diff ;
  for ( ; l1 > 0 && l2 > 0 ; l1--, l2-- ) {
    diff = (u_char) *s1++ - (u_char) *s2++ ;
    if ( diff) return(diff) ;
  }
  if ( l1 == 0 )
    if ( l2 == 0 ) return(0) ;
    else return(-1) ;
  else return(1) ;
#else
  int res ;
  res = memcmp(s1, s2, (l1 < l2) ? l1 : l2) ;
  if ( res != 0 || (res == 0 && l1 == l2) ) return res ;
  else return ( l1 > l2 ) ? 1 : -1 ;
#endif 
}

/* cpssi 12 Jan 88 - compare strings, first string given length, second
   string indirect and with a precount. Returns -, =, +
*/

int
cpssi(char *s1, int l1, char **s2)
{
  return(cpbytes(s1, l1, *s2 + 1, len(*s2))) ;
}

/* cptspcs() - compare terminated and precount strings - 4 Feb 88 */

int
cptspcs(char *ts, char *pcs)
{
  register int diff, l2 ;

  l2 = len(pcs) ; pcs++ ;
  for ( ; *ts != 0 && l2 > 0 ; l2-- ) {
    diff = (u_char) *ts++ - (u_char) *pcs++ ;
    if ( diff) return(diff) ;
  }
  if ( *ts == 0 )
    if ( l2 == 0 ) return(0) ;
    else return(-1) ;
  else return(1) ;
}
  
/******************************************************************************

  cptsks(ts, ks, ksl)   SW march 02

  Compare terminated string with known length string

  Noticeably slower than the library's strcmp, particularly for long common
  substrings, but note that strncmp and memcmp need an additional test in the
  case where they return 0. I've tried various ways of writing this and can't
  find a more efficient one (without going into "word" comparisons).

******************************************************************************/

int
cptsks(u_char *ts, u_char *ks, size_t ksl)
{
  u_char t, k ;
  do {
    t = *ts++ ;
    k = (ksl-- == 0) ? 0 : *ks++ ;
    if ( t == 0 ) return t - k ;
  } while ( t == k ) ;
  return t - k ;
}


/* cpasc, cppasc - compare strings of known length / strings with precount.

   Return 0 if strictly equal (so "if ( !cpasc(..) .. " is a test for strict 
   equality). The full result is bit 0 set if 1st greater, bit 1 set if 2nd 
   greater, bit 7 set if the strings fail to be equal only because one is 
   longer than the other.  Thus bit 0 and bit 7 means 'equal so far as they go 
   but 1st longer'; bit 1 and bit 7 'equal but 2nd longer.
 
   An empty string is strictly < a non-empty string.  Two empty 
   strings are strictly equal.

   The routines expect a pointer to an integer. The number of characters 
   considered is returned in this. This enables calling routines to 
   work out things like remaining lengths, the last characters 
   considered, and so on.

   A precount string is not longer than 255 chars, but a string of given 
   length can be (almost) indefinitely long.

   Tested 10 Apr 87 SW seems OK 
   Revised 10 June 87
*/

int
cpasc(u_char *s1, int l1, u_char *s2, int l2, int *nchars)
{
#define equ 0x80
#define gtr 0x1
#define less 0x2

  *nchars = 0 ;
  s1-- ; s2-- ;
  while (( l1 > 0 ) && ( l2 > 0 )){
    l1-- ; l2-- ; s1++ ; s2++ ; (*nchars)++ ; 
    if ( *s1 < *s2 ) return(less) ;
    else if ( *s1 > *s2 ) return(gtr) ; 
  }  /* (while) */
  if ( l1 == 0 )
    if ( l2 == 0 ) return(0) ;
    else return(less | equ) ;
  else return(gtr | equ) ;
} /* (cpasc) */

int
cppasc(u_char *s1, u_char *s2, int *nchars)
{ 
  return cpasc(s1+1, len(s1), s2+1, len(s2), nchars) ; 
}

#ifdef MISSING_STRINGS

/*****************************************************************************

  strncasecmp(s1, s2, n)  Byte comparison as strncmp but case independent

  This is supposed to be a library function but we don't seem to have it

*****************************************************************************/

int
strncasecmp(char *s1, char *s2, int n)
{
  register int diff = 0, c1, c2 ;
  while ( n-- ) {
    if ( isupper( c1 = *s1++ ) ) c1 = tolower(c1) ;
    if ( isupper( c2 = *s2++ ) ) c2 = tolower(c2) ;
    if ( diff = c1 - c2 ) break ;
  }
  return(diff) ;
}

/* Next two routines now replaced by library functions strcasecp()
   strncasecmp() */


/*****************************************************************************

  strcasecmp(s1, s2)  Byte comparison as strcmp but case independent

  This is supposed to be a library function but we don't seem to have it

*****************************************************************************/

int
strcasecmp(char *s1, char *s2)		/* As strncmp but case-independent */
{
  register int diff = 0, c1, c2 ;
  while ( *s1 && *s2 ) {
    if ( isupper( c1 = *s1++ ) ) c1 = tolower(c1) ;
    if ( isupper( c2 = *s2++ ) ) c2 = tolower(c2) ;
    if ( diff = c1 - c2 ) break ;
  }
  if ( diff ) return(diff) ;
  if ( *s1 == 0 && *s2 != 0 ) return(-1) ;
  else if ( *s1 != 0 && *s2 == 0 ) return(1) ;
  else return(0) ;
}

#endif /* MISSING_STRINGS */

/*****************************************************************************

    Miscellaneous string operations

 *****************************************************************************/

/******************************************************************************

  extwd extracts the next word from a string of known length and places it 
  in output. The length of the output word is returned in olength. 
  extwd returns zero if there are no more words (after this one)
  otherwise the number of characters considered up to and including any 
  delimiter.

  The input string would normally have been Phase2'ed. 

  A word starts at the first alphanumeric at or after the entry pointer and 
  ends with the character before an element of the null-terminated string 
  Delims or when the end of the string is reached. 

  Revised 1 Jan 93: now "returns" start offset (0..) in string. This must
  of course be ignored if the length (olength) of the returned word is 0.
  Revised 24 Oct 89 to retain characters which are in the null-terminated
  retain_list. (Characters which are not delimiters or in the list or
  alphanumeric are squashed.) Delims are looked for first.
  Revised 28 Dec 87 to leave dots alone (assume decimal point)
    (no longer true (July 2000))
  Revised 12 Oct 87 to output strings of known length, insted of pc strings.
  Revised 7 Oct 87 to leave apostrophes alone.
    (no longer true (July 2000))

  NB Note about dots and apostrophes doesn't hold any more. Use retain_list.
  NB  Can it overwrite itself? I think this would be OK

  July 2000: note that as it's written a word must begin with an alphanumeric.
  I think other chars should be allowed. Maybe this should be done using
  an additional argument (start_chars).

******************************************************************************/

int
extwd(u_char *string, int length, u_char *output, int *olength,
      u_char *delims, u_char *retain_list, int *start_offset)
{
  register int ch ;		/* Current character */
  register int result = 0 ;
  *olength = *start_offset = 0 ;
  ch = *string ;
  while( length > 0 && ! tok_alnum(ch) ) { 
    length-- ; string++ ; result++ ; start_offset++ ;
    ch = *string ;
  }
  if ( length <= 0 ) return(0) ;
  while ( length > 0 && ! isintab(ch, delims) ) {
    length-- ; result++ ; 
    if ( tok_alnum(ch) || isintab(ch, retain_list ) ) {
      *output++ = ch ; (*olength)++ ;
    }
    string++ ;
    ch = *string ;
  }
  if ( length > 0 ) result++ ;
  return(result) ;
}

 
/*****************************************************************************

  findwd() finds the start of the next word in a string of given length.
  The address and length of the word found are  returned in KLS Target. 
  Findwd returns zero if there are no more words, 
  otherwise the number of characters considered up to and including any 
  delimiter.

  A word starts at the first alphanumeric at or after the entry pointer and 
  ends with the character before an element of the null-terminated string 
  Delims or when the end of the string is reached. 

  See also extwd(), which does much the same but copies the output.
  See also find_tok() (March 94), more efficient
  
*****************************************************************************/

int
findwd(u_char *string, int ilength, KLS *target, u_char *delims)
{
  register int olength = 0 ;
  register int result = 0 ;

  while( ilength > 0 && ! tok_alnum(*string) ) {
    ilength-- ; string++ ; result++ ;
  }
  if ( ilength <= 0 ) return(0) ;
  target->p = (char *)string ;
  while ( ilength > 0 && ! isintab((int)(*string), delims) ) {
    ilength-- ; result++ ; olength++ ; string++ ;
  }
  target->l = olength ;
  if ( ilength > 0 ) result++ ;
  return(result) ;
}

/*****************************************************************************

  find_tok() finds the start of the next "token" in a string of known length.
  If it finds a token it returns the offset ( 0 - ) of the token in the
  string, and the length is written to the arg outlength. If end of string
  is reached without finding a token it returns -1 and writes 0 to outlength.

  Token start characters are from the character class or classes start
  and the token ends with the character before a member of the class
  finish (see charclass.h and setup_toktab.c). It would be usual to use
  the classes TOK_START and TOK_FIN

  See also findwd() and extwd()
  
*****************************************************************************/

int
find_tok(char *string, int ilength, int *outlength, int start, int finish)
{
  int olength = 0 ;
  int offset = 0 ;

  while( ilength > 0 && ! (Toktab[(unsigned)(*string)] & start) ) {
    ilength-- ; string++ ; offset++ ;
  }
  if ( ilength <= 0 ) {
    *outlength =0;
    return -1 ;
  }
  while ( ilength > 0 && ! (Toktab[(unsigned)(*string)] & finish) ) {
    ilength-- ; olength++ ; string++ ;
  }
  *outlength = olength ;
  return offset ;
}
 
/* find_name(name, names)  SW March 99 but has been used all along e.g. in
   the reader.

   Searches for name in {name, value} table and returns value or zero if
   name not in table. Comparison is case independent. Note no valid value
   may be zero.
*/

int
find_name(char *name, struct names names[])
{
  int j;

  for (j = 0; names[j].value; j++) 
    if (!strcasecmp(names[j].name, name)) return(names[j].value);	
  return 0 ;
}

/******************************************************************************

  strccpy(target, src, delim) copies source to target ending with the char
  before delim, or end of src. It terminates target and returns the number of
  copied, excluding terminating null.

******************************************************************************/

int
strdcpy(char *target, const char *source, int delim)
{
  int ch, l = 0 ;
  while ((ch = *source++) && ch != delim) {
    *target++ = ch ;
    l++ ;
  }
  *target = '\0' ;
  return l ;
}

/*****************************************************************************

Other string comparisons

*****************************************************************************/

int LWD = 0 ; /* Length of amount to compare for cpwd() */

/* Compare Lwd (extern) bytes at s1 with pc string at *s2.
   Used to allow Bsearch to find candidate keys in a gsl.
   Lwd would normally be the length of the 
   1st word of s1 (neater but slower if calculated within 
   this function) */
int
cpwd(const void *s1, const void *ps2) 
{
  register int diff ;
  register int length = LWD ;
  
  char *s2 = *(char **)ps2 ;
  if ( len(s2) > length ) 
    diff = memcmp(s1, s2 + 1, length) ;
  else
    diff = memcmp(s1, s2 + 1, len(s2)) ;
  return diff ;
}

/*

isequal now a macro in defines.h just negating memcmp() (Jan 93)
- about twice as fast as this function - don't know why

*/

/*****************************************************************************

Cptowb - compare two precount strings for equality to word boundary

    Returns -1 if s1 strictly > s2
	    -2 if s2 > s1
	     unmatched length of s1 if s2 is a left substring of s1 up to 
	     a word boundary (0 means the strings are equal).

*****************************************************************************/

int
cptowb(u_char *s1, u_char *s2) 
{
  int nchars ;
  int c ;

  c = cpasc(s1+1, len(s1), s2+1, len(s2), &nchars) ;
  if ( c & 2 ) return(-2) ;
  if ( c == 0 ) return(0) ;
  if ( !(c & 0x80) || *(s1 + nchars + 1) != ' ' ) return(-1) ;
  return(len(s1) - nchars) ;
}


/*****************************************************************************

  stringscan(t, s)   SW May 1991

  Look for terminated string t in terminated string of terminated strings s

  Returns 1..number of strings in s if found, or 0 if not.
  NB Case independent

*****************************************************************************/

int
stringscan(char *t, char *s)
{
  register int l, count = 0 ;
  register BOOL found = FALSE ;

  while ( (l = strlen(s)) != 0 ) {
    count++ ;
    if ( strcasecmp(s, t) == 0 ) {
      found = TRUE ;
      break ;
    }
    s += l + 1 ;
  }
  if ( found ) return count ;
  return 0 ;
}

/*****************************************************************************

  dedup()                                                        17 Nov, 1989
                                                                 (A. Goker)
  Function which removes duplicate substrings from a string.
  Substrings are identified by a preceding "start character"
  and end with one of the characters specified in "endlist".
  "Endlist" might typically be ";:.,".

  Eg: "^Information retrieval; ^Technical processes; ^information retrieval."
                        or
      "Information retrieval; ^information retrieval; ^Technical processes."
                      both convert to 
      "Information retrieval; ^Technical processes."
  
  The function constitutes of two passes.
  Pass 1: Reads through the string and records the starting position and
          length of each substring.
  Pass 2: Starting from the last substring compares it with each preceding
          substring. If they are the same then eliminate the rightmost of 
	  the two substrings.

*****************************************************************************/


#define MAXSUBS 128		/* Maximum number of substrings to process */


int
dedup(char *s, int l, int startch, char *endlist)
     /* char *s;	  pointer to string */
     /* int l;	          length of string */
     /* char startch;	  the character which starts the string */
     /* char *endlist ;	  Null-terminated list of characters which end a substring */
{

    /* .......................................................................
       "sublist" is an array of "substring"s
       "substring" is a structure holding the following details for 
       each substring:
         - start position of each substring eg. the position of the 1st letter
	 - the length of each substring

    ........................................................................*/

     struct substring {
       char *startpos;
       int slength;
     };

     struct substring   sublist[MAXSUBS];
     char *p, *endptr, *reppos1, *reppos2, *endpos, ch;
     int noofsubs, newslength, in_substring, last, othersubs, lastsubs;
     int noofchars, now_break;

     noofsubs=0;
     p=s;
     newslength = l;
     endptr = s + l;
     endpos = endptr;
     in_substring = FALSE;
     now_break = FALSE;

     ch = *p;
     if ( (l > 0) && ch != startch) {        /* even if there is no */
        sublist[noofsubs].startpos = p;      /* startch at start of string, */
        sublist[noofsubs].slength = 0;       /* still start a substring */ 
        in_substring = TRUE;
     }

     /* .....................................................................
	PASS 1
	Note the starting position and length of each substring before 
	comparing the substrings. Following is a summary of actions enabling
	this.

	char-type (ch)    in_substring  action
        --------------    ------------  ------
	start-char        F             start a substring, set in_substring T
	    "             T             finish substring, start next substring
	end-char          F             does not happen
	    "             T             finish substring, set in_substring F
        others            F             continue as normal 
                                    ie. advance the pointer p (pt to next char)
	    "             T             increment length of substring
	
	Note: end-char is only test if in_substring=T

     ......................................................................*/

     while (p < endptr) {
       
       ch=*p++;
       if (ch == startch) {
	 if (in_substring) { 
	   noofsubs++;		/* finish substring */
	   if (noofsubs >= MAXSUBS) /* exit if too many substrings */
	     return (-1);
         }
	 sublist[noofsubs].startpos = p; /* start new substring at p */
	 sublist[noofsubs].slength = 0;
	 in_substring = TRUE;
       }

       else if ( isintab(ch, (u_char *)endlist) && in_substring ) {
	   noofsubs++;		/* finish substring */
	   if (noofsubs >= MAXSUBS) /* exit if too many substrings */
	     return (-1);
	   in_substring = FALSE;
	   }
		
       else if (!in_substring);	/* do nothing */
            else sublist[noofsubs].slength++;
    
     }

     if (in_substring) {
       noofsubs++;
       if (noofsubs >= MAXSUBS)	/* exit if too many substrings */
	 return (-1);
     }

     /* ....................................................................

	PASS 2
	Starting with last substring, compare it with the preceding 
	substring. If the two substrings are identical (irrespective of
	upper/lower case) then remove the rightmost (string currently 
	pointing to) of the substrings. Do this by rewriting the portion of
	the original string "s" to the right of the current substring on
	top of the current substring.

	Eg: If string "s" constitutes of substrings s1,s2,s3,s4,s5 AND
	    current substring is s4                                AND
	    s4 is identical to s2                                  THEN
	    replace s5 over s4 inorder to get the new string s=s1,s2,s3,s5
       
	Repeat the above with substring before the current substring, 
	making that the current substring, until no more substrings to
	compare.

     .....................................................................*/
 

     last = noofsubs-1;         /* index to last substring */
     for (lastsubs=last; lastsubs >= 1; lastsubs--) {
       for (othersubs = lastsubs-1; othersubs >= 0; othersubs--) {
	 now_break = FALSE;
	 if (sublist[lastsubs].slength == sublist[othersubs].slength) {
	   if ( strncasecmp (sublist[lastsubs].startpos,
			   sublist[othersubs].startpos, 
			   sublist[lastsubs].slength) == 0 ) {
	     reppos1 = sublist[lastsubs - 1].startpos +
	               sublist[lastsubs - 1].slength;
	     reppos2 = sublist[lastsubs].startpos +
	               sublist[lastsubs].slength;
	     noofchars = endpos - reppos2;
	     repover(reppos1, reppos2, noofchars);
	     newslength -= reppos2 - reppos1;
	     endpos = s + newslength;
	     now_break=TRUE;
	     break;
	   }
	   if (now_break == TRUE) break;

	 }
	 if (now_break == TRUE) break;
       }
     }

     return (newslength);

}

/* function replaces string s2 over s1.
   s1 and s2 actually point to different parts of the same string s.
   s2 should point somewhere to the right of s1. */
void
repover (char *s1, char *s2, int length)
{
  while (length-- > 0) *s1++ = *s2++ ;
}


/******************************************************************************

  GNU qsort

  Named gqsort Jan 95 to avoid clash with stdlib

******************************************************************************/

/*
 * qsort.c:
 * Our own version of the system qsort routine which is faster by an average
 * of 25%, with lows and highs of 10% and 50%.
 * The THRESHold below is the insertion sort threshold, and has been adjusted
 * for records of size 48 bytes.
 * The MTHREShold is where we stop finding a better median.
 */

#define		THRESH		4		/* threshold for insertion */
#define		MTHRESH		6		/* threshold for median */

static  int		qsz;			/* size of each record */
static  int		(*qcmp)(void *, void *); /* the comparison routine */
static  int		thresh;			/* THRESHold in chars */
static  int		mthresh;		/* MTHRESHold in chars */

/*
 * qst:
 * Do a quicksort
 * First, find the median element, and put that one in the first place as the
 * discriminator.  (This "median" is just the median of the first, last and
 * middle elements).  (Using this median instead of the first element is a big
 * win).  Then, the usual partitioning/swapping, followed by moving the
 * discriminator into the right place.  Then, figure out the sizes of the two
 * partions, do the smaller one recursively and the larger one via a repeat of
 * this code.  Stopping when there are less than THRESH elements in a partition
 * and cleaning up with an insertion sort (in our caller) is a huge win.
 * All data swaps are done in-line, which is space-losing but time-saving.
 * (And there are only three places where this is done).
 */

static void
qst(char *base, char *max)
{
  register char *i, *j, *jj, *mid;
  register int ii, c;
  char *tmp;
  int lo, hi;

  lo = max - base;		/* number of elements as chars */
  do {
    /*
     * At the top here, lo is the number of characters of elements in the
     * current partition.  (Which should be max - base).
     * Find the median of the first, last, and middle element and make that the
     * middle element.  Set j to largest of first and middle.  If max is larger
     * than that guy, then it's that guy, else compare max with loser of first
     * and take larger.  Things are set up to prefer the middle, then the first
     * in case of ties.
     */
    mid = i = base + qsz * ((lo/qsz) >> 1);
    if (lo >= mthresh) {
      j = ((*qcmp) ((jj = base), i) > 0 ? jj : i);
      if ((*qcmp) (j, (tmp = max - qsz)) > 0) {
	j = (j == jj ? i : jj);	/* switch to first loser */
	if ((*qcmp) (j, tmp) < 0)
	  j = tmp;
      }
      if (j != i) {
	ii = qsz;
	do {
	  c = *i;
	  *i++ = *j;
	  *j++ = c;
	}
	while(  --ii  );
      }
    }
    /*
     * Semi-standard quicksort partitioning/swapping
     */
    for (i = base, j = max - qsz; ;) {
      while (i < mid && (*qcmp) (i, mid) <= 0)
	i += qsz;
      while (j > mid) {
	if ((*qcmp) (mid, j) <= 0) {
	  j -= qsz;
	  continue;
	}
	tmp = i + qsz;		/* value of i after swap */
	if (i == mid) {		/* j <-> mid, new mid is j */
	  mid = jj = j; 
	}
	else {			/* i <-> j */
	  jj = j;
	  j -= qsz;
	}
	goto  swap;
      }
      if (i == mid) {
	break;
      }
      else {			/* i <-> mid, new mid is i */
	jj = mid;
	tmp = mid = i;		/* value of i after swap */
	j -= qsz;
      }
    swap:
      ii = qsz;
      do {
	c = *i;
	*i++ = *jj;
	*jj++ = c;
      }
      while (--ii);
      i = tmp;
    }
    /*
     * Look at sizes of the two partitions, do the smaller one first by
     * recursion, then do the larger one by making sure lo is its size,
     * base and max are update correctly, and branching back.
     * But only repeat (recursively or by branching) if the partition is
     * of at least size THRESH.
     */
    i = (j = mid) + qsz;
    if ((lo = j - base) <= (hi = max - i)) {
      if (lo >= thresh) qst (base, j);
      base = i;
      lo = hi;
    }
    else {
      if (hi >= thresh)
	qst (i, max);
      max = j;
    }
  }
  while (lo >= thresh);
}

/*
 * qsort:
 * First, set up some global parameters for qst to share.  Then, quicksort
 * with qst(), and then a cleanup insertion sort ourselves.  Sound simple?
 * It's not...
 */

void
gqsort(char *base, int n, int size, int (*compar)(void *, void *))
{
  register char *i, *j, *lo, *hi, *min;
  register int c;
  char *max;

  if (n <= 1)  return;
  qsz = size;
  qcmp = compar;
  thresh = qsz*THRESH;
  mthresh = qsz*MTHRESH;
  max = base + n*qsz;
  if (n >= THRESH) {
    qst (base, max);
    hi = base + thresh;
  }
  else {
    hi = max;
  }
  /*
   * First put smallest element, which must be in the first THRESH, in
   * the first position as a sentinel.  This is done just by searching
   * the first THRESH elements (or the first n if n < THRESH), finding
   * the min, and swapping it into the first position.
   */
  for (j = lo = base; (lo += qsz) < hi; ) {
    if ((*qcmp) (j, lo) > 0)
      j = lo;
  }
  if (j != base) {		/* swap j into place */
    for (i = base, hi = base + qsz; i < hi;) {
      c = *j;
      *j++ = *i;
      *i++ = c;
    }
  }
  /*
   * With our sentinel in place, we now run the following hyper-fast
   * insertion sort.  For each remaining element, min, from [1] to [n-1],
   * set hi to the index of the element AFTER which this one goes.
   * Then, do the standard insertion sort shift on a character at a time
   * basis for each element in the frob.
   */
  for (min = base; (hi = min += qsz) < max;) {
    while ( (*qcmp) (hi -= qsz, min) > 0);
    if ((hi += qsz) != min) {
      for (lo = min + qsz; --lo >= min;) {
	c = *lo;
	for (i = j = lo; (j -= qsz) >= hi; i = j) *i = *j;
	*i = c;
      }
    }
  }
}

void
gqsort_ins_thresh(char *base, int n, int size, int (*compar)(void *, void *),
		  int ins_thresh)
{
  register char *i, *j, *lo, *hi, *min;
  register int c;
  char *max;

  if (n <= 1)  return;
  qsz = size;
  qcmp = compar;
  thresh = qsz*ins_thresh;
  mthresh = qsz*MTHRESH;
  max = base + n*qsz;
  if (n >= ins_thresh) {
    qst (base, max);
    hi = base + thresh;
  }
  else {
    hi = max;
  }
  /*
   * First put smallest element, which must be in the first ins_thresh, in
   * the first position as a sentinel.  This is done just by searching
   * the first ins_thresh elements (or the first n if n < ins_thresh), finding
   * the min, and swapping it into the first position.
   */
  for (j = lo = base; (lo += qsz) < hi; ) {
    if ((*qcmp) (j, lo) > 0)
      j = lo;
  }
  if (j != base) {		/* swap j into place */
    for (i = base, hi = base + qsz; i < hi;) {
      c = *j;
      *j++ = *i;
      *i++ = c;
    }
  }
  /*
   * With our sentinel in place, we now run the following hyper-fast
   * insertion sort.  For each remaining element, min, from [1] to [n-1],
   * set hi to the index of the element AFTER which this one goes.
   * Then, do the standard insertion sort shift on a character at a time
   * basis for each element in the frob.
   */
  for (min = base; (hi = min += qsz) < max;) {
    while ( (*qcmp) (hi -= qsz, min) > 0);
    if ((hi += qsz) != min) {
      for (lo = min + qsz; --lo >= min;) {
	c = *lo;
	for (i = j = lo; (j -= qsz) >= hi; i = j) *i = *j;
	*i = c;
      }
    }
  }
}

/*
 * qst:
 * Do a quicksort
 * First, find the median element, and put that one in the first place as the
 * discriminator.  (This "median" is just the median of the first, last and
 * middle elements).  (Using this median instead of the first element is a big
 * win).  Then, the usual partitioning/swapping, followed by moving the
 * discriminator into the right place.  Then, figure out the sizes of the two
 * partions, do the smaller one recursively and the larger one via a repeat of
 * this code.  Stopping when there are less than THRESH elements in a partition
 * and cleaning up with an insertion sort (in our caller) is a huge win.
 * All data swaps are done in-line, which is space-losing but time-saving.
 * (And there are only three places where this is done).
 */

static void
qst_int(u_int *base, u_int *max)
{
  register u_int *i, *j, *jj, *mid;
  register u_int c;
  u_int *tmp;
  int lo, hi;

  lo = max - base;		/* number of elements */
  do {
    /*
     * At the top here, lo is the number of elements in the
     * current partition.  (Which should be max - base).
     * Find the median of the first, last, and middle element and make that the
     * middle element.  Set j to largest of first and middle.  If max is larger
     * than that guy, then it's that guy, else compare max with loser of first
     * and take larger.  Things are set up to prefer the middle, then the first
     * in case of ties.
     */
    mid = i = base + (lo >> 1);
    if (lo >= mthresh) {
      jj = base ;
      j = (*jj > *i ? jj : i);
      tmp = max - 1 ;
      if (*j > *tmp ) {
	j = (j == jj ? i : jj);	/* switch to first loser */
	if (*j < *tmp )
	  j = tmp;
      }
      if (j != i) {
	c = *i;
	*i++ = *j;
	*j++ = c;
      }
    }
    /*
     * Semi-standard quicksort partitioning/swapping
     */
    for (i = base, j = max - 1; ;) {
      while (i < mid && *i <= *mid )
	i++;
      while (j > mid) {
	if ( *mid <= *j ) {
	  j--;
	  continue;
	}
	tmp = i + 1;		/* value of i after swap */
	if (i == mid) {		/* j <-> mid, new mid is j */
	  mid = jj = j; 
	}
	else {			/* i <-> j */
	  jj = j;
	  j--;
	}
	goto  swap;
      }
      if (i == mid) {
	break;
      }
      else {			/* i <-> mid, new mid is i */
	jj = mid;
	tmp = mid = i;		/* value of i after swap */
	j--;
      }
    swap:
      c = *i;
      *i++ = *jj;
      *jj++ = c;
      i = tmp;
    }
    /*
     * Look at sizes of the two partitions, do the smaller one first by
     * recursion, then do the larger one by making sure lo is its size,
     * base and max are update correctly, and branching back.
     * But only repeat (recursively or by branching) if the partition is
     * of at least size THRESH.
     */
    i = (j = mid) + 1;
    if ((lo = j - base) <= (hi = max - i)) {
      if (lo >= thresh) qst_int (base, j);
      base = i;
      lo = hi;
    }
    else {
      if (hi >= thresh)
	qst_int (i, max);
      max = j;
    }
  }
  while (lo >= thresh);
}


void
gqsort_int(u_int *base, int n, int ins_thresh)
{
  register u_int *i, *j, *lo, *hi, *min;
  register int c;
  u_int *max;

  if (n <= 1)  return;
  thresh = ins_thresh;
  mthresh = MTHRESH;
  max = base + n;
  if (n >= ins_thresh) {
    qst_int (base, max);
    hi = base + thresh;
  }
  else {
    hi = max;
  }
  /*
   * First put smallest element, which must be in the first ins_thresh, in
   * the first position as a sentinel.  This is done just by searching
   * the first ins_thresh elements (or the first n if n < ins_thresh), finding
   * the min, and swapping it into the first position.
   */
  for (j = lo = base; (lo += 1) < hi;) {
    if ( *j > *lo )
      j = lo;
  }
  if (j != base) {		/* swap j into place */
    c = *j;
    *j = *base;
    *base = c;
  }
  /*
   * With our sentinel in place, we now run the following hyper-fast
   * insertion sort.  For each remaining element, min, from [1] to [n-1],
   * set hi to the index of the element AFTER which this one goes.
   * Then, do the standard insertion sort shift on a character at a time
   * basis for each element in the frob.
   */
  for (min = base; (hi = min += 1) < max;) {
    while (*(hi -= 1) > *min );
    if ((hi += 1) != min) {
      lo = min + 1;
      c = *lo;
      for (i = j = lo; (j -= 1) >= hi; i = j) *i = *j;
      *i = c;
    }
  }
}

/*****************************************************************************
  lmsort() ... 26 feb 88

  List merge sort, which sorts the list into ascending order retaining original
  ordering of any equal items.

  Can be used in place of QUICKSORT when a STABLE sort is required, sort times
  are comparable.

  Returns 0 unless there isn't enough memory in which case it returns -1.

  Amount of storage to malloc = sizeof(int) * (nel + 2) + sizeof(width) * nel
  
*****************************************************************************/

int
lmsort(char *base, int nel, int width, int (*comp)(char *, char *))
{
  register int * link, i, j, s, t, p, q, n_link = 0 ; 
  register char *temp ;
  register char * bp, * tp ;
  BOOL sorted = FALSE, end_of_pass = FALSE ;
  
  if ( nel <= 1 ) return(0) ;

  if ( (link = (int *) malloc((nel + 2) * sizeof(int))) == NULL) return(-1) ;
  if ( (temp =         malloc( nel * width))            == NULL) return(-1) ;

  /* L1 ... prepare 2 lists ... */

  link[0]       = 1 ;
  link[nel + 1] = 2 ;
  for (i = 1 ; i <= nel - 2 ; i++ ) link[i] = - (i + 2) ;
  link[nel - 1] = link[nel] = 0 ;

  while (! sorted) {            /* L2 ... Begin new pass ... */

    s = 0 ;			/* usually recent-est processed record of sublist */
    t = nel + 1 ;		/* end of previously output sublist */
    p = link[s] ;		/* for each pass p and q traverse lists */
    q = link[t] ;

    if (q == 0) { sorted = TRUE ; break ; }
    end_of_pass = FALSE ;

    while (! end_of_pass) {             /* L3 ... compare Kp : Kq ... */

      if (comp(base + ((p-1)*width), base + ((q-1)*width) ) <= 0) {

	if (link[s] < 0) link[s] = -p ;	/* L4 ... ADVANCE p ... */
	else             link[s] = p ;
	s = p ;
	p = link[p] ;
	if (p > 0 ) continue ;   	/* ... i.e. return to L3 ... */

	link[s] = q ;                 	/* L5 ... complete sublist ... */
	s       = t ;
	while (q > 0) { t = q ; q = link[q] ; }
      }
      else {           	        	/* L6 ... ADVANCE q ... */
	if (link[s] < 0) link[s] = -q ;
	else             link[s] = q;
	s = q ;
	q = link[q] ;
	if (q > 0) continue ;    	/* ... return to L3 ... */

	link[s] = p ;                   /* L7 ... complete sublist ...*/
	s       = t ;
	while (p > 0) { t = p ; p = link[p] ;}
      }
	
      p = -p ;                  	/* L8 ... end of pass ? ... */
      q = -q ;
      if (q == 0) {
	if (link[s] < 0) link[s] = -p ;
	else             link[s] =  p ;
	link[t]                  =  0 ;
	end_of_pass = TRUE ;
      }
      else
	end_of_pass = FALSE ; 

    } /*  */

  } /* while not sorted */

  bp = base ;
  tp = temp ;
  for (i = 0 ; i < nel ; i++ ) {              /* base into sorted order */
    n_link  = link[n_link] ;
    bp      = base + (width*(n_link-1)) ;
    for ( j = 0 ; j < width ; j++ )
      *tp++ = *bp++ ;
  }
  memcpy(base, temp, nel * width) ;

  free(link) ;
  free(temp) ;

  return(0) ;
}
    

/***************************************************************************/

/* Looks for S1 as substring of S2 at or after Pos
   If s1 is not a substring of s2 at or after 
   character number pos, issub returns 0, 
   otherwise it returns the character position  
   in s2 where s1 starts (1..L2) */
int
issub(char *s1, int l1, char *s2, int l2, int pos) 
{
  if ( pos == 0 ) pos = 1 ;
  l2 = l2 - pos + 1 ; s2 += pos - 1 ;
  if ( l1 == 0 ) return(1) ; /* Empty string is a substring of any string */
  if ( l2 <= 0 ) return(0) ; /* Empty string doesn't have any nonempty sub-
				strings */
  for ( ; l2 >= l1 ; pos++, l2--, s2++ )
    if ( isequal(s1, s2, l1) ) return(pos) ;
  return(0) ;
}

                      
/*****************************************************************************

  Search for & replace sub1 by sub2 

  (See also Replace, which doesn't search)

  Replace at most COUNT occurrences of SUB1 in WORD at or after POS (1..len)
   by SUB2.  If count = 0 all occurrences are replaced. The output length
   is returned in Lw. The function returns the 
   character position immediately following the end of the last replacement, 
   or zero if no replacements.

   It is the programmers reponsibility to ensure that the output string (word) 
   has enough empty space to take any lengthening that may result.

   Example call: replace("s", 1, "", 0, word, &length, length, 0)  
   removes trailing 's' 

*****************************************************************************/

int
srchrep(char *sub1, int l1, char *sub2, int l2, char *word, int *lw,
	int pos, int count)
{
  if ( count == 0 ) count = 65535 ;
  while ( count-- && (pos = ( issub(sub1, l1, word, *lw, pos) )) ) {
    *lw = replace(word, *lw, pos, l1, sub2, l2) ;
    pos += l2 ;
  }
  return(pos) ;
}


/*****************************************************************************

  replace

  See also Srchrep, which does search and replace if found.

   Replace replaces lp bytes at pos (1..ls ; pos = 0 is treated as pos = 1) 
   of s by string t of length lt.  If lp = 0, t is inserted in front of the 
   pos'th character of s. If the substring to be replaced would extend beyond 
   the end of s, repsubst does nothing. Lt can be greater than lp - bytes are 
   moved up to make room for it. The programmer must avoid acess violations 
   (i.e. ensure there is enough room for any lengthening of S). To append to 
   a string, do Replace(s, ls, ls, 0, t, lt).

   Examples: replace("goat", 4, 1, 1, "G", 1) does "goat" -> "Goat"
		    ("goat", 4, 5, 0, "herd", 4) does -> "goatherd"
		    ("goat", 4, 1, 4, "", 0) does -> ""

   Replace always returns the new length of S (even if nothing has been done).
  
*****************************************************************************/

int
replace(char *s, int ls, int pos, int lp, char *t, int lt)
{
  if ( pos == 0 ) pos = 1 ;
  if ( lp  + pos - 1 > ls ) return(ls) ; /* and do nothing */
  (void)memmove((void *)(s+pos-1+lt), (void *)(s+pos-1+lp), ls-(pos-1)-lp) ;
  (void)memmove((void *)(s+pos-1), (void *)t, lt) ;
  return ls+(lt-lp) ;
}

/*****************************************************************************

  Logical functions

*****************************************************************************/


/******************************************************************************

  Returns 0 if ch not in terminated string  table, otherwise position ( 1.. )
  of ch in tab.

  NB: null character cannot be found 

  Correction Jan 1990 to handle empty table

  Effectively same as strchr() but returns int offset instead of ptr,
  strchr() wd be more efficient unless table short 

******************************************************************************/


int
isintab(register int ch, register u_char *table)
{
  register int i = 1 ; 

  while ( ch != *table ) { 
    if ( *table == '\0' ) return(0) ;
    table++ ;
    i++ ; 
  }
  return(i) ;
}

/* inw() returns position (1 - n) of int in int array of length n
   or 0 if not present */

int
inw(register int i, register int *table, register int n)
{
  register int j ;
 
  for ( j = 0 ; j < n ; j++, table++ ) 
    if ( i == *table ) return j + 1 ;
  return 0 ;
}

/* Find pos (1..l, or 0 if none) of first non-digit.
   Used for identifying numerals, dates etc. */
int
posnondig(u_char *s, int l) 
{
  register int pos = 1 ;
  while ( l-- ) {
    if ( ! isdigit(*s) ) return(pos) ;
    pos++ ; s++ ;
  }
  return(0) ;
}

   
/* Find position (1..length) of an element of 
   the table Chars in length l of string s.
   Returns 0 if not found. */
/* gcc -O3 appears to inline the isintab code */

int
findchars(u_char *chars, u_char *s, int length)
{
  register int pos = 0 ;
  while ( ++pos <= length ) if ( isintab((int)*s++, chars) ) return(pos) ;
  return(0) ;
}

/* Returns the number of occurrences of Ch in 
   the next Length characters of s. */
/* See also count_toks() (extsubs.c) */

int
countchar(u_int ch, u_char *s, int length) 
{
  int result = 0 ;
    if (length < 0)
    {
      fprintf(stderr, "\nWhoa, < 0 for length is unexpected!!\n");
    }
  while ( length > 0 && length-- ) 
  {
    if ( *s++ == ch )
    {
      result++ ;
    }
  }
  return(result) ;
}

/***********************************************************************/

/*                      Arithmetic etc       		               */

/***********************************************************************/

int
lg2(int n) /* Return least integer not less than log2(n) (log2(0) = 0) */
{
  register int power, res ;

  for ( power = 1, res = 0 ; power < n ; res++, power += power ) ;
  return(res) ;
}


/*****************************************************************************
  ispower2()                                                         Feb 1990
                                                                     A. Goker
  Function returns TRUE or FALSE depending on whether the number passed
  to it is a power of 2 (ie. 1,2,4,8...)
*****************************************************************************/

int
ispower2(int num)
{
  int powerof2, oldpowerof2;

  for ( oldpowerof2=powerof2=1; powerof2<=num;
       oldpowerof2=powerof2, powerof2+=powerof2 );
  return (oldpowerof2 == num);
}

/*****************************************************************************

  n_choose_r

  Returns 0 on error

  Should of course be some kind of integer

*****************************************************************************/

double 
n_choose_r(int n, int r)
{
  double res ;
  double dn = n, dr = r ;
  int j ;

  if ( n < 0 || r > n || r < 0 ) return 0 ;
  if ( n == 0 || r == 0 ) return 1 ;
  for ( j = 1, res = 1 ; j <= r ; j++ ) {
    res *= dn/dr ;
    dn = dn - 1 ; dr = dr - 1 ;
  }
  return res ;
}

/*****************************************************************************

  lige(n, m)

  Least integer not less than (float) n / (float) m

*****************************************************************************/

int
lige(int n, int m)
{
  if ( n % m ) return n/m + 1 ;
  else return n/m ;
}




/***********************************************************************/

/*                      Case conversion etc			       */

/***********************************************************************/

u_char U_to_lc[]={
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
  64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
  128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
  144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
  160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
  208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
  224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
  240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255   
} ;



void
lcase(char *s)
{
  while (*s) {
    *s = U_to_lc[(int)(*s)] ;
    s++ ;
  }
}

/* convert known-length string to UC in place */
void
cvlus(char *s, int l)  
{
  while ( l-- ) { *s = cvlu(*s) ; s++ ; }
}

/* convert known-length string to lc in place */
void
cvuls(char *s, int l)  
{
  while ( l-- ) { *s = cvul(*s) ; s++ ; }
}

/* Convert pc string to lc in place */
void
cvulpcs(char *s)  
{
  cvuls(s+1, len(s)) ; 
}

/******************************************************************************

  is_upper_text() returns TRUE if all letters in the input are UC

  is_upper_tok() returns TRUE if all the input is letters and
  all the letters are UC

  Similarly for is_lower_text() and is_lower_tok()

  is_init_upper() returns TRUE if input starts with a UC letter and any
  other letters are lower (silly to call it with a single letter).

  All return FALSE if the string is empty.

******************************************************************************/

int
is_upper_text(char *s, int l)
{
  if ( l <= 0 ) return FALSE ;
  while ( l-- ) 
    if (  tok_lower(*s++) )
      return FALSE ;
  return TRUE ;
}

int
is_upper_tok(char *s, int l)
{
  if ( l <= 0 ) return FALSE ;
  while ( l-- ) {
    if ( ! tok_upper(*s++) )
      return FALSE ;
  }
  return TRUE ;
}

int
is_lower_text(char *s, int l)
{
  if ( l <= 0 ) return FALSE ;
  while ( l-- ) 
    if ( tok_upper(*s++) )
      return FALSE ;
  return TRUE ;
}

int
is_lower_tok(char *s, int l)
{
  while ( l-- ) {
    if ( ! tok_lower(*s) )
      return FALSE ;
    s++ ;
  }
  return TRUE ;
}

int
is_init_upper(char *s, int l)
{
  if ( l <= 0 || ! tok_upper(*s) )
    return FALSE ;
  l-- ; s++ ;
  while ( l-- )
    if ( tok_upper(*s++) )
      return FALSE ;
  return TRUE ;
}

/***********************************************************************/

/*                      Time functions                                 */

/***********************************************************************/

u_int Secs;  /* stores the number of seconds since creation (1st Jan 1970) */

long
getsecs(void)			/* returns current time */ 

{
  return (long)time(NULL) ; 
} 


/* Time elapsed since since. If since = 0 it is taken as Secs. */

long
elapsed(int since)
{
  if ( since == 0 ) since = Secs ;
  return (long)(time(NULL) - since); 
}

/* Returns number of ticks since previous call. NB first call returns rubbish.
   Altered to use gettimeofday() Nov 94 as ftime() not in SysV */

int
ticks_elapsed(void)
{
  static int oldsecs, oldmils ;
  struct timeval tp ;
  struct timezone tz ;
  int ticks ;

  gettimeofday(&tp, &tz) ;

  ticks = ((int) (tp.tv_sec) - oldsecs) * 50 +
    ((int) (tp.tv_usec / 1000) - oldmils)/20;
  oldsecs = (int) tp.tv_sec ;
  oldmils = (int) tp.tv_usec / 1000 ;
  return(ticks) ;
}
  

int
updatesecs(int since)		/* Same as elapsed() but updates Secs */
{ 
  if ( since == 0 ) since = Secs ;
  Secs = (time(NULL));
  return (int)(Secs - since);
}

#ifndef NO_SETITIMER
int
start_timers(struct itimerval *val)
{
  int j ;
  for ( j = 0 ; j < 3 ; j++ ) {	/* Ugh */
    if (setitimer(j, val, (struct itimerval *)NULL) == -1 )
      fprintf(stderr, "Can't set timer %d\n", j) ;
  }
  return 0 ;
}

double
get_utime(void) 
{
  double timeleft ;
  struct itimerval val ;
  getitimer(ITIMER_VIRTUAL, &val) ;
  timeleft = (double)val.it_value.tv_sec + (double)(val.it_value.tv_usec)/1000000 ;
  return (double)TIMER_LOAD_VAL - timeleft ;
}

double
get_stime(void) 
{
  double timeleft ;
  struct itimerval val ;
  getitimer(ITIMER_PROF, &val) ;
  timeleft = (double)val.it_value.tv_sec + (double)(val.it_value.tv_usec)/1000000 ;
  return (double)TIMER_LOAD_VAL - timeleft - get_utime() ;
}
#endif /* NO_SETITIMER */
/*****************************************************************************

  is_a_num ... 25 Jan 88

  checks whether string is all digit 

*****************************************************************************/

int
is_a_num(char *num, int length)
{
  int i, no_points = 0 ;
  BOOL found ;
  char *ptr ;

  for (i = 0, ptr = num ; i < length ; i++, ptr++ ) {

    if (   tok_digit(*ptr)
	|| *ptr == 'l'
	|| *ptr == 'O'
	|| *ptr == ','
	|| (*ptr == '.' && ++no_points <= 1 ) ) found = TRUE ;
    else return(FALSE) ;
  }
  return(found) ;
}


/*****************************************************************************

pcit(), psit() 28 Jan 88 - put character/string into null-terminated table.
rsft() 15 Feb 88 - Remove string from table (only removes one copy) - 
doesn't matter if it wasn't there in the first place.
pssit() 10 Feb 88 - puts n <= 20 strings into table

Inserts character at end of table and puts null on end.  No check for
overflow. If character was already in table does nothing. Returns position
(1..) of ch in table.

 *****************************************************************************/

int
pcit(u_int ch, u_char *table)
{
  register int count ;
  for ( count = 1 ; *table && *table != ch ; table++, count++ ) ;
  if ( *table != ch ) {
    *table++ = ch ;
    *table = 0 ;
  }
  return(count) ;
}

/*****************************************************************************

  psit()

  Checks whether the terminated string "string" is in the null-terminated
  table of precount strings "table". If it is not, psit() adds the string
  as a precount string to the end of the table, and terminates the table.

  After some calls of psit(), the table contains

    <precount1><string1>.....'\0'

  This function is mainly used to set up tables of valid commands for
  calls of getcmnd().

  Warning: be sure that the first time psit() is called for a table the table
  has been initialised by putting '\0' in its first byte.

*****************************************************************************/

int
psit(char *string, char *table)
{
  register int l = strlen(string) ;
  register int count ;
  for ( count = 1 ; *table && cpbytes(string, l, table + 1, len(table)) != 0 ;
       table += len(table) + 1, count++ ) ;
  if ( len(table) == 0 ) {	/* I.e. have reached end of table */
    *table++ = l ;
    memcpy(table, string, l) ;
    table += l ;
    *table = 0 ;
  }
  return(count) ;		/* What is count for? */
}

void
rsft(char *string, char *table)
{
  register int l = strlen(string) ;
  char * p ;

  for ( ; *table && cpbytes(string, l, table + 1, len(table)) != 0 ;
       table += len(table) + 1 ) ;
  if ( len(table) == 0 ) return ; /* Have reached end of table */
  /* Have found string. Move rest of table down to overwrite it. */
  p = table + l + 1 ;
  l = strlen(p) + 1 ;		/* Includes the null byte at the end */
  memcpy(table, p, l) ;
  return ;
}

/*****************************************************************************
  search_for_command() 28 Jan 88 - searches for given length string in
  null-terminated sequence of precount strings (e.g. acceptable commands).
  Search succeeds if source string is a left substring of at least one of
  the target strings. Returns number of matches and the absolute address
  of the first match.
  
*****************************************************************************/

int
search_for_command(char *source, int length, u_char *targets, u_char **first)
{
  register u_char * addr ;
  register int l, count ;
  for ( addr = targets, count = 0 ; (l = len(addr)) != 0 ; addr += l + 1 ) {
    if (length > l) continue ;	/* Can't be a left match */
    if ( cpbytes(source, length, addr + 1, length) == 0 ) {
      count++ ;
      if ( count == 1 ) *first = addr ;
    }
  }
  return(count) ;
}
  
/*****************************************************************************

  read_file_buf  July 1990 SW
  Reads text file given by name into given buffer of length max, terminates
  with \0.
  
  Returns amount read or EOF if file couldn't be opened.
  
  file_to_buf does same for open file (doesn't close it)

  Note these funcs don't indicate whether they have truncated the input,
  but if return value is max - 1 this is a strong indication.

*****************************************************************************/

int
read_file_buf(char *name, char *buf, int max)
{
  register FILE *g ;
  register int n_read = 0 ;
  register char *ip = buf ;
  register int ch ;

  if ((g = fopen(name, "r")) == NULL) return EOF ;
  while ( (ch = getc(g)) != EOF && n_read < max - 1 ) {
    *ip++ = ch ;
    n_read++ ;
  }
  *ip = '\0' ;

  fclose(g) ;
  return n_read ;
}

int
file_to_buf(FILE *input, char *buf, int max)
{
  register int n_read = 0 ;
  register char *ip = buf ;
  register int ch ;

  if ( ferror(input) ) return EOF ;
  while ( (ch = getc(input)) != EOF && n_read < max - 1 ) {
    *ip++ = ch ;
    n_read++ ;
  }
  *ip = '\0' ;
  return n_read ;
}

/*****************************************************************************

  Bit manipulation

*****************************************************************************/

/*****************************************************************************

  is_a_bit

  Returns TRUE if its (unsigned) arg has precisely one bit set else FALSE
*****************************************************************************/

int
is_a_bit(u_int v)
{
  register u_int j ;

  for ( j = 1 ; j <= v ; j = (j << 1) ) {
    if ( j == v ) return TRUE ;
    if ( j == 0x80000000 ) return FALSE ;
  }
  
  return FALSE ;
}

/*****************************************************************************

  bits_to_right(val, bit)

  Returns the number of 1 bits to the right of a specified bit (given as
  an unsigned int) in an unsigned value.

  If the specified bit is not a power of two <= 2^31 or the specified bit
  in the value is not set it returns -1

  Examples: btr(1, 1) = 0, btr(3, 16) = -1 [bit not set], btr(3, 3) = -1
  [bit not a power of 2], btr(0xffffffff, 0x80000000) = 31

  Used for locating the appropriate NP field in an Okapi Index Term Record
  (type 1 index)

*****************************************************************************/

int
bits_to_right(u_int value, u_int bit)
{
  register int count;
  register u_int j ;

  if ( ! is_a_bit(bit) ) return -1 ;
  if ( (value & bit) == 0 ) return -1 ;
  for (count = 0, j = 1 ; j < bit && j < 0x80000000 ; j = j << 1,
       value = value >> 1) 
    if ( value & 1 ) count++ ;
  return count ;
}

/*****************************************************************************

  num_bits

  Returns number of bits set in its argument

*****************************************************************************/

int
num_bits(u_int value)
{
  int count = 0 ;
  
  while ( value ) {
    if ( value & 1 ) count++ ;
    value = value >> 1 ;
  }
  return count ;
}

/*****************************************************************************

  qaddslash  SW Feb 92

  Add slash to end of terminated string if it hasn't got one (for directories)
  Returns length of string after, or -1 if it was NULL

*****************************************************************************/

int
qaddslash(char *s)
{
  int l ;
  if ( s == NULL ) return -1 ;
  l = strlen(s) ;
  /*if ( *(s + l - 1) != '/' ) {*/
  if ( *(s + l - 1) != SLASH_CHAR ) {
    /**(s + l) = '/' ;*/
    *(s + l) = SLASH_CHAR ;
    *(s + l + 1) = '\0' ;
    l++ ;
  }
  return l ;
}


/*****************************************************************************

  A few fns moved here from dskutils.c May 93

  rea() and writ() removed June 02 (no longer used)

*****************************************************************************/

/*****************************************************************************

  readstr(f, buf, max)  SW Oct 93

  Reads from file and transfers to buf until it has read a '\0', max
  chars have been transferred (in which case the last is pushed back unless
  it is a '\0', and replaced in the output by '\0'), or EOF is read.

  It returns the number of characters WRITTEN including the terminator.
  Thus 1 indicates an empty string and 0 EOF before anything read.

*****************************************************************************/

int
readstr(FILE *f, char *buf, int max)
{
  int count = 0 ;
  int ch ;
  if ( max <= 0 ) return 0 ;
  while ( count < max ) {
    if ( (ch = getc(f)) == EOF ) break ;
    *buf++ = ch ;
    count++ ;
    if ( ch == 0 ) break ;
  }
  if ( count == max && ch != 0  ) {
    *--buf = '\0' ;
    if ( ch != EOF ) ungetc(ch, f) ;
  }
  return count ;
}


/*****************************************************************************

  readcstr(f, buf, ch, max, &error)  SW Apr 02

  Reads from file and transfers to buf until it has read a terminating
  character ch, which is discarded, max - 1 chars have been transferred
  or EOF is read. In any case a terminating NULL is written.

  It returns the number of characters written excluding the NULL
  i.e. strlen(output).

  If the buffer is full (max - 1 chars read) without reading a terminator
  error is set to 1.

  If EOF or error is reached without a terminator being read error
  is set to EOF.


  readstrstr(f, buf, str, max, &error) does the same for a string instead of
  a character

*****************************************************************************/

int
readcstr(FILE *f, char *buf, int terminator, int max, int *error)
{
  int count = *error = 0 ;
  int ch ;
  if ( max <= 0 ) return 0 ;
  max-- ;
  while ( (ch = getc(f)) != terminator && ch != EOF && count < max ) {
    *buf++ = ch ;
    count++ ;
  }
  *buf = '\0' ;
  if ( ch != terminator ) {
    if ( count == max ) *error = 1 ;
    else *error = EOF ;
  }
  return count ;
}

int readstrstr(FILE *f, char *buf, char *terminator, int max, int *error)
{
  int count = *error = 0 ;
  int ch ;
  int l = strlen(terminator) ;
  int lc = *(terminator + l - 1) ;
  int gotit = 0 ;

  max-- ;
  while ( count < max && (ch = getc(f)) != EOF ) {
    *buf++ = ch ;
    count++ ;
    if ( ch == lc && count >= l ) {
      if ( strncmp(buf - l, terminator, l) == 0 ) {
	*buf = '\0' ;
	gotit = 1 ;
	break ;
      }
    }
  }
  if ( ! gotit ) {
    if ( count == max ) *error = 1 ;
    else *error = EOF ;
  }
  return count ;
}

/*****************************************************************************

  readline()

  Reads line terminated by newline character from file into buf. Newline is
  also read. Does not write null char into buf. For this see rdln.

  If EOF and no newline but length > 0 newline is added.
  If line longer than max - 1 only max - 1 characters (and a newline) are
  read into the buffer, but the file pointer is advanced through the newline.
  Returns length of line (including newline) output, or EOF if eof or error
  and no chars read.

  See also fgets(), which could doubtless be used instead.

*****************************************************************************/

int
readline(FILE *f, char *buf, int max)
{
  register int ch, l = 0 ;
  register char *op = buf ;

  if ( f == NULL ) return EOF ;
  while ( ( ch = getc(f) ) != '\n' && l < max - 1 && ch != EOF ) {
    *op++ = ch ;
    l++ ;
  }
  if ( ch == EOF && l == 0 ) return EOF ;
  if ( l == max - 1 && ch != '\n' && ch != EOF ) {
    while ( ( ch = getc(f) ) != '\n' && ch != EOF ) ;
  }
  *op = '\n' ;
  return l + 1 ;
}

/*****************************************************************************

  readln()

  Reads line terminated by newline character from file into buf. Does not
  write newline into buf. Writes null char on end.

  See also readline().

  If line longer than max - 1 only max -1 characters are read into the buffer,
  but the file pointer is advanced through the newline.
  Returns length of line (excluding terminating null char) or EOF if EOF or
  error.

  readlln() is same but may contain escaped newlines which are removed

  readxlln() same as readlln() but output stops with character before
  any member of string of terminators (e.g. # for comment)

*****************************************************************************/

int
readln(FILE *f, char *buf, int max)	
{
  register int ch, l = 0 ;
  register char *op = buf ;

  if ( f == NULL ) return EOF ;
  while ( ( ch = getc(f) ) != '\n' && l < max - 1 && ch != EOF ) {
    *op++ = ch ;
    l++ ;
  }
  if ( ch == EOF && l == 0 ) return EOF ;

  if ( l == max - 1 && ch != '\n' && ch != EOF ) {
    while ( ( ch = getc(f) ) != '\n' && ch != EOF ) ;
  }
  *op = '\0' ;

  return l ;
}

int
readlln(FILE *f, char *buf, int max)	
{
  register int ch, lch, l = 0 ;
  register char *op = buf ;

  if ( f == NULL ) return EOF ;
  lch = 0 ;
  while ( ( ch = getc(f) ) != EOF && l < max - 1 ) {
    if ( ch == '\n' ) {
      if ( lch == '\\' ) {
	op-- ;
	l-- ;
	lch = ch ;
	continue ;
      }
      else break ;
    }
    *op++ = ch ;
    l++ ;
    lch = ch ;
  }
  if ( ch == EOF && l == 0 ) return EOF ;

  if ( l == max - 1 && ch != EOF ) {
    /* Use up the rest */
    while ( ( ch = getc(f) ) != EOF ) {
      if ( ch == '\n' && lch != '\\' ) break ;
      lch = ch ;
    }
  }
  *op = '\0' ;

  return l ;
}

/*****************************************************************************

  skip_to_nextline  SW Feb 92

  Skip to start of next line in file. Intended for skipping comment lines
  starting with # etc. Returns the number of characters skipped, or EOF
  if EOF or error.

*****************************************************************************/

int
skip_to_nextline(FILE *f)
{
  register int l=0, ch ;

  if ( f == NULL ) return EOF ;
  while ( (ch = getc(f)) != EOF && ch != '\n' ) l++ ;
  if ( ch == EOF ) return EOF ;
  else return l + 1 ;
}

/*****************************************************************************

  lastslash(s)  SW July 90

  Returns pointer to the position after the last '/' in a terminated string
  or the beginning of the string if there isn't one

  dir = dirname(s)    SW Nov 95

  Almost identical to the dirname command.
  Returns modified string in static storage, max length 512
  If the rightmost slash is not in the initial position it returns the string
  up to but not including the rightmost slash (this differs
  from the dirname command, which removes the last segment even if there's
  a trailing slash).
  If slash is in initial position it returns slash.
  If there's no slash it returns . (including empty string input)
  If input is NULL it returns NULL
  
*****************************************************************************/

char *
lastslash(char *s)
{
  int l, i ;


  if ( s == NULL ) return NULL ;
  l = strlen(s) ;
  if ( l == 0 ) return s ;
  /*for ( i = l - 1 ; i > 0 && *(s + i) != '/' ; i-- ) ;
    if ( i == 0 && *s != '/' ) return s ; */
  for ( i = l - 1 ; i > 0 && *(s + i) != SLASH_CHAR ; i-- ) ;
  if ( i == 0 && *s != SLASH_CHAR ) return s ;
  else return s + i + 1 ;
}

/**********
 * NOTE : Function renamed to dirnameh from dirname because our
 *        system was always using the OTHER dirname command which
 *        would ruin the dirname string (not leaving it in original condition)
*/
char *
dirnameh(char *s)
{
  int j, l ;
  static char *out_s = NULL ;
  static int outsize = 0 ;

  if ( s == NULL ) return NULL ;
  l = strlen(s) ;
  if ( outsize < l + 1 ) {
    if ( out_s == NULL ) out_s = malloc(l + 1) ;
    else out_s = realloc(out_s, l + 1) ;
  }
  if ( out_s == NULL ) return NULL ;
  else outsize = l + 1 ;

  /*for ( j = l - 1 ; j >= 0 ; j-- ) if ( s[j] == '/' ) break ;*/
  for ( j = l - 1 ; j >= 0 ; j-- ) if ( s[j] == SLASH_CHAR ) break ;
  if ( j > 0 ) {
    memcpy(out_s, s, j) ; out_s[j] = '\0' ;
  }
  else if ( j == 0 ) {
    /*out_s[0] = '/' ;*/
    out_s[0] = SLASH_CHAR ;
    out_s[1] = '\0' ;
  }
  else {
    out_s[0] = '.' ;
    out_s[1] = '\0' ;
  }
  return out_s ;
}


/******************************************************************************

  Routines for reading/writing "1 1/2 byte" integers, 0 - 16383.

  If val > 127 it is held as (val / 128) | 0x80 + val % 128

  Conversely, if first byte doesn't have msb set val is first byte
  otherwise it is (first byte | 0xff) * 128 + second byte.

  The "get" functions return the value got, or -1 if there was a detectable
  error.

  The "put" functions return the number of bytes written (1 or 2), or zero
  on error.

  When reading from file the file pointer is advanced (of course)
  When reading from memory the pointer is not advanced. You may need
  to advance by 1 or 2 bytes according as the value read is < 128 or not.

  Nov 95: when reading from or writing to memory the argument memory
  pointer is advanced (passed as char **)

******************************************************************************/

#ifndef MMPUT

u_int
mget_vshort(u_char *p)
{
  unsigned u ;

  if ( p == NULL ) return -1 ;
  u = *p ;
  if ( u & 0x80 ) u = (u & 0x7f) * 128 + *(p + 1) ;
  return u ;
}

#endif

u_int
fget_vshort(FILE *f)
{
  unsigned u ;

  if ( f == NULL || (u = (unsigned)getc(f)) == (unsigned)EOF)
    return -1 ;
  if ( u & 0x80 ) u = (u & 0x7f) * 128 + (unsigned)getc(f) ;
  return u ;
}

#ifndef MMPUT
int
mput_vshort(int u, u_char *p)
{
  if ( u > 16383 || u < 0 ) return 0 ;
  if ( u > 127 ) {
    *p = ((u / 128) | 0x80) ;
    *(p + 1) = u % 128 ;
    return 2 ;
  }
  else {
    *p = u ;
    return 1 ;
  }
}
#endif

int
fput_vshort(int u, FILE *f)
{
  if ( u > 16383 || u < 0 ) return 0 ;
  if ( u > 127 ) {
    if ( f == NULL || putc(((u / 128) | 0x80), f) == EOF ) return 0 ;
    putc((u % 128), f) ;
    return 2 ;
  }
  else {
    if ( putc(u, f) == EOF ) return 0 ;
    return 1 ;
  }
}

/******************************************************************************

  Writing and reading to or from file variable length (1-4 byte) values
  with code in first byte giving length.

  Regardless of architecture values are stored LOHI and multiplied by 4 with
  a length code (= number of bytes - 1) in the 2 ls bits.

  w1to4 returns -2 if the value is too large (>0x3fffffff), EOF if there's been
  a write error, or the number of bytes written.

  r1to4 returns the value read (0-0x3fffffff) or EOF on file error.

  w3or4 and r3or4 are analogous (1 ls bit shows length and mult by 2)

  ld.. and st.. do same for memory read and write.

******************************************************************************/

int
w1to4(int u, FILE *f)
{
  char *au = (char *)&u ;
  int code ;
  unsigned v ;
  if ((unsigned)u > 0x3fffffff) return -2 ; /* Argument too big */
  if ( u < 0x40 ) code = 0 ;
  else if ( u < 0x4000 ) code = 1 ;
  else if ( u < 0x400000 ) code = 2 ;
  else code = 3 ;
  u = ( u << 2 ) | code ;
#if HILO
  u = reverse(u) ;
#endif
  v = *au++ ;
  putc(v, f) ;
  if ( code > 0 ) {
    v = *au++ ;
    putc(v, f) ;
    if ( code > 1 ) {
      v = *au++ ;
      putc(v, f) ;
      if ( code > 2 ) {
	v = *au ;
	putc(v, f) ;
      }
    }
  }
  if ( ferror(f) ) return EOF ;
  return code + 1 ;
}

int
r1to4(int *l, FILE *f)
{
  unsigned v, u ;
  int code ;
  v = getc(f) ;
  code = v & 3 ;
  u = v ;
  if ( code > 0 ) {
    switch (code) {
    case 3:
      v = getc(f) ;
      u += 0x100 * v ;
      v = getc(f) ;
      u += 0x10000 * v ;
      v = getc(f) ;
      u += 0x1000000 * v ;
      break ;
    case 2:
      v = getc(f) ;
      u += 0x100 * v ;
      v = getc(f) ;
      u += 0x10000 * v ;
      break ;
    case 1:
      v = getc(f) ;
      u += 0x100 * v ;
    }
  }
  u = u >> 2 ;
  if ( v == (unsigned)EOF ) return EOF ;
  *l = code + 1 ;
  return (int)u ;
}

int
w3or4(int u, FILE *f)
{
  char *au = (char *)&u ;
  int code ;
  unsigned v ;
  if ((unsigned)u > 0x7fffffff) return -2 ; /* Argument too big */
  if ( u < 0x800000 ) code = 0 ;
  else code = 1 ;
  u = ( u < 1 ) | code ;
#if HILO
  u = reverse(u) ;
#endif
  v = *au++ ;
  putc(v, f) ;
  v = *au++ ;
  putc(v, f) ;
  v = *au ;
  putc(v, f) ;
  if ( code > 0 ) {
    v = *(++au) ;
    putc(v, f) ;
  }
  if ( ferror(f) ) return EOF ;
  return code + 3 ;
}

int
st3or4(int u, char *m)
{
  char *au = (char *)&u ;
  int code ;
  if ((unsigned)u > 0x7fffffff) return -2 ; /* Argument too big */
  if ( u < 0x800000 ) code = 0 ;
  else code = 1 ;
  u = ( u < 1 ) | code ;
#if HILO
  u = reverse(u) ;
#endif
  *m++ = *au++ ;
  *m++ = *au++ ;
  *m++ = *au ;
  if ( code > 0 ) *m = *(++au) ;
  return code + 3 ;
}

int
r3or4(int *l, FILE *f)
{
  unsigned v, u ;
  int code ;
  v = getc(f) ;
  code = v & 1 ;
  u = v ;
  v = getc(f) ;
  u += 0x100 * v ;
  v = getc(f) ;
  u += 0x10000 * v ;
  if ( code > 0 ) {
    v = getc(f) ;
    u += 0x1000000 * v ;
  }
  u = u >> 1 ;
  if ( v == (unsigned)EOF ) return EOF ;
  *l = code + 3 ;
  return (int)u ;
}

int
ld3or4(int *l, char *m)
{
  unsigned v, u ;
  int code ;
  v = *m++ ;
  code = v & 1 ;
  u = v ;
  v = *m++ ;
  u += 0x100 * v ;
  v = *m ;
  u += 0x10000 * v ;
  if ( code > 0 ) {
    v = *(++m) ;
    u += 0x1000000 * v ;
  }
  u = u >> 1 ;
  *l = code + 3 ;
  return (int)u ;
}

int
w1or2(int v, FILE *f)
{
  u_int u = v ;
  int code ;

  if ( u > 0x7fff ) return -2 ;
  code = (u < 0x80) ? 0 : 1 ;
  u = (( u << 1 ) | code) ;
  putc((int)(u & 0xff), f) ;
  if ( code > 0 ) putc((int)((u & 0xff00) >> 8), f) ;
  if ( ferror(f) ) return EOF ;
  return code + 1 ;
}

int
r1or2(int *l, FILE *f)
{
  u_int v, u ;
  int code ;
  v = getc(f) ;
  code = v & 1 ;
  u = v ;
  if ( code > 0 ) {
    v = getc(f) ;
    u += v << 8 ;
  }
  if ( v == (u_int)EOF ) return EOF ;
  u = ( u >> 1 ) ;
  *l = code + 1 ;
  return u ;
}


/*****************************************************************************

  Display routines
  
*****************************************************************************/

#if HILO

char *
expand_pos_text(U_pos *p)
{
  static char output[32] ;
  sprintf(output, "%u.%u.%u.%u.%u", p->pthl.f, p->pthl.s, p->pthl.t, p->pthl.nt, p->pthl.sw) ;
  return output ;
}

char *
expand_pos6(U_pos *p)
{
  static char output[24] ;
  sprintf(output, "%u.%u.%u.%u.%u", p->lphl.f, p->lphl.s, p->lphl.t, p->lphl.nt, p->lphl.sw) ;
  return output ;
}

char *
expand_posnf(U_pos *p)
{
  static char output[24] ;
  sprintf(output, "%u.%u.%u.%u", p->pnfhl.s, p->pnfhl.t, p->pnfhl.nt, p->pnfhl.sw) ;
  return output ;
}

#else

char *
expand_pos_text(U_pos *p)
{
  static char output[32] ;
  sprintf(output, "%u.%u.%u.%u.%u", p->ptlh.f, p->ptlh.s, p->ptlh.t, p->ptlh.nt, p->ptlh.sw) ;
  return output ;
}

char *
expand_pos6(U_pos *p)
{
  static char output[24] ;
  sprintf(output, "%u.%u.%u.%u.%u", p->lplh.f, p->lplh.s, p->lplh.t, p->lplh.nt, p->lplh.sw) ;
  return output ;
}

char *
expand_posnf(U_pos *p)
{
  static char output[24] ;
  sprintf(output, "%u.%u.%u.%u", p->pnflh.s, p->pnflh.t, p->pnflh.nt, p->pnflh.sw) ;
  return output ;
}

#endif

/*****************************************************************************

  Fsize 6 July 87

  Sep 2000: probably stopped using fsize() as defective in CYGWIN

 *****************************************************************************/

OFFSET_TYPE
fsize(char *filename)		/* Returns size of named file, or -1 */
{
  struct stat file ; /* See sys/stat.h */
  if ( stat(filename, &file) == -1 ) return (OFFSET_TYPE)-1 ;
  return file.st_size ;
}

OFFSET_TYPE
ofsize(int desc)		/* Returns size of open file, or -1 */
{
  struct stat file ; /* See sys/stat.h */
  if ( fstat(desc, &file) == -1 ) return (OFFSET_TYPE)-1 ;
  return file.st_size ;
}



/*****************************************************************************

  dfk(path)  SW Sep 96

  Returns kbytes available on filesystem, path any filename in the system
  (Mod to return K July 97)

*****************************************************************************/

#ifndef NO_DF

int
dfk(char *path)
{
  int result ;
#ifdef NO_STATVFS
  struct statfs b ;
#else
  struct statvfs b ;
#endif

#ifdef NO_STATVFS
  result = statfs(path, &b) ;
#else
  result = statvfs(path, &b) ;
#endif
  if ( result != 0 ) return -1 ;
  /* fprintf(stderr, "bs=%d, b=%d, bfree=%d, bav=%d, ffree=%d\n",
	  b.f_bsize, b.f_blocks, b.f_bfree, b.f_bavail, b.f_ffree) ; */
#ifdef NO_STATVFS
  return (int)((double)b.f_bsize/1024.0 * (double)b.f_bavail) ;
#else
  return (int)((double)b.f_frsize/1024.0 * (double)b.f_bavail) ;
#endif
}

#endif /* NO_DF */

/******************************************************************************

  get(), fget(): get any number of bytes from anywhere in a file

  Return -1 on error or number of bytes transferred

******************************************************************************/

ssize_t
get(int fd, OFFSET_TYPE pos, char *buf, size_t n)
{
  if ( lseek(fd, pos, SEEK_SET) == (OFFSET_TYPE)-1 ) return (ssize_t)-1 ;
  return read(fd, buf, n) ;
}

#ifdef LARGEFILES
#ifdef NO_FSEEKO
int
fget(FILE *f, fpos_t pos, char *buf, int n)
#else
int
fget(FILE *f, OFFSET_TYPE pos, char *buf, int n)
#endif
#else
int
fget(FILE *f, long pos, char *buf, int n)
#endif
{
  register int j ;
  register char *p ;
  register int ch ;

#ifdef LARGEFILES
#ifdef NO_FSEEKO
  if ( f == NULL || fsetpos(f, &pos) != 0 ) return -1 ;
#else
  if ( f == NULL || fseeko(f, pos, 0) != 0 ) return -1 ;
#endif
#else
  if ( f == NULL || fseek(f, pos, 0) != 0 ) return -1 ;
#endif
  for ( j = 0, p = buf ; j < n ; j++, p++ ) {
    ch = getc(f) ;
    if ( ch != EOF ) *p = ch ;
    else break ;
  }
  return j ;
}

/** put: put any number of bytes in the 'buf'
         at the position 'pos' in a file.
                      16 feb 90 - T.DO          **/

ssize_t
put(int fd, OFFSET_TYPE pos, char *buf, size_t n)
{
  ssize_t iRet=-1;

  if ( lseek(fd, pos, SEEK_SET) >= (OFFSET_TYPE)0 ) iRet=write(fd, buf, n);
  return iRet ;
}


