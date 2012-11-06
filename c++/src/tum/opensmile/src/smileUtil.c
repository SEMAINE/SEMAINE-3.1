/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/

/*

Smile Util:

* modular DSP functions
* math functions
* sort algoritms
* wave file I/O functions
* vector file I/O functions (for dumping debug data)

... and other utility functions 

*/
//------ remove this include if you are using smileUtil as standalone code...
//#include <exceptions.hpp>
//--------------------

#include <smileUtil.h>


//#include <smileTypes.hpp>

/*******************************************************************************************
 ***********************=====   Sort functions   ===== **************************************
 *******************************************************************************************/

/** inplace quicksort algorithms **/

/* QuickSort algorithm for a float array with nEl elements */
void smileUtil_quickSort_float(float *arr, long nEl)
{
  #ifdef MAX_LEVELS
  #undef MAX_LEVELS
  #endif
  #define  MAX_LEVELS  300

  float piv;
  long beg[MAX_LEVELS], end[MAX_LEVELS],swap;
  long i=0, L, R;

  beg[0]=0; end[0]=nEl;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L]; 
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
      if (end[i]-beg[i]>end[i-1]-beg[i-1]) {
        swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
        swap=end[i]; end[i]=end[i-1]; end[i-1]=swap;
      }
    } else { i--; }
  }
}

/* QuickSort algorithm for a double array with nEl elements */
void smileUtil_quickSort_double(double *arr, long nEl)
{
  #ifndef MAX_LEVELS
  #define MAX_LEVELS  300
  #endif

  double piv;
  long beg[MAX_LEVELS], end[MAX_LEVELS],swap;
  long i=0, L, R;
  
  beg[0]=0; end[0]=nEl;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L];
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
      if (end[i]-beg[i]>end[i-1]-beg[i-1]) {
        swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
        swap=end[i]; end[i]=end[i-1]; end[i-1]=swap;
      }
    } else { i--; }
  }
}

/* QuickSort algorithm for a FLOAT_DMEM array with nEl elements */
void smileUtil_quickSort_FLOATDMEM(FLOAT_DMEM *arr, long nEl)
{
  #ifndef MAX_LEVELS
  #define MAX_LEVELS  300
  #endif

  FLOAT_DMEM piv;
  long beg[MAX_LEVELS], end[MAX_LEVELS],swap;
  long i=0, L, R;
  
  beg[0]=0; end[0]=nEl;
  while (i>=0) {
    L=beg[i]; R=end[i]-1;
    if (L<R) {
      piv=arr[L];
      while (L<R) {
        while (arr[R]>=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]<=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; }
      arr[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
      if (end[i]-beg[i]>end[i-1]-beg[i-1]) {
        swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
        swap=end[i]; end[i]=end[i-1]; end[i-1]=swap;
      }
    } else { i--; }
  }
}

/* Reverse the order in an array of elements, i.e. swap first and last element, etc. */
void smileUtil_reverseOrder_FLOATDMEM(FLOAT_DMEM *arr, long nEl)
{
  long i; int range;
  FLOAT_DMEM tmp;
  if (nEl % 2 == 0) {
    range = nEl>>1;
  } else {
    range = (nEl-1)>>1;
  }
  for (i=0; i<nEl>>1; i++) {
    tmp = arr[i];
    arr[i] = arr[nEl-i-1];
    arr[nEl-i-1] = tmp;
  }
}


/*******************************************************************************************
 ***********************=====   Filter functions   ===== **************************************
 *******************************************************************************************/



/* allocate workspace (history matrix) for a temporal median filter */
FLOAT_DMEM * smileUtil_temporalMedianFilterInit(long N, long T)
{
  FLOAT_DMEM *ws = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*(N*(T+1)+2+T));
  // NOTE: first two floats of workspace are N and T
  if (ws != NULL) {
    ws[0] = (FLOAT_DMEM)N;
    ws[1] = (FLOAT_DMEM)T;
  }
  return ws;
}



/* allocate workspace (history matrix) for a temporal median filter */
FLOAT_DMEM * smileUtil_temporalMedianFilterInitSl(long N, long Ns, long T)
{
  FLOAT_DMEM *ws = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*(N*(Ns+1)*(T+1)+2+2*T));
  // NOTE: first two floats of workspace are N and T
  if (ws != NULL) {
    ws[0] = (FLOAT_DMEM)(N*(Ns+1));
    ws[1] = (FLOAT_DMEM)T;
  }
  return ws;
}

/* free the temporal median filter workspace and return NULL */
FLOAT_DMEM * smileUtil_temporalMedianFilterFree(FLOAT_DMEM *workspace)
{
  if (workspace != NULL) free(workspace);
  return NULL;
}

/*
  Perform median filter of each element in frame x (over time) using a history matrix given in *workspace
  The workspace must be created with smileUtil_temporalMedianFilterInit.
  workspace : ptr el0 el0 el0(t-1)... el0(t) ; ptr el1 el1 el1(t-1) ... el1(t)
*/
void smileUtil_temporalMedianFilter(FLOAT_DMEM *x, long N, FLOAT_DMEM *workspace)
{
  long i;
  long _N;
  long Nw;
  long T, T1;
  FLOAT_DMEM *ws;

  if (workspace == NULL) return;
  if (N<=0) return;
  
  
  // check for matching N and find minimal _N we will work with
  Nw = (long)workspace[0];
  if (Nw > N) _N = N;
  else _N = Nw;
  T = (long)workspace[1];
  T1 = T+1;

  ws = workspace + Nw*(T+1)+2;

  for (i=0; i<_N; i++) { // apply median filter to each element 0.._N-1
    long ws0 = i*T1+2;

    // add new element to history
    long ptr = (long)(workspace[ws0])+1;
    workspace[ws0+(ptr++)] = x[i];
    if (ptr > T) ptr = 1;
    workspace[ws0] = (FLOAT_DMEM)(ptr-1);

    // compute median and save in vector x
    x[i] = smileMath_median(&(workspace[ws0+1]), T, ws);
  }
}

/*
  Perform median filter of each element in frame x (over time) using a history matrix given in *workspace
  The workspace must be created with smileUtil_temporalMedianFilterInit.
  workspace : ptr el0 el0 el0(t-1)... el0(t) ; ptr el1 el1 el1(t-1) ... el1(t)
  **> Filter with slave data (Ns is number of slave elements for each element in x (total size of x thus is N*Ns))
      The workspace must be allocated for N*(Ns+1) elements!
*/
void smileUtil_temporalMedianFilterWslave(FLOAT_DMEM *x, long N, long Ns, FLOAT_DMEM *workspace)
{
  long i,j;
  long _N;
  long Nw;
  long T, T1;
  FLOAT_DMEM *ws;

  if (workspace == NULL) return;
  if (N<=0) return;
  
  
  // check for matching N and find minimal _N we will work with
  Nw = (long)workspace[0];
  if (Nw > N) _N = N;
  else _N = Nw;
  T = (long)workspace[1];
  T1 = T+1;

  ws = workspace + Nw*(T+1)+2;

  for (i=0; i<_N; i++) { // apply median filter to each element 0.._N-1
    long ws0 = i*T1+2;

    // add new element to history
    long ptr = (long)(workspace[ws0])+1;
    workspace[ws0+(ptr++)] = x[i];
    if (ptr > T) ptr = 1;
    workspace[ws0] = (FLOAT_DMEM)(ptr-1);

    // add slave elements to history
    if (Nw >= N*(Ns+1)) {
      for (j=1; j<=Ns; j++) {
        long ws0 = (i+j*Nw/(Ns+1))*T1+2;
        long ptr = (long)(workspace[ws0])+1;
        workspace[ws0+(ptr++)] = x[i+j*N];
        if (ptr > T) ptr = 1;
        workspace[ws0] = (FLOAT_DMEM)(ptr-1);
      }
    }

    // compute median and save in vector x
    x[i] = smileMath_medianOrdered(&(workspace[ws0+1]), T, ws);

    // use indicies in ws to sort slave data (if workspace is large enough)
    if (Nw >= N*Ns) {
      for (j=1; j<=Ns; j++) {
        if (T&1) { // odd
          long ws0 = (i+j*Nw/(Ns+1))*T1+2;
          long ptr = (long)(workspace[ws0]+(FLOAT_DMEM)1.0-ws[0]);
          if (ptr < 1) ptr += T;
          x[i+j*N] = workspace[ws0+ptr]; 
        } else { // even
          long ws0 = (i+j*Nw/(Ns+1))*T1+2;
          long ptr0 = (long)(workspace[ws0]+(FLOAT_DMEM)1.0-ws[0]);
          long ptr1 = (long)(workspace[ws0]+(FLOAT_DMEM)1.0-ws[1]);
          if (ptr0 < 1) ptr0 += T;
          if (ptr1 < 1) ptr1 += T;
          x[i+j*N] = (FLOAT_DMEM)0.5 * (workspace[ws0+ptr0] + workspace[ws0+ptr1]);
        }
      }
    }

  }
}


/*******************************************************************************************
 ***********************=====   Math functions   ===== **************************************
 *******************************************************************************************/

/*
  median of vector x
  (workspace can be a pointer to an array of N FLOAT_DMEMs which is used to sort the data in x without changing x)
  (if workspace is NULL , the function will allocate and free the workspace internally)
*/
FLOAT_DMEM smileMath_median(const FLOAT_DMEM *x, long N, FLOAT_DMEM *workspace)
{
  long i;
  FLOAT_DMEM median=0.0;
  FLOAT_DMEM *tmp = workspace;
  if (tmp == NULL) tmp = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*N);
  if (tmp==NULL) return 0.0;
  for (i=0; i<N; i++) { tmp[i] = x[i]; }
  //memcpy(tmp, x, sizeof(FLOAT_DMEM)*N);
  
  smileUtil_quickSort_FLOATDMEM(tmp,N);
  if (N&1) { // easy median for odd N
    median = tmp[N>>1];
  } else { // median as mean of the two middle elements for even N
    median = (FLOAT_DMEM)0.5 * (tmp[N/2]+tmp[N/2-1]);
  }
  if (workspace == NULL) free(tmp);
  return median;
}

