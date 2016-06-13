#define HILO 0
#define HAVE_LL
#define OFFSET_TYPE off_t
#define LL_TYPE long long
/*#define DIR_ADDR_TYPE LL_TYPE*/
#define DIR_ADDR_TYPE off_t
#define NO_STATVFS
#define O_BINARY 0
#define D_C "/"
#define SLASH_CHAR '/'

#define LARGEFILES
#define _FILE_OFFSET_BITS 64

#define _GNU_SOURCE

/*#include <features.h>*/
#include <unistd.h>
#include <sys/time.h>
