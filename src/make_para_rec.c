/******************************************************************************

  make_para_rec.c

  Extracted from make_parafile.c Nov 99 SW

  Only makes type 2 records (I don't see any point retaining other types)

  Record structure is:

  [fdnum (1-)
  offset of field from start of document
  field length
  number of paragraphs in field
  [offset from start of field of NEXT para
  sentence number (0-) of last sentence in PREV para]]

  where the whole thing repeats NF times and the last two fields
  number of paras in field times.

  fdnum is a single byte; all other fields are 1-4 byte encoded little-endian
  values (see w1to4() and r1to4())

  Returns length written or EOF

******************************************************************************/

#include "bss.h"
#include "para.h"


int
make_para_rec(int fdnum, 
	      char *fdstart, int fdlen,	/* Field text and length */
	      int toffset,	/* Offset of text in document */
	      FILE *parf,	/* Output files */
	      int maxtoks	/* Max tokens in a sentence */
	      )
{
  int paracount ;
  int paralen, l ;
  char *pp ;
  int se_num ;
  int sentence_count ;
  int outlen = 0 ;

  se_num = 0 ;
  putc(fdnum, parf) ; outlen++ ;
  outlen += w1to4(toffset, parf) ;
  outlen += w1to4(fdlen, parf) ;
  l = fdlen ; pp = fdstart ;
  paracount = count_paras(pp, l, INDENT|GAP) ;
  outlen += w1to4(paracount, parf) ;
  while ( l > 0 ) {
    paralen = find_para(pp, l, INDENT|GAP) ;
    /* Last arg temp, see the function (July 98) */
    sentence_count = count_sentences(pp, paralen, 0, maxtoks) ;
    se_num += sentence_count ;
    pp += paralen ; l -= paralen ;
    /* Then pp - 1 is last char in paragraph */
    /* Write out start of NEXT para, # of LAST sentence */
    outlen += w1to4(pp - fdstart, parf) ;
    outlen += w1to4(se_num - 1, parf) ;
  }
  if ( ferror(parf) ) outlen = EOF ;
  return outlen ;
}