/*
  median of vector x
  (workspace can be a pointer to an array of 2*N (!) FLOAT_DMEMs which is used to sort the data in x without changing x)
  (if workspace is NULL , the function will allocate and free the workspace internally)
  THIS function should return the original vector index of the median in workspace[0] (and workspace[1] if N is even), to use this functionality you must provide a workspace pointer!
*/
FLOAT_DMEM smileMath_medianOrdered(const FLOAT_DMEM *x, long N, FLOAT_DMEM *workspace)
{
  long i,j;
  long oi0=0, oi1=0;
  FLOAT_DMEM median=0.0;
  FLOAT_DMEM *tmp = workspace;
  if (tmp == NULL) tmp = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*2*N);
  if (tmp==NULL) return 0.0;

  
  for (i=0; i<N; i++) { tmp[i] = x[i]; }
  //memcpy(tmp, x, sizeof(FLOAT_DMEM)*N);

  for (i=0; i<N; i++) {
    tmp[N+i] = (FLOAT_DMEM)i;
  }
  
  // we cannot use quicksort, since it doesn't preserve the original indexing
  //smileUtil_quickSort_FLOATDMEM(tmp,N);
  for (i=0; i<N; i++) {
    for (j=i+1; j<N; j++) {
      if (tmp[i] > tmp[j]) { //swap data and indicies
        FLOAT_DMEM t = tmp[i]; // swap data
        tmp[i] = tmp[j];
        tmp[j] = t;
        t = tmp[i+N]; // swap indicies
        tmp[i+N] = tmp[j+N];
        tmp[j+N] = t;
      }
    }
  }

  if (N&1) { // easy median for odd N
    median = tmp[N>>1];
    tmp[0] = tmp[N+(N>>1)];
  } else { // median as mean of the two middle elements for even N
    median = (FLOAT_DMEM)0.5 * (tmp[N>>1]+tmp[(N>>1)-1]);
    tmp[0] = tmp[N+(N>>1)-1];
    tmp[1] = tmp[N+(N>>1)];
  }
  if (workspace == NULL) free(tmp);
  return median;
}

/* check if number is power of 2 (positive or negative) */
long smileMath_isPowerOf2(long x)
{
  if (x==1) return 1;  // 1 is a power of 2
  if (((x&1) == 0)&&(x != 0)) { // only even numbers > 1
    x=x>>1;
    while ((x&1) == 0) { x=x>>1;  }
    return ((x==1)||(x==-1));
  }
  return 0;
}

/* round to nearest power of two */
long smileMath_roundToNextPowOf2(long x)
{
  // round x up to nearest power of 2
  unsigned long int flng = (unsigned long int)x;
  unsigned long int fmask = 0x8000;
  while ( (fmask & flng) == 0) { fmask = fmask >> 1; }
  // fmask now contains the MSB position
  if (fmask > 1) {
    if ( (fmask>>1)&flng ) { flng = fmask<<1; }
    else { flng = fmask; }
  } else {
    flng = 2;
  }

  return (long)flng;
}

/* round up to next power of 2 */
long smileMath_ceilToNextPowOf2(long x)
{
  long y = smileMath_roundToNextPowOf2(x);
  if (y<x) y *= 2;
  return y;
}

/* round down to next power of two */
long smileMath_floorToNextPowOf2(long x)
{
  long y = smileMath_roundToNextPowOf2(x);
  if (y>x) y /= 2;
  return y;
}

/***** vector math *******/

/* compute euclidean norm of given vector x */
FLOAT_DMEM smileMath_vectorLengthEuc(FLOAT_DMEM *x, long N)
{
  long i; FLOAT_DMEM norm = 0.0;
  for (i=0; i<N; i++) norm += x[i]*x[i];
  return (FLOAT_DMEM)sqrt(norm);
}

/* compute L1 norm (absolute sum) of given vector x */
FLOAT_DMEM smileMath_vectorLengthL1(FLOAT_DMEM *x, long N)
{
  long i; FLOAT_DMEM norm = 0.0;
  for (i=0; i<N; i++) norm += (FLOAT_DMEM)fabs(x[i]);
  return norm;
}

/* normalise euclidean length of x to 1 */
FLOAT_DMEM smileMath_vectorNormEuc(FLOAT_DMEM *x, long N)
{
  FLOAT_DMEM norm = smileMath_vectorLengthEuc(x,N);
  long i; 
  if (norm > 0.0) for (i=0; i<N; i++) x[i] /= norm;
  return norm;
}

/* normalise vector sum to 1 */
FLOAT_DMEM smileMath_vectorNormL1(FLOAT_DMEM *x, long N)
{
  FLOAT_DMEM norm = smileMath_vectorLengthL1(x,N);
  long i; 
  if (norm > 0.0) for (i=0; i<N; i++) x[i] /= norm;
  return norm;
}

/* normalise values of vector x to range [min - max] */
void smileMath_vectorNormMax(FLOAT_DMEM *x, long N, FLOAT_DMEM min, FLOAT_DMEM max)
{
  long i;
  FLOAT_DMEM _min=x[0];
  FLOAT_DMEM _max=x[0];
  FLOAT_DMEM scale;
  for (i=0; i<N; i++) {
    if (x[i] < _min) _min = x[i];
    else if (x[i] > _max) _max = x[i];
  }
  if (_max==_min) scale = 1.0;
  else scale = (max-min)/(_max-_min);
  for (i=0; i<N; i++) {
    x[i] = (x[i]-_min)*scale+min;
  }
}

/* compute the arithmetic mean of vector x */
FLOAT_DMEM smileMath_vectorAMean(FLOAT_DMEM *x, long N)
{
  long i; FLOAT_DMEM sum = 0.0;
  for (i=0; i<N; i++) sum += x[i];
  return sum / (FLOAT_DMEM)N;
}

/* root of each element in a vector */
void smileMath_vectorRoot(FLOAT_DMEM *x, long N)
{
  long i;
  for (i=0; i<N; i++) { if (x[i]>=(FLOAT_DMEM)0.0) x[i]=(FLOAT_DMEM)sqrt(x[i]); }
}

/* root of each element in a vector */
void smileMath_vectorRootD(double *x, long N)
{
  long i;
  for (i=0; i<N; i++) { if (x[i]>=0.0) x[i]=sqrt(x[i]); }
}

/**** complex number math ****/

/* compute A/B , store in C */
void smileMath_complexDiv(double ReA, double ImA, double ReB, double ImB, double *ReC, double *ImC)
{
  double r, den;
  double R=0,I=0;

  if (fabs (ReB) >= fabs (ImB)) {
    if (ReB != 0.0) {
      r = ImB / ReB;
      den = ReB + r * ImB;
      if (den != 0.0) {
        R = (ReA + ImA*r ) / den; // R = (ReA  + r*ImA ) / den;
        I = (ImA - r*ReA) / den; // I = (ImA * ReB - r * ReA) / den;
      }
    }
  } else {
    if (ImB != 0.0) {
      r = ReB / ImB;
      den = ImB + r * ReB;
      if (den != 0.0) {
        R = (ReA * r + ImA) / den;
        I = (ImA * r - ReA) / den;
      }
    }
  }
  if (ReC != NULL) *ReC = R;
  if (ImC != NULL) *ImC = I;
}

/* fix roots to inside the unit circle */
// ensure all roots are within the unit circle
// if abs(root) > 1  (outside circle)
// then root = 1 / root*
//
// *roots is an array of n complex numbers (2*n doubles)
void smileMath_complexIntoUnitCircle(double *roots, int n)
{
  long i;
  for (i=0; i<n; i++) {
    long i2 = i*2;
    // if abs(root) > 1.0 
    if (smileMath_complexAbs(roots[i2],roots[i2+1]) > 1.0) {
      // root = 1.0 / root*
      smileMath_complexDiv(1.0 , 0.0 , roots[i2], -roots[i2+1], &roots[i2], &roots[i2+1]);
    }
  }
}

/***** spline interpolation *****/


/********** SHS and spline code partially taken from :  *****************/
/* Sound_to_Pitch2.c / NUM2.c
 *
 * Copyright (C) 1993-2007 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Adapted for integration into openSMILE by Florian Eyben, 2010 
 * TUM MMK, Munich Germany 
 *
 */

/*
        Given arrays a[1..n] and y[1..n] containing a tabulated function, i.e.,
        y[i] = f(x[i]), with x[1] < x[2] < ... < x[n], and given values yp1 and
        ypn for the first derivative of the interpolating function at point
        1 and n, respectively, this routine returns an array y2[1..n] that
        contains the second derivative of the interpolating function at the
        tabulated point x.
        If yp1 and/or ypn are greaterequal 10^30, the routine is signaled to
        set the corresponding boundary condition for a natuarl spline, with
        zero second derivative on that boundary.
*/
// u: is optional pointer to a workspace pointer (smileMath_spline will allocate
//    a vector there if the pointer pointed to is NULL
//    the calling code is responsible of freeing this memory with free() at any 
//    later time which seems convenient (i.e. at the end of all calculations)
int smileMath_spline (const double *x, const double *y, long n, double yp1, double ypn, double *y2, double **workspace)
{
  double p, qn, sig, un, *u;
  long i,k;

  if (workspace!=NULL) u = *workspace;
  if (u==NULL) u = (double*)malloc(sizeof(double)*(n-1));

  if (yp1 > 0.99e30) {
    y2[0] = u[0] = 0.0;
  } else {
    y2[0] = -0.5;
    u[0] = (3.0 / (x[1] - x[0])) * ((y[1] - y[0]) / (x[1] - x[0]) - yp1);
  }

  for (i=1; i < n-1; i++) { // for (i=2; i <= n-1; i++) {
    sig = (x[i] - x[i-1]) / (x[i+1] - x[i-1]);
    p = sig * y2[i-1] + 2.0;
    y2[i] = (sig - 1.0) / p;
    u[i] = (y[i+1] - y[i]) / (x[i+1] - x[i]) - (y[i] - y[i-1]) / (x[i] - x[i-1]);
    u[i] = (6.0 * u[i] / (x[i+1] - x[i-1]) - sig * u[i-1]) / p;
  }

  if (ypn > 0.99e30) {
    qn = un = 0.0;
  } else {
    qn = 0.5;
    un = (3.0 / (x[n-1] - x[n-2])) * (ypn - (y[n-1] - y[n-2]) / (x[n-1] - x[n-2]));
  }

  y2[n-1] = (un - qn * u[n-2]) / (qn * y2[n-2] + 1.0);
  for (k=n-2; k >= 0; k--) { // for (k=n-1; k >= 1; k--) {
    y2[k] = y2[k] * y2[k+1] + u[k];
  }

  if (workspace == NULL) {
    free(u);
  } else {
    *workspace = u;
  }
  
  return 1;
}


/* spline interpolation
        Given arrays xa[1..n] and ya[1..n] containing a tabulated function,
        i.e., y[i] = f(x[i]), with x[1] < x[2] < ... < x[n], and given the
        array y2a[1..n] which is the output of NUMspline above, and given
        a value of x, this routine returns an interpolated value y.
*/
int smileMath_splint (double xa[], double ya[], double y2a[], long n, double x, double *y)
{
  long klo, khi, k;
  double h, b, a;

  klo = 1; khi = n;
  while (khi-klo > 1)
  {
    k = (khi + klo) >> 1;
    if (xa[k-1] > x) khi = k;
    else klo = k;
  }
  khi--; klo--;
  h = xa[khi] - xa[klo];
  if (h == 0.0) {
    printf("smileMath: splint() : bad input value (h=0)\n");
    return 0;
  }
  a = (xa[khi] - x) / h;
  b = (x - xa[klo]) / h;
  *y = a * ya[klo] + b * ya[khi]+((a * a * a - a) * y2a[klo] +
    (b * b * b - b) * y2a[khi]) * (h * h) / 6.0;
  return 1;
}

/***************** end spline thirdparty code ************/


