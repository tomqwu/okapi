/******************************************************************************

  bss_db_desc.c

******************************************************************************/

#include "bss.h"
#include "bss_errors.h"
#include "charclass.h"

#define PR(f,fldname,fldcontent,format) \
  fprintf(f,"%s=%format\n", (fldname), (fldcontent))

static char *store(char *, char *, char **) ;
static char *storen(char *, char *, char **, int) ; 
static int err_mess(char *) ;
     
char *tags[] = {
  "name", "***", "lastbibvol", "bib_basename", "bib_dir", /* 0 - 4 */
  "bibsize", "real_bibsize",	                          /* 5 - 6 */
  "display_name", "explanation", "nr", "nf",              /* 7 - 10 */
  "f_abbrev", "rec_mult", "ni",                           /* 11 - 13 */
  "ix_stem", "fixed", "***", "***********",               /* 14 - 17 */
  "*************", "ix_type", "db_type",                  /* 18 - 20 */
  "has_lims", "maxreclen",                                /* 21 - 22 */
  /* Next 7 no longer used Dec 97 but not rejected if in parm */
  "k1_bm11", "k2_bm11", "k1_bm15", "k2_bm15",             /* 23 - 26 */
  "k1_bm25", "k2_bm25", "bm25_b",                         /* 27 - 29 */
  "last_ixvol", "ix_volsize",                             /* 30 - 31 */
  "parafile_type",		                          /* 32 */
  "stemfunc", "extreg", "gsl",	/* 33-35, Apr 02 for profile type db */
  "no_drl",			/* 36 1-field db has reclens in .bibdrl,
				   unless this is true. */
  ""
} ;

/* "bib" unused from July 93 */

/*****************************************************************************
  read_db_desc()                                                  Feb, 1990
                                                                  A. Goker

  Nov 99: whole thing needs tidying as regards handling of errors, defaults etc
          Should checking be done here at all?
  July 96: Made bib_basename array
  1 June 94: added k1 and k2 for each index

  19 July 93: deleted "bib" and "upd"
  22 June 92: deleted some fields and added lastbibvol, bib_basename,
              bib_dir and bibsize, for multivolume bib files
  27 May 92: added BOOL has_lims
  27 Feb 92: corrected bug in case 24 (another level of indirection)
  27 Nov 91: added index chunk sizes and index type (SW)

  Nov 91: this function should diagnose all the errors rather than dropping
  out on each one. There's something buggy about the diagnostics too.
  I think 'i' may be out by one.

  8 March 90: Altered case 4 and 5 so accepts 0 for nr (new file has no
  records) - SW

*****************************************************************************/

static char *
store(char *ptr, char *tag_var, char **field)
{
  strcpy(ptr, tag_var);
  *field = ptr;
  ptr += strlen(tag_var) + 1;
  return(ptr);           

}

static char *
storen(char *ptr, char *tag_var, char **field, int n)
{
  strncpy(ptr, tag_var, n);
  *field = ptr;
  ptr += n ;
  *ptr++ = '\0' ;
  return(ptr);           
}



