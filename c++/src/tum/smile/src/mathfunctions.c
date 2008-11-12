/***************************************************************
  Rhythm AS v0.1  (c) by Florian Eyben
  ------------------------------------
  ~~~~ mathfunctions.c / mathfunctions.h ~~~~~~~
  
  various math & vector functions
 ***************************************************************/

#include "mathfunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

/*
 abs function for data-type <int>
 */
inline int intabs(int n)
{
  if (n<0) return -n;
  return n;    
}

int fit_in_modrange(int val, int range)
{
  int res;
  
  if (range == 0) return 0;
  res = val%range;
  if (res < range/2) return (res+range);
  else return res;
}

/* sin(x) / x  function */
double sinc(double x)
{
  if (x==0) return 1.0;
  else return sin(x)/x;       
}

/*
 sum of squares of vector elements
 n is the number of elements in the vector (array) v1
 */
double vector_squaresum(double *v1, int n)
{
  int i;
  double sum = 0.0;
  
  for (i=0; i<n; i++) {
    sum += v1[i]*v1[i];
  }
  return sum;
}

/*
 sum of vector elements
 n is the number of elements in the vector (array) v1
 */
double vector_sum(double *v1, int n)
{
  int i;
  double sum = 0.0;
  for (i=0; i<n; i++) {
      sum += v1[i];
  }
  return sum;
}

/*
 mean square error between two different vectors v1 and v2 of equal dimension
 n is the number of elements in both vectors
 */
double vectors_mse(double *v1, double *v2, int n)
{
  int i;
  double sum = 0.0;
  
  for (i=0; i<n; i++) {
      sum += (v1[i]-v2[i])*(v1[i]-v2[i]);
  }
  sum /= (double)n;
  return sum;
}

/*double vectors_mse_rising(double *v1, double *v2, int n)
{
  int i;
  double sum = 0.0;
  
  for (i=0; i<n; i++) {
    if (v1[i] - v2[i] > 0.0) 
      sum += (v1[i]-v2[i])*(v1[i]-v2[i]);
  }
  sum /= (double)n;
  return sum;
}*/

/*
 simple, unnormalized cross-correlation
 */
double vectors_crossCorr(double *v1, double *v2, int n)
{
  int i;
  double sum = 0.0;
  
  for (i=0; i<n; i++) {
    sum += v1[i]*v2[i];
  }
  sum /= (double)n;
  return sum;
}

/*
 ??
 */
void vector_make_unitVector_meanZero(double *arr, int n)
{
  int i;
  double sum = 0.0;
  double mean = 0.0;
  
  if (arr == NULL) return;
  
  for (i=0; i<n; i++) {
    sum += arr[i]*arr[i];
    mean += arr[i];
  }
  sum=sqrt(sum);
  mean /= sum*(double)n;
  for (i=0; i<n; i++) {
    arr[i] /= sum;
    arr[i] -= mean;
  }
}

/*
 Normalize vector to mean 0
 then make it a unit vector
 */
void vector_make_zeroMean_unitVector(double *arr, int n)
{
  int i;
  double sum = 0.0;
  double mean = 0.0;
  
  if (arr == NULL) return;
  
  for (i=0; i<n; i++) {
    mean += arr[i];
  }
  mean /= (double)n;

  for (i=0; i<n; i++) {
    sum += (arr[i]-mean)*(arr[i]-mean);
  }
  sum=sqrt(sum);

  for (i=0; i<n; i++) {
    arr[i] /= sum;
  }
}

/*
 Normalize to a vector of euclidean length 1
 */
void vector_make_unitVector(double *arr, int n)
{
  int i;
  double sum = 0.0;
  
  if (arr == NULL) return;
  
  for (i=0; i<n; i++) {
    sum += arr[i]*arr[i];
  }
  sum=sqrt(sum);
  for (i=0; i<n; i++) {
    arr[i] /= sum;
  }
}

/* Get global max. */
double vector_max(double *arr, int n)
{
  double state;
  int i;
  
  if (arr == NULL) return -0.1;
  
  state = arr[0];
  for (i=1; i<n; i++) {
    if (arr[i] > state) state = arr[i];
  }
  return state;
}