// constructs a parabola from three points (parabolic interpolation)
// returns: peak x of parabola, and optional (if not NULL) the y value of the peak in *y and the steepness in *_a
double smileMath_quadFrom3pts(double x1, double y1, double x2, double y2, double x3, double y3, double *y, double *_a)
{
  double den = x1*x1*x2 + x2*x2*x3 + x3*x3*x1 - x3*x3*x2 - x2*x2*x1 - x1*x1*x3;
  if (den != 0.0) {
    double a = (y1*x2 + y2*x3 + y3*x1 - y3*x2 - y2*x1 - y1*x3)/den;
    double b = (x1*x1*y2 + x2*x2*y3 + x3*x3*y1 - x3*x3*y2 - x2*x2*y1 - x1*x1*y3) / den;
    double c = (x1*x1*x2*y3 + x2*x2*x3*y1 + x3*x3*x1*y2 - x3*x3*x2*y1 - x2*x2*x1*y3 - x1*x1*x3*y2) / den;
    if (a != 0.0) {
      double x;
      if (_a != NULL) *_a = a;
      x = -b/(2.0*a);
      if (y!=NULL) *y = c - a*x*x;
      return x;
    } 
  } 
      
  // fallback to peak picking if we can't construct a parabola
  if (_a!=NULL) *_a = 0.0;
  if ((y1>y2)&&(y1>y3)) { if (y!=NULL) *y = y1; return x1; }
  else if ((y2>y1)&&(y2>y3)) { if (y!=NULL) *y = y2; return x2; }
  else if ((y3>y1)&&(y3>y2)) { if (y!=NULL) *y = y3; return x3; }
  
  // fallback to keep compiler happy.. this will only happen if all input values are equal:
  if (y!=NULL) *y = y1; return x1;
}


/*******************************************************************************************
 ***********************=====   DSP functions   ===== **************************************
 *******************************************************************************************/

/* Hermansky 1990, JASA , scale corrected to max == 1 */
double smileDsp_equalLoudnessWeight(double frequency) 
{
  double w = 2.0*M_PI*frequency;
  double w2 = w*w;
  double c = w2 + 6300000.0;
  if (c > 0.0) {
  //  printf("x = %f\n",((w2 +56.8*1000000.0)*w2*w2 ) / (c*c * (w2 + 380000000.0)));
    //return 1.755e27*((w2 +56.8e6)*w2*w2 ) / (c*c * (w2 + 0.38e9)*(w2*w2*w2 + 9.58e26));
    return ( 1e32 * ((w2 +56.8e6)*w2*w2 ) / (c*c * (w2 + 0.38e9)*(w2*w2*w2*w + 1.7e31))  );
  } else {
    return 0.0;
  }
}

double smileDsp_equalLoudnessWeight_htk(double frequency) 
{
  /* HTK version of the PLP equal loudness curve:*/
  double f2 = (frequency * frequency);
  double fs = f2/(f2 + 1.6e5);
  return fs * fs * ((f2 + 1.44e6)/(f2 + 9.61e6));
}

  /*======= spectral scaling functions === */

// +++ forward scale transformation function (source (linear) -> target) +++
// 'x' : is the (linear) source frequency in Hertz
// 'scale' : is one of the following:
//   SPECTSCALE_LOG, SPECTSCALE_SEMITONE, SPECTSCALE_BARK, 
//   SPECTSCALE_BARK_SCHROED, SPECTSCALE_BARK_SPEEX, SPECTSCALE_MEL
// 'param' is either:
//   logScaleBase if scale == SPECTSCALE_LOG
//   firstNote    if scale == SPECTSCALE_SEMITONE
double smileDsp_specScaleTransfFwd(double x, int scale, double param)
{
  switch(scale) {
    case SPECTSCALE_LOG: 
      return log(x)/log(param); // param = logScaleBase
    case SPECTSCALE_SEMITONE:
      if (x/param>1.0) // param = firstNote
        return 12.0 * smileMath_log2(x / param); // param = firstNote
      else return 0.0;
    case SPECTSCALE_BARK: // Bark scale according to : H. Traunmüller (1990) "Analytical expressions for the tonotopic sensory scale" J. Acoust. Soc. Am. 88: 97-100.   
      if (x>0) {
        return (26.81 / (1.0 + 1960.0/x)) - 0.53;
      } else return 0.0;
    case SPECTSCALE_BARK_SCHROED:
      if (x>0) {
        double f6 = x/600.0;
        return (6.0 * log(f6 + sqrt(f6*f6 + 1.0) ) );
      } else return 0.0;
    case SPECTSCALE_BARK_SPEEX:
        return (13.1*atan(.00074*x)+2.24*atan(x*x*1.85e-8)+1e-4*x);
    case SPECTSCALE_MEL: // Mel scale according to: L.L. Beranek (1949) Acoustic Measurements, New York: Wiley. 
      if (x>0.0) {
        return 1127.0 * log(1.0 + x/700.0);
      } else return 0.0;
    case SPECTSCALE_LINEAR: 
    default:
      return x;
  }
  return x;
}

// +++ inverse scale transformation function (linear -> target) +++
// 'x' : is the (non-linear) source frequency in Bark/Mel/etc. scale ...
// 'scale' : is one of the following:
//   SPECTSCALE_LOG, SPECTSCALE_SEMITONE, SPECTSCALE_BARK, 
//   SPECTSCALE_BARK_SCHROED, SPECTSCALE_BARK_SPEEX, SPECTSCALE_MEL
// 'param' is either:
//   logScaleBase if scale == SPECTSCALE_LOG
//   firstNote    if scale == SPECTSCALE_SEMITONE
// return value: linear frequency in Hertz
double smileDsp_specScaleTransfInv(double x, int scale, double param)
{
  switch(scale) {
    case SPECTSCALE_LOG: 
      return exp(x * log(param)); // param = logScaleBase
    case SPECTSCALE_SEMITONE:
      return param * pow(2.0, x/12.0); // param = firstNote
    case SPECTSCALE_BARK: { // Bark scale according to : H. Traunmüller (1990) "Analytical expressions for the tonotopic sensory scale" J. Acoust. Soc. Am. 88: 97-100.   
      double z0 = (x+0.53)/26.81;
      if (z0 != 1.0) return (1960.0 * z0)/(1.0-z0);
      else return 0.0;
      }
    case SPECTSCALE_BARK_SCHROED:
      return 600.0 * sinh(x/6.0);
      //return 0.0;
    case SPECTSCALE_BARK_SPEEX:
      fprintf(stderr,"smileDsp_specScaleTransfInv: SPECTSCALE_BARK_SPEEX: inversion not yet implemented");
    case SPECTSCALE_MEL :  // Mel scale according to: L.L. Beranek (1949) Acoustic Measurements, New York: Wiley. 
      return 700.0*(exp(x/1127.0)-1.0);        
    case SPECTSCALE_LINEAR:
    default:
      return x;
  }
  return x;
}


  /*======= window functions ==========*/

/* rectangular window */
double * smileDsp_winRec(long _N)
{
  int i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  for (i=0; i<_N; i++) {
    *x = 1.0; x++;
  }
  return ret;
}

/* triangular window (non-zero endpoints) */
double * smileDsp_winTri(long _N)
{
  long i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  for (i=0; i<_N/2; i++) {
    *x = 2.0*(double)(i+1)/(double)_N;
    x++;
  }
  for (i=_N/2; i<_N; i++) {
    *x = 2.0*(double)(_N-i)/(double)_N;
    x++;
  }
  return ret;
}

/* powered triangular window (non-zero endpoints) */
double * smileDsp_winTrP(long _N)
{
  double *w = smileDsp_winTri(_N);
  double *x = w;
  long n; for (n=0; n<_N; n++) *x = *x * (*(x++));
  return w;
}

/* bartlett (triangular) window (zero endpoints) */
double * smileDsp_winBar(long _N)
{
  long i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  for (i=0; i<_N/2; i++) {
    *x = 2.0*(double)(i)/(double)(_N-1);
    x++;
  }
  for (i=_N/2; i<_N; i++) {
    *x = 2.0*(double)(_N-1-i)/(double)(_N-1);
    x++;
  }
  return ret;
}

/* hann(ing) window */
double * smileDsp_winHan(long _N)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  for (i=0.0; i<NN; i += 1.0) {
    *x = 0.5*(1.0-cos( (2.0*M_PI*i)/(NN-1.0) ));
    x++;
  }
  return ret;
}

/* hamming window */
double * smileDsp_winHam(long _N)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  for (i=0.0; i<NN; i += 1.0) {
/*    *x = 0.53836 - 0.46164 * cos( (2.0*M_PI*i)/(NN-1.0) ); */
    *x = 0.54 - 0.46 * cos( (2.0*M_PI*i)/(NN-1.0) );
    x++;
  }
  return ret;
}

/* half-wave sine window (cosine window) */
double * smileDsp_winSin(long _N)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  for (i=0.0; i<NN; i += 1.0) {
    *x = sin( (1.0*M_PI*i)/(NN-1.0) );
    x++;
  }
  return ret;
}

/* Lanczos window */
double * smileDsp_winLac(long _N)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  for (i=0.0; i<NN; i += 1.0) {
    *x = smileDsp_lcSinc( (2.0*i)/(NN-1.0) - 1.0 );
    x++;
  }
  return ret;
}

/* gaussian window ...??? */
double * smileDsp_winGau(long _N, double sigma)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  double tmp;
  if (sigma <= 0.0) sigma = 0.01;
  if (sigma > 0.5) sigma = 0.5;
  for (i=0.0; i<NN; i += 1.0) {
    tmp = (i-(NN-1.0)/2.0)/(sigma*(NN-1.0)/2.0);
    *x = exp( -0.5 * ( tmp*tmp ) );
    x++;
  }
  return ret;
}

/* Blackman window */
double * smileDsp_winBla(long _N, double alpha0, double alpha1, double alpha2)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  double tmp;
  for (i=0.0; i<NN; i += 1.0) {
    tmp = (2.0*M_PI*i)/(NN-1.0);
    *x = alpha0 - alpha1 * cos( tmp ) + alpha2 * cos( 2.0*tmp );
    x++;
  }
  return ret;

}

/* Bartlett-Hann window */
double * smileDsp_winBaH(long _N, double alpha0, double alpha1, double alpha2)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  for (i=0.0; i<NN; i += 1.0) {
    *x = alpha0 - alpha1 * fabs( i/(NN-1.0) - 0.5 ) - alpha2 * cos( (2.0*M_PI*i)/(NN-1.0) );
    x++;
  }
  return ret;
}

/* Blackman-Harris window */
double * smileDsp_winBlH(long _N, double alpha0, double alpha1, double alpha2, double alpha3)
{
  double i;
  double * ret = (double *)malloc(sizeof(double)*_N);
  double * x = ret;
  double NN = (double)_N;
  double tmp;
  for (i=0.0; i<NN; i += 1.0) {
    tmp = (2.0*M_PI*i)/(NN-1.0);
    *x = alpha0 - alpha1 * cos( tmp ) + alpha2 * cos( 2.0*tmp ) - alpha3 * cos( 3.0*tmp );
    x++;
  }
  return ret;
}

/****** other dsp functions *******/

