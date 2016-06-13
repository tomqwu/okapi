#ifndef _i0_defs_h
#define _i0_defs_h

/* Defines for the interface */

#define FLEX

#include "bss.h"
#include "bss_errors.h"
#include "bss_defaults.h"

/* Some "code" values (June 97 only for ds_trec) */

#ifndef I0_OUTBUFSIZE
#define I0_OUTBUFSIZE 8 * 1024 * 1024 /* but could be defined in site.h */
#endif

#define DEFAULT_LMASK 0
#define DEFAULT_TERM ""
#define DEFAULT_DB NULL
#define DEFAULT_DEBUG 0
#define DEFAULT_VERBOSE 0
#define DEFAULT_ATTR "default"
#define DEFAULT_QATTR "default"
#define DEFAULT_CLASSES " SGIHFP" /* i.e. all classes */
#define DEFAULT_SOURCES TRUE
#define DEFAULT_FFL FFL_FILE
#define DEFAULT_SAVE 1
#define DEFAULT_SEARCH_TYPE 0
#define DEFAULT_OP OP_BM1
#define DEFAULT_MAXGAP 0
#define DEFAULT_AW MINWEIGHT
#define DEFAULT_GW 0
#define DEFAULT_TARGET 0
#define DEFAULT_NOPOS FALSE
#define DEFAULT_K1 NONE_ASSIGNED /* See bss_do_combine() for defaults */
#define DEFAULT_K2 NONE_ASSIGNED
#define DEFAULT_K3 NONE_ASSIGNED
#define DEFAULT_K4 0.0
#define DEFAULT_K5 0.5
#define DEFAULT_K6 100000
#define DEFAULT_K7 0
#define DEFAULT_K8 0
#define DEFAULT_B NONE_ASSIGNED
#define DEFAULT_B3 NONE_ASSIGNED
#define DEFAULT_AVEDOCLEN 0
#define DEFAULT_AVEQUERYLEN 0
#define DEFAULT_QUERYLEN 0
#define DEFAULT_QTF 1
#define DEFAULT_P_UNIT 1
#define DEFAULT_P_STEP 1
#define DEFAULT_P_MAXLEN 20
#define DEFAULT_WFUNC 1
#define DEFAULT_WTFACTOR 1
#define DEFAULT_SCOREFUNC 0
#define DEFAULT_WNUMBER -400
#define DEFAULT_TTF 0
#define DEFAULT_R 0
#define DEFAULT_BIGR 0
#define DEFAULT_S 0
#define DEFAULT_BIGS 0
#define DEFAULT_RLOAD 0
#define DEFAULT_BIGRLOAD 0
#define DEFAULT_THETA 1
#define DEFAULT_LAMDA 0
#define DEFAULT_TNT 0
#define DEFAULT_TNP 0
#define DEFAULT_CTTF 0
#define DEFAULT_BIGN 0
#define DEFAULT_FORMAT 1
#define DEFAULT_SHOW_NUMBER 1
#define DEFAULT_SET NONE_ASSIGNED
#define DEFAULT_REC -1
#define DEFAULT_RECNUM NONE_ASSIGNED
#define DEFAULT_LASTREC -1
#define DEFAULT_EXTRACT_NUMBER 20
#define DEFAULT_EXTRACT_WEIGHT_FUNC 2
#define DEFAULT_EXTRACT_FLAGS 0
#define DEFAULT_DB_FLAGS 0
#define DEFAULT_MARK 1
#define DEFAULT_MEM 0
#define DEFAULT_WT_FRIG_FACTOR 1
#define DEFAULT_LASTWEIGHT 0
#define DEFAULT_LASTN 0
#define DEFAULT_LAST_TTF 0
#define DEFAULT_PREFER_LENGTH 0
#define DEFAULT_ACCEPT_LENGTH 0
#define DEFAULT_TS TS_STANDARD
#define DEFAULT_NOERROR FALSE

