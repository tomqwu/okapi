%{
#define EXTERN extern
#include "i0_defs.h"
#include "charclass.h"
#include "y.tab.h"
#ifdef LEX
static GETC() ;
#endif
#ifdef FLEX
#undef YY_INPUT
static int buf_input() ;
#define YY_INPUT(buf, res, maxl) (res = buf_input(buf, maxl))

static int
buf_input(buf, maxl)
     char *buf ;
     int maxl ;
{
  int i ;
  int n = 0 ;
  for ( i = 0 ; i < maxl ; i++ ) {
    n++ ;
    if ( *_IN_BUF == '\0' ) {
      *buf = (unsigned char)255 ;
      break ;
    }
    else *buf++ = *_IN_BUF++ ;
  }
  return n ;
}
#endif /* FLEX */
  
struct token {char *pattern; int value; };
static int tokenize(struct token[]) ;

/* Not all of these tokens are used (Dec 94) */

static struct token _commands[] = {
                {"parse", PARSETOKEN},
                {"p", PARSETOKEN},
                {"superparse", SUPERPARSETOKEN},
                {"superp", SUPERPARSETOKEN},
                {"sp", SUPERPARSETOKEN},
		{"extract", EXTRACTTOKEN},
		{"ex", EXTRACTTOKEN},
                {"weight", WEIGHTTOKEN},
                {"wt", WEIGHTTOKEN},
                {"w", WEIGHTTOKEN},
		{"rscore", RSCORETOKEN},
		{"rsc", RSCORETOKEN},
                {"find", FINDTOKEN},
                {"fin", FINDTOKEN},
                {"fi", FINDTOKEN},
                {"f", FINDTOKEN},
		{"combine", COMBTOKEN},
		{"comb", COMBTOKEN},
		{"c", COMBTOKEN},
		{"stem", STEMTOKEN},
		{"st", STEMTOKEN},
                {"choose", CHOOSETOKEN},
                {"ch", CHOOSETOKEN},
                {"info", INFOTOKEN},
                {"inf", INFOTOKEN},
                {"show", SHOWTOKEN},
                {"sho", SHOWTOKEN},
                {"sh", SHOWTOKEN},
                {"s", SHOWTOKEN},
                {"delete", DELETETOKEN},
                {"del", DELETETOKEN},
		{"gsl", GSLTOKEN},
		{"findlimit", FINDLIMITTOKEN},
		{"fl", FINDLIMITTOKEN},
		{"set", SETTOKEN},
                {"debug", DEBUGTOKEN},
                {"dbg", DEBUGTOKEN},
		{"limit", LIMITTOKEN},
		{"lim", LIMITTOKEN},
		{"l", LIMITTOKEN},
		{"help", HELPTOKEN},
		{"h", HELPTOKEN},
		{"doclen", DOCLENGTHTOKEN},
		{"doclength", DOCLENGTHTOKEN},
		{"tf", TFTOKEN},
                {"display", DISPLAYTOKEN},
                {"disp", DISPLAYTOKEN},
                {"di", DISPLAYTOKEN},
                {"d", DISPLAYTOKEN},
		{"displaytrecstats", DISPLAYTRECSTATSTOKEN},
		{"ds_trec", DISPLAYTRECSTATSTOKEN},
                {"perror", PERRORTOKEN},
                {"dump", DUMPTOKEN},
                {"dmp", DUMPTOKEN},
		{"report", REPORTTOKEN},
		{"rep", REPORTTOKEN},
                {NULL, 0}
		};

static struct token arguments[] = {
                {"flags", FLAGSTOKEN},
		{"r", RTOKEN},
		{"rec", RTOKEN},
		{"record", RTOKEN},
                {"irn", IRNTOKEN},
                {"rn", IRNTOKEN},
		{"rload", RLOADTOKEN},
		{"r_load", RLOADTOKEN},
		{"big_rload", BIGRLOADTOKEN},
		{"bigrload", BIGRLOADTOKEN},
		{"bigr_load", BIGRLOADTOKEN},
		{"big_r_load", BIGRLOADTOKEN},
		{"theta", THETATOKEN},
		{"th", THETATOKEN},
		{"lamda", LAMDATOKEN},
		{"tnt", TNT_TOKEN},
		{"ctnt", TNT_TOKEN},
		{"vocab_size", TNT_TOKEN},
		{"tnp", TNPTOKEN},
		{"cttf", CTTF_TOKEN},
		{"ttf", TTF_TOKEN},
		{"big_r", BIGRTOKEN},
		{"bigr", BIGRTOKEN},
		{"big_n", BIGNTOKEN},
		{"bign", BIGNTOKEN},
		{"big_s", BIGSTOKEN},
		{"bigs", BIGSTOKEN},
		{"save", SAVETOKEN},
		{"save_set", SAVETOKEN},
		{"delete", DELTOKEN},
		{"del", DELTOKEN},
		{"type", TYPETOKEN},
		{"search_type", TYPETOKEN},
		{"op", OPTOKEN},
		{"opcode", OPTOKEN},
                {"operator", OPTOKEN},
		{"gap", GAPTOKEN},
		{"maxgap", GAPTOKEN},
                {"k1", K1TOKEN},
                {"k2", K2TOKEN},
		{"k3", K3TOKEN},
                {"k4", K4TOKEN},
                {"k5", K5TOKEN},
                {"k6", K6TOKEN},
                {"k7", K7TOKEN},
                {"k8", K8TOKEN},
                {"bm25b", BM25_B_TOKEN},
                {"bm25_b", BM25_B_TOKEN},
		{"b", BM25_B_TOKEN},
		{"b3", B3_TOKEN},	/* Preferred */
                {"bm25b3", B3_TOKEN},
                {"bm25_b3", B3_TOKEN},
		{"mode", MODETOKEN},
		{"avedoclen", AVEDOCLEN_TOKEN},
		{"adl", AVEDOCLEN_TOKEN},
		{"aql", AVEQUERYLEN_TOKEN},
		{"avequerylen", AVEQUERYLEN_TOKEN},
		{"qtf", QTFTOKEN},
		{"qlen", QUERYLEN_TOKEN},
                {"passage_unit", P_UNIT_TOKEN},
                {"p_unit", P_UNIT_TOKEN},
                {"passage_step", P_STEP_TOKEN},
                {"p_step", P_STEP_TOKEN},
                {"passage_maxlen", P_MAXLEN_TOKEN},
                {"p_maxlen", P_MAXLEN_TOKEN},
		{"aw", AWTOKEN},
		{"gw", GWTOKEN},
                {"nopos", NOPOSTOKEN},
                {"no_pos", NOPOSTOKEN},
                {"notf", NOTFTOKEN},
                {"no_tf", NOTFTOKEN},
		{"w", WTOKEN},
		{"weight", WTOKEN},
		{"wt", WTOKEN},
                {"limit", LIMITTOKEN},
		{"lim", LIMITTOKEN},
		{"l", LIMITTOKEN},
                {"target", TARGETTOKEN},
                {"top", TOPTOKEN},
		{"s", STOKEN},
		{"set", STOKEN},
		{"n", NTOKEN},
		{"offset", OFFSETTOKEN},
		{"oset", OFFSETTOKEN},
		{"field", FIELDTOKEN},
		{"fld", FIELDTOKEN},
		{"fd", FIELDTOKEN},
		{"sources", SRCTOKEN},
		{"source", SRCTOKEN},
		{"src", SRCTOKEN},
		{"class", CLASSTOKEN},
		{"classes", CLASSTOKEN},
		{"regime", REGTOKEN},
		{"reg", REGTOKEN},
		{"gsl", GSLTOKEN},
		{"stemfunction", STEMFUNCTIONTOKEN},
		{"stemfunc", STEMFUNCTIONTOKEN},
		{"stemfn", STEMFUNCTIONTOKEN},
		{"f", FTOKEN},  /* Deprecated (Feb 99) */
                {"format", FTOKEN},
                {"fmt", FTOKEN},
/* Next three to be replaced soon (Dec 94), see below 
		{"function", FTOKEN},
		{"func", FTOKEN},
                {"fn", FTOKEN},*/
/* Feb 99: keep generalized FUNCTOKEN but deprecate */
		{"function", FUNCTOKEN},
		{"func", FUNCTOKEN},
                {"fn", FUNCTOKEN}, 
		{"weight_function", WTFUNCTOKEN},
		{"weight_func", WTFUNCTOKEN},
		{"wt_func", WTFUNCTOKEN},
                {"wt_fn", WTFUNCTOKEN},
                {"weightfunction", WTFUNCTOKEN},
                {"wtfunction", WTFUNCTOKEN},
                {"wtfunc", WTFUNCTOKEN},
                {"wtfn", WTFUNCTOKEN},
		{"wtfactor", WTFACTORTOKEN},
		{"scorefunction", SCOREFUNCTOKEN},
		{"scorefunc", SCOREFUNCTOKEN},
		{"scorefn", SCOREFUNCTOKEN},
		{"score_function", SCOREFUNCTOKEN},
		{"score_func", SCOREFUNCTOKEN},
		{"score_fn", SCOREFUNCTOKEN},
		{"a", ATOKEN},
                {"attr", ATOKEN},
                {"attribute", ATOKEN},
                {"error", ERRORTOKEN},
                {"noerror", NOERRORTOKEN},
                {"option", OPTIONSTOKEN},
                {"opt", OPTIONSTOKEN},
                {"length", LENGTHTOKEN},
                {"len", LENGTHTOKEN},
                {"para", PARATOKEN},
                {"paras", PARASTOKEN},
                {"highlight", HIGHLIGHTTOKEN},
                {"hl", HIGHLIGHTTOKEN},
		{"db_flags", DBFLAGSTOKEN},
		{"dbflags", DBFLAGSTOKEN},
		{"mark", MARKTOKEN},
		{"version", VERSIONTOKEN},
		{"vers", VERSIONTOKEN},
		{"mem", MEMTOKEN},
		{"memory", MEMTOKEN},
		{"hold", HOLDTOKEN},
		{"hold_mem", HOLDTOKEN},
		{"no_st", NO_ST_TOKEN},
		{"no_selection_tree", NO_ST_TOKEN},
		{"st", ST_TOKEN},
		{"selection_tree", ST_TOKEN},
		{"open", OPENTOKEN},
		{"keep", KEEPTOKEN},
                {"debug", DEBUGTOKEN},
                {"dbg", DEBUGTOKEN},
                {"verbosity", VERBOSETOKEN},
                {"verbose", VERBOSETOKEN},
                {"frig", FRIGTOKEN},
		{"accept", ACCEPTTOKEN},
		{"prefer", PREFERTOKEN},
                {"database", DBTOKEN},
                {"db", DBTOKEN},
		{"code", CODETOKEN},
		{"stats", STATS_TOKEN},
		{"statistics", STATS_TOKEN},
		{"noshow", NOSHOW_TOKEN},
		{"no_show", NOSHOW_TOKEN},
		{"intermdiate", NOSHOW_TOKEN},
		{"startstring", STARTSTRTOKEN},
		{"startstr", STARTSTRTOKEN},
		{"finishstring", FINSTRTOKEN},
		{"finstr", FINSTRTOKEN},
                {NULL, 0}
		};

static struct token _words[] = {
		{"attributes", ATTSTOKEN},
		{"attrs", ATTSTOKEN},
		{"attribute", ATOKEN},
		{"attr", ATOKEN},
		{"avedoclen", AVEDOCLEN_TOKEN},
		{"adl", AVEDOCLEN_TOKEN},
		{"big_n", BIGNTOKEN},
		{"bign", BIGNTOKEN},
		{"cttf", CTTF_TOKEN},
                {"dbs", DBSTOKEN},
                {"databases", DBSTOKEN},
                {"db", DBTOKEN},
                {"database", DBTOKEN},
                {"gsls", GSLSTOKEN},
		{"lastweight", WTOKEN},
		{"lastwt", WTOKEN},
		{"lastw", WTOKEN},
		{"limits", LIMITSTOKEN},
		{"lims", LIMITSTOKEN},
		{"lastnp", NTOKEN},
		{"lastn", NTOKEN},
		{"np", NTOKEN},
		{"n", NTOKEN},
		{"regimes", REGIMESTOKEN},
		{"regs", REGIMESTOKEN},
		{"stats", STATS_TOKEN},
		{"statistics", STATS_TOKEN},
		{"stemfunctions", STEMFUNCTIONSTOKEN},
		{"stemfuncs", STEMFUNCTIONSTOKEN},
		{"stemfns", STEMFUNCTIONSTOKEN},
		{"tnt", TNT_TOKEN},
		{"ctnt", TNT_TOKEN},
		{"vocab_size", TNT_TOKEN},
		{"last_ttf", TTF_TOKEN},
		{"ttf", TTF_TOKEN},
		{"version", VERSIONTOKEN},
		{"vers", VERSIONTOKEN},
		{"weight", WTOKEN},
		{"wt", WTOKEN},
		{"w", WTOKEN},
		{NULL, 0}
		} ;

#undef getc
#define getc GETC

%}

