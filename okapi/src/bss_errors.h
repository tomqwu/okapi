/******************************************************************************

  bss_errors.h  SW Jan 93

  Okapi BSS error-handling

******************************************************************************/

extern int bss_Errno ;		/* Error number from last error or warning */
extern int bss_Result ;		/* Result code from last function. If it is
				   not zero, bss_Errno has the same value */
extern int bss_Outlength ;
extern char *bss_Errs[1024] ;	/* Error and warning message list */
extern char bss_Err_buf[2048] ;	/* For assembling current error */
extern char Err_buf[2048] ;	/* System-type errors (for sys_err()) */
extern FILE *Err_file, *bss_syslog ;
extern int Dbg ;
extern struct names debug_names[] ;

/******************************************************************************

  Defines for debug bits (for global Dbg)

******************************************************************************/

#define D_PSTGS 1
#define DD_PSTGS 2
#define D_MISC 4
#define D_DB 8
#define D_FILES 0x10
#define D_MEM 0x20
#define DD_MEM 0x40
#define D_RECORD_MEM 0x80
#define D_COMBINE 0x100
#define D_TARGET 0x200
#define D_READER 0x400
#define D_PARSER 0x800
#define D_PARAS 0x1000
#define D_PASSAGES 0x2000
#define DD_PASSAGES 0x4000
#define D_SEARCH 0x8000
#define D_TEMP_1 0x10000
#define DD_TEXT 0x20000
#define D_GSL   0x40000
#define D_EXTRACT 0x80000
#define D_SENTENCE 0x100000
#define D_HASH 0x200000

#define OK 0
#define OKmes "OK"
#define SPURIOUS_ERROR 1
#define SPURIOUS_ERRORmes "Unknown error code, please contact programmers"

/* Misc status codes */

#define ST_UNTRIED 0
#define ST_OK 1
#define ST_FAIL_FOPEN -2
#define ST_FAIL_MISC -1
#define ST_FAIL_MEM -5
#define ST_FAIL_READ -3
#define ST_FAIL_WRITE -4
#define ST_FAIL_GSL -6

/* Database */

#define DB_NOT_AVAIL -2
#define DB_NOT_AVAILmes "database not available, db=%s"
#define NO_DB_AVAIL -3
#define NO_DB_AVAILmes "no databases available"
#define NO_DB_OPEN -4
#define NO_DB_OPENmes "no database open"
#define DB_OPEN_FAIL -5
#define DB_OPEN_FAILmes "database open failed, db=%s"
#define NO_SUCH_INDEX -6
#define NO_SUCH_INDEXmes \
     "index invalid for this database, db=%s, index_num=%s"