/* 
 return the larger one of the two arguments
 */
inline int i2_argMax(int arg1, int arg2) 
{
   if (arg1 > arg2) return arg1;
   else return arg2;
}

/* 
 return the smaller one of the two arguments
 */
inline int i2_argMin(int arg1, int arg2) 
{
   if (arg1 < arg2) return arg1;
   else return arg2;
}
 
/* 
 Get index of global max and the peak's width 
  (from its left min to its right min) 
 */
int vector_max_index_width(double *arr, int n, int *width)
{
  int iMax = 0, min_l, min_r;
  int i;
  
  if (arr == NULL) return -0.1;
  
  for (i=1; i<n; i++) {
    if (arr[i] > arr[iMax]) iMax = i;
  }
  if (width != NULL) {
    min_l = 0;
    for (i=iMax-1; i>0; i--) {
      if ( (arr[i]<arr[i-1]) && (arr[i]<arr[i+1]) ) {
        min_l = i;
        break;
      }     
    }  
           
    min_r = 0;
    for (i=iMax+1; i<n-1; i++) {
      if ( (arr[i]<arr[i-1]) && (arr[i]<arr[i+1]) ) {
        min_r = i;
        break;
      }     
    }         
    
    *width = 2* i2_argMax(iMax-min_l,min_r-iMax);
  }
  
  return iMax;
}

/* 
 Computes the average of all n elements of vector <arr>
 */
double vector_mean(double *arr, int n)
{
  double sum;
  int i;
  
  if (arr == NULL) return 0.0;
  
  for (i=0; i<n; i++) {
    sum += arr[i];
  }
  sum /= (double)n;
  return sum;
}

/* 
 Get mean of all non-zero elements 
 */
double vector_mean_nz(double *arr, int n)
{
  double sum;
  int i,num=0;
  
  if (arr == NULL) return 0.0;
  
  for (i=0; i<n; i++) {
    if (arr[i] != 0) {
      sum += arr[i];
      num++;           
    }
  }
  sum /= (double)num;
  return sum;
}

//subtract mean and return mean
double vector_mean_normalize(double *arr, int n)
{
  double sum;
  int i;
  
  if (arr == NULL) return 0.0;
  
  for (i=0; i<n; i++) {
    sum += arr[i];
  }
  sum /= (double)n;
  for (i=0; i<n; i++) {
    arr[i] -= sum;
  }
  return sum;
}

//divide by max. value, and return max. value
double vector_max_normalize(double *arr, int n)
{
  double max;
  int i;
  
  if (arr == NULL) return 0.0;
  
  max = vector_max(arr,n);
  if (max == 0.0) return max;
  
  for (i=0; i<n; i++) {
    arr[i] /= max;
  }
  return max;
}

/*
 subtract mean, and clip to positive values
 (i.e. all negative values in the result are set to 0)
 return mean value
 */
double vector_mean_normalize_pos(double *arr, int n)
{
  double sum;
  int i;
  
  if (arr == NULL) return 0.0;
  
  for (i=0; i<n; i++) {
    sum += arr[i];
  }
  sum /= (double)n;
  for (i=0; i<n; i++) {
    arr[i] -= sum;
    if (arr[i] < 0.0) arr[i] = 0.0;
  }
  return sum;
}

/*****
  Symmetric window functions:
  compute specific value over a window of size <range> centered on 
  absolute index <middle_index> of vector <arr>. 
  n is the total(!) length of vector <arr>.
 *****/

double vector_symmWin_mean(double *arr, int n, int middle_index, int range)
{
  int i;
  int start = middle_index - range/2;
  int end = middle_index + range/2;
  double sum = 0.0;
  
  if (start < 0) start = 0;
  if (end > n) end = n;
  
  for (i=start; i<end; i++) {
    sum += arr[i];    
  }
  return sum/(double)(end-start);    
}

double vector_symmWin_max(double *arr, int n, int middle_index, int range)
{
  int i;
  int start = middle_index - range/2;
  int end = middle_index + range/2;

  if (start < 0) start = 0;
  if (end > n) end = n;
  
  double max = arr[start];  
  for (i=start+1; i<end; i++) {
    if (arr[i]>max) max = arr[i];    
  }
  return max;    
}