%start 	AA BB

%%
<AA>[^\xff]*			{
                                  if ( Dbg & D_READER )
				    fprintf(bss_syslog,
					    "Rdr: got TERM=%s\n", yytext) ;
                                  return(TERM); 
                                }
[tT][ \t]*=[ \t]*	BEGIN AA;

<BB>[A-Za-z_][A-Za-z0-9_]*[ \t]*=	{
			 	char *p = yytext;
				int l = 0 ;
				while (! tok_space(*p) && *p != '=') {
				  p++; l++ ;
				}
                                yyless(l) ;
				*p = '\0'; 
/* Warning: unput() doesn't to work in flex 2.5.2 if %pointer */
				/* unput('='); */
                                if ( Dbg & D_READER )
                                  fprintf(bss_syslog,
					  "Rdr: looking in ARGUMENTS '%s'\n",
					  yytext) ;
				return(tokenize(arguments)); 
			}

<BB>"="			{
                          if ( Dbg & D_READER )
			    fprintf(bss_syslog, "Rdr: got EQUALS\n") ;
                          return(EQUALS);
                        }
<BB>-?[0-9]*\.?[0-9]*   {
                          if ( Dbg & D_READER )
			    fprintf(bss_syslog, "Rdr: got NUMBER\n") ;
                          return(NUMBER);
                        }
