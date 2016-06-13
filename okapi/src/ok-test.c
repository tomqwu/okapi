/*
**     OkapiYorkInterface.c
**
**     Adapted for OkapiYork JNI interface to the okapi C api.
**
**     Originally provided by:
**     mg:   CISR, City University, London EC1V 0HB.   April 1998
**     Example C program to do a search on single terms read
**     in from a file.
**
**     The program is meant only as an input to the 
**     The program is called as:
**
**         ok-test   <database_name>   <term_filename>
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

/*
**     #define various macros
*/

#define TRUE 1
#define FALSE 0
#define NULL 0

#define NONE_ASSIGNED -1

#define V_SMALL_BUFFER 256
#define SMALL_BUFFER 1024
#define BIG_BUFFER 10 * 1024

#define MAX_FIELDS 32
#define MAX_INDEXES 32

#define LEGAL_GSL "HFISGPN"
#define STOP_TERMS "FH"

/*
**     Structure to hold parameter file data
*/

struct params_file {
    char name[V_SMALL_BUFFER];
    int lastbibvol;
    char bib_basename[V_SMALL_BUFFER];
    char bib_dir[V_SMALL_BUFFER];
    int bibsize;
    int real_bibsize;
    char display_name[V_SMALL_BUFFER];
    char explanation[V_SMALL_BUFFER];
    int nr;
    int nf;
    char f_abbrev[V_SMALL_BUFFER][MAX_FIELDS];
    int rec_mult;
    int fixed;
    char db_type[V_SMALL_BUFFER];
    int has_lims;
    int maxreclen;
    int ni;
    int last_ixvol[MAX_INDEXES];
    char ix_stem[V_SMALL_BUFFER][MAX_INDEXES];
    int ix_volsize[MAX_INDEXES];
    int ix_type[MAX_INDEXES];
    int para_file;
} params_file;

/*
**     Function to leave the BSS cleanly
*/

void quit_prog() {
    iexit();
    exit(0);
}

/*
**     malloc_error Generates an error message if malloc()
**     fails and terminates the program. The program/function
**     and buffer name/size are passed to the function.
*/

void malloc_error ( char *program, char *function, char *buffer, int buffer_size) {

    fprintf(stderr, "%s:: Failed to allocate [%d] bytes for %s\n", function, buffer_size, buffer);
    quit_prog();

}

/*
**     terminate_run() displays the BSS error/warning by calling the function i0()
**     If an error the program is terminated.
*/

void terminate_run ( char *calling_fcn, char *command, int return_code) {

    char bss_command[SMALL_BUFFER];
    char bss_results[SMALL_BUFFER];

    sprintf(bss_command, "perror");

    i0(bss_command, bss_results);

    fprintf(stderr, "Calling function [%s]\n\n", calling_fcn);

    if (return_code < 0) {
        fprintf(stderr, "There was an ERROR executing the command:\n\n");
    }
    else {
        fprintf(stderr, "There was a WARNING executing the command:\n\n");
    }

    fprintf(stderr, "%s\n\n", command);
    fprintf(stderr, "BSS error: [%s] (%d)\n", bss_results, return_code);
    fprintf(stderr, "BSS_PARMPATH = [%s]\n", getenv("BSS_PARMPATH"));

    if (return_code < 0) {
        fprintf(stderr, "The program run will be terminated.\n");
        quit_prog();
    }
}

/*
**     Function to check if there is a paragraph file for the
**     database. This is not used in this program but is needed
**     if you wish to include code for passage retrieval.
*/

int check_for_paragraph_file () {
    int buffer_size;

    char *para_file;
    FILE *pf;

    /*
    **     set buffer size large enough for filename
    */

    buffer_size = strlen(params_file.bib_dir) + strlen(params_file.name) + 16;

    if ((para_file = ( char *) malloc(buffer_size)) == NULL) {
        malloc_error("utilities.c", "check_for_paragraph_file()",
                                "para_file", buffer_size);
    }

    sprintf(para_file,
                    "%s/%s.par",
                    params_file.bib_dir, params_file.name);

    if ((pf = fopen(para_file, "r")) == NULL) {
        params_file.para_file = FALSE;
    }
    else {
        fclose(pf);

        sprintf(para_file, "%s/%s.pardir", params_file.bib_dir, params_file.name);

        if ((pf = fopen(para_file, "r")) == NULL) {
            params_file.para_file = FALSE;
        }
        else {
            fclose(pf);

            params_file.para_file = TRUE;
        }
    }

    free(para_file);
}


/*
**     Read database parameter files. The parameter information is not
**     used by this program but is needed for reading the header
**     information displayed by a show with show format 259.
*/

