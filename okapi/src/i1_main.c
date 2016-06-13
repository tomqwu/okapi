/* main routine for interface */

#define EXTERN
#include "i1_defs.h"
#include "bss.h"

extern FILE *Err_file, *stdnothing ;

char usage[] = \
"Usage: %s [-silent] [-exit_on_error|-e] [-f[lush]]\n\
           -v[ersion]\n\
           -h[elp]\n\
  -silent eliminates the user prompt \"U: \"\n\
  -flush flushes output after every command\n\
  -exit_on_error causes an exit(1) if the BSS returns an error code\n\
  -version outputs the BSS version number and date\n" ;

static void cleanup(int code)
{
	iexit();
	exit(code);
}


static void initialise(void)
{
	iinit();
#ifndef _WIN32
	signal(SIGHUP, cleanup);
	signal(SIGQUIT, cleanup);
	signal(SIGINT, cleanup);
#endif
}

int
read_line_input(char *buf)
{
  int l = 0 ;
  int backslash = FALSE;

  while ( (*buf = getchar()) != EOF ) {
    if (*buf == '\\')
      backslash = TRUE;
    else {
      if (*buf == '\n') {
	if (backslash != TRUE)
	  break;
      }
      else {
	buf++; l++ ;
      }
      backslash = FALSE;
    }
  }
  *buf = '\0';
  return l ;
}