double vector_symmWin_max_index(double *arr, int n, int middle_index, int range)
{
  int i;
  int start = middle_index - range/2;
  int end = middle_index + range/2;

  if (start < 0) start = 0;
  if (end > n) end = n;
  
  int iMax = start;  
  for (i=start; i<end; i++) {
    if (arr[i]>arr[iMax]) iMax = i;    
  }
  return iMax;    
}

double vector_symmWin_peak_index(double *arr, int n, int middle_index, int range)
{
  int i;
  int start = middle_index - range/2;
  int end = middle_index + range/2;

  if (start < 0) start = 0;
  if (end > n) end = n;
  
  int iMax = middle_index;  
  int iMax_l=start,iMax_r=end;
  
  for (i=middle_index; i>start; i--) {
    if ( (arr[i]>arr[i+1]) && (arr[i]>arr[i-1]) ) {
         iMax_l = i; break;
    }
  }
  for (i=middle_index; i<end-1; i++) {
    if ( (arr[i]>arr[i+1]) && (arr[i]>arr[i-1]) ) {
         iMax_r = i; break;
    }
  }
  if (iMax_r-iMax < iMax-iMax_l) return iMax_r;
  else return iMax_l;
}

double vector_symmWin_sum(double *arr, int n, int middle_index, int range)
{
  int i;
  int start = middle_index - range/2;
  int end = middle_index + range/2;
  double sum = 0.0;
  
  if (start < 0) start = 0;
  if (end > n) end = n;
  
  for (i=start; i<end; i++) {
    sum += arr[i];    
  }
  return sum;    
}

/*
 Global minimum
 */
double vector_min(double *arr, int n)
{
  double state;
  int i;
  
  if (arr == NULL) return -0.1;
  
  state = arr[0];
  for (i=1; i<n; i++) {
    if (arr[i] < state) state = arr[i];
  }
  return state;
}

/*   OBSOLETE !!! ?

void printMax(double *arr, int n)
{
  printf("Max: %f\n",vector_max(arr,n)); 
}

void printMin(double *arr, int n)
{
  printf("Min: %f\n",vector_min(arr,n)); 
}

void printMinMax(double *arr, int n)
{
  printf("Min: %f *** Max: %f\n",vector_min(arr,n),vector_max(arr,n)); 
}
*/


/******************************************************
 functions for comparison with certain tolerance
 ******************************************************/

int approx_match_int(int i1, int i2, int tolerance)
{
  if (i1>i2) {
    if ((i1-i2)<=tolerance) return 1;
  } else {
    if ((i2-i1)<=tolerance) return 1;
  } 
  return 0;      
}

int approx_multmatch_int(int i1, int i2, int tolerance, int maxmult)
{ /* i2 is the one being multiplied */
  int i,i22;
  for (i=1; i<=maxmult; i++) {
    i22=i2*i;  
    if (i1>i22) {
      if ((i1-i22)<=tolerance) return 1;
    } else {
      if ((i22-i1)<=tolerance) return 1;
    } 
  }  
  return 0;      
}

int approx_multmatch_double(double i1, double i2, double tolerance, int minmult, int maxmult)
{ /* i2 is the one being multiplied */
  double i;
  double i22;
  for (i=(double)minmult; i<=(double)maxmult; i += 1.0) {
    i22=i2*i;  
    if (i1>i22) {
      if ((i1-i22)<=tolerance) return 1;
    } else {
      if ((i22-i1)<=tolerance) return 1;
    } 
  }  
  return 0;      
}

int approx_match_double(double i1, double i2, double tolerance)
{
  if (i1>i2) {
    if ((i1-i2)<=tolerance) return 1;
  } else {
    if ((i2-i1)<=tolerance) return 1;
  }       
  return 0;
}


/* symmetric (result range 0 - +b/2)  a mod b */
inline int symmetric_absmod(int a, int b)
{
  int tmp; 
  if (b==0) return a;
  tmp = (a % b);
  if (tmp > b/2) tmp = b - tmp;    
  
  return tmp;
    
}