void read_parameter_files ( char *database_name) {
    int length;

    int field_no = 0;
    int index_no = 0;

    char parameter[SMALL_BUFFER];
    char value[SMALL_BUFFER];

    char db_params_file[SMALL_BUFFER];
    FILE *pf;

    char temp[SMALL_BUFFER];
    char *tbuf;

    sprintf(db_params_file, "%s/%s", getenv("BSS_PARMPATH"), database_name);

    if ((pf = fopen(db_params_file, "r")) == NULL) {
        fprintf(stderr, "Can't open parameter file [%s]\n", db_params_file);
        exit(-1);
    }
    else {
        while (!feof(pf)) {
            if (fgets(temp, SMALL_BUFFER, pf) == NULL) {
                break;
            }
            else {
                temp[strlen(temp) - 1] = '\0';
                tbuf = temp;
               
                length = strchr(tbuf, '=') - tbuf;
                                       
                memcpy(parameter, tbuf, length);
                parameter[length] = '\0';
                tbuf += length + 1;
                sprintf(value, "%s", tbuf);
               
                if (strcmp(parameter, "name") == 0) {
                    sprintf(params_file.name, "%s", value);
                }
                else if (strcmp(parameter, "lastbibvol") == 0) {
                    params_file.lastbibvol = atoi(value);
                }
                else if (strcmp(parameter, "bib_basename") == 0) {
                    sprintf(params_file.bib_basename, "%s", value);
                }
                else if (strcmp(parameter, "bib_dir") == 0) {
                    sprintf(params_file.bib_dir, "%s", value);
                }
                else if (strcmp(parameter, "bibsize") == 0) {
                    params_file.bibsize = atoi(value);
                }
                else if (strcmp(parameter, "real_bibsize") == 0) {
                    params_file.real_bibsize = atoi(value);
                }
                else if (strcmp(parameter, "display_name") == 0) {
                    sprintf(params_file.display_name, "%s", value);
                }
                else if (strcmp(parameter, "explanation") == 0) {
                    sprintf(params_file.explanation, "%s", value);
                }
                else if (strcmp(parameter, "nr") == 0) {
                    params_file.nr = atoi(value);
                }
                else if (strcmp(parameter, "nf") == 0) {
                    params_file.nf = atoi(value);
                }
                else if (strcmp(parameter, "f_abbrev") == 0) {
                    sprintf(params_file.f_abbrev[index_no++], "%s", value);
                }
                else if (strcmp(parameter, "rec_mult") == 0) {
                params_file.rec_mult = atoi(value);
                }
                else if (strcmp(parameter, "fixed") == 0) {
                    params_file.fixed = atoi(value);
                }
                else if (strcmp(parameter, "db_type") == 0) {
                    sprintf(params_file.db_type, "%s", value);
                }
                else if (strcmp(parameter, "has_lims") == 0) {
                    params_file.has_lims = atoi(value);
                }
                else if (strcmp(parameter, "maxreclen") == 0) {
                    params_file.maxreclen = atoi(value);
                }
                else if (strcmp(parameter, "ni") == 0) {
                    params_file.ni = atoi(value);
                }
                else if (strcmp(parameter, "last_ixvol") == 0) {
                    params_file.last_ixvol[index_no] = atoi(value);
                }
                else if (strcmp(parameter, "ix_stem") == 0) {
                    sprintf(params_file.ix_stem[index_no], "%s", value);
                }
                else if (strcmp(parameter, "ix_volsize") == 0) {
                    params_file.ix_volsize[index_no] = atoi(value);
                }
                else if (strcmp(parameter, "ix_type") == 0) {
                    params_file.ix_type[index_no++] = atoi(value);
                }
            }
        }
       
        fclose(pf);
    }

    check_for_paragraph_file();

    fprintf(stderr,
                    "read_parameter_files() :: database = [%s] -- ", database_name);

    if (!params_file.para_file) {
        fprintf(stderr, "NO PARAGRAPH FILE.\n");
    }
    else {
        fprintf(stderr, "PARAGRAPH FILE EXISTS.\n");
    }       
}

/*
**     open the database whose name is passed to the program.
*/

int open_database ( char *database_name) {
    int database_open = FALSE;
    int return_code;

    char bss_command[SMALL_BUFFER];
    char bss_result[SMALL_BUFFER];
    char *rbuf;

    fprintf(stderr, "open_database():: Open \"%s\" -- ", database_name);

    sprintf(bss_command, "choose %s", database_name);

    if (database_open = (i0(bss_command, bss_result) == 0)) {
        fprintf(stderr, "OK.\n");
        read_parameter_files(database_name);
    }
    else {
        fprintf(stderr, "ERROR.\n");
    }

    return database_open;
}

void initialise_bss () {
    char *DISPLAY_VAR = NULL;

    /*
    **     Initialise the BSS
    */

    iinit();
}