EXTERN unsigned _q_lmask, _q_index ;
EXTERN char _q_term[MAXEXITERMLEN + 2] ;
EXTERN char *_q_db ;
EXTERN char _q_attr[NNAME_MAX] ;
EXTERN char default_attr[NNAME_MAX] ;
EXTERN char _q_fdname[NNAME_MAX] ; /* June 01 */
EXTERN char _q_classes[NNAME_MAX] ;
EXTERN char _q_regname[NNAME_MAX] ;
EXTERN char _q_stemfuncname[NNAME_MAX] ;
EXTERN char _q_gslname[NNAME_MAX] ;
EXTERN char _q_startstr[NNAME_MAX] ; /* For bracketing doc text
					(formats 1,10,36) */
EXTERN char _q_finstr[NNAME_MAX] ; /* "" */
EXTERN BOOL _q_sources ;
EXTERN int _q_find_flags, default_ffl ;
EXTERN int _q_type, default_search_type ;
EXTERN int _q_target;
EXTERN int _q_op, default_op ;
EXTERN int _q_scorefunc, default_scorefunc ;
EXTERN double _q_k1, default_k1, _q_k2, default_k2, _q_k3, default_k3,
  _q_bm25_b, default_bm25_b, _q_b3, default_b3 ;
EXTERN double _q_k4, default_k4, _q_k5, default_k5, _q_k6, default_k6 ;
EXTERN double _q_k7, default_k7 ;
EXTERN double _q_k8, default_k8 ;
EXTERN int _q_avedoclen, default_avedoclen,
  _q_avequerylen, default_avequerylen, _q_querylen ;
EXTERN int _q_qtf ;
EXTERN int _q_p_unit, default_p_unit, _q_p_step, default_p_step ;
EXTERN int _q_p_maxlen, default_p_maxlen ;
EXTERN double _q_aw, _q_gw;
EXTERN int _q_maxgap ;
EXTERN double _frigvalue ;
EXTERN double _q_wt_frig_factor[MAXMERGESTREAMS + 1] ;
EXTERN double lastweight ;
EXTERN int lastn, last_ttf ;
EXTERN int _number_sets;
EXTERN int _q_set, _q_rec, _q_recnum, _q_lastrec, _q_format, default_format;
EXTERN int default_wfunc ;
EXTERN int _q_r, _q_bigr, default_bigr, _q_bign, default_bign ;
EXTERN int _q_s, _q_bigs, default_bigs ;
EXTERN int _q_rload, _q_bigrload, default_rload, default_bigrload ;
EXTERN int _q_theta, default_theta ;
EXTERN double _q_lamda, default_lamda ;
EXTERN int _q_tnt, default_tnt ;
#ifdef HAVE_LL
EXTERN LL_TYPE _q_tnp, default_tnp ;
EXTERN LL_TYPE _q_cttf, default_cttf ;
#else
EXTERN double _q_tnp, default_tnp ;
EXTERN double _q_cttf, default_cttf ;
#endif
EXTERN int _q_wnumber, _q_func, _q_shownumber;
EXTERN double _q_wtfactor;
EXTERN int _q_ttf ;
EXTERN int default_index, default_lmask ;
EXTERN int _limit_set;
EXTERN int _dump_set ;
EXTERN int _dump_code ;
EXTERN int _stat_set ;
EXTERN int _q_flags;
EXTERN int _q_sources;
EXTERN BOOL _r_was_set;
EXTERN int _q_db_flags ;
EXTERN int _q_mark ;
EXTERN int _q_number ;
EXTERN int _q_prefer_length, _q_accept_length ;
EXTERN int _q_field, _q_offset, _q_length ;
EXTERN int _q_code ;
EXTERN int _q_debug; 
EXTERN int _q_verbose; 
EXTERN BOOL _q_noerror;

#ifdef FLEX
EXTERN char *yytext ;
#else
EXTERN char yytext[]  ;
#endif
EXTERN int yydebug, yychar;

EXTERN int _arg_sets[MAXMERGESTREAMS + 1];
EXTERN double _arg_weights[MAXMERGESTREAMS + 1];
EXTERN int _arg_qtfs[MAXMERGESTREAMS + 1 ] ;
EXTERN char *_IN_BUF, *_OUT_BUF;
EXTERN char *bss_Outbuf ;

#endif /* _i0_defs_h */
