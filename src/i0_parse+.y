%{
#define EXTERN
#include "i0_defs.h"
static int _wvalue, _tvalue, _inumber ;
static int _t_set ;
static double _dnumber ;
static char _word[512] ;

static void do_limit(void) ;
static void clear(void) ;
static void i0_do_find(void) ;
static void i0_do_combine(void) ;
static void do_display_limits(void) ;
static void do_parse(void) ;
static void do_parse_rgf(void) ;
static void do_superparse(void) ;
static void do_superparse_rgf(void) ;
static void gsl_lookup(void) ;
static void _finish_show(void) ;
static int get_parse_buf(int) ;
%}
%pure_parser

%token ACCEPTTOKEN
%token ATOKEN
%token ATTSTOKEN
%token AVEDOCLEN_TOKEN
%token AVEQUERYLEN_TOKEN
%token AWTOKEN
%token BIGNTOKEN
%token BIGRLOADTOKEN
%token BIGRTOKEN
%token BIGSTOKEN
%token BM25_B_TOKEN
%token B3_TOKEN
%token CHOOSETOKEN  
%token CLASSTOKEN
%token CODETOKEN
%token COMBTOKEN 
%token TNT_TOKEN
%token CTTF_TOKEN
%token DBFLAGSTOKEN
%token DBSTOKEN
%token DBTOKEN
%token DEBUGTOKEN
%token DELETETOKEN
%token DELTOKEN
%token DISPLAYTOKEN
%token DISPLAYTRECSTATSTOKEN
%token DOCLENGTHTOKEN
%token DOLLAR
%token DUMPTOKEN
%token EQUALS
%token ERRORTOKEN		/* Don't think this is used (May 01) */
%token NOERRORTOKEN
%token EXTRACTTOKEN
%token FIELDTOKEN
%token FILETOKEN
%token FINDLIMITTOKEN
%token FINDTOKEN 
%token FINSTRTOKEN
%token FLAGSTOKEN 
%token FRIGTOKEN
%token FTOKEN
%token FUNCTOKEN
%token GAPTOKEN
%token GSLSTOKEN
%token GSLTOKEN
%token GWTOKEN
%token HELPTOKEN
%token HIGHLIGHTTOKEN
%token HOLDTOKEN
%token INFOTOKEN
%token IRNTOKEN
%token K1TOKEN
%token K2TOKEN
%token K3TOKEN
%token K4TOKEN
%token K5TOKEN
%token K6TOKEN
%token K7TOKEN
%token K8TOKEN
%token KEEPTOKEN
%token LAMDATOKEN
%token LENGTHTOKEN
%token LIMITTOKEN
%token LIMITSTOKEN
%token MARKTOKEN
%token MEMTOKEN
%token MODETOKEN
%token NOPOSTOKEN
%token NOSHOW_TOKEN
%token NOTFTOKEN
%token NO_ST_TOKEN
%token NTOKEN
%token NUMBER
%token OFFSETTOKEN
%token OPENTOKEN
%token OPTIONSTOKEN
%token OPTOKEN
%token PARASTOKEN
%token PARATOKEN
%token PARSETOKEN 
%token PERRORTOKEN
%token PREFERTOKEN
%token P_MAXLEN_TOKEN
%token P_STEP_TOKEN
%token P_UNIT_TOKEN
%token QETOKEN
%token QTFTOKEN
%token QUERYLEN_TOKEN
%token REAL
%token REGIMESTOKEN
%token REGTOKEN
%token REPORTTOKEN
%token RLOADTOKEN
%token RSCORETOKEN 
%token RTOKEN
%token SAVETOKEN
%token SETTOKEN
%token SCOREFUNCTOKEN
%token SHOWTOKEN
%token SRCTOKEN
%token STARTSTRTOKEN
%token STATS_TOKEN
%token STATS_TOKEN
%token STEMFUNCTIONSTOKEN
%token STEMFUNCTIONTOKEN
%token STEMTOKEN
%token STOKEN
%token ST_TOKEN
%token SUPERPARSETOKEN
%token TARGETTOKEN
%token TERM
%token TFTOKEN
%token THETATOKEN
%token TNPTOKEN
%token TOPTOKEN
%token TRECTOKEN
%token TTF_TOKEN
%token TYPETOKEN
%token VERBOSETOKEN
%token VERSIONTOKEN
%token WEIGHTTOKEN 
%token AWORD
%token WTFUNCTOKEN
%token WTOKEN
%token WTFACTORTOKEN

%token UNKNOWNCOMMAND
%token ENDOFQUERY

%start request

%%

request : command ENDOFQUERY 	
{
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got command ENDOFQUERY\n") ;
  clear(); YYACCEPT;
}
	| ENDOFQUERY	    
{
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got ENDOFQUERY\n") ;
  clear(); YYACCEPT;
}
	|		    
{
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got nothing\n");
  clear(); YYACCEPT;
}
/*	| error	   
{
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got error\n") ;
  clear(); YYACCEPT;
}*/
	| error	ENDOFQUERY
{
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got error ENDOFQUERY\n") ;
  clear(); YYACCEPT;
}
	;

command : choose
	| find
        | combine
        | stem
	| delete
	| info
	| show
        | weight
        | rscore
	| parse
        | superparse
        | extract
	| limit
	| gsl
	| findlimit
	| settoken
        | debug
	| tf
	| doclength
        | perror
        | dump
        | report
        | display
        | displaytrecstats
        | unknowncommand
	;


unknowncommand : UNKNOWNCOMMAND
{
  yyerror("unknown command", yytext) ;
}

word   : AWORD
{
  strcpy(_word, yytext) ;
}
;

inumber : NUMBER 
{
  _inumber = atoi(yytext) ;
}
;

dnumber : NUMBER 
{
  _dnumber = atof(yytext) ;
}
;

setnum  : STOKEN EQUALS NUMBER
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got setnum\n") ;
  _t_set = atoi(yytext) ;
}
;

rnum             : RTOKEN EQUALS inumber
;

irn              : IRNTOKEN EQUALS inumber
;

frignum          : FRIGTOKEN EQUALS dnumber
{
  _frigvalue = _dnumber ;
}
;

attr             : ATOKEN EQUALS AWORD
{
  strrncpy(_q_attr, yytext, NNAME_MAX) ;
}
;


lmask            : LIMITTOKEN EQUALS inumber
{
  _q_lmask = _inumber ;
}
;

choose : CHOOSETOKEN 
{
  _q_db_flags = DEFAULT_DB_FLAGS ;
} 

        chooseargs dbname
{
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got CHOOSETOKEN chooseargs dbname\n") ;
}
        ENDOFQUERY
{

  if (bss_open_db(_word, _q_db_flags) == 0) {
    reinit() ;
  }
}
;

chooseargs      :
                | DBFLAGSTOKEN EQUALS inumber
{
  _q_db_flags = _inumber ;
}
;

dbname          : word
                | DBTOKEN EQUALS word
;

