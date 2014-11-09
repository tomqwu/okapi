/******************************************************************************

  ix_deconstruct.c SW Dec 98

  Input index si and oi & output index runfile (mergefile)

  Works for index types 8 - 13

  Return 0 if oi and si finished, < 0 on error.

  If outname is "-" outputs to stdout, other wise it opens the file.

  May 02: added convert arg: if TRUE converts 3-byte recs to 4 or vice versa
  according to the index type.

  Oct 02: modified so can do a wider range of conversions:

  Input            Output
  8                8, 10

******************************************************************************/

#include "bss.h"

static void *inf_3 = "\377\377\377" ;
static void *inf_4 = "\377\377\377\377" ;

int
ix_deconstruct(int itype, char *si_name, int numvols, char *oi_names[],
	       char *outname, int chunksize, int noisy, int convert)
{
  FILE *oi, *si ;
  FILE *outf ;
  int lastpvol = -1 ;		/* Dummy starter */
  int irn = 0 ;
  int pvol ;
  OFFSET_TYPE ppos ;
  unsigned int pos ;
  BOOL eosi = FALSE, eochunk = TRUE ;
  void *sibuf ;
  void *itr ;
  int rres ;
  int errc = 0 ;
  int numchunks = 0, numterms = 0 ;
  LL_TYPE numpstgs = 0, numposes = 0 ;
  int tnp ;
  unsigned short tf ;
  int length ;
  char term[MAXEXITERMLEN] ;
  BOOL convert34 = FALSE, convert43 = FALSE ;
  int i, j ;

  
  if ( convert ) {
    if ( itype == 8 || itype == 9 || itype == 10 ) convert34 = TRUE ;
    else convert43 = TRUE ;
  }
  if ( chunksize == 0 ) {
    chunksize = SCHUNKSIZE ;
    if ( noisy ) 
      fprintf(stderr, "Assuming zero chunksize means %d\n", SCHUNKSIZE) ;
  }
  si = fopen(si_name, "rb") ;
  if ( si == NULL ) {
    if ( noisy ) fprintf(stderr, "Can't open dictionary file %s\n", si_name) ;
    return -1 ;
  }
  if ( strcmp(outname, "-") == 0 ) outf = stdout ;
  else {
    outf = fopen(outname, "wb") ;
    if ( outf == NULL ) {
      if ( noisy ) fprintf(stderr, "Can't open output file %s\n", outname) ;
      fclose(si) ;
      return -1 ;
    }
  }
  sibuf = (char *)malloc(chunksize) ;
  if ( sibuf == NULL ) {
    if ( noisy )
      fprintf(stderr, "Can't allocate %d bytes for dictionary file buffer\n",
	      chunksize) ;
    fclose(si) ;
    if ( outf != stdout ) fclose(outf) ;
    return -3 ;
  }
  putw(MAGIC_MERGEFILE, outf) ;
  while ( ! eosi ) {
    if ( eochunk ) {
      rres = (int)fread(sibuf, (size_t)chunksize, (size_t)1, si) ;
      if ( rres != 1 ) {
	if ( noisy )
  fprintf(stderr, "Unexpected EOF or read error on dictionary file\n") ;
	errc = -2 ;
	break ;
      }
      eochunk = FALSE ;
      numchunks++ ;
      if ( noisy > 2 ) fprintf(stderr, "Read chunk %d\n", numchunks) ;
      itr = sibuf + 2 ;	/* skipping chunkword */
    }
    while ( ! eochunk ) {
      length = *(unsigned char *)itr ;
      if ( length == 0xff ) {
	eosi = TRUE ;
	if ( noisy > 1 )
	  fprintf(stderr, "Read EOSI (`inf' term) after term %s\n", term) ;
	break ;
      }
      if ( length == 0 ) {
	eochunk = TRUE ;
	if ( noisy > 2 )
	  fprintf(stderr, "Read eochunk after term %s\n", term) ;
	break ;
      }
      (void)memcpy((void *)term, itr + 1, (size_t)length) ;
      term[length] = 0 ;
      numterms++ ;
      if ( noisy == 2 && numterms % 10000 == 0 ) 
	fprintf(stderr, "%d: %s\n", numterms, term) ;
      else if ( noisy > 2 ) fprintf(stderr, "%d: %s\n", numterms, term) ;
      putc(length, outf) ;
      fwrite((void *)term, (size_t)length, (size_t)1, outf) ;
      tnp = get_tnp_itr(itr, itype) ;
      numpstgs += tnp ;
      pvol = get_pvol_itr(itr, itype) ;
      if ( pvol != lastpvol ) {
	if ( lastpvol >= 0 ) fclose(oi) ;
	oi = fopen(oi_names[pvol], "rb") ;
	if ( oi == NULL ) {
	  if ( noisy ) fprintf(stderr, "Can't open postings file %s\n",
		  oi_names[pvol]) ;
	  errc = -1 ;
	  break ;
	}
	if ( noisy ) {
	  fprintf(stderr, "Opened postings file vol %d of %d\n",
		  pvol + 1, numvols) ;
	}
      }
      lastpvol = pvol ;
      ppos = get_ppos_itr(itr, itype) ;
      itr = skip_itr(itr, itype) ;
      for ( i = 0 ; i < tnp ; i++ ) {
	tf = fget_vshort(oi) ;
	numposes += tf ;
	switch (itype) {
	case 8:
	case 9:
	case 10:
#if ! HILO
	  fread((void *)&irn, 3, 1, oi) ;
	  if ( convert34 ) fwrite((void *)&irn, 4, 1, outf) ;
	  else fwrite((void *)&irn, 3, 1, outf) ;
	  
#else
	  fread((void *)&irn + 1, 3, 1, oi) ;
	  if ( convert34 ) fwrite((void *)&irn, 4, 1, outf) ;
	  else fwrite((void *)&irn + 1, 3, 1, outf) ;
#endif
	  break ;
	case 12:
	case 13:
	case 11:
	default:
	  if ( convert43 ) {
#if ! HILO	    
	    fread((void *)&irn, 4, 1, oi) ;
	    fwrite((void *)&irn, 3, 1, outf) ;
#else
	    fread((void *)&irn, 4, 1, oi) ;
	    fwrite((void *)&irn + 1, 3, 1, outf) ;
#endif
	  }
	  else {
	    fread((void *)&irn, 4, 1, oi) ;
	    fwrite((void *)&irn, 4, 1, outf) ;
	  }
	  break ;
	}
	if ( noisy > 2 ) fprintf(stderr, "IRN=%d, tf=%d\n", irn, tf) ;
	if ( itype == 10 || itype == 11 ) {
	  fwrite((void *)&tf, sizeof(unsigned short), 1, outf) ;
	}
	else {
	  for ( j = 0 ; j < tf ; j++ ) {
	    fread((void *)&pos, (size_t)4, (size_t)1, oi) ;
	    fwrite((void *)&pos, (size_t)4, (size_t)1, outf) ;
	  }
	  fwrite(inf_4, (size_t)4, (size_t)1, outf) ;
	}
      }
      switch (itype) {
      case 8:
      case 9:
      case 10:
	if ( convert34 ) fwrite(inf_4, (size_t)4, (size_t)1, outf) ;
	else fwrite(inf_3, (size_t)3, (size_t)1, outf) ;
	break ;
      case 12:
      case 13:
      case 11:
      default:
	if ( convert43 ) fwrite(inf_3, (size_t)3, (size_t)1, outf) ;
	else fwrite(inf_4, (size_t)4, (size_t)1, outf) ;
	break ;
      }
      if ( errc ) break ;
    }
  }
  if ( noisy ) {
    if ( itype == 10 || itype == 11 ) 
#ifdef _WIN32
      fprintf(stderr,
	      "%d SI chunks %d terms %I64d pstgs %I64d total term freq\n",
	      numchunks, numterms, numpstgs, numposes) ;
    else
      fprintf(stderr,
	      "%d SI chunks %d terms %I64d pstgs %I64d total pos recs\n",
	      numchunks, numterms, numpstgs, numposes) ;
#else
      fprintf(stderr,
	      "%d SI chunks %d terms %lld pstgs %lld total term freq\n",
	      numchunks, numterms, numpstgs, numposes) ;
    else
      fprintf(stderr,
	      "%d SI chunks %d terms %lld pstgs %lld total pos recs\n",
	      numchunks, numterms, numpstgs, numposes) ;
#endif
  }
  if ( sibuf ) free(sibuf) ;
  fclose(si) ;
  fclose(oi) ;
  if ( outf != stdout ) fclose(outf) ;
  return errc ;
}

