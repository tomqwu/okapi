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

/*****************************************************************
 * xml_path.c
 * index inex xml path By Lu Wei July 2005
*/

#include <stdio.h>
#include <stdlib.h>
#include <expat.h>
#include <string.h>
#include <time.h>

#define  BUFFSIZE 8192
#define  XMLMAXSTACKSIZE 512
#define  XMLMAXTAGNO 20000

// THE FOLLOWING DEFINITION WAS FOR DETERMINE THE NUMBER OF THE NODE. EG: article[1]/bdy[1]/sec[2]
#define XMLMAXDEPTH 20
#define XMLMAXNODE 100
char xml_node[XMLMAXTAGNO][200] ;
char temp_node[50] ;
char node[XMLMAXDEPTH][XMLMAXNODE][20] ;
int node_count[XMLMAXDEPTH][XMLMAXNODE] ;
int node_num[XMLMAXDEPTH] ;
int cur_depth ;

// XML_Parser for parsing the xml file.
XML_Parser p;

struct stack_elem { 
  int field_index; 
  char name[20];
  union { 
    int field_offset; 
#if defined(HAVE_LL) 
    LL_TYPE rec_offset; 
#endif 
  }off; 
}; 
  
struct stack_elem xml_stack[XMLMAXSTACKSIZE]; 
int stack_top = -1 ;
int field_index = 0 ;
int inside_article = 0;
int rec_offset ;

char xml_path[XMLMAXTAGNO][200], xml_temp_path[200], xml_root[XMLMAXTAGNO][200];
int xml_tag[XMLMAXTAGNO][3], xml_root_off[XMLMAXTAGNO][7], xml_temp_tag[3] ;
int xml_tag_no = 0, xml_tag_root = 0, rec_flag=0, root_flag = 0, rec_num[20];

char record_tag_name[100] ;
int record_tag_depth ;

char Buff[BUFFSIZE];

int Depth = 0;
FILE *f_input, *f_el_offset, *f_rec_offset, *f_path_ix, *f_path_id;
char fname_input[100], fname_el[100], fname_rec[100], fname_path_ix[100], fname_path_id[100] ;

int XMLPush(struct stack_elem stack_item);
struct stack_elem XMLPop();
static void start(void *data, const char *el, const char **attr);
static void end(void *data, const char *el);
int sort_root(int num);
int sort_path_tag(int numbers[][3], int left, int right) ;
int display() ;

/* functions to manage elements stack */
int XMLPush(struct stack_elem stack_item)
{
  if(stack_top >= XMLMAXSTACKSIZE-1) {
    printf("Reached maximum stack size while parsing xml\n");
    return (-1);
  }
  xml_stack[++stack_top] = stack_item;

  return 0;
}

struct stack_elem XMLPop()
{
  struct stack_elem dummy_elem;

  if(stack_top == -1) {
    dummy_elem.field_index = -1;
    return dummy_elem;;
  }
  return xml_stack[stack_top--];
}