int
main(int argc, char *argv[])
{

  int j ;
  int silent = 0 ;
  int flush = 0 ;
  int result = 0 ;
  char *envp ;
  char *p ;
  int db_flags ;
  int exit_on_error = FALSE ;
  FILE *outf = stdout ;
  char *user_prompt = "U:";
  char *command, *results ;

  /* Initialise BSS first as this does env (May 98) */
  /* Can for instance override BSS_PARMPATH with a -c (below) */
  initialise();
  /* Database etc should also be done here so could be overridden
     by command line args if nec, but can't be bothered now */

  for ( j = 1 ; j < argc ; j++ ) {
    if ( strcmp(argv[j], "-silent") == 0 ) silent = 1 ;
    else if ( strcmp(argv[j], "-c") == 0 ) Control_dir = argv[++j] ;
    else if ( strcmp(argv[j], "-e") == 0 ||
	     strcmp(argv[j], "-exit_on_error") == 0 )
      exit_on_error = TRUE ;
    else if ( strcmp(argv[j], "-h") == 0 ||
	     strcmp(argv[j], "-help") == 0 ) {
      fprintf(stderr, usage, argv[0]) ;
      exit(1) ;
    }
    else if ( strcmp(argv[j], "-f") == 0 || strcmp(argv[j], "-flush") == 0 ) {
      flush = 1 ;
    }
    else if ( strcmp(argv[j], "-v") == 0 ||
	      strcmp(argv[j], "-version") == 0 ) {
      sprintf(Command, "info version\n") ;
      result = i00(Command, &results) ;
      fprintf(outf, results) ;
      cleanup(0) ;
    }
    else {
      fprintf(stderr, "Unrecognized arg %s\n", argv[j]) ;
      fprintf(stderr, usage, argv[0]) ;
      exit(1) ;
    }
  }

  /* Do environment (May 30 95, ...) */
  /* NB. All environment vars except BSS_DEBUG are ignored unless BSS_DB
     is set and the database is successfully opened */
  if ( (envp = getenv("BSS_DEBUG")) ) {
    if ( atoi(envp) > 0 ) {
      sprintf(Command, "set debug=%d\n", atoi(envp)) ;
      /* result = i0(Command, Results) ;*/
      result = i00(Command, &results) ;
    }
  }
  if ( (envp = getenv("BSS_DB")) && *envp != '\0' ) {
    p = envp ;
    if ( (envp = getenv("BSS_DB_FLAGS")) && *envp != '\0' ) {
      db_flags = atoi(envp) ;
      sprintf(Command, "choose dbflags=%d %s\n", db_flags, p) ;
    }
    else sprintf(Command, "choose %s\n", p) ;
    result = i00(Command, &results) ;
    if ( result < 0 ) {
      fprintf(Err_file, "%s\n", bss_perror()) ;
      if ( exit_on_error ) cleanup(1) ;
    }
    else {
      if ( (envp = getenv("BSS_ATTRIBUTE")) && *envp != '\0' ) {
	sprintf(Command, "set a=%s\n", envp) ;
	result = i00(Command, &results) ;
	if ( result < 0 ) {
	  fprintf(Err_file, "%s\n", bss_perror()) ;
	  if ( exit_on_error ) cleanup(1) ;
	}
      }
      /*
      if ( (envp = getenv("BSS_LIMIT")) && *envp != '\0' ) {
	sprintf(command, "set l=%s\n", envp) ;
	result = i00(command, &results) ;
	if ( result < 0 ) {
	  fprintf(Err_file, "%s\n", bss_perror()) ;
	  if ( exit_on_error ) cleanup(1) ;
	}
      }
      */
    }
  }
  while(!feof(stdin)) {
    command = Command ; /*results = Results ;*/
    *command /* = *results */ = 0;
    
    if ( ! silent ) printf("%s ", user_prompt);
    read_line_input(command);
    command = slws(command) ;
    if (!feof(stdin)) {
      if ( command[0] == '!' ) {
	system(slws(command + 1)) ;
	continue ;
      }
      if (!strcasecmp(command,"exit") ||
	  !strcasecmp(command,"quit") ||
	  !strcasecmp(command,"q") ||
	  !strcasecmp(command,"stop"))
	break;
    }
    if ( !strncasecmp(command, "output=", 7) ) {
      char *p = command + 7 ;
      FILE *oldoutf = outf ;
      if ( strcmp(p, "0") == 0 ) outf = stdnothing ;
      else if ( strcmp(p, "1") == 0 ) outf = stdout ;
      else if ( strcmp(p, "2") == 0 ) outf = Err_file ;
      else {			/* Assume file name */
	outf = fopen(p, "ab") ;
	if ( outf == NULL ) {
	  fprintf(stderr,
		  "Can't write (append) to %s, output unchanged\n", p) ;
	  outf = oldoutf ;
	}
	else if ( oldoutf != NULL && oldoutf != stdnothing  &&
		  oldoutf != stdout && oldoutf != stderr &&
		  oldoutf != Err_file )
	  /* (added Err_file 2 Apr 01) */
	  fclose(oldoutf) ;
      }
      continue ;
    }
    else if (! strncasecmp(command, "echo ", 5) ) {
      char *p = command + 5 ;
      fprintf(outf, "%s\n", p) ;
    }
    else if (! strcasecmp(command, "silent") ) {
      silent = 1 ;
    }

    /* else send the command to the bss */
    else {
      result = i00(command, &results) ;
      /* If the command was CHOOSE see if BSS_ATTRIBUTE is set (Sep 7 2000) */
      if ( (!strncasecmp(command, "choose ",7) ||
	    !strncasecmp(command, "ch ",3)) && result == 0 ) {
	if ( (envp = getenv("BSS_ATTRIBUTE")) && *envp ) {
	  sprintf(command, "set attr=%s\n", envp) ;
	  result = i00(command, &results) ;
	}
      }
      if ( result >= 0 ) { /* Doesn't display warnings, only errors */
	if ( fprintf(outf, "%s", results) < 0 ) {
	  fprintf(stderr, "Can't write output\n") ;
	  continue ;
	}
      }
      else {			/* (result < 0) */
	if ( exit_on_error ) {
	  fprintf(stderr, "%s\n", bss_perror()) ;
	  cleanup(1) ;
	}
	else printf("%s\n", bss_perror()) ;
      }
    } /* command was sent to i00() */
    if ( flush ) fflush(outf) ;
  }
  cleanup(0);
  /*NOTREACHED*/
}