/* symmetric (result range -b/2 to +b/2)  a mod b */
inline int symmetric_mod(int a, int b)
{
  int tmp; 
  tmp = (a % b);
  if (tmp > b/2) tmp -= b;    
  
  return tmp;
}

int approx_modmultmatch_int(int i1, int i2, int tolerance)
{
   if (i2 == 0) return 0; 
   if (i1 == 0) return 0;
   if (symmetric_absmod(i1,i2) <= tolerance) return 1;
   if (symmetric_absmod(i2,i1) <= tolerance) return 1;
   return 0;
}

/* Get global max. w. index */
int vector_max_index_double(double *arr, int n)
{
  int i,iMax=0;
  
  if (arr == NULL) return 0;
  
  for (i=1; i<n; i++) {
    if (arr[i] > arr[iMax]) iMax = i;
  }
  return iMax;
}

/* Get global min. w. index */
int vector_min_index_double(double *arr, int n)
{
  int i,iMax=0;
  
  if (arr == NULL) return 0;
  
  for (i=1; i<n; i++) {
    if (arr[i] < arr[iMax]) iMax = i;
  }
  return iMax;
}


int vector_pick_dominant_peak(double *arr, int n, double *dominance)
{
  int i;
  int *iMinMax;
  int niMinMax=0,first=1;   
  // variable first: 1 = min, 0 = max  (= pos. of first max)
  
  // we search for min/max, then pick the peak with highest value of:
  //  ( (max-min_left) + (max - min_right) )/2
  
  if (arr == NULL) return 0;
  
  iMinMax = (int *) calloc(1,sizeof(int)*n);
  
  for (i=1; i<n-1; i++) {
    // search for min and max, they only occur in turns
    if ( (arr[i-1]<arr[i]) && (arr[i] > arr[i+1]) ) { // local max
      if (niMinMax == 0) first = 0;
      iMinMax[niMinMax] = i; niMinMax++;
    }
    if ( (arr[i-1]>arr[i]) && (arr[i] < arr[i+1]) ) { // local min
      if (niMinMax == 0) first = 1;
      iMinMax[niMinMax] = i; niMinMax++;
    }
    // PROBLEM (only very rare!) plateu points with exact equal values!
  }
  
  // check if no max was found, or only 1
  if (niMinMax < 4) { // not enough min/max to determine dominant max
    int ret = vector_max_index_double(arr,n);
    if (dominance != NULL) *dominance = arr[ret];
    return ret;
  } 

  // find dominat max. as described above
  // start at first max.
  double maxSum = 0.0;
  int maxIndex=first;  // good guess, but favours high tempos
//printf("zero: %i %i\n",iMinMax[0],0);  

//printf("first: %i %i\n",iMinMax[first],first);  
  for (i=first; i<niMinMax; i += 2) {
    double sum = 0.0;
    if (i<niMinMax-1) {  // right min
      sum += arr[iMinMax[i]] - arr[iMinMax[i+1]];
    }
    if (i > 0) {  // left min
      if (sum == 0.0) 
        sum += arr[iMinMax[i]] - arr[iMinMax[i-1]];
      else 
        sum = (sum + arr[iMinMax[i]] - arr[iMinMax[i-1]]) / 2.0;
    }
    if (sum > maxSum) { // sum always assumed to be positive
      maxSum = sum;
      maxIndex = i;
    }
  }
  maxIndex = iMinMax[maxIndex];
//printf("maxInd: %i\n",maxIndex); 
  
  free(iMinMax);
  if (dominance != NULL) *dominance = maxSum;
  return maxIndex;
}

