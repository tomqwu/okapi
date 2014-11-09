#define HILO 0
#define NO_FSEEKO
#define NO_HASRLIMIT
#define NO_MMAP
#define NO_ZERO
#define NO_DF
#define NO_LN
#define NO_SIGSTOP
#define NO_SETITIMER
#define NO_PERMS
#define NO_TRUNCATE
#define getpagesize() 4096
#define HAVE_LL
#define LL_TYPE __int64
#define OFFSET_TYPE __int64
#undef DIR_ADDR_TYPE
#define DIR_ADDR_TYPE OFFSET_TYPE
#define LARGEFILES
#define atoll(x) _atoi64(x)
#define D_C "\\"
#define SLASH_CHAR '\\'
#include "win32extras.h"