/* compute harmonic product spectrum from a linear scale magnitude spectrum, use up to maxMul down-scaled spectra */
long smileDsp_harmonicProductLin(const FLOAT_DMEM *src, long Nsrc, FLOAT_DMEM *dst, long Ndst, int maxMul)
{
  long i; int m;
  long oLen = Ndst;
  if (Nsrc < oLen) oLen = Nsrc;
  for (i=0; i<oLen; i++) {
    dst[i] = src[i];
    for (m=2; m<=maxMul; m++) {
      long idx = i*m;
      if (idx < Nsrc) dst[i] *= src[idx];
    }
  }
  return oLen;
}

/* compute harmonic sum spectrum from a linear scale magnitude spectrum, use up to maxMul down-scaled spectra */
long smileDsp_harmonicSumLin(const FLOAT_DMEM *src, long Nsrc, FLOAT_DMEM *dst, long Ndst, int maxMul)
{
  long i; int m;
  long oLen = Ndst;
  if (Nsrc < oLen) oLen = Nsrc;
  for (i=0; i<oLen; i++) {
    dst[i] = src[i];
    for (m=2; m<=maxMul; m++) {
      long idx = i*m;
      if (idx < Nsrc) dst[i] += src[idx]/(FLOAT_DMEM)m;
    }
  }
  return oLen;
}

/* compute LPC coefficients with Burg's method (N. Anderson (1978)):
     N. Anderson (1978), "On the calculation of filter coefficients for maximum entropy spectral analysis", in Childers, Modern Spectrum Analysis, IEEE Press, 252-255.
   x : wave samples, 
   n : number of samples
   a : array to hold the coefficients
   m : number of coefficients desired
   gain : optional pointer to FLOAT_DMEM, which will be filled with the computed LPC gain
   burgB1, burgB2, burgAA : pointers to pointers to work area memory, 
     will be initialised automtically on first use, 
     must be freed by calling application on exit ;
     if these pointers are NULL, calcLpcBurg will allocate 
     the work area at the beginning of the function and
     free it at the end of the function 
     (the latter is ineffective if memory allocation is slow)
 */
/* Modified by Florian Eyben, 2010
 *   integration into openSMILE, array indicies all start at 0 
 *                               instead of 1 (as in original)
 *
 * Original: 
 *   NUMburg function taken from praat source code (NUM2.c / Sound_and_LPC.c)
 *
 * Copyright (C) 1994-2008 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
int smileDsp_calcLpcBurg (const FLOAT_DMEM *x, long n, FLOAT_DMEM *a, int m, FLOAT_DMEM *gain, FLOAT_DMEM **burgB1, FLOAT_DMEM **burgB2, FLOAT_DMEM **burgAA)
{
  long i = 1, j; int status = 0;
  FLOAT_DMEM p = 0.0;
  FLOAT_DMEM *aa = NULL;
  FLOAT_DMEM xms = 0.0;
  FLOAT_DMEM *b1 = NULL, *b2 = NULL;

  if (x == NULL) return 0;
  if (a == NULL) return 0;
  if ((n<m)||(m<=0)) return 0;
  
  if (burgB1 != NULL) b1 = *burgB1;
  if (b1 == NULL) b1 = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*n);

  if (burgB2 != NULL) b2 = *burgB2;
  if (b2 == NULL) b2 = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*n);

  if (burgAA != NULL) aa = *burgAA;
  if (aa == NULL) aa = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*m);

  /* (3) */
  for (j = 0; j < n; j++) {  // for (j = 1; j <= n; j++) {
    p += x[j] * x[j];
  }

  xms = p / n;
  if (xms <= 0) goto end;

  /* (9) */
  b1[0] = x[0];       // b1[1] = x[1];
  b2[n - 2] = x[n-1]; // b2[n - 1] = x[n];
  for (j = 1; j < n - 1; j++) { // for (j = 2; j <= n - 1; j++) {
    b1[j] = b2[j - 1] = x[j];
  }

  for (i = 0; i < m; i++) { // for (i = 1; i <= m; i++) {

    /* (7) */
    FLOAT_DMEM num = 0.0, denum = 0.0;
    for (j = 0; j < n - i - 1; j++) {  // for (j = 1; j <= n - i; j++) {  
      num += b1[j] * b2[j];
      denum += b1[j] * b1[j] + b2[j] * b2[j];
    }

    if (denum <= 0) goto end;

    a[i] = (FLOAT_DMEM)2.0 * num / denum;

    /* (10) */
    xms *= (FLOAT_DMEM)1.0 - a[i] * a[i];

    /* (5) */
    for (j = 0; j < i; j++) {  // for (j = 1; j <= i - 1; j++) {    
      a[j] = aa[j] - a[i] * aa[i - j - 1]; //  aa[i-j]
    }

    if (i < m-1) { // if (i < m) {
    
      /* (8) */
      /* Watch out: i -> i+1 */
      for (j = 0; j <= i; j++) { // for (j = 1; j <= i; j++) {   
        aa[j] = a[j];
      }
      for (j = 0; j < n - i - 2; j++) { // for (j = 1; j <= n - i - 1; j++)
        b1[j] -= aa[i] * b2[j];
        b2[j] = b2[j + 1] - aa[i] * b1[j + 1];
      }
    }
  }

  status = 1;

end:
  if (burgB1 != NULL) *burgB1 = b1;
  else if (b1 != NULL) free(b1);
  if (burgB2 != NULL) *burgB2 = b2;
  else if (b2 != NULL) free(b2);
  if (burgAA != NULL) *burgAA = aa;
  else if (aa != NULL) free(aa);

  for (j = 0; j < i; j++) a[j] = -a[j]; // invert coefficients for compatibility with ACF method's lpcs
  for (j = i; j < m; j++) a[j] = 0.0;   // pad remaining coefficients with zeroes
  // adjust gain:
  if (gain != NULL) *gain = xms * (FLOAT_DMEM)n;
  return status;
}

/********** SHS and spline code partially taken from :  *****************/
/* Sound_to_Pitch2.c / NUM2.c
 *
 * Copyright (C) 1993-2007 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Adapted for integration into openSMILE by Florian Eyben, 2010 
 * TUM MMK, Munich Germany 
 *
 */

/* peak enhancement in a linear magnitude spectrum */
int smileDsp_specEnhanceSHS (double *a, long n)
{
  long i, j, nmax = 0, *posmax;
  posmax = (long *)calloc(1,sizeof(long)*( (n + 1) / 2 + 1/*?*/ ));
  if ( (n < 2) || (posmax == NULL)) return 0;

  if (a[0] > a[1]) posmax[nmax++] = 0;
  
  for (i=1; i < n-1; i++) {  // for (i=2; i <= n-1; i++) {
    if (a[i] > a[i-1] && a[i] >= a[i+1]) {
      posmax[nmax++] = i; // ++nmax] = i;
    }
  }

  if (a[n-1] > a[n-2]) posmax[nmax++] = n-1;

  if (nmax == 1) {
    for (j=0; j <= posmax[1]-3; j++) a[j] = 0; // for (j=1; j <= posmax[1]-3; j++) a[j] = 0;
    for (j=posmax[1]+3; j < n; j++) a[j] = 0; // for (j=posmax[1]+3; j <= n; j++) a[j] = 0;
  }
  else {
    for (i=1; i < nmax; i++) { // for (i=2; i <= nmax; i++) {
      for (j=posmax[i-1]+3; j <= posmax[i]-3; j++) a[j] = 0;
    }
  }

  free(posmax);
  return 1;
}

/* smooth a magnitude spectrum (linear) */
void smileDsp_specSmoothSHS (double *a, long n)
{
  double ai, aim1 = 0; long i;
  for (i=0; i < n-1; i++) { // for (i=1; i <= n-1; i++)
    ai = a[i]; a[i] = (aim1 + 2.0 * ai + a[i+1]) / 4.0; aim1 = ai;
  }
}


/********************end thirdparty code********************/


/* convert lp coefficients to cepstra 
   HTKbook , equation 5.11
 */
FLOAT_DMEM smileDsp_lpToCeps(const FLOAT_DMEM *lp, int nLp, FLOAT_DMEM lpGain, FLOAT_DMEM *ceps, int firstCC, int lastCC) 
{
  // CHECK: nCeps <= nLp !
  int i,n;
  int nCeps;
  
  if (firstCC < 1) firstCC = 1;
  if (lastCC > nLp) lastCC = nLp;

  nCeps = lastCC-firstCC+1;

  for (n=firstCC; n<=lastCC; n++) {
  //for (n=1; n<=nCeps; n++) {

    double sum=0;
    for (i=1; i<n; i++) { 
      sum += (n-i)*lp[i-1]*ceps[n-i-1]; 
    }

    ceps[n-firstCC] = -(lp[n-firstCC] + (FLOAT_DMEM)(sum / (double)n));
  }

  if (lpGain <= 0.0) { lpGain = (FLOAT_DMEM)1.0; }
  return (FLOAT_DMEM)(-log(1.0/(double)lpGain));
}


/* autocorrelation in the time domain (used for LPC autocorrelation method) */
void smileDsp_autoCorr(const FLOAT_DMEM *x, const int n, FLOAT_DMEM *acf, int lag)
{
  int i;
  while (lag) {
    acf[--lag] = 0.0;
    for (i=lag; i < n; i++) {
      acf[lag] += x[i] * x[i-lag];
    }
  }
}

#if 0
/* LPC analysis via acf */
int smileDsp_calcLpcAcf(FLOAT_DMEM * acf, FLOAT_DMEM *lpc, int _p, FLOAT_DMEM *gain, FLOAT_DMEM *refl)
{
  int i,j;
  FLOAT_DMEM error = acf[0];
  int errF = 1;
  FLOAT_DMEM rc_i;

  if (lpc == NULL) return 0;
  if (acf == NULL) return 0;

  if ((acf[0] == 0.0)||(acf[0] == -0.0)) {
    for (i=0; i < _p; i++) lpc[i] = 0.0;
    return 0;
  }

  lpc[0] = 1.0; lpc[1] = - acf[1] / acf[0];
  if (refl != NULL) refl[0] = lpc[1];

  error += acf[1] * lpc[1];

  for (i = 1; i < _p; i++) {  // for (i = 2; i <= _p; i++) {
    FLOAT_DMEM s = 0;
    for (j = 0; j <= i; j++) { // for (j = 1; j <= i; j++) {
      s += acf[i-j+1] * lpc[j]; //s += r[i-j+2] * lpc[j];
    }

    rc_i = 0.0;
    if ((error != 0.0)&&(errF)) {
      rc_i = - s / error;

      for (j = 1; j <= (i+1)/2; j++) {  // for (j = 2; j <= i/2+1; j++) {
        FLOAT_DMEM at = lpc[j] + rc_i * lpc[i-j+1];  // FLOAT_DMEM at = lpc[j] + rc_i * lpc[i-j+2];
        lpc[i-j+1] += rc_i * lpc[j];                 // lpc[i-j+2] += rc_i * lpc[j];
        lpc[j] = at;                                 // lpc[j] = at;
      }
    } 
    // save reflection coefficient
    if (refl != NULL) refl[i-1] = rc_i;
    lpc[i] = rc_i; error += rc_i * s;
    if (error < 0.0) errF=0;
  }

  for (i=0; i<_p; i++) lpc[i] = lpc[i+1];

  if (gain != NULL) *gain = error;
  return 1; 
}