int vector_pick_dominant_peak_iv(double *arr, int n, double *dominance, int *second, double *dominance2)
{
  int i;
  int *iMinMax;
  int niMinMax=0,first=1;   
  // variable first: 1 = min, 0 = max  (= pos. of first max)
  
  // we search for min/max, then pick the peak with highest value of:
  //  ( (max-min_left) + (max - min_right) )/2
  
  if (arr == NULL) return 0;
  
  iMinMax = (int *) calloc(1,sizeof(int)*n);
  
  for (i=1; i<n-1; i++) {
    // search for min and max, they only occur in turns
    if ( (arr[i-1]<arr[i]) && (arr[i] > arr[i+1]) ) { // local max
      if (niMinMax == 0) first = 0;
      iMinMax[niMinMax] = i; niMinMax++;
    }
    if ( (arr[i-1]>arr[i]) && (arr[i] < arr[i+1]) ) { // local min
      if (niMinMax == 0) first = 1;
      iMinMax[niMinMax] = i; niMinMax++;
    }
    // PROBLEM (only very rare!) plateu points with exact equal values!
  }
  
  // check if no max was found, or only 1
  if (niMinMax < 4) { // not enough min/max to determine dominant max
    int ret = vector_max_index_double(arr,n);
    if (dominance != NULL) *dominance = arr[ret];
    return ret;
  } 

  // find dominat max. as described above
  // start at first max.
  double maxSum = 0.0;
  int maxIndex=first;  // good guess, but favours high tempos
//printf("zero: %i %i\n",iMinMax[0],0);  

//printf("first: %i %i\n",iMinMax[first],first);  
  for (i=first; i<niMinMax; i += 2) {
    double sum = 0.0;
    if (i<niMinMax-1) {  // right min
      sum += arr[iMinMax[i]] - arr[iMinMax[i+1]];
    }
    if (i > 0) {  // left min
      if (sum == 0.0) 
        sum += arr[iMinMax[i]] - arr[iMinMax[i-1]];
      else 
        sum = (sum + arr[iMinMax[i]] - arr[iMinMax[i-1]]) / 2.0;
    }
    if (sum > maxSum) { // sum always assumed to be positive
      maxSum = sum;
      maxIndex = i;
    }
  }
  
  //with first max, search second highest, left AND RIGHT of it
  int secondMaxIndex = first;
  double secondMaxSum = 0.0;
  for (i=first; i<maxIndex; i += 2) {
    double sum = 0.0;
    if (i<maxIndex-1) {  // right min
      sum += arr[iMinMax[i]] - arr[iMinMax[i+1]];
    }
    if (i > 0) {  // left min
      if (sum == 0.0) 
        sum += arr[iMinMax[i]] - arr[iMinMax[i-1]];
      else 
        sum = (sum + arr[iMinMax[i]] - arr[iMinMax[i-1]]) / 2.0;
    }
    if (sum > secondMaxSum) { // sum always assumed to be positive
      secondMaxSum = sum;
      secondMaxIndex = i;
    }
  }
  
  maxIndex = iMinMax[maxIndex];
  secondMaxIndex = iMinMax[secondMaxIndex];
  
  // check which max to choose
/*  double sum1=0.0,sum2=0.0;
  if ((secondMaxIndex > 1)&&(secondMaxIndex < n-2)) {
    sum1 = (arr[secondMaxIndex-2] + arr[secondMaxIndex-1] +arr[secondMaxIndex] + arr[secondMaxIndex+1] + arr[secondMaxIndex+2])/arr[secondMaxIndex];
  }
  if ((maxIndex > 1)&&(maxIndex < n-2)) {
    sum2 = (arr[maxIndex-2] + arr[maxIndex-1] +arr[maxIndex] + arr[maxIndex+1] + arr[maxIndex+2])/arr[maxIndex];    
  }  
  if (sum1 > sum2) maxIndex = secondMaxIndex;*/
  
//  if (arr[secondMaxIndex] > arr[maxIndex]) maxIndex = secondMaxIndex;
//printf("maxInd: %i\n",maxIndex); 
  
  free(iMinMax);
  if (dominance != NULL) *dominance = maxSum;
  if (second != NULL) *second = secondMaxIndex;
  if (dominance2 != NULL) *dominance2 = secondMaxSum;
  return maxIndex;
}

/* find first peak having a average relative hight of at least 1.2
   (whereby the global minimum is the zero reference point)
   and a total height of greater than 0.2 the maximum */
