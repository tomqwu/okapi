#define EXTERN extern
#include "i0_defs.h"
#include "charclass.h"

void reinit(void)
{
  default_lmask = DEFAULT_LMASK ;
  strcpy(_q_term, DEFAULT_TERM) ;
  strrncpy(default_attr, DEFAULT_ATTR, NNAME_MAX) ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  default_search_type = DEFAULT_SEARCH_TYPE ;
  default_op = DEFAULT_OP ;
  if ( _q_db != NULL ) {
    bss_free(_q_db) ;
    _q_db = NULL ;
  }
  _q_aw = DEFAULT_AW ;
  _q_gw = DEFAULT_GW ;
  _q_maxgap = DEFAULT_MAXGAP ;
  _q_target = DEFAULT_TARGET ;
  _q_find_flags = default_ffl = DEFAULT_FFL ;
  _q_set = DEFAULT_SET ;
  _q_lastrec =  DEFAULT_LASTREC ;
  _q_rec = DEFAULT_REC ;
  _q_recnum = DEFAULT_RECNUM ;
  default_format = DEFAULT_FORMAT ;
  default_k1 = DEFAULT_K1 ; default_k2 = DEFAULT_K2 ; default_k3 = DEFAULT_K3 ;
  default_k4 = DEFAULT_K4 ; default_k5 = DEFAULT_K5 ; default_k6 = DEFAULT_K6 ;
  default_k7 = DEFAULT_K7 ;
  default_k8 = DEFAULT_K8 ;
  default_bm25_b = DEFAULT_B ;
  default_b3 = DEFAULT_B3 ;
  default_avedoclen = DEFAULT_AVEDOCLEN ;
  default_avequerylen = DEFAULT_AVEQUERYLEN ;
  _q_qtf = DEFAULT_QTF ;
  default_p_unit = DEFAULT_P_UNIT ; default_p_step = DEFAULT_P_STEP ;
  default_p_maxlen = DEFAULT_P_MAXLEN ;
  default_wfunc = DEFAULT_WFUNC ;
  default_scorefunc = DEFAULT_SCOREFUNC ;
  _q_wnumber = DEFAULT_WNUMBER ;
  _q_ttf = DEFAULT_TTF ;
  _q_wtfactor = DEFAULT_WTFACTOR ;
  _q_r = DEFAULT_R ;
  default_bigr = DEFAULT_BIGR ;
  _q_s = DEFAULT_S ;
  default_bigs = DEFAULT_BIGS ;
  default_rload = DEFAULT_RLOAD ;
  default_bigrload = DEFAULT_BIGRLOAD ;
  default_theta = DEFAULT_THETA ;
  default_tnp = DEFAULT_TNP ;
  default_tnt = DEFAULT_TNT ;
  default_cttf = DEFAULT_CTTF ;
  default_lamda = DEFAULT_LAMDA ;
  default_bign = DEFAULT_BIGN ;
  _q_shownumber = DEFAULT_SHOW_NUMBER ;
  _q_db_flags = DEFAULT_DB_FLAGS ;
  lastweight = DEFAULT_LASTWEIGHT ;
  lastn = DEFAULT_LASTN ;
  last_ttf = DEFAULT_LAST_TTF ;
  _q_code = DEFAULT_TS ;
  _q_noerror = DEFAULT_NOERROR ;
}

int
i0(char *command, char *result)
{
  int value, length;
  char *p ;
  //fprintf(stderr, "command=%s", command); //mko
  _IN_BUF = command;
  /* _OUT_BUF = result; */
  _OUT_BUF = bss_Outbuf ;
  length = strlen(command) ;
  fprintf(stderr, "  sect1", command);
  while ( length > 0 && tok_space(*(p = command + length - 1)) ) {
    *p-- = '\0' ; length-- ;
  }
  fprintf(stderr, "  sect2", command);
  /* *result = '\0'; */
  *bss_Outbuf = '\0' ;
  bss_Result = 0 ;
  bss_Outlength = 0 ;
  fprintf(stderr, "  sect3", command);
 value = yyparse();
  fprintf(stderr, "  sect4", command);
  bss_Outlength = strlen(bss_Outbuf) ;
  fprintf(stderr, "  sect5", command);
  /* fprintf(stderr, "OL=%d\n", bss_Outlength) ;
     fprintf(stderr, "Res=%d\n", bss_Result) ; */
  memcpy(result, bss_Outbuf, bss_Outlength + 1) ;
  fprintf(stderr, "  sect6", command);
  return bss_Result ;
}

int
i00(char *command, char **result)
{
  int value, length;
  char *p ;

  _IN_BUF = command;
  _OUT_BUF = bss_Outbuf ;
  length=strlen(command);
  // Remove trailing blanks from command
  while ( length > 0 && tok_space(*(p = command + length - 1)) ) {
    *p-- = '\0' ; length-- ;
  }
  *bss_Outbuf = '\0' ;
  bss_Result = 0 ;
  bss_Outlength = 0 ;
  //fprintf(stderr, "  command=%s,bssOutbuf=%s\n", command,bss_Outbuf);//mko
  value = yyparse();
  bss_Outlength = strlen(bss_Outbuf) ;
  *result = bss_Outbuf ;
  return bss_Result ;
}



void iinit(void)
{
  _q_verbose = DEFAULT_VERBOSE ;
  _q_debug = DEFAULT_DEBUG ;
  _q_prefer_length = DEFAULT_PREFER_LENGTH ;
  _q_accept_length = DEFAULT_ACCEPT_LENGTH ;
  bss_Outbuf = (char *)bss_malloc(I0_OUTBUFSIZE) ;
  bss_startup();
  reinit() ;
}


void
iexit(void)
{
  bss_free(bss_Outbuf) ;
  bss_exit();
}


