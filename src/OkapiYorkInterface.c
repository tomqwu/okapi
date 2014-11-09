#include <jni.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <stdlib.h>
#include <defines.h>
#include <math.h>
#include <errno.h>
#include <parms.h>
#include <bss_errors.h>

#define I0_OUTBUFFER 65536 // Length as per I0_OUTBUFSIZE

/*
 * Class:     OkapiYorkJNInterface
 * Method:    i0
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_OkapiYorkJNInterface_i0
  (JNIEnv *env, jobject jobj, jstring bss_command)
{
  char *bss_result = NULL; // Memory will be managed by okapi system
                           // for this result 

  // Input command buffer, for 'C' lang layer
  char buffer[I0_OUTBUFFER] = {'\0'};

  int rc = 0;  // return code

  /*
    Initialize okapi environment
   */
  //iinit ();
  /*
    Converts the string passed in from the java function
    to UTF-8 in C. We can now use the reference to the
    string pointer just like a regular char *.
	*/
  const char *str = (*env)->GetStringUTFChars(env, bss_command, NULL);
  
  /*
    If there was a memory allocation error, we would
    have NULL, so we check for it and return the NULL
    so that the JNI exception will be thrown.
	*/
  if (str == NULL)
  {
     return NULL;
  }

  // Copy the value of the string into our buffer
  // Length of the string but be STRICTLY less than the size
  // of our buffer so that we have the null terminator in tact.
  if ( strlen(str) < sizeof(buffer))
  {
    sprintf(buffer, "%s", str);
  }
  else
  {
    printf ("Command string is too long. Length:%d, Maximum:%d\n",
              strlen(str), sizeof(buffer) - 1);
    // Expect JNI layer to choke here - crude way of error handling :)
    return NULL;
  }

  // Call the okapi API
  rc = i00(buffer, &bss_result);
  // CANT_READ_SET is a special error that's handled by compassIR.
  // This error happens for some *strange* reason when performing
  // the find command the final time. Sometimes, a certain set
  // can not be read. This is most likely due to the distributed
  // database mechanism currently in use.
  // CompassIR will handle this by continuing to remove sets from
  // the find command until the command works again. Its a crude
  // workaround, but its better than logging messages here
  // all the time!
  // The result for CANT_READ_SET ends up being an empty
  // string.
  if (rc && rc != NO_SUCH_RECORD && rc != CANT_READ_SET)
  {
      printf("Error running command : %d\n", rc);
      printf(" Result: %s\n", bss_result);
  } 

  /* Call the Release function to make sure that memory 
     used by the Get method above is now freed. Otherwise,
     it can lead to a memory leak. 
  */
  (*env)->ReleaseStringUTFChars (env, bss_command, str);

  // Return the value made up of the bss_result variable
  return (*env)->NewStringUTF(env, bss_result);
}

/*
 * Class:     OkapiYorkJNInterface
 * Method:    iinit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_OkapiYorkJNInterface_iinit
  (JNIEnv * env, jobject obj)
{
  iinit();
  return;
}

/*
 * Class:     OkapiYorkJNInterface
 * Method:    iexit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_OkapiYorkJNInterface_iexit
  (JNIEnv *env, jobject obj)
{
  iexit();
  return;
}
