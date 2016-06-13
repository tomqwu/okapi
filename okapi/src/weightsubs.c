/*****************************************************************************

  weightsubs ... 21 Jan 88

  Functions for calculating weights for individual terms, and for calculating
  'good' and 'acceptable' weights for a merge.

  Jan 99: added bm25()

  27 Feb 91: added selection_value() - see SER: the selection of terms for QE

  Revision 25 Apr 88 - passing big_n instead of using #define

  Revision 5 Feb 88 - added Orig_assign_weights (prev in subsrch.c)

  9 Sept 88 - noticed several mistakes in orig_assign_weights() -
  the case no concepts (all terms Q-terms) is not dealt with but (luckily)
  falls under "more than two concepts" - Q-terms are too highly weighted,
  each should probably be given a weight which is less than that of the 
  lightest concept (this becomes less important as the number of concepts
  increases) - the two-concept case will not always work properly if there are 
  Q-terms ..

*****************************************************************************/

#include "bss.h"

/******************************************************************************

  New weight func nr_wt() Dec 96 -- see SER paper dated 10/2/96

  (N, n, R, r, S, S, k4, k5, k6)


  March 99: new "theta" version of f4_p

******************************************************************************/

double
nr_wt(double N, double n, double R, double r, double S, double s,
      double k4, double k5, double k6)
{
  double wp, wq ;

  if ( n == N ) return 0 ;
  if ( n <= 0 || N < n || k5 == 0 && R == 0 || k6 == 0 && S == 0 ||
       R < r || S < s || N < 0 || n < 0 || R < 0 || r < 0 || S < 0 || s < 0 ) {
    return 0 ;			/* Used to be quiet_nan, but not SYSV */
  }       

  wp = k5 / (k5 + sqrt(R)) * (k4 + log(N / (N - n))) +
    sqrt(R) / (k5 + sqrt(R)) * log((r + 0.5) / (R - r + 0.5)) ;
  wq = k6 / (k6 + sqrt(S)) * log(n / (N - n)) +
    sqrt(S) / (k6 + sqrt(S)) * log((s + 0.5) / (S - s + 0.5)) ;
  return wp - wq ;
}

double f4_p_theta_ln(double N, double n, double R, double r, double theta)
{
  double t1, t2, b1, b2 ;

  t1 = r + theta * n / N ; b1 = R - r + theta * (1 - n / N) ;
  t2 = n - r + theta * n / N ; b2 = N - n - R + r + theta * (1 - n / N) ;

  if ( t1 <= 0 || t2 <= 0 || b1 <= 0 || b2 <= 0 ) return 0 ;
  else return log((t1/b1)/(t2/b2)) ;
}


double
plain_f4(int N, int n, int R, int r, int Rload, int rload) 
{
  return f4_p_loaded_ln((double)N, (double)n, (double)R, (double)r,
			0.5, (double)Rload, (double)rload) ;
}

double f4_p_loaded_ln(double N, double n, double R, double r,
		      double c, double Rloading, double rloading)
     /* R and r-loaded version of F4_p with natural logs */
{
  /* mko Hard coding 'N' for terabyte data!! */
  //N = 23522628;
  double t1, t2, b1, b2 ;
 
  /*
  // mko Print out parameter values for debugging.
  printf ("Values \n \
        N = %10.3f\n \
        n = %10.3f\n \
        R = %10.3f\n \
        r = %10.3f\n \
        c = %10.3f\n \
        Rloading = %10.3f\n \
        rloading = %10.3f\n ", N, n, R, r, c, Rloading, rloading);
   */


  t1 = r + rloading + c ; b1 = R + Rloading - r -rloading + 1 -c ;
  t2 = n - r + c ; b2 = N - n - R + r + 1 - c ;

  if ( t1 < 0.1 || t2 < 0.1 || b1 < 0.1 || b2 < 0.1 ) return 0 ;
  else return log( (t1/b1) / (t2/b2) ) ;
}

double idf(double N, double n)
{
  return log(N/n) ;
}

#ifdef OLDWEIGHTFUNCS

double f0_r(N, n)	 /* W0 ... retrospective weighting */
     double N, n ;
{
  return( lg(N / n) ) ;
}

double f1_r(N, n, R, r)	 /* W1 ... retrospective weighting  */

     double N, n, R, r ;
{
  return( lg((r / R) / (n / N)) ) ;
}

 
double f2_r(N, n, R, r)	 /* W2 ... retrospective weighting */
     double N, n, R, r ;
{
  return( lg(   (r/R) / ( (n-r) / (N-R))  ) )  ;
}


