#include "bss.h"
#include "bss_errors.h"
#include "charclass.h"

/*****************************************************************************

read_gsl() etc 22 June 87

Revision 28 Apr 88: token[] is now initialised to @0000,
erroneous 'close' replaced by 'fclose', initial block allocation 
increased.
Revision 7 Jan 87 - 'mode' changed to 'gsclass'.

Nov 87: now uses dynamic reallocation of string and pointer storage. If
comments are removed will display what it is doing.  It would be more
elegant to use in preformatted form as originally intended, and just to
read in a structured file. But the whole structure needs revising to allow
more flexibility, so will leave for now.

Revision 12 Oct 87 - stemming subroutines no longer use precounts
Revision 6-7 Oct 87

Reads go/see list file and structures and sorts it into Gsl.

Format of gsl file is <gsclass><text>,[blank<text>,]:\n 

<gsclass> is one of S[ee] G[o] H (stop) P[refix] F[unction] I (words of
dubious value which would have a low prior weight, and probably be treated
as stopwords during query expansion) (others will be added)

Stemfn() is applied to each word of the input.

There is a limit on the length of a line (record) and item (field).
Only S records (at present) can have more than one field in a record.

The struct GSL (typedef in stdeqs.h) must be declared, but read_gsl()
allocates storage for the data and pointers as required. 

free_gsl() frees the storage allocated for a gsl & marks the gsl unused

Mod July 90 - was readgsl which was identical except that stem function was 
passed as argument. read_gsl() is used by search programs, readgsl() (temp) by
indexing progs. SEE ALSO readgsl(). Mar 00: readgsl() was deleted years ago!

Oct 93: empty record on front - think this was never done. Don't know
what the front 8 bytes are for, but left them for now.

*****************************************************************************/

static int
realloc_data_space(Gsl *gsl, int *gsdblocks, char **op, char **reclenptr)
{
  /* If the data space is altered all the pointers in sptrs will be wrong.
     This routine shifts them. Returns 0 if OK, otherwise -1 */
  
  char * oldbase = gsl->data ;
  register int i ;
  int diff ;
  
  *gsdblocks += 8 ;
  if ((gsl->data = bss_realloc(gsl->data, BLOCKSIZE * *gsdblocks)) == NULL)
    return -1 ;
  diff = gsl->data - oldbase ;
  for ( i = 0 ; i < gsl->numrecs ; i++ ) gsl->sptrs[i] += diff ; 
  *op += diff ; *reclenptr += diff ;
  return 0 ;
}



/* Compare indirect pc strings returning 1, 0, -1 */

static int
comp(u_char **s1, u_char **s2) 
{
#ifdef OBSOLETE
  int c ;
  int result ;
  c = cpasc(*s1+1, len(*s1), *s2+1, len(*s2), &result) ;
  if ( c & 1 ) return(1) ; 
  if ( c & 2 ) return(-1) ;
  return(0) ;
#endif
  return cpbytes(*s1 + 1, len(*s1), *s2 + 1, len(*s2)) ;

}

