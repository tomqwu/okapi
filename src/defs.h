/*Copyright (c) 2008, City University London
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and
/or other materials provided with the distribution.
    * Neither the name of the City University London nor the names of its contributors may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY D
IRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS O
F USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING N
EGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
**  ok-test defs.h
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <stdlib.h>
#include <defines.h>
#include <math.h>
#include <errno.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define NONE_ASSIGNED -1

#define V_SMALL_BUFFER 256
#define SMALL_BUFFER 1024
#define BIG_BUFFER 10 * 1024
#define MAXSETS (16*1024)

#define MAX_FIELDS 32
#define MAX_INDEXES 32
#define MAX_TERM_LENGTH 256

#define WRITE 0
#define READ 1
#define APPEND 2

#define LEGAL_GSL "HFISGPN"
#define STOP_TERMS "FH"