double f3_r(N, n, R, r)  /* W3 ... retrospective weighting */
     double N, n, R, r ;
{
  return( lg(   (r / (R-r)) / (n / (N-n)) )  ) ;
}

				
double f4_r(N, n, R, r)	 /* W4 ... retrospective weighting */
     double N, n, R, r ;
{
  return( lg(  (r / (R-r)) / ((n-r) / (N-n-R+r))  )  ) ;
}


double f0_p(N, n)	/* W0 for predictive weighting scheme */
     double N, n ;
{
  return( lg((N + 2) / (n + 1)) ) ;
}


double f1_p(N, n, R, r)	/* W1 for predictive weighting scheme */

     double N, n, R, r ;
{
  return( lg( ((r+0.5)/(R+1)) / ((n+1)/(N+2)) ) ) ;
}

 
double f2_p(N, n, R, r)	/* W2 for predictive weightinf scheme  */
     double N, n, R, r ;
{
  return( lg( ((r+0.5)/(R+1)) / ((n-r+0.5)/(N-R+1)) ) )  ;
}


double f3_p(N, n, R, r)	/* W3 for predictive weighting scheme */
     double N, n, R, r ;
{
  return( lg( ((r+0.5)/(R-r+0.5)) / ((n+1)/(N-n+1)) ) ) ;
}



double f4_p(N, n, R, r, c)	/* W4 for predictive weighting scheme */
     double N, n, R, r, c ;
{
  double t1, t2, b1, b2 ;

  t1 = r + c ; b1 = R - r + 1 -c ;
  t2 = n - r + c ; b2 = N - n - R + r + 1 - c ;
  if ( t1 < 0.1 || t2 < 0.1 || b1 < 0.1 || b2 < 0.1 ) return 0 ;
  return lg( (t1/b1) / (t2/b2) ) ;
}


/* See Robertson: On relevance weight estimation and query expansion. J Doc
   42 (3). */

double f4_p_loaded(N, n, R, r, c, Rloading, rloading)
     /* R and r-loaded version of F4_p */
     double N, n, R, r, c, Rloading, rloading ;
{
  double t1, t2, b1, b2 ;

  t1 = r + rloading + c ; b1 = R + Rloading - r -rloading + 1 -c ;
  t2 = n - r + c ; b2 = N - n - R + r + 1 - c ;

  if ( t1 < 0.1 || t2 < 0.1 || b1 < 0.1 || b2 < 0.1 ) return 0 ;
  else return lg( (t1/b1) / (t2/b2) ) ;
}

double f4_qt(N, n)		/* Added 21 Apr 92 rep f0_p in
				   orig_assign_weights() */
     int N, n ;
{
  return f4_p((double)N, (double)n, 0.0, 0.0, 0.5) ;
}

/* Note intf4_qt() will work as an integer version of f0_p() except that
   N becomes N-n */

intf4_qt(N, n, R, r)		/* Version of above returning a WEIGHT */
				/* For query terms */
     int N, n, R, r ;
{
  double f4_p() ;
  return
    round(f4_p((double) N, (double) n, (double) R, (double) r, .5 ) + 0.5 ) ;
}

intf4_p(N, n, R, r)		/* Same as intf4_qt() */
     int N, n, R, r ;
{
  return(intf4_qt(N, n, R, r)) ;
}

intf4_qe(N, n, R, r)		/* Version of above returning a WEIGHT */
				/* For terms extracted from rel. recs. */
     int N, n, R, r ;
{
  double f4_p() ;
  return round(f4_p((double) N, (double) n, (double) R, (double) r,
		    (double) ((double) n/ (double) N)) + 0.5 ) ;
}

double
emim(N, n, R, r, S, s)
     int N, n , R, r, S, s ;
{
  double p11, p12, p21, p22 ;	/* log components */
  double i11, i12, i21, i22 ;	/* Degrees of involvement */
  double dN = N+2, dR = R+1, dn = n+1, dr = r+0.5, dS = S+2, ds = s+1 ;
  double result ;


  printf("\nComponents ") ;
  p11 = lg(dr*dN/(dn*dR)) ;
  p12 = lg((dn-dr)*dN/(dn*(dN-dR))) ;
  p21 = lg((dR-dr)*dN/((dN-dn)*dR)) ;
  p22 = lg((dN-dn-dR+dr)*dN/((dN-dn)*(dN-dR))) ;
  printf("%.2f %.2f %.2f %.2f\n", p11, p12, p21, p22) ;
  printf("Degrees of involvement (comp)") ;
  i11 = dr ; i12 = (dn -dr) ;
  i21 = (dR-dr) ; i22 = (dN-dR-dn+dr) ;
  printf("%.2f %.2f %.2f %.2f\n", i11, i12, i21, i22) ;
  result = p11*i11 - p12*i12 - p21*i21 + p22*i22 ;
  printf("emim (comp/comp) %.2f\n", result) ;
  printf("Degrees of involvement (sample)") ;
  i11 = dr ; i12 = (ds -dr) ;
  i21 = (dR-dr) ; i22 = (dS-dR-ds+dr) ;
  printf("%.2f %.2f %.2f %.2f\n", i11, i12, i21, i22) ;
  
  result = p11*i11 - p12*i12 - p21*i21 + p22*i22 ;
  return(result) ;
  /* Note the degrees of involvement are based on S and s  */
}