find            : lkupcmd ENDOFQUERY         
{
  strrncpy(_q_term, _word, MAXEXITERMLEN - 1) ;
  /* May need truncation warning here */
  i0_do_find() ;
}
                | combcmd ENDOFQUERY
{
  printf ("doing combining..\n");
  i0_do_combine() ;
}
                | ftoken irn ENDOFQUERY
{
  int result = make_set(-1, _inumber) ;
  if ( result >= 0 ) {
    _q_set = result ;
    _q_lastrec = -1 ;
    sprintf(_OUT_BUF, "S%d np=1\n", result) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}

                | ftoken setnum rnum ENDOFQUERY
{
  int result = make_set(_t_set, _inumber) ;
  if ( result >= 0 ) {
    _q_set = result ;
    _q_lastrec = -1 ;
    sprintf(_OUT_BUF, "S%d np=1\n", result) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
                | ftoken rnum setnum ENDOFQUERY
{
  int result = make_set(_t_set, _inumber) ;
  if ( result >= 0 ) {
    _q_set = result ;
    _q_lastrec = -1 ;
    sprintf(_OUT_BUF, "S%d np=1\n", result) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
;

/* combine is the same as find in its primary combine mode (added March 99) */
combine         : COMBTOKEN 
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got COMBTOKEN\n") ;
  /* NB this initialization is almost same as for ftoken */
  _q_find_flags = default_ffl ;
  _q_op = default_op ;
  _q_scorefunc = default_scorefunc ;
  _q_maxgap = DEFAULT_MAXGAP ;
  _q_aw = DEFAULT_AW ;
  _q_gw = DEFAULT_GW ;
  _q_k1 = default_k1 ; _q_k2 = default_k2 ;
  _q_k3 = default_k3 ;
  _q_k7 = default_k7 ;
  _q_k8 = default_k8 ;
  _q_avedoclen = default_avedoclen ;
  _q_avequerylen = default_avequerylen ;
  _q_querylen = DEFAULT_QUERYLEN ;
  _q_bm25_b = default_bm25_b ;
  _q_b3 = default_b3 ;
  _q_p_unit = default_p_unit ; _q_p_step = default_p_step ;
  _q_p_maxlen = default_p_maxlen ;
  _q_target = DEFAULT_TARGET ;
  _q_lmask = default_lmask ;
  _q_mark = DEFAULT_MARK ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  _number_sets = 0 ;
}
                   sets options ENDOFQUERY
{
  i0_do_combine() ;
}
;

lkupcmd		: ftoken qualifiers TERM
{ 
  if ( Dbg & D_PARSER ) fprintf(bss_syslog,
				"Prsr: got ftoken qualifiers TERM\n") ;
  strcpy(_word, yytext) ;
}
                | ftoken qualifiers
{
  /* No term given, use current _q_term */
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got ftoken qualifiers\n") ;
  strcpy(_word, _q_term) ;
}
;

combcmd                : ftoken sets options
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got ftoken sets options\n") ;
}
                | ftoken set TOPTOKEN EQUALS inumber
{
  _q_target = _inumber ;		/* Might as well use _q_target */
  _q_op = OP_TOPN ;
}
;	

ftoken          : FINDTOKEN
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got FINDTOKEN\n") ;
  /* NB this initialization is almost same as for combine */
  _q_find_flags = default_ffl ;
  _q_type = default_search_type ;
  _q_op = default_op ;
  _q_scorefunc = default_scorefunc ;
  _q_maxgap = DEFAULT_MAXGAP ;
  _q_aw = DEFAULT_AW ;
  _q_gw = DEFAULT_GW ;
  _q_k1 = default_k1 ; _q_k2 = default_k2 ;
  _q_k3 = default_k3 ;
  _q_k7 = default_k7 ;
  _q_k8 = default_k8 ;
  _q_avedoclen = default_avedoclen ;
  _q_avequerylen = default_avequerylen ;
  _q_querylen = DEFAULT_QUERYLEN ;
  _q_bm25_b = default_bm25_b ;
  _q_b3 = default_b3 ;
   _q_p_unit = default_p_unit ; _q_p_step = default_p_step ;
  _q_p_maxlen = default_p_maxlen ;
  _q_target = DEFAULT_TARGET ;
  _q_lmask = default_lmask ;
  _q_mark = DEFAULT_MARK ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  _number_sets = 0 ;
  /*  _q_wt_frig_factor = DEFAULT_WT_FRIG_FACTOR ; Now an array */
}
;

sets		: set
		| sets set
		;

set		: setnum WTOKEN EQUALS dnumber
{
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = _dnumber ;
  _arg_qtfs[_number_sets] = 0 ;
  _number_sets++;
}

                | setnum WTOKEN EQUALS DOLLAR
{
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = lastweight ;
  _arg_qtfs[_number_sets] = 0 ;
  _number_sets++;
}

		| setnum
{
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = WEIGHT_NOT_ASSIGNED ;
  _arg_qtfs[_number_sets] = 0 ;
  _q_wt_frig_factor[_number_sets] = DEFAULT_WT_FRIG_FACTOR ;
  _number_sets++;
}
                | setnum frignum
{
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = WEIGHT_NOT_ASSIGNED ;
  _arg_qtfs[_number_sets] = 0 ;
  _q_wt_frig_factor[_number_sets] = _frigvalue ;
  _number_sets++;
}
                | set QTFTOKEN EQUALS inumber
{
  _arg_qtfs[_number_sets - 1] = _inumber ;
  /* already been incremented */
}
;

options		: 
                | options option
		;

option		: OPTOKEN EQUALS word
{ 
  if (!strcasecmp(_word,"bm1")) _q_op=OP_BM1;
  else if (!strcasecmp(_word,"bm2")) _q_op=OP_BM2;
  else if (!strcasecmp(_word,"bm11")) _q_op=OP_BM11;
  else if (!strcasecmp(_word,"bm15")) _q_op=OP_BM15;
  else if (!strcasecmp(_word,"bm25")) _q_op=OP_BM25;
  else if (!strcasecmp(_word,"bm25r")) _q_op=OP_BM25R;
  else if (!strcasecmp(_word,"bm25s")) _q_op=OP_BM25S;
  else if (!strcasecmp(_word,"bm25w")) _q_op=OP_BM2;
  else if (!strcasecmp(_word,"bm26")) _q_op=OP_BM26;
  else if (!strcasecmp(_word,"bm250")) _q_op=OP_BM250;
  else if (!strcasecmp(_word,"bm251")) _q_op=OP_BM251;
  else if (!strcasecmp(_word, "bm1100")) _q_op=OP_BM1100;
  else if (!strcasecmp(_word, "bm1500")) _q_op=OP_BM1500;
  else if (!strcasecmp(_word, "bm2500")) _q_op = OP_BM25 ;
  else if (!strcasecmp(_word, "bm2600")) _q_op = OP_BM26 ;
  else if (!strcasecmp(_word, "bm30")) _q_op = OP_BM30 ;
  else if (!strcasecmp(_word, "bm3000")) _q_op = OP_BM3000 ;
  else if (!strcasecmp(_word, "bm40")) _q_op = OP_BM40 ;
  else if (!strcasecmp(_word, "bm41")) _q_op = OP_BM41 ;
  else if (!strcasecmp(_word, "bm42")) _q_op = OP_BM42 ;
  else if (!strcasecmp(_word,"and")) _q_op=OP_AND1;
  else if (!strcasecmp(_word,"and1")) _q_op=OP_AND1;
  else if (!strcasecmp(_word,"and2")) _q_op=OP_AND2;
  else if (!strcasecmp(_word,"mark")) _q_op=OP_MARK;
  else if (!strcasecmp(_word,"and3")) _q_op=OP_MARK;
  else if (!strcasecmp(_word,"or")) _q_op=OP_OR1;
  else if (!strcasecmp(_word,"or1")) _q_op=OP_OR1;
  else if (!strcasecmp(_word,"not")) _q_op=OP_NOT1;
  else if (!strcasecmp(_word,"not1")) _q_op=OP_NOT1;
  else if (!strcasecmp(_word,"not2")) _q_op=OP_NOT2;
  else if (!strcasecmp(_word,"not3")) _q_op=OP_NOT3;
  else if (!strcasecmp(_word,"adj")) _q_op=OP_ADJ1;
  else if (!strcasecmp(_word,"adj1")) _q_op=OP_ADJ1;
  else if (!strcasecmp(_word,"adj2")) _q_op=OP_ADJ2;
  else if (!strcasecmp(_word,"sames")) _q_op=OP_SAMES1;
  else if (!strcasecmp(_word,"samef")) _q_op=OP_SAMEF1;
  else if (!strcasecmp(_word,"near")) _q_op=OP_NEAR1;
  else if (!strcasecmp(_word,"near1")) _q_op=OP_NEAR1;
  else if (!strcasecmp(_word,"copy")) _q_op=OP_COPY;
  else  _q_op = OP_NOOP ;
}
		| SCOREFUNCTOKEN EQUALS inumber 
{
  _q_scorefunc = _inumber ;
}
		| GAPTOKEN EQUALS inumber 
{
  _q_maxgap = _inumber ;
}


		| AWTOKEN EQUALS dnumber 
{
  _q_aw = _dnumber ;
}

		| GWTOKEN EQUALS dnumber
{
  _q_gw = _dnumber ;
}
               | TARGETTOKEN EQUALS inumber
{
  _q_target = _inumber ;
}

                       | SAVETOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags &= ~FFL_NOSAVE ;
  else _q_find_flags |= FFL_NOSAVE ;
}

                       | MEMTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_MEM ;
  else _q_find_flags &= ~FFL_MEM ;
}
                       | HOLDTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_HOLD_MEM ;
  else _q_find_flags &= ~FFL_HOLD_MEM ;
}

                       | FILETOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_FILE ;
  else _q_find_flags &= ~FFL_FILE ;
}
                       | NOPOSTOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= FFL_NOPOS ;
  else _q_find_flags &= ~(FFL_NOPOS|FFL_NOTF) ;
} 
                       | NOTFTOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= (FFL_NOTF|FFL_NOPOS) ;
  else _q_find_flags &= ~FFL_NOTF ;
} 
                       | OPENTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_OPEN ;
  else _q_find_flags &= ~FFL_OPEN ;
}

                       | KEEPTOKEN EQUALS tnumber
{ 
  if ( _tvalue ) _q_find_flags |= FFL_KEEP ;
  else _q_find_flags &= ~FFL_KEEP ;
}

                       | DELTOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= FFL_DELETE ;
  else _q_find_flags &= ~FFL_DELETE ;
}
                       | NO_ST_TOKEN EQUALS tnumber
{
  if ( _tvalue ) {
    _q_find_flags |= FFL_NO_SELECTION_TREE ;
    _q_find_flags &= ~FFL_SELECTION_TREE ;
  }
  else _q_find_flags &= ~FFL_NO_SELECTION_TREE ;
}
                       | ST_TOKEN EQUALS tnumber
{
  if ( _tvalue ) {
    _q_find_flags |= FFL_SELECTION_TREE ;
    _q_find_flags &= ~FFL_NO_SELECTION_TREE ;
  }
  else _q_find_flags &= ~FFL_SELECTION_TREE ;
}
                       | STATS_TOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= FFL_DO_STATS ;
  else _q_find_flags &= ~FFL_DO_STATS ;
}
                       | NOSHOW_TOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= FFL_NOSHOW ;
  else _q_find_flags &= ~FFL_NOSHOW ;
}
                       | MODETOKEN EQUALS word
{
  if ( strcasecmp(_word, "quick") == 0 ) _q_find_flags |= FFL_QUICK_COMBINE ;
  else _q_find_flags &= ~FFL_QUICK_COMBINE ;
}
                       | lmask

                       | K1TOKEN EQUALS dnumber
{
  _q_k1 = _dnumber ;
}

                       | K2TOKEN EQUALS dnumber 
{
  _q_k2 = _dnumber ;
}
                       | K3TOKEN EQUALS dnumber 
{
  _q_k3 = _dnumber ;
}
                       | K7TOKEN EQUALS dnumber 
{
  _q_k7 = _dnumber ;
}
                       | K8TOKEN EQUALS dnumber 
{
  _q_k8 = _dnumber ;
}
                       | BM25_B_TOKEN EQUALS dnumber
{
  _q_bm25_b = _dnumber ;
}
                       | B3_TOKEN EQUALS dnumber
{
  _q_b3 = _dnumber ;
}
                       | AVEDOCLEN_TOKEN EQUALS inumber
{
  _q_avedoclen = _inumber ;
}
                       | QUERYLEN_TOKEN EQUALS inumber
{
  _q_querylen = _inumber ;
}
                       | AVEQUERYLEN_TOKEN EQUALS inumber
{
  _q_avequerylen = _inumber ;
}
                       | P_UNIT_TOKEN EQUALS inumber
{
  _q_p_unit = _inumber ;
}
                       | P_STEP_TOKEN EQUALS inumber
{
  if ( _inumber >= 1 ) _q_p_step = _inumber ;
}
                       | P_MAXLEN_TOKEN EQUALS inumber
{
  _q_p_maxlen = _inumber ;
}
                       | MARKTOKEN EQUALS inumber
{
  /* Needs range check here */
  _q_mark = _inumber ;		/* Goes in top 8 bits of find_flags */
}

