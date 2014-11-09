#ifndef _charclass_h
#define _charclass_h

/******************************************************************************

  charclass.h SW Jan 93

  Character classification on the lines of ctype.h

  Added TOK_PAREN Mar 30 93

******************************************************************************/

#define TOK_IGN 1
#define TOK_START 2
#define TOK_ESTART (TOK_START|TOK_UNDERSCORE)
#define TOK_FIN  4
#define TOK_SPEC 8
#define TOK_APUNCT 0x10
#define TOK_MPUNCT 0x20
#define TOK_STOP 0x40
#define TOK_VOWEL 0x80
#define TOK_CONS 0x100
#define TOK_UPPER 0x200
#define TOK_LOWER 0x400
#define TOK_LSZ 0x800
#define TOK_EILS 0x1000
#define TOK_WXY 0x2000
#define TOK_DIG 0x4000
#define TOK_ALPH (TOK_UPPER|TOK_LOWER)
#define TOK_ALNUM (TOK_DIG|TOK_ALPH)
#define TOK_QUOTE 0x8000
#define TOK_PARENS 0x10000
#define TOK_SPACE 0x20000
#define TOK_CLOSING 0x40000
#define TOK_UNDERSCORE 0x80000
#define TOK_AMPERSAND 0x100000

extern int Toktab[256] ;

#define tok_alnum(c) (Toktab[(unsigned)(c)] & TOK_ALNUM)
#define tok_alpha(c)  (Toktab[(unsigned)(c)] & TOK_ALPH)
#define tok_upper(c)  (Toktab[(unsigned)(c)] & TOK_UPPER)
#define tok_lower(c)  (Toktab[(unsigned)(c)] & TOK_LOWER)
#define tok_digit(c)  (Toktab[(unsigned)(c)] & TOK_DIG)
#define tok_space(c)  (Toktab[(unsigned)(c)] & TOK_SPACE)
#define tok_vowel(c)  (Toktab[(unsigned)(c)] & TOK_VOWEL)

#endif /* _charclass_h */