int vector_pick_first_peak(double *arr, int n, double *dominance)
{
  int i;
  int *iMinMax;
  int niMinMax=0,first=1;   
  // variable first: 1 = min, 0 = max  (= pos. of first max)
  
  double vMax;
  double vMin;

  if (arr == NULL) return 0;
  
  vMax = vector_max(arr,n);
  vMin = vector_min(arr,n);
  
  iMinMax = (int *) calloc(1,sizeof(int)*n);
  
  for (i=1; i<n-1; i++) {
    // search for min and max, they only occur in turns
    if ( (arr[i-1]<arr[i]) && (arr[i] > arr[i+1]) ) { // local max
      if (niMinMax == 0) first = 0;
      iMinMax[niMinMax] = i; niMinMax++;
    }
    if ( (arr[i-1]>arr[i]) && (arr[i] < arr[i+1]) ) { // local min
      if (niMinMax == 0) first = 1;
      iMinMax[niMinMax] = i; niMinMax++;
    }
    // PROBLEM (only very rare!) plateu points with exact equal values!
  }
  
  // check if no max was found, or only 1
  if (niMinMax < 2) { // not enough min/max to determine dominant max
    int ret = vector_max_index_double(arr,n);
    if (dominance != NULL) *dominance = arr[ret];
    return ret;
  } 

  int maxIndex=first;  // good guess, but favours high tempos
  while ( ((arr[iMinMax[maxIndex]]-vMin) < 0.4*(vMax-vMin))
          && maxIndex < niMinMax ) {
    maxIndex += 2;  // jump to next maximum
  }
  

  if (dominance != NULL) *dominance = arr[iMinMax[maxIndex]];
  maxIndex=iMinMax[maxIndex];
  free(iMinMax);
  return maxIndex;
}

/* Get global max. w. index */
int vector_max_index_uint32_t(uint32_t *arr, int n)
{
  int i,iMax=0;
  
  if (arr == NULL) return 0;
  
  for (i=1; i<n; i++) {
    if (arr[i] > arr[iMax]) iMax = i;
  }
  return iMax;
}

int vectors_modfit_by_corr_double(double *v1, double *v, int n, int winhalf)
{
  int i,j;
  
  double maxSum=0; int maxIndex=0;
  double tmp_sum;
  
  for (i=-winhalf; i<=winhalf; i++) {
    tmp_sum = 0.0;
    for (j=0; j<n; j++) {
      tmp_sum += v1[j] * v[(j+i+n)%n];
    }
    if (tmp_sum > maxSum) {
                maxSum = tmp_sum;
                maxIndex = i;
    }
  }
  return maxIndex;
}




/************** UNUSED ??????? ********************/

typedef struct {
  double val;
  int16_t index;  
} s_peak;

void fit_max(s_peak *pk,int np,double val,int index)
{
  int i,j;
  for (i = 0;  i<np; i++) {
    if (val > pk[i].val) {
      // move follwing peaks down
      for (j = np-1; j>i; j--) {
        pk[j].val = pk[j-1].val;  
        pk[j].index = pk[j-1].index;
      }
      // add at current position    
      pk[i].val = val;
      pk[i].index = index;    
      break;
    }
  }  
}

s_peak *vector_pick_peaks(double *v, int n, int n_peaks)
{
  int i;
  int ambiguos_max = 0;
  
  s_peak *peaks;
  peaks = (s_peak *)calloc(1,sizeof(s_peak)*n_peaks);
  
  for (i=1; i<n-1; i++) {
    if ((v[i-1] < v[i]) && (v[i] > v[i+1])) {
      fit_max(peaks,n_peaks,v[i],i);          
    }
    if ((v[i-1] < v[i]) && (v[i] == v[i+1])) {
      ambiguos_max = 1;            
    }            
    if (ambiguos_max && (v[i] > v[i+1])) {
      fit_max(peaks,n_peaks,v[i],i);              
      ambiguos_max = 0;                 
    }
  }     
  
  return peaks;
}

void vector_sort_int16_t(int16_t *v, int n)
{
  int i,j,max;
  int16_t tmp;
  
  if (n<=1) return;
  
  for (i=0; i<n-1; i++) {
    max = i;   
    for (j=i+1; j<n; j++) {
      if (v[j] > v[max]) max = j;
    }    
    //swap
    if (max != i) {
      tmp = v[max];
      v[max] = v[i];
      v[i] = tmp;
    }  
  }     
}