static int
read_gsl(char *fname, Gsl *gsl)
{
  int (* stem_fn)(char *, int, char *) = gsl->stem_fn ;
  static char token[5] = {'@', '0', '0', '0', '0'} ;
  char word[256]; /* For processing individual words and building up output 
		     records */
  register int wl ; /* Length of current word */
  char *wp = word + 1 ;
  char *op ;
  char *reclenptr ;
  int reclen = 0 ; /* Length of current output record */
  int gspblocks, gsdblocks ; /* Number of blocks allocated for gsl storage */
  int totlength = 2 * sizeof(int) ;
  int inrecs = 0 ;
  FILE *ifp ;
  int ch ; /* Input character */
  char gsclass ; /* Current gsclass: S, G, H etc */
  int i ;

  static char *func = "read_gsl" ;
  
  token[0] = '@' ;
  for ( i = 1 ; i < 5 ; i++ ) token[i] = '0' ;

  gspblocks = 16 ; /* initial storage for pointers */
  gsdblocks = 32 ; /* Initial storage for data */
  gsl->numrecs = 0 ; gsl->data = NULL ; gsl->sptrs = (char **) NULL ;

  if ( ( ifp = bss_fopen(fname, "r") ) == NULL ) {
    sprintf(Err_buf, "can't find GSL file %s", fname) ;
    sys_err(func) ;
    return -1 ;
  }
  gsl->data = bss_malloc(BLOCKSIZE * gsdblocks) ;
  gsl->sptrs = (char **) bss_malloc(BLOCKSIZE * gspblocks) ;
  if ( gsl->data == NULL || gsl->sptrs == NULL ) return -5 ;
  op = gsl->data + 2 * sizeof(int) ; /* Don't know why */
  gsl->sptrs[gsl->numrecs++] = op ;
  *op++ = '\001' ; *op++ = '\0' ;

  reclenptr = op ;
  
  op++ ; wl = 0 ;
  while ( 1 ) {
    while ( (ch = getc(ifp)) != EOF && ! tok_alnum(ch) && ch != '#') ; 
    if ( ch == EOF ) break ;
    gsclass = ch ;
    if ( ! isintab((int)gsclass, (u_char *)"#SGHFPI") ) {
      sprintf(Err_buf, "Unrecognised gsclass %c after %.*s",
		gsclass, len(gsl->sptrs[gsl->numrecs - 1]),
		gsl->sptrs[gsl->numrecs - 1]);
      sys_err(func) ;
      return -3 ;
    }
    if ( gsclass == '#' ) { /* This line is a comment */
      while ( (ch = getc(ifp)) != '\n' ) ;
      continue ;
    }
    while ( !tok_alnum(ch = getc(ifp)) ) ; 
    while ( ch != ':' ) {
      while ( ch != ',' ) {
	while ( ch != ' ' && ch != ',' && ch != '\n' ) {
	  *wp++ = ch ; wl++ ;ch = getc(ifp) ;
	} /* End of word */
	wl = stem_fn(word+1, wl, word+1) ;
	reclen += wl ;
	if ( reclen > 255 ) return(-2) ; 
	for (i = wl, wp = word + 1 ; i > 0 ; i--) *op++ = *wp++ ;
	wl = 0 ; wp = word + 1 ;
	if ( ch == ' ' || ch == '\n' ) { 
	  while ( !tok_alnum(ch = getc(ifp)) && ch != ',' ) ; 
	}
	if ( tok_alnum(*(op - 1)) ) { *op++ = ' ' ; reclen++ ; } 
      } /* End of output record */
      *reclenptr = reclen ;
      gsl->sptrs[(gsl->numrecs)++] = reclenptr ; 
      if (gsl->numrecs * sizeof(char *) > (gspblocks-1) * BLOCKSIZE ) {
	gspblocks += 4 ;
	gsl->sptrs = (char **) bss_realloc(gsl->sptrs, 
				       BLOCKSIZE * gspblocks ) ;
	if ( gsl->sptrs == NULL ) return -4 ;
      }
      /* Write start address of record & incr count */
      *op++ = gsclass ; reclen++ ;
      if ( gsclass == 'S' ) { 
	for (i = 0 ; i <= 4 ; i++) *op++ = token[i] ;
	reclen += 5 ; 
      }
      totlength += reclen + 1 ; reclen = 0 ; reclenptr = op++ ;
      if (totlength > (gsdblocks-1) * BLOCKSIZE) {
	if (realloc_data_space(gsl, &gsdblocks, &op, &reclenptr) != 0)
	  return -4 ;
      }
      while ( !tok_alnum(ch = getc(ifp)) && ch != ':' ) ;
    } /* End of input record */
    inrecs++ ;
    if ( gsclass == 'S' ) for ( i = 4 ; i >= 1 ; i-- ) {
      token[i]++ ;
      if ( token[i] <= '9' ) break ; else token[i] = '0' ; }
  }
  /* Infinite record on end */
  *reclenptr = 1 ; *(reclenptr+1) = 0x7f ; *(reclenptr+2) = 'H' ; 
  totlength += 3 ;
  gsl->sptrs[gsl->numrecs++] = reclenptr ;
  
  /* Sort */
  
  gqsort( (char *) gsl->sptrs, gsl->numrecs, sizeof(char *),
	  (int (*)(void *, void *))comp) ;
  
  bss_fclose(ifp) ;
  gsl->name = bss_strdup(lastslash(fname)) ;
  return 0 ;
}

/*****************************************************************************

  find_gsl()  July 90 SW

  March 99: now looks for function address, not function name. Also now
  the global array holds Gsl pointers, not Gsls themselves.

  Searches an array of gsls (typically the global array Gsls[]) for one which
  is live and has the sought name and stem function.

  If found, find_gsl() returns 1, and the array subscript is returned in
  'which'.

  If not found it returns 0 & places the first free subscript in 'which'; if
  there is no free subscript, 'which' contains -1.

*****************************************************************************/

