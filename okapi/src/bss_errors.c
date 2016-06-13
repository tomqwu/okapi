#ifndef _bss_errors_h
#define _bss_errors_h

/*****************************************************************************

  bss_errors.c  SW Jan 93

  Error handling stuff

*****************************************************************************/

#include "bss.h"
#include "bss_errors.h"
#include "charclass.h"
#include <stdarg.h>

char bss_Err_buf[2048] ;	/* For assembling current error or warning.
				   Reserved for prepare_error(). See also
				   Err_buf */
char Err_buf[2048] ;

int bss_Errno ;			/* Error number from last error or warning */

int bss_Result ;		/* Result code from last function. If it is
				   not zero, bss_Errno has the same value */
int bss_Outlength ;

FILE *Err_file, *bss_syslog ;

int Dbg ;			/* Bits for various syslog output.
				   Defines in bss_errors.h */
struct names debug_names[] = {
  { "pstgs", D_PSTGS },
  { "postings", D_PSTGS },
  { "d_pstgs", DD_PSTGS },
  { "d_postings", DD_PSTGS },
  { "misc", D_MISC },
  { "db", D_DB },
  { "database", D_DB },
  { "files", D_FILES },
  { "mem", D_MEM },
  { "memory", D_MEM },
  { "d_mem", DD_MEM },
  { "d_memory", DD_MEM },
  { "combine", D_COMBINE },
  { "comb", D_COMBINE },
  { "reader", D_READER },
  { "parser", D_PARSER },
  { "paras", D_PARAS },
  { "passages", D_PASSAGES },
  { "d_passages", DD_PASSAGES },
  { "search", D_SEARCH },
  { "d_text", DD_TEXT },
  { "gsl", D_GSL },
  { "extract", D_EXTRACT },
  { "sentence", D_SENTENCE },
  { "hash", D_HASH },
  { "", 0 }
} ;

char *bss_Errs[] = {	/* Error message list */
  OKmes,
  SPURIOUS_ERRORmes,
  DB_NOT_AVAILmes,
  NO_DB_AVAILmes,
  NO_DB_OPENmes,
  DB_OPEN_FAILmes,
  NO_SUCH_INDEXmes,
  NO_LIMITSmes,
  NO_SUCH_LIMITmes,
  NO_SUCH_ATTRIBUTEmes,
  RECORD_OUT_OF_RANGEmes,
  RECORD_OUT_OF_RANGE_DBmes,
  EMPTY_SETmes,
  END_OF_INDEXmes,
  BEGINNING_OF_INDEXmes,
  NO_FREE_SETSmes,
  SYS_ERRmes,
  NO_SUCH_FORMATmes,
  NO_SUCH_SETmes,
  NO_SUCH_RECORDmes,
  UNSUPPORTED_OPmes,
  NO_SUCH_OPmes,
  INCOMPATIBLE_OPmes,
  NO_SETSmes,
  TOO_MANY_SETSmes,
  CANT_READ_SETmes,
  CANT_WRITE_SETmes,
  NO_PROX_1mes,
  NO_PROX_2mes,
  SYNTAXmes,
  NO_SUCH_WEIGHTFUNCmes,
  NO_SUCH_SEARCHTYPEmes,
  SYS_NO_MEM_1mes,
  SYS_NO_MEM_2mes,
  NO_SUCH_QEPARMmes,
  NO_QEPARMSmes,
  RECLIST_FULLmes,
  UNSUPPORTEDCOMMANDmes,
  NOPASSAGESmes,
  NO_PROX_3mes,
  SET_NO_WEIGHTmes,
  SILLY_LIMITmes,
  CANT_SET_LIMITmes,
  BAD_PASSAGE_PARMSmes,
  NO_SET_ASSIGNEDmes,
  CANT_LIMIT_SETmes,
  NO_SUCH_STEMFUNCTIONmes,
  INT_ERRmes,
  DEFECTIVE_INDEXmes,
  ONLY_ONE_SETmes,
  CANT_GET_DOCLENSmes,
  MORE_THAN_ONE_ATTRIBUTEmes,
  IMPURE_SETmes,
  PHONEY_WEIGHT_ARGSmes,
  CANT_SETUP_ATTRIBUTEmes,
  UNKNOWN_FIELD_TYPEmes,
  CANT_READ_DOCmes,
  EMPTY_DBmes,
  DB_NOT_OPENmes,
  PHONEY_BIGN_OR_Nmes,
  TEMPUNSUPPORTEDCOMMANDmes,
  NO_SUCH_REGIMEmes,
  INVALID_REGIMEmes,
  CANT_SETUP_GSLmes,
  NO_SUCH_DEBUG_NAMEmes,
  DB_NOPARMmes,
  PHONEY_RSCOREmes,
  DOCLENS_MIXED_ATTRIBUTEmes,
  NO_RECORD_ASSIGNEDmes,
  NO_SUCH_FIELDNUMmes,
  DEFECTIVE_PASSAGESmes,
  NO_SUPERPARSEmes,
  NO_INDEX_WEIGHTmes
} ;

