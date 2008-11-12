/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
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
 *******************************************************************************/


 
void functionals_comp_maxmin(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *maxmin_names[] = {"max","min","maxPos","minPos","range"};

void functionals_comp_mmmdist(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *mmmdist_names[] = {"maxmeandist","minmeandist"};

void functionals_comp_mean(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *mean_names[] = {"mean","absmean"};

void functionals_comp_nzmean(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *nzmean_names[] = {"nzmean","nzN"};

void functionals_comp_qmean(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *qmean_names[] = {"qmean"};

void functionals_comp_quartile(FLOAT_TYPE_FTMEM *sorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *quartile_names[] = {"quartile1","median","quartile3","qrange21","qrange32","iqrange31"};

void functionals_comp_percentile(FLOAT_TYPE_FTMEM *sorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *percentile_names[] = {"percentile95","percentile98"};

void functionals_comp_moments(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *moments_names[] = {"stddev","variance","skewness","kurtosis"};

void functionals_comp_centroid(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *centroid_names[] = {"centroid"};

void functionals_comp_regression(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *regression_names[] = {"linregc1","linregc2","linregerrA","linregerrQ","qregc1","qregc2","qregc3","qregerrA","qregerrQ"};

void functionals_comp_zcr(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *zcr_names[] = {"zcr"};

void functionals_comp_mcr(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *mcr_names[] = {"mcr"};

void functionals_comp_peaks(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *peaks_names[] = {"numPeaks","meanPeakDist","peakMean","peakMeanMeanDist"};

void functionals_comp_segments(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *segments_names[] = {"numSegments"};

void functionals_comp_times(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *times_names[] = {"uplevelTime75","downlevelTime25","riseTime","fallTime"};

#define FIRST_DCT 0
#define N_DCT     6
void functionals_comp_dct(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared);
const char *dct_names[] = {"dct0","dct1","dct2","dct3","dct4","dct5"};

// define functionals that depend on other functionals BELOW those they depend on!
sFunctionalsConfig functConf[] = 
{
  {"maxmin",&functionals_comp_maxmin,NULL,5,UNSORTED, 0, maxmin_names},
  {"mean",&functionals_comp_mean,NULL,2,UNSORTED, 0, mean_names},     // arithmetic mean
  {"nzmean",&functionals_comp_nzmean,NULL,2,UNSORTED, 0, nzmean_names},     // arithmetic mean of non-zero values
  {"qmean",&functionals_comp_qmean,NULL,1,UNSORTED, 0, qmean_names},  // quadratic mean
  {"mmmdist",&functionals_comp_mmmdist,NULL,2,UNSORTED, 0, mmmdist_names},
  {"quartile",NULL,&functionals_comp_quartile,6,SORTED, 0, quartile_names},
  {"percentile",NULL,&functionals_comp_percentile,2,SORTED, 0, percentile_names},
  {"moments",&functionals_comp_moments,NULL,4,UNSORTED, 0, moments_names},
  {"centroid",&functionals_comp_centroid,NULL,1,UNSORTED, 0, centroid_names},  // depends on arithmetic mean
  {"regression",&functionals_comp_regression,NULL,9,UNSORTED, 0, regression_names},  // depends on centroid and arithmetic mean
  {"zcr",&functionals_comp_zcr,NULL,1,UNSORTED, 0, zcr_names},   // zero crossing rate
  {"mcr",&functionals_comp_mcr,NULL,1,UNSORTED, 0, mcr_names},  // depends on arithmetic mean  (mean crossing rate)
  {"peaks",&functionals_comp_peaks,NULL,4,UNSORTED, 0, peaks_names},  // depends on arithmetic mean  
  {"segments",&functionals_comp_segments,NULL,1,UNSORTED, 0, segments_names}, 
  {"times",&functionals_comp_times,NULL,4,UNSORTED, 0, times_names},  // depends on quartiles
  {"dct",&functionals_comp_dct,NULL,N_DCT,UNSORTED, 0, dct_names},  // no dependencies
  {NULL,NULL,NULL,0,0,0,NULL}
};

/******************************************* FUNCTION IMPLEMENTATIONS *******************************/

// output array:  max , min,  maxpos,  minpos , range
void functionals_comp_maxmin(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_maxmin"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM min,max;
  LONG_IDX i;
  LONG_IDX _minpos=0, _maxpos=0;

  if (bufLen <= 0) {
    min=0.0; max=0.0;           
  } else {
    min = unsorted[0];
    max = unsorted[0];
  }

  for (i=1; i<bufLen; i++) { // for each frame in the given range
    if (unsorted[i] > max) { max = unsorted[i]; _maxpos = i; }
    if (unsorted[i] < min) { min = unsorted[i]; _minpos = i; }
  }  

  if (shared != NULL) {
    shared->max = max;
    shared->min = min;
  }

  if (oCnt > 0) output[0] = max; 
  if (oCnt > 1) output[1] = min; 
  if (bufLen > 0) {
    if (oCnt > 2) output[2] = (FLOAT_TYPE_FTMEM)_maxpos / (FLOAT_TYPE_FTMEM)bufLen;  
    if (oCnt > 3) output[3] = (FLOAT_TYPE_FTMEM)_maxpos / (FLOAT_TYPE_FTMEM)bufLen;
  } 
  if (oCnt > 4) output[4] = max-min; 

  _FUNCTION_RETURNV_
}
#undef FUNCTION      


// arithmetic mean
// output array: mean
void functionals_comp_mean(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_mean"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM mean = 0.0;
  FLOAT_TYPE_FTMEM absmean = 0.0;
  LONG_IDX i;

  for (i=0; i<bufLen; i++) { // for each frame in the given range
    mean += unsorted[i];
    absmean += ftAbs(unsorted[i]);
  }  
  
  if (bufLen > 0) {
    if (oCnt > 0) output[0] = mean / (FLOAT_TYPE_FTMEM)bufLen;
    if (oCnt > 1) output[1] = absmean / (FLOAT_TYPE_FTMEM)bufLen;
  } 
  if (shared != NULL) { shared->mean = mean; }

  _FUNCTION_RETURNV_  
}
#undef FUNCTION

// non-zero value's arithmetic mean
// output array: nzmean
void functionals_comp_nzmean(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_nzmean"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM nzmean = 0.0;
  LONG_IDX i,Nz=0;

  if (bufLen <= 0) _FUNCTION_RETURNV_;
  
  for (i=0; i<bufLen; i++) { // for each frame in the given range
    if (unsorted[i] != 0.0) {
      nzmean += unsorted[i]; Nz++;
    }
  }  
  
  if (bufLen > 0) {
    if (oCnt > 0) 
      if (Nz > 0) output[0] = nzmean / (FLOAT_TYPE_FTMEM)Nz;
      else output[0] = 0.0;
    if (oCnt > 1) 
      output[1] = (FLOAT_TYPE_FTMEM)Nz / (FLOAT_TYPE_FTMEM)bufLen;
  } 

  _FUNCTION_RETURNV_  
}
#undef FUNCTION


// quadratic mean
// output array: qmean
void functionals_comp_qmean(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_qmean"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM qmean = 0;
  LONG_IDX i;

  for (i=0; i<bufLen; i++) { // for each frame in the given range
    qmean += unsorted[i]*unsorted[i];
  }  

  if ((bufLen > 0)&&(qmean >0.0)) {  
    if (oCnt > 0) output[0] = sqrt( qmean / (FLOAT_TYPE_FTMEM)bufLen );
  } 

  _FUNCTION_RETURNV_  
}
#undef FUNCTION

// for quartiles only the function working with sorted input is available
// output array: quartile1, quartile2(median), quartile3, qrange21, qrange32, iqrange31
void functionals_comp_quartile(FLOAT_TYPE_FTMEM *sorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_quartile"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM q1 =  (FLOAT_TYPE_FTMEM)(bufLen*25)/100.0 ;
  FLOAT_TYPE_FTMEM q3 =  (FLOAT_TYPE_FTMEM)(bufLen*75)/100.0 ;
 

  //q1
  LONG_IDX q1l = (LONG_IDX)q1; if (q1l <= 0) q1l = 0;
  LONG_IDX q1h = q1l+1; if (q1h >= bufLen) q1h = bufLen-1;
  FLOAT_TYPE_FTMEM rdiff = q1 - (FLOAT_TYPE_FTMEM)(q1l);
  
  if (oCnt > 0) {
    output[0] = sorted[q1l]*(1.0-rdiff) + sorted[q1h]*rdiff;
    if (shared != NULL) 
      shared->quartile1 =  output[0];
  }
  
  // median
  if (oCnt > 1) {
    if ((bufLen % 2) == 0) {  // mean of two mid values
      output[1] = (sorted[bufLen/2]+sorted[bufLen/2 - 1])/2.0;               
    } else {  // mid value
      output[1] = sorted[(bufLen-1)/2];
    }
    if (shared != NULL) 
      shared->quartile2 =  output[1];
  }

  //q3
  if (oCnt > 2) {
    q1l = (LONG_IDX)q3; if (q1l <= 0) q1l = 0;
    q1h = q1l+1; if (q1h >= bufLen) q1h = bufLen-1;
    FLOAT_TYPE_FTMEM rdiff = q3 - (FLOAT_TYPE_FTMEM)(q1l);
    output[2] = sorted[q1l]*(1.0-rdiff) + sorted[q1h]*rdiff;
//printf("q3xxxxx:  %f -> %f \n",q1l,);

    if (shared != NULL) 
      shared->quartile3 =  output[2];
  }
  
  if (oCnt > 3) output[3] = output[1]-output[0];
  if (oCnt > 4) output[4] = output[2]-output[1];
  if (oCnt > 5) output[5] = output[2]-output[0];
  
  _FUNCTION_RETURNV_  
}
#undef FUNCTION


// for quartiles only the function working with sorted input is available
// output array: percentile95, percentile98
void functionals_comp_percentile(FLOAT_TYPE_FTMEM *sorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_percentile"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM q95 =  (FLOAT_TYPE_FTMEM)((bufLen-1)*95)/100.0 ;
  FLOAT_TYPE_FTMEM q98 =  (FLOAT_TYPE_FTMEM)((bufLen-1)*98)/100.0 ;
 

  //q95
  LONG_IDX ql = (LONG_IDX)q95; 
  if (ql <= 0) ql = 0; if (ql>=bufLen) ql=bufLen-1;
  LONG_IDX qh = ql+1; if (qh >= bufLen) qh = bufLen-1;
  if (qh < ql) qh = ql;
  FLOAT_TYPE_FTMEM rdiff = q95 - (FLOAT_TYPE_FTMEM)(ql);
  
  if (oCnt > 0) output[0] = sorted[ql]*(1.0-rdiff) + sorted[qh]*rdiff;

  //q98
  ql = (LONG_IDX)q98; 
  if (ql <= 0) ql = 0; if (ql>=bufLen) ql=bufLen-1;
  qh = ql+1; if (qh >= bufLen) qh = bufLen-1;
  if (qh < ql) qh = ql;
  rdiff = q98 - (FLOAT_TYPE_FTMEM)(ql);
  
  if (oCnt > 1) output[1] = sorted[ql]*(1.0-rdiff) + sorted[qh]*rdiff;
  
  _FUNCTION_RETURNV_  
}
#undef FUNCTION

// standard deviation, variance, skewness and kurtosis
// output array: stddev, variance, skewness, kurtosis
void functionals_comp_moments(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_moments"
{_FUNCTION_ENTER_
  LONG_IDX i; 
  FLOAT_TYPE_FTMEM v2=0.0;
  FLOAT_TYPE_FTMEM v3=0.0;
  FLOAT_TYPE_FTMEM v4=0.0;
  FLOAT_TYPE_FTMEM mean;
  FLOAT_TYPE_FTMEM tmp,tmp2;
  
  
  if ((shared != NULL)&&(bufLen > 0)) {
    mean = shared->mean;
    for (i=0; i<bufLen; i++) {
      tmp = (unsorted[i] - mean);
      tmp2 = tmp*tmp;
      v2 += tmp2;
      tmp2 *= tmp;
      v3 += tmp2;
      v4 += tmp2*tmp;
    }
    v2 /= (FLOAT_TYPE_FTMEM)bufLen;  // variance
    if (oCnt > 1) output[1] = v2;

    if (v2 <= 0.0) {
      output[0] = 0.0;
      if (oCnt > 2) output[2] = 0.0;
      if (oCnt > 3) output[3] = 0.0;

    } else {
      output[0] = sqrt( v2 ); // stddev (sigma)
      tmp = ((FLOAT_TYPE_FTMEM)bufLen)*output[0]*output[0]*output[0];
    
      if (oCnt > 2) output[2] = v3 / tmp;
      if (oCnt > 3) output[3] = v4 / (tmp*output[0]);
    }
  }
  _FUNCTION_RETURNV_
}
#undef FUNCTION


// for this to run properly, mean, and maxmin must be enabled and listed in functsConf prior to mmmdist
// output array: maxmeandist, minmeandist
void functionals_comp_mmmdist(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_mmmdist"
{_FUNCTION_ENTER_
  if (shared != NULL) {
    output[0] = shared->max - shared->mean; if (oCnt == 1) _FUNCTION_RETURNV_;
    output[1] = shared->mean - shared->min;
  }
}
#undef FUNCTION

// centroid
// output array: centroid
void functionals_comp_centroid(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_centroid"
{_FUNCTION_ENTER_
  LONG_IDX i; 
  FLOAT_TYPE_FTMEM num=0.0;
  FLOAT_TYPE_FTMEM num2=0.0;  
  FLOAT_TYPE_FTMEM tmp;
  
  if ((shared != NULL)&&(bufLen > 0)) {
    if (shared->mean == 0.0) { output[0] = 0.0; }
    else {
      for (i=0; i<bufLen; i++) {
        tmp = unsorted[i]*(FLOAT_TYPE_FTMEM)i;
        num  += tmp;
        tmp *= (FLOAT_TYPE_FTMEM)i;
        num2 += tmp;
      }
      shared->centroid_numerator = num;
      shared->centroid_numerator2 = num2;
      output[0] = num / ( shared->mean * (FLOAT_TYPE_FTMEM)bufLen);
    }
  }
  _FUNCTION_RETURNV_
}
#undef FUNCTION

// linear and quadratic regression and regression error
// output array: "linregc1","linregc2","linregerrA","linregerrQ","qregc1","qregc2","qregc3","qregerrA","qregerrQ"
void functionals_comp_regression(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_regression"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM m,t,N,NNm1;
  LONG_IDX i;
  
  if ((shared != NULL)&&(bufLen>0)) {
    N=(FLOAT_TYPE_FTMEM)bufLen;
    NNm1=N*(N-1.0);
    m = (shared->centroid_numerator - shared->mean) / ( NNm1*(2.0*N-1.0)/6.0 - NNm1*(N-1.0)/4.0 );
    t = (shared->mean - m * NNm1/2.0);

    if (oCnt > 0) output[0] = m;
    if (oCnt > 1) output[1] = t;
//printf("m = %f\n"    ,m);
//printf("t = %f  (mean = %f)\n"    ,t,shared->mean);

    if (oCnt > 2) {
      // compute regression error:
      FLOAT_TYPE_FTMEM eqsum = 0.0;  // quadratic error
      FLOAT_TYPE_FTMEM easum = 0.0;  // absolute linear error
      FLOAT_TYPE_FTMEM e;
      FLOAT_TYPE_FTMEM iF;
      for (i=0; i<bufLen; i++) {
        iF = (FLOAT_TYPE_FTMEM)i;
        e = unsorted[i] - ( m*iF + t );
        eqsum += e*e;
        easum += ftAbs(e);
      }
      output[2] = easum / N;
      if (oCnt > 3) 
        if (eqsum > 0.0) output[3] = sqrt( eqsum ) / N;
        else output[3]  = 0.0;
//      printf(" Aerr: %f  N:%f\n",output[2],N);
//      printf(" Qerr: %f\n",output[3]);
    }
    
    if (oCnt > 4) {
      // compute quadratic regression
      FLOAT_TYPE_FTMEM s1, s2, s3, s4, m1, m2, m3;
      s1 = NNm1/2.0;
      s2 = NNm1*(2.0*N-1.0)/6.0;
      s3 = NNm1*NNm1 / 4.0;
      s4 = 0.0;
      FLOAT_TYPE_FTMEM I;
      for (I=0.0; I<N; I+=1.0) { 
          s4 += I*I*I*I;
      }
      m1 = shared->centroid_numerator;
      m2 = shared->centroid_numerator2;
      m3 = shared->mean * N;

      // solve 3x3 equation system via cramer's rule      
      FLOAT_TYPE_FTMEM a,b,c;
      FLOAT_TYPE_FTMEM det;
      det = N*(s3*s3 - s2*s4) + s1*(s1*s4 - 2.0*s2*s3);
      if (det != 0.0) {
        a = ( m1*(s3*N - s1*s2) + m2*(s1*s1 - N*s2) + m3*(s1*s3 + s2*s2) ) / det;
        b = ( m1*(s2*s2 - s4*N) + m2*(s3*N - s1*s2) + m3*(s1*s4 - s2*s3) ) / det;
        c = ( m1*(s1*s4 - s2*s3) + m2*(s2*s2 - s1*s3) + m3*(s3*s3 - s2*s4) ) / det;
      } else { a = 0.0; b= 0.0; c= 0.0; }

      output[4] = a;
      if (oCnt > 5) output[5] = b;
      if (oCnt > 6) output[6] = c;      
//printf("a = %f\n"    ,a);
//printf("b = %f\n"    ,b);
//printf("c = %f\n"    ,c);      

      if (oCnt > 7) {
        // compute quadratic regression errors
        FLOAT_TYPE_FTMEM eqsum = 0.0;  // quadratic error
        FLOAT_TYPE_FTMEM easum = 0.0;  // absolute linear error
        FLOAT_TYPE_FTMEM e;
        FLOAT_TYPE_FTMEM iF;
        for (i=0; i<bufLen; i++) {
          iF = (FLOAT_TYPE_FTMEM)i;
          e = unsorted[i] - ( a*iF*iF + b*iF + c );
          eqsum += e*e;
          easum += ftAbs(e);
        }
        output[7] = easum / N;
        if (oCnt > 8) 
          if (eqsum > 0.0) output[8] = sqrt( eqsum ) / N;
          else output[8]  = 0.0;
     // printf(" Aerr: %f  N:%f\n",output[7],N);
     // printf(" Qerr: %f\n",output[8]);
        
                 
      }
    }
  }
  _FUNCTION_RETURNV_
}
#undef FUNCTION


// zero crossing rate
// output array: zcr
void functionals_comp_zcr(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_zcr"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM last = 0.0;
  FLOAT_TYPE_FTMEM zcr = 0.0;
  int flag = 0;
  LONG_IDX i;

  if (bufLen <= 0) _FUNCTION_RETURNV_
  
  last = unsorted[0];
  for (i=1; i<bufLen; i++) { // for each frame in the given range
    if (  (unsorted[i] > 0.0)&&(last < 0.0)
         || (unsorted[i] < 0.0)&&(last > 0.0)  ) zcr += 1.0;
    else {
      if (flag) {
        if (  (unsorted[i] > 0.0) && (flag == -1) ) { zcr += 1.0; flag = 0; }
        else if ( (unsorted[i] < 0.0) && (flag == +1) ) { zcr += 1.0; flag = 0; }
        else if ( unsorted[i] != 0.0 ) flag = 0;
      } else {
        if ( (last < 0.0) && (unsorted[i] == 0.0) ) flag = -1;
        else if ( (last > 0.0) && (unsorted[i] == 0.0) ) flag = +1;
      }
    }
    
    last = unsorted[i];
  }  
  
  output[0] = zcr / (FLOAT_TYPE_FTMEM)bufLen;

  _FUNCTION_RETURNV_  
}
#undef FUNCTION


// mean crossing rate
// output array: mcr
void functionals_comp_mcr(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_mcr"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM last = 0.0;
  FLOAT_TYPE_FTMEM mcr = 0.0;
  FLOAT_TYPE_FTMEM m;
  int flag = 0;
  LONG_IDX i;

  if (bufLen <= 0) _FUNCTION_RETURNV_
  if (shared != NULL) {
    m  = shared->mean;
    last = unsorted[0];
    for (i=1; i<bufLen; i++) { // for each frame in the given range
      if (  (unsorted[i] > m)&&(last < m)
         || (unsorted[i] < m)&&(last > m)  ) mcr += 1.0;
      else {
        if (flag) {
          if (  (unsorted[i] > m) && (flag == -1) ) { mcr += 1.0; flag = 0; }
          else if ( (unsorted[i] < m) && (flag == +1) ) { mcr += 1.0; flag = 0; }
          else if ( unsorted[i] != m ) flag = 0;
        } else {
          if ( (last < m) && (unsorted[i] == m) ) flag = -1;
          else if ( (last > m) && (unsorted[i] == m) ) flag = +1;
        }
      }
      last = unsorted[i];
    }
  }  
  
  output[0] = mcr / (FLOAT_TYPE_FTMEM)bufLen;

  _FUNCTION_RETURNV_  
}
#undef FUNCTION

// peaks
// output array: "numPeaks","meanPeakDist","peakMean","peakMeanMeanDist"
void functionals_comp_peaks(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_peaks"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM npeaks = 0.0;
  FLOAT_TYPE_FTMEM peakDist = 0.0;  
  FLOAT_TYPE_FTMEM nPeakDist = 0.0;
  FLOAT_TYPE_FTMEM peakMean = 0.0;
  FLOAT_TYPE_FTMEM lastmin, lastmax=0.0;
  LONG_IDX i,curmaxPos=0;
  LONG_IDX lastmaxPos = -1;

  // running average over 1/10 of bufLen, also running max and min
  // peak: if x[i] > 2*mean-min  AND peak

  if (shared == NULL) return;
  if (bufLen <= 0) _FUNCTION_RETURNV_
  
  FLOAT_TYPE_FTMEM range = shared->max-shared->min;

  lastmin=unsorted[0];
  int peakflag = 0;  
  for (i=1; i<bufLen-1; i++) {
    // find ALL peaks:
    if ((unsorted[i-1] < unsorted[i])&&(unsorted[i] > unsorted[i+1])) { // max
      if (!peakflag) lastmax = unsorted[i];
      else { if (unsorted[i] > lastmax) { lastmax = unsorted[i]; curmaxPos = i; } }

      // TODO: detect peak only, if x[i] < lastmax - 0.05*range
      if (lastmax - lastmin > 0.11*range) { 
        peakflag = 1; curmaxPos = i;
      }

    } else 
    if ((unsorted[i-1] > unsorted[i])&&(unsorted[i] < unsorted[i+1])) { // min
      lastmin = unsorted[i];
      //peakflag = 0;
    }
    
    if ((peakflag)&& ( (unsorted[i] < lastmax-0.09*range) || (i==bufLen-2)) ) {
        npeaks += 1.0;
        peakMean += lastmax;
        if (lastmaxPos >= 0) {
          peakDist += (FLOAT_TYPE_FTMEM)(curmaxPos-lastmaxPos);
          nPeakDist += 1.0;
        }
        lastmaxPos = curmaxPos;
        peakflag = 0;
    }

/*
        npeaks += 1.0;
        peakMean += unsorted[i];
        if (lastmaxPos >= 0) {
          peakDist += (FLOAT_TYPE_FTMEM)(i-lastmaxPos);
          nPeakDist += 1.0;
        }
        lastmaxPos = i;
*/    
  }
  if (oCnt > 0) output[0] = npeaks / (FLOAT_TYPE_FTMEM)bufLen;
  if (peakDist == 0.0) { peakDist = 1.0; }
  else if (oCnt > 1) {
    if (nPeakDist>0)
      output[1] = peakDist / ( (FLOAT_TYPE_FTMEM)bufLen * nPeakDist);
    else 
      output[1] = 0.0;
  }
  if (oCnt > 2) output[2] = peakMean / (FLOAT_TYPE_FTMEM)bufLen;
  if (oCnt > 3) output[3] = output[2] - shared->mean;
  
//  printf(" nPeaks: %f\n",output[3]);  
//  printf(" peakDist: %f\n",output[1]);  

  _FUNCTION_RETURNV_  
}
#undef FUNCTION


// number of segments
// output array: numSegments
void functionals_comp_segments(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_segments"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM max;
  LONG_IDX i;

  FLOAT_TYPE_FTMEM segThresh = 0.2;
  LONG_IDX maxNumSeg = 20;

  if (shared == NULL) _FUNCTION_RETURNV_;
  segThresh *= (shared->max - shared->min);
  LONG_IDX ravgLng = bufLen/(maxNumSeg/2);
  LONG_IDX segMinLng = bufLen/maxNumSeg;
  LONG_IDX lastSeg = 0;
  FLOAT_TYPE_FTMEM ravg = 0.0;
  LONG_IDX nSeg = 0;
  
  max = unsorted[0];
  for (i=1; i<bufLen; i++) { // for each frame in the given range
    ravg += unsorted[i];
    if (i>=ravgLng) ravg -= unsorted[i-ravgLng];
    
    FLOAT_TYPE_FTMEM ra = ravg / (FLOAT_TYPE_FTMEM)ravgLng;
    if ((unsorted[i]-ra > segThresh)&&(i - lastSeg > segMinLng) )
    { // found new segment begin
      nSeg++;
      
      lastSeg = i;
    }
    
  }
  
  if (oCnt > 0) output[0] =(FLOAT_TYPE_FTMEM)nSeg / 20;
  //printf(" nSeg: %i\n", nSeg);
  
  _FUNCTION_RETURNV_  
}
#undef FUNCTION


// various level and rise times
// output array: "uplevelTime75","downlevelTime25","riseTime","fallTime"
void functionals_comp_times(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_times"
{_FUNCTION_ENTER_
  FLOAT_TYPE_FTMEM N=(FLOAT_TYPE_FTMEM)bufLen;
  LONG_IDX i;
  LONG_IDX downTime=0,riseTime=0;
  LONG_IDX upTime=0,fallTime=0;
  FLOAT_TYPE_FTMEM last = unsorted[0];  

  if (bufLen == 0) _FUNCTION_RETURNV_

  if (shared != NULL) {
    for (i=0; i<bufLen; i++) { // for each frame in the given range
      if (unsorted[i] > shared->quartile3) {
        upTime++;   
      }
      else if (unsorted[i] < shared->quartile1) {
        downTime++;   
      }
      
      if (last < unsorted[i]) {
        riseTime++;   
      }
      if (last > unsorted[i]) {
        fallTime++;   
      }

      last = unsorted[i];
    }
    
  }
  if (oCnt > 0) output[0] = (FLOAT_TYPE_FTMEM)upTime / N;
  if (oCnt > 1) output[1] = (FLOAT_TYPE_FTMEM)downTime / N;
  if (oCnt > 2) output[2] = (FLOAT_TYPE_FTMEM)riseTime / N;
  if (oCnt > 3) output[3] = (FLOAT_TYPE_FTMEM)fallTime / N;
  
  _FUNCTION_RETURNV_  
}
#undef FUNCTION


// dct coefficients
// output array: "dctFIRST_DCT",...,"dctN_DCT"
void functionals_comp_dct(FLOAT_TYPE_FTMEM *unsorted, LONG_IDX bufLen, FLOAT_TYPE_FTMEM *output, int oCnt, pSharedFuncs shared)
#define FUNCTION "functionals_comp_dct"
{_FUNCTION_ENTER_

  int i,m;
  // compute dct of data:
  FLOAT_TYPE_FTMEM outp[N_DCT];
  if (bufLen == 0) _FUNCTION_RETURNV_
  
  FLOAT_TYPE_FTMEM factor = sqrt(2.0/(FLOAT_TYPE)(bufLen));
  int nM = bufLen; 

    if ((shared == NULL)||(shared->costable == NULL)) {
      for (i=FIRST_DCT; i < FIRST_DCT+N_DCT; i++) {
         FLOAT_TYPE_FTMEM * outc = outp + (i-FIRST_DCT); *outc = 0.0;
         for (m=0; m<bufLen; m++) {
           *outc +=  unsorted[m] * cos(M_PI*(FLOAT_TYPE_FTMEM)i/(FLOAT_TYPE_FTMEM)(bufLen) * ((FLOAT_TYPE_FTMEM)(m+1) - 0.5) );
         }
         *outc *= factor; 
      }
    } else { 
      for (i=FIRST_DCT; i < FIRST_DCT+N_DCT; i++) {
         FLOAT_TYPE_FTMEM * outc = outp + (i-0); *outc = 0.0;
         for (m=0; m<bufLen; m++) {
           *outc +=  unsorted[m] * shared->costable[m+(i-FIRST_DCT)*nM];
         }
         *outc *= factor; 
      }
    }
   for (i=0; i<N_DCT; i++) {
     if (i<oCnt) output[i] = outp[i];   
   }

  _FUNCTION_RETURNV_  
}
#undef FUNCTION