#endif /* OLDWEIGHTFUNCS */

/******************************************************************************

  weight_to_s_v gives the Robertson selection value
  s_v_to_weight does what you'd expect

  21 Apr 92.

******************************************************************************/

double
weight_to_s_v(double weight, int R, int r)
{
  if ( R == 0 ) return weight ;
  if ( r == 0 ) r = 1 ;		/* SER suggested bodge Sept 97 (prev was as R==0) */
  return weight * (double)r / (double)R ;
}

double
s_v_to_weight(double sv, int R, int r) /* Modified Sept 97 in line with weight_to_sv() */
{
  if ( R == 0 ) return sv ;
  if ( r == 0 ) r = 1 ;
  return sv * (double)R / (double)r ;
}

/******************************************************************************

  rscore(N, n, R, r, T)

  Calc new Robertson term selection score

  SW Dec 99

  Aug 00: NB needs TNT (types in index) not CTTF or TNP (tokens in index)

******************************************************************************/

double
rscore(int N, int n, int R, int r, int V)
{
  double ch = n_choose_r(R, r) ;
  if ( ch <= 0 ) return -400 ;	/* Means r>R or r<0 or R<0 */
  if ( N < n || n <= 0 ) return -400 ;
  return (double)r * log(((double)N + 0.5)/((double)n + 0.5)) - log(ch)
    - log((double)V) ;
}

/******************************************************************************

  bm25mult(k, b, tf, D)  SW Jan 99

  Returns bm25 multiplier. D is doclen/avedoclen

  k3mult(k3, qtf)  SW Mar 01

  does bm25mult with b=1 and D=1

******************************************************************************/

double
bm25mult(double k, double b, int tf, double D)
{
 printf ("applying bm25mult multiplier\n");
  return (k + 1) * (double)tf / ((1 - b + b * D) * k + (double)tf) ;
}

double 
k3mult(double k3, int qtf)
{
  return bm25mult(k3, 1, qtf, 1) ;
}

/******************************************************************************

  Keen-type proximity measures

  keen_1(pstg, thresh) calcs the distance between successive occurrences
  of some term within the same sentence and scores thresh - dist for each
  such pair, with a min score of 2. If successive terms are in adjacent
  sentences it scores 1 (it isn't possible to measure the true distance
  between terms in different sentences). 

  The function will give spurious results unless the posting really contains
  positional fields of the "long" (32 bit) type.

******************************************************************************/

#ifdef OLDWEIGHTFUNCS

keen_1(pstg, thresh)
     bss_Gp *pstg ;
     int thresh ;
{
  u_int *pos, *prev ;
  int i ;
  int field, sentence, prev_field, prev_sentence ;
  int diff, score ;
  int result = 0 ;

  pos = pstg->pos ;
  prev = pos ;
  prev_field = ((Pos6 *)prev)->f ;
  prev_sentence = ((Pos6 *)prev)->s ;
  pos++ ;
  for ( i = 1 ; i < pstg->numpos ; i++, pos++ ) {
    score = 0 ;
    field = ((Pos6 *)pos)->f ;
    sentence = ((Pos6 *)pos)->s ;
    if ( field == prev_field ) {
      if ( sentence == prev_sentence ) {
	diff = ((Pos6 *)pos)->t -
	  ((Pos6 *)pos)->sw - (((Pos6 *)prev)->t + ((Pos6 *)prev)->nt ) ;
	score = thresh - diff ;	/* score <= thresh */
	if ( score < 2 ) score = 2 ;
      }
      else if ( sentence - prev_sentence == 1 ) score = 1 ;
    }
    result += score ;
    prev = pos ;
    prev_field = field ; prev_sentence = sentence ;
  }
  return result ;
}

#endif /* OLDWEIGHTFUNCS */
      
