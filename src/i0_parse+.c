/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "i0_parse+.y" /* yacc.c:339  */

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

#line 88 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ACCEPTTOKEN = 258,
    ATOKEN = 259,
    ATTSTOKEN = 260,
    AVEDOCLEN_TOKEN = 261,
    AVEQUERYLEN_TOKEN = 262,
    AWTOKEN = 263,
    BIGNTOKEN = 264,
    BIGRLOADTOKEN = 265,
    BIGRTOKEN = 266,
    BIGSTOKEN = 267,
    BM25_B_TOKEN = 268,
    B3_TOKEN = 269,
    CHOOSETOKEN = 270,
    CLASSTOKEN = 271,
    CODETOKEN = 272,
    COMBTOKEN = 273,
    TNT_TOKEN = 274,
    CTTF_TOKEN = 275,
    DBFLAGSTOKEN = 276,
    DBSTOKEN = 277,
    DBTOKEN = 278,
    DEBUGTOKEN = 279,
    DELETETOKEN = 280,
    DELTOKEN = 281,
    DISPLAYTOKEN = 282,
    DISPLAYTRECSTATSTOKEN = 283,
    DOCLENGTHTOKEN = 284,
    DOLLAR = 285,
    DUMPTOKEN = 286,
    EQUALS = 287,
    ERRORTOKEN = 288,
    NOERRORTOKEN = 289,
    EXTRACTTOKEN = 290,
    FIELDTOKEN = 291,
    FILETOKEN = 292,
    FINDLIMITTOKEN = 293,
    FINDTOKEN = 294,
    FINSTRTOKEN = 295,
    FLAGSTOKEN = 296,
    FRIGTOKEN = 297,
    FTOKEN = 298,
    FUNCTOKEN = 299,
    GAPTOKEN = 300,
    GSLSTOKEN = 301,
    GSLTOKEN = 302,
    GWTOKEN = 303,
    HELPTOKEN = 304,
    HIGHLIGHTTOKEN = 305,
    HOLDTOKEN = 306,
    INFOTOKEN = 307,
    IRNTOKEN = 308,
    K1TOKEN = 309,
    K2TOKEN = 310,
    K3TOKEN = 311,
    K4TOKEN = 312,
    K5TOKEN = 313,
    K6TOKEN = 314,
    K7TOKEN = 315,
    K8TOKEN = 316,
    KEEPTOKEN = 317,
    LAMDATOKEN = 318,
    LENGTHTOKEN = 319,
    LIMITTOKEN = 320,
    LIMITSTOKEN = 321,
    MARKTOKEN = 322,
    MEMTOKEN = 323,
    MODETOKEN = 324,
    NOPOSTOKEN = 325,
    NOSHOW_TOKEN = 326,
    NOTFTOKEN = 327,
    NO_ST_TOKEN = 328,
    NTOKEN = 329,
    NUMBER = 330,
    OFFSETTOKEN = 331,
    OPENTOKEN = 332,
    OPTIONSTOKEN = 333,
    OPTOKEN = 334,
    PARASTOKEN = 335,
    PARATOKEN = 336,
    PARSETOKEN = 337,
    PERRORTOKEN = 338,
    PREFERTOKEN = 339,
    P_MAXLEN_TOKEN = 340,
    P_STEP_TOKEN = 341,
    P_UNIT_TOKEN = 342,
    QETOKEN = 343,
    QTFTOKEN = 344,
    QUERYLEN_TOKEN = 345,
    REAL = 346,
    REGIMESTOKEN = 347,
    REGTOKEN = 348,
    REPORTTOKEN = 349,
    RLOADTOKEN = 350,
    RSCORETOKEN = 351,
    RTOKEN = 352,
    SAVETOKEN = 353,
    SETTOKEN = 354,
    SCOREFUNCTOKEN = 355,
    SHOWTOKEN = 356,
    SRCTOKEN = 357,
    STARTSTRTOKEN = 358,
    STATS_TOKEN = 359,
    STEMFUNCTIONSTOKEN = 360,
    STEMFUNCTIONTOKEN = 361,
    STEMTOKEN = 362,
    STOKEN = 363,
    ST_TOKEN = 364,
    SUPERPARSETOKEN = 365,
    TARGETTOKEN = 366,
    TERM = 367,
    TFTOKEN = 368,
    THETATOKEN = 369,
    TNPTOKEN = 370,
    TOPTOKEN = 371,
    TRECTOKEN = 372,
    TTF_TOKEN = 373,
    TYPETOKEN = 374,
    VERBOSETOKEN = 375,
    VERSIONTOKEN = 376,
    WEIGHTTOKEN = 377,
    AWORD = 378,
    WTFUNCTOKEN = 379,
    WTOKEN = 380,
    WTFACTORTOKEN = 381,
    UNKNOWNCOMMAND = 382,
    ENDOFQUERY = 383
  };