static void 
start(void *data, const char *el, const char **attr)
{
  int i, j;
  struct stack_elem current_el;
  
  //for (i = 0; i < Depth; i++)
  //  printf("  ");

  //printf("%d ", XML_GetCurrentByteIndex(p)) ;
  //printf("%s", el);
  /*if (rec_flag == 10056)
  for (i = 0; attr[i]; i += 2) {
    printf(" %s='%s'", attr[i], attr[i + 1]);
  }*/
  //printf("\n");

  /*
  if (((Depth == record_tag_depth - 1)&&(!strcmp(el, "article"))) || ((Depth>=)&&(inside_article==1)))
  //if ((Depth == 1) || ((Depth>1)&&(inside_article==1)))
  {
    inside_article = 1 ;
    current_el.field_index = field_index++ ;
    current_el.off.field_offset = XML_GetCurrentByteIndex(p) ;
	if (Depth == 2) rec_offset = current_el.off.field_offset ;
    strcpy(current_el.name, el) ;
    XMLPush(current_el) ;
    
    if (Depth==2) {
      for (i=0; i<XMLMAXDEPTH; i++) {
        for (j=0; j<XMLMAXNODE; j++)
          node_count[i][j] = 0 ;
        node_num[i] = 0 ;
      }
      
      cur_depth = Depth ;
    }
    else {
      for (i=node_num[Depth-3]-1; i>=0; i--)
        if (strcmp(node[Depth-3][i], el)==0) {
          strcpy(node[Depth-3][node_num[Depth-3]], el) ;
          node_count[Depth-3][node_num[Depth-3]] = node_count[Depth-3][i] + 1 ;
          node_num[Depth-3]++ ;
          break ;
        }  
        
      if (i==-1) {
        strcpy(node[Depth-3][node_num[Depth-3]], el) ;
        node_count[Depth-3][node_num[Depth-3]] = 0 ;
        node_num[Depth-3]++  ;
      }
      node_num[Depth-3 + 1] = 0 ;
    }      
  }
  else inside_article = 0 ;

  Depth++;
  */

  Depth++;

  if (((Depth == record_tag_depth)&&(!strcmp(el, record_tag_name))) || ((Depth>record_tag_depth)&&(inside_article==1)))
  //if ((Depth == 1) || ((Depth>1)&&(inside_article==1)))
  {
    inside_article = 1 ;
    current_el.field_index = field_index++ ;
    current_el.off.field_offset = XML_GetCurrentByteIndex(p) ;
	if (Depth == record_tag_depth) rec_offset = current_el.off.field_offset ;
    strcpy(current_el.name, el) ;
    XMLPush(current_el) ;
    
    if (Depth==record_tag_depth) {
      for (i=0; i<XMLMAXDEPTH; i++) {
        for (j=0; j<XMLMAXNODE; j++)
          node_count[i][j] = 0 ;
        node_num[i] = 0 ;
      }
      
      cur_depth = Depth ;
    }
    else {
      for (i=node_num[Depth-record_tag_depth-1]-1; i>=0; i--)
        if (strcmp(node[Depth-record_tag_depth-1][i], el)==0) {
          strcpy(node[Depth-record_tag_depth-1][node_num[Depth-record_tag_depth-1]], el) ;
          node_count[Depth-record_tag_depth-1][node_num[Depth-record_tag_depth-1]] = node_count[Depth-record_tag_depth-1][i] + 1 ;
          node_num[Depth-record_tag_depth-1]++ ;
          break ;
        }  
        
      if (i==-1) {
        strcpy(node[Depth-record_tag_depth-1][node_num[Depth-record_tag_depth-1]], el) ;
        node_count[Depth-record_tag_depth-1][node_num[Depth-record_tag_depth-1]] = 0 ;
        node_num[Depth-record_tag_depth-1]++  ;
      }
      node_num[Depth-record_tag_depth] = 0 ;
    }      
  }
  else inside_article = 0 ;
}