<BB>\$                   {
                          if ( Dbg & D_READER )
			    fprintf(bss_syslog, "Rdr: got DOLLAR\n") ;
                          return(DOLLAR);
                        }
<BB>[^ \t\n=\xff]+	{
                          if ( Dbg & D_READER )
			    fprintf(bss_syslog, "Rdr: looking in WORDS\n") ;
                          return(tokenize(_words));
                        }

[A-Za-z_][A-Za-z0-9_]*	{ BEGIN BB;
                          if ( Dbg & D_READER )
			    fprintf(bss_syslog, "Rdr: looking in COMMANDS\n") ;
                          return(tokenize(_commands)); 
                        }

 /*\n              	{ BEGIN 0; return(ENDOFQUERY); }*/
\xff              	{ BEGIN 0;
                          if ( Dbg & D_READER )
			    fprintf(bss_syslog, "Rdr: got ENDOFQUERY\n") ;
                          return(ENDOFQUERY); 
                        }

[ \t]*			;
.			;

%%

static int
tokenize(tokens)
     struct token tokens[];
{
  int counter;

  /* Tokenise the command */
  for (counter = 0; tokens[counter].pattern; counter++) 
    if (!strcasecmp(tokens[counter].pattern, yytext)) {
      if ( Dbg & D_READER ) fprintf(bss_syslog, "Rdr: got %s\n", yytext) ;
      return(tokens[counter].value);	
    }
    
  /* Doesn't match anything */
  if ( Dbg & D_READER ) fprintf(bss_syslog,
				"Rdr: got unlisted WORD (AWORD)\n") ;
  return AWORD ;
}


#ifdef LEX
/* If flex see YY_INPUT above */

static
GETC(instream)
     FILE *instream;
{
  int value;

  if (instream == yyin) {
    if (*_IN_BUF) value = *_IN_BUF++;
    else value = '\n';
  } 
  else value = getc(instream);
  return value ;
}
#endif /* LEX */

int
yywrap()
{
  return(1) ;
}