/* LPC analysis via acf */
int smileDsp_calcLpcAcfS(FLOAT_DMEM * acf, FLOAT_DMEM *lpc, int _p, FLOAT_DMEM *gain, FLOAT_DMEM *refl)
{
  int i,j;
  FLOAT_DMEM error = acf[0];
  int errF = 1;
  FLOAT_DMEM k;

  if (lpc == NULL) return 0;
  if (acf == NULL) return 0;

  if ((acf[0] == 0.0)||(acf[0] == -0.0)) {
    for (i=0; i < _p; i++) lpc[i] = 0.0;
    return 0;
  }


  for (i = 0; i < _p; i++) {

    /* compute this iteration's reflection coefficient k_m */
    FLOAT_DMEM rr = -(acf[i+1]);
    for (j=0; j < i; j++) {
      rr -= lpc[j] * acf[i-j]; 
    }
    if (error == 0.0) k=0.0;      
    else k = rr/(error);  

    // TODO: save refl. coeff. 
    if (refl != NULL) {
      refl[i] = k;
    }

    /* update LPC coeffs. and error sum */ // ????? check this code
    lpc[i] = k;
    for (j = 0; j < i/2; j++) {
      FLOAT_DMEM tmp  = lpc[j];
      lpc[j]     += k * lpc[i-1-j];
      lpc[i-1-j] += k * tmp;
    }
    if (i & 1) lpc[j] += lpc[j] * k;

    error -= (k*k)*(error); 
    //   printf("err(%i): %f\n",i,error);
  }

  return error;
}
#endif


/* LPC analysis via acf (=implementation of Durbin recursion)*/
int smileDsp_calcLpcAcf(FLOAT_DMEM * r, FLOAT_DMEM *a, int p, FLOAT_DMEM *gain, FLOAT_DMEM *k)
{
  int i,m;
  FLOAT_DMEM e;
  int errF = 1;
  FLOAT_DMEM k_m;
  FLOAT_DMEM *al;

  if (a == NULL) return 0;
  if (r == NULL) return 0;

  if ((r[0] == 0.0)||(r[0] == -0.0)) {
    for (i=0; i < p; i++) a[i] = 0.0;
    return 0;
  }

  al = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*(p));
  
  /* Initialisation, Eq. 158 */
  e = r[0];

  /* The iterations: m=1..p (here), Eq. 159 */
  for (m=1; m<=p; m++) {
    /* Eq. 159 (a) */
    FLOAT_DMEM sum = (FLOAT_DMEM)1.0 * r[m];
    for (i=1; i<m; i++) {
      sum += a[i-1] * r[m-i];
    }
    k_m = ( (FLOAT_DMEM)-1.0 / e ) * sum;

    // save reflection coefficient
    if (k != NULL) k[m-1] = k_m;

    /* Eq. 159 (b) */
    a[m-1] = k_m;

    for (i=1; i<=m/2; i++) {
      FLOAT_DMEM x = a[i-1];
      a[i-1] += k_m * a[m-i-1];
      if ((i < (m/2))||((m&1)==1)) a[m-i-1] += k_m * x;
    }

    // update the error:
    e *= ((FLOAT_DMEM)1.0-k_m*k_m);
    if (e==0.0) {
      for (i=m; i<=p; i++) {
        a[i] = 0.0;
        if (k!=NULL) k[m] = 0.0;
      }
      break;
    }
  }

  free(al);

  if (gain != NULL) *gain=e;
  return 1;
}

// initialise a inverse RDFT work area
// K is number of frequency bins (input)
// I is the number of actual output samples to compute
// nI is the denominator of the sin/cos functions (usually =I) 
sDftWork * smileDsp_initIrdft(long K, long I, double nI, int antialias)
{
  long i,k;
  double pi2 = 2.0*M_PI;
  sDftWork * w = (sDftWork *)malloc(sizeof(sDftWork));

  w->K = K;
  w->I = I;
  if (antialias) {
    w->kMax = K>I ? I : K; // MIN(K,I);
  } else {
    w->kMax = K;
  }
  if (w->kMax & 1) { w->kMax--; } // "round" down to even number

  w->antiAlias = antialias;

  /* fill cos and sin tables: */
  w->costable = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*(w->kMax/2)*I);
  w->sintable = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*(w->kMax/2)*I);

  for (i=0; i<I; i++) {
    long i_n = i*w->kMax/2 - 1;
    if (I>=K)
      w->costable[i_n+K/2] = (FLOAT_DMEM)cos((pi2*(double)((K/2)*i))/nI) ; /* Nyquist */
    
    for (k=2; k<w->kMax; k+=2) {
      double kn = pi2 * (double)(k/2 * i) / nI;
      w->costable[i_n + k/2] = (FLOAT_DMEM)cos(kn); /*real*/
      w->sintable[i_n + k/2] = (FLOAT_DMEM)sin(kn); /*imaginary*/
    }
  }

  return w;
}

// free a DFT work area
sDftWork * smileDsp_freeDftwork(sDftWork * w)
{
  if (w != NULL) {
    if (w->costable != NULL) free(w->costable);
    if (w->sintable != NULL) free(w->sintable);
    free(w);
  }
  return NULL;
}

// perform an arbitrary inverse RDFT (slow version)
void smileDsp_irdft(const FLOAT_DMEM * inData, FLOAT_DMEM *out, sDftWork *w) 
{
  FLOAT_DMEM * costable = w->costable - 1;
  FLOAT_DMEM * sintable = w->sintable - 1;
  long i,k;
  for (i=0; i<w->I; i++) {
    //long i_n = i*w->kMax/2 - 1;
    out[i] = inData[0]; /* DC */
    if (w->I >= w->K) 
      out[i] += inData[1] * costable[w->K/2]; /* Nyquist */

    for (k=2; k<w->kMax; k+=2) {
      long k2 = k>>1;
      out[i] += inData[k] * costable[k2]; /*real*/
      out[i] += inData[k+1] * sintable[k2]; /*imaginary*/
    }
    out[i] /= (FLOAT_DMEM)(w->K/2);
    costable += w->kMax/2;
    sintable += w->kMax/2;
  }
}


sResampleWork * smileDsp_resampleWorkFree(sResampleWork * work)
{
  if (work != NULL) {
    if (work->winF != NULL) free(work->winF);
    if (work->winFo != NULL) free(work->winFo);
    if (work->_w != NULL) free(work->_w);
    if (work->_ip != NULL) free(work->_ip);
    if (work->x != NULL) free(work->x);
    smileDsp_freeDftwork(work->irdftWork);
    free(work);
  }
  return NULL;
}

sResampleWork * smileDsp_resampleWorkInit(long Nin)
{
  sResampleWork * ret = (sResampleWork *)calloc(1,sizeof(sResampleWork));
  ret->x = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*Nin);
  return ret;
}

int smileDsp_doResample(FLOAT_TYPE_FFT *x, long Nin, FLOAT_DMEM *y, long Nout, double nd, sResampleWork ** _work)
{
  long i;
  sResampleWork *work;

  if (x==NULL) return 0;
  if (y==NULL) return 0;
  if (_work == NULL) return 0;
  
  if (*_work==NULL) {
    *_work = smileDsp_resampleWorkInit(Nin);
  }
  work = *_work;

  // window the frame
  if (work->winF == NULL) {
    work->winF = smileDsp_winHan(Nin);
    smileMath_vectorRootD(work->winF,Nin);
  }
  for (i=0; i<Nin; i++) {
    x[i] *= (FLOAT_DMEM)(work->winF[i]);
  }
  // fft the frame
  if (work->_ip == NULL) { work->_ip = (int *)calloc(1,sizeof(int)*(Nin+2)); }
  if (work->_w==NULL) work->_w = (FLOAT_TYPE_FFT *)calloc(1,sizeof(FLOAT_TYPE_FFT)*(Nin*5)/4+2);
  rdft(Nin, 1, x, work->_ip, work->_w);

  // convert from FLOAT_TYPE_FFT to FLOAT_DMEM if necessary
#if FLOAT_FFT_NUM != FLOAT_DMEM_NUM
  for (i=0; i<Nin; i++) {
    work->x[i] = (FLOAT_DMEM)x[i];
  }
  // inverse dft to new sample rate:
  if (work->irdftWork == NULL) { work->irdftWork = smileDsp_initIrdft(Nin, Nout, nd, 1); } 
  smileDsp_irdft(work->x, y, work->irdftWork);
#else
  // inverse dft to new sample rate:
  if (work->irdftWork == NULL) { work->irdftWork = smileDsp_initIrdft(Nin, Nout, nd, 1); } 
  smileDsp_irdft(x, y, work->irdftWork);

  // window again
  if (work->winFo == NULL) {
    work->winFo = smileDsp_winHan(Nout);
    smileMath_vectorRootD(work->winFo,Nout);
  }
  for (i=0; i<Nout; i++) {
    y[i] *= (FLOAT_DMEM)(work->winFo[i]);
  }

#endif
  return 1;
}

/* Implementation of a lattice filter 
   This function processed a single value per call
 k: coefficients
 *b : temporary work area, initialise with 0 at the beginning! (size: sizeof(FLOAT_DMEM)*M ) 
 M: order (number of coefficients)
 in: s(n) input sample
 *bM : optional b(M) result
 return value: f(M) = filter output
 */
FLOAT_DMEM smileDsp_lattice(FLOAT_DMEM *k, FLOAT_DMEM *b, int M, FLOAT_DMEM in, FLOAT_DMEM *bM)
{
  int i;
  FLOAT_DMEM f0,f1,b0,b1;
  /* initialisation */
  b0 = f0 = in;
  for (i=0; i<M; i++) {
    f1 = f0 + k[i] * b[i];
    b1 = k[i] * f0 + b[i];
    b[i] = b0; // store b[n-1]
    // save old coefficients for next iteration:
    f0 = f1;
    b0 = b1;
  }
  /* return resulting b (optional) */
  if (bM != NULL) *bM = b1;
  /* return resulting f */
  return f1;
}

/* lattice filter for an array *in of N floats, NOTE: *bM is an array of size N here! */
void smileDsp_latticeArray(FLOAT_DMEM *k, FLOAT_DMEM *b, int M, FLOAT_DMEM *in, FLOAT_DMEM *out, FLOAT_DMEM *bM, int N)
{
  int i,n;
  FLOAT_DMEM f0,f1,b0,b1;

  for (n=0; n<N; n++) {

    /* initialisation */
    b0 = f0 = in[n];
    for (i=0; i<M; i++) {
      f1 = f0 + k[i] * b[i]; // recursive computation of filter states
      b1 = k[i] * f0 + b[i]; // recursive computation of filter states
      b[i] = b0; // store b[n-1] for next sample
      f0 = f1; // save old coefficients for next iteration
      b0 = b1; // save old coefficients for next iteration
    }
    /* return resulting b (optional) */
    if (bM != NULL) bM[n] = b1;
    /* return resulting f */
    out[n] = f1;

  }
}