static void 
end(void *data, const char *el)
{
  int i, j, k, l, m, n;
  struct stack_elem current_el;
  
  if (stack_top>=0) {
    xml_tag[xml_tag_no][0] = Depth - record_tag_depth + 1;
        
    xml_node[xml_tag_no][0] = '\0' ;
    for (i=0; i<Depth-record_tag_depth; i++) {
      // store full node path.
      sprintf(temp_node, "%s[%d]\0", node[i][node_num[i]-1], node_count[i][node_num[i]-1]+1) ;
        
      // store just path num.
      /*if (i<Depth-3 - 1) 
        sprintf(temp_node, "%d,\0", node_count[i][node_num[i]-1]+1) ;
      else
        sprintf(temp_node, "%d\0", node_count[i][node_num[i]-1]+1) ;
      */
      strcat(xml_node[xml_tag_no], temp_node) ; 
    }
  
    xml_path[xml_tag_no][0] = '\0' ;
    for (i=0; i<=stack_top; i++) 
      //if (i == 0)
      //  strcat(strcat(strcat(xml_path[xml_tag_no], "//"), xml_stack[i].name),"/");
      //else
      if(i!=0)
	  if(i==stack_top)
		strcat(xml_path[xml_tag_no], xml_stack[i].name);
	  else
	    strcat(strcat(xml_path[xml_tag_no], xml_stack[i].name), "/");
    
    strcat(xml_path[xml_tag_no], "\0") ;
    
    //if (stack_top == 0) 
    //  xml_tag[xml_tag_no][1] = xml_stack[stack_top].off.field_offset ;
    //else
    xml_tag[xml_tag_no][1] = xml_stack[stack_top].off.field_offset - rec_offset ;
    if ((xml_tag[xml_tag_no][1]>10000000)||(xml_tag[xml_tag_no][1]<0)) 
	  printf("\nel: %d %d %d %d %d %s\n", xml_stack[stack_top].off.field_offset, rec_offset, xml_tag_no, stack_top, XML_GetCurrentByteIndex(p),xml_path[xml_tag_no]) ;
    xml_tag[xml_tag_no][2] = XML_GetCurrentByteIndex(p) - xml_stack[stack_top].off.field_offset ;
    xml_tag_no++ ;
  }

  if (stack_top == 0) {
    strcpy(xml_node[xml_tag_no-1], record_tag_name) ;
	strcat(xml_node[xml_tag_no-1], "[1]\0") ;
	sort_path_tag(xml_tag, 0, xml_tag_no - 1) ;

	// The following loop is used to do validation. Ignored when formal runing, especially for effeciency evaluation!
	/*for (i=0; i<xml_tag_no; i++)
	  if ((xml_tag[i][1]>10000000)||(xml_tag[i][1]<0)) {
		printf("%d %d %d\n", xml_tag[i][1], i, rec_flag) ;
		//exit(0) ;
	  }
	*/
    xml_tag[0][1] = rec_offset ;
    
    // write rec offset of element path file and el offset
    l = ftell(f_el_offset) ;
    
    fwrite(&l, 1, 4, f_rec_offset) ;
    if ((rec_flag % 1000) == 0 )
      printf("%d\n", rec_flag) ;
      
    rec_flag++ ;
    
    k = 0 ;
    for (i=0; i<xml_tag_no; i++) {
      /*m = k ;
      
      for (; k<xml_tag_root; k++) {
        if (strcmp(xml_path[i], xml_root[k]) == 0) {
          root_flag = 1 ;
          break ;
        }
      }
      
      if ((m>0)&&(root_flag==0))
        for (k=0; k<m; k++) {
          if (strcmp(xml_path[i], xml_root[k]) == 0) {
            root_flag = 1 ;
            break ;
          }
        }
      */
      /*if (xml_tag_root > 0) k = cmp_root(xml_tag_root, xml_path[i]);
      else k = -1 ;
      if (k>=0) root_flag == 1 ;
      if(rec_flag==27) printf("%d\n", k);
      */
      strcpy(xml_root[xml_tag_root], xml_path[i]);
      if (xml_tag_root>0) k = sort_root(xml_tag_root) ;
      
      //write path ix file |rec_flag 4|path start pos in path file 4| next path pos in ix 4|
      if (root_flag == 1) {
        j = ftell(f_path_ix);
        fseek(f_path_ix, xml_root_off[k][1], SEEK_SET);
        fwrite(&j, 1, 4, f_path_ix) ;
        fseek(f_path_ix, j, SEEK_SET);
        fwrite(&rec_flag, 1, 4, f_path_ix) ;
        j = ftell(f_el_offset) ;
        fwrite(&j, 1, 4, f_path_ix) ;
        xml_root_off[k][1] = ftell(f_path_ix) ;
        j = 0xffffffff ;
        fwrite(&j, 1, 4, f_path_ix) ;
        xml_root_off[k][2] += 1 ;
        root_flag = 0 ;
      }
      else {
        //k = xml_tag_root ;
        //strcpy(xml_root[k], xml_path[i]) ;
        xml_root_off[k][0] = ftell(f_path_ix) ;
        fwrite(&rec_flag, 1, 4, f_path_ix) ;
        j = ftell(f_el_offset) ;
        fwrite(&j, 1, 4, f_path_ix) ;
        xml_root_off[k][1] = ftell(f_path_ix) ;
        j = 0xffffffff ;
        fwrite(&j, 1, 4, f_path_ix) ;
        xml_root_off[k][2] = 1 ;
        xml_root_off[k][3] = xml_tag[i][0] ;
        xml_root_off[k][4] = xml_tag_root ;
        //if (xml_tag_root>0) k = sort_root(xml_tag_root) ;
        xml_tag_root++ ;
      }
      
      //fprintf(f_offset, "%d ", xml_tag[i][1]);
      //fprintf(f_offset, "%d ", xml_tag[i][2]);
      //fprintf(f_offset, "%d ", xml_tag[i][0]);
      //fprintf(f_offset, "%d\n", xml_root_off[k][4]); 
      
      /*write path file  |start pos 4|end pos 4|Depth 4|path id 4|
      changed to write path file  |start pos 4|end pos 4|path id 4|path name|
      */
      fwrite(&xml_tag[i][1], 1, 4, f_el_offset) ;
      fwrite(&xml_tag[i][2], 1, 4, f_el_offset) ;
      //fwrite((char *)&xml_tag[i][0], 1, 2, f_el_offset) ;
      fwrite((char *)&xml_root_off[k][4], 1, 4, f_el_offset) ;
      fwrite(xml_node[i], 1, strlen(xml_node[i]), f_el_offset) ;
      temp_node[0] = '\n' ;
      fwrite(temp_node, 1, 1, f_el_offset) ;
      //printf("%s %d\n", xml_node[i], xml_tag[i][0]) ;
    }
    
    l = ftell(f_el_offset) - l ;
    fwrite(&l, 1, 4, f_rec_offset) ;
    
    xml_tag_no = 0 ;

	//exit(0) ;  
  }
  
  current_el = XMLPop();
    
  Depth--;
}