#endif
/* Tokens.  */
#define ACCEPTTOKEN 258
#define ATOKEN 259
#define ATTSTOKEN 260
#define AVEDOCLEN_TOKEN 261
#define AVEQUERYLEN_TOKEN 262
#define AWTOKEN 263
#define BIGNTOKEN 264
#define BIGRLOADTOKEN 265
#define BIGRTOKEN 266
#define BIGSTOKEN 267
#define BM25_B_TOKEN 268
#define B3_TOKEN 269
#define CHOOSETOKEN 270
#define CLASSTOKEN 271
#define CODETOKEN 272
#define COMBTOKEN 273
#define TNT_TOKEN 274
#define CTTF_TOKEN 275
#define DBFLAGSTOKEN 276
#define DBSTOKEN 277
#define DBTOKEN 278
#define DEBUGTOKEN 279
#define DELETETOKEN 280
#define DELTOKEN 281
#define DISPLAYTOKEN 282
#define DISPLAYTRECSTATSTOKEN 283
#define DOCLENGTHTOKEN 284
#define DOLLAR 285
#define DUMPTOKEN 286
#define EQUALS 287
#define ERRORTOKEN 288
#define NOERRORTOKEN 289
#define EXTRACTTOKEN 290
#define FIELDTOKEN 291
#define FILETOKEN 292
#define FINDLIMITTOKEN 293
#define FINDTOKEN 294
#define FINSTRTOKEN 295
#define FLAGSTOKEN 296
#define FRIGTOKEN 297
#define FTOKEN 298
#define FUNCTOKEN 299
#define GAPTOKEN 300
#define GSLSTOKEN 301
#define GSLTOKEN 302
#define GWTOKEN 303
#define HELPTOKEN 304
#define HIGHLIGHTTOKEN 305
#define HOLDTOKEN 306
#define INFOTOKEN 307
#define IRNTOKEN 308
#define K1TOKEN 309
#define K2TOKEN 310
#define K3TOKEN 311
#define K4TOKEN 312
#define K5TOKEN 313
#define K6TOKEN 314
#define K7TOKEN 315
#define K8TOKEN 316
#define KEEPTOKEN 317
#define LAMDATOKEN 318
#define LENGTHTOKEN 319
#define LIMITTOKEN 320
#define LIMITSTOKEN 321
#define MARKTOKEN 322
#define MEMTOKEN 323
#define MODETOKEN 324
#define NOPOSTOKEN 325
#define NOSHOW_TOKEN 326
#define NOTFTOKEN 327
#define NO_ST_TOKEN 328
#define NTOKEN 329
#define NUMBER 330
#define OFFSETTOKEN 331
#define OPENTOKEN 332
#define OPTIONSTOKEN 333
#define OPTOKEN 334
#define PARASTOKEN 335
#define PARATOKEN 336
#define PARSETOKEN 337
#define PERRORTOKEN 338
#define PREFERTOKEN 339
#define P_MAXLEN_TOKEN 340
#define P_STEP_TOKEN 341
#define P_UNIT_TOKEN 342
#define QETOKEN 343
#define QTFTOKEN 344
#define QUERYLEN_TOKEN 345
#define REAL 346
#define REGIMESTOKEN 347
#define REGTOKEN 348
#define REPORTTOKEN 349
#define RLOADTOKEN 350
#define RSCORETOKEN 351
#define RTOKEN 352
#define SAVETOKEN 353
#define SETTOKEN 354
#define SCOREFUNCTOKEN 355
#define SHOWTOKEN 356
#define SRCTOKEN 357
#define STARTSTRTOKEN 358
#define STATS_TOKEN 359
#define STEMFUNCTIONSTOKEN 360
#define STEMFUNCTIONTOKEN 361
#define STEMTOKEN 362
#define STOKEN 363
#define ST_TOKEN 364
#define SUPERPARSETOKEN 365
#define TARGETTOKEN 366
#define TERM 367
#define TFTOKEN 368
#define THETATOKEN 369
#define TNPTOKEN 370
#define TOPTOKEN 371
#define TRECTOKEN 372
#define TTF_TOKEN 373
#define TYPETOKEN 374
#define VERBOSETOKEN 375
#define VERSIONTOKEN 376
#define WEIGHTTOKEN 377
#define AWORD 378
#define WTFUNCTOKEN 379
#define WTOKEN 380
#define WTFACTORTOKEN 381
#define UNKNOWNCOMMAND 382
#define ENDOFQUERY 383

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 394 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  106
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   860

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  129
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  97
/* YYNRULES -- Number of rules.  */
#define YYNRULES  333
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  659

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   383

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   157,   157,   163,   170,   181,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   217,   222,   228,   234,   240,   247,   250,   253,   259,
     266,   273,   278,   272,   291,   292,   298,   299,   302,   308,
     313,   324,   334,   348,   347,   380,   386,   394,   398,   405,
     436,   437,   440,   448,   456,   464,   472,   479,   480,   483,
     526,   530,   536,   541,   545,   550,   556,   561,   567,   572,
     577,   582,   588,   594,   599,   607,   615,   620,   625,   630,
     632,   637,   641,   645,   649,   653,   657,   661,   665,   669,
     673,   677,   681,   685,   694,   695,   701,   707,   712,   718,
     723,   728,   733,   739,   745,   749,   751,   754,   766,   767,
     771,   780,   788,   798,   805,   810,   811,   815,   819,   826,
     830,   834,   838,   842,   847,   851,   855,   859,   863,   867,
     871,   875,   885,   893,   892,   908,   909,   912,   925,   931,
     936,   941,   946,   950,   955,   963,   967,   973,   980,   984,
     990,   996,   997,  1003,  1004,  1007,  1008,  1011,  1014,  1018,
    1022,  1029,  1028,  1092,  1093,  1096,  1100,  1105,  1109,  1113,
    1117,  1121,  1125,  1129,  1134,  1133,  1177,  1178,  1182,  1184,
    1188,  1192,  1196,  1200,  1204,  1208,  1212,  1217,  1221,  1226,
    1230,  1234,  1239,  1243,  1247,  1251,  1255,  1263,  1271,  1278,
    1279,  1293,  1292,  1313,  1314,  1318,  1320,  1322,  1326,  1330,
    1338,  1336,  1350,  1358,  1365,  1382,  1390,  1389,  1395,  1404,
    1528,  1529,  1533,  1537,  1541,  1545,  1549,  1553,  1561,  1569,
    1573,  1577,  1582,  1591,  1599,  1605,  1612,  1620,  1627,  1637,
    1647,  1652,  1656,  1660,  1664,  1668,  1672,  1676,  1681,  1685,
    1689,  1693,  1698,  1703,  1708,  1713,  1718,  1723,  1728,  1733,
    1738,  1744,  1749,  1754,  1803,  1808,  1814,  1818,  1825,  1824,
    1838,  1839,  1843,  1847,  1849,  1857,  1864,  1863,  1870,  1876,
    1880,  1887,  1898,  1914,  1921,  1944,  1949,  1955,  1961,  1969,
    1970,  1974,  1976,  1980,  1979,  1995,  1996,  1999,  2003,  2007,
    2011,  2015,  2020,  2024,  2033,  2040,  2048,  2047,  2064,  2065,
    2068,  2072,  2077,  2081,  2087,  2086,  2098,  2099,  2106,  2111,
    2116,  2120,  2124,  2130
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ACCEPTTOKEN", "ATOKEN", "ATTSTOKEN",
  "AVEDOCLEN_TOKEN", "AVEQUERYLEN_TOKEN", "AWTOKEN", "BIGNTOKEN",
  "BIGRLOADTOKEN", "BIGRTOKEN", "BIGSTOKEN", "BM25_B_TOKEN", "B3_TOKEN",
  "CHOOSETOKEN", "CLASSTOKEN", "CODETOKEN", "COMBTOKEN", "TNT_TOKEN",
  "CTTF_TOKEN", "DBFLAGSTOKEN", "DBSTOKEN", "DBTOKEN", "DEBUGTOKEN",
  "DELETETOKEN", "DELTOKEN", "DISPLAYTOKEN", "DISPLAYTRECSTATSTOKEN",
  "DOCLENGTHTOKEN", "DOLLAR", "DUMPTOKEN", "EQUALS", "ERRORTOKEN",
  "NOERRORTOKEN", "EXTRACTTOKEN", "FIELDTOKEN", "FILETOKEN",
  "FINDLIMITTOKEN", "FINDTOKEN", "FINSTRTOKEN", "FLAGSTOKEN", "FRIGTOKEN",
  "FTOKEN", "FUNCTOKEN", "GAPTOKEN", "GSLSTOKEN", "GSLTOKEN", "GWTOKEN",
  "HELPTOKEN", "HIGHLIGHTTOKEN", "HOLDTOKEN", "INFOTOKEN", "IRNTOKEN",
  "K1TOKEN", "K2TOKEN", "K3TOKEN", "K4TOKEN", "K5TOKEN", "K6TOKEN",
  "K7TOKEN", "K8TOKEN", "KEEPTOKEN", "LAMDATOKEN", "LENGTHTOKEN",
  "LIMITTOKEN", "LIMITSTOKEN", "MARKTOKEN", "MEMTOKEN", "MODETOKEN",
  "NOPOSTOKEN", "NOSHOW_TOKEN", "NOTFTOKEN", "NO_ST_TOKEN", "NTOKEN",
  "NUMBER", "OFFSETTOKEN", "OPENTOKEN", "OPTIONSTOKEN", "OPTOKEN",
  "PARASTOKEN", "PARATOKEN", "PARSETOKEN", "PERRORTOKEN", "PREFERTOKEN",
  "P_MAXLEN_TOKEN", "P_STEP_TOKEN", "P_UNIT_TOKEN", "QETOKEN", "QTFTOKEN",
  "QUERYLEN_TOKEN", "REAL", "REGIMESTOKEN", "REGTOKEN", "REPORTTOKEN",
  "RLOADTOKEN", "RSCORETOKEN", "RTOKEN", "SAVETOKEN", "SETTOKEN",
  "SCOREFUNCTOKEN", "SHOWTOKEN", "SRCTOKEN", "STARTSTRTOKEN",
  "STATS_TOKEN", "STEMFUNCTIONSTOKEN", "STEMFUNCTIONTOKEN", "STEMTOKEN",
  "STOKEN", "ST_TOKEN", "SUPERPARSETOKEN", "TARGETTOKEN", "TERM",
  "TFTOKEN", "THETATOKEN", "TNPTOKEN", "TOPTOKEN", "TRECTOKEN",
  "TTF_TOKEN", "TYPETOKEN", "VERBOSETOKEN", "VERSIONTOKEN", "WEIGHTTOKEN",
  "AWORD", "WTFUNCTOKEN", "WTOKEN", "WTFACTORTOKEN", "UNKNOWNCOMMAND",
  "ENDOFQUERY", "$accept", "request", "command", "unknowncommand", "word",
  "inumber", "dnumber", "setnum", "rnum", "irn", "frignum", "attr",
  "lmask", "choose", "$@1", "$@2", "chooseargs", "dbname", "find",
  "combine", "$@3", "lkupcmd", "combcmd", "ftoken", "sets", "set",
  "options", "option", "qualifiers", "qualifier", "stem", "stemfunc",
  "delete", "deltoken", "numbers", "number", "info", "show", "$@4",
  "show_args", "show_arg", "parse", "ptoken", "superparse", "sptoken",
  "pterm", "p1qualifiers", "p2qualifiers", "p1qualifier", "p2qualifier",
  "extract", "$@5", "extractargs", "extractarg", "weight", "$@6",
  "weightqualifiers", "weightqualifier", "wfunc", "rscore", "$@7",
  "rscorequalifiers", "rscorequalifier", "gsl", "$@8", "debug",
  "findlimit", "settoken", "$@9", "setargs", "setarg", "tnumber",
  "wnumber", "limit", "$@10", "limitargs", "limitarg", "display", "$@11",
  "disparg", "dispqualifiers", "dispqualifier", "displaytrecstats", "$@12",
  "stat_args", "stat_arg", "perror", "tf", "doclength", "$@13",
  "doclengthargs", "doclengtharg", "dump", "$@14", "dumpargs", "dumparg",
  "report", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383
};
# endif

#define YYPACT_NINF -393

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-393)))