char *bss_Warnings[] = {
  OKmes,
  TERM_NO_POSTINGSmes,
  SET_NO_POSTINGSmes,
  SET_NO_WEIGHT_WARNINGmes,
  EMPTY_INDEXmes,
  SYS_WARNINGmes,
  NO_TF_WARNINGmes,
  NO_FIELD_TYPE_FILEmes,
} ;  

/******************************************************************************

  prepare_error(code[, args])

  Sets bss_Result and bss_Errno to code, then formats the corresponding
  message into bss_Err_buf ready for display by bss_perror()

******************************************************************************/

void
prepare_error(int code, ... )
{
  va_list ap ;
  char *p ;
  va_start(ap, code) ;
  if ( code < -(NUM_ERRORS) || code > 0 )
    code = bss_Result = bss_Errno = -1 ;
  else bss_Result = bss_Errno = code ;
  p = bss_Err_buf ;
  sprintf(p, "%d, ", code) ; p += strlen(p) ;
  vsprintf(p, bss_Errs[-code], ap) ;
  va_end(ap) ;
}

/******************************************************************************

  prepare_warning(code[, args])

  Sets bss_Result and bss_Errno to code, then formats the corresponding
  message into bss_Err_buf ready for display by bss_perror()

  Almost identical to prepare_error()

******************************************************************************/

void
prepare_warning(int code, ...)
{
  va_list ap ;
  va_start(ap, code) ;
  if ( code <= (NUM_WARNINGS) && code >= 0 ) {
    bss_Result = bss_Errno = code ;
    sprintf(bss_Err_buf, "%d, warning, ", code) ;
    vsprintf(bss_Err_buf+strlen(bss_Err_buf),
	    bss_Warnings[code], ap) ;
  }
  va_end(ap) ;
}

void
mem_err(char *func, char *mess, int bytes)
{
  sprintf(Err_buf, "out of memory allocating %d bytes for %s",
	  bytes, mess) ;
  sys_err(func) ;
}


/******************************************************************************

  bss_perror()

  Returns a static string of the form <bss_Errno>, <error message>

  The components of the string are always comma-separated

  Always succeeds (bss_Result is set to 0)

******************************************************************************/

char *
bss_perror(void)
{
  bss_Result = 0 ;
  return bss_Err_buf ;
}

/******************************************************************************

  sys_error()

******************************************************************************/

void
sys_err(char *func)
{
  if ( Err_file != NULL )
    fprintf(Err_file, "%s(): %s\n", func, Err_buf) ;
}
 
void
internal_err(char *func, char *mess)
{
  sprintf(Err_buf, "%s(): %s\n", func, mess) ;
  prepare_error(INT_ERR, Err_buf, 0) ;
  if ( bss_syslog != NULL )
    fprintf(bss_syslog, "%s\n", Err_buf) ;
}


/******************************************************************************

  Translate debug codes

  dbg_from_string(s)

  Input a string of zero or more blank-terminated words, all terminated by a
  null character, output a debug code, the OR of the keywords, or -1 if there
  is an unrecognised word.

******************************************************************************/

struct {
  char *key ;
  int code ;
} d_keys[] = {
  { "pstgs", 1 },
  { "postings", 1 },
  { "h_pstgs", 3 },
  { "h_postings", 3 },
  { "misc", 4 },
  { "db", 8 },
  { "database", 8 },
  { "files", 0x10 },
  { "mem", 0x20 },
  { "memory", 0x20 },
  { "h_mem", 0x60 },
  { "h_memory", 0x60 },
  { "record_mem", 0x80 },
  { "record_memory", 0x80 },
  { "combine", 0x100 },
  { "target", 0x200 },
  { "parser", 0x800 },
  { "reader", 0x400 },
  { "temp", 0x10000 },
} ;

void
dbg_from_string(char *s)	/* Unfinished */
{
  /* int code = 0 ; unused */
  char *p = s ;
  while ( 1 ) {
    while ( tok_space(*p++) ) ;
    if ( *p == '\0' ) break ;
    /*    if ( strcasecmp*/
  }
}

#endif