static int
find_gsl(char *name, PFI stem_func, int *which)
{
  Gsl *g ;
  int i, num ;

  if ( Dbg & D_GSL ) 
    fprintf(bss_syslog,
	    "find_gsl(): looking for gsl %s func at %x\n",
	    name, (unsigned int)stem_func) ;
  for ( i = 0, num = 0 ; i < MAXGSLS && num < Num_gsls ; i++ ) {
    if ( (g = Gsls[i]) ) {
      num++ ;
      if ( Dbg & D_GSL )
	fprintf(bss_syslog,
		"find_gsl(): gsl[%d], links=%d, no_db=%d, name=%s, func=%s\n",
		i, g->num_links, g->no_db, g->name, g->func_name) ;
      if ( strcmp(name, g->name) == 0 && stem_func == g->stem_fn ) {
	if ( Dbg & D_GSL )
	  fprintf(bss_syslog,
		  "find_gsl(): found existing gsl at Gsls[%d]\n", i) ;
	*which = i ;
	return 1 ;
      }
    }
  }
  /* Then there isn't a suitable live one to attach to */
  *which = new_gsl() ;
  if ( Dbg & D_GSL )
    fprintf(bss_syslog, "find_gsl(): new gsl Gsls[%d]\n", *which) ;
  return 0 ;
}

/*****************************************************************************

  setup_gsl() etc March 99

  setup_gsl_ix() deleted. Only used by ixinit(), now makes direct call of
  setup_gsl().

  setup_gsl_sg()       June 97

  March 99: all below historical interest only. Now returns 0 if normal,
  1 if already done, -1 on failure.

  If the sg's gsl field is non-NULL (already set up) returns 1.
  Otherwise searches the global GSL array for one already set up and
  with the correct name and stem function. If it finds one it attaches it
  to the sg and returns 2, otherwise if there is a free GSL it sets it up,
  attaches it and returns 0.

  Errors: can't open GSL file         -1
          GSL read failure:           -2   [?]
	  faulty GSL file:            -3
	  out of memory on read_gsl:  -5
	  no such stem function:      -6
	  no free GSL:                -7

  (first 4 inherited from read_gsl())	  

  setup_gsl_ix()       June 1997

  Return codes more or less as setup_gsl_sg() with the addition of

    NULL index:                 -8
    index number out of range:  -9
    index's search group NULL: -10

  setup_gsl_sg needs to be done before any parse command is executed
  (unnecessary if setup_gsl_ix has been done). It can be called
  from anywhere, provided there is an open database.

  setup_gsl_ix must be done before anything is looked up, but it should
  only be called from ixinit() (or if ixinit() has already been done)

  Redundant calls are harmless.


  Formerly these were setup_gsls_g() SW June 1992

  Attaches the gsls & their stemming functions specified by name in the
  global Db's Search_groups array to the global gsl array Gsls[] and reads
  them. Returns 0 if OK, negative if not. If ixnum >= 0 and valid does it
  only for the specified index (this is for indexing), otherwise does it
  for all indexes.

  NB June 92. Interactive system still uses old setup_gsls(). Indexing uses
  this one (ix1_ex.new), and BSS uses this one.

  June 97. Now a single gsl per index. (And a single search_group)
  Now only does it for the specified index or sg. 

*****************************************************************************/

/******************************************************************************

  Gsl *setup_gsl(char *filename, int *stem_fn(char *, int, char *, int *result))

  SW March 99

  To allow parsing without an open database: parse controlled by named
  regime stemfunction and GSL 

  The stem function may be passed as NULL in which case it is obtained
  from the name.

  result=0 if new gsl struct allocated and set up OK, 1 if already exists,
  -1 if can't read (e.g. filename wrong), -2 if no such stem function name,
  -3 if no free Gsl slots.

******************************************************************************/

Gsl *
setup_gsl(char *name, char *func_name, PFI stem_fn, int *result)
{
  int res, which ;
  char filename[PATHNAME_MAX] ;
  Gsl *g ;
  static char *func = "setup_gsl" ;

  /* If stem_fn is NULL find it */
  if ( stem_fn == NULL ) {
    if ( (stem_fn = check_stemfn(func_name)) == NULL )
      /* NO_SUCH_STEMFUNCTION will be set */
      *result = -2 ;
      return NULL ;
  }

  /* Check whether this gsl already set up (with this function) */
  *result = find_gsl(name, stem_fn, &which) ;
  if ( *result == 1 ) {		/* This gsl already set up */
    g = Gsls[which] ;	/* Done */
  }
  else if ( which == -1 ) {	/* No free GSL */
    internal_err(func, "no free GSL structs") ;
    *result = -3 ;
    return NULL ;
  }
  else {
    /* *result == 0 */
    g = Gsls[which] ;
    g->func_name = bss_strdup(func_name) ;
    g->stem_fn = stem_fn ;
    sprintf(filename, "%s"D_C"%s", Control_dir, name) ;
    res = read_gsl(filename, g) ; 
    if ( Dbg & D_GSL )
      fprintf(bss_syslog, "Read gsl %s, result %d\n",
	      filename, res) ;
    if ( res != 0 ) {
      sprintf(Err_buf, "can't read gsl %s", filename) ;
      sys_err(func) ;
      free_gsl(Gsls+which, 1) ;
      *result = -1 ;
      return NULL ;
    }
  } /* (gsl not null and not already open) */
  return g ;
}
  
