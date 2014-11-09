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

/***********************************************************************
  
  This file is added by Lu Wei May 2005 to do field weighting.

******************************************************************************/

#include "bss.h"
#include "bss_errors.h"
#include <string.h>

int f_num ;
int f_oset ;
int f_len ;
int f_numpars ;
int reclen = 0;    

/* #define field tf and wt  by Lu Wei Apr 2005 */
struct field_record {
  short fd ;
  short stream ; 
  short tf ;
  double wt ;
};

/* copy from bss_passages.c By Lu Wei on May 2005 */
int 
get_field(int fdnum)
{
  int num_paras ;
  int j ;
  int *ip = Cdb.parabuf ;
  ip += 3 ;
  for ( j = 0 ; j < fdnum - 1 ; j++ ) {
    ip += 3 ;
    num_paras = *ip++ ;
    ip += num_paras * 2 ;
  }
  f_num = *ip++ ; f_oset = *ip++ ; f_len = *ip++ ; f_numpars = *ip++ ;
}

/******************************************************************************

  setup_field_tfs_faster and setup_field_tfs  added by Lu Wei Apr 2005

******************************************************************************/

#ifdef FASTER

double setup_field_tfs_faster(FILE *f_tf, int rec, int num, int numstreams, int streams[], bss_Gp *pstgs[], 
                           int nk, int pl, double wts[], double k1, 
                           double k2, double bm25_b, double avedl, double doclen)
{
  int tf, f_sent ;

#if HILO
  Pos_text_hl *pos ; 
#else
  Pos_text_lh *pos ; 
#endif

  int *ip;
  int field_num = Cdb.nf;
  int j, k, l ;
  int numpos ; //number of pos in a rec.
  char fname_tf[50];
  double w = 0.0 ;
  double bm25_K, tfp ;
    
  struct field_record field_rec[field_num*numstreams];
  static char *func = "setup_field_tfs_faster" ;

  
  for ( j = 0 ; j < numstreams ; j++ ) {
    pos = pstgs[streams[j]]->p ;
    numpos = pstgs[streams[j]]->numpos ;
    for (; l< num; l++)
  if (streams[j] == l) {
    for ( k = 0 ; k < field_num ; k++ ) {
      tf = 0 ;
      
      for ( ; ; pos++ ){ 
        if (*(u_int *)pos = 0xffffffff) break;
        if (numpos<=0) break ;
        if (pos->f > k) break;
        if (pos->f < k) continue;
        if (pos->f == k) tf++ ;
      }
      w = 0.0 ;
      
      if ((k==0)&&(l==0)) fprintf(f_tf, "%d %.0f %.0f %.3f %d", rec, doclen, avedl, wts[l], tf) ;
      else if ((k==0)&&(l>0)) fprintf(f_tf, " %.3f %d", wts[l], tf) ;
      else fprintf(f_tf, " %d", tf) ;
      if ((k==(field_num - 1))&& (l==(num-1))) fprintf(f_tf, "\n");      
    }
    if (j!=(numstreams - 1)) { 
      l++ ; break ; 
    }
  }
  else 
    for ( k = 0 ; k < field_num ; k++ ) {
      if ((k==0)&&(l==0)) fprintf(f_tf, "%d %.0f %.0f %.3f %d", rec, doclen, avedl, wts[l], 0) ;
      else if ((k==0)&&(l>0)) fprintf(f_tf, " %.3f %d", wts[l], 0) ;
      else fprintf(f_tf, " %d", 0) ;
      if ((k==(field_num - 1))&& (l==(num-1))) fprintf(f_tf, "\n");
    }
  }

  return w;
}

#else