/* Implementation of an inverse lattice filter 
   This function processed a single value per call
 k: coefficients
 *b : temporary work area, initialise with 0 at the beginning! (size: sizeof(FLOAT_DMEM)*M ) 
 M: order (number of coefficients)
 out: e(n) input sample
 return value: f(M) = filter "input" s(n)
 */
FLOAT_DMEM smileDsp_invLattice(FLOAT_DMEM *k, FLOAT_DMEM *b, int M, FLOAT_DMEM out)
{
  int i;
  FLOAT_DMEM fM;
  FLOAT_DMEM last = b[M-1];
  /* initialisation */
  fM = out;
  for (i=M-1; i>0; i--) {
    fM -= k[i] * b[i-1];
    b[i] = k[i] * fM + b[i-1];
  }
  b[M-1] = last;
  fM = fM - k[0] * b[M-1];
  b[0] = k[0]*fM + b[M-1];
  b[M-1] = fM;
  /* return resulting f0 */
  return fM;
}


/* computes formant frequencies and bandwidths from lpc polynomial roots 
    return value: number of valid formants detected from given lpc polynomial roots
 */
int smileDsp_lpcrootsToFormants(double *r, int nR, double *fc, double *bc, int nF, double samplePeriod, double fLow, double fHigh)
{
  double f;
  int i, n = nF;
  int nFormants = 0;
  double spPi = samplePeriod * M_PI;  // (1/sp*2*pi)
  double spPi2 = spPi * 2.0;

  if ((fHigh < fLow)||(fHigh > 1.0/samplePeriod)) fHigh = 0.5 / samplePeriod - fLow;
  
  for (i = 0; i < nR; i++) { 
    int iRe = i*2;
    int iIm = iRe+1;

    if (r[iIm] < 0) continue; // no formant here...
    
    // compute frequency (phase of the complex number)
    f = fabs (atan2 (r[iIm], r[iRe])) / (spPi2);
    
    if ((f >= fLow) && (f <= fHigh)) { // if in range... compute bandwidth
      if (bc != NULL) {
        bc[nFormants] = -log (smileMath_complexAbs(r[iRe],r[iIm])) / spPi;
      }
      fc[nFormants] = f;
      nFormants++;
      if (nFormants >= nF) break;
    }
  }
  /*if (nFormants < (nR)/2) {
    for (i = 0; i< nFormants; i++) {
      fc[i] /= 2.0; //????
    }
  }*/
  for (i = nFormants; i < nF; i++) {
    fc[i] = 0.0; 
    if (bc != NULL) bc[i] = 0.0;
  }

  return nFormants;
}

/*******************************************************************************************
 ***********************=====   Statistics functions   ===== *******************************
 *******************************************************************************************/

/*
Note: the entropy functions compute entropy from a PMF, thus a sequence of values must be converted to a PMF beforehand!
For spectral entropy the normalised spectrum is assumed to be a PMF, thus it is not converted...
*/

/* compute entropy of normalised values (the values will be normalised to represent probabilities by this function) */
FLOAT_DMEM smileStat_entropy(FLOAT_DMEM *_vals, long N)
{
  double e = 0.0;
  int i;
  double dn = 0.0;
  FLOAT_DMEM min=0.0;
  double l2 = (double)log(2.0);

  // get sum of values and minimum value
  for (i=0; i<N; i++) {
    dn += (double)_vals[i];
    if (_vals[i] < min) min = _vals[i];
  }
  
  // floor values < 0
  if (min < 0.0) {
    for (i=0; i<N; i++) {
      _vals[i] -= min;
      if (_vals[i] == 0.0) { _vals[i] = (FLOAT_DMEM)0.00001; dn += (double)0.00001; }
      dn -= (double)min;
    }
  }
  if (dn<(FLOAT_DMEM)0.000001) dn = (FLOAT_DMEM)0.000001;

  // normalise sample values and compute entropy
  for (i=0; i<N; i++) {
    double ln = (double)_vals[i] / dn;
    if (ln > 0.0) 
      e += ln * (double)log(ln) / l2;
  }

  return (FLOAT_DMEM)(-e);
}

/* compute entropy of normalised values (the values will be normalised to represent probabilities by this function) */
// ??? difference to normal entropy ???
FLOAT_DMEM smileStat_relativeEntropy(FLOAT_DMEM *_vals, long N)
{
  double e = 0.0;
  int i;
  double dn = 0.0;
  FLOAT_DMEM min=0.0;
  double lnorm = (double)log((double)N);

  // get sum of values and minimum value
  for (i=0; i<N; i++) {
    dn += (double)_vals[i];
    if (_vals[i] < min) min = _vals[i];
  }
  
  // floor values < 0
  if (min < 0.0) {
    for (i=0; i<N; i++) {
      _vals[i] -= min;
      if (_vals[i] == 0.0) { _vals[i] = (FLOAT_DMEM)0.00001; dn += (double)0.00001; }
      dn -= (double)min;
    }
  }
  if (dn<(FLOAT_DMEM)0.000001) dn = (FLOAT_DMEM)0.000001;

  // normalise sample values and compute entropy
  for (i=0; i<N; i++) {
    double ln = (double)_vals[i] / dn;
    if (ln > 0.0) 
      e += ln * (double)log(ln) / lnorm;
  }

  return (FLOAT_DMEM)(-e);
}


/* TODO:
compute a PMF from a sample sequence using a histogram sampling method

if a valid pointer is given in h->bins, then the histogram will be added to the existing data,
the resulting histogram will then be unnormalised, you will have to call smileMath_vectorNormEuc in the end..
--TODO: this is not the correct way of handling things....! We must normalise both histograms, add and divide by 2
--actually the input histogram is normalised.. so we only divide by 2

if h->bins is NULL the histogram will be initialised with zeros and the output will be normalised.

the memory pointed to by *h must be initialised with 0s! (at least h->bins must be NULL...)

set range to 0.0 to automatically have it determined from the data (max-min)
*/
void smileStat_getPMF(FLOAT_DMEM *_vals, long N, sHistogram *h)
{
  long i; long x; int renorm=0;
  //double sum=(double)_vals[0];
  FLOAT_DMEM min=_vals[0];
  FLOAT_DMEM max=_vals[0];
  FLOAT_DMEM p = (FLOAT_DMEM)(1.0/(double)N);

  if (h == NULL) return;

  if (h->Nbins == 0) h->Nbins = 1000;
  if (h->bins == NULL) {
    h->bins = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*h->Nbins);
    if ((h->min == 0.0) && (h->max == 0.0)) {
      // get sum of values and min/max value
      for (i=1; i<N; i++) {
        //sum += (double)_vals[i];
        if (_vals[i] < min) min = _vals[i];
        if (_vals[i] > max) max = _vals[i];
      }
      h->max = max;
      h->min = min;
    }
    h->stepsize = (h->max - h->min) / (FLOAT_DMEM)(h->Nbins);
    //printf("stepsize = %f  min = %f  max=%f (_vals0=%f)\n",h->stepsize,h->min, h->max,_vals[0]);
  } else {
    renorm = 1;
  }
  
  if (renorm && (h->weight > 1.0)) {
    for (i=0; i<h->Nbins; i++) {
      h->bins[i] *= h->weight;
    }
    p = 1.0;
  }

  for (i=0; i<N; i++) {
    x = (long)floor((_vals[i] - h->min)/h->stepsize);
    if (x<0) x = 0;  /* range clipping*/
    else if (x>=h->Nbins) x=h->Nbins-1; /* range clipping*/
    h->bins[x] += p;
  }

  if (renorm) {
    if (h->weight > 1.0) {
      h->weight += (FLOAT_DMEM)N;
      for (i=0; i<h->Nbins; i++) {
        h->bins[i] /= h->weight;
      }
    } else {
      for (i=0; i<h->Nbins; i++) {
        h->bins[i] /= 2.0;
      }
    }
  } else {
    h->weight = (FLOAT_DMEM)N;
  }
}

/* Estimate the probability of a value x belonging to a given pmf h (the pmf must be normalised to sum 1!) */
FLOAT_DMEM smileStat_probEstim(FLOAT_DMEM x, sHistogram *h, FLOAT_DMEM probFloor)
{
  long i = (long)floor((x - h->min)/h->stepsize);
  FLOAT_DMEM ret;
  if (h==NULL) return 0.0;
  if (i>=h->Nbins) return 0.0;
  if (i<0) return 0.0;
  // TODO: interpolate actual value more accurately by considering neighbour probabilities
  ret = MAX(h->bins[i],probFloor);
  if (ret > 1.0) {
#ifdef DEBUG
    fprintf("WARNING: Clipped probability to 1.0 in smileStat_probEstim (%f) (probFloor = %f)\n",ret,probFloor);
#endif
    ret = 1.0; // perform clipping
  }
  return ret;
}

/* get a PMF vector from a data matrix (result is a pmf histogram for each row of the matrix) */
/* *h must point to an allocated array of R x sHistogram */
/* N: number of colums in matrix  , R: number of rows in the matrix (must match the size of *h) */
/* matrix in _vals is read rowwise.. cMatrix in openSMILE is read columnwise, so you must transpose here!*/
void smileStat_getPMFvec(FLOAT_DMEM *_vals, long N, long R, sHistogram *h)
{
  long i;
  if (h== NULL) return;
  if (_vals == NULL) return;
  for (i=0; i<R; i++) {
    smileStat_getPMF(_vals+(N*i), N, h+i);
  }
}

/* get a PMF vector from a data matrix (result is a pmf histogram for each row of the matrix) */
/* *h must point to an allocated array of R x sHistogram */
/* N: number of colums in matrix  , R: number of rows in the matrix (must match the size of *h) */
/* matrix in _vals is read columnwise.. use this for compatiblity with openSMILE cMatrix!*/
void smileStat_getPMFvecT(FLOAT_DMEM *_vals, long N, long R, sHistogram *h)
{
  long i,n;
  FLOAT_DMEM *_row;
  if (h== NULL) return;
  if (_vals == NULL) return;
  _row = (FLOAT_DMEM *)malloc(sizeof(FLOAT_DMEM)*N);
  for (i=0; i<R; i++) {
    for (n=0; n<N; n++) {
      _row[n] = _vals[n*R+i];
    }
    smileStat_getPMF(_row, N, h+i);
  }
}

/* estimate probability of a vector belonging to a pmf array */
void smileStat_probEstimVec(FLOAT_DMEM *x, sHistogram *h, FLOAT_DMEM **p, long R, FLOAT_DMEM probFloor)
{
  long i;
  if (x==NULL) return;
  if (h==NULL) return;
  if (p==NULL) return;
  
  if (*p==NULL) 
     *p = (FLOAT_DMEM*)calloc(1,sizeof(FLOAT_DMEM)*R);

  for (i=0; i<R; i++) {
    (*p)[i] = smileStat_probEstim(x[i], h+i, probFloor);
  }
}