/*
**     Calculate the Robertson Spark-Jones F4 predictive weight with halves
**     for each term.
**
**     Each term is given a loading using rload=4 and bigrload=5.
*/

double calc_wgt ( int np) {
    int return_code;

    int r = 0;
    int bigr = 0;
    int rload = 4;
    int bigrload = 5;
    int weight_function = 2;

    double weight;

    char bss_command[SMALL_BUFFER];
    char bss_result[SMALL_BUFFER];
    char *rbuf;

    /*
    **     Calculate weight, returned as a double.
    */

    sprintf(bss_command,
                    "w fn=%d r=%d bigr=%d rload=%d bigrload=%d n=%d",
                    weight_function, r, bigr, rload, bigrload, np);

    if ((return_code = i0(bss_command, bss_result)) < 0) {
        terminate_run("calc_wgt()", bss_command, return_code);
    }

    weight = atof(bss_result);

    return weight;
}

/*
**     do_search()
**
**     The terms are read from the file passed as the second parameter
**     to the program. Each line of this file may contain one or more
**     terms.
**
**     The function returns the BSS document set number (*docset) and
**     the number of postings (*npostings) for the bestmatch search
**     on all of the terms. The bestmatch operator BM25 is used.
*/

int do_search ( char *term_file, int *docset, int *npostings) {
    /*
    ** no. of terms per line, determined by the superparse (sp) function
    */

    int no_terms;

    /*
    **     stem, gsl_class & source for each term as returned by the "sp" command
    */

    char stem[SMALL_BUFFER];
    char gsl_class;
    char source[SMALL_BUFFER];

    /*
    **     BSS set, no. postings and weight for each non-stop term
    */

    int set;
    int np;
    double weight;

    /*
    **     variables used in parsing the result of "sp"
    */

    int current_term;
    int length;
    int chars_read;

    /*
    **     value returned by i0()
    */

    int return_code;

    /*
    **     buffers for bss commands and results
    */

    char bss_command[SMALL_BUFFER];
    char bss_result[SMALL_BUFFER];
    char *result_buffer;

    char find_command[SMALL_BUFFER];
    char find_result[SMALL_BUFFER];
    char *find_buffer;

    char search_command[SMALL_BUFFER];
    char search_result[SMALL_BUFFER];
    char *search_buffer;

    char error_message[SMALL_BUFFER];

    /*
    **     buffer for reading lines of file into
    */

    char temp[SMALL_BUFFER];

    FILE *tf;

    /*
    **     Open file of terms for reading; quit if there's an error.
    */

    if ((tf = fopen(term_file, "r")) == NULL) {
        fprintf(stderr,
                        "do_search():: Error opening \"%s\" for reading.",
                        term_file);
        quit_prog();
    }

    /*
    **     Initialise "search_command".
    **
    **     "f" is the minimum abbreviation for the BSS command "find"
    */

    sprintf(search_command, "f");

    /*
    **     Read file of terms
    */

    while (!feof(tf)) {
        if (fgets(temp, SMALL_BUFFER, tf) == NULL) {
            break;
        }
        else {
            temp[strlen(temp) - 1] = '\0';

            /*
            ** superparse the line
            */

            sprintf(bss_command, "sp t=%s", temp);

            fprintf(stderr, "do_search() - command = [%s]\n", bss_command);

            if ((return_code = i0(bss_command, bss_result)) != 0) {
                terminate_run("main()", bss_command, return_code);
            }

            /*
            **     "bss_result" will always end in a char. Removing
            **     this is not necessary, it just stops an extra
            **     character being printed by the fprintf() statement.
            */

            bss_result[strlen(bss_result) - 1] = '\0';

            fprintf(stderr, "do_search() - result = [%s]\n", bss_result);

            result_buffer = bss_result;
       
            sscanf(result_buffer, "%d %n", &no_terms, &chars_read);
            result_buffer += chars_read;
           
            fprintf(stderr, "do_search() - no_terms = %d\n", no_terms);

            /*
            **     Now extract component terms (parsed, gsl_class & source)
            */

            for (current_term = 0; current_term < no_terms; current_term++) {
                result_buffer += 2;
                length = strstr(result_buffer, " c=") - result_buffer;
                memcpy(stem, result_buffer, length);
                stem[length] = '\0';

                fprintf(stderr, "do_search() - stem = [%s], ", stem);

                result_buffer += length + 1;
                sscanf(result_buffer, "c=%c s=%n", &gsl_class, &chars_read);

                fprintf(stderr, "gsl = [%c], ", gsl_class);

                result_buffer += chars_read;
               
                if (current_term < no_terms - 1) {
                    length = strstr(result_buffer, " t=") - result_buffer;
                    memcpy(source, result_buffer, length);
                    source[length] = '\0';
                    result_buffer += length + 1;
                }
                else {
                    length = strlen(result_buffer) - 1;
                    sprintf(source, "%s", result_buffer);
                    source[length] = '\0';
                }

                fprintf(stderr, "source = [%s]\n", source);

                /*
                **     First check for stop terms. STOP_TERMS defined in defs.h
                */
               
                if (strchr(STOP_TERMS, gsl_class) == NULL) {
                    sprintf(find_command, "f t=%s", stem);

                    fprintf(stderr, "do_search() - find = [%s]\n", find_command);

                    if ((return_code = i0(find_command, find_result)) != 0) {
                        terminate_run("main()", find_command, return_code);
                    }

                    /*
                    **     "find_result" will always end in a char. Removing
                    **     this is not necessary, it just stops an extra
                    **     character being printed by the fprintf() statement.
                    */

                    find_result[strlen(find_result) - 1] = '\0';

                    fprintf(stderr, "do_search() - = [%s]\n", find_result);

                    find_buffer = find_result;

                    sscanf(find_buffer, "S%d np=%d", &set, &np);

                    if (np > 0) {
                        /*
                        **     The term exists. Determine weight then append
                        **     set and weight values as:
                        **
                        **     s= w=
                        **
                        **     onto the search_command string
                        */

                        weight = calc_wgt(np);

                        fprintf(stderr, "do_search():: weight = %.3f\n", weight);

                        sprintf(search_command + strlen(search_command), " s=%d w=%.3f", set, weight);
                    }
                    else {
                        /*
                        **     Not an indexed term.
                        **     Delete the set; we don't need it.
                        */

                        sprintf(bss_command, "delete %d", set);

                        if ((return_code = i0(bss_command, bss_result)) != 0) {
                            terminate_run("term_input()", bss_command, return_code);
                        }
                    }
                }
            }
        }
    }

    if (strlen(search_command) > 1) {
        /*
        **     search_command was initialised by adding an 'f' to it.
        **     If strlen(search_command) > 1 then we must have added
        **     at least one set and its weight.
        **
        **     Append the bestmatch operator onto "search_command"
        */

        sprintf(search_command + strlen(search_command), " op=bm25");

        fprintf(stderr, "do_search() - command = [%s]\n", search_command);

        if ((return_code = i0(search_command, search_result)) != 0) {
            terminate_run("term_input()", search_command, return_code);
        }

        fprintf(stderr, "do_search() - result = [%s]\n", search_result);

        search_buffer = search_result;

        sscanf(search_buffer, "S%d np=%d", docset, npostings);
    }
    else {
        *docset = NONE_ASSIGNED;
        *npostings = NONE_ASSIGNED;
    }
}