int
read_db_desc (char *db_name, Db *dbs)
{

  FILE * f ;		  /* file pointer - to '.db_desc' file */
  int i, j ;		  /* references tags elements etc. */
  int iRet;		  /* is 0 if all is ok, -1 at EOF, < -1 on err */
  int c;		  /* used in atoi conversion */
#ifdef HAVE_LL
  LL_TYPE ll ;
#endif
  int filesize ;          /* size of db_desc file */
  char fname[PATHNAME_MAX] ;	  /* For full pathname - added SW 26 Feb 90 */
  char tag[512];          /* contains string upto '=', ie. 'name' */
  int tag_number ;
  char tag_var[512];      /* contains string after '=', ie. the actual name */
  char *ptr;	          /* points to positions in parm_buffer */
  int bib_dir_num = 0 ;	  /* when reading bib_dir */
  int bibsize_num = 0 ;	  /* when reading bibsize */
  int real_bibsize_num = 0 ;/* when reading real_bibsize */
  int basename_num = 0 ;	/* When reading bib_basename */
  int f_a_num = 0 ;       /* field number when reading field name abbrevs */
  int i_type_num = 0 ;    /* index number when reading index type */
  int last_ixvol_num = 0 ;	/* Count last_ixvols (ni of them) */
  int ix_volsize_num[BSS_MAXIX] ;	/* Count ix vol sizes for each index */
  int ix_volsize_num_ixnum = 0 ;
  int ix_stem_num[BSS_MAXIX] ;	/* Count ix_stem names for each index */
  int ix_stem_num_ixnum = 0 ;

  int Missing;	          /* flag set if any of the fields are missing */
  int Found;	          /* flag set if a valid label is found for string */
  
  char *func = "read_db_desc" ;

  if (dbs->parm_buffer != NULL) {   /* free malloc'd memory if struct */
    free_buf(dbs);	          /* was used before */
  }

  iRet = 0;

  /***** struct initialisation ******/
  memset(dbs, '\0', sizeof(Db)) ;
  memset((char *)&ix_volsize_num, '\0', sizeof(ix_volsize_num)) ;
  memset((char *)&ix_stem_num, '\0', sizeof(ix_stem_num)) ;
  for ( i = 0 ; i < BSS_MAXIX ; i++ ) {
    for ( j = 0 ; j < MAXIXVOLS ; j++ ) 
#ifdef LARGEFILES
      dbs->ix_volsize[i][j] = (OFFSET_TYPE)4095 * (OFFSET_TYPE)1024 * (OFFSET_TYPE)1024 ; 
#else
      dbs->ix_volsize[i][j] = 2047 * 1024 * 1024 ;
#endif
  }
  for ( i = 0 ; i < MAXBIBVOLS ; i++ ) {
#ifdef LARGEFILES
    dbs->bibsize[i] = 4095 ;
#else
    dbs->bibsize[i] = 2047 ;
#endif
  }
  dbs->rec_mult = 1 ;
  dbs->parafile_type = DEFAULT_PARAFILE_TYPE ;

  /**** end of struct initialisation ****/

  sprintf(fname, "%s"D_C"%s", Control_dir, db_name) ;
  f = bss_fopen(fname, "rb") ;
  if (f == NULL ) {
    sprintf(Err_buf, "can't find file %s (or file is empty)", fname);
    sys_err(func) ;
    return ST_FAIL_FOPEN ;
  }
  filesize = (int)ofsize(fileno(f)) ;
  dbs->parm_buffer = (char *)bss_calloc(filesize, 1);

  ptr = dbs->parm_buffer;
  for (tag_number = 0; strlen(tags[tag_number]) > 0; tag_number++) ;

  while ( ( iRet = read_parm_line (f, tag, tag_var) ) == 0 || iRet == -4 ) {
  /* If iRet is <= -1 then there is an error(or EOF) , hence no point in
        continuing reading the remaining description lines (exc. iRet -4,
	blank line)
     Else if iRet==0 then everything was ok on the last line read,
        now check if 'read_parm_line' detects any error.           */
    
    if ( iRet == -4 ) continue ; /* Blank line OK */

    Found=FALSE;
    for (i=0; i<tag_number && !Found; i++) {
      if ( strcmp(tag, tags[i]) == 0 ) {
	Found=TRUE;
	switch (i) {
	case 0:
	  ptr = store(ptr, tag_var, &dbs->name);
	  break;
	case 2:
	  if ( ( c = atoi(tag_var) ) < 0 )
	    iRet = -9;
	  else if ( c > MAXBIBVOLS - 1 && ( Dbg & D_DB ) )
	    fprintf(Err_file,
		    "ignoring lastbibvol > %d\n", MAXBIBVOLS - 1 ) ;
	  else dbs->lastbibvol = c;
	  break;
	case 3:
	  if ( basename_num > dbs->lastbibvol && ( Dbg & D_DB ) )
	    fprintf(Err_file,
		    "Ignoring superfluous bib_basename %s\n", tag_var) ;
	  else
	    ptr = store(ptr,tag_var, &dbs->bib_basename[basename_num++]);
	  break ;
	case 4:
	  qaddslash(tag_var) ;
	  if ( bib_dir_num > dbs->lastbibvol && (Dbg & D_DB) )
	    fprintf(Err_file, "Ignoring superfluous bib_dir %s\n", tag_var) ;
	  else ptr = store(ptr, tag_var, &dbs->bib_dir[bib_dir_num++]);
	  break ;
	case 5:
	  c = atoi(tag_var) ;
#ifndef LARGEFILES
	  if ( c >= 2048 ) {
	    fprintf(Err_file, "Bibsize %d, this must be in megabytes\n",
		    c) ;
	    fprintf(Err_file, "Maximum size of a volume is 2047 MB\n") ;
	    return ST_FAIL_MISC ;
	  }
#endif
	  /* Removed June 98, can be done by conversion progs,
	     shouldn't be in the parm at all really */
	  /* c *= 1024 * 1024 ;  Later divided by rec_mult */
	  if ( bibsize_num > dbs->lastbibvol && (Dbg & D_DB) )
	    fprintf(Err_file, "Ignoring superfluous bibsize value %d\n", c) ;
	  else dbs->bibsize[bibsize_num++] = c ;
	  break ;
	case 6:
#ifdef LARGEFILES
	  ll = atoll(tag_var) ;
	  dbs->real_bibsize[real_bibsize_num++] = ll ;
#else	  
	  c = atoi(tag_var) ;
	  dbs->real_bibsize[real_bibsize_num++] = c ;
#endif
	  break ;
	case 7:
	  ptr = store(ptr, tag_var, &dbs->display_name);
	  break;
	case 8:
	  ptr = store(ptr, tag_var, &dbs->explanation);
	  break;
	case 9:
	  if ( ( c = atoi(tag_var) ) < 0 )
	    iRet = -9;
	  else 
	    dbs->nr = c;
	  break;
	case 10:
	  if ( ( c = atoi(tag_var) ) <= 0 )
	    iRet = -7;
	  else
	    dbs->nf = c; break;
	  break;
	case 11:
	  if ( f_a_num >= dbs->nf )
	    fprintf(Err_file, "%s(): too many field abbrevs, ignoring\n",
		    func) ;
	  else {		/* Hard coded max abbrev length of 3 */
	    if ( strlen(tag_var) > 3 )
	      fprintf(Err_file, "%s(): field abbrev %s trunc to 3 chars\n",
		      func, tag_var) ;
	    /* Store 3 chars, storen ensures null-terminated */
	    ptr = storen(ptr, tag_var, &dbs->f_abbrevs[++f_a_num], 3);
	  }
	  break ;
	case 12:
	  c = atoi(tag_var) ;
	  dbs->rec_mult = c;
	  break;
	case 15:
	  if ( ( c = atoi(tag_var) ) < 0)
	    iRet = -9;
	  else 
	    dbs->fixed = c; break;
	  break;
	
	case 13:
	  if ( ( (c = atoi(tag_var)) < 0 ) || (c > BSS_MAXIX) )
	    iRet = -11;
	  else {
	    dbs->ni = c;
	    if ( Dbg & D_DB ) {
	      if ( c == 0 ) fprintf(bss_syslog, "Warning: no indexes\n") ;
	    }
	  }
	  break;
	case 14:
	  /* ix_stem_num_ixnum is the number of the index to which this
	     ix_stem belongs (worked out by checking against last_ixvol
	     for the current ix_stem_num_ixnum)

	     So must have last_ixvol before any of these for the index in
	     question, or better, for all indexes. */
	  if ( ix_stem_num[ix_stem_num_ixnum] >
	       dbs->last_ixvol[ix_stem_num_ixnum] )
	    ix_stem_num_ixnum++ ;
	  if ( ix_stem_num_ixnum >= dbs->ni ) {
	    if ( Dbg & D_DB )
	      fprintf(bss_syslog,
		      "Ignoring superfluous index stem %s\n", tag_var) ;
	  }
	  else
	    ptr = store(ptr, tag_var,
    &dbs->ix_stem[ix_stem_num_ixnum][ix_stem_num[ix_stem_num_ixnum]++]) ;
	  break;
	case 16:		/* gsl name */
	  break;
	case 18:		/* stemming function name for corresponding
				   gsl */
	  break;
	case 17:
	  break;
	case 19:
	  c = atoi(tag_var) ;
	  if ( i_type_num > dbs->ni - 1 ) {
	    if ( Dbg & D_DB )
	      fprintf(bss_syslog, "Ignoring superfluous index type number\n") ;
	  }
	  else if ((c == 4 || c == 6 || c == 8) && dbs->nf > 4)
	    iRet = -11 ;
	  else {
	    if ( (Dbg & D_DB) && (c < 4 || (c > 13 && c != 20) ) )
	      fprintf(bss_syslog,
		      "Won't be able to use index %d of unsupported type %d\n",
		      i_type_num, c) ;
	    dbs->itype[i_type_num++] = c ;
	  }
	  break ;
	case 20:		/* Database type name. Will need validation */
	  /* March 95: may need several classes, blank separated  */
	  /* Or OK if field repeats. */
	  ptr = store(ptr, tag_var, &dbs->db_type) ;
	  {
	    char *p = tag_var ;
	    int l = strlen(p) ;
	    int ol, offset ;
	    while ( l > 0 ) {
	      offset = find_tok(p, l, &ol, TOK_ESTART, TOK_FIN) ;
	      if ( offset < 0 ) break ;
	      p += offset ; l -= offset ;
	      if ( strncasecmp(p, "cat", ol) == 0 )
		dbs->i_db_type |= DB_CAT_TYPE ;
	      else if ( strncasecmp(p, "ai", ol) == 0 )
		dbs->i_db_type |= DB_AI_TYPE ;
	      else if ( strncasecmp(p, "text", ol) == 0 ) 
		dbs->i_db_type |= DB_TEXT_TYPE ;
	      else if ( strncasecmp(p, "freetext", ol) == 0 ) 
		dbs->i_db_type |= DB_TEXT_TYPE ;
	      else if ( strncasecmp(p, "free_text", ol) == 0 ) 
		dbs->i_db_type |= DB_TEXT_TYPE ;
	      else if ( strncasecmp(p, "external", ol) == 0 ) 
		dbs->i_db_type |= DB_EXTERNAL_TYPE ;
	      else if ( strncasecmp(p, "extern", ol) == 0 ) 
		dbs->i_db_type |= DB_EXTERNAL_TYPE ;
	      else if ( strncasecmp(p, "ext", ol) == 0 ) 
		dbs->i_db_type |= DB_EXTERNAL_TYPE ;
	      else if ( strncasecmp(p, "free_format", ol) == 0 ) 
		dbs->i_db_type |= DB_FREE_FORMAT_TYPE ;
	      else if ( strncasecmp(p, "freeformat", ol) == 0 ) 
		dbs->i_db_type |= DB_FREE_FORMAT_TYPE ;
	      else if ( strncasecmp(p, "large", ol) == 0 ) 
		dbs->i_db_type |= DB_LARGE_TYPE ;
	      else if ( strncasecmp(p, "prof", ol) == 0 ) 
		dbs->i_db_type |= DB_PROFILE_TYPE ;
	      else if ( strncasecmp(p, "profile", ol) == 0 ) 
		dbs->i_db_type |= DB_PROFILE_TYPE ;
	      else {
		if ( Dbg & D_DB )
		  fprintf(bss_syslog,
			  "Ignoring unknown database type name %s\n", p) ;
	      }
	      p += ol ; l -= ol ;
	    }
	  }
	  break;

	case 21:
	  c = atoi(tag_var) ;
	  dbs->has_lims = (c != 0) ;
	  break ;

	case 22:
	  c = atoi(tag_var) ;
	  dbs->maxreclen = c ;
	  break ;
	case 23:
	  break;
	case 24:
	  break;
	case 25:
	  break;
	case 26:
	  break;
	case 27:
	  break;
	case 28:
	  break;
	case 29:
	  break;
	case 30:
	  c = atoi(tag_var) ;
	  if ( c > MAXIXVOLS - 1 ) {
	    iRet = -17 ;
	    break ;
	  }
	  if ( last_ixvol_num > dbs->ni - 1 && (Dbg & D_DB) )
	    fprintf(Err_file, "Ignoring superfluous last_ixvol\n") ;
	  else dbs->last_ixvol[last_ixvol_num++] = c;
	  break;
	case 31:
	  /* This should be a 2D array, temp only (May 95) */
	  c = atoi(tag_var) ;
#ifndef LARGEFILES
	  if ( c < 0 || c > 2047 ) {
	    iRet = -18 ;
	    break ;
	  }
#else
	  if ( c < 0 || c > 524287 ) {
	    iRet = -18 ;
	    break ;
	  }
#endif
	  if ( ix_volsize_num[ix_volsize_num_ixnum] >
	       dbs->last_ixvol[ix_volsize_num_ixnum] )
	    ix_volsize_num_ixnum++ ;
	  if ( ix_volsize_num_ixnum >= dbs->ni ) {
	    if ( Dbg & D_DB )
	      fprintf(bss_syslog,
		      "Ignoring superfluous index volsize %s\n", tag_var) ;
	  }
	  else
  dbs->ix_volsize[ix_volsize_num_ixnum][ix_volsize_num[ix_volsize_num_ixnum]++]
    = 1024 * 1024 * (OFFSET_TYPE)c ;
	  break ;
	case 32:
	  c = atoi(tag_var) ;
	  if ( c < 0 || c > 2 ) {
	    fprintf(Err_file,
		    "Parameter parafile_type=%d, must be 0, 1 or 2\n", c);
	    return ST_FAIL_MISC ;
	  }
	  dbs->parafile_type = c ;
	  break ;
	  /* Next three cases for profile dbs only (Apr 02) */
	case 33:
	  ptr = store(ptr, tag_var, &dbs->stemfunc);
	  break ;
	case 34:
	  ptr = store(ptr, tag_var, &dbs->extreg);
	  break ;
	case 35:
	  ptr = store(ptr, tag_var, &dbs->gsl);
	  break ;
	case 36:
	  c = atoi(tag_var) ;
	  dbs->no_drl = c ;	/* But is written as 1 or 0 */
	  break ;
	}			/* end of switch (i) */
      }				/* end of 'if' */
      if ( iRet < 0 ) break ;	/* This added SW Nov 91 */
    }				/* end of 'for' */
    if (!Found && (Dbg & D_DB) )
	fprintf(bss_syslog, "Unknown tag '%s'\n", tag);
    if (iRet < 0 && iRet != -4 ) break ; /* (Nov 91 */
  }				/* end of 'while' - parm file read complete*/

  /* i--; Deleted Dec 91 - seems wrong (SW) */

  switch (iRet) {
  case -2:
    fprintf(Err_file, "ERROR: End of Line is read before '='\n"); break;
  case -3:
    fprintf(Err_file, "ERROR: EOF read before '='\n"); break;
  case -7:
    fprintf(Err_file, "%s: %s can not be <= 0\n",tags[i],tags[i]);
    break;
  case -9:
    fprintf(Err_file, "%s: %s can not be < 0\n",tags[i],tags[i]);
    break;
  case -11:
    fprintf(Err_file,
 "%s: %s must be 4 - 9 or 10 and can't have %s=4, 6 or 8 if nf > 4\n",
	     tags[i],tags[i], tags[i]);
    break;
  case -12:
    fprintf(Err_file, "%s: Can not have more than BSS_MAXIX indexes\n",
	    tags[i]) ;
    break;
  case -17:
    fprintf(Err_file, "%s: last index volume number must be less than %d\n",
	    tags[i], MAXIXVOLS) ;
    break ;
  case -18:
#ifndef LARGEFILES
    fprintf(Err_file,
   "%s: index volume size must be in megabytes and between 1 and 2047\n",
	    tags[i]) ;
#else
    fprintf(Err_file,
   "%s: index volume size must be in megabytes and between 1 and 524287\n",
	    tags[i]) ;
#endif    
    break ;
  }

  if ( (iRet < -1) && (iRet >= -18) ) {
    free_buf(dbs);
    return ST_FAIL_MISC ;
  }
  else {
    Missing = FALSE;
    for (i=0; i < tag_number && !Missing; i++) {
      switch (i) {
      case 10:
	if (dbs->nf == 0)
	  Missing = err_mess ("nf");
	break;
      default:			/* cases mainly concerned with */
	switch (i) {
	case 0:
	  if (dbs->name == NULL) Missing = TRUE; break;
	case 7:
	  if (dbs->display_name == NULL) Missing = TRUE; break;
	case 10:
	  if (dbs->nf == 0) Missing = TRUE; break;
	}			/* end of 'default switch'i */
	if (Missing) err_mess(tags[i]);
	break;
      }				/* end of 'switch' */
	     
    }				/* end of 'for' */
  
  }				/* end of 'else' */

  bss_fclose(f);
  if (Missing) {
     free_buf(dbs);
     return ST_FAIL_MISC ;
  }
  /* Nov 99: as next comment */
  if ( dbs->ni > 0 && dbs->ix_stem[0][0] == NULL ) {
    dbs->ix_stem[0][0] = dbs->bib_dir[0] ;
  }
  /* Nov 96: added temp til separate index parms */
  for ( i = 0 ; i < dbs->ni ; i++ ) {
    for ( j = 0 ; j <= dbs->last_ixvol[i] ; j++ ) {
      if ( dbs->ix_stem[i][j] == NULL )
	dbs->ix_stem[i][j] = dbs->ix_stem[0][0] ;
    }
  }
  if ( dbs->nf == 1 && ! dbs->no_drl ) {
    dbs->fddir_recsize = 0 ;
    if ( dbs->i_db_type & DB_TEXT_TYPE ) dbs->dir_lensize = 3 ;
    else dbs->dir_lensize = 2 ;
  }
  else {
    dbs->dir_lensize = 0 ;
    if ( dbs->i_db_type & DB_TEXT_TYPE ) dbs->fddir_recsize = 3 ;
    else dbs->fddir_recsize = 2 ;
  }
  if ( ! (dbs->i_db_type & DB_TEXT_TYPE) ) dbs->parstat = -2 ;
  if ( dbs->i_db_type & DB_LARGE_TYPE ) dbs->dir_recsize = 5 ;
  else dbs->dir_recsize = 4 ;

  return ST_OK ;
}    
    