int
main(int argc, char *argv[])
{
  int i, j, len, total_bibfile, f_len ;
  char tmp[10] ;
  
  time_t start_time, end_time ;
  
  if (argc < 6) {
    printf("pls input the des database, input stem, number of bib files, record tag and record tag level!\n eg:  xml_path inex05.xml i:\\inex\\inex-1.6\\inex05.xml 2 article 3\n") ;
    return -1 ;
  }
  
  strcpy(fname_rec, argv[1]) ;
  strcat(fname_rec, ".rec.offset\0") ;
  strcpy(fname_path_id, argv[1]) ;
  strcat(fname_path_id, ".path.id\0") ;
  strcpy(fname_path_ix, argv[1]) ;
  strcat(fname_path_ix, ".path.ix\0") ;
  total_bibfile = atoi(argv[3]) ;
  strcpy(record_tag_name, argv[4]) ;
  record_tag_depth = atoi(argv[5]) ;
  
  time ( &start_time );
  //printf ( "Start date and time are: %s", ctime (&start_time) );
  
  f_rec_offset = fopen(fname_rec, "wb") ;
  if ( f_rec_offset == NULL ) {
    printf("can't open xml rec file %s\n", fname_rec) ;
    return -1 ;
  }
  //display() ;
  //exit(0) ;
  
  f_path_ix = fopen(fname_path_ix, "wb") ;
  if ( f_path_ix == NULL ) {
    printf("can't open xml rec file %s\n", fname_path_ix) ;
    return -1 ;
  }
  
  f_path_id = fopen(fname_path_id, "wb") ;
  if ( f_path_id == NULL ) {
    printf("can't open xml rec file %s\n", fname_path_id) ;
    return -1 ;
  }

// initialize the first line of the record_offset file.
fwrite(&total_bibfile, 1, 4, f_rec_offset) ;
for (i=0; i<total_bibfile; i++) {
  fwrite(&i, 1, 4, f_rec_offset) ;
  rec_num[i] = 0 ;
}


for (i=0; i<total_bibfile; i++) {
  if (i>0) fclose(f_el_offset) ;
  sprintf(tmp, "%d", i) ;
  strcpy(fname_el, argv[1]) ;
  strcat(fname_el, tmp) ;
  strcat(fname_el, ".el.offset\0") ;
  f_el_offset = fopen(fname_el, "wb") ;
  if ( f_el_offset == NULL ) {
    printf("can't open xml el file %s\n", fname_el) ;
    exit(0) ;
  }
  
  strcpy(fname_input, argv[2]) ;
  sprintf(tmp, "%d", i) ;
  strcat(fname_input, tmp) ;
  strcat(fname_input, ".bib\0") ;
  
  f_input = fopen(fname_input, "rb") ;
  if ( f_input == NULL ) {
    printf("can't open xml assessement file %s\n", fname_input) ;
    exit(0) ;
  }
  
  fseek(f_input, 0, SEEK_END) ;
  f_len = ftell(f_input) ;
  fseek(f_input, 0, SEEK_SET) ;
  
  p = XML_ParserCreate(NULL);
  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    exit(-1);
  }

  XML_SetElementHandler(p, start, end);
  
  for (;;) {
    int done;
    int cur_len, len;
    
	cur_len = ftell(f_input) ;   
    if (f_len - cur_len < BUFFSIZE) {
      len = fread(Buff, 1, f_len - cur_len, f_input);
      done = 1 ;
	}
	else {
      len = fread(Buff, 1, BUFFSIZE, f_input);
      done = 0 ;
    }
    
	//printf("%d %d %d %s\n", len, f_len, cur_len, fname_input) ;
	 
    /*if (ferror(f_input)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }*/
    if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr, "Parse error at line %d:\n%s\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

	if (done) {
	  fclose(f_input) ;
	  break ;
	}
  }
  
  rec_num[i] = rec_flag ;
}

  
  