double setup_field_tfs(FILE *f_tf, int rec, int num, int numstreams, int streams[], bss_Gp *pstgs[], 
                    int nk, int pl, double wts[], double k1, 
                    double k2, double bm25_b, double avedl, double doclen)
{
  int tf, f_sent ;
  
  U_pos *pos ;
  int *ip;
  int field_num = Cdb.nf;
  int j, k, l ;
  int numpos ; //number of pos in a rec.
  char fname_tf[50];
  double w = 0.0 ;
  double bm25_K, tfp ;
  
  FILE *f_ipath ;
  char fname_ipath[100] = "./inex.xml.ipath\0" ;

  struct field_record field_rec[field_num*numstreams] ;
  static char *func = "setup_field_tfs" ;
  
  f_ipath = fopen(fname_ipath, "ab") ;
  if ( f_ipath == NULL ) {
    f_ipath = fopen(fname_ipath, "wb") ;
    if ( f_ipath == NULL ) {
      printf("can't open xml ipath file %s\n", fname_ipath) ;
      exit(-1) ;
    }
  }
  
  l = 0 ;
 
  for ( j = 0 ; j < numstreams ; j++ ) {
    pos = pstgs[streams[j]]->p ;
    numpos = pstgs[streams[j]]->numpos ;
    //printf("%d  ", pstgs[streams[j]]->numpos);
    for (; l< num; l++)
  if (streams[j] == l) {
    for ( k = 0 ; k < field_num ; k++ ) {
      tf = 0 ;
  
      for ( ; ; pos++, numpos--){ 
      #if HILO
          if (*(u_int *)pos == 0xffffffff) break; 
          if (numpos<=0) break ;
          if (pos->pthl.f == k) {
            unsigned offset ; 
            tf++ ;
            offset = pos->pthl.offset;
            fwrite(&rec, 1, 4, f_ipath) ;
            fwrite(&l, 1, 4, f_ipath) ;
            fwrite(&offset, 1, 4, f_ipath) ;
            fwrite(&wts[l], 1, 8, f_ipath) ;
            //fwrite(&doclen, 1, 8, f_ipath) ;
            //fwrite(&avedl, 1, 8, f_ipath) ;
          }
          if (pos->pthl.f > k) break;
          if (pos->pthl.f < k) continue;
      #else
          if (*(u_int *)pos == 0xffffffff) break;
          if (numpos<=0) break ;
          //if ((pos->ptlh.f>4)||(pos->ptlh.offset>8000))printf("%d %d %u\n", rec, pos->ptlh.f, pos->ptlh.offset) ;
          if (pos->ptlh.f == k) {
            unsigned offset ; 
            tf++ ;
            offset = pos->ptlh.offset;
            fwrite(&rec, 1, 4, f_ipath) ;
            fwrite(&l, 1, 4, f_ipath) ;
            fwrite(&offset, 1, 4, f_ipath) ;
            fwrite(&wts[l], 1, 8, f_ipath) ;
            //fwrite(&doclen, 1, 8, f_ipath) ;
            //fwrite(&avedl, 1, 8, f_ipath) ;
            //fprintf(f_ipath, "%d %d %.3f %.0f %.0f\n", rec, pos->ptlh.offset, wts[l], doclen, avedl) ;
          }
          if (pos->ptlh.f > k) break;
          if (pos->ptlh.f < k) continue;
      #endif
      }
      w = 0.0 ;
 /*     
      if ((k==0)&&(l==0)) fprintf(f_tf, "%d %.0f %.0f %.3f %d", rec, doclen, avedl, wts[l], tf) ;
      else if ((k==0)&&(l>0)) fprintf(f_tf, " %.3f %d", wts[l], tf) ;
      else fprintf(f_tf, " %d", tf) ;
      if ((k==(field_num - 1))&& (l==(num-1))) fprintf(f_tf, "\n");
    }
    if (j!=(numstreams - 1)) { 
      l++ ; break ; 
    }
  }
  else 
    for ( k = 0 ; k < field_num ; k++ ) {
      if ((k==0)&&(l==0)) fprintf(f_tf, "%d %.0f %.0f %.3f %d", rec, doclen, avedl, wts[l], 0) ;
      else if ((k==0)&&(l>0)) fprintf(f_tf, " %.3f %d", wts[l], 0) ;
      else fprintf(f_tf, " %d", 0) ;
      if ((k==(field_num - 1))&& (l==(num-1))) fprintf(f_tf, "\n");
    }   
 */
      if ((k==0)&&(l==0)) {
        fwrite(&rec, 1, 4, f_tf) ;
        fwrite(&doclen, 1, 8, f_tf) ;
        fwrite(&avedl, 1, 8, f_tf) ;
        fwrite(&wts[l], 1, 8, f_tf) ;
        fwrite(&tf, 1, 4, f_tf) ;
      }
      else if ((k==0)&&(l>0)) {
        fwrite(&wts[l], 1, 8, f_tf) ;
        fwrite(&tf, 1, 4, f_tf) ;
      }
      else fwrite(&tf, 1, 4, f_tf) ;
    }
    if (j!=(numstreams - 1)) { 
      l++ ; break ; 
    }
  }
  else 
    for ( k = 0 ; k < field_num ; k++ ) {
      tf = 0 ;
      if ((k==0)&&(l==0)) {
        fwrite(&rec, 1, 4, f_tf) ;
        fwrite(&doclen, 1, 8, f_tf) ;
        fwrite(&avedl, 1, 8, f_tf) ;
        fwrite(&wts[l], 1, 8, f_tf) ;
        fwrite(&tf, 1, 4, f_tf) ;
      }
      else if ((k==0)&&(l>0)) {
        fwrite(&wts[l], 1, 8, f_tf) ;
        fwrite(&tf, 1, 4, f_tf) ;
      }
      else fwrite(&tf, 1, 4, f_tf) ;
    } 
  }
  
  fclose(f_ipath) ;
  return w;
}
#endif

double do_fields(FILE *f_tf, int rec, int num, int nk, int numstreams, int streams[], 
                bss_Gp *pstgs[], double wts[], double k1, 
                    double k2, double bm25_b, double avedl, double doclen)
{
  int pl ;
  
  //get_para_rec(rec) ;
  
  #ifdef FASTER
    return setup_field_tfs_faster(f_tf, rec, num, numstreams, streams, pstgs, nk, pl, wts, k1, k2, bm25_b, avedl, doclen) ;
  #else
    return setup_field_tfs(f_tf, rec, num, numstreams, streams, pstgs, nk, pl, wts, k1, k2, bm25_b, avedl, doclen) ;
  #endif
  
  /*
  #ifdef FASTER
    return setup_field_tfs_faster(f_tf, rec, num, numstreams, streams, pstgs, nk, pl, wts, k1, k2, bm25_b, avedl, doclen) ;
  #else
    return setup_field_tfs(f_tf, rec, num, numstreams, streams, pstgs, nk, pl, wts, k1, k2, bm25_b, avedl, doclen) ;
  #endif
  */
}