;


qualifiers 	: 
		| qualifiers qualifier
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got qualifiers\n") ;
}
		;

qualifier              : SAVETOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags &= ~FFL_NOSAVE ;
  else _q_find_flags |= FFL_NOSAVE ;
}

                       | MEMTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_MEM ;
  else _q_find_flags &= ~FFL_MEM ;
}
                       | HOLDTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_HOLD_MEM ;
  else _q_find_flags &= ~FFL_HOLD_MEM ;
}

                       | FILETOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_FILE ;
  else _q_find_flags &= ~FFL_FILE ;
}
                       | NOPOSTOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= FFL_NOPOS ;
  else _q_find_flags &= ~(FFL_NOPOS|FFL_NOTF) ;
} 
                       | NOTFTOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= (FFL_NOTF|FFL_NOPOS) ;
  else _q_find_flags &= ~FFL_NOTF ;
} 
                       | OPENTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_OPEN ;
  else _q_find_flags &= ~FFL_OPEN ;
}

                       | KEEPTOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags |= FFL_KEEP ;
  else _q_find_flags &= ~FFL_KEEP ;
}

                       | TYPETOKEN EQUALS inumber
{
  _q_type = _inumber ;
}
                       | lmask

                       | attr
;

stem            : STEMTOKEN stemfunc EQUALS word TERM
{
  int l ;
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog,
	    "Prsr: got STEMTOKEN STEMFUNCTIONTOKEN EQUALS AWORD TERM\n");
  sprintf(_OUT_BUF, "t=%s", _word) ;
  l = bss_stem_term(yytext, _OUT_BUF + 2, _OUT_BUF + 2) ;
}

;

stemfunc        : STEMFUNCTIONTOKEN
                | FUNCTOKEN

;

delete		: deltoken numbers ENDOFQUERY
{
  int j ;
  for ( j = 0 ; j < _number_sets ; j++ ) {
    bss_clear_setrec(_arg_sets[j]) ;
    if ( _q_set == _arg_sets[j] ) _q_set = DEFAULT_SET ;
  }
}

                | deltoken STOKEN EQUALS word ENDOFQUERY
{
  if ( strcasecmp(_word, "all") == 0 ) {
    bss_clear_all_setrecs() ;
    _q_set = DEFAULT_SET ;
  }
  else yyerror(_word) ;
}
                | deltoken word ENDOFQUERY
{
  if ( strcasecmp(_word, "all") == 0 ) {
    bss_clear_all_setrecs() ;
    _q_set = DEFAULT_SET ;
  }
  else yyerror(_word) ;
}


		| deltoken ENDOFQUERY
{
  bss_clear_setrec(_q_set) ;
  _q_set = DEFAULT_SET ;
}
;

deltoken        : DELETETOKEN
{
  _number_sets = 0 ;
}

numbers         : number
                | numbers number
;


number          : inumber
{
  _arg_sets[_number_sets++] = _inumber ;
}
                | setnum

{
  _arg_sets[_number_sets++] = _t_set ;
}
;

info            : INFOTOKEN irn ENDOFQUERY
{
  get_rec_info_text(_inumber, _OUT_BUF) ;
}
                | INFOTOKEN DBTOKEN ENDOFQUERY
{
  bss_describe_database(NULL, _OUT_BUF, 0) ;
}
                | INFOTOKEN DBTOKEN EQUALS word ENDOFQUERY
{
  bss_describe_database(_word, _OUT_BUF, 0) ;
}
                | INFOTOKEN DBSTOKEN ENDOFQUERY
{
  bss_describe_databases(_OUT_BUF, 0) ;
}
                | INFOTOKEN attr ENDOFQUERY
{
  (void)bss_describe_attribute(_q_attr, _OUT_BUF) ;
  /*prepare_error(TEMPUNSUPPORTEDCOMMAND, "info attribute=<name>", 0) ;*/
}
                | INFOTOKEN ATOKEN ENDOFQUERY
{
  prepare_error(TEMPUNSUPPORTEDCOMMAND, "info attribute", 0) ;
}
                | INFOTOKEN ATTSTOKEN ENDOFQUERY
{
  prepare_error(TEMPUNSUPPORTEDCOMMAND, "info attributes", 0) ;
}
                | INFOTOKEN STEMFUNCTIONSTOKEN
{
    bss_get_stemfunctions(_OUT_BUF) ;
}
                | INFOTOKEN REGIMESTOKEN
{
    bss_get_regimes(_OUT_BUF) ;
}
                | INFOTOKEN GSLSTOKEN
{
  prepare_error(TEMPUNSUPPORTEDCOMMAND, "info gsls", 0) ;
}
                | INFOTOKEN VERSIONTOKEN
{
    bss_get_version(_OUT_BUF) ;
}
                | INFOTOKEN LIMITSTOKEN
{
    do_display_limits() ;
}
                | INFOTOKEN word ENDOFQUERY
{
  if (!strcasecmp(_word,"sets"))
    prepare_error(UNSUPPORTEDCOMMAND, "info sets", 0) ;
  else {
    char buf[256] ;
    sprintf(buf, "info %s", _word) ;
    prepare_error(SYNTAX, "info", _word, 0) ;
  }
}
| INFOTOKEN ENDOFQUERY
{
  prepare_error(SYNTAX, "info,", "no subject", 0) ;
}
;


show		: SHOWTOKEN 
{
  _r_was_set = FALSE ;
  _q_format = default_format ;
  _q_shownumber = DEFAULT_SHOW_NUMBER ;
  _q_recnum = DEFAULT_RECNUM ;
  _q_noerror = DEFAULT_NOERROR ;
  *(_q_startstr) = *(_q_finstr) = '\0' ;
}
                  show_args ENDOFQUERY
{
  _finish_show() ;
}

;

show_args	: 
                | show_args show_arg
		;

show_arg	: setnum
{
  /* Noticed bug here 6 Dec 94: if r is given before s on the command line
     the given value of r will be overwritten when the s argument is
     processed, and r will be zero. Hence introduced a flag _r_was_set. */

  if ( ! _r_was_set && _q_set != _t_set ) {
    _q_rec = 0;
    _q_lastrec = -1 ;
  }
  _q_set = _t_set ;
}

                | rnum
{
  _q_rec = _inumber ;
  _r_was_set = TRUE ;
}

                | FTOKEN EQUALS inumber
{
  _q_format = _inumber ;
}

| NTOKEN EQUALS inumber /* deprecated */
{
  _q_shownumber = _inumber ;
}

                | irn
{
  _q_recnum = _inumber ;
}

                | NOERRORTOKEN EQUALS tnumber
{
  _q_noerror = _tvalue ;
}
                | STARTSTRTOKEN EQUALS word
{
  strrncpy(_q_startstr, _word, NNAME_MAX) ;
}

                | FINSTRTOKEN EQUALS word
{
  strrncpy(_q_finstr, _word, NNAME_MAX) ;
}

;


parse           : ptoken p1qualifiers pterm
{
  do_parse() ;
}
                | ptoken p2qualifiers pterm
{
  do_parse_rgf() ;
}
;

ptoken 		: PARSETOKEN 
{
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  *_q_regname = *_q_stemfuncname = *_q_gslname = '\0' ;
}
;

superparse      : sptoken p1qualifiers pterm
{
  do_superparse() ;
}
                | sptoken p2qualifiers pterm
{
  do_superparse_rgf() ;
}
;

sptoken 	: SUPERPARSETOKEN 
{
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  *_q_regname = *_q_stemfuncname = *_q_gslname = '\0' ;
}

pterm          :
               | TERM
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got pterm\n") ;
}
;

p1qualifiers	:
		| p1qualifiers p1qualifier
;

p2qualifiers	: p2qualifier 
                | p2qualifiers p2qualifier
;