/******************************************************************************

  setup_gsl_sg()     SW June 97

  March 99: only function which uses this is setup_ixparms(), others
  use just setup_gsl()

******************************************************************************/

int
setup_gsl_sg(Sg *sg) 
{
  char *name ;
  int result ;
  Gsl *g ;

  static char *func = "setup_gsl_sg" ;

  if ( sg->gsl != NULL ) return 1 ;
  else {
    name = sg->gsl_name ;
    g = setup_gsl(sg->gsl_name, sg->stem_name, sg->stem_fn, &result) ;
  } 
  if ( g == NULL || result < 0 ) {
    sg->status |= SG_GSL_FAIL ;
    return -1 ;
  }
  sg->gsl = g ;
  sg->status |= SG_GSL_OK ;
  return 0 ;
}

/*****************************************************************************
  lookup_gsl and do_gsl fetched here from searchsubs.c Oct 1989

   Lookup_gsl 19 june 87

   Searches a GSL struct for a left substring of the sought 
   string. It uses Bsearch to find whether there is a match on the first 
   LWD (extern, generally the length of the first word of the sought 
   string.  If there is a match (and this is not complete), it goes 
   forwards in Strings until it finds one which is greater than the 
   sought string.  While doing this it marks any which are partial 
   matches as 'candidate' results. If there is a candidate, this must be the 
   best match (because Strings is sorted in ascending order). If there is no 
   candidate, Lookup skips back to where it started, and starts scanning 
   backwards. If there is a partial match at any stage, this must be the 
   longest. Otherwise it gives up as soon as the current element of Strings 
   is less than the 1st LWD characters of the sought string.

   Lookup_gsl returns the address (char *) of the actual string in Strings, 
   or NULL.

   Normally called by do_gsl, which actually returns some info.

 *****************************************************************************/

char *
lookup_gsl(char *ss, int length, Gsl *gsl)
{
  extern int LWD ;
  char **pp, **sp, *candidate ;
  register int res ;
  int nchars ;

  if ( gsl == NULL ) return(NULL) ;
  if ( gsl->data == NULL ) return NULL ;
  /* Prev line added March 99 to efficiently cope with empty Gsls (i.e. ones
     where file name given as - or null etc */
  LWD = findchars((u_char *)" -'", (u_char *)ss, length) ;
  if ( LWD < 2 ) return NULL ;
  pp =
    (char **)bsearch((void *)ss, (void *)gsl->sptrs, gsl->numrecs,
		     sizeof(char *), cpwd) ;
  if (pp == NULL) return NULL ;

/* They match on at least LWD chars, but is *pp a substring of ss? */

  res = cpasc((u_char *)ss, length, (u_char *)*pp+1, len(*pp), &nchars) ; 
  if ( res == 0 ) return(*pp) ; 
  else {
    /* Save current pointer */
    sp = pp ; candidate = NULL ;
    while ( res & 1 ) {
	if ( res & 0x80 ) candidate = *pp ;
	pp++ ;
        res = cpasc((u_char *)ss, length, (u_char *)*pp+1, len(*pp), &nchars) ; 
	if ( res == 0 ) { candidate = *pp ; break ; } /* Found best */
	else if ( res == 0x81 ) candidate = *pp ;

     }
     /* If no candidate, res must be 2 or 82.
	If there is a candidate, this must be the best match */
    if ( candidate != NULL ) return(candidate) ;
    /* Res must be 2 or 82, recover start position & go back (NB 
       there must be a null string at the beginning of gsl.) */
    pp = sp ;
    nchars = LWD ;
    while ( nchars >= LWD ) {
        pp-- ;
        res = cpasc((u_char *)ss, length, (u_char *)*pp+1, len(*pp), &nchars) ; 
	if ( res == 0x81 || res == 0 ) { candidate = *pp ; break ; }
    }
    if ( candidate != NULL ) return(*pp) ; else return(NULL) ; 
  }
}