#define NO_LIMITS -7
#define NO_LIMITSmes "no limits available for this database, db=%s"
#define NO_SUCH_LIMIT -8
#define NO_SUCH_LIMITmes "limit name not recognized, limit=%s"
#define NO_SUCH_ATTRIBUTE -9
#define NO_SUCH_ATTRIBUTEmes \
"attribute name not found for this database, name=%s, db=%s"
#define RECORD_OUT_OF_RANGE -10
#define RECORD_OUT_OF_RANGEmes "record out of range for set, rec=%s, set=%s"
#define RECORD_OUT_OF_RANGE_DB -11
#define RECORD_OUT_OF_RANGE_DBmes "record out of range for database, irn=%s-%s"
#define EMPTY_SET -12
#define EMPTY_SETmes "set %s is empty"
#define END_OF_INDEX -13
#define END_OF_INDEXmes "end of index"
#define BEGINNING_OF_INDEX -14
#define BEGINNING_OF_INDEXmes "beginning of index"
#define NO_FREE_SETS -15
#define NO_FREE_SETSmes "no free sets"
#define SYS_ERR -16
#define SYS_ERRmes "system error, %s"
#define NO_SUCH_FORMAT -17
#define NO_SUCH_FORMATmes "no such format, format=%s"
#define NO_SUCH_SET -18
#define NO_SUCH_SETmes "no such set, set=%s"
#define NO_SUCH_RECORD -19
#define NO_SUCH_RECORDmes "no such record, rec=%s, set=%s"
#define UNSUPPORTED_OP -20
#define UNSUPPORTED_OPmes "operation not available with this type of index"
#define NO_SUCH_OP -21
#define NO_SUCH_OPmes "no such operation"
#define INCOMPATIBLE_OP -22
#define INCOMPATIBLE_OPmes "set(s) incompatible with op, set=%s"
#define NO_SETS -23
#define NO_SETSmes "no sets"
#define TOO_MANY_SETS -24
#define TOO_MANY_SETSmes "can't combine more than %s sets"
#define CANT_READ_SET -25
#define CANT_READ_SETmes "can't read postings, set=%s"
#define CANT_WRITE_SET -26
#define CANT_WRITE_SETmes "can't write output file, set=%s"
#define NO_PROX_1 -27
#define NO_PROX_1mes "can't do proximity op, sets not all same type"
#define NO_PROX_2 -28
#define NO_PROX_2mes "can't do proximity op, set is wrong type, set=%s"
#define SYNTAX -29
#define SYNTAXmes "interface syntax error, %s %s"
#define NO_SUCH_WEIGHTFUNC -30
#define NO_SUCH_WEIGHTFUNCmes "unknown weight function number, func=%s"
#define NO_SUCH_SEARCHTYPE -31
#define NO_SUCH_SEARCHTYPEmes "unimplemented search type, type=%s"
#define SYS_NO_MEM_1 -32
#define SYS_NO_MEM_1mes "unable to allocate memory, func=%s, bytes=%s"
#define SYS_NO_MEM_2 -33
#define SYS_NO_MEM_2mes "unable to allocate memory, command=%s, bytes=%s"
#define NO_SUCH_QEPARM -34
#define NO_SUCH_QEPARMmes "query expansion parameter not recognized, name=%s"
#define NO_QEPARMS -35
#define NO_QEPARMSmes "no query expansion parameters for this database"
#define RECLIST_FULL -36
#define RECLIST_FULLmes "stored record list full"
#define UNSUPPORTEDCOMMAND -37
#define UNSUPPORTEDCOMMANDmes "unsupported command, command=%s"
#define NOPASSAGES -38
#define NOPASSAGESmes "can't do passage searching on non-text-type database"
#define NO_PROX_3 -39
#define NO_PROX_3mes \
  "can't do proximity op, set has no positional info, set=%s"
#define SET_NO_WEIGHT -40
#define SET_NO_WEIGHTmes \
  "can't do best match op, no weight given or implied, set=%s"
#define SILLY_LIMIT -41
#define SILLY_LIMITmes "limit out of range, limit=%s"
#define CANT_SET_LIMIT -42
#define CANT_SET_LIMITmes "can't set limit"
#define BAD_PASSAGE_PARMS -43
#define BAD_PASSAGE_PARMSmes \
 "passage parameters inconsistent, p_unit=%s, p_step=%s, p_maxlen=%s"
#define NO_SET_ASSIGNED -44
#define NO_SET_ASSIGNEDmes "no set assigned"
#define CANT_LIMIT_SET -45
#define CANT_LIMIT_SETmes "can't limit set, set=%s"
#define NO_SUCH_STEMFUNCTION -46
#define NO_SUCH_STEMFUNCTIONmes "unrecognized stem function, stemfunction=%s"
#define INT_ERR -47
#define INT_ERRmes "internal error, %s"
#define DEFECTIVE_INDEX -48
#define DEFECTIVE_INDEXmes "missing or defective index, attribute=%s"
#define ONLY_ONE_SET -49
#define ONLY_ONE_SETmes "can't do operation on more than one set, op=%s"
#define CANT_GET_DOCLENS -50
#define CANT_GET_DOCLENSmes "can't read document lengths, attribute=%s"
#define MORE_THAN_ONE_ATTRIBUTE -51
#define MORE_THAN_ONE_ATTRIBUTEmes \
"atomic sets must all be from same attribute, op=%s"
#define IMPURE_SET -52
#define IMPURE_SETmes "set must be from single attribute, set=%s, op=%s"
#define PHONEY_WEIGHT_ARGS -53
#define PHONEY_WEIGHT_ARGSmes \
  "peculiar arguments to weight function, func=%s"