/* estimate probability of a vector belonging to a pmf array */
FLOAT_DMEM smileStat_probEstimVecLin(FLOAT_DMEM *x, sHistogram *h, long R, FLOAT_DMEM probFloor)
{
  FLOAT_DMEM *p = NULL;
  FLOAT_DMEM P = 1.0;
  long i;
  smileStat_probEstimVec(x,h,&p,R,probFloor);
  for (i=0; i<R; i++) {
    P *= p[i];
  }
  //P/=(FLOAT_DMEM)R;
  if (p!=NULL) free(p);
  return MAX(P,probFloor);
}

/* estimate probability of a vector belonging to a pmf array */
FLOAT_DMEM smileStat_probEstimVecLog(FLOAT_DMEM *x, sHistogram *h, long R, FLOAT_DMEM probFloorLog)
{
  FLOAT_DMEM *p = NULL;
  FLOAT_DMEM P = 1.0;
  long i;
  smileStat_probEstimVec(x,h,&p,R,0.0);
  for (i=0; i<R; i++) {
    if (p[i] < exp(probFloorLog)) P += probFloorLog;
    else P += (FLOAT_DMEM)log(p[i]);
  }
  if (p!=NULL) free(p);
  return MAX(P,probFloorLog);
}

/*******************************************************************************************
 ***********************=====   WAVE file I/O   ===== **************************************
 *******************************************************************************************/


/* WAVE Header struct, valid only for PCM Files */
typedef struct {
  uint32_t	Riff;    /* Must be little endian 0x46464952 (RIFF) */
  uint32_t	FileSize;
  uint32_t	Format;  /* Must be little endian 0x45564157 (WAVE) */

  uint32_t	Subchunk1ID;  /* Must be little endian 0x20746D66 (fmt ) */
  uint32_t	Subchunk1Size;
  uint16_t	AudioFormat;
  uint16_t	NumChannels;
  uint32_t	SampleRate;
  uint32_t	ByteRate;
  uint16_t	BlockAlign;
  uint16_t	BitsPerSample;

  uint32_t	Subchunk2ID;  /* Must be little endian 0x61746164 (data) */
  uint32_t  Subchunk2Size;
} sRiffPcmWaveHeader;

typedef struct {
  uint32_t SubchunkID;
  uint32_t SubchunkSize;
} sRiffChunkHeader;


/* parse wave header from in-memory wave file pointed to by *raw */
int smilePcm_parseWaveHeader(void *raw, long long N, sWaveParameters *pcmParam)
{
  if ((raw != NULL)&&(pcmParam!=NULL)) {
    long long nRead;
    sRiffPcmWaveHeader *head;
    sRiffChunkHeader *chunkhead;
    int safetytimeout = 20;  // max <safetytimeout> chunks of 99kb size before 'data' chunk
    long long ptr = 0;

    head = (sRiffPcmWaveHeader *)raw;
    nRead = sizeof(sRiffPcmWaveHeader);
    if (N<nRead) nRead=N;
    raw = (char*)raw + nRead;
    ptr += nRead;
    if (nRead != sizeof(sRiffPcmWaveHeader)) {
      printf("smilePcm: Error reading %i bytes (header) from beginning of wave data! The given array is too short (N=%i)!",sizeof(sRiffPcmWaveHeader),(long)N);
      return 0;
    }

    /* Check for valid header , TODO: support other endianness */
	if ((head->Riff != 0x46464952) ||
		(head->Format != 0x45564157) ||
		(head->Subchunk1ID != 0x20746D66) ||
//		(head->Subchunk2ID != 0x61746164) ||
		(head->AudioFormat != 1) ||
		(head->Subchunk1Size != 16)) {
                            fprintf(stderr,"smilePcm:  Riff: %x\n  Format: %x\n  Subchunk1ID: %x\n  Subchunk2ID: %x\n  AudioFormat: %x\n  Subchunk1Size: %x",
                                        head->Riff, head->Format, head->Subchunk1ID, head->Subchunk2ID, head->AudioFormat, head->Subchunk1Size);
                            fprintf(stderr,"smilePcm: bogus wave/riff header or data/file in wrong format!");
                            return 0;
        }

    while ((head->Subchunk2ID != 0x61746164)&&(safetytimeout>0)) { // 0x61746164 = 'data'
      // keep searching for 'data' chunk:
      if (head->Subchunk2Size < 99999) {
        //char * tmp = (char*)malloc(head->Subchunk2Size);
        //nRead = (int)fread(tmp, 1, head->Subchunk2Size, filehandle);
        //nRead = head->Subchunk2Size + ...;
        char * tmp = (char *)raw; 
        raw = (char * )raw + head->Subchunk2Size;
        ptr += head->Subchunk2Size; 
        if (ptr > N) {
          fprintf(stderr,"smilePcm: less bytes read from wave data than indicated by Subchunk2Size (%i)! File seems broken!\n",head->Subchunk2Size);
          return 0;
        }
        free(tmp);
      } else {
        fprintf(stderr,"smilePcm: Subchunk2Size > 99999. This seems to be a bogus file!\n");
        return 0;
      }
      chunkhead = (sRiffChunkHeader *)raw;
      raw = (char*)raw + sizeof(sRiffChunkHeader);
      ptr += sizeof(sRiffChunkHeader);
      //nRead = (int)fread(&chunkhead, 1, sizeof(chunkhead), filehandle);
      if (ptr > N) {
        fprintf(stderr,"smilePcm: less bytes read from wave data than there should be (%i) while reading sub-chunk header! File seems broken!\n",sizeof(sRiffChunkHeader));
        return 0;
      }
      head->Subchunk2ID = chunkhead->SubchunkID;
      head->Subchunk2Size = chunkhead->SubchunkSize;
      safetytimeout--;
    }
    if (safetytimeout <= 0) {
      fprintf(stderr,"smilePcm: No 'data' subchunk found in wave-file among the first %i chunks! corrupt file?\n",safetytimeout);
      return 0;
    }
    
    pcmParam->sampleRate = head->SampleRate;
    pcmParam->nChan = head->NumChannels;
    pcmParam->nBPS = head->BlockAlign/head->NumChannels;
    pcmParam->nBits = head->BitsPerSample;
    pcmParam->nBlocks = head->Subchunk2Size / head->BlockAlign;
    pcmParam->blockSize = head->BlockAlign;

    // TODO: ???
    pcmParam->byteOrder = BYTEORDER_LE;
    pcmParam->memOrga = MEMORGA_INTERLV;
     
    return 1;

  }
  return 0;
}

// filename is optional and can be NULL ! It is used only for log messages.
int smilePcm_readWaveHeader(FILE *filehandle, sWaveParameters *pcmParam, const char *filename)
{
  if (filename == NULL) filename = "unknown";

  if ((filehandle != NULL)&&(pcmParam!=NULL)) {

    int nRead;
    sRiffPcmWaveHeader head;
    sRiffChunkHeader chunkhead;
    int safetytimeout = 20;  // max <safetytimeout> chunks of 99kb size before 'data' chunk

    fseek(filehandle, 0, SEEK_SET);
    nRead = (int)fread(&head, 1, sizeof(head), filehandle);
    if (nRead != sizeof(head)) {
      printf("smilePcm: Error reading %i bytes (header) from beginning of wave file '%s'! File too short??",sizeof(head),filename);
      return 0;
    }
    
    /* Check for valid header , TODO: support other endianness */
	if ((head.Riff != 0x46464952) ||
		(head.Format != 0x45564157) ||
    (head.Subchunk1ID != 0x20746D66) ||
//		(head.Subchunk2ID != 0x61746164) ||
    (head.AudioFormat != 1) ||   // 32-bit: 0xfffe
		(head.Subchunk1Size != 16)) {  // 32-bit: 28
                            fprintf(stderr,"smilePcm:  Riff: %x\n  Format: %x\n  Subchunk1ID: %x\n  Subchunk2ID: %x\n  AudioFormat: %x\n  Subchunk1Size: %x",
                                        head.Riff, head.Format, head.Subchunk1ID, head.Subchunk2ID, head.AudioFormat, head.Subchunk1Size);
                            fprintf(stderr,"smilePcm: bogus wave/riff header or file in wrong format ('%s')! (maybe you are trying to read a 32-bit wave file which is not yet supported (new header type...)?)",filename);
                            return 0;
        }

    while ((head.Subchunk2ID != 0x61746164)&&(safetytimeout>0)) { // 0x61746164 = 'data'
      // keep searching for 'data' chunk:
      if (head.Subchunk2Size < 99999) {
        char * tmp = (char*)malloc(head.Subchunk2Size);
        nRead = (int)fread(tmp, 1, head.Subchunk2Size, filehandle);
        if (nRead != head.Subchunk2Size) {
          fprintf(stderr,"smilePcm: less bytes read (%i) from wave file '%s' than indicated by Subchunk2Size (%i)! File seems broken!\n",nRead,filename,head.Subchunk2Size);
          return 0;
        }
        free(tmp);
      } else {
        fprintf(stderr,"smilePcm: Subchunk2Size > 99999. This seems to be a bogus file!\n");
        return 0;
      }
      nRead = (int)fread(&chunkhead, 1, sizeof(chunkhead), filehandle);
      if (nRead != sizeof(chunkhead)) {
        fprintf(stderr,"smilePcm: less bytes read (%i) from wave file '%s' than there should be (%i) while reading sub-chunk header! File seems broken!\n",nRead,filename,sizeof(chunkhead));
        return 0;
      }
      head.Subchunk2ID = chunkhead.SubchunkID;
      head.Subchunk2Size = chunkhead.SubchunkSize;
      safetytimeout--;
    }
    if (safetytimeout <= 0) {
      fprintf(stderr,"smilePcm: No 'data' subchunk found in wave-file among the first %i chunks! corrupt file?\n",safetytimeout);
      return 0;
    }
    
    pcmParam->sampleRate = head.SampleRate;
    pcmParam->nChan = head.NumChannels;
    pcmParam->nBPS = head.BlockAlign/head.NumChannels;
    pcmParam->nBits = head.BitsPerSample;
//    p->bits = head.BitsPerSample;
//    SMILE_DBG(5,"bits per sample = %i",head.BitsPerSample);
//    pcmParam.sampleType = pcmBitsToSampleType( head.BitsPerSample, BYTEORDER_LE, 0 );
/*
    if (head.NumChannels * head.BitsPerSample / 8 != head.BlockAlign) {
      FEATUM_ERR_FATAL(0,"Error reading wave file header: head.BlockAlign != head.NumChannels * head.BitsPerSample / 8");
      return 0;
    }
    */
    pcmParam->nBlocks = head.Subchunk2Size / head.BlockAlign;
    pcmParam->blockSize = head.BlockAlign;

    // TODO: ???
    pcmParam->byteOrder = BYTEORDER_LE;
    pcmParam->memOrga = MEMORGA_INTERLV;

    //pcmDataBegin = ftell(filehandle);

    return 1;
  }
  return 0;
}