void vector_sort_uint16_t(uint16_t *v, int n)
{
  int i,j,max;
  uint16_t tmp;
  
  if (n<=1) return;
  
  for (i=0; i<n-1; i++) {
    max = i;   
    for (j=i+1; j<n; j++) {
      if (v[j] > v[max]) max = j;
    }    
    //swap
    if (max != i) {
      tmp = v[max];
      v[max] = v[i];
      v[i] = tmp;
    }  
  }     
}

void vector_invsort_int16_t(int16_t *v, int n)
{
  int i,j,min;
  int16_t tmp;
  
  if (n<=1) return;
  
  for (i=0; i<n-1; i++) {
    min = i;   
    for (j=i+1; j<n; j++) {
      if (v[j] < v[min]) min = j;
    }    
    //swap
    tmp = v[min];
    v[min] = v[i];
    v[i] = tmp;
  }     
}

void vector_insert_int16_t(int16_t *v, int n, int16_t val, int index)
{
  int i;
  
  if (n<=0) return;
  if ((index >= n)||(index<0)) return;
  
  for (i=n-1; i>index; i--) {
    v[i] = v[i-1];
  }     
  v[index] = val;
}

void vector_insertinv_int16_t(int16_t *v, int n, int16_t val, int index)
{
  int i;
  
  if (n<=0) return;
  if ((index >= n)||(index<0)) return;
  
  for (i=0; i<index; i++) {
    v[i] = v[i+1];
  }     
  v[index] = val;
}

void vector_insertsorted_int16_t(int16_t *v, int n, int16_t val)
{
  int i;
  if (n<=0) return;  
    
  for (i=0; i<n; i++) {
    if (v[i] <= val) {
      vector_insert_int16_t(v,n,val,i);        
      break;
    }
  }     
}

void vector_insertinvsorted_int16_t(int16_t *v, int n, int16_t val)
{
  int i;
  if (n<=0) return;  
    
  for (i=n-1; i>=0; i--) {
    if (v[i] < val) {
      vector_insertinv_int16_t(v,n,val,i);        
      break;
    }
  }     
}

void vector_insert_double(double *v, int n, double val, int index)
{
  int i;
  if (n<=0) return;  
  if ((index >= n)||(index<0)) return;
  
  for (i=n-1; i>index; i--) {
    v[i] = v[i-1];
  }     
  v[index] = val;
}

void vector_insert_uint32_t(uint32_t *v, int n, uint32_t val, int index)
{
  int i;
  if (n<=0) return;  
  if ((index >= n)||(index<0)) return;
  
  for (i=n-1; i>index; i--) {
    v[i] = v[i-1];
  }     
  v[index] = val;
}

void vector_delete_uint32_t(uint32_t *v, int n, int index)
{
  int i;
  if (n<=0) return;  
  if ((index >= n)||(index<0)) return;
  
  for (i=index; i<n-1; i++) {
    v[i] = v[i+1];
  } 
  v[n-1] = 0;
}

void vector_sort_double(double *v, int n)
{
  int i,j,max;
  double tmp;
  
  if (n<=1) return;
  
  for (i=0; i<n-1; i++) {
    max = i;   
    for (j=i; j<n; j++) {
      if (v[j] > v[max]) max = j;
    }    
    //swap
    tmp = v[max];
    v[max] = v[i];
    v[i] = tmp;
  }     
}

int is_element_of_int16_t(int16_t e, int16_t *v, int n)
{
  int i;
  for(i=0; i<n; i++) {
    if (e == v[i]) return 1;
  }
  return 0;
}

int is_element_of_double(double e, double *v, int n)
{
  int i;
  for(i=0; i<n; i++) {
    if (e == v[i]) return 1;
  }
  return 0;
}

int is_approx_element_of_int16_t(int16_t e, int16_t *v, int n, int16_t abserr)
{
  int i;
  for(i=0; i<n; i++) {
    if (intabs(e-v[i])<abserr) return 1;
  }
  return 0;
}

int is_approx_element_of_double(double e, double *v, int n, double abserr)
{
  int i;
  for(i=0; i<n; i++) {
    if (abs(e-v[i])<abserr) return 1;
  }
  return 0;
}

