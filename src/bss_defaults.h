#ifndef _bss_defaults_h
#define _bss_defaults_h

/******************************************************************************

  bss_defaults.h

  New Dec 97, will grow

******************************************************************************/

/******************************************************************************

  The following replace values in individual database parameters
  Don't know how good the values are for BM11 and BM15

  For BM11, 15 and 25 etc they are used direct by bss_do_combine()

******************************************************************************/

#define NONE_ASSIGNED 	-400

#define DEF_K1_BM11 0.8
#define DEF_K2_BM11 -1.0
#define DEF_K1_BM15 1.0
#define DEF_K2_BM15 .8
#define DEF_BM25_K1 1.2
#define DEF_BM25_K2 0.0
#define DEF_BM25_K3 0.0
#define DEF_BM25_B .75
#define DEF_BM25_B3 0.0
#define DEF_K2_BM40 0.0

#endif /* _bss_defaults_h */
