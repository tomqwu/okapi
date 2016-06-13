
#include "relex.h"
#include "i1_defs.h"
#include "bss.h"
//extern int main1();









JNIEXPORT void JNICALL Java_relex_javainit
  (JNIEnv * xxb, jobject xxa){
//char *command, *results ;

//main1();
iinit();
//i00("ch med05.sample",&results);
//i00("f t=@0110",&results);
//i00("s r=0",&results);
//printf(results);
}

JNIEXPORT jstring JNICALL Java_relex_comm
  (JNIEnv * xxb, jobject xxa, jstring command1){
	const char *str = (*xxb)->GetStringUTFChars(xxb,command1,0);
char * results;
char buf[30000];
sprintf(buf, "%s", str) ;	
i00(buf,&results);
(*xxb)->ReleaseStringUTFChars(xxb,command1,str);
return (*xxb)->NewStringUTF(xxb,results);


}


