/***************************************************************
  Rhythm AS v0.1  (c) by Florian Eyben
  ------------------------------------
  ~~~~ mathfunctions.c / mathfunctions.h ~~~~~~~
  
  various math & vector functions
 ***************************************************************/

#ifndef __MATHFUNCTIONS_H
#define __MATHFUNCTIONS_H

#include <inttypes.h>

inline int symmetric_mod(int a, int b);
inline int symmetric_absmod(int a, int b);
int fit_in_modrange(int val, int range);
inline int intabs(int n);
inline int i2_argMax(int arg1, int arg2);
inline int i2_argMin(int arg1, int arg2);

/* sin(x) / x  function */
double sinc(double x);

double vector_max(double *arr, int n);
double vector_min(double *arr, int n);
double vector_mean(double *arr, int n);
double vector_mean_nz(double *arr, int n);

int vector_max_index_double(double *arr, int n);
int vector_max_index_uint32_t(uint32_t *arr, int n);
int vector_min_index_double(double *arr, int n);
int vector_max_index_width(double *arr, int n, int *width);

double vector_sum(double *v1, int n);
double vector_squaresum(double *v1, int n);

void vector_make_unitVector(double *arr, int n);
double vector_mean_normalize(double *arr, int n);
double vector_mean_normalize_pos(double *arr, int n);
double vector_max_normalize(double *arr, int n);
void vector_make_unitVector_meanZero(double *arr, int n);  /* obsolete !? */
void vector_make_zeroMean_unitVector(double *arr, int n);

double vectors_mse(double *v1, double *v2, int n);
double vectors_crossCorr(double *v1, double *v2, int n);

int vectors_modfit_by_corr_double(double *v1, double *v, int n, int winhalf);

/*******************************************************************
  Symmetric window functions:
  compute specific value over a window of size <range> centered on 
  absolute index <middle_index> of vector <arr>. 
  n is the total(!) length of vector <arr>.
 *******************************************************************/
double vector_symmWin_mean(double *arr, int n, int middle_index, int range);
double vector_symmWin_max(double *arr, int n, int middle_index, int range);
double vector_symmWin_max_index(double *arr, int n, int middle_index, int range);
double vector_symmWin_peak_index(double *arr, int n, int middle_index, int range);
double vector_symmWin_sum(double *arr, int n, int middle_index, int range);

/******************************************************
     functions for comparison with certain tolerance
 ******************************************************/
int approx_match_int(int i1, int i2, int tolerance);
int approx_multmatch_int(int i1, int i2, int tolerance, int maxmult);
int approx_multmatch_double(double i1, double i2, double tolerance, int minmult, int maxmult);
int approx_match_double(double i1, double i2, double tolerance);
int approx_modmultmatch_int(int i1, int i2, int tolerance);

/******************************************************
                     peak picking
 ******************************************************/
int vector_pick_dominant_peak(double *arr, int n, double *dominance);
int vector_pick_dominant_peak_iv(double *arr, int n, double *dominance, int *second, double *dominance2);
/* find first peak having a average relative hight of at least 1.2
   (whereby the global minimum is the zero reference point)
   and a total height of greater than 0.2 the maximum */
int vector_pick_first_peak(double *arr, int n, double *dominance);

#endif /* __MATHFUNCTIONS_H */