//return : -1 eof, 0 error, > 0 , num samples read
int smilePcm_readSamples(FILE **filehandle, sWaveParameters *pcmParam, float *a, int nChan, int nSamples, int monoMixdown)
{
  #define SETF(c,s,val) a[nChan*s+c]=val;

// reads data into matix m, size is determined by m, also performs format conversion to float samples and matrix format
//int cWaveSource::readData(cMatrix *m)
//{
  int bs, nRead;
  uint8_t *buf;

  if (filehandle==NULL) return -1;
  if (*filehandle==NULL) return 0;
  if (a==NULL) return 0;
  if (pcmParam==NULL) return 0;
  if (feof(*filehandle)) {
    //fprintf(stderr,"smilePcm: not reading from file, already EOF");
    return -1;
  }
  
  bs = pcmParam->blockSize * nSamples;
  buf = (uint8_t *)malloc(bs);
  
  if (buf==NULL) return 0;
  nRead = (int)fread(buf, 1, bs, *filehandle);
  if (nRead != bs) {
    nSamples = nRead / pcmParam->blockSize; // int div rounds down..!? it should better do so...
    fclose(*filehandle); *filehandle = NULL;
  }

  if (nRead > 0) {
 
    int i,c;
    int8_t *b8=(int8_t*)buf;
    uint8_t *bu8=(uint8_t*)buf;
    int16_t *b16 = (int16_t*)buf;
    int32_t *b32 = (int32_t*)buf;

    if (monoMixdown) {

      switch(pcmParam->nBPS) {
        case 1: // 8-bit int
          for (i=0; i<nSamples; i++) {
            float tmp = 0.0;
            for (c=0; c<pcmParam->nChan; c++) {
              tmp += (float)b8[i*pcmParam->nChan+c];
            }
            SETF(0,i,(tmp/(float)pcmParam->nChan)/(float)127.0);
          }
          break;
        case 2: // 16-bit int
          for (i=0; i<nSamples; i++) {
            float tmp = 0.0;
            for (c=0; c<pcmParam->nChan; c++) {
              tmp += (float)b16[i*pcmParam->nChan+c];
            }
            SETF(0,i,(tmp/(float)pcmParam->nChan)/(float)32767.0);
          }
          break;
        case 3: // 24-bit int
          
          for (i=0; i<nSamples; i++) {
              float tmp = 0.0;
              for (c=0; c<pcmParam->nChan; c++) {
                // the only known file with 3bytes was exported by matlab
                // a byte order conversion was necessary here.. is that always the case?? FIXME!
                uint32_t is=0;
                int32_t * iis;
                is |= (uint32_t)(bu8[(i*pcmParam->nChan+c)*3])<<8;
                is |= (uint32_t)(bu8[(i*pcmParam->nChan+c)*3+1])<<16;
                is |= (uint32_t)(bu8[(i*pcmParam->nChan+c)*3+2])<<24;
                iis = (int32_t*)&is;
                tmp += (float)(*iis >> 8);
              }
              SETF(0,i,(tmp/(float)pcmParam->nChan)/(float)(32767.0*256.0));
            }
        
          break;
        case 4: // 32-bit int or 24-bit packed int
          if (pcmParam->nBits == 24) {
            for (i=0; i<nSamples; i++) {
              float tmp = 0.0;
              for (c=0; c<pcmParam->nChan; c++) {
                tmp += (float)(b32[i*pcmParam->nChan+c]&0xFFFFFF);
              }
              SETF(0,i,(tmp/(float)pcmParam->nChan)/(float)(32767.0*256.0));
            }
            break;
          } else if (pcmParam->nBits == 32) {
            for (i=0; i<nSamples; i++) {
              float tmp = 0.0;
              for (c=0; c<pcmParam->nChan; c++) {
                tmp += (float)(b32[i*pcmParam->nChan+c]);
              }
              SETF(0,i,(tmp/(float)pcmParam->nChan)/(float)(32767.0*32767.0*2.0));
            }
            break;
        }
        default:
          fprintf(stderr,"smilePcm: readData: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",pcmParam->nBPS,pcmParam->nBits);
          nRead=0;
      }
    
    } else { // no mixdown, multi-channel matrix output

      switch(pcmParam->nBPS) {
        case 1: // 8-bit int
          for (i=0; i<nSamples; i++) for (c=0; c<pcmParam->nChan; c++) {
            SETF(c,i,((float)b8[i*pcmParam->nChan+c])/(float)127.0);
          } break;
        case 2: // 16-bit int
          for (i=0; i<nSamples; i++) for (c=0; c<pcmParam->nChan; c++) {
            SETF(c,i,((float)b16[i*pcmParam->nChan+c])/(float)32767.0);
          } break;
        case 3: // 24-bit int
          for (i=0; i<nSamples; i++) {
              float tmp = 0.0;
              for (c=0; c<pcmParam->nChan; c++) {
                // the only known file with 3bytes was exported by matlab
                // a byte order conversion was necessary here.. is that always the case?? FIXME!
                uint32_t is=0;
                int32_t * iis;
                is |= (uint32_t)(bu8[(i*pcmParam->nChan+c)*3])<<8;
                is |= (uint32_t)(bu8[(i*pcmParam->nChan+c)*3+1])<<16;
                is |= (uint32_t)(bu8[(i*pcmParam->nChan+c)*3+2])<<24;
                iis = (int32_t*)&is;
                tmp = (float)(*iis >> 8);
                SETF(c,i,(tmp)/(float)(32767.0*256.0));
              }
            }
          break;
        case 4: // 32-bit int or 24-bit packed int
          if (pcmParam->nBits == 24) {
            for (i=0; i<nSamples; i++) for (c=0; c<pcmParam->nChan; c++) {
              SETF(c,i,((float)(b32[i*pcmParam->nChan+c]&0xFFFFFF))/(float)(32767.0*32767.0*2.0));
            } break;
          } else if (pcmParam->nBits == 32) {
            for (i=0; i<nSamples; i++) for (c=0; c<pcmParam->nChan; c++) {
              SETF(c,i,((float)b32[i*pcmParam->nChan+c])/(float)(32767.0*32767.0*2.0));
            } break;
          }

        default:
          fprintf(stderr,"smilePcm: readData: cannot convert unknown sample format to float! (nBPS=%i, nBits=%i)",pcmParam->nBPS,pcmParam->nBits);
          nRead=0;
      }
    }
  }
  free(buf);
  return nSamples; //(nRead>0);
}

/*******************************************************************************************
 *******************=====   Vector save/load debug helpers   ===== *************************
 *******************************************************************************************/

/* these functions are not safe and should only be used for data output during debugging ! */

#include <stdio.h>

void saveDoubleVector_csv(const char * filename, double * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"a");
  else
    f = fopen(filename,"w");

  if (f!=NULL) {
    long i;
    for (i=0; i<N-1; i++) 
      fprintf(f, "%f,", vec[i]);
    fprintf(f, "%f\n", vec[i]);
    fclose(f);
  }
}

void saveFloatVector_csv(const char * filename, float * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"ab");
  else
    f = fopen(filename,"wb");

  if (f!=NULL) {
    long i;
    for (i=0; i<N-1; i++) 
      fprintf(f, "%f,", vec[i]);
    fprintf(f, "%f\n", vec[i]);
    fclose(f);
  }
}

void saveFloatDmemVector_csv(const char * filename, FLOAT_DMEM * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"ab");
  else
    f = fopen(filename,"wb");

  if (f!=NULL) {
    long i;
    for (i=0; i<N-1; i++) 
      fprintf(f, "%f,", vec[i]);
    fprintf(f, "%f\n", vec[i]);
    fclose(f);
  }
}


void saveDoubleVector_bin(const char * filename, double * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"ab");
  else
    f = fopen(filename,"wb");

  if (f!=NULL) {
    fwrite(vec, sizeof(double)*N, 1, f);
    fclose(f);
  }
}

void saveFloatVector_bin(const char * filename, float * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"ab");
  else
    f = fopen(filename,"wb");

  if (f!=NULL) {
    fwrite(vec, sizeof(float)*N, 1, f);
    fclose(f);
  }
}

void saveFloatDmemVector_bin(const char * filename, FLOAT_DMEM * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"ab");
  else
    f = fopen(filename,"wb");

  if (f!=NULL) {
    fwrite(vec, sizeof(FLOAT_DMEM)*N, 1, f);
    fclose(f);
  }
}

void saveFloatDmemVectorWlen_bin(const char * filename, FLOAT_DMEM * vec, long N, int append)
{
  FILE * f = NULL;
  if (append)
    f = fopen(filename,"ab");
  else
    f = fopen(filename,"wb");

  if (f!=NULL) {
    FLOAT_DMEM Nf = (FLOAT_DMEM)N;
    fwrite(&Nf, sizeof(FLOAT_DMEM), 1, f);
    fwrite(vec, sizeof(FLOAT_DMEM)*N, 1, f);
    fclose(f);
  }
}

//** HTK 

static int smileHtk_vax = 0;

__inline void smileHtk_Swap32 ( uint32_t *p )
{
  uint8_t temp,*q;
  q = (uint8_t*) p;
  temp = *q; *q = *( q + 3 ); *( q + 3 ) = temp;
  temp = *( q + 1 ); *( q + 1 ) = *( q + 2 ); *( q + 2 ) = temp;
}

__inline void smileHtk_Swap16 ( uint16_t *p ) 
{
  uint8_t temp,*q;
  q = (uint8_t*) p;
  temp = *q; *q = *( q + 1 ); *( q + 1 ) = temp;
}

void smileHtk_SwapFloat( float *p )
{
  uint8_t temp,*q;
  q = (uint8_t*) p;
  temp = *q; *q = *( q + 3 ); *( q + 3 ) = temp;
  temp = *( q + 1 ); *( q + 1 ) = *( q + 2 ); *( q + 2 ) = temp;
}

void smileHtk_prepareHeader( sHTKheader *h )
{
  if ( smileHtk_vax ) {
    smileHtk_Swap32 ( &(h->nSamples) );
    smileHtk_Swap32 ( &(h->samplePeriod) );
    smileHtk_Swap16 ( &(h->sampleSize) );
    smileHtk_Swap16 ( &(h->parmKind) );
  }
}

int smileHtk_readHeader(FILE *filehandle, sHTKheader *head)
{
  if (filehandle==NULL) return 0;
  if (!fread(head, sizeof(sHTKheader), 1, filehandle)) {
    SMILE_IERR(1,"error reading HTK header from file.");
    return 0;
  }
  smileHtk_prepareHeader(head); // convert to host byte order
  return 1;
}

#include <string.h>

int smileHtk_writeHeader(FILE *filehandle, sHTKheader *_head)
{
  sHTKheader head;  // local copy, due to prepareHeader! we don't want to change 'header' variable!
  if (filehandle==NULL) return 0;

  // adjust endianness
  memcpy(&head, _head, sizeof(sHTKheader));
  smileHtk_prepareHeader(&head);

  // seek to beginning of file:
  fseek(filehandle, 0 , SEEK_SET);
  
  // write header:
  if (!fwrite(&head, sizeof(sHTKheader), 1, filehandle)) {
    SMILE_IERR(1,"Error writing to htk feature file!");
    return 0;
  }

  return 1;
}

int smileHtk_IsVAXOrder ()
{
  short x;
  unsigned char *pc;
  pc = (unsigned char *) &x;
  *pc = 1; *( pc + 1 ) = 0;			// store bytes 1 0
  smileHtk_vax = (x==1);	  		// does it read back as 1?
  return smileHtk_vax; 
}
