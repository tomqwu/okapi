/*   isdewey
     
     determines whether the portion of the string terminated
     by the first blank or end of field is a Dewey number
     ... Returns 0 if non-Dewey else the length of D.N. ...
     D.N. is nnn or nnn.nnn.....  also accepted is nnn. but a
     length of 3 is returned

 */

#include "defines.h"
#include "charclass.h"

int
isdewey(char *s, int l)  /* determines whether Dewey No within string */
{
  int i, length;
  length = 0;

  for ( i = 0; i < l; i++) {
   
    if ( tok_digit(*s) || *s == 'l' || *s == 'O' )  {
      if (length == 3) return(0); /* 4th element must be '.' */
      else {
	length++;
        s++;
      }
    }
    else {
           if ( length >  4 && *s == ' ') /* space terminating DN */
	     return(length);
      else if ( length == 4 && *s == ' ') /* nnn. taken as nnn */
	     return(--length);
      else if ( length < 3 )	/* min length of DN is 3 */
             return(0) ;
      else if (length == 3 && *s == '.') { /* 4th element of DN is . */
	     length++;
	     s++;
      }
      else if (length == 3 && *s == ' ')
	     return(length);
      else
	     return(0);		/* not DN */
    }
  }
  return((length>=3) ? length : 0);
}