p1qualifier 	:  attr
;

p2qualifier     : REGTOKEN EQUALS word
{
  strrncpy(_q_regname, _word, NNAME_MAX) ;
}
                | STEMFUNCTIONTOKEN EQUALS word
{
  strrncpy(_q_stemfuncname, _word, NNAME_MAX) ;
}
                | GSLTOKEN EQUALS word
{
  strrncpy(_q_gslname, _word, NNAME_MAX) ;
}
;

extract         : EXTRACTTOKEN 
{
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  _q_rec = _q_lastrec ;
  _q_field = 3 ;		/* Testing */
  _q_offset = 0 ;
  _q_length = 0 ;
  _q_sources = DEFAULT_SOURCES ; /* TRUE */
  strcpy(_q_classes, DEFAULT_CLASSES) ; /* ALL */
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "PRSR: got EXTRACTTOKEN\n") ;
}

                  extractargs ENDOFQUERY
{
  int num_terms, counter ;
  char *s ;
  char *buf = NULL ;
  int gsclass ;
  int count = 0 ;

  num_terms =
    bss_superparse_doc(_q_set, _q_rec, &Cdb, _q_field, _q_offset, _q_length,
		       _q_attr, &buf, _q_sources ) ;
  if ( num_terms >= 0 ) {
    s = buf ;
    if ( _q_sources ) {
      sprintf(_OUT_BUF, "%d\n", num_terms);
      _OUT_BUF += strlen(_OUT_BUF) ;
      for (counter = 0; counter < num_terms; counter++) {
	sprintf(_OUT_BUF, "t=%s ", s + 1);
	_OUT_BUF += strlen(_OUT_BUF) ;
	if ( *s != ' ' ) sprintf(_OUT_BUF, "c=%c ", *s) ;
	else sprintf(_OUT_BUF, "c=N ") ;
	_OUT_BUF += strlen(_OUT_BUF) ;
	s += strlen(s) + 1;
	sprintf(_OUT_BUF, "s=%s\n", s);
	_OUT_BUF += strlen(_OUT_BUF) ;
	s += strlen(s) + 1;
      }
    }
    else {
      for (counter = 0; counter < num_terms; counter++) {
	gsclass = *s ;
	if ( strchr(" SGI", gsclass) ) count++ ;
	s += strlen(s) + 1 ;
      }
      sprintf(_OUT_BUF, "%d\n", count);
      _OUT_BUF += strlen(_OUT_BUF) ;
      s = buf ;
      for (counter = 0; counter < num_terms; counter++) {
	gsclass = *s ;
	if ( gsclass == ' ' ) gsclass = 'N' ;
	if ( strchr("NSGI", gsclass) ) {
	  sprintf(_OUT_BUF, "%c%s\n", gsclass, s + 1);
	  _OUT_BUF += strlen(_OUT_BUF) ;
	}
	s += strlen(s) + 1;
      }
    }
  }
  if ( buf != NULL ) bss_free(buf) ;
}
;

extractargs     : 
                | extractargs extractarg
;

extractarg      : setnum
{
  _q_set = _t_set ;
}
                | irn
{
  _q_rec = _inumber ;
  _q_set = -1 ;
}
                | rnum
{
  _q_rec = _inumber ;
}
                | OFFSETTOKEN EQUALS inumber
{
  _q_offset = _inumber ;
}
                | LENGTHTOKEN EQUALS inumber
{
  _q_length = _inumber ;
}
                | FIELDTOKEN EQUALS inumber 
{
  _q_field = _inumber ;
}
                | FIELDTOKEN EQUALS word 
{
  _q_field = find_field_num(_word) ;
}
                | SRCTOKEN EQUALS tnumber
{
  _q_sources = _tvalue ;
}
                | attr

;

weight	: WEIGHTTOKEN
{
  /* _q_wnumber = DEFAULT_WNUMBER ; No. Initially unassigned, then result
     of last lookup. Makes some scripts easier. */
  _q_wnumber = lastn ;
  _q_ttf = last_ttf ;
  _q_lmask = default_lmask ;
  _q_func = default_wfunc ;
  _q_r = DEFAULT_R ;
  _q_bigr = default_bigr ;
  _q_rload = default_rload ;
  _q_bigrload = default_bigrload ;
  _q_bign = default_bign ;
  _q_s = DEFAULT_S ;
  _q_bigs = default_bigs ;
  _q_k3 = default_k3 ;
  _q_k4 = default_k4 ;
  _q_k5 = default_k5 ;
  _q_k6 = default_k6 ;
  _q_qtf = DEFAULT_QTF ;
  _q_theta = default_theta ;
  _q_lamda = default_lamda ;	/* For bm40/41 */
  _q_tnp = default_tnp ;	/* For bm40 */
  _q_cttf = default_cttf ;	/* For bm41 */
  _q_wtfactor = DEFAULT_WTFACTOR ; /* Nov 00 */
}

                      weightqualifiers ENDOFQUERY

{

  lastweight = bss_assign_weight_ln(_q_func, _q_lmask, _q_wnumber, _q_ttf,
				    _q_bigr, _q_r, _q_bigrload, _q_rload,
				    _q_bign, _q_bigs, _q_s, _q_qtf,
				    _q_k3, _q_k4, _q_k5, _q_k6,
				    _q_theta, _q_lamda, _q_tnp, _q_cttf,
				    _q_wtfactor ) ;
  sprintf(_OUT_BUF, "%.3f\n", lastweight) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}

;


weightqualifiers: 
		| weightqualifiers weightqualifier
		;


weightqualifier	: lmask

		| wfunc EQUALS inumber
{
  _q_func = _inumber ;
}
		| NTOKEN EQUALS inumber
{
  _q_wnumber = _inumber ;
}
		| TTF_TOKEN EQUALS inumber
{
  _q_ttf = _inumber ;
}
		| QTFTOKEN EQUALS inumber
{
  _q_qtf = _inumber ;
}
		| K3TOKEN EQUALS NUMBER
{
  _q_k3 = atof(yytext) ;
}
		| K4TOKEN EQUALS NUMBER
{
  _q_k4 = atof(yytext) ;
}
		| K5TOKEN EQUALS NUMBER
{
  _q_k5 = atof(yytext) ;
}
		| K6TOKEN EQUALS NUMBER
{
  _q_k6 = atof(yytext) ;
}

		| rnum
{
  _q_r = _inumber ;
}
		| STOKEN EQUALS inumber
{
  _q_s = _inumber ;
}

		| BIGRTOKEN EQUALS inumber
{
  _q_bigr = _inumber ;
}
		| BIGSTOKEN EQUALS inumber
{
  _q_bigs = _inumber ;
}
		| RLOADTOKEN EQUALS inumber
{
  _q_rload = _inumber ;
}

		| BIGRLOADTOKEN EQUALS inumber
{
  _q_bigrload = _inumber ;
}
		| BIGNTOKEN EQUALS inumber
{
  _q_bign = _inumber ;		/* Normally zero, then it uses Cdb.bign */
}
                | THETATOKEN EQUALS inumber
{
  _q_theta = _inumber ;		
}
                | LAMDATOKEN EQUALS NUMBER
{
  _q_lamda = atof(yytext) ;
}
                | TNPTOKEN EQUALS NUMBER
{
#ifdef LARGEFILES
  _q_tnp = atoll(yytext) ;
#else
  _q_tnp = atof(yytext) ;
#endif
}
                | CTTF_TOKEN EQUALS NUMBER
{
#ifdef LARGEFILES
  _q_cttf = atoll(yytext) ;
#else
  _q_cttf = atof(yytext) ;
#endif
}
                | WTFACTORTOKEN EQUALS NUMBER
{
  _q_wtfactor = atof(yytext) ;
}

;

wfunc           : WTFUNCTOKEN
                | FUNCTOKEN
;


/* Might want rscore to allow no Db open and thus need simply R, r, N, n, Cttf.
   Wd mean change to bss_get_rscore. Also might want to use function numbers
   to mean e.g. new rscore, old RSV etc. */

/* Aug 2000: NB rscore should use TNT not CTTF
   Temporarily naming corrected version newrscore.
   Sep 2000: newrsocre now rscore, rscore now oldrscore.
   oldrscore deleted July 02 */

rscore	: RSCORETOKEN
{
  _q_wnumber = lastn ;
  _q_lmask = default_lmask ;
  _q_r = DEFAULT_R ;
  _q_bigr = default_bigr ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
}

                      rscorequalifiers ENDOFQUERY

{

  double score ;
  score = bss_get_rscore(&Cdb, _q_attr, _q_lmask, _q_wnumber, _q_bigr, _q_r) ;
  sprintf(_OUT_BUF, "%.3f\n", score) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}

;

rscorequalifiers: 
		| rscorequalifiers rscorequalifier
		;


rscorequalifier	: lmask

                | attr 

		| NTOKEN EQUALS inumber
{
  _q_wnumber = _inumber ;
}
		| rnum
{
  _q_r = _inumber ;
}
		| BIGRTOKEN EQUALS inumber
{
  _q_bigr = _inumber ;
}
;

gsl		: GSLTOKEN 

{
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
}

                  p1qualifiers TERM
{
  gsl_lookup();
}

;