/*****************************************************************************
  free_buf()                                                      Feb, 1990
                                                                  A. Goker
  Frees memory allocated by 'malloc' and initialises parm_buffer

*****************************************************************************/

void
free_buf(Db *db)
{
  if ( db->paraptrs != NULL ) bss_free(db->paraptrs) ;
  if ( db->recbuf != NULL ) bss_free(db->recbuf) ;
  if ( db->lims != NULL ) bss_free(db->lims) ;
  if ( db->parabuf != NULL ) bss_free(db->parabuf) ;
  if ( db->parm_buffer != NULL ) bss_free(db->parm_buffer) ;
  if ( Dbg & D_DB ) fprintf(bss_syslog, "Freed database buffers\n") ;
  memset((char *)db, 0, sizeof(Db)) ;
}

/*****************************************************************************
  err_mess()                                                      Feb, 1990
                                                                   A. Goker
  Prints error message for elements in dbs that should but don't have
  a ('meaningful') value attached to them.
*****************************************************************************/

static int
err_mess(char *s)
{
  sprintf(Err_buf, "read_db_desc(): missing: error/empty on '%s'\n", s);
  fprintf(Err_file, "%s", Err_buf) ;
  return 1 ;
}

  
/***************************************************************************
  read_parm_line()                                                Feb, 1990
                                                                  Thien Do
  This routine read each line from the file 'f' then parsing it into two 
  string S1 and S2 by the deliminator '=' ( which occurs first).
  Return Value :
      0 : Normal Case of read from the file.
     EOF: Reach the EOF.
  ERROR : When the read line does not contain '='
          -2 : read up to '\n' but not found '='
          -3 : read up to 'EOF' but not found '='
	  -4 : the blank line or (Apr 92) commented line 
******************************************************************************/