/*****************************************************************************

  Do_gsl 6 Oct 87 - does lookup_gsl, then if found, returns info

  Returns NULL if no match. If not NULL, returns gsclass, length
  matched (in matchlen) and, if gsclass = 'S', the address of the
  token field of the gsl record. If NULL, returns gsclass as ' '.

 *****************************************************************************/

char *
do_gsl(char *s, int l, Gsl *gsl, char *gsclass, int *matchlength, char **token)
{
   register char * p ;

   p = lookup_gsl(s, l, gsl) ;
   if ( p == NULL ) {
     *gsclass = ' ' ;
     return(NULL) ;
   }

   *matchlength = len(p) ;
   *gsclass = *(p + *matchlength + 1) ;
   if ( *gsclass == 'S' ) *token = p + *matchlength + 2 ;
   return p ;
}

/*****************************************************************************

  gsl_translate_token  1 Oct 90  SW

  Utility function to search for a token in a formatted gsl (i.e. gsl which
  has been 'read') and return term which maps to the next occurrence of the
  token.

  If gsl is null it returns null, otherwise if called with non-null token
  it returns the first term which maps to the token, else (token null) returns
  the next term which maps to the token (the sought token and the next
  position to search are saved in external storage). In all cases NULL return
  means no [more] occurrences of the token, non-NULL points to the [next]
  term which maps to the token.

*****************************************************************************/

char sav_tok[GSLTOKLEN + 1] ;
int next_num ;			/* gsl record number to start search */

char *
gsl_translate_token(Gsl *gsl, char *token)
{
  register char *tok, *p, *data ;
  register int num ;
  register BOOL found ;
  char gsclass ;

  if ( gsl == NULL ) return NULL ;
  if ( token != NULL ) {
    next_num = 0 ;
    strncpy(sav_tok, token, GSLTOKLEN) ;
    tok = token ;
  }
  else tok = sav_tok ;		/* Use stored token if token NULL */
  found = FALSE ;
  for ( num = next_num ; num < gsl->numrecs && ! found ; num++ ) {
    data = *(gsl->sptrs + num) ;
    p = data + len(data) + 1 ;
    gsclass = *p ;
    if ( gsclass == 'S' && strncmp(p + 1, tok, GSLTOKLEN) == 0 ) 
      found = TRUE ;
  }
  if ( found ) {
    next_num = ++num ;
    return data ;
  }
  else return NULL ;
}

/******************************************************************************

  new_gsl() SW March 99

  Gsl structs now to be allocated as required & atached to global pointer
  array. Looks for first NULL gsl address and returns its suffix, 0 - MAXGSLS-1,
  or -1 if no memory, -2 if no free elements.

  Guaranteed to return lowest available suffix.

******************************************************************************/

int
new_gsl(void)
{
  int i ;

  if ( Num_gsls == MAXGSLS ) return -1 ;
  for ( i = 0 ; i < MAXGSLS ; i++ ) if ( Gsls[i] == NULL ) break ;
  if ( i == MAXGSLS ) return -2 ;
  if ( (Gsls[i] = (Gsl *)bss_calloc(1, sizeof(Gsl))) == NULL ) return -1 ;
  Gsls[i]->num = i ;
  Num_gsls++ ;
  return i ;
}

/*****************************************************************************

  free_gsl()  SW July 90

  Frees memory allocated for gsl and marks it unused.
  Returns 0 if OK, non-zero if apparently not allocated.

  June 97: now, if code = 0 decrements number of links and frees if zero.
  If code != 0 force-frees it.

  free_all_gsls()

  Force-frees all GSLs, e.g. when closing a database.

*****************************************************************************/

int
free_gsl(Gsl **g, int code)
{
  Gsl *gsl = *g ;

  if ( gsl == NULL ) return 3 ;
  if ( gsl->num_links != 0 ) gsl->num_links-- ;
  /* May have no links but some memory allocated (e.g. if file read failed) */
  if ( code == 0 ) {
    if ( gsl->num_links != 0 ) return 1 ;
  }
  /* If code not 0 free regardless */
  if ( gsl->data != NULL ) bss_free(gsl->data) ;
  if ( gsl->sptrs != NULL ) bss_free(gsl->sptrs) ;
  if ( gsl->name != NULL ) bss_free(gsl->name) ;
  if ( gsl->func_name != NULL ) bss_free(gsl->func_name) ;
  bss_free(gsl) ;
  *g = NULL ;
  Num_gsls-- ;
  return 0 ;
}

int
free_all_gsls(void)
{
  int j, count ;

  for ( j = 0, count = 0 ; count < Num_gsls && j < MAXGSLS ; j++ ) 
    if ( free_gsl(Gsls + j, 1) == 0 ) count++ ;
  return count ;
}