/*  p = XML_ParserCreate(NULL);
  if (! p) {
    fprintf(stderr, "Couldn't allocate memory for parser\n");
    exit(-1);
  }

  XML_SetElementHandler(p, start, end);
  
  for (;;) {
    int done;
    int len;
    
    len = fread(Buff, 1, BUFFSIZE, stdin);
    if (ferror(stdin)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }
    done = feof(stdin);

    if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr, "Parse error at line %d:\n%s\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

    if (done)
      break;
  }
*/


// get the neighbouring path and the first child path.
{ 
  int xml_stack_id[XMLMAXSTACKSIZE] ;

  Depth = 0 ;
  for (i=0; i<xml_tag_root; i++) {
    xml_root_off[i][5] = 0 ;
    xml_root_off[i][6] = 0 ;
  }

  for (i=0; i<xml_tag_root; i++) {
    if (i==0) len = 4 ;
    
    if (Depth==0) {
      xml_stack_id[Depth++] = i ;
      len += 4*8 + strlen(xml_root[i]) ;
      continue ;
    }
    
    j = xml_stack_id[Depth-1] ;
    if (xml_root_off[i][3] == xml_root_off[j][3]) {
      xml_root_off[j][5] = len ;
      xml_stack_id[Depth-1] = i ;
    }
    else if (xml_root_off[i][3] > xml_root_off[j][3]) {
      xml_root_off[j][6] = len ;
      xml_stack_id[Depth++] = i ;
    }
    else {
      Depth = xml_root_off[i][3] - 1 ;
      if (Depth != 0) {
        j = xml_stack_id[Depth-1] ;
        xml_root_off[j][5] = len ;
		xml_stack_id[Depth-1] = i ;
	  }
    }
    
    len += 4*8 + strlen(xml_root[i]) ;
  }  
}

// modify the rec_offset record distribution in bibfiles
fseek(f_rec_offset, 4, SEEK_SET) ;
for(i=0; i<total_bibfile; i++) {
  fwrite(&rec_num[i], 1, 4, f_rec_offset) ;
}
    
  //write path id file |Depth 4|id 4|start in ix 4|end in ix 4|number of path 4|next pos in the same level|the 1st child pos|path len| path var|  
  for(i=0; i<xml_tag_root; i++) {
    //fprintf(f_path_id, "%d ", xml_root_off[i][3]) ;
    //fprintf(f_path_id, "%d ", xml_root_off[i][4]) ;
    //fprintf(f_path_id, "%d ", xml_root_off[i][0]) ;
    //fprintf(f_path_id, "%d ", xml_root_off[i][1]) ;
    //fprintf(f_path_id, "%d \n", xml_root_off[i][2]) ;
    //fprintf(f_path_id, "%s ", xml_root[i]) ;
	
	//write number of path first. 
    if (i==0) fwrite(&xml_tag_root, 1, 4, f_path_id) ;

	// write information for each path.
    fwrite(&xml_root_off[i][3], 1, 4, f_path_id) ;
    fwrite(&xml_root_off[i][4], 1, 4, f_path_id) ;
    fwrite(&xml_root_off[i][0], 1, 4, f_path_id) ;
    fwrite(&xml_root_off[i][1], 1, 4, f_path_id) ;
    fwrite(&xml_root_off[i][2], 1, 4, f_path_id) ;
    fwrite(&xml_root_off[i][5], 1, 4, f_path_id) ;
    fwrite(&xml_root_off[i][6], 1, 4, f_path_id) ;
    len = strlen(xml_root[i]) ;
    fwrite(&len, 1, 4, f_path_id) ;
    fwrite(&xml_root[i], 1, len, f_path_id) ;
    //putc('\n', f_path_id) ;
  }
  
  fclose(f_input) ;
  fclose(f_el_offset) ;
  fclose(f_rec_offset) ;
  fclose(f_path_ix) ;
  fclose(f_path_id) ;
  
  time ( &end_time );
  printf ( "Totolly use %.0f seconds to cope with %d records!\n", difftime(end_time, start_time), rec_flag);
  //printf ("You have taken %.4lf seconds to do indexing.\n", difftime(end_time, start_time) );
  return 0;
}