#define YYTABLE_NINF -230

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     234,   -95,  -393,  -393,    59,  -393,   -50,  -393,  -393,  -393,
    -393,   -80,  -393,  -393,   190,  -393,  -393,   -86,   -76,  -393,
     -89,  -393,   -14,  -393,   -85,  -393,  -393,  -393,    53,   -73,
    -393,  -393,  -393,  -393,   -39,   -35,   -26,  -393,  -393,   -47,
    -393,  -393,  -393,   -31,  -393,   -31,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,    65,    -7,  -393,  -393,  -393,    -7,    68,  -393,
      -2,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,    -9,
      18,    19,    -8,  -393,   122,  -393,  -393,  -393,  -393,  -393,
    -393,    37,    39,    47,  -393,  -393,    48,  -393,    50,   652,
    -393,  -393,  -393,   147,  -393,  -393,  -393,  -393,  -393,  -393,
     152,   -28,    -7,    58,    -7,   -43,   345,   155,  -393,    61,
    -393,  -393,   -49,  -393,   158,   160,   161,     2,   -42,  -393,
       2,   -42,   164,   -19,   -29,    -7,   108,  -393,   125,  -393,
     179,   291,    41,    13,    27,    16,    78,  -393,  -393,  -393,
     -76,  -393,   128,  -393,  -393,  -393,   -13,  -393,    25,  -393,
     172,   173,   175,   177,   178,   182,   185,   188,   189,   194,
     196,   205,   206,   208,  -393,   213,   215,   216,   218,   219,
     222,   223,   225,   228,   232,   235,   236,   238,   239,   242,
     243,   244,   245,   246,   247,   248,   251,   252,   253,   255,
     256,   261,  -393,  -393,   262,   652,  -393,   323,   -76,   280,
     128,   264,   266,    83,  -393,   138,  -393,   108,   749,   269,
     271,   273,   274,   275,   277,   278,   282,   283,   287,   288,
    -393,   290,  -393,  -393,  -393,   -67,  -393,  -393,  -393,   -76,
     -76,   -76,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
     128,   293,  -393,  -393,   542,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,   294,   295,
     297,   299,   300,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
     308,   310,   318,   319,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,   184,  -393,   321,  -393,  -393,  -393,   328,   332,
    -393,  -393,  -393,  -393,  -393,   292,   249,   296,   298,   303,
     304,   305,   306,   309,   311,   128,   312,   128,   314,   315,
     316,   317,   325,   326,   327,   330,   334,   336,   128,   -57,
     -57,   -57,   -57,   260,   337,   341,   343,   346,   348,   -57,
     128,   -57,   128,   349,   -40,   350,   128,  -393,   338,   342,
     353,   361,   371,  -393,  -393,  -393,  -393,  -393,   322,   395,
     396,   401,   403,   404,   405,   406,   409,   410,   412,   413,
     414,   415,   424,   426,   429,   430,   431,  -393,  -393,  -393,
    -393,   433,  -393,   391,   -15,  -393,  -393,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   464,   465,   467,   468,   469,
     470,  -393,  -393,   128,   128,   -57,   -57,   -57,   -57,   -57,
     -57,   -57,   -57,   128,   227,  -393,  -393,  -393,  -393,   -76,
     240,  -393,   128,   128,   128,   -57,   128,   -25,   128,   128,
     -57,  -393,   -57,   128,   128,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,   -57,   -76,   128,   128,   -76,  -393,   128,   128,   128,
     128,   428,   432,   463,   471,   476,   478,   128,   128,   128,
     128,   128,   479,   128,   482,   128,  -393,  -393,  -393,  -393,
     128,   128,   391,   391,   391,   -57,   -57,   128,   391,   -57,
     391,   391,   391,   391,   391,   -57,   128,   -57,   -76,   -57,
     -57,   -57,   -57,   -57,   -76,   128,   128,   128,   128,   -57,
     128,   -57,   -57,   128,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,    41,    53,     0,   124,   286,   303,   316,   324,
     171,     0,    59,   220,     0,   278,   157,     0,     0,   211,
     226,   143,     0,   160,     0,   184,    31,     3,     0,     0,
      30,     6,     7,     8,     0,     0,   104,     9,    10,     0,
      11,    12,    15,   163,    16,   163,    17,    13,    14,    19,
      22,    20,    21,    18,    28,    29,    25,    23,    24,    26,
      27,     5,    44,     0,    33,   222,   223,   288,     0,   285,
       0,   299,   305,   318,   326,   173,   224,   225,   163,     0,
       0,     0,     0,   138,     0,   140,   137,   136,   139,    32,
     142,     0,     0,     0,   280,   314,     0,   213,     0,     0,
     145,   119,   118,     0,   315,   186,     1,     2,    48,    49,
       0,    64,     0,     0,    67,    60,    56,     0,   123,     0,
     127,   128,     0,   125,     0,     0,     0,   161,   161,   165,
     161,   161,     0,     0,    64,    67,    60,   290,     0,   289,
       0,     0,     0,     0,     0,     0,     0,   134,   135,   132,
       0,   130,     0,   141,   129,   133,   279,   333,     0,   228,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   210,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   209,   232,     0,   227,   230,     0,     0,     0,
       0,     0,     0,     0,    65,     0,    50,    61,    57,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      55,     0,   116,   115,   105,     0,   122,   120,   126,     0,
       0,     0,   162,   167,   155,   164,   156,   166,   158,   159,
       0,     0,    46,    42,     0,    35,   292,   291,   293,   294,
     295,   297,   298,   296,   302,   301,   287,   300,     0,     0,
       0,     0,     0,   313,   304,   307,   306,   317,   320,   322,
     321,   323,   319,   332,   331,   330,   325,   328,   329,   327,
       0,     0,     0,     0,   172,   175,   177,   176,   183,   174,
     221,    39,     0,    37,     0,   282,   283,   281,     0,     0,
     212,   218,   216,   215,   214,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   231,     0,     0,
       0,     0,     0,   144,   147,   148,   151,   146,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   185,   197,   188,
     187,     0,    36,     0,     0,    51,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    89,    68,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   170,   168,   169,    45,     0,
       0,    54,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   131,     0,     0,     0,   272,   242,   260,   261,   269,
     267,   265,   266,   258,   259,   236,   238,   270,   241,   250,
     251,   252,   253,   254,   255,   256,   257,   235,    40,   275,
     274,   247,   245,   246,   248,   273,   271,   264,   263,   262,
     268,   243,   240,   249,   234,   237,   277,   276,   244,   233,
     239,     0,     0,     0,     0,     0,   117,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,    38,    63,    62,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    66,    58,   109,   108,   113,   107,
     110,   111,   112,   106,   114,   121,    47,    43,   309,   312,
     308,   311,   310,   181,   180,   179,   178,   182,   284,   219,
     217,   152,   154,   149,   150,   153,   203,   202,   199,   200,
     207,   193,   194,   195,   196,   205,   190,   192,   201,   198,
     204,   206,   191,   208,   189,    97,    99,    72,    95,    96,
      83,    78,    71,    73,    77,    90,    91,    92,    93,    94,
      82,   103,    76,    88,    79,    87,    80,    84,    81,    69,
     102,   101,   100,    98,    75,    70,    86,    85,    74
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -393,  -393,  -393,  -393,   -17,    -4,  -392,     1,   -70,   -27,
    -393,    24,   -91,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,   472,    21,   369,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,   383,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,    42,    -1,   466,  -393,    46,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,   313,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
     301,  -330,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    28,    29,    30,    91,   120,   537,   134,   112,    92,
     214,   243,   203,    31,    62,   450,   133,   253,    32,    33,
      63,    34,    35,    36,   114,   217,   218,   432,   116,   234,
      37,   103,    38,    39,   122,   123,    40,    41,   100,   207,
     367,    42,    43,    44,    45,   244,   127,   128,   245,   129,
      46,    75,   144,   299,    47,   105,   209,   390,   204,    48,
      97,   158,   314,    49,    78,    50,    51,    52,    99,   205,
     206,   491,   508,    53,    94,   156,   307,    54,    71,   266,
     140,   267,    55,    72,   141,   276,    56,    57,    58,    73,
     142,   282,    59,    74,   143,   289,    60
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      66,    96,   539,    98,   251,   124,   221,    70,   255,   113,
     492,   493,   494,   211,   211,   538,   124,   283,   489,   501,
     221,   503,   119,   146,   150,   233,    64,    84,    64,   221,
     101,   221,    76,    61,    89,   506,   308,   111,    93,  -229,
     121,   213,    95,   104,   130,   221,   219,    89,    77,   265,
      64,   125,   184,   106,    67,   107,    89,   115,    68,    68,
     536,   117,   125,   290,   126,   306,   490,   313,   137,   110,
     242,   110,   279,   220,   296,   126,    89,   145,    69,   237,
      84,   118,    68,   507,   136,   304,   132,   284,   311,   108,
     184,   291,   102,   109,    84,    68,   212,   212,    89,   309,
     138,    68,   139,   292,    89,   576,   577,   578,   579,   580,
     581,   582,   583,   215,   242,   280,   252,   297,   389,   147,
     151,    68,   110,   121,   110,   591,   287,   431,   300,   293,
     597,   285,   598,   302,    64,    68,    89,   365,   110,   388,
     232,   286,   275,   278,   288,   295,   148,   149,   303,    68,
     627,   628,   629,   310,   152,   294,   633,   305,   635,   636,
     637,   638,   639,   431,   264,   153,   281,   154,   298,   277,
     246,    65,   248,   249,   247,   155,   157,   247,   159,   208,
     366,   601,   312,   221,   210,   256,   216,   235,   257,   236,
     239,   368,   240,   241,    79,    80,   250,   219,   258,   259,
     255,   301,   260,    64,   315,   316,   392,   317,   364,   318,
     319,   395,    81,    82,   320,   630,   631,   321,   444,   634,
     322,   323,   445,   446,   447,   640,   324,   642,   325,   644,
     645,   646,   647,   648,    -4,     1,    83,   326,   327,   654,
     328,   656,   657,    84,   184,   329,   448,   330,   331,     2,
     332,   333,     3,   261,   334,   335,    85,   336,     4,     5,
     337,     6,     7,     8,   338,     9,   396,   339,   340,    10,
     341,   342,    11,    12,   343,   344,   345,   346,   347,   348,
     349,    13,    86,   350,   351,   352,    14,   353,   354,   369,
     370,   371,   372,   355,   356,    87,   393,   262,   394,    15,
     373,   433,   268,   434,   263,   146,   435,   436,   269,   437,
     438,    88,   461,    89,   439,   440,    16,    17,    90,   441,
     442,   475,   443,   477,   174,   449,   452,   453,    18,   454,
      19,   455,   456,    20,   488,    21,   374,   375,   376,   377,
     457,    22,   458,   378,    23,   184,   502,    24,   504,   221,
     459,   460,   510,   462,   379,   585,    25,   358,   270,   271,
     463,    26,    27,   359,   464,   272,   360,   465,   587,   380,
     511,   467,   466,   468,   512,   381,    84,   110,   469,   470,
     471,   472,   222,   495,   473,   513,   474,   476,   382,   478,
     479,   480,   481,   514,   383,   384,   223,   361,   385,    68,
     482,   483,   484,   515,   202,   485,   386,   224,   387,   486,
     184,   487,   496,   225,   273,   226,   497,   227,   498,   274,
     110,   499,   228,   500,   505,   509,   362,   517,   518,   574,
     575,    68,   586,   519,   516,   520,   521,   522,   523,   584,
     593,   524,   525,   229,   526,   527,   528,   529,   588,   589,
     590,   363,   592,   594,   595,   596,   530,   230,   531,   599,
     600,   532,   533,   534,   231,   535,   536,   540,   541,   542,
     543,   544,   545,   546,   547,   548,   549,   550,   551,   552,
     553,   554,   555,   556,   557,   558,   559,   560,   561,   562,
     563,   564,   565,   566,   567,   602,   568,   569,   605,   570,
     571,   572,   573,   610,   254,   238,   357,   611,     0,   603,
     604,   131,     0,   606,   607,   608,   609,     0,     0,     0,
       0,     0,   391,   616,   617,   618,   619,   620,     0,   622,
       0,   624,     0,     0,     0,   135,   625,   626,   612,     0,
       0,   643,     0,   632,     0,     0,   613,   649,   397,   398,
     399,   614,   641,   615,   621,   400,   401,   623,     0,     0,
       0,   650,   651,   652,   653,     0,   655,     0,   402,   658,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   403,
       0,     0,     0,     0,     0,     0,     0,   404,     0,     0,
     405,     0,     0,   406,     0,     0,   407,   408,   409,     0,
       0,     0,   410,   411,   412,     0,     0,   184,     0,   413,
     414,   415,   416,   417,   418,   419,     0,     0,     0,   420,
       0,   421,     0,     0,     0,     0,     0,   422,   423,   424,
       0,     0,   425,     0,     0,     0,     0,     0,     0,     0,
     426,     0,   427,     0,     0,     0,   428,     0,     0,     0,
       0,   429,     0,   430,     0,   160,   161,     0,   162,   163,
       0,   164,   165,   166,   167,   168,   169,     0,     0,     0,
     451,   170,   171,     0,     0,     0,   172,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   173,   174,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   175,   176,   177,   178,
     179,   180,   181,   182,     0,   183,     0,   184,     0,     0,
     185,     0,   186,     0,   187,   188,     0,     0,     0,     0,
       0,   189,     0,     0,     0,     0,   190,   191,   192,   193,
       0,     0,     0,     0,     0,     0,     0,   194,     0,     0,
     195,     0,   196,     0,     0,   397,   398,   399,     0,     0,
       0,   197,   400,   401,     0,     0,   198,   199,     0,     0,
       0,   200,   201,     0,     0,   402,   202,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   403,     0,     0,     0,
       0,     0,     0,     0,   404,     0,     0,   405,     0,     0,
     406,     0,     0,   407,   408,   409,     0,     0,     0,   410,
     411,   412,     0,     0,   184,     0,   413,   414,   415,   416,
     417,   418,   419,     0,     0,     0,   420,     0,   421,     0,
       0,     0,     0,     0,   422,   423,   424,     0,     0,   425,
       0,     0,     0,     0,     0,     0,     0,   426,     0,   427,
       0,     0,     0,   428,     0,     0,     0,     0,   429,     0,
     430
};

