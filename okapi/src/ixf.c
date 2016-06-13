/*****************************************************************************

  ixf_ex  Final index production program for Type 1 and Type 2 indexes
  Also Type 3 (Jan 93)
  (NOT type 0)
  Also type 4 (1994) and type 5 (1995)
  Also types 6 and 7 (1995) and 8 and 9 (1996)
  Also type 10 (Aug 97)
  Also types 11, 12, 13 (July 98)

  ixf_ex 26 Nov 91, based on ixf 
  Okapi final index production program for extended index (index type 1)
  with positional information. For input file structure see ix1_ex.c
  
  Some sections depend on "if ( itype == ? ) ", but much of the code is common.
  Some variables are unused but harmless if itype != 1

  Tested on 5000 cat recs 22 May 92: produced results consistent between types
  1 and 2, and type 1 index produced by this prog identical to index produced
  by previous ixf_ex

  Various dates: modified to prevent positional field overflow. Can also
  be restricted at runtime. See the code. 

  Jan 96: references to index types < 4 removed

  Substantial revision March 96: added new index types 8 and 9.

  March 96: index types 8 and 9 will replace 6 and 7. They differ in SI only
  and the layout is as follows:

  <termlen><term><lims><maxtf><ttf><tnp><plength><vol><ptr><termlen>
      1      v      2     2     4    4      4      1    4     1

  which is about another 7 bytes per ITR. For the layout of the other
  types see below (in the text somewhere, under itrlen).

  Aug 97: ix type 10 has identical si and pi, and postings files are
  the same except that there are never any positional records. The <tf>
  field just says that instead of the fact that there are <tf> positional
  records. The streamer input is different (see, e.g., ixsubs.c for a
  description).

  For all types, each SI chunk starts with 2-byte chunk word containing
  the number of bytes to go back to the start of the last ITR in the
  previous chunk, with the high bit set. Start of 1st chunk is 0x8000.
  (Not sure why have to have high bit set, this was something to do with not
  confusing with the start of a real ITR, but I think functions all know
  when a new chunk has been read.)

  For types 4 - 7 (obsolescent), last ITR in chunk is signalled at the end
  by the following 2 bytes being zero (would normally be the length of the
  next ITR). For 8 and 9, this is just one zero byte. For 4 - 7, end of index
  is a 2-byte 0x8000 value. For 8 and 9 this is a single 0xff byte (this
  means the maximum term length for 8 and 9 is 254).

  SI layout for types 12 and 13 (July 98) is like 8 and 9 but with an extra
  byte for <ptr> (allowing pf vols to be up to a terabyte)

  <termlen><term><lims><maxtf><ttf><tnp><plength><vol><ptr><termlen>
      1      v      2     2     4    4      4      1    5     1

  Type 11 has identical SI but (as 10) there are no pos recs in the pf.

  May 02: type 20 (profile type) is like 10 except that a posting record is
    <qtf><rec><wt>
      1    3   4
  The streamer input is <term><rec><wt><qtf>
  qtf may be (is usually) zero
  
*****************************************************************************/
#include "bss.h"

#define PF_FILL_FACTOR 0.98	/* When a multi-vol pstgs file vol reaches this
				   a new volume is started. */
#define DIAG 0
#define SPURIOUS_REC_LIMIT 2048 
#define OSEQLIM 2048

void Usage(char *prog) {
  char *usage = "[-c <control directory>] [-no_out] [-diag] [-silent]\n\
[-stdout {sends posting file to stdout}] [-version] [-help]\n\
[-report <reportlevel>] <database> <index number (0..%d)>\n\
Database name and index number must come last." ;
  char buf[512] ;
  sprintf(buf, usage, BSS_MAXIX - 1) ;
  fprintf(stderr, "Usage: %s %s\n", prog, buf) ;
  exit(1) ;
}