debug           : DEBUGTOKEN TERM
{
  int result = bss_calc_debug(yytext) ;
  if ( result > 0 ) {
    bss_set_debug(result) ;
    _q_debug = result ;
  }
}
                | DEBUGTOKEN inumber
{
  bss_set_debug(_inumber) ;
  _q_debug = _inumber ;
}
;

findlimit       : FINDLIMITTOKEN TERM
{
  int result;
      
  if (!strcasecmp(yytext, "none")) {
    sprintf(_OUT_BUF, "0\n");
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
  else {
    result = bss_findlimit(yytext);
    if ( result > 0 ) {
      sprintf(_OUT_BUF, "%d\n", result);
      _OUT_BUF += strlen(_OUT_BUF) ;
    }
  }
}

		| FINDLIMITTOKEN ENDOFQUERY
{
  yyerror("no limit name given") ;
}
;


settoken:         SETTOKEN 
{
  bss_Result = 0 ;
}
                  setargs

                | SETTOKEN word ENDOFQUERY
{
  bss_Result = 0 ;
  if ( strcasecmp(_word, "defaults") == 0 ) {
    reinit() ;
  }
  else yyerror(_word) ;
}

                | SETTOKEN
{
  bss_Result = 0 ;
  _q_find_flags = default_ffl ;

  sprintf(_OUT_BUF, "accept %d\n", _q_accept_length) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "attribute %s\n", default_attr) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_avedoclen == DEFAULT_AVEDOCLEN )
    sprintf(_OUT_BUF, "avedoclen unassigned\n") ;
  else sprintf(_OUT_BUF, "avedoclen %d\n", default_avedoclen) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_avequerylen == DEFAULT_AVEQUERYLEN )
    sprintf(_OUT_BUF, "avequerylen unassigned\n") ;
  else sprintf(_OUT_BUF, "avequerylen %d\n", default_avequerylen) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_bign == DEFAULT_BIGN )
    sprintf(_OUT_BUF, "big_n unassigned\n") ;
  else sprintf(_OUT_BUF, "big_n %d\n", default_bign) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "big_r %d\n", default_bigr) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "rload %d\n", default_rload) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "big_rload %d\n", default_bigrload) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "big_s %d\n", default_bigs) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "format %d\n", default_format) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_k1 == NONE_ASSIGNED )
    sprintf(_OUT_BUF, "k1 unassigned\n") ;
  else sprintf(_OUT_BUF, "k1 %.2f\n", default_k1) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_k2 == NONE_ASSIGNED )
    sprintf(_OUT_BUF, "k2 unassigned\n") ;
  else sprintf(_OUT_BUF, "k2 %.2f\n", default_k2) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "k3 %.2f\n", default_k3) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "k4 %.2f\n", default_k4) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "k5 %.2f\n", default_k5) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "k6 %.2f\n", default_k6) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "k7 %.2f\n", default_k7) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "k8 %.2f\n", default_k8) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_bm25_b == NONE_ASSIGNED )
    sprintf(_OUT_BUF, "bm25b unassigned\n") ;
  else sprintf(_OUT_BUF, "bm25b %.2f\n", default_bm25_b) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  if ( default_b3 == NONE_ASSIGNED )
    sprintf(_OUT_BUF, "b3 unassigned\n") ;
  else sprintf(_OUT_BUF, "b3 %.2f\n", default_b3) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "lamda %.2f\n", default_lamda) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "limit %d\n", default_lmask) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "mem %d\n", (_q_find_flags & FFL_MEM) != 0 ) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "nopos %d\n", (_q_find_flags & FFL_NOPOS) != 0) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "notf %d\n", (_q_find_flags & FFL_NOTF) != 0) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "force no_selection_tree %d\n",
	  (_q_find_flags & FFL_NO_SELECTION_TREE) != 0) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "force selection_tree %d\n",
	  (_q_find_flags & FFL_SELECTION_TREE) != 0) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "opcode %s\n", translate_opcode(default_op)) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "score_function %d\n", default_scorefunc) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "passage_unit %d\n", default_p_unit) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "passage_step %d\n", default_p_step) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "passage_maxlen %d\n", default_p_maxlen) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "prefer %d\n", _q_prefer_length) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "save_set %d\n", (_q_find_flags & FFL_NOSAVE) == 0 ) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "search_type %d\n", default_search_type) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "tnt %d\n", default_tnt) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
#ifdef HAVE_LL
#ifdef _WIN32
  sprintf(_OUT_BUF, "tnp %I64d\n", default_tnp) ;
#else
  sprintf(_OUT_BUF, "tnp %lld\n", default_tnp) ;
#endif
#else
  sprintf(_OUT_BUF, "tnp %.0f\n", default_tnp) ;
#endif
  _OUT_BUF += strlen(_OUT_BUF) ;
#ifdef HAVE_LL
#ifdef _WIN32
  sprintf(_OUT_BUF, "cttf %I64d\n", default_cttf) ;
#else
  sprintf(_OUT_BUF, "cttf %lld\n", default_cttf) ;
#endif
#else
  sprintf(_OUT_BUF, "cttf %.0f\n", default_cttf) ;
#endif
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "theta %d\n", default_theta) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "weight_function %d\n", default_wfunc) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "verbosity %d\n", _q_verbose) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
  sprintf(_OUT_BUF, "debug %d\n", _q_debug) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
;

setargs	: setarg
	| setargs setarg
	;


setarg	: lmask
{
  if ( bss_set_limit(_q_lmask) != (u_int)-1 ) default_lmask = _q_lmask ;
}
        | VERBOSETOKEN EQUALS NUMBER
{
  _q_verbose = atoi(yytext) ;
}
        | THETATOKEN EQUALS inumber
{
  default_theta = _inumber ;
}
        | LAMDATOKEN EQUALS NUMBER
{
  default_lamda = atof(yytext) ;
}
        | TNT_TOKEN EQUALS inumber
{
  default_tnt = _inumber ;
}
        | TNPTOKEN EQUALS NUMBER
{
#ifdef LARGEFILES
  default_tnp = atoll(yytext) ;
#else
  default_tnp = atof(yytext) ;
#endif
}
        | CTTF_TOKEN EQUALS NUMBER
{
#ifdef LARGEFILES
  default_cttf = atoll(yytext) ;
#else
  default_cttf = atof(yytext) ;
#endif
}
        | wfunc EQUALS inumber
{
  default_wfunc = _inumber ;
}
        | SCOREFUNCTOKEN EQUALS inumber
{
  default_scorefunc = _inumber ;
}
	| FTOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_format = atoi(yytext);
}

	| ATOKEN EQUALS AWORD
{
  Sg *result, *find_search_name() ;

    result = find_search_name(&Cdb, yytext) ;
    if ( result != NULL ) 
      strrncpy(default_attr, yytext, NNAME_MAX) ;
}

        | SAVETOKEN EQUALS tnumber
{ 
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl &= ~FFL_NOSAVE ;
    else default_ffl |= FFL_NOSAVE ;
  }
}

        | TYPETOKEN EQUALS wnumber
{
  if ( check_db_open(&Cdb) ) {
    if ( _wvalue >= -2 && _wvalue <= 2 ) default_search_type = _wvalue ;
  }
}
        | NOPOSTOKEN EQUALS tnumber
{
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl |= FFL_NOPOS ;
    else default_ffl &= ~(FFL_NOPOS|FFL_NOTF) ;
  }
}
                       | NOTFTOKEN EQUALS tnumber
{
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl |= (FFL_NOTF|FFL_NOPOS) ;
    else default_ffl &= ~FFL_NOTF ;
  }
} 

        | MEMTOKEN EQUALS tnumber
{
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl |= FFL_MEM ;
    else default_ffl &= ~FFL_MEM ;
  }
}
        | NO_ST_TOKEN EQUALS tnumber
{
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) {
      default_ffl |= FFL_NO_SELECTION_TREE ;
      default_ffl &= ~FFL_SELECTION_TREE ;
    }
    else default_ffl &= ~FFL_NO_SELECTION_TREE ;
  }
}
        | ST_TOKEN EQUALS tnumber
{
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) {
      default_ffl |= FFL_SELECTION_TREE ;
      default_ffl &= ~FFL_NO_SELECTION_TREE ;
    }
    else default_ffl &= ~FFL_SELECTION_TREE ;
  }
}
        | K1TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k1 = atof(yytext) ;
}

        | K2TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k2 = atof(yytext) ;
}
        | K3TOKEN EQUALS NUMBER
{
  default_k3 = atof(yytext) ;
}
        | K4TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k4 = atof(yytext) ;
}
        | K5TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k5 = atof(yytext) ;
}
        | K6TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k6 = atof(yytext) ;
}
        | K7TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k7 = atof(yytext) ;
}
        | K8TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_k8 = atof(yytext) ;
}

        | BM25_B_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_bm25_b = atof(yytext) ;
}
        | B3_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_b3 = atof(yytext) ;
}
        | AVEDOCLEN_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_avedoclen = atoi(yytext) ;
}
        | AVEQUERYLEN_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) ) default_avequerylen = atoi(yytext) ;
}

        | P_UNIT_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 1 ) default_p_unit = atoi(yytext) ;
}

        | P_STEP_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 1 ) default_p_step = atoi(yytext) ;
}

        | P_MAXLEN_TOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 1 ) default_p_maxlen = atoi(yytext) ;
}

        | BIGRTOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bigr = atoi(yytext) ;
}

        | BIGSTOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bigs = atoi(yytext) ;
}

        | BIGRLOADTOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bigrload = atoi(yytext) ;
}

        | RLOADTOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_rload = atoi(yytext) ;
}

        | BIGNTOKEN EQUALS NUMBER
{
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bign = atoi(yytext) ;
}

        | DEBUGTOKEN EQUALS inumber
{
  _q_debug = _inumber ;
  bss_set_debug(_q_debug) ;
}

        | PREFERTOKEN EQUALS NUMBER
{
  _q_prefer_length = atoi(yytext) ;
}

        | ACCEPTTOKEN EQUALS NUMBER
{
  _q_accept_length = atoi(yytext) ;
}

        | OPTOKEN EQUALS AWORD
{
  if ( check_db_open(&Cdb) ) {
    if (!strcasecmp(yytext,"bm1")) default_op=OP_BM1;
    else if (!strcasecmp(yytext,"bm2")) default_op=OP_BM2;
    else if (!strcasecmp(yytext,"bm11")) default_op=OP_BM11;
    else if (!strcasecmp(yytext,"bm15")) default_op=OP_BM15;
    else if (!strcasecmp(yytext,"bm25")) default_op=OP_BM25;
    else if (!strcasecmp(yytext,"bm25r")) default_op=OP_BM25R;
    else if (!strcasecmp(yytext,"bm25s")) default_op=OP_BM25S;
    else if (!strcasecmp(yytext,"bm25w")) default_op=OP_BM2;
    else if (!strcasecmp(yytext,"bm26")) default_op=OP_BM26;
    else if (!strcasecmp(yytext,"bm250")) default_op=OP_BM250;
    else if (!strcasecmp(yytext,"bm251")) default_op=OP_BM251;
    else if (!strcasecmp(yytext, "bm1100")) default_op=OP_BM1100;
    else if (!strcasecmp(yytext, "bm1500")) default_op=OP_BM1500;
    else if (!strcasecmp(yytext, "bm2500")) default_op=OP_BM25;
    else if (!strcasecmp(yytext, "bm2600")) default_op=OP_BM26;
    else if (!strcasecmp(yytext, "bm30")) default_op=OP_BM30;
    else if (!strcasecmp(yytext, "bm3000")) default_op=OP_BM3000;
    else if (!strcasecmp(yytext, "bm40")) default_op=OP_BM40;
    else if (!strcasecmp(yytext, "bm41")) default_op=OP_BM41;
    else if (!strcasecmp(yytext, "bm42")) default_op=OP_BM42;
    
    else if (!strcasecmp(yytext,"and")) default_op=OP_AND1;
    else if (!strcasecmp(yytext,"and1")) default_op=OP_AND1;
    else if (!strcasecmp(yytext,"and2")) default_op=OP_AND2;
    else if (!strcasecmp(yytext,"mark")) default_op=OP_MARK;
    else if (!strcasecmp(yytext,"and3")) default_op=OP_MARK;
    else if (!strcasecmp(yytext,"or")) default_op=OP_OR1;
    else if (!strcasecmp(yytext,"or1")) default_op=OP_OR1;
    else if (!strcasecmp(yytext,"not")) default_op=OP_NOT1;
    else if (!strcasecmp(yytext,"not1")) default_op=OP_NOT1;
    else if (!strcasecmp(yytext,"not2")) default_op=OP_NOT2;
    else if (!strcasecmp(yytext,"not3")) default_op=OP_NOT3;
    else if (!strcasecmp(yytext,"adj")) default_op=OP_ADJ1;
    else if (!strcasecmp(yytext,"adj1")) default_op=OP_ADJ1;
    else if (!strcasecmp(yytext,"adj2")) default_op=OP_ADJ2;
    else if (!strcasecmp(yytext,"sames")) default_op=OP_SAMES1;
    else if (!strcasecmp(yytext,"near")) default_op=OP_NEAR1;
    else if (!strcasecmp(yytext,"near1")) default_op=OP_NEAR1;
    else if (!strcasecmp(yytext,"samef")) default_op=OP_SAMEF1;
    else if (!strcasecmp(yytext,"copy")) default_op=OP_COPY;
    else  prepare_error(NO_SUCH_OP, 0) ;
  }
}