int sort_root(int num)
{
   int start, end, i, k, l;
   int xml_temp_root_off[5] ;
   char xml_temp_root[200];
   
   start = 1 ; 
   end = num;

   if (strcmp(xml_root[num],xml_root[0]) < 0)
   {
     xml_temp_root_off[0] = xml_root_off[num][0];
     xml_temp_root_off[1] = xml_root_off[num][1];
     xml_temp_root_off[2] = xml_root_off[num][2];
     xml_temp_root_off[3] = xml_root_off[num][3];
     xml_temp_root_off[4] = xml_root_off[num][4];
     strcpy(xml_temp_root, xml_root[num]);
     for (i = num; i > 0; i--) 
     {  
       xml_root_off[i][0] = xml_root_off[i-1][0];
       xml_root_off[i][1] = xml_root_off[i-1][1];
       xml_root_off[i][2] = xml_root_off[i-1][2];
       xml_root_off[i][3] = xml_root_off[i-1][3];
       xml_root_off[i][4] = xml_root_off[i-1][4];
       strcpy(xml_root[i], xml_root[i-1]);
     }
     
     xml_root_off[0][0] = xml_temp_root_off[0];
     xml_root_off[0][1] = xml_temp_root_off[1];
     xml_root_off[0][2] = xml_temp_root_off[2];
     xml_root_off[0][3] = xml_temp_root_off[3];
     xml_root_off[0][4] = xml_temp_root_off[4];
     strcpy(xml_root[0], xml_temp_root);
     
     return 0;
   }
                                                                  
   if (strcmp(xml_root[num],xml_root[num - 1]) > 0) return num; 
   
   for (;end - start > 1; )
   {
     k = (end + start)/2 ;
     if (strcmp(xml_root[num],xml_root[k - 1]) >= 0 )
       start = k ;
     else
       end = k ;
   }
   
   if (strcmp(xml_root[num],xml_root[start - 1]) == 0) {
     root_flag = 1 ;
     return start - 1 ;
   }
   if (strcmp(xml_root[num],xml_root[end - 1]) == 0) {
     root_flag = 1 ;
     return end - 1 ;
   }
   xml_temp_root_off[0] = xml_root_off[num][0];
   xml_temp_root_off[1] = xml_root_off[num][1];
   xml_temp_root_off[2] = xml_root_off[num][2];
   xml_temp_root_off[3] = xml_root_off[num][3];
   xml_temp_root_off[4] = xml_root_off[num][4];
   strcpy(xml_temp_root, xml_root[num]);
   
   for (i = num; i > start; i--) 
   {  
     xml_root_off[i][0] = xml_root_off[i-1][0];
     xml_root_off[i][1] = xml_root_off[i-1][1];
     xml_root_off[i][2] = xml_root_off[i-1][2];
     xml_root_off[i][3] = xml_root_off[i-1][3];
     xml_root_off[i][4] = xml_root_off[i-1][4];
     strcpy(xml_root[i], xml_root[i-1]);
   }
     
   xml_root_off[start][0] = xml_temp_root_off[0];
   xml_root_off[start][1] = xml_temp_root_off[1];
   xml_root_off[start][2] = xml_temp_root_off[2];
   xml_root_off[start][3] = xml_temp_root_off[3];
   xml_root_off[start][4] = xml_temp_root_off[4];
   strcpy(xml_root[start], xml_temp_root);
  
   return start;
}