int
main(int argc, char *argv[])
{
  char *version = "3.01 20020619" ;

  /* Buffers */
  u_char *sbuf ;			/* SI output buffer */
  u_char *sptr ;			/* Output position in sbuf */
  int srem ;			/* Space remaining in buffer */
  int schunksize = SCHUNKSIZE ;	/* But NB will be parameterized (March 02) */
  u_char term[MAXEXITERMLEN + 2 ] ; /* Current index term */
  int termlen ;			/* (bytes) */
  static u_char *prevterm = (u_char *)"\001\000" ; /* previous term output */
  int lp ;			/* Prevterm's length */
  
  u_char *p4 ;
  u_char pstg4[5 + 32767 * sizeof(Pos_text)] ; /* Type 4 or 5 */
  u_int dummy ;

  BOOL first ;			/* True when 1st pstg for term */
  BOOL no_out = FALSE ;		/* No output if true */
  BOOL stdoutflag = FALSE ;	/* Posting file goes to stdout if true */
  int reportlevel = IXF_REPORTLEVEL ;
  BOOL silent = FALSE ;

  /* Parts of SI record, etc */
    
  u_short limits ;
  int tnp ;			/* Total postings for term */
  unsigned short maxtf ;	/* Max tf for a term */

  short log_tf ;
  u_short itrlen ;			/* This goes in length fields */
  int prev_itrlen ;
  u_short schunkword ;		/* Start of chunk word */
  OFFSET_TYPE opos ;			/* Posting file offset */
  int opos_offset ;		/* (for writing out opos) */
  int plength ;			/* Length of term's postings */
  OFFSET_TYPE oposition = 0 ;		/* Running .oi offset for types >= 2 */
  
  int pos_this_term ;
  int poslimit ;		/* Max no of pos flds per pstg */
  int posoverflow ;		/* Counts excess positional fields */

  /* Booleans */
  BOOL 
    eorec = 0, /* True when input record finished */
    eofile = 0 , /* True when input file finished */
    dia = DIAG ; /* If true, verbose */
  int spuriousrecs = 0 ;
  int oseqterms = 0 ;

  u_int rec = 0 ;			/* Also used for recnum */
  u_int prevrec = 0 ;		/* Recs go from 1 -, but still.. */
  float wt ;			/* For profile type 20 */
  int qtf ;			/* Profile type */

  /* Files */
  FILE
    *p, /* Primary index */
    *diag, /* Diagnostics */
    *stfile /* status */
    ;
  int sfd, ofd ;		/* File descriptors */
  FILE *ofp ;			/* Current .oi file for index types >= 2 */
  FILE *ofps[MAXIXVOLS] ;
  int pf_vol = 0 ;		/* .oi file vol for types 6 and 7 */
  /* When a postings volume is filled to pf_fill_factor of its nominal size
     a new volume is started (types 6 and 7, 8, 9). */
  double pf_fill_factor = PF_FILL_FACTOR ;

  char ofstem[PATHNAME_MAX] ;		/* Posting file names */
  char fstem[PATHNAME_MAX] ;		/* Other file names */

  /* Statistics etc */
  int
    nterms = 0,			/* Number of records output */
    nschunks = 0,		/* Number of chunks output */
    npchars = 0,		/* No of characters written to PI */
    npstgs = 0, 		/* Postings for term */
    numposes = 0,		/* No of positional fields */
    maxpos = 0 ; 		/* Max total tf */
#ifdef HAVE_LL
  LL_TYPE totpstgs = 0,	/* Total output postings */
    totnumposes = 0 ;		/* CTTF */
#else
  double totpstgs = 0,
    totnumposes = 0 ;
#endif
  u_int tottermlen = 0 ;	/* (Apr 02) */

  unsigned short wtf ;		/* tf for index type 10 */

  static int powers[32] ;	/* Powers of 2 */
  
  int termswith[33] ;		/* Number of terms with _powers_ postings */
  int pstgswithposes[MAXTEXTPOS] ; /* No of postings with .. pos fields */
  int power ;
  int npforbeast[16] ;		/* Number of postings for each beast */
  int termsforbeast[16] ;	/* Number of unique terms for each beast */
  int maxnpstgs = 0 ;		/* NP for most highly posted term */
  int l  ;
  int i, j ;

  int index_number, result ;
  int itype ;
  int large_rn ;		/* TRUE if 4-byte recnums */
  int haspos ;
  unsigned magic = 0 ;
  
  FILE *null_f ;		/* For /dev/null if no_out */
  int null_fd ;

  char *getenv() ;

#ifdef _WIN32
  _setmode(_fileno(stdin), _O_BINARY) ;
#endif

  for ( i = 1 ; i < argc ; i++ ) {
    if ( strcmp(argv[i], "-version") == 0 || strcmp(argv[i], "-v") == 0 ){
      fprintf(stderr, "%s\n", version) ;
      exit(0) ;
    }
    else if ( strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-h") == 0 ){
      Usage(argv[0]) ;
    }
  }
  for ( i = 1 ; i < argc - 2 ; i++ ) {
    if ( strcmp(argv[i], "-c") == 0 ) Control_dir = argv[++i] ;
    else if ( strcmp(argv[i], "-no_out") == 0 ) no_out = TRUE ;
    else if ( strcmp(argv[i], "-diag") == 0 ) dia = TRUE ;
    else if ( strcmp(argv[i], "-report") == 0 ) reportlevel = atoi(argv[++i]) ;
    else if ( strcmp(argv[i], "-stdout") == 0 ) stdoutflag = TRUE ;
    else if ( strcmp(argv[i], "-silent") == 0 ) silent = TRUE ;
    else {
      fprintf(stderr, "Unknown flag %s\n", argv[i]) ;
      Usage(argv[0]) ;
    }
  }

  if (argc < 3) Usage(argv[0]) ;

  /* Before doing anything, check ix1 OK so far */
  /* Mod Sep 98 for new magic word, but still temporarily accepts "OK\n" */
  {
    char *p = (char *)&magic ;
    for ( i = 0 ; i < 3 ; i++ ) *p++ = getchar() ;
    if ( strcmp((char *)&magic, "OK\n") == 0 ) {
      if ( ! silent ) fprintf(stderr, "ixf: input has old-style OK header\n") ;
    }
    else {
      *p = getchar() ;
      if ( magic != MAGIC_MERGEFILE ) {
	fprintf(stderr,
		"ixf: input has incorrect magic number %08x\n", magic) ;
	exit(1) ;
      }
    }
  }
  result = bss_startup() ;		/* Added March 96 */

  result = open_db(argv[argc - 2], &Cdb, DB_OPEN_NOSEARCH) ;
  
  if ( result < 0 ) {
    fprintf(stderr, "Can't open database %s\n", argv[argc - 2]) ;
    exit(1) ;
  }

  index_number = atoi(argv[argc - 1]) ;
  if ( index_number < 0 || index_number > Cdb.ni ) {
    fprintf(stderr,
     "Index number %d not consistent with database parm file\n", index_number);
    exit(1) ;
  }
  itype = Cdb.itype[index_number] ;
  large_rn = (itype == 11)||(itype == 12)||(itype == 13) ;
  haspos = (itype != 10 && itype != 11 && itype != 20) ;
  if ( itype == 11 || itype == 12 || itype == 13 ) {
    if ( sizeof(OFFSET_TYPE) < 5 ) {
      fprintf(stderr,
 "Can't make index type %d, OFFSET_TYPE type is only %d bytes, needs at least 5\n",
	      itype, sizeof(OFFSET_TYPE)) ;
      exit(1) ;
    }
    opos_offset = 3 ;		/* Not used unless HILO */
  }
  else opos_offset = sizeof(OFFSET_TYPE) - 4 ;
#if ! HILO
  opos_offset = 0 ;
#endif

  result = prepare_limits(&Cdb) ;
  if ( result < 0 ) {
    fprintf(stderr, "Can't read .lims file\n") ;
    exit(1) ;
  }

  poslimit = MAXTEXTPOS ;

  for ( i = 0 ; i < 32 ; i++ ) powers[i] = 1 << i ;
  for ( i = 0 ; i < 33 ; i++ ) termswith[i] = 0 ;
  memset((void *)pstgswithposes, 0, sizeof(pstgswithposes)) ;
  for ( i = 0 ; i < 16 ; i++ ) npforbeast[i] = 0 ;
  for ( i = 0 ; i < 16 ; i++ ) termsforbeast[i] = 0 ;

  if ( ! silent ) 
    fprintf(stderr,
	    "Final index production program ixf - version date Jan 15 1998\n") ;

  if ( no_out ) {
    null_f = fopen("/dev/null", "wb") ;
    null_fd = open("/dev/null", O_WRONLY|O_BINARY) ;
    if ( null_f == NULL || null_fd < 0 ) {
      fprintf(stderr, "Can't open nullfiles\n") ;
      exit(1) ;
    }
  }

  /* Open index files */
  if ( *lastslash(Cdb.ix_stem[index_number][0]) == '\0' ) {
    sprintf(ofstem, "%s%s", Cdb.ix_stem[index_number][0], Cdb.name) ;
  }
  else sprintf(ofstem, "%s", Cdb.ix_stem[index_number][0]) ;
  sprintf(ofstem + strlen(ofstem), ".%d.oi", index_number) ;

  sprintf(fstem, "%s"D_C"%s.%d", dirnameh(Cdb.ix_stem[index_number][0]),
	  Cdb.name, index_number) ;
  l = strlen(fstem) ;
  if ( ! no_out ) {
    strcpy(fstem + l, ".si") ;	/* Secondary index */
    sfd = open(fstem, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0644) ;
    if ( sfd < 0 ) {
      fprintf(stderr, "Can't open .si file %s\n", fstem) ;
      exit(1) ;
    }
    /* Deal with postings file - may go to stdout */
    if ( ! stdoutflag ) {
      ofp = ofps[0] = fopen(ofstem, "wb") ;
      if ( ofp == NULL ) {
	fprintf(stderr, "Can't open .oi file %s\n", ofstem) ;
	exit(1) ;
      }
      /* If multivolume type open remaining postings files */
      if ( itype >= 6 ) {
	for ( j = 1 ; j <= Cdb.last_ixvol[index_number] ; j++ ) {

	  if ( *lastslash(Cdb.ix_stem[index_number][j]) == '\0' ) {
	    sprintf(ofstem, "%s%s", Cdb.ix_stem[index_number][j], Cdb.name) ;
	  }
	  else sprintf(ofstem, "%s", Cdb.ix_stem[index_number][j]) ;
	  sprintf(ofstem + strlen(ofstem), ".%d.oi", index_number) ;
	  if ( (ofps[j] = fopen(ofstem, "wb")) == NULL ) {
	    fprintf(stderr, "Can't open postings file %s\n", ofstem) ;
	    exit(1) ;
	  }
	}
      }
    }
    else ofp = stdout ;

    strcpy(fstem + l, ".pi") ;	/* Primary index */
    p = fopen(fstem, "wb") ;
    if ( p == NULL ) {
      fprintf(stderr, "Can't open .pi file\n") ;
      exit(1) ;
    }
  } /* (if not no_out) */
  else {
    sfd = ofd = null_fd ;
    ofp = p = null_f ;
  }
  if ( ! no_out ) {
    strcpy(fstem + l, ".di") ;	/* Diagnostic file */
    diag = fopen(fstem, "wb") ;
    if ( diag == NULL ) {
      fprintf(stderr,"Can't open diagnostic file %s\n", fstem) ;
      exit(1) ;
    }
    strcpy(fstem + l, ".st") ;	/* New status file (Apr 02) */
    stfile = fopen(fstem, "wb") ;
    if ( stfile == NULL ) {
      fprintf(stderr, "Can't open status file %s\n", fstem) ;
      exit(1) ;
    }
  }
  else {
    diag=stdout ;		/* Well.. */
    stfile = null_f ;
  }

  /* Make buffers */

  sbuf = (u_char *)malloc(schunksize) ;
  if ( sbuf == NULL ) {
    fprintf(stderr, "Out of memory allocating buffers\n") ;
    exit(1) ;
  }

  /* Initialise PI */
  /* Write a few empty bytes at beginning to leave room for lengths */

  for (i = 1 ; i <= sizeof(long) + sizeof(int) ; i++ ) putc('\0', p) ;

  /* Initialise SI and OI */

  /* Zero backpointer (with high bit set) at start */
  sptr = sbuf ; srem = schunksize ;
  schunkword = 0x8000 ;
  memcpy((void *)sptr, (void *)&schunkword, sizeof(u_short)) ; 
  sptr += sizeof(u_short) ; srem -= sizeof(u_short) ;

  while ( ! eofile ) {
    /* Read next term */
    int kk ;
    maxtf = 0 ;
    limits = 0 ;
    tnp = 0 ;
    npstgs = 0 ;
    termlen = (unsigned) getchar() ;
    if ( (eofile = feof(stdin)) ) break ;
    if ( termlen == 0 ) {
      fprintf(stderr, "ixf: warning: zero length term\n") ;
    } /* Don't know what to do about this, appeared indexing INSPEC titles */
    *term = (u_char) termlen ; 
    result = fread(term + 1, termlen, 1, stdin) ;
    if ( result == 0 && termlen > 0 ) goto read_error ;
    tottermlen += termlen ;	/* (Apr 02) */
    termlen++ ;			/* Now includes precount */
    if ( nterms >= 1 ) {
      int tl = termlen - 1 ;
      int pl = *(u_char *)prevterm ;
      int min = (tl > pl)? pl : tl ;
      kk = memcmp(term + 1, prevterm + 1, min) ;
      if ( kk == 0 ) {
	if ( pl > tl ) kk = -1 ;
	else if ( pl < tl ) kk = 1 ;
      }
      if ( kk < 0 ) {
	fprintf(stderr,
 "Terms out of sequence at (nterms=%d) term=%.*s, prev term=%.*s\n",
		nterms, tl, term + 1, pl, prevterm + 1) ;
	oseqterms++ ;
	if ( oseqterms > OSEQLIM ) {
	  fprintf(stderr,
		  "Exiting because too many (%d) terms out of sequence\n",
		  oseqterms) ;
	  exit(1) ;
	}
      }
      else if ( kk == 0 ) {
	fprintf(stderr,
  "Successive terms identical at (nterms=%d) term=%.*s, prev term=%.*s\n",
		nterms, tl, term + 1, pl, prevterm + 1) ;
      }
    }
    /* Read all the postings for the term */
    eorec = FALSE ;
    first = TRUE ;		/* Have to know 1st pstg to get address
                                   or recnum to go in the term record */
    pos_this_term = 0 ;
    opos = oposition ;		/* opos is the start position for this
				   term's postings. oposition - opos is going
				   to be the total length of this term's
				   postings. */
    
    {				/* Used to be a switch statement, now
				   any type (March 96) */
      /* In cases itype >= 6 can use more than one volume.
	 Postings are written until one fills the file to within PF_FILL_FACTOR
	 (e.g. 0.95), then a new volume is started. This can break of course,
	 but is probably better than buffering everything in temporary files
	 (as we originally tried). */

      rec = prevrec = 0 ;	/* Shouldn't need it, but still.. */
      spuriousrecs = 0 ;
      if ( ! large_rn ) {
#if HILO
	fread((char *)&rec +  1, 3, 1, stdin) ;
#else
	fread((char *)&rec, 3, 1, stdin) ;
#endif
	eorec = (rec == 0x00ffffff) ;
      }
      else {
	fread((void *)&rec, 4, 1, stdin) ;
	eorec = (rec == 0xffffffff) ;
      }
      if ( eorec ) {
	fprintf(stderr, "Term with no postings, term=%.*s\n",
		termlen - 1, term + 1) ;
      }
      prevrec = rec ;		/* In case of trouble, diagnostic */
      while ( ! eorec ) {
	if ( Cdb.has_lims ) limits |= Cdb.lims[rec - 1] ;
	if ( itype == 20 ) {
	  fread((void *)&wt, sizeof(float), 1, stdin) ;
	  qtf = getchar() ;
	  putc(qtf, ofp) ;
	  oposition++ ;
	}
	else {			/* itype not 20 */
	  numposes = posoverflow = 0 ;
	  if ( itype != 10 && itype != 11 ) {
	    p4 = pstg4 ;
	    while ( (dummy = getw(stdin)) != 0xffffffff ) {
	      if ( numposes < poslimit ) {
		memcpy((void *)p4, (void *)&dummy, 4) ;
		p4 += 4 ;
		numposes++ ;
	      }
	      else posoverflow++ ;
	    }
	  }
	  else /* itype == 10 || itype == 11 */ {
	    fread((void *)&wtf, sizeof(short), 1, stdin) ;
	    if ( wtf >= poslimit ) {
	      posoverflow = wtf - poslimit ;
	      wtf = poslimit - 1 ;
	    }
	    numposes = wtf ;
	  }
	  if ( numposes < 128 ) {
	    putc(numposes, ofp) ;
	    oposition += 1 ;
	  }
	  else {
	    putc(((numposes / 128) | 0x80), ofp) ;
	    putc((numposes % 128), ofp) ; 
	    oposition += 2 ;
	  }
	  if ( numposes > maxtf ) maxtf = (unsigned)numposes ;
	} /* itype not 20 */

	if ( ! large_rn ) {
#if HILO
	  if ( fwrite((char *)&rec + 1, 3, 1, ofp) != 1 ) 
#else
          if ( fwrite((char *)&rec, 3, 1, ofp) != 1 ) 
#endif
	    {
	      goto o_write_error ;
	    }
	  oposition += 3 ;
        }
        else {
	  if ( fwrite((void *)&rec, 4, 1, ofp) != 1 ) {
	    goto o_write_error ;
	  }
	  oposition += 4 ;
	}
	if ( itype == 20 ) {
	  if ( fwrite((void *)&wt, sizeof(float), 1, ofp) != 1 ) {
	    goto o_write_error ;
	  }
	  oposition += sizeof(float) ;
	}
	if ( haspos ) {
	  if ( numposes > 0 &&
	       fwrite((void *)pstg4, numposes * sizeof(U_pos), 1, ofp)
	       != 1 ) {
	    fprintf(stderr, "OI write fail\n") ;
	    goto write_error ;
	  }
	  oposition += numposes * sizeof(U_pos) ;
	}
	pstgswithposes[numposes]++ ;
	pos_this_term += numposes + posoverflow ;
	totnumposes += numposes ;
	npstgs++ ; tnp++ ;
	prevrec = rec ;
	if ( ! large_rn ) {
#if HILO
	  fread((char *)&rec +  1, 3, 1, stdin) ;
#else
	  fread((char *)&rec, 3, 1, stdin) ;
#endif
	  eorec = ( rec == 0x00ffffff ) ;
	}
	else {
	  fread((void *)&rec, 4, 1, stdin) ;
	  eorec = ( rec == 0xffffffff ) ;
	}
      }	/* (while not eorec) */
      if ( posoverflow ) {
	fprintf(stderr, "Posting overflow %d at term %.*s\n",
		posoverflow, termlen-1, term+1) ;
      }
    } /* (dummy) */

    plength = oposition - opos ;

    /* Statistical stuff */
    if ( npstgs > maxnpstgs ) maxnpstgs = npstgs ;
    totpstgs += npstgs ;
    power = 0 ;
    while ( npstgs >= powers[power] && power < 32 ) power++ ;
    power-- ;
    termswith[power]++ ;
    if ( npstgs >= reportlevel ) {
      fprintf(diag, "%.*s %d %d\n", (int)len(term), term+1,
		npstgs, pos_this_term) ; 
    }
    /* End of a term record, all postings written and np fields calculated */
    
    /* Assemble ITR */
    prev_itrlen = itrlen ;	/* See "no you ninny .." below */
    switch (itype) {
    case 11:
    case 12:
    case 13:
      itrlen = termlen + 2 + 2 + 4 + 4 + 4 + 1 + 5 + 1 ;
      break ;
    case 8:
    case 9:
    case 10:
    case 20:
      itrlen = termlen + 2 + 2 + 4 + 4 + 4 + 1 + 4 + 1 ;
      break ;
    case 6:
    case 7:
      itrlen = sizeof(u_short) * 2 + termlen + 2 + 2 + 3 + 1 + 4 ;
      /* (<len><pcterm><limits><log tf><tnp><vol><opos><len>) */
      /*    2    var      2        2     3    1    4    2    */
      break ;
    case 4:
    case 5:
    default:
      itrlen = sizeof(u_short) * 2 + termlen + 2 + 2 + 3 + 4 ;
      /* (<len><pcterm><limits><log tf><tnp><opos><len>) */
      /*    2    var      2        2     3    4    2    */
      break ;
    }
    /* Start new chunk? */
    if ( (itype >= 8  && itrlen > srem - 1 ) ||
	(itype >=4 && itype <= 7 && itrlen > srem - sizeof(u_short)) ) { 
      lp = len(prevterm) + 1 ;
      if ( fwrite(prevterm, lp, 1, p) != 1 ) {
	fprintf(stderr, "PI write fail at %.*s\n", lp - 1, prevterm+1) ;
	goto write_error ;
      }
      
      npchars += lp ; /* Length of PI file */
      memset((void *)sptr, '\0', srem) ;
      if ( write(sfd, (void *)sbuf, schunksize) != schunksize ) {
	fprintf(stderr, "SI write fail after chunk %d\n", nschunks) ;
	goto write_error ;
      }
      nschunks++ ;
      if ( dia ) 
	fprintf(stderr, "Ch. %d ends with %.*s\n",
		nschunks, (int)len(prevterm), prevterm + 1) ;

      /* schunkword = ((itrlen + srem)|0x8000) ; */
      /* No you ninny this should be the length of the PREVIOUS ITR,
	 this has gone unnoticed for years, there's a bodge in gprt()
	 to get round it. */
      schunkword = ((prev_itrlen + srem)|0x8000) ; 
      
      srem = schunksize ; sptr = sbuf ;
      memcpy((void *)sptr, (void *)&schunkword, sizeof(u_short)) ;
      srem -= sizeof(u_short) ; sptr += sizeof(u_short) ;
    } /* (ITR won't fit) */
#if ! HILO
    limits = reverse_16(limits) ; /* All limits filed as HILO */
#endif
    switch (itype) {
    case 8:
    case 9:
    case 10:
    case 20:
      memcpy((void *)sptr, (void *)term, termlen) ;
      prevterm = sptr ;		/* Have to keep track of last term in chunk
				   for writing it to PI */
      sptr += termlen ;
      memcpy((void *)sptr, (void *)&limits, sizeof(u_short)) ;
      sptr += sizeof(u_short) ;
      memcpy((void *)sptr, (void *)&maxtf, sizeof(u_short)) ;
      sptr += sizeof(u_short) ;
      memcpy((void *)sptr, (void *)&pos_this_term, sizeof(int)) ;
      sptr += sizeof(int) ;
      memcpy((void *)sptr, (void *)&tnp, sizeof(int)) ; sptr += sizeof(int) ;
      memcpy((void *)sptr, (void *)&plength, sizeof(int)) ;
      sptr += sizeof(int) ;
      *sptr++ = pf_vol ;
      memcpy(sptr, (u_char *)&opos + opos_offset, 4) ;
      sptr += 4 ;
      *sptr++ = (u_char)(termlen - 1) ;		/* Length byte at end */
      break ;
    case 11:
    case 12:
    case 13:
      memcpy((void *)sptr, (void *)term, termlen) ;
      prevterm = sptr ;		/* Have to keep track of last term in chunk
				   for writing it to PI */
      sptr += termlen ;
      memcpy((void *)sptr, (void *)&limits, sizeof(u_short)) ;
      sptr += sizeof(u_short) ;
      memcpy((void *)sptr, (void *)&maxtf, sizeof(u_short)) ;
      sptr += sizeof(u_short) ;
      memcpy((void *)sptr, (void *)&pos_this_term, sizeof(int)) ;
      sptr += sizeof(int) ;
      memcpy((void *)sptr, (void *)&tnp, sizeof(int)) ; sptr += sizeof(int) ;
      memcpy((void *)sptr, (void *)&plength, sizeof(int)) ;
      sptr += sizeof(int) ;
      *sptr++ = pf_vol ;
      memcpy(sptr, (u_char *)&opos + opos_offset, 5) ;
      sptr += 5 ;
      *sptr++ = (u_char)(termlen - 1) ;		/* Length byte at end */
      break ;

    default:			/* 4, 5, 6, 7 */
      memcpy((void *)sptr, (void *)&itrlen, sizeof(u_short)) ;
      sptr += sizeof(u_short) ;
      memcpy((void *)sptr, (void *)term, termlen) ;
      prevterm = sptr ;		/* Have to keep track of last term in chunk
				   for writing it to PI */
      sptr += termlen ;
      memcpy((void *)sptr, (void *)&limits, sizeof(u_short)) ; sptr += 2 ;
      log_tf = (short)round(lg((double)(pos_this_term + 1))) ;
      memcpy((void *)sptr, (void *)&log_tf, sizeof(short)) ;
      sptr+= sizeof(short) ;     
      memcpy(sptr, (u_char *)&tnp + 1, 3) ; sptr += 3 ;
      if ( itype == 6 || itype == 7 ) *sptr++ = pf_vol ;
      memcpy(sptr, (u_char *)&opos + opos_offset, 4) ;
      sptr += 4 ;
      memcpy((void *)sptr, (void *)&itrlen, sizeof(u_short)) ;
      sptr += sizeof(u_short) ;
      break ;
    } /* Switch */
    if ( pos_this_term > maxpos ) {
      if ( dia )
	fprintf(stderr, "%.*s: total tf %d, log_tf %d\n",
		termlen - 1, term + 1, pos_this_term, (int)log_tf);
      maxpos = pos_this_term ;
    }
    srem -= itrlen ;
    nterms++ ;

    /* If multivol start new pstgs vol if necessary. */
    if ( (double)oposition >
	(double)Cdb.ix_volsize[index_number][pf_vol] * pf_fill_factor ) {
      if ( ! no_out && ! stdoutflag ) fclose(ofps[pf_vol]) ;
#ifdef LARGEFILES
      if ( ! silent ) fprintf(stderr,
#ifdef _WIN32
	      "Closed pf_vol %d at %I64d bytes, term %.*s\n",
#else
	      "Closed pf_vol %d at %lld bytes, term %.*s\n",
#endif
	      pf_vol, (LL_TYPE)oposition, *term, term+1) ;
#else
      if ( ! silent ) fprintf(stderr,
	      "Closed pf_vol %d at %u bytes, term %.*s\n",
	      pf_vol, (unsigned)oposition, *term, term+1) ;
#endif
      if ( pf_vol >= Cdb.last_ixvol[index_number] ) {
	fprintf(stderr, "Fatal error: no more index volumes allocated\n") ;
	exit(1) ;
      }
      oposition = 0 ;
      if ( ! no_out && ! stdoutflag ) ofp = ofps[++pf_vol] ;
    }
  } /* Main loop */
  /* Dummy record on end */
  switch (itype) {
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 20:
    *sptr++ = 0xff ; srem-- ;
    break ;
  default:
    itrlen = 0x8000 ;
    memcpy((void *)sptr, (void *)&itrlen, sizeof(u_short)) ; sptr += sizeof(u_short) ;
    srem -= sizeof(u_short) ;
  }
  memset((void *)sptr, '\0', srem) ;
  if ( write(sfd, (void *)sbuf, schunksize) != schunksize ) {
    fprintf(stderr, "SI write fail after chunk %d\n", nschunks) ;
    goto write_error ;
  }
  nschunks++ ;
  if ( ! silent ) fprintf(stderr, "Written schunk %d\n", nschunks) ;
  if ( ! no_out ) close(sfd) ;

  if ( ! no_out && ! stdoutflag ) fclose(ofp) ;

  /* Infinite record on end of PI */
  putc(1, p) ; putc(0x7f, p) ;
  npchars += 2 ;
  rewind(p) ;
  fwrite((void *) &npchars, sizeof(long), 1, p) ;
  fwrite((void *) &nschunks, sizeof(int), 1, p) ;
  fclose(p) ;


  /* Close database */
  close_db(&Cdb, 0) ;

  /* Write statistics */
  if ( ! silent ) {
    fprintf(stderr, "\nRun finished - statistics follow\n") ;
    
    fprintf(stderr, "\n%6d terms with %6d posting\n", termswith[0], 1) ; 
    for ( i = 1 ; i < 32 ; i++ ) {
      if ( termswith[i] == 0 ) continue ;
      fprintf(stderr,
	      "%6d terms with %6d - %6d postings\n",
	      termswith[i], powers[i], powers[i + 1] - 1) ; 
    }
    fprintf(stderr, "\n\n") ;
    if ( pstgswithposes[0] >  0 )
      fprintf(stderr, "\n%8d postings with %4d positional fields\n",
	      pstgswithposes[0], 0) ; 
    fprintf(stderr, "\n%8d postings with %4d positional field\n",
	    pstgswithposes[1], 1) ; 
    for ( i = 2 ; i < MAXTEXTPOS ; i++ ) {
      if ( pstgswithposes[i] == 0 ) continue ;
      if ( pstgswithposes[i] == 1 ) 
	fprintf(stderr,
		"%8d posting  with %4d positional fields\n",
		pstgswithposes[i], i) ; 
      else
	fprintf(stderr,
		"%8d postings with %2d positional fields\n",
		pstgswithposes[i], i) ; 
    }
  }
  if ( ! silent ) fprintf(stderr, "\nTOTALS (index type %d)\n\n", itype) ;
#ifdef HAVE_LL
  if ( ! silent ) {
    fprintf(stderr, "%12lld positional fields\n", totnumposes) ;
    fprintf(stderr, "%12lld postings\n", totpstgs) ;
  }
  fprintf(stfile, "%lld cttf\n%lld tnp\n", totnumposes, totpstgs) ;
#else
  if ( ! silent ) {
    fprintf(stderr, "%12.0f positional fields\n", totnumposes) ;
    fprintf(stderr, "%12.0f postings\n", totpstgs) ;
  }
  fprintf(stfile, "%.0f cttf\n%.0f tnp\n", totnumposes, totpstgs) ;
#endif
  fprintf(stfile, "%d terms\n%u total_termlen\n",
	  nterms, (u_int)tottermlen) ;
  if ( ! silent ) {
    fprintf(stderr, "%12d terms\n%12u total termlength\n",
	    nterms, (u_int)tottermlen) ;
    fprintf(stderr, "%12d SI chunks\n", nschunks) ;
    fprintf(stderr, "Most highly posted term has %d postings\n", maxnpstgs) ;
  }
  if ( ! no_out ) fclose(diag) ;
  exit(0) ;

    read_error:
  fprintf(stderr, "Input read error\n") ;
  perror(NULL) ;
  exit(1) ;

    write_error:
  fprintf(stderr, "Write error\n") ;
  perror(NULL) ;
  exit(1) ;

    o_write_error:
  fprintf(stderr, "Posting file write error\n") ;
  perror(NULL) ;
  exit(1) ;
}