int read_parm_line(FILE *f, char *s1, char *s2)
{
  int c;                       /* Input character from file */
  int bEqual=FALSE;		/* Test of Equal sign. 
				  TRUE : Input string includes '='
				  False : no '=' */
  int iLens1=0, iLens2=0;	/* Counts of string lengths */
  int iRet;			/* return value */
  int i=0;
  char *runh, *runt;		/* for truncation on S1 */
			
  *s1='\0' ; *s2='\0';
  while ((c=getc(f))!=EOF && c != '\r' && c!='\n') 
    if (c=='='&& !bEqual) bEqual=TRUE; 
    else if (!bEqual) {
      *(s1+iLens1)=c; iLens1++;
    } else if (bEqual) {
      *(s2+iLens2)=c; iLens2++;
    } 
  if ( c == '\r' ) getc(f) ;
  /* truncate Leading and trailing spaces in the string S1 */
  if (iLens1>0){
    iLens1--;
    runh = s1;
    runt = s1+iLens1;
    while (*runh == ' ' && iLens1 >=0) ++runh;   /* truncate leading spaces */
    while (*runt==' ') --runt;	         /* truncate ending spaces */
    *(++runt)='\0';
    while ((s1[i] = runh[i])!= '\0') i++;     /* copy string */
  } 
  if (iLens2>0) *(s2+iLens2)='\0';

  /* Test for condition of return */
  if (c==EOF && !bEqual) {             
    if (*s1=='\0') iRet= EOF;	          /* Normal */
    else iRet = -3;		          /* Error : Read EOF before '=' */
  }
  else if (c=='\n' && !bEqual && *s1=='\0' && *s2=='\0') iRet=-4;
				          /* Error : This is a blank line */
  else if (c=='\n' && !bEqual && *s1!='\0') iRet=-2; 
                                          /* Error : '\n' read before '=' */
  else iRet =0;			          /* normal return */
  if ( iRet != EOF && *s1 == '#' ) iRet = -4 ; /* Added Apr 92 SW */
  return (iRet);
}