int cmp_root(int num, char el[])
{
   int start, end, k, l;
      
   start = 1 ; 
   end = num;

   if (strcmp(el,xml_root[0]) < 0)   
     return -1;
                                                                  
   if (strcmp(el,xml_root[num - 1 ]) > 0) return -1; 
   
   for (;end - start > 1; )
   {
     k = (end + start)/2 ;
     //printf("%d ", k) ;
     
     l = strcmp(el,xml_root[k - 1]) ;
     if ( l >= 0 ){
       start = k ;
       //if (l == 0) return k - 1 ;
     }
     else if (l < 0)
       end = k ;
   }
   
   if (strcmp(el,xml_root[start - 1]) == 0) return start - 1;
   if (strcmp(el,xml_root[end - 1]) == 0) return end - 1 ;
   
   return -1 ;
}

int sort_path_tag(int numbers[][3], int left, int right)
{
  int pivot, l_hold, r_hold ;

  l_hold = left;
  r_hold = right;
  pivot = numbers[left][1];
  xml_temp_tag[0] = numbers[left][0] ;
  xml_temp_tag[2] = numbers[left][2] ;
  strcpy(xml_temp_path, xml_path[left]) ; 
  strcpy(temp_node, xml_node[left]) ;
  
  while (left < right)
  {
    while ((numbers[right][1] >= pivot) && (left < right))
      right--;
    if (left != right)
    {
      numbers[left][0] = numbers[right][0];
      numbers[left][1] = numbers[right][1];
      numbers[left][2] = numbers[right][2];
      strcpy(xml_path[left], xml_path[right]) ;
      strcpy(xml_node[left], xml_node[right]) ;
      left++;
    }
    while ((numbers[left][1] <= pivot) && (left < right))
      left++;
    if (left != right)
    {
      numbers[right][0] = numbers[left][0];
      numbers[right][1] = numbers[left][1];
      numbers[right][2] = numbers[left][2];
      strcpy(xml_path[right], xml_path[left]) ;
      strcpy(xml_node[right], xml_node[left]) ;
      right--;
    }
  }
  
  numbers[left][1] = pivot ;
  numbers[left][0] = xml_temp_tag[0] ;
  numbers[left][2] = xml_temp_tag[2] ;
  strcpy(xml_path[left], xml_temp_path) ; 
  strcpy(xml_node[left], temp_node) ;
  
  pivot = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    sort_path_tag(numbers, left, pivot-1);
  if (right > pivot)
    sort_path_tag(numbers, pivot+1, right);

  return 0;
}

int display()
{
  int a, b, c[20000], i=0, j, k, l=0 ;
  char tmp[200] ;
  
  fseek(f_rec_offset, 0, SEEK_SET) ;
  while(!feof(f_rec_offset)) {
	fread(&a, 1, 4, f_rec_offset) ;
	fread(&b, 1, 4, f_rec_offset) ;
	fseek(f_el_offset, a, SEEK_SET) ;
	k = ftell(f_el_offset) ;
	i = 0 ;
	while((k>=a)&&(k<a+b)) {
	  fread(&c[i++], 1, 4, f_el_offset ) ;
	  fread(&j, 1, 4, f_el_offset ) ;
	  fread(&j, 1, 2, f_el_offset ) ;
	  fgets(tmp, 200, f_el_offset ) ;
	  k = ftell(f_el_offset) ;
	  //printf("%d %d, %s\n", c[i-1], j, tmp) ;
    }
    c[0] = 0 ;
	for (j=0; j<i-1; j++)
	  if (c[j]>c[j+1])
		printf("%d %d %d %d %d\n", c[j], c[j+1], l, j, i) ;

	  //if (l==8000) return 0 ;
	l++;
	printf("Rec %d is processed!\n", l) ;
  }

  return 1;

}