/*
**     Show the first from the resultant set
*/

void do_show ( int set, int no_docs) {
    int next;
    int show_format = 3;
    int return_code;

    char bss_command[SMALL_BUFFER];
    char bss_result[BIG_BUFFER];

    for (next = 0; next < no_docs; next++) {
        sprintf(bss_command, "s s=%d r=%d", set, next);

        if ((return_code = i0(bss_command, bss_result)) != 0) {
            terminate_run("main()", bss_command, return_code);
        }

        fprintf(stderr, "%s\n", bss_result);

        fprintf(stderr, "%s\n\n", bss_result);
    }
}

int main ( int argc, char *argv[]) {
    int db_open = FALSE;
    char database_name[V_SMALL_BUFFER];

    int buffer_size;
    int docset;
    int npostings;

    int docs_to_show;

    char term_file[SMALL_BUFFER];

    /*
    **     There must be two command line arguments passed in the order:
    **
    **         database_name, terms file
    */

    if (argc != 3) {
        fprintf(stderr, "ERROR: Insufficient arguments passed to program.");
        quit_prog();
    }

    sprintf(database_name, "%s", argv[1]);
    sprintf(term_file, "%s", argv[2]);

    fprintf(stderr, "main(\"%s\", \"%s\")\n", database_name, term_file);

    /*
    **     initialise_bss() initialises the BSS
    */

    initialise_bss();
    db_open = open_database(database_name);

    if (db_open) {
        printf("Okapi initialisation complete.\n");

        do_search(term_file, &docset, &npostings);

        if ((docset != NONE_ASSIGNED) && (npostings > 0)) {
            fprintf(stderr, "Search OK: set = %d, np = %d\n", docset, npostings);

            docs_to_show = (npostings > 5) ? 5 : npostings;

        do_show(docset, docs_to_show);
        }
        else {
            fprintf(stderr, "ERROR: Quitting.\n");
        }
    }
    else {
        fprintf(stderr, "ERROR opening [%s]\n", database_name);
    }

    quit_prog();
} 