/*****************************************************************************
  write_db_desc()                                                28 Feb, 1990
                                                                 Ayse Goker
  Writes out the relevant fields of dbs back onto the file specified.
  The reverse of read_db_desc.

  Returns 0 if OK, otherwise - (SW, noticed and added Nov 94)

  NOT used by search programs, only by conversion I think.

  WARNING: read_db_desc has to be done first (of course).
  close_db() must NOT be called first (as this frees some of the memory
  used for the db struct (noticed 3 Apr 92 ref ixf)

*****************************************************************************/

int
write_db_desc(char *db_name, Db *dbs)
{
  FILE *f;			/* file ptr - to .db_desc that will write to */
  char fname[PATHNAME_MAX];	        /* For full pathname  */
  int tnum ;
  int c ;
  int j, k ;
  char *str ;			/* Jan 2000: string to be written */

  sprintf(fname, "%s"D_C"%s", Control_dir, db_name) ;
  if ( (f = bss_fopen(fname, "w")) == NULL ) {
    fprintf(Err_file, "can't open database parameter file %s for writing\n",
	    fname) ;
    return -1 ;
  }

  tnum = 0;
  fprintf(f, "%s=%s\n", tags[tnum],dbs->name);
  tnum = 2;
  fprintf(f, "%s=%d\n", tags[tnum],dbs->lastbibvol);
  tnum = 3;
  for ( j = 0 ; j <= dbs->lastbibvol ; j++ )
    fprintf(f, "%s=%s\n", tags[tnum],dbs->bib_basename[j]);
  tnum = 4;
  for ( j = 0 ; j <= dbs->lastbibvol ; j++ )
    fprintf(f, "%s=%s\n", tags[tnum],dbs->bib_dir[j]) ;
  tnum = 5;
  for ( j = 0 ; j <= dbs->lastbibvol ; j++ ) {
    c = (dbs->bibsize[j]) ;
    /*c = (dbs->bibsize[j] * dbs->rec_mult / (1024 * 1024) ) ;*/
    fprintf(f, "%s=%d\n", tags[tnum],c) ;
  }
  tnum = 6;
  for ( j = 0 ; j <= dbs->lastbibvol ; j++ ) 
#ifdef LARGEFILES
#ifdef _WIN32
    fprintf(f, "%s=%I64d\n", tags[tnum],dbs->real_bibsize[j]) ;
#else
    fprintf(f, "%s=%lld\n", tags[tnum],dbs->real_bibsize[j]) ;
#endif
#else
    fprintf(f, "%s=%d\n", tags[tnum],dbs->real_bibsize[j]) ;
#endif
  tnum = 7;
  if ( ! dbs->display_name) str = empstr ;
  else str = dbs->display_name ;
  fprintf(f, "%s=%s\n", tags[tnum],str);
  tnum = 8;
  if ( ! dbs->explanation ) str = empstr ;
  else str = dbs->explanation ;
    fprintf(f, "%s=%s\n", tags[tnum], str);
  tnum = 9;
  fprintf(f, "%s=%d\n", tags[tnum],dbs->nr);
  tnum = 10;
  fprintf(f, "%s=%d\n", tags[tnum],dbs->nf);
  tnum = 11;
  for ( j = 1 ; j <= dbs->nf ; j++ ) {
    if ( ! dbs->f_abbrevs[j] ) str = empstr ;
    else str = dbs->f_abbrevs[j] ;
    fprintf(f, "%s=%s\n", tags[tnum], str) ;
  }
  /* No longer output rec_mult unless > 1 (virtually obsolete) */
  tnum = 12;
  if ( dbs->rec_mult > 1 )
    fprintf(f, "%s=%d\n", tags[tnum], dbs->rec_mult);
  /* No longer output zero fixed field (virtually obsolete) */
  tnum = 15;
  if ( dbs->fixed > 0 ) 
    fprintf(f, "%s=%d\n", tags[tnum],dbs->fixed);
  tnum = 20;
  if ( ! dbs->db_type ) str = empstr ;
  else str = dbs->db_type ;
  fprintf(f, "%s=%s\n", tags[tnum], str) ;
  tnum = 21;
  fprintf(f, "%s=%d\n", tags[tnum],dbs->has_lims) ;
  tnum = 22;
  fprintf(f, "%s=%d\n", tags[tnum],dbs->maxreclen) ;
  tnum = 32 ;
  if ( dbs->parafile_type != DEFAULT_PARAFILE_TYPE )
    fprintf(f, "%s=%d\n", tags[tnum], dbs->parafile_type);
  tnum = 13;
  fprintf(f, "%s=%d\n", tags[tnum],dbs->ni);
  /* Now the parms for each index */
  for ( j = 0 ; j < dbs->ni ; j++ ) {
    tnum = 30 ;
    fprintf(f, "%s=%d\n", tags[tnum], dbs->last_ixvol[j]) ;
    for ( k = 0 ; k <= dbs->last_ixvol[j] ; k++ ) {
      tnum = 14;
      fprintf(f, "%s=%s\n", tags[tnum], dbs->ix_stem[j][k]);
      tnum = 31;
      fprintf(f, "%s=%d\n",
	      tags[tnum], (int)(dbs->ix_volsize[j][k] / (1024 * 1024))) ;
    }
    tnum = 19;
    fprintf(f, "%s=%d\n", tags[tnum],dbs->itype[j]) ;
  }
  if ( dbs->i_db_type & DB_PROFILE_TYPE ) {
    tnum = 33 ;
    str = dbs->stemfunc ;
    fprintf(f, "%s=%s\n", tags[tnum], str) ;
    tnum = 34 ;
    str = dbs->extreg ;
    fprintf(f, "%s=%s\n", tags[tnum], str) ;
    tnum = 35 ;
    str = dbs->gsl ;
    fprintf(f, "%s=%s\n", tags[tnum], str) ;
  }
  tnum = 36 ;
  fprintf(f, "%s=%d\n", tags[tnum], (dbs->no_drl != 0)) ;

  bss_fclose(f);
  return 0 ;
}
    
