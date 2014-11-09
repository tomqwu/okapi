/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

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