;

tnumber       : AWORD 
{
  _tvalue = ( strcasecmp(yytext, "T") == 0 || strcasecmp(yytext, "Y") == 0 ||
	      strcasecmp(yytext, "TRUE") == 0 || strcasecmp(yytext, "YES") == 0 ) ;
}
              | NUMBER
{
  _tvalue = ( atoi(yytext) != 0 ) ;
}
;

wnumber       : AWORD 
{
  _wvalue = 0 ;
}
              | NUMBER
{
  _wvalue = atoi(yytext) ;
}
;

limit	: LIMITTOKEN 
{
  _q_find_flags = default_ffl ;
  _q_lmask = default_lmask ;
  _limit_set = _q_set ;
}

                      limitargs
{
  do_limit() ;
}

;

limitargs       : 
                | limitargs limitarg

;

limitarg        : setnum
{
  _limit_set = _t_set ;
}
                | lmask

                | SAVETOKEN EQUALS tnumber 
{ 
  if ( _tvalue ) _q_find_flags &= ~FFL_NOSAVE ;
  else _q_find_flags |= FFL_NOSAVE ;
}

;

display         : DISPLAYTOKEN ENDOFQUERY 

{
  yyerror("nothing to display") ;
}

                | DISPLAYTOKEN 
{
  _q_lmask = default_lmask ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
}
                 dispqualifiers disparg

                | DISPLAYTOKEN STATS_TOKEN

{
  bss_display_setstats(_q_set, _OUT_BUF) ;
}

                | DISPLAYTOKEN setnum STATS_TOKEN
{
  bss_display_setstats(_t_set, _OUT_BUF) ;
}
                | DISPLAYTOKEN STATS_TOKEN setnum
{
  bss_display_setstats(_t_set, _OUT_BUF) ;
}

;

