/*****************************************************************************

  site.h Jan 92

  Okapi include file with site-specific definitions
  Included by defines.h

*****************************************************************************/

/*
#define OKAPI "/home/okapi"
#define CONTROL_DIR "/home/okapi/parms/"
#define SYSTEM_TEMPDIR "/tmp/"
#define TEMP_DIR "/tmp/" 
#define BIB_DIR "/home/okapi/docs"
#define LOCALBIB_DIR "."
#define MAXFILES 125
#define BSS_WT_PWR

#define MACHINE "m.i386-linux.lf64.h"
*/

/* #define I0_OUTBUFSIZE 8 * 1024 * 1024  // need to reduce the size of buffer for speed */
/* mko Define I0_OUTBUFSIZE to be 200 KB */
#define I0_OUTBUFSIZE 65536
#define OKAPI "/home/okapi/okapi-2.4.1/"
#define CONTROL_DIR "/home/okapi/okapi-2.4.1/databases/"
#define SYSTEM_TEMPDIR "/tmp/"
#define TEMP_DIR "/tmp/" 
#define BIB_DIR "/home/okapi/okapi-2.4.1/bibfiles"
#define LOCALBIB_DIR "."
#define MAXFILES 125
#define BSS_WT_PWR

#define MACHINE "m.i386-linux.lf64.h"