static const yytype_int16 yycheck[] =
{
       4,    18,   394,    20,    23,    47,     4,     6,    75,    36,
     340,   341,   342,    42,    42,    30,    47,     4,    75,   349,
       4,   351,    39,    32,    32,   116,    75,    53,    75,     4,
      44,     4,   112,   128,   123,    75,    11,    36,    14,   128,
      39,   111,   128,   128,    45,     4,    89,   123,   128,   140,
      75,    93,    65,     0,   104,   128,   123,    36,   108,   108,
      75,   108,    93,    36,   106,   156,   123,   158,    67,    97,
     112,    97,   142,   116,   144,   106,   123,    78,   128,   128,
      53,   128,   108,   123,    63,    98,    21,    74,   158,   128,
      65,    64,   106,   128,    53,   108,   125,   125,   123,    74,
      32,   108,   104,    76,   123,   435,   436,   437,   438,   439,
     440,   441,   442,   112,   112,   142,   133,   144,   209,   128,
     128,   108,    97,   122,    97,   455,   143,   218,   112,   102,
     460,   118,   462,   150,    75,   108,   123,   207,    97,   209,
     116,   128,   141,   142,   143,   144,   128,   128,   152,   108,
     542,   543,   544,   128,    32,   128,   548,   156,   550,   551,
     552,   553,   554,   254,   140,   128,   142,   128,   144,   128,
     128,   112,   130,   131,   128,   128,   128,   131,   128,    32,
     207,   511,   158,     4,    32,     6,   128,    32,     9,   128,
      32,   208,    32,    32,     4,     5,    32,    89,    19,    20,
      75,   123,    23,    75,    32,    32,   210,    32,   207,    32,
      32,   128,    22,    23,    32,   545,   546,    32,   235,   549,
      32,    32,   239,   240,   241,   555,    32,   557,    32,   559,
     560,   561,   562,   563,     0,     1,    46,    32,    32,   569,
      32,   571,   572,    53,    65,    32,   250,    32,    32,    15,
      32,    32,    18,    74,    32,    32,    66,    32,    24,    25,
      32,    27,    28,    29,    32,    31,   128,    32,    32,    35,
      32,    32,    38,    39,    32,    32,    32,    32,    32,    32,
      32,    47,    92,    32,    32,    32,    52,    32,    32,     9,
      10,    11,    12,    32,    32,   105,    32,   118,    32,    65,
      20,    32,    11,    32,   125,    32,    32,    32,    17,    32,
      32,   121,   128,   123,    32,    32,    82,    83,   128,    32,
      32,   325,    32,   327,    44,    32,    32,    32,    94,    32,
      96,    32,    32,    99,   338,   101,    56,    57,    58,    59,
      32,   107,    32,    63,   110,    65,   350,   113,   352,     4,
      32,    32,   356,    32,    74,   128,   122,    34,    67,    68,
      32,   127,   128,    40,    32,    74,    43,    75,   128,    89,
      32,    75,   123,    75,    32,    95,    53,    97,    75,    75,
      75,    75,    37,   123,    75,    32,    75,    75,   108,    75,
      75,    75,    75,    32,   114,   115,    51,    74,   118,   108,
      75,    75,    75,    32,   124,    75,   126,    62,   128,    75,
      65,    75,    75,    68,   123,    70,    75,    72,    75,   128,
      97,    75,    77,    75,    75,    75,   103,    32,    32,   433,
     434,   108,   449,    32,   112,    32,    32,    32,    32,   443,
     457,    32,    32,    98,    32,    32,    32,    32,   452,   453,
     454,   128,   456,   457,   458,   459,    32,   112,    32,   463,
     464,    32,    32,    32,   119,    32,    75,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,   512,    32,    32,   515,    32,
      32,    32,    32,    75,   135,   122,   205,    75,    -1,   513,
     514,    45,    -1,   517,   518,   519,   520,    -1,    -1,    -1,
      -1,    -1,   209,   527,   528,   529,   530,   531,    -1,   533,
      -1,   535,    -1,    -1,    -1,    63,   540,   541,    75,    -1,
      -1,   558,    -1,   547,    -1,    -1,    75,   564,     6,     7,
       8,    75,   556,    75,    75,    13,    14,    75,    -1,    -1,
      -1,   565,   566,   567,   568,    -1,   570,    -1,    26,   573,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    -1,    -1,
      48,    -1,    -1,    51,    -1,    -1,    54,    55,    56,    -1,
      -1,    -1,    60,    61,    62,    -1,    -1,    65,    -1,    67,
      68,    69,    70,    71,    72,    73,    -1,    -1,    -1,    77,
      -1,    79,    -1,    -1,    -1,    -1,    -1,    85,    86,    87,
      -1,    -1,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,   100,    -1,    -1,    -1,   104,    -1,    -1,    -1,
      -1,   109,    -1,   111,    -1,     3,     4,    -1,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
     128,    19,    20,    -1,    -1,    -1,    24,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    -1,    63,    -1,    65,    -1,    -1,
      68,    -1,    70,    -1,    72,    73,    -1,    -1,    -1,    -1,
      -1,    79,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    -1,    -1,
      98,    -1,   100,    -1,    -1,     6,     7,     8,    -1,    -1,
      -1,   109,    13,    14,    -1,    -1,   114,   115,    -1,    -1,
      -1,   119,   120,    -1,    -1,    26,   124,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    45,    -1,    -1,    48,    -1,    -1,
      51,    -1,    -1,    54,    55,    56,    -1,    -1,    -1,    60,
      61,    62,    -1,    -1,    65,    -1,    67,    68,    69,    70,
      71,    72,    73,    -1,    -1,    -1,    77,    -1,    79,    -1,
      -1,    -1,    -1,    -1,    85,    86,    87,    -1,    -1,    90,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,   100,
      -1,    -1,    -1,   104,    -1,    -1,    -1,    -1,   109,    -1,
     111
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,    15,    18,    24,    25,    27,    28,    29,    31,
      35,    38,    39,    47,    52,    65,    82,    83,    94,    96,
      99,   101,   107,   110,   113,   122,   127,   128,   130,   131,
     132,   142,   147,   148,   150,   151,   152,   159,   161,   162,
     165,   166,   170,   171,   172,   173,   179,   183,   188,   192,
     194,   195,   196,   202,   206,   211,   215,   216,   217,   221,
     225,   128,   143,   149,    75,   112,   134,   104,   108,   128,
     136,   207,   212,   218,   222,   180,   112,   128,   193,     4,
       5,    22,    23,    46,    53,    66,    92,   105,   121,   123,
     128,   133,   138,   140,   203,   128,   133,   189,   133,   197,
     167,    44,   106,   160,   128,   184,     0,   128,   128,   128,
      97,   136,   137,   138,   153,   154,   157,   108,   128,   133,
     134,   136,   163,   164,    47,    93,   106,   175,   176,   178,
     175,   176,    21,   145,   136,   153,   154,   136,    32,   104,
     209,   213,   219,   223,   181,   175,    32,   128,   128,   128,
      32,   128,    32,   128,   128,   128,   204,   128,   190,   128,
       3,     4,     6,     7,     9,    10,    11,    12,    13,    14,
      19,    20,    24,    43,    44,    54,    55,    56,    57,    58,
      59,    60,    61,    63,    65,    68,    70,    72,    73,    79,
      84,    85,    86,    87,    95,    98,   100,   109,   114,   115,
     119,   120,   124,   141,   187,   198,   199,   168,    32,   185,
      32,    42,   125,   137,   139,   136,   128,   154,   155,    89,
     116,     4,    37,    51,    62,    68,    70,    72,    77,    98,
     112,   119,   140,   141,   158,    32,   128,   128,   164,    32,
      32,    32,   112,   140,   174,   177,   174,   178,   174,   174,
      32,    23,   133,   146,   155,    75,     6,     9,    19,    20,
      23,    74,   118,   125,   140,   141,   208,   210,    11,    17,
      67,    68,    74,   123,   128,   136,   214,   128,   136,   137,
     138,   140,   220,     4,    74,   118,   128,   133,   136,   224,
      36,    64,    76,   102,   128,   136,   137,   138,   140,   182,
     112,   123,   133,   134,    98,   136,   141,   205,    11,    74,
     128,   137,   140,   141,   191,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,   199,    34,    40,
      43,    74,   103,   128,   136,   137,   138,   169,   133,     9,
      10,    11,    12,    20,    56,    57,    58,    59,    63,    74,
      89,    95,   108,   114,   115,   118,   126,   128,   137,   141,
     186,   187,   134,    32,    32,   128,   128,     6,     7,     8,
      13,    14,    26,    37,    45,    48,    51,    54,    55,    56,
      60,    61,    62,    67,    68,    69,    70,    71,    72,    73,
      77,    79,    85,    86,    87,    90,    98,   100,   104,   109,
     111,   141,   156,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,   133,   133,   133,   133,   134,    32,
     144,   128,    32,    32,    32,    32,    32,    32,    32,    32,
      32,   128,    32,    32,    32,    75,   123,    75,    75,    75,
      75,    75,    75,    75,    75,   134,    75,   134,    75,    75,
      75,    75,    75,    75,    75,    75,    75,    75,   134,    75,
     123,   200,   200,   200,   200,   123,    75,    75,    75,    75,
      75,   200,   134,   200,   134,    75,    75,   123,   201,    75,
     134,    32,    32,    32,    32,    32,   112,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    75,   135,    30,   135,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,   134,   134,   200,   200,   200,   200,
     200,   200,   200,   200,   134,   128,   133,   128,   134,   134,
     134,   200,   134,   133,   134,   134,   134,   200,   200,   134,
     134,   200,   133,   134,   134,   133,   134,   134,   134,   134,
      75,    75,    75,    75,    75,    75,   134,   134,   134,   134,
     134,    75,   134,    75,   134,   134,   134,   135,   135,   135,
     200,   200,   134,   135,   200,   135,   135,   135,   135,   135,
     200,   134,   200,   133,   200,   200,   200,   200,   200,   133,
     134,   134,   134,   134,   200,   134,   200,   200,   134
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   129,   130,   130,   130,   130,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   143,   144,   142,   145,   145,   146,   146,   147,   147,
     147,   147,   147,   149,   148,   150,   150,   151,   151,   152,
     153,   153,   154,   154,   154,   154,   154,   155,   155,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   156,   156,   156,   157,   157,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   159,   160,   160,
     161,   161,   161,   161,   162,   163,   163,   164,   164,   165,
     165,   165,   165,   165,   165,   165,   165,   165,   165,   165,
     165,   165,   165,   167,   166,   168,   168,   169,   169,   169,
     169,   169,   169,   169,   169,   170,   170,   171,   172,   172,
     173,   174,   174,   175,   175,   176,   176,   177,   178,   178,
     178,   180,   179,   181,   181,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   184,   183,   185,   185,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   187,
     187,   189,   188,   190,   190,   191,   191,   191,   191,   191,
     193,   192,   194,   194,   195,   195,   197,   196,   196,   196,
     198,   198,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   200,   200,   201,   201,   203,   202,
     204,   204,   205,   205,   205,   206,   207,   206,   206,   206,
     206,   208,   208,   208,   208,   208,   208,   208,   208,   209,
     209,   210,   210,   212,   211,   213,   213,   214,   214,   214,
     214,   214,   214,   214,   215,   216,   218,   217,   219,   219,
     220,   220,   220,   220,   222,   221,   223,   223,   224,   224,
     224,   224,   224,   225
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     0,     0,     6,     0,     3,     1,     3,     2,     2,
       3,     4,     4,     0,     5,     3,     2,     3,     5,     1,
       1,     2,     4,     4,     1,     2,     4,     0,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     0,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     5,     1,     1,
       3,     5,     3,     2,     1,     1,     2,     1,     1,     3,
       3,     5,     3,     3,     3,     3,     2,     2,     2,     2,
       2,     3,     2,     0,     4,     0,     2,     1,     1,     3,
       3,     1,     3,     3,     3,     3,     3,     1,     3,     3,
       1,     0,     1,     0,     2,     1,     2,     1,     3,     3,
       3,     0,     4,     0,     2,     1,     1,     1,     3,     3,
       3,     3,     3,     1,     0,     4,     0,     2,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     0,     4,     0,     2,     1,     1,     3,     1,     3,
       0,     4,     2,     2,     2,     2,     0,     3,     3,     1,
       1,     2,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     1,     0,     3,
       0,     2,     1,     1,     3,     2,     0,     4,     2,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       2,     1,     1,     0,     4,     0,     2,     1,     3,     3,
       3,     3,     3,     1,     2,     2,     0,     4,     0,     2,
       1,     1,     1,     1,     0,     4,     0,     2,     1,     1,
       1,     1,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 158 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got command ENDOFQUERY\n") ;
  clear(); YYACCEPT;
}
#line 2017 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 164 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got ENDOFQUERY\n") ;
  clear(); YYACCEPT;
}
#line 2027 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 170 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got nothing\n");
  clear(); YYACCEPT;
}
#line 2037 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 182 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got error ENDOFQUERY\n") ;
  clear(); YYACCEPT;
}
#line 2047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 218 "i0_parse+.y" /* yacc.c:1646  */
    {
  yyerror("unknown command", yytext) ;
}
#line 2055 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 223 "i0_parse+.y" /* yacc.c:1646  */
    {
  strcpy(_word, yytext) ;
}
#line 2063 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 229 "i0_parse+.y" /* yacc.c:1646  */
    {
  _inumber = atoi(yytext) ;
}
#line 2071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 235 "i0_parse+.y" /* yacc.c:1646  */
    {
  _dnumber = atof(yytext) ;
}
#line 2079 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 241 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got setnum\n") ;
  _t_set = atoi(yytext) ;
}
#line 2088 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 254 "i0_parse+.y" /* yacc.c:1646  */
    {
  _frigvalue = _dnumber ;
}
#line 2096 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 260 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_attr, yytext, NNAME_MAX) ;
}
#line 2104 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 267 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_lmask = _inumber ;
}
#line 2112 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 273 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_db_flags = DEFAULT_DB_FLAGS ;
}
#line 2120 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 278 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog, "Prsr: got CHOOSETOKEN chooseargs dbname\n") ;
}
#line 2129 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 283 "i0_parse+.y" /* yacc.c:1646  */
    {

  if (bss_open_db(_word, _q_db_flags) == 0) {
    reinit() ;
  }
}
#line 2140 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 293 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_db_flags = _inumber ;
}
#line 2148 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 303 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_term, _word, MAXEXITERMLEN - 1) ;
  /* May need truncation warning here */
  i0_do_find() ;
}
#line 2158 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 309 "i0_parse+.y" /* yacc.c:1646  */
    {
  printf ("doing combining..\n");
  i0_do_combine() ;
}
#line 2167 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 314 "i0_parse+.y" /* yacc.c:1646  */
    {
  int result = make_set(-1, _inumber) ;
  if ( result >= 0 ) {
    _q_set = result ;
    _q_lastrec = -1 ;
    sprintf(_OUT_BUF, "S%d np=1\n", result) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
#line 2181 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 325 "i0_parse+.y" /* yacc.c:1646  */
    {
  int result = make_set(_t_set, _inumber) ;
  if ( result >= 0 ) {
    _q_set = result ;
    _q_lastrec = -1 ;
    sprintf(_OUT_BUF, "S%d np=1\n", result) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
#line 2195 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 335 "i0_parse+.y" /* yacc.c:1646  */
    {
  int result = make_set(_t_set, _inumber) ;
  if ( result >= 0 ) {
    _q_set = result ;
    _q_lastrec = -1 ;
    sprintf(_OUT_BUF, "S%d np=1\n", result) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
#line 2209 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 348 "i0_parse+.y" /* yacc.c:1646  */
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
#line 2240 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 375 "i0_parse+.y" /* yacc.c:1646  */
    {
  i0_do_combine() ;
}
#line 2248 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 381 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( Dbg & D_PARSER ) fprintf(bss_syslog,
				"Prsr: got ftoken qualifiers TERM\n") ;
  strcpy(_word, yytext) ;
}
#line 2258 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 387 "i0_parse+.y" /* yacc.c:1646  */
    {
  /* No term given, use current _q_term */
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got ftoken qualifiers\n") ;
  strcpy(_word, _q_term) ;
}
#line 2268 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 395 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got ftoken sets options\n") ;
}
#line 2276 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 399 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_target = _inumber ;		/* Might as well use _q_target */
  _q_op = OP_TOPN ;
}
#line 2285 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 406 "i0_parse+.y" /* yacc.c:1646  */
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
#line 2318 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 441 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = _dnumber ;
  _arg_qtfs[_number_sets] = 0 ;
  _number_sets++;
}
#line 2329 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 449 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = lastweight ;
  _arg_qtfs[_number_sets] = 0 ;
  _number_sets++;
}
#line 2340 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 457 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = WEIGHT_NOT_ASSIGNED ;
  _arg_qtfs[_number_sets] = 0 ;
  _q_wt_frig_factor[_number_sets] = DEFAULT_WT_FRIG_FACTOR ;
  _number_sets++;
}
#line 2352 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 465 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_sets[_number_sets] = _t_set ;
  _arg_weights[_number_sets] = WEIGHT_NOT_ASSIGNED ;
  _arg_qtfs[_number_sets] = 0 ;
  _q_wt_frig_factor[_number_sets] = _frigvalue ;
  _number_sets++;
}
#line 2364 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 473 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_qtfs[_number_sets - 1] = _inumber ;
  /* already been incremented */
}
#line 2373 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 484 "i0_parse+.y" /* yacc.c:1646  */
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
#line 2420 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 527 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_scorefunc = _inumber ;
}
#line 2428 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 531 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_maxgap = _inumber ;
}
#line 2436 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 537 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_aw = _dnumber ;
}
#line 2444 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 542 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_gw = _dnumber ;
}
#line 2452 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 546 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_target = _inumber ;
}
#line 2460 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 551 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags &= ~FFL_NOSAVE ;
  else _q_find_flags |= FFL_NOSAVE ;
}
#line 2469 "y.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 557 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_MEM ;
  else _q_find_flags &= ~FFL_MEM ;
}
#line 2478 "y.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 562 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_HOLD_MEM ;
  else _q_find_flags &= ~FFL_HOLD_MEM ;
}
#line 2487 "y.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 568 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_FILE ;
  else _q_find_flags &= ~FFL_FILE ;
}
#line 2496 "y.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 573 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= FFL_NOPOS ;
  else _q_find_flags &= ~(FFL_NOPOS|FFL_NOTF) ;
}
#line 2505 "y.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 578 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= (FFL_NOTF|FFL_NOPOS) ;
  else _q_find_flags &= ~FFL_NOTF ;
}
#line 2514 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 583 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_OPEN ;
  else _q_find_flags &= ~FFL_OPEN ;
}
#line 2523 "y.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 589 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_KEEP ;
  else _q_find_flags &= ~FFL_KEEP ;
}
#line 2532 "y.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 595 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= FFL_DELETE ;
  else _q_find_flags &= ~FFL_DELETE ;
}
#line 2541 "y.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 600 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) {
    _q_find_flags |= FFL_NO_SELECTION_TREE ;
    _q_find_flags &= ~FFL_SELECTION_TREE ;
  }
  else _q_find_flags &= ~FFL_NO_SELECTION_TREE ;
}
#line 2553 "y.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 608 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) {
    _q_find_flags |= FFL_SELECTION_TREE ;
    _q_find_flags &= ~FFL_NO_SELECTION_TREE ;
  }
  else _q_find_flags &= ~FFL_SELECTION_TREE ;
}
#line 2565 "y.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 616 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= FFL_DO_STATS ;
  else _q_find_flags &= ~FFL_DO_STATS ;
}
#line 2574 "y.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 621 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= FFL_NOSHOW ;
  else _q_find_flags &= ~FFL_NOSHOW ;
}
#line 2583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 626 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( strcasecmp(_word, "quick") == 0 ) _q_find_flags |= FFL_QUICK_COMBINE ;
  else _q_find_flags &= ~FFL_QUICK_COMBINE ;
}
#line 2592 "y.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 633 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k1 = _dnumber ;
}
#line 2600 "y.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 638 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k2 = _dnumber ;
}
#line 2608 "y.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 642 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k3 = _dnumber ;
}
#line 2616 "y.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 646 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k7 = _dnumber ;
}
#line 2624 "y.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 650 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k8 = _dnumber ;
}
#line 2632 "y.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 654 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bm25_b = _dnumber ;
}
#line 2640 "y.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 658 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_b3 = _dnumber ;
}
#line 2648 "y.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 662 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_avedoclen = _inumber ;
}
#line 2656 "y.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 666 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_querylen = _inumber ;
}
#line 2664 "y.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 670 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_avequerylen = _inumber ;
}
#line 2672 "y.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 674 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_p_unit = _inumber ;
}
#line 2680 "y.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 678 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _inumber >= 1 ) _q_p_step = _inumber ;
}
#line 2688 "y.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 682 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_p_maxlen = _inumber ;
}
#line 2696 "y.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 686 "i0_parse+.y" /* yacc.c:1646  */
    {
  /* Needs range check here */
  _q_mark = _inumber ;		/* Goes in top 8 bits of find_flags */
}
#line 2705 "y.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 696 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got qualifiers\n") ;
}
#line 2713 "y.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 702 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags &= ~FFL_NOSAVE ;
  else _q_find_flags |= FFL_NOSAVE ;
}
#line 2722 "y.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 708 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_MEM ;
  else _q_find_flags &= ~FFL_MEM ;
}
#line 2731 "y.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 713 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_HOLD_MEM ;
  else _q_find_flags &= ~FFL_HOLD_MEM ;
}
#line 2740 "y.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 719 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_FILE ;
  else _q_find_flags &= ~FFL_FILE ;
}
#line 2749 "y.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 724 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= FFL_NOPOS ;
  else _q_find_flags &= ~(FFL_NOPOS|FFL_NOTF) ;
}
#line 2758 "y.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 729 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= (FFL_NOTF|FFL_NOPOS) ;
  else _q_find_flags &= ~FFL_NOTF ;
}
#line 2767 "y.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 734 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_OPEN ;
  else _q_find_flags &= ~FFL_OPEN ;
}
#line 2776 "y.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 740 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags |= FFL_KEEP ;
  else _q_find_flags &= ~FFL_KEEP ;
}
#line 2785 "y.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 746 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_type = _inumber ;
}
#line 2793 "y.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 755 "i0_parse+.y" /* yacc.c:1646  */
    {
  int l ;
  if ( Dbg & D_PARSER )
    fprintf(bss_syslog,
	    "Prsr: got STEMTOKEN STEMFUNCTIONTOKEN EQUALS AWORD TERM\n");
  sprintf(_OUT_BUF, "t=%s", _word) ;
  l = bss_stem_term(yytext, _OUT_BUF + 2, _OUT_BUF + 2) ;
}
#line 2806 "y.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 772 "i0_parse+.y" /* yacc.c:1646  */
    {
  int j ;
  for ( j = 0 ; j < _number_sets ; j++ ) {
    bss_clear_setrec(_arg_sets[j]) ;
    if ( _q_set == _arg_sets[j] ) _q_set = DEFAULT_SET ;
  }
}
#line 2818 "y.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 781 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( strcasecmp(_word, "all") == 0 ) {
    bss_clear_all_setrecs() ;
    _q_set = DEFAULT_SET ;
  }
  else yyerror(_word) ;
}
#line 2830 "y.tab.c" /* yacc.c:1646  */
    break;

  case 122:
#line 789 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( strcasecmp(_word, "all") == 0 ) {
    bss_clear_all_setrecs() ;
    _q_set = DEFAULT_SET ;
  }
  else yyerror(_word) ;
}
#line 2842 "y.tab.c" /* yacc.c:1646  */
    break;

  case 123:
#line 799 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_clear_setrec(_q_set) ;
  _q_set = DEFAULT_SET ;
}
#line 2851 "y.tab.c" /* yacc.c:1646  */
    break;

  case 124:
#line 806 "i0_parse+.y" /* yacc.c:1646  */
    {
  _number_sets = 0 ;
}
#line 2859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 127:
#line 816 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_sets[_number_sets++] = _inumber ;
}
#line 2867 "y.tab.c" /* yacc.c:1646  */
    break;

  case 128:
#line 821 "i0_parse+.y" /* yacc.c:1646  */
    {
  _arg_sets[_number_sets++] = _t_set ;
}
#line 2875 "y.tab.c" /* yacc.c:1646  */
    break;

  case 129:
#line 827 "i0_parse+.y" /* yacc.c:1646  */
    {
  get_rec_info_text(_inumber, _OUT_BUF) ;
}
#line 2883 "y.tab.c" /* yacc.c:1646  */
    break;

  case 130:
#line 831 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_describe_database(NULL, _OUT_BUF, 0) ;
}
#line 2891 "y.tab.c" /* yacc.c:1646  */
    break;

  case 131:
#line 835 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_describe_database(_word, _OUT_BUF, 0) ;
}
#line 2899 "y.tab.c" /* yacc.c:1646  */
    break;

  case 132:
#line 839 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_describe_databases(_OUT_BUF, 0) ;
}
#line 2907 "y.tab.c" /* yacc.c:1646  */
    break;

  case 133:
#line 843 "i0_parse+.y" /* yacc.c:1646  */
    {
  (void)bss_describe_attribute(_q_attr, _OUT_BUF) ;
  /*prepare_error(TEMPUNSUPPORTEDCOMMAND, "info attribute=<name>", 0) ;*/
}
#line 2916 "y.tab.c" /* yacc.c:1646  */
    break;

  case 134:
#line 848 "i0_parse+.y" /* yacc.c:1646  */
    {
  prepare_error(TEMPUNSUPPORTEDCOMMAND, "info attribute", 0) ;
}
#line 2924 "y.tab.c" /* yacc.c:1646  */
    break;

  case 135:
#line 852 "i0_parse+.y" /* yacc.c:1646  */
    {
  prepare_error(TEMPUNSUPPORTEDCOMMAND, "info attributes", 0) ;
}
#line 2932 "y.tab.c" /* yacc.c:1646  */
    break;

  case 136:
#line 856 "i0_parse+.y" /* yacc.c:1646  */
    {
    bss_get_stemfunctions(_OUT_BUF) ;
}
#line 2940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 137:
#line 860 "i0_parse+.y" /* yacc.c:1646  */
    {
    bss_get_regimes(_OUT_BUF) ;
}
#line 2948 "y.tab.c" /* yacc.c:1646  */
    break;

  case 138:
#line 864 "i0_parse+.y" /* yacc.c:1646  */
    {
  prepare_error(TEMPUNSUPPORTEDCOMMAND, "info gsls", 0) ;
}
#line 2956 "y.tab.c" /* yacc.c:1646  */
    break;

  case 139:
#line 868 "i0_parse+.y" /* yacc.c:1646  */
    {
    bss_get_version(_OUT_BUF) ;
}
#line 2964 "y.tab.c" /* yacc.c:1646  */
    break;

  case 140:
#line 872 "i0_parse+.y" /* yacc.c:1646  */
    {
    do_display_limits() ;
}
#line 2972 "y.tab.c" /* yacc.c:1646  */
    break;

  case 141:
#line 876 "i0_parse+.y" /* yacc.c:1646  */
    {
  if (!strcasecmp(_word,"sets"))
    prepare_error(UNSUPPORTEDCOMMAND, "info sets", 0) ;
  else {
    char buf[256] ;
    sprintf(buf, "info %s", _word) ;
    prepare_error(SYNTAX, "info", _word, 0) ;
  }
}
#line 2986 "y.tab.c" /* yacc.c:1646  */
    break;

  case 142:
#line 886 "i0_parse+.y" /* yacc.c:1646  */
    {
  prepare_error(SYNTAX, "info,", "no subject", 0) ;
}
#line 2994 "y.tab.c" /* yacc.c:1646  */
    break;

  case 143:
#line 893 "i0_parse+.y" /* yacc.c:1646  */
    {
  _r_was_set = FALSE ;
  _q_format = default_format ;
  _q_shownumber = DEFAULT_SHOW_NUMBER ;
  _q_recnum = DEFAULT_RECNUM ;
  _q_noerror = DEFAULT_NOERROR ;
  *(_q_startstr) = *(_q_finstr) = '\0' ;
}
#line 3007 "y.tab.c" /* yacc.c:1646  */
    break;

  case 144:
#line 902 "i0_parse+.y" /* yacc.c:1646  */
    {
  _finish_show() ;
}
#line 3015 "y.tab.c" /* yacc.c:1646  */
    break;

  case 147:
#line 913 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3031 "y.tab.c" /* yacc.c:1646  */
    break;

  case 148:
#line 926 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_rec = _inumber ;
  _r_was_set = TRUE ;
}
#line 3040 "y.tab.c" /* yacc.c:1646  */
    break;

  case 149:
#line 932 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_format = _inumber ;
}
#line 3048 "y.tab.c" /* yacc.c:1646  */
    break;

  case 150:
#line 937 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_shownumber = _inumber ;
}
#line 3056 "y.tab.c" /* yacc.c:1646  */
    break;

  case 151:
#line 942 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_recnum = _inumber ;
}
#line 3064 "y.tab.c" /* yacc.c:1646  */
    break;

  case 152:
#line 947 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_noerror = _tvalue ;
}
#line 3072 "y.tab.c" /* yacc.c:1646  */
    break;

  case 153:
#line 951 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_startstr, _word, NNAME_MAX) ;
}
#line 3080 "y.tab.c" /* yacc.c:1646  */
    break;

  case 154:
#line 956 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_finstr, _word, NNAME_MAX) ;
}
#line 3088 "y.tab.c" /* yacc.c:1646  */
    break;

  case 155:
#line 964 "i0_parse+.y" /* yacc.c:1646  */
    {
  do_parse() ;
}
#line 3096 "y.tab.c" /* yacc.c:1646  */
    break;

  case 156:
#line 968 "i0_parse+.y" /* yacc.c:1646  */
    {
  do_parse_rgf() ;
}
#line 3104 "y.tab.c" /* yacc.c:1646  */
    break;

  case 157:
#line 974 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  *_q_regname = *_q_stemfuncname = *_q_gslname = '\0' ;
}
#line 3113 "y.tab.c" /* yacc.c:1646  */
    break;

  case 158:
#line 981 "i0_parse+.y" /* yacc.c:1646  */
    {
  do_superparse() ;
}
#line 3121 "y.tab.c" /* yacc.c:1646  */
    break;

  case 159:
#line 985 "i0_parse+.y" /* yacc.c:1646  */
    {
  do_superparse_rgf() ;
}
#line 3129 "y.tab.c" /* yacc.c:1646  */
    break;

  case 160:
#line 991 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  *_q_regname = *_q_stemfuncname = *_q_gslname = '\0' ;
}
#line 3138 "y.tab.c" /* yacc.c:1646  */
    break;

  case 162:
#line 998 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got pterm\n") ;
}
#line 3146 "y.tab.c" /* yacc.c:1646  */
    break;

  case 168:
#line 1015 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_regname, _word, NNAME_MAX) ;
}
#line 3154 "y.tab.c" /* yacc.c:1646  */
    break;

  case 169:
#line 1019 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_stemfuncname, _word, NNAME_MAX) ;
}
#line 3162 "y.tab.c" /* yacc.c:1646  */
    break;

  case 170:
#line 1023 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_gslname, _word, NNAME_MAX) ;
}
#line 3170 "y.tab.c" /* yacc.c:1646  */
    break;

  case 171:
#line 1029 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3185 "y.tab.c" /* yacc.c:1646  */
    break;

  case 172:
#line 1041 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3239 "y.tab.c" /* yacc.c:1646  */
    break;

  case 175:
#line 1097 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_set = _t_set ;
}
#line 3247 "y.tab.c" /* yacc.c:1646  */
    break;

  case 176:
#line 1101 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_rec = _inumber ;
  _q_set = -1 ;
}
#line 3256 "y.tab.c" /* yacc.c:1646  */
    break;

  case 177:
#line 1106 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_rec = _inumber ;
}
#line 3264 "y.tab.c" /* yacc.c:1646  */
    break;

  case 178:
#line 1110 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_offset = _inumber ;
}
#line 3272 "y.tab.c" /* yacc.c:1646  */
    break;

  case 179:
#line 1114 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_length = _inumber ;
}
#line 3280 "y.tab.c" /* yacc.c:1646  */
    break;

  case 180:
#line 1118 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_field = _inumber ;
}
#line 3288 "y.tab.c" /* yacc.c:1646  */
    break;

  case 181:
#line 1122 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_field = find_field_num(_word) ;
}
#line 3296 "y.tab.c" /* yacc.c:1646  */
    break;

  case 182:
#line 1126 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_sources = _tvalue ;
}
#line 3304 "y.tab.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1134 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3334 "y.tab.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1162 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3350 "y.tab.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1185 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_func = _inumber ;
}
#line 3358 "y.tab.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1189 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_wnumber = _inumber ;
}
#line 3366 "y.tab.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1193 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_ttf = _inumber ;
}
#line 3374 "y.tab.c" /* yacc.c:1646  */
    break;

  case 192:
#line 1197 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_qtf = _inumber ;
}
#line 3382 "y.tab.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1201 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k3 = atof(yytext) ;
}
#line 3390 "y.tab.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1205 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k4 = atof(yytext) ;
}
#line 3398 "y.tab.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1209 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k5 = atof(yytext) ;
}
#line 3406 "y.tab.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1213 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_k6 = atof(yytext) ;
}
#line 3414 "y.tab.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1218 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_r = _inumber ;
}
#line 3422 "y.tab.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1222 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_s = _inumber ;
}
#line 3430 "y.tab.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1227 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bigr = _inumber ;
}
#line 3438 "y.tab.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1231 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bigs = _inumber ;
}
#line 3446 "y.tab.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1235 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_rload = _inumber ;
}
#line 3454 "y.tab.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1240 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bigrload = _inumber ;
}
#line 3462 "y.tab.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1244 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bign = _inumber ;		/* Normally zero, then it uses Cdb.bign */
}
#line 3470 "y.tab.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1248 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_theta = _inumber ;		
}
#line 3478 "y.tab.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1252 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_lamda = atof(yytext) ;
}
#line 3486 "y.tab.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1256 "i0_parse+.y" /* yacc.c:1646  */
    {
#ifdef LARGEFILES
  _q_tnp = atoll(yytext) ;
#else
  _q_tnp = atof(yytext) ;
#endif
}
#line 3498 "y.tab.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1264 "i0_parse+.y" /* yacc.c:1646  */
    {
#ifdef LARGEFILES
  _q_cttf = atoll(yytext) ;
#else
  _q_cttf = atof(yytext) ;
#endif
}
#line 3510 "y.tab.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1272 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_wtfactor = atof(yytext) ;
}
#line 3518 "y.tab.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1293 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_wnumber = lastn ;
  _q_lmask = default_lmask ;
  _q_r = DEFAULT_R ;
  _q_bigr = default_bigr ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
}
#line 3530 "y.tab.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1303 "i0_parse+.y" /* yacc.c:1646  */
    {

  double score ;
  score = bss_get_rscore(&Cdb, _q_attr, _q_lmask, _q_wnumber, _q_bigr, _q_r) ;
  sprintf(_OUT_BUF, "%.3f\n", score) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
#line 3542 "y.tab.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1323 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_wnumber = _inumber ;
}
#line 3550 "y.tab.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1327 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_r = _inumber ;
}
#line 3558 "y.tab.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1331 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bigr = _inumber ;
}
#line 3566 "y.tab.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1338 "i0_parse+.y" /* yacc.c:1646  */
    {
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
}
#line 3574 "y.tab.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1343 "i0_parse+.y" /* yacc.c:1646  */
    {
  gsl_lookup();
}
#line 3582 "y.tab.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1351 "i0_parse+.y" /* yacc.c:1646  */
    {
  int result = bss_calc_debug(yytext) ;
  if ( result > 0 ) {
    bss_set_debug(result) ;
    _q_debug = result ;
  }
}
#line 3594 "y.tab.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1359 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_set_debug(_inumber) ;
  _q_debug = _inumber ;
}
#line 3603 "y.tab.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1366 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3623 "y.tab.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1383 "i0_parse+.y" /* yacc.c:1646  */
    {
  yyerror("no limit name given") ;
}
#line 3631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1390 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_Result = 0 ;
}
#line 3639 "y.tab.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1396 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_Result = 0 ;
  if ( strcasecmp(_word, "defaults") == 0 ) {
    reinit() ;
  }
  else yyerror(_word) ;
}
#line 3651 "y.tab.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1405 "i0_parse+.y" /* yacc.c:1646  */
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
#line 3777 "y.tab.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1534 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( bss_set_limit(_q_lmask) != (u_int)-1 ) default_lmask = _q_lmask ;
}
#line 3785 "y.tab.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1538 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_verbose = atoi(yytext) ;
}
#line 3793 "y.tab.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1542 "i0_parse+.y" /* yacc.c:1646  */
    {
  default_theta = _inumber ;
}
#line 3801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1546 "i0_parse+.y" /* yacc.c:1646  */
    {
  default_lamda = atof(yytext) ;
}
#line 3809 "y.tab.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1550 "i0_parse+.y" /* yacc.c:1646  */
    {
  default_tnt = _inumber ;
}
#line 3817 "y.tab.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1554 "i0_parse+.y" /* yacc.c:1646  */
    {
#ifdef LARGEFILES
  default_tnp = atoll(yytext) ;
#else
  default_tnp = atof(yytext) ;
#endif
}
#line 3829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1562 "i0_parse+.y" /* yacc.c:1646  */
    {
#ifdef LARGEFILES
  default_cttf = atoll(yytext) ;
#else
  default_cttf = atof(yytext) ;
#endif
}
#line 3841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1570 "i0_parse+.y" /* yacc.c:1646  */
    {
  default_wfunc = _inumber ;
}
#line 3849 "y.tab.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1574 "i0_parse+.y" /* yacc.c:1646  */
    {
  default_scorefunc = _inumber ;
}
#line 3857 "y.tab.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1578 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_format = atoi(yytext);
}
#line 3865 "y.tab.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1583 "i0_parse+.y" /* yacc.c:1646  */
    {
  Sg *result, *find_search_name() ;

    result = find_search_name(&Cdb, yytext) ;
    if ( result != NULL ) 
      strrncpy(default_attr, yytext, NNAME_MAX) ;
}
#line 3877 "y.tab.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1592 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl &= ~FFL_NOSAVE ;
    else default_ffl |= FFL_NOSAVE ;
  }
}
#line 3888 "y.tab.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1600 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) {
    if ( _wvalue >= -2 && _wvalue <= 2 ) default_search_type = _wvalue ;
  }
}
#line 3898 "y.tab.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1606 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl |= FFL_NOPOS ;
    else default_ffl &= ~(FFL_NOPOS|FFL_NOTF) ;
  }
}
#line 3909 "y.tab.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1613 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl |= (FFL_NOTF|FFL_NOPOS) ;
    else default_ffl &= ~FFL_NOTF ;
  }
}
#line 3920 "y.tab.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1621 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) default_ffl |= FFL_MEM ;
    else default_ffl &= ~FFL_MEM ;
  }
}
#line 3931 "y.tab.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1628 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) {
      default_ffl |= FFL_NO_SELECTION_TREE ;
      default_ffl &= ~FFL_SELECTION_TREE ;
    }
    else default_ffl &= ~FFL_NO_SELECTION_TREE ;
  }
}
#line 3945 "y.tab.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1638 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) {
    if ( _tvalue ) {
      default_ffl |= FFL_SELECTION_TREE ;
      default_ffl &= ~FFL_NO_SELECTION_TREE ;
    }
    else default_ffl &= ~FFL_SELECTION_TREE ;
  }
}
#line 3959 "y.tab.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1648 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k1 = atof(yytext) ;
}
#line 3967 "y.tab.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1653 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k2 = atof(yytext) ;
}
#line 3975 "y.tab.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1657 "i0_parse+.y" /* yacc.c:1646  */
    {
  default_k3 = atof(yytext) ;
}
#line 3983 "y.tab.c" /* yacc.c:1646  */
    break;

  case 253:
#line 1661 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k4 = atof(yytext) ;
}
#line 3991 "y.tab.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1665 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k5 = atof(yytext) ;
}
#line 3999 "y.tab.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1669 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k6 = atof(yytext) ;
}
#line 4007 "y.tab.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1673 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k7 = atof(yytext) ;
}
#line 4015 "y.tab.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1677 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_k8 = atof(yytext) ;
}
#line 4023 "y.tab.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1682 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_bm25_b = atof(yytext) ;
}
#line 4031 "y.tab.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1686 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_b3 = atof(yytext) ;
}
#line 4039 "y.tab.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1690 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_avedoclen = atoi(yytext) ;
}
#line 4047 "y.tab.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1694 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) ) default_avequerylen = atoi(yytext) ;
}
#line 4055 "y.tab.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1699 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 1 ) default_p_unit = atoi(yytext) ;
}
#line 4063 "y.tab.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1704 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 1 ) default_p_step = atoi(yytext) ;
}
#line 4071 "y.tab.c" /* yacc.c:1646  */
    break;

  case 264:
#line 1709 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 1 ) default_p_maxlen = atoi(yytext) ;
}
#line 4079 "y.tab.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1714 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bigr = atoi(yytext) ;
}
#line 4087 "y.tab.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1719 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bigs = atoi(yytext) ;
}
#line 4095 "y.tab.c" /* yacc.c:1646  */
    break;

  case 267:
#line 1724 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bigrload = atoi(yytext) ;
}
#line 4103 "y.tab.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1729 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_rload = atoi(yytext) ;
}
#line 4111 "y.tab.c" /* yacc.c:1646  */
    break;

  case 269:
#line 1734 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( check_db_open(&Cdb) && atoi(yytext) >= 0 ) default_bign = atoi(yytext) ;
}
#line 4119 "y.tab.c" /* yacc.c:1646  */
    break;

  case 270:
#line 1739 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_debug = _inumber ;
  bss_set_debug(_q_debug) ;
}
#line 4128 "y.tab.c" /* yacc.c:1646  */
    break;

  case 271:
#line 1745 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_prefer_length = atoi(yytext) ;
}
#line 4136 "y.tab.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1750 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_accept_length = atoi(yytext) ;
}
#line 4144 "y.tab.c" /* yacc.c:1646  */
    break;

  case 273:
#line 1755 "i0_parse+.y" /* yacc.c:1646  */
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
#line 4194 "y.tab.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1804 "i0_parse+.y" /* yacc.c:1646  */
    {
  _tvalue = ( strcasecmp(yytext, "T") == 0 || strcasecmp(yytext, "Y") == 0 ||
	      strcasecmp(yytext, "TRUE") == 0 || strcasecmp(yytext, "YES") == 0 ) ;
}
#line 4203 "y.tab.c" /* yacc.c:1646  */
    break;

  case 275:
#line 1809 "i0_parse+.y" /* yacc.c:1646  */
    {
  _tvalue = ( atoi(yytext) != 0 ) ;
}
#line 4211 "y.tab.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1815 "i0_parse+.y" /* yacc.c:1646  */
    {
  _wvalue = 0 ;
}
#line 4219 "y.tab.c" /* yacc.c:1646  */
    break;

  case 277:
#line 1819 "i0_parse+.y" /* yacc.c:1646  */
    {
  _wvalue = atoi(yytext) ;
}
#line 4227 "y.tab.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1825 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_find_flags = default_ffl ;
  _q_lmask = default_lmask ;
  _limit_set = _q_set ;
}
#line 4237 "y.tab.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1832 "i0_parse+.y" /* yacc.c:1646  */
    {
  do_limit() ;
}
#line 4245 "y.tab.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1844 "i0_parse+.y" /* yacc.c:1646  */
    {
  _limit_set = _t_set ;
}
#line 4253 "y.tab.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1850 "i0_parse+.y" /* yacc.c:1646  */
    { 
  if ( _tvalue ) _q_find_flags &= ~FFL_NOSAVE ;
  else _q_find_flags |= FFL_NOSAVE ;
}
#line 4262 "y.tab.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1859 "i0_parse+.y" /* yacc.c:1646  */
    {
  yyerror("nothing to display") ;
}
#line 4270 "y.tab.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1864 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_lmask = default_lmask ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
}
#line 4279 "y.tab.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1872 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_display_setstats(_q_set, _OUT_BUF) ;
}
#line 4287 "y.tab.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1877 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_display_setstats(_t_set, _OUT_BUF) ;
}
#line 4295 "y.tab.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1881 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_display_setstats(_t_set, _OUT_BUF) ;
}
#line 4303 "y.tab.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1888 "i0_parse+.y" /* yacc.c:1646  */
    {
  int N  ;
  if ( default_bign > 0 ) N = default_bign ;
  else N = bss_get_big_n(_q_lmask) ;
  if ( N >= 0 ) {
    sprintf(_OUT_BUF, "%d\n", N) ;
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
#line 4317 "y.tab.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1899 "i0_parse+.y" /* yacc.c:1646  */
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
#line 4336 "y.tab.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1915 "i0_parse+.y" /* yacc.c:1646  */
    {
  int tnt ;
  tnt = bss_get_tnt(_q_attr, _q_lmask) ;
  if ( tnt >= 0 ) sprintf(_OUT_BUF, "%d\n", tnt) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
#line 4347 "y.tab.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1922 "i0_parse+.y" /* yacc.c:1646  */
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
#line 4373 "y.tab.c" /* yacc.c:1646  */
    break;

  case 295:
#line 1945 "i0_parse+.y" /* yacc.c:1646  */
    {
    bss_get_db_name(_OUT_BUF) ;
}
#line 4381 "y.tab.c" /* yacc.c:1646  */
    break;

  case 296:
#line 1950 "i0_parse+.y" /* yacc.c:1646  */
    {
  sprintf(_OUT_BUF, "%.3f\n", lastweight) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
#line 4390 "y.tab.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1956 "i0_parse+.y" /* yacc.c:1646  */
    {
  sprintf(_OUT_BUF, "%d\n", lastn) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
#line 4399 "y.tab.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1962 "i0_parse+.y" /* yacc.c:1646  */
    {
  sprintf(_OUT_BUF, "%d\n", last_ttf) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
#line 4408 "y.tab.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1980 "i0_parse+.y" /* yacc.c:1646  */
    {
  _stat_set = _q_set ;
  _q_mark = DEFAULT_MARK ;
  _q_bigr = default_bigr ;
  _q_find_flags = default_ffl ;
  _q_number = 0 ;
  _q_code = DEFAULT_TS ;
}
#line 4421 "y.tab.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1989 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_do_trec_stats(_OUT_BUF, _stat_set, _q_mark, _q_bigr,
		    _q_find_flags, _q_number, _q_code) ;
}
#line 4430 "y.tab.c" /* yacc.c:1646  */
    break;

  case 307:
#line 2000 "i0_parse+.y" /* yacc.c:1646  */
    {
  _stat_set = _t_set ;
}
#line 4438 "y.tab.c" /* yacc.c:1646  */
    break;

  case 308:
#line 2004 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_mark = _inumber ;
}
#line 4446 "y.tab.c" /* yacc.c:1646  */
    break;

  case 309:
#line 2008 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_bigr = _inumber ;
}
#line 4454 "y.tab.c" /* yacc.c:1646  */
    break;

  case 310:
#line 2012 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_number = _inumber ;
}
#line 4462 "y.tab.c" /* yacc.c:1646  */
    break;

  case 311:
#line 2016 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( _tvalue ) _q_find_flags |= FFL_MEM ;
  else _q_find_flags &=  ~FFL_MEM ;
}
#line 4471 "y.tab.c" /* yacc.c:1646  */
    break;

  case 312:
#line 2021 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_code = _inumber ;
}
#line 4479 "y.tab.c" /* yacc.c:1646  */
    break;

  case 313:
#line 2025 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( strcasecmp(yytext, "full") == 0 || strcasecmp(yytext, "long") == 0 )
    _q_code = TS_LONG ;
  else if ( strcasecmp(yytext, "short") == 0 || strcasecmp(yytext, "brief") == 0 )
    _q_code = TS_SHORT ;
}
#line 4490 "y.tab.c" /* yacc.c:1646  */
    break;

  case 314:
#line 2034 "i0_parse+.y" /* yacc.c:1646  */
    {
  sprintf(_OUT_BUF, "%s\n", bss_perror()) ;
  _OUT_BUF += strlen(_OUT_BUF) ;
}
#line 4499 "y.tab.c" /* yacc.c:1646  */
    break;

  case 315:
#line 2041 "i0_parse+.y" /* yacc.c:1646  */
    {
  prepare_error(UNSUPPORTEDCOMMAND, "tf", 0) ;
}
#line 4507 "y.tab.c" /* yacc.c:1646  */
    break;

  case 316:
#line 2048 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "PRSR: got DOCLENGTHTOKEN\n") ;
  strrncpy(_q_attr, default_attr, NNAME_MAX) ;
  _q_rec = _q_lastrec ;
}
#line 4517 "y.tab.c" /* yacc.c:1646  */
    break;

  case 317:
#line 2055 "i0_parse+.y" /* yacc.c:1646  */
    {
  int doclen = bss_get_doclen(&Cdb, _q_attr, _q_set, _q_rec ) ;
  if ( doclen >= 0 ) {
    sprintf(_OUT_BUF, "%d\n", doclen);
    _OUT_BUF += strlen(_OUT_BUF) ;
  }
}
#line 4529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 320:
#line 2069 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_set = _t_set ;
}
#line 4537 "y.tab.c" /* yacc.c:1646  */
    break;

  case 321:
#line 2073 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_rec = _inumber ;
  _q_set = -1 ;
}
#line 4546 "y.tab.c" /* yacc.c:1646  */
    break;

  case 322:
#line 2078 "i0_parse+.y" /* yacc.c:1646  */
    {
  _q_rec = _inumber ;
}
#line 4554 "y.tab.c" /* yacc.c:1646  */
    break;

  case 324:
#line 2087 "i0_parse+.y" /* yacc.c:1646  */
    {
  _dump_code = D_ALL ;
  _dump_set = _q_set ;
}
#line 4563 "y.tab.c" /* yacc.c:1646  */
    break;

  case 325:
#line 2093 "i0_parse+.y" /* yacc.c:1646  */
    {
  bss_dump_setrec(_dump_set, _OUT_BUF, _dump_code) ;
}
#line 4571 "y.tab.c" /* yacc.c:1646  */
    break;

  case 327:
#line 2100 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got dumpargs\n") ;
}
#line 4579 "y.tab.c" /* yacc.c:1646  */
    break;

  case 328:
#line 2107 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got dumparg\n") ;
  _dump_code = D_ERROR ;
}
#line 4588 "y.tab.c" /* yacc.c:1646  */
    break;

  case 329:
#line 2112 "i0_parse+.y" /* yacc.c:1646  */
    {
  if ( Dbg & D_PARSER ) fprintf(bss_syslog, "Prsr: got dump setnum\n") ;
  _dump_set = _t_set ;
}
#line 4597 "y.tab.c" /* yacc.c:1646  */
    break;

  case 330:
#line 2117 "i0_parse+.y" /* yacc.c:1646  */
    {
  _dump_code = D_TTF ;
}
#line 4605 "y.tab.c" /* yacc.c:1646  */
    break;

  case 331:
#line 2121 "i0_parse+.y" /* yacc.c:1646  */
    {
  _dump_code = D_NP ;
}
#line 4613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 332:
#line 2125 "i0_parse+.y" /* yacc.c:1646  */
    {
  _dump_code = D_ATTR ;
}
#line 4621 "y.tab.c" /* yacc.c:1646  */
    break;

  case 333:
#line 2131 "i0_parse+.y" /* yacc.c:1646  */
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
#line 4640 "y.tab.c" /* yacc.c:1646  */
    break;


#line 4644 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 2147 "i0_parse+.y" /* yacc.c:1906  */



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
     