#define CANT_SETUP_ATTRIBUTE -54
#define CANT_SETUP_ATTRIBUTEmes \
  "can't setup attribute, attribute=%s"
#define UNKNOWN_FIELD_TYPE -55
#define UNKNOWN_FIELD_TYPEmes \
  "unknown field type, type=%s"
#define CANT_READ_DOC -56
#define CANT_READ_DOCmes \
  "can't read document, set=%s, rec=%s, file=%s"
#define EMPTY_DB -57
#define EMPTY_DBmes "empty database, db=%s"
#define DB_NOT_OPEN -58
#define DB_NOT_OPENmes "database not open, db=%s"
#define PHONEY_BIGN_OR_N -59
#define PHONEY_BIGN_OR_Nmes \
  "incompatible arguments to weight command, bign=%s, n=%s"
#define TEMPUNSUPPORTEDCOMMAND -60
#define TEMPUNSUPPORTEDCOMMANDmes "command temporarily unavailable, command=%s"
#define NO_SUCH_REGIME -61
#define NO_SUCH_REGIMEmes "unknown extraction regime, name=%s"
#define INVALID_REGIME -62
#define INVALID_REGIMEmes "extract regime not currently valid, name=%s"
#define CANT_SETUP_GSL -63
#define CANT_SETUP_GSLmes "can't set up GSL, name=%s, stemfunction=%s"
#define NO_SUCH_DEBUG_NAME -64
#define NO_SUCH_DEBUG_NAMEmes "no debug code name, name=%s"
#define DB_NOPARM -65
#define DB_NOPARMmes "database open failed, no parameter found, db=%s"
#define PHONEY_RSCORE -66
#define PHONEY_RSCOREmes \
  "phoney arguments to rscore, R=%s, r=%s, N=%s, n=%s"
#define DOCLENS_MIXED_ATTRIBUTE -67
#define DOCLENS_MIXED_ATTRIBUTEmes \
 "can't get doclens, more than one attribute in input"
#define NO_RECORD_ASSIGNED -68
#define NO_RECORD_ASSIGNEDmes "no record assigned"
#define NO_SUCH_FIELDNUM -69
#define NO_SUCH_FIELDNUMmes "invalid field name or number, db=%s"
#define DEFECTIVE_PASSAGES -70
#define DEFECTIVE_PASSAGESmes "can't set up paragraph information"
#define NO_SUPERPARSE -71
#define NO_SUPERPARSEmes "superparse not supported for this extraction regime"
#define NO_INDEX_WEIGHT -72
#define NO_INDEX_WEIGHTmes "can't do this op, no posting weights, set=%s"
#define NUM_ERRORS 72

#define TERM_NO_POSTINGS 1
#define TERM_NO_POSTINGSmes "term not found, term=%s"
#define SET_NO_POSTINGS 2
#define SET_NO_POSTINGSmes "empty set, set=%d"
#define SET_NO_WEIGHT_WARNING 3
#define SET_NO_WEIGHT_WARNINGmes "no weight given or implied, set=%s"
#define EMPTY_INDEX 4
#define EMPTY_INDEXmes \
  "index for this attribute seems to be empty or not searchable, attr=%s"
#define SYS_WARNING 5
#define SYS_WARNINGmes "system warning, %s"
#define NO_TF_WARNING 6
#define NO_TF_WARNINGmes \
 "incomplete or absent term frequency information"
#define NO_FIELD_TYPE_FILE 7
#define NO_FIELD_TYPE_FILEmes \
 "can't read a field type file"
#define NUM_WARNINGS 7