disparg         : BIGNTOKEN
{
  int N  ;
  if ( default_bign > 0 ) N = default_bign ;
  else N = bss_get_big_n(_q_lmask) ;
  if ( N >= 0 ) {
    sprintf(_OUT_BUF, "%d\n", N) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}

                | AVEDOCLEN_TOKEN
{
  int adl ;
#ifdef HAVE_LL
  LL_TYPE cttf ;
#else
  double cttf ;
#endif
  if ( default_avedoclen > 0 ) adl = default_avedoclen ;
  else adl = bss_get_avedoclen(_q_attr, _q_lmask, &cttf) ;
  if ( adl >= 0 ) {
    sprintf(_OUT_BUF, "%d\n", adl) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}

                | TNT_TOKEN
{
  int tnt ;
  tnt = bss_get_tnt(_q_attr, _q_lmask) ;
  if ( tnt >= 0 ) sprintf(_OUT_BUF, "%d\n", tnt) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
                | CTTF_TOKEN
{
  int adl ;
#ifdef HAVE_LL
  LL_TYPE cttf ;
#else
  double cttf ;
#endif
  adl = bss_get_avedoclen(_q_attr, _q_lmask, &cttf) ;
  if ( adl >= 0 ) {
#ifdef HAVE_LL
#ifdef _WIN32
    sprintf(_OUT_BUF, "%I64d\n", cttf) ;
#else
    sprintf(_OUT_BUF, "%lld\n", cttf) ;
#endif
#else
    sprintf(_OUT_BUF, "%.0f\n", cttf) ;
#endif
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}

                | DBTOKEN
{
    bss_get_db_name(_OUT_BUF) ;
}

                | WTOKEN
{
  sprintf(_OUT_BUF, "%.3f\n", lastweight) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}

                | NTOKEN
{
  sprintf(_OUT_BUF, "%d\n", lastn) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}

                | TTF_TOKEN
{
  sprintf(_OUT_BUF, "%d\n", last_ttf) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}

; 

dispqualifiers  :
                | dispqualifiers dispqualifier

;

dispqualifier   : lmask

                | attr
;

displaytrecstats : DISPLAYTRECSTATSTOKEN
{
  _stat_set = _q_set ;
  _q_mark = DEFAULT_MARK ;
  _q_bigr = default_bigr ;
  _q_find_flags = default_ffl ;
  _q_number = 0 ;
  _q_code = DEFAULT_TS ;
}  
                    stat_args ENDOFQUERY
{
  bss_do_trec_stats(_OUT_BUF, _stat_set, _q_mark, _q_bigr,
		    _q_find_flags, _q_number, _q_code) ;
}
;

stat_args       :
                | stat_args stat_arg
;

stat_arg        : setnum
{
  _stat_set = _t_set ;
}
                | MARKTOKEN EQUALS inumber
{
  _q_mark = _inumber ;
}
                | BIGRTOKEN EQUALS inumber
{
  _q_bigr = _inumber ;
}
                | NTOKEN EQUALS inumber
{
  _q_number = _inumber ;
}
                | MEMTOKEN EQUALS tnumber
{
  if ( _tvalue ) _q_find_flags |= FFL_MEM ;
  else _q_find_flags &=  ~FFL_MEM ;
}
                | CODETOKEN EQUALS inumber
{
  _q_code = _inumber ;
}
                | AWORD
{
  if ( strcasecmp(yytext, "full") == 0 || strcasecmp(yytext, "long") == 0 )
    _q_code = TS_LONG ;
  else if ( strcasecmp(yytext, "short") == 0 || strcasecmp(yytext, "brief") == 0 )
    _q_code = TS_SHORT ;
}
;

perror          : PERRORTOKEN ENDOFQUERY
{
  sprintf(_OUT_BUF, "%s\n", bss_perror()) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
;

tf		: TFTOKEN ENDOFQUERY
{
  prepare_error(UNSUPPORTEDCOMMAND, "tf", 0) ;
}

;

doclength        : DOCLENGTHTOKEN 
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "PRSR: got DOCLENGTHTOKEN\n") ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  _q_rec = _q_lastrec ;
}

                  doclengthargs ENDOFQUERY
{
  int doclen = bss_get_doclen(&Cdb, _q_attr, _q_set, _q_rec ) ;
  if ( doclen >= 0 ) {
    sprintf(_OUT_BUF, "%d\n", doclen);
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
;

doclengthargs   : 
                | doclengthargs doclengtharg
;

doclengtharg    : setnum
{
  _q_set = _t_set ;
}
                | irn
{
  _q_rec = _inumber ;
  _q_set = -1 ;
}
                | rnum
{
  _q_rec = _inumber ;
}
                | attr

;


dump            : DUMPTOKEN 
{
  _dump_code = D_ALL ;
  _dump_set = _q_set ;
}

                  dumpargs ENDOFQUERY
{
  bss_dump_setrec(_dump_set, _OUT_BUF, _dump_code) ;
}
;

dumpargs        :
                | dumpargs dumparg
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got dumpargs\n") ;
}
;


dumparg         : word
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got dumparg\n") ;
  _dump_code = D_ERROR ;
}
                | setnum
{
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got dump setnum\n") ;
  _dump_set = _t_set ;
}
                | TTF_TOKEN
{
  _dump_code = D_TTF ;
}
                | NTOKEN
{
  _dump_code = D_NP ;
}
                | ATOKEN
{
  _dump_code = D_ATTR ;
}
;

report          : REPORTTOKEN word ENDOFQUERY
{
  if ( strcasecmp(_word, "memory") == 0 || strcasecmp(_word, "mem") == 0 ) 
    report_allocation(_OUT_BUF) ;
  else if ( strcasecmp(_word, "files") == 0 )
    report_files(_OUT_BUF) ;
#ifndef NO_SETITIMER
  else if ( strcasecmp(_word, "time") == 0 )
    report_time(_OUT_BUF) ;
#endif
  else {
    yyerror(_word) ;
    YYACCEPT;
  }
}
;

%%


static void
do_limit(void)
{
  int new_set ;
  Setrec *s ;


  prepare_error(TEMPUNSUPPORTEDCOMMAND, "limit", 0) ;
  return ;


  new_set = bss_dolimit(_limit_set, _q_lmask, _q_find_flags);
  
  if (new_set >=  0) {
    s = Setrecs[new_set] ;
    if ( ! (_q_find_flags & FFL_NOSAVE) ) {
      _q_set = new_set;
      _q_lastrec = -1;
    }	    
    if ( s->pstgtype & HASWT ) {
      if ( ! (_q_find_flags & FFL_NOSAVE) ) {
	if ( _q_verbose == 0 ) 
	  sprintf(_OUT_BUF,
  "S%d np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=%.3f nmpw=%d\n",
		 new_set, s->naw, 
		 s->maxweight, s->nmaxweight,
		 s->ngw, s->mpw, s->nmpw);
	else
	  sprintf(_OUT_BUF,
  "S%d np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=%.3f nmpw=%d realmpw=%.3f\n",
		 new_set, s->naw, 
		 s->maxweight, s->nmaxweight,
		 s->ngw, s->mpw, s->nmpw, s->realmpw);
      }
      else {
	if ( _q_verbose == 0 )
	  sprintf(_OUT_BUF,
		  "np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=%.3f nmpw=%d\n",
		 s->naw, 
		 s->maxweight, s->nmaxweight,
		 s->ngw, s->mpw, s->nmpw);
	else
	  sprintf(_OUT_BUF,
	 "np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=%.3f nmpw=%d realmpw=%.3f\n",
		 s->naw, 
		 s->maxweight, s->nmaxweight,
		 s->ngw, s->mpw, s->nmpw, s->realmpw);
      }
    }
    else {
      if ( ! (_q_find_flags & FFL_NOSAVE) ) 
	sprintf(_OUT_BUF, "S%d np=%d\n", new_set, s->naw);
      else
	sprintf(_OUT_BUF, "np=%d\n", s->naw);
    }
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}




static void
clear(void)
{
  while(yychar != ENDOFQUERY) yychar = yylex();
}


int
yyerror(char *message)
{
  char buf[1024] ;
  if ((u_char)*yytext == 0xff) sprintf(buf, "NULL:") ;
  else sprintf(buf, "\"%s\": ", yytext) ;
  prepare_error(SYNTAX, buf, message, 0) ;
  clear() ;
  return 0 ;
}



static void
i0_do_find(void)
{
  int res, set ;
  int np ;
  int ttf ;			/* Added March 2000, 2.291 */
  char *found_term ;	/* Added May 93, comes back pointing to
			   static storage. Not copying to set record
			   but this could easily be done if wanted.
			   (1996) This is now put in the set, has been
			   for some time. */
  
  if ( ! (_q_find_flags & FFL_NOSAVE) ) {
    set = bss_lkup_a(_q_term, strlen(_q_term), _q_attr, _q_lmask, 
		     _q_type, &res, &found_term, _q_find_flags,
		     &np, &ttf);

    if (set >=  0) {
      _q_set = set;
      _q_lastrec = -1;
      
      strcpy(_q_term, found_term) ;
      lastn = np ;
      last_ttf = ttf ;
      if ( _q_verbose == 0 ) 
	sprintf(_OUT_BUF, "S%d np=%d t=%s\n", set, np, found_term) ;
      else sprintf(_OUT_BUF, "S%d np=%d ttf=%d t=%s\n",
		   set, np, ttf, found_term) ;
      _OUT_BUF += strlen(_OUT_BUF) ;
    }

  } else {
    
    /* res was originally np, but np is now "returned" in var np
       (for consistency with ttf) */
    res = bss_lkup_a(_q_term, strlen(_q_term), _q_attr, _q_lmask, 
		    _q_type, &res, &found_term, _q_find_flags,
		    &np, &ttf);
    
    if (res >= 0) {
      strcpy(_q_term, found_term) ;
      lastn = np ;
      last_ttf = ttf ;
      if ( _q_verbose == 0 )
	sprintf(_OUT_BUF, "np=%d t=%s\n", np, found_term);
      else sprintf(_OUT_BUF, "np=%d ttf=%d, t=%s\n", np, ttf, found_term);
      _OUT_BUF += strlen(_OUT_BUF) ;
    }
  }
}

static void
i0_do_combine(void)
{
  int combine_set ;
  Setrec *s;

  _q_find_flags |= (_q_mark << 24) ;
  if ( _number_sets == 1 && _arg_weights[0] == WEIGHT_NOT_ASSIGNED &&
       _q_op != OP_TOPN && _q_op != OP_MARK && _q_op != OP_BM2 )
    _q_op = OP_COPY ;		/* Dubious (very (May 02)) */
  /* Does it need a warning message in that case? */
  combine_set =
    bss_do_combine(_number_sets, _arg_sets, _arg_weights, _arg_qtfs,
		   _q_op, _q_scorefunc, _q_maxgap, _q_aw, _q_gw, _q_target,
		   _q_find_flags, _q_lmask, _q_k1, _q_k2, _q_k3, _q_k7, _q_k8,
		   _q_bm25_b, _q_b3, _q_avedoclen, _q_querylen, _q_avequerylen,
		   _q_p_unit, _q_p_step, _q_p_maxlen, _q_wt_frig_factor);
  if (combine_set >= 0) {
    s = Setrecs[combine_set];
    if ( ! (_q_find_flags & FFL_NOSAVE) ) {
      _q_set = combine_set;
      _q_lastrec = -1;
    }	    
    if ( (s->pstgtype & HASWT) ) { /* and ? HAS_INDEX_WT */
      if ( ! ( _q_find_flags & FFL_NOSAVE)  ) {
	if ( s->mpw >= MAXWEIGHT ) {
	  sprintf(_OUT_BUF,
		  "S%d np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=INF nmpw=%d",
		  combine_set, s->naw, 
		  s->maxweight, s->nmaxweight,
		  s->ngw, s->nmpw);
	}
	else {
	  sprintf(_OUT_BUF,
		  "S%d np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=%.3f nmpw=%d",
		  combine_set, s->naw, 
		  s->maxweight, s->nmaxweight,
		  s->ngw, s->mpw, s->nmpw);
	}
      }
      else {
	if ( s->mpw >= MAXWEIGHT ) {
	  sprintf(_OUT_BUF, "np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=INF nmpw=%d",
		  s->naw, 
		  s->maxweight, s->nmaxweight,
		  s->ngw, s->nmpw);
	}
	else {
	  sprintf(_OUT_BUF, "np=%d maxwt=%.3f nmaxwt=%d ngw=%d mpw=%.3f nmpw=%d",
		  s->naw, 
		  s->maxweight, s->nmaxweight,
		  s->ngw, s->mpw, s->nmpw);
	}
      }
      _OUT_BUF += strlen(_OUT_BUF) ;
      if ( _q_verbose > 0 ) {
	if ( s->realmpw >= MAXWEIGHT )
	  sprintf(_OUT_BUF, " realmpw=INF") ;
	else sprintf(_OUT_BUF, " realmpw=%.3f", s->realmpw) ;

	_OUT_BUF += strlen(_OUT_BUF) ;
      }
      if ( (getenv("BSS_COMBINE_DO_STATS")) ) {
	sprintf(_OUT_BUF, " sum=%.1f sumsq=%.1f", s->weightsum, s->weightsumsq) ;
	_OUT_BUF += strlen(_OUT_BUF) ;
      }
      sprintf(_OUT_BUF, "\n") ;
      _OUT_BUF += strlen(_OUT_BUF) ;
    }
    else { /* (no weights in set) */ 
      if ( ! (_q_find_flags & FFL_NOSAVE) )
	sprintf(_OUT_BUF, "S%d np=%d\n", combine_set, s->naw);
      else
	sprintf(_OUT_BUF, "np=%d\n", s->naw);
      _OUT_BUF += strlen(_OUT_BUF) ;
    }
    lastn = s->naw ;
    last_ttf = s->ttf ;
  }
}


static void
do_display_limits(void)
{
  int num_lims, i;
  char *l_names, *p;

  l_names = bss_limits_avail(&num_lims) ;

  if ( num_lims == 1 ) sprintf(_OUT_BUF, "1 limit\n") ;
  else sprintf(_OUT_BUF, "%d limits\n", num_lims) ;
  _OUT_BUF += strlen(_OUT_BUF) ;

  for ( i = 0, p = l_names ; i < num_lims ; i++ ) {
    sprintf(_OUT_BUF, "%s\n", p) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
    p += strlen(p) + 2 ;
  }
}


static void
do_parse(void)
{
  char *parsed, *s ;
  int num_terms, counter ;

  parsed = bss_malloc((int)(1.5 * get_parse_buf(strlen(yytext)))) ;
  if ( parsed == NULL ) num_terms = -1 ;
  else num_terms = bss_parse(yytext, strlen(yytext), _q_attr, parsed);

  if ( num_terms >= 0 ) {
    sprintf(_OUT_BUF, "%d ", num_terms);
    _OUT_BUF += strlen(_OUT_BUF) ;
    s = parsed;
    for (counter = 0; counter < num_terms; counter++) {
      if ( counter > 0 ) {
	sprintf(_OUT_BUF, " ") ;
	_OUT_BUF++ ;
      }
      else strrncpy(_q_term, s, MAXEXITERMLEN) ;
      sprintf(_OUT_BUF, "t=%s", s);
      _OUT_BUF += strlen(_OUT_BUF) ;
      s += strlen(s) + 1;
    }
    sprintf(_OUT_BUF, "\n");
    _OUT_BUF++ ;
  }
  if ( parsed != NULL ) bss_free(parsed) ;
}


static void
do_parse_rgf(void)
{
  char *parsed, *s ;
  int num_terms, counter ;
	
  parsed = bss_malloc((int)(1.5 * get_parse_buf(strlen(yytext)))) ;
  if ( parsed == NULL ) num_terms = -1 ;
  else num_terms = bss_parse_rgf(yytext, strlen(yytext), _q_regname, _q_gslname,
				 _q_stemfuncname, parsed);

  if ( num_terms >= 0 ) {
    sprintf(_OUT_BUF, "%d ", num_terms);
    _OUT_BUF += strlen(_OUT_BUF) ;
    s = parsed;
    for (counter = 0; counter < num_terms; counter++) {
      if ( counter > 0 ) {
	sprintf(_OUT_BUF, " ") ;
	_OUT_BUF++ ;
      }
      else strrncpy(_q_term, s, MAXEXITERMLEN) ;
      sprintf(_OUT_BUF, "t=%s", s);
      _OUT_BUF += strlen(_OUT_BUF) ;
      s += strlen(s) + 1;
    }
    sprintf(_OUT_BUF, "\n");
    _OUT_BUF++ ;
  }
  if ( parsed != NULL ) bss_free(parsed) ;
}



static void
do_superparse(void)
{
  char *parsed, *s ;
  int num_terms, counter ;
	

  parsed = bss_malloc(3 * get_parse_buf(strlen(yytext))) ;
  if ( parsed == NULL ) num_terms = - 1 ;
  else num_terms = bss_superparse(yytext, strlen(yytext),
				  _q_attr, parsed);

  if ( num_terms >= 0 ) {
    sprintf(_OUT_BUF, "%d ", num_terms);
    _OUT_BUF += strlen(_OUT_BUF) ;
    s = parsed;
    for (counter = 0; counter < num_terms; counter++) {
      if ( counter > 0 ) {
	sprintf(_OUT_BUF, " ") ;
	_OUT_BUF++ ;
      }
      else strrncpy(_q_term, s + 1, MAXEXITERMLEN) ;
      sprintf(_OUT_BUF, "t=%s ", s + 1);
      _OUT_BUF += strlen(_OUT_BUF) ;
      if ( *s != ' ' ) sprintf(_OUT_BUF, "c=%c ", *s) ;
      else sprintf(_OUT_BUF, "c=N ") ;
      _OUT_BUF += strlen(_OUT_BUF) ;
      s += strlen(s) + 1;
      sprintf(_OUT_BUF, "s=%s", s);
      _OUT_BUF += strlen(_OUT_BUF) ;
      s += strlen(s) + 1;
    }
    sprintf(_OUT_BUF, "\n");
    _OUT_BUF++ ;
  }
  if ( parsed != NULL ) bss_free(parsed) ;
}

static void
do_superparse_rgf(void)
{
  char *parsed, *s ;
  int num_terms, counter ;
	
  parsed = bss_malloc(3 * get_parse_buf(strlen(yytext))) ;
  if ( parsed == NULL ) num_terms = -1 ;
  else num_terms = bss_superparse_rgf(yytext, strlen(yytext),
				      _q_regname, _q_gslname,
				      _q_stemfuncname, parsed);

  if ( num_terms >= 0 ) {
    sprintf(_OUT_BUF, "%d ", num_terms);
    _OUT_BUF += strlen(_OUT_BUF) ;
    s = parsed;
    for (counter = 0; counter < num_terms; counter++) {
      if ( counter > 0 ) {
	sprintf(_OUT_BUF, " ") ;
	_OUT_BUF++ ;
      }
      else strrncpy(_q_term, s + 1, MAXEXITERMLEN) ;
      sprintf(_OUT_BUF, "t=%s ", s + 1);
      _OUT_BUF += strlen(_OUT_BUF) ;
      if ( *s != ' ' ) sprintf(_OUT_BUF, "c=%c ", *s) ;
      else sprintf(_OUT_BUF, "c=N ") ;
      _OUT_BUF += strlen(_OUT_BUF) ;
      s += strlen(s) + 1;
      sprintf(_OUT_BUF, "s=%s", s);
      _OUT_BUF += strlen(_OUT_BUF) ;
      s += strlen(s) + 1;
    }
    sprintf(_OUT_BUF, "\n");
    _OUT_BUF++ ;
  }
  if ( parsed != NULL ) bss_free(parsed) ;
}

static void
gsl_lookup(void)
{
  char *result;
  Sg *sg;
  Gsl *gsl;

  sg = find_search_name(&Cdb, _q_attr);
  if ( sg == NULL ) return ;
  gsl = sg->gsl;
  result = gsl_translate_token(gsl, yytext);
  
  if  (result) {
    sprintf(_OUT_BUF, "%.*s\n", result[0] - 1, result+1);
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}


static void
_finish_show(void)
{
  double weight;
  int totlength = 0, length, counter;

  char *buf = _OUT_BUF ;

  if ( _q_recnum != DEFAULT_RECNUM ) { /* Record given by number takes
					  precedence */
    /* Set = -1 means just get by record number (if db.type = text) */
    totlength=bss_show(-1, _q_recnum, &Cdb, _q_format, &weight, buf,
		       0, _q_startstr, _q_finstr) ;
    buf += totlength ;
    if ( *(buf - 1) != '\n' ) {
      *buf++ = '\n' ; totlength++ ;
    }
    *buf = '\0' ;
  }
  else {
    if ( ! _r_was_set ) _q_rec = _q_lastrec + 1;
    
    for (counter = _q_rec; counter < _q_rec + _q_shownumber; counter++) {
      if ((length=bss_show(_q_set, counter, &Cdb, _q_format, 
			     &weight, buf, 0, _q_startstr, _q_finstr)) < 0) {
	if ( bss_Result == NO_SUCH_RECORD ) {
	  if ( counter > _q_rec || _q_noerror )
	    bss_Result = 0 ;	/* Frig up to allow error-free case where
				   at least one record has been produced */
	}
	break;
      }
      buf += length ;
      totlength += length ;
      if ( *(buf - 1) != '\n' ) {
	*buf++ = '\n' ; length++ ; totlength++ ;
      }
    }
    *buf = '\0' ;
    if ( ! (_q_format & 512) ) _q_lastrec = counter - 1 ;
  }
  if ( _q_format & 512 ) {
    sprintf(_OUT_BUF, "%d\n", totlength) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}

#undef LMIN
#define LMIN 10

static int get_parse_buf(int tlength)
{
  if ( tlength <= LMIN ) return LMIN ;
  else return tlength ;
}
     
