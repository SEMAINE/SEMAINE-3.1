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


/*  openSMILE component: cVectorHEQ

This component extends the class cVectorMVN and implements Histogramm equalisation

HOWTO USE:
==========

good configurations:

a) update over buffer of last X (5) seconds:
updateMethod = buf
fixedBuffer = 5.0   (seconds)
stdRange = 0
fixedRange = 0
noHeqInit = currently irrelevant, because inital histogram is not used in 'buf' update method

b) update using cummulative sum (will be less dynamic the longer the input gets)
updateMethod = avg
stdRange = 1
fixedRange = 0
noHeqInit = 1  (this will reset the histogram at the beginning, if an old transformation file for MVN parameters is loaded)



TODO: full support of MRN instead of MVN .. fix issues of different typeID's infiles, etc.?
      what about loading an HVN_HEQ type file and checking for MVN type in alloc.... ?

*/

#include <vectorHEQ.hpp>

#define MODULE "cVectorHEQ"

// enable this to save various internal data to .dat files (csv format)
//#define SAVE_DEBUG_DATA 1   

SMILECOMPONENT_STATICS(cVectorHEQ)

SMILECOMPONENT_REGCOMP(cVectorHEQ)
{
  SMILECOMPONENT_REGCOMP_INIT
    scname = COMPONENT_NAME_CVECTORHEQ;
  sdescription = COMPONENT_DESCRIPTION_CVECTORHEQ;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorMVN")

       //REM: 'iavg'  cummulative average method with weighted variable (from the last turn) inital values\n   NOTE: if 'resetOnTurn'==0 then 'avg' and 'iavg' are identical
    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("updateMethod","Specifies how to incrementally update the transform. One of the following strings:\n   'fix'/'buf'  compute tranform over history buffer of fixed length\n   'avg'  cummulative average method with weighted fixed inital values\n   ","buf");
    ct->setField("nameAppend",(const char *)NULL,(const char *)NULL);
    ct->setField("numIntervals","The number of discrete bins to use during histogram computation (a higher number leads to a more fine grained histogram)",1000);
    ct->setField("histRange","The range of the histogram in terms of a factor applied to the standard deviation (range [mean-histRange*stddev;mean+histRange*stddev]). Please note, that this can lead to a changing range, if the standard deviation changes. This is not yet wll supported since it requires histogram rescaling. We recommend to initialise with an MVN file, and disable on-line updates of the standard deviation, or use the option 'fixedRange=1'.",4.0);
    ct->setField("meanTolerance","The percentage of allowed mean drift bevor the histogram range is adjusted and the histogram is rescaled",0.01);
    ct->setField("rangeTolerance","The percentage of allowed range drift bevor the histogram range is adjusted and the histogram is rescaled",0.01);
    ct->setField("fixedRange","1 = use a fixed range from the MVN initialisation file and do not attempt to update it, when the standard deviation changes",0);
    ct->setField("noHeqInit","1 = do not use HEQ data from init file (this is to be used in conjunction with 'fixedRange=1')",0);
    ct->setField("stdRange","1 = use a fixed absolute histogram range (implies fixedRange, but does not use the MVN data from the init file. Instead, the init data is generated as mean 0 and standard deviation 1, the histogram range is then given by the 'histRange' parameter as: -histRange to +histRange)",0);
  )

    SMILECOMPONENT_MAKEINFO(cVectorHEQ);
}



SMILECOMPONENT_CREATE(cVectorHEQ)

//-----

cVectorHEQ::cVectorHEQ(const char *_name) :
cVectorMVN(_name),
gauss(NULL),
cumhist(NULL),
lut(NULL),
curRange(NULL),
tfIsComp(0)
{
}

void cVectorHEQ::fetchConfig()
{
  cVectorMVN::fetchConfig();

  numIntervals = getInt("numIntervals");
  histRange = (FLOAT_DMEM)getDouble("histRange");

  if (numIntervals < 2) {
    SMILE_IWRN(2,"numIntervals < 2 : flooring to 2!");
    numIntervals = 2;
  }
  if (histRange < 1.0) {
    SMILE_IWRN(2,"histRange < 1.0 : this makes no sense , flooring to 1.0 (if you really intended to use this value, you must edit vectorHEQ.cpp and recompile.. sorry for that)!");
    histRange = 1.0;
  }

  SMILE_IDBG(2,"numIntervals = '%i'",numIntervals);
  SMILE_IDBG(2,"histRange = %f",histRange);

  meanTolerance = getDouble("meanTolerance");
  rangeTolerance = getDouble("rangeTolerance");
  SMILE_IDBG(2,"meanTolerance = '%i'",meanTolerance);
  SMILE_IDBG(2,"rangeTolerance = %f",rangeTolerance);

  stdRange = getInt("stdRange");
  SMILE_IDBG(2,"stdRange=%i",stdRange);
  //if (stdRange) fixedRange = 1;
  fixedRange = getInt("fixedRange");
  SMILE_IDBG(2,"fixedRange=%i",fixedRange);
  noHeqInit = getInt("noHeqInit");
  SMILE_IDBG(2,"noHeqInit=%i",noHeqInit);

}

void cVectorHEQ::allocTransformData(struct sTfData *tf, int Ndst, int idxi) 
{
  cVectorMVN::allocTransformData(tf,Ndst,idxi);

  if (tf->head.typeID == 0) tf->head.typeID = TRFTYPE_MVN;
  // check.. we need MVN or MRN type for HEQ!!
  if ( (tf->head.typeID != TRFTYPE_MVN) && (tf->head.typeID != TRFTYPE_MRN) ) {
    SMILE_IERR(1,"histogram equalisation (HEQ) requires MVN (mean/variance) or MRN (mean/range) normalisation data to be loaded as init data (if any) [type %i]. It seems that the currently loaded data is of a different type (%i). The values will be bogus, or openSMILE will crash...",TRFTYPE_MVN,tf->head.typeID);
    COMP_ERR("Thus, we are aborting right here, to allow you to fix the problem");
  }
  startVec = tf->head.nVec;  // startVec is index of vector where HEQ histogram data begins

  tf->head.nGroups++;
  tf->head.nVec += numIntervals;
  if (tf->head.typeID == TRFTYPE_MVN)
    tf->head.typeID = TRFTYPE_MVN_HEQ;
  else if (tf->head.typeID == TRFTYPE_MRN) {
    tf->head.typeID = TRFTYPE_MRN_HEQ;
    SMILE_IWRN(1,"Transform type MRN_HEQ (mean/range normalisation HEQ) is not yet fully supported! I.e. the code is not tested and possibly very buggy! You are WARNED!! (However, you are welcome to submit bugfixes ;-) )");
  }

  struct sTfData * tf0 = getTransformInitData();
  if (tf0->head.typeID == 0) tf0->head.typeID = tf->head.typeID;

  // reallocate memory for vectors matrix
  tf->vectors = (double *)realloc(tf->vectors, sizeof(double)*tf->head.nVec*tf->head.vecSize);

  // allocate memory for ranges, and init standard ranges if option is set
  long i;
  if (curRange != NULL) free(curRange);
  curRange = (sHEQrange *)calloc(1,sizeof(sHEQrange)*tf->head.vecSize);
  if (stdRange) {
    for (i=0; i<tf->head.vecSize; i++) {
      curRange[i] = generateStdRange();
    }
  }

  // allocate various other arrays
  if (lut != NULL) free(lut); 
  lut = (double*)malloc(sizeof(double)*(numIntervals+1));
  if (cumhist != NULL) free(cumhist); 
  cumhist = (double*)malloc(sizeof(double)*numIntervals);
  if (gauss != NULL) free(gauss); 
  gauss = (double*)malloc(sizeof(double)*numIntervals);

  /* compute gaussian reference cummulative pdf */
  double start = (double)(-histRange);
  double end = (double)(histRange);
  double step = (end-start)/(double)numIntervals;
  double factor = 1.0/(sqrt(2.0*M_PI)) ;
  double x = start;
  // compute normalised gaussian PDF
  for(i=0; i<numIntervals; i++) {
    gauss[i] = step * factor * exp ( - (x*x)/2.0 ); 
    x += step;
  }
#ifdef SAVE_DEBUG_DATA
  saveDoubleVector_csv("gauss.dat",gauss,numIntervals,0);
#endif
  // sum up to normalised cummulative PDF
  for(i=1; i<numIntervals; i++) {
    gauss[i] += gauss[i-1];
  }
#ifdef SAVE_DEBUG_DATA
  saveDoubleVector_csv("gauss.dat",gauss,numIntervals,1);
#endif
  return;
}

void cVectorHEQ::modifyInitTransform(struct sTfData *tf0) 
{
  int err=0;
  struct sTfData * tf = getTransformData();
  if (tf0->head.typeID == TRFTYPE_MVN_HEQ) return;
  if (tf0->head.typeID == TRFTYPE_MRN_HEQ) return;
  if (tf0->head.typeID == TRFTYPE_MVN) {
    if (tf->head.typeID = TRFTYPE_MVN_HEQ) tf0->head.typeID = TRFTYPE_MVN_HEQ;
    else { SMILE_IERR(1,"MVN init data is incompatible with MRN_HEQ !"); err=1; }
  } else if (tf0->head.typeID == TRFTYPE_MVN) {
    if (tf->head.typeID = TRFTYPE_MRN_HEQ) tf0->head.typeID = TRFTYPE_MRN_HEQ;
    else { SMILE_IERR(1,"MRN init data is incompatible with MVN_HEQ !"); err=1; }
  } else {
    SMILE_IERR(1,"Init data loaded from file [typeID=%i] is incompatible with MVN_HEQ transform type !",tf0->head.typeID);
    err=1;
  }
  if (err) {
    COMP_ERR("Aborting due to previous error!");
  }
}

void cVectorHEQ::transformInitDone(struct sTfData *tf, struct sTfData *tf0)
{
  if (noHeqInit) {
    // zero the HEQ histograms:
    long i;
    double * hist = tf->vectors+(startVec*tf->head.vecSize);
    for (i=0; i<tf->head.vecSize*numIntervals; i++) {
      hist[i] = 0.0;
    }
    nFrames=0;  // TODO: does this affect mean/range/variance computation / initialisation ?
  }
}

sHEQrange cVectorHEQ::generateStdRange(void)
{
  sHEQrange ret;
  ret.mean = 0;
  ret.start = - (double)histRange;
  ret.end = (double)histRange;
  ret.step = 2.0*(double)histRange / (double)numIntervals;
  ret.isInit = 1;
  return ret;
}

sHEQrange cVectorHEQ::computeHEQrange(const struct sTfData * tf, int index)
{
  double *mean = tf->vectors;
  sHEQrange ret;
  ret.mean = mean[index];
  if (tf->head.typeID == TRFTYPE_MVN_HEQ) {
    double *stddev = mean + tf->head.vecSize;

    ret.start =  - (double)histRange * stddev[index];
    //if (isnan(ret.start)) printf("HHHH %f %f %i\n",stddev[index],mean[index],index);

    ret.end = + (double)histRange * stddev[index];
  } else if (tf->head.typeID == TRFTYPE_MRN_HEQ) {
    double *min = mean + tf->head.vecSize;
    double *max = min + tf->head.vecSize;
    ret.start = min[index] - ret.mean;
    ret.end = max[index] - ret.mean;
  }
  ret.step = (ret.end - ret.start) / (double)numIntervals;
  //  printf (" compute: %f _ %f _ %f \n",ret.start,ret.mean,ret.end);
  return ret;
}

/* get the index of the interval the given a real value (NON mean normalised) is in */
long cVectorHEQ::getIntervalIndex(double x, sHEQrange &curRange)
{
  long ret;
  //printf("x: %f , mean: %f, start: %f , step: %f\n",x,curRange.mean, curRange.start, curRange.step);
  if (curRange.step <= 0.0) { 
    ret = -1; SMILE_IWRN(4,"WARNING: HEQ RANGE stepsize=0\n"); 
  } else {
    ret = (long) floor(  ((x-curRange.mean)-curRange.start) / curRange.step  );
    if (ret < 0) ret = 0;
  }
 
  return ret;
}

/* get the start value (NON mean normalised) of the interval, 
   given an index  (use index + 1 to get the end value) */
double cVectorHEQ::getIntervalStart(long n, sHEQrange &curRange)
{
//  printf (" cr: %f _ %f _ %f \n",curRange.start,curRange.mean,curRange.end);
  double ret = (double)n * curRange.step + curRange.start + curRange.mean;
  return ret;
}

int cVectorHEQ::HEQrangeChanged(sHEQrange &newRange, sHEQrange &curRange)
{
  double meanDiff = fabs(newRange.mean - curRange.mean);
  double curRa = curRange.end - curRange.start;
  double rangeDiff = fabs(newRange.end - newRange.start - curRa);
  if (rangeDiff > curRa * rangeTolerance) return 1;
  if (meanDiff > curRange.mean * meanTolerance) return 1;
  return 0;
}

void cVectorHEQ::HEQrangeAssign(sHEQrange &curRange, sHEQrange &newRange)
{
  curRange.start = newRange.start;
  curRange.end = newRange.end;
  curRange.mean = newRange.mean;
  curRange.step = newRange.step;
  curRange.isInit = 1;
}

// remap a histogram to a new mean, range and stepsize
void cVectorHEQ::remapHistogram(struct sTfData * tf, int i, sHEQrange &curRange, sHEQrange &newRange)
{
  long j,n;
  long N = tf->head.vecSize;
  double *hist = tf->vectors+(startVec*tf->head.vecSize) + i*numIntervals;
  // note: we use the cumhist array for temporary storage of the new histogram

  // the start/end values in range are mean normalised, we must add the mean here to get the absolute values
  double newstart = newRange.start + newRange.mean;
  double newend = newRange.end + newRange.mean;
  double curstart = curRange.start + curRange.mean;
  double curend = curRange.end + curRange.mean;

  double cpos = curstart;
  //// mapping:

/*
  for (j=0; j<numIntervals; j++) {
    if (hist[j] < 0.0) { printf("H<0 %i %f\n",j,hist[j]); }
  }
  */

  /*
    start with first bin in new histogram
      -if no old data present, set to zero
      -else sum up all old data up to current pos
  */
  if (curRange.step <= 0.0) {
    // if the current histogram is singular, create a histogram for new range with a single peak
    long iv = getIntervalIndex(curRange.mean, newRange);
    for (j=0; j<numIntervals; j++) {
      cumhist[j] = 0.0;
    }
    if (iv==-1) { iv = 0; }
    else if (iv>=numIntervals) iv = numIntervals-1;
    cumhist[iv] = 1.0; // / (double)numIntervals; // !!!
  } else {
    for (j=0; j<numIntervals; j++) {
      double iv_start = getIntervalStart(j,newRange);  // interval start
      double iv_end = getIntervalStart(j+1,newRange);  // interval end = start of next interval
      long cposI = getIntervalIndex(cpos, curRange);
      long iv_endI = getIntervalIndex(iv_end, curRange);
      long iv_startI = getIntervalIndex(iv_start, curRange);

      cumhist[j] = 0.0;
      if ((iv_endI >= 0)&&(iv_startI<numIntervals)) {

        if (cpos < iv_start) {
          // sum to current bin:
          for (; cposI<iv_startI; cposI++) {
            cumhist[j] += hist[cposI];
            cpos += curRange.step;
          }
          // add partial last element, if it is the first element of the new histogram
          if ( (j==0) && ((cpos < iv_start)&&(cpos + curRange.step > iv_start)) ) {
            cumhist[j] += (iv_start-cpos)/curRange.step * hist[iv_startI];
          }
        }

        // add partial last element... ?
        int isFrac = 0;
        if ((cpos < iv_start)&&(cposI >= 0)&&(cposI<numIntervals)) {
          double frac = (iv_start-cpos);
          if (frac < curRange.step) {
            if (frac > newRange.step) {
              cumhist[j] += newRange.step/curRange.step * hist[cposI];
              isFrac = 1;
            } else {
              cumhist[j] += ( (curRange.step - frac)/curRange.step ) * hist[cposI];
              cposI++;
              cpos += curRange.step;
            }
          }
        }

        // sum from cpos to iv_end
        for (n=cposI; (n<iv_endI)&&(n<numIntervals); n++) {
          cumhist[j] += hist[n];
          cpos += curRange.step;
        }
        cposI = n;

        // compute weight w
        if ((!isFrac)&&(iv_endI < numIntervals)) {
          double w;

          if (iv_end > curend) w = (curend - cpos)/(curRange.step);
          else w = (iv_end - cpos)/(curRange.step);
          // partially add iv_endI bin using weight w
          if (w>0.0) {
            cumhist[j] += hist[iv_endI] * w;
          }
        }

      } // else exp. fill from first cur element...?
      if (cumhist[j] < 0) {
        SMILE_ERR(1,"HEQ remap histogram: A value in the remapped histogram is < 0 !! This is a bug in the code! Please send configuration files and all necessary data file to reproduce this error (and this logfile) to the authors! Thanks!  (index %i, value %f)",j,cumhist[j]);
      }
    }
    // add remaining bins of old histogram to end of new histogram (if any...)
    // TODO!?

    if (newend < curend) {
      long cposI = getIntervalIndex(cpos, curRange);
      if (cposI==-1) cposI = 0;
      for (n=cposI; n<numIntervals; n++) {
        cumhist[numIntervals-1] += hist[n];
        //cpos += curRange.step;
      }
      //cposI = n;
    }

  }

#ifdef SAVE_DEBUG_DATA
  saveDoubleVector_csv("maphist.dat",hist,numIntervals,1);
  saveDoubleVector_csv("maphist.dat",cumhist,numIntervals,1);
#endif

  // copy cumhist temp. storage to hist:
  for (j=0; j<numIntervals; j++) {
    hist[j] = cumhist[j];
  }

}


int cVectorHEQ::transformDataFloat(const struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi)
{
  long i,j;
  long N = tf->head.vecSize;


  if (stdRange) {
    int ret = cVectorMVN::transformDataFloat(tf,src,dst,Nsrc,Ndst,idxi);
    if (!ret) return ret;
  }

  // NOTE: if not stdRange is set, the parent transform (from cVectorMVN) is deliberately NOT called !!

  double *hist = tf->vectors+(startVec*tf->head.vecSize);

  double start = (double)(-histRange);
  double end = (double)(histRange);
  double step = (end-start)/(double)numIntervals;

  for (i=0; i<MIN(Nsrc,Ndst); i++) {

    if (curRange[i].isInit == 0) {
      sHEQrange newRange = computeHEQrange(tf,i);
      HEQrangeAssign(curRange[i], newRange); // assign new range
    }

    double X; // <-- current source value 
    if (stdRange) {
      X=dst[i];  // use dst if stdRange (the MVN parent transform was applied before, thus our source data is in dst[] ...)
    } else {
      X=src[i];
    }
    //X=src[i];

    
    if ((!tfIsComp)||(mode != MODE_TRANSFORMATION)) {
      // compute cummulative histogram & look-up table
      long x=0; // <-- current pos in gaussian histogram
      lut[0] = start;
      double lutpos = start+step;

      //long c = i*numIntervals;
      double *hi = hist+(i*numIntervals);
      double *ci = cumhist;
      
      double ci_old = *(ci++) = *(hi++); // cumhist[0] = hist[c];
      long ni1 = numIntervals-1;

      for (j=1; j<numIntervals; j++) {
        //cumhist[j] = cumhist[j-1] + hist[c+j];
        ci_old += *(hi++);
        *(ci++) = ci_old; //+ *(hi++);
        while ( (x<ni1)&&( gauss[x] < ci_old /* cumhist[j]*/) ) 
          { lutpos += step; x++; }
        lut[j] = lutpos;
      }
      lut[numIntervals] = lutpos+step;

      // normalisation (is necessary due to roundoff errors during histogram remapping and on-line updates)
      //double factor = 1.0/cumhist[numIntervals-1];
      //for (j=0; j<numIntervals; j++) {
      //cumhist[j] *= factor;
      //}
#ifdef SAVE_DEBUG_DATA
      saveDoubleVector_csv("cumhist.dat",cumhist,numIntervals,1);
#endif
      // compute look-up table  (already done above...)
      //  start with cumhist and for each entry  search match in cum. gaussian (index of match is new range)
      /*
      for (j=1; j<numIntervals; j++) {
                while ( (x<numIntervals-1)&&( gauss[x] < cumhist[j]) ) 
          { x++; lutpos += step; }
        lut[j] = lutpos;
      }
      lut[numIntervals] = lutpos+step;
      */

#ifdef SAVE_DEBUG_DATA
      saveDoubleVector_csv("heqlut.dat",lut,numIntervals+1,0);
#endif
      tfIsComp = 1;
    }

    /* apply transform using the look-up-table */
    double startV = curRange[i].start; // * stddev[i];
    double stepV = curRange[i].step; //2.0*startV / (double)numIntervals;
    long ivI = getIntervalIndex(X,curRange[i]); //(src[i]-mean[i]-startV)/stepV;
    if (ivI >= numIntervals) ivI = numIntervals - 1;
    if (ivI < 0) ivI = 0;
    //long ivI = (long)floor(iv);
    // interval start/end in original (linear) space
    double istart = getIntervalStart(ivI,curRange[i]); //(double)ivI * stepV + startV;
    double iend = getIntervalStart(ivI+1,curRange[i]); //istart + stepV;
    // interval start/end in target (gaussian) space
//    printf("end: %f  %f  %f (ni)\n",curRange[i].end,end,lut[numIntervals]);
//    printf("start: %f  %f  %f (ni)\n",curRange[i].start,start,lut[0]);

    double gstart = lut[ivI];
    double gend = lut[ivI+1];

    // istart <(=) src[i] <(=) iend should be now true...?
    // so do the actual transform using linear interpolation:
    if ((iend-istart)<=0.0) dst[i] = (FLOAT_DMEM)gstart;
    else dst[i] = (FLOAT_DMEM)( ((double)X-istart)/(iend-istart) * (gend - gstart) + gstart );
  }
  //printf("dst[0] = %f  src[0] = %f  [%i]\n",dst[0],src[0],MIN(Nsrc,Ndst));
  return MIN(Nsrc,Ndst);
}

/* TODO: this update strategy is broken, and probably makes no sense.... */
int cVectorHEQ::updateTransformFloatExp(struct sTfData * tf, const FLOAT_DMEM *src, int idxi)
{
  long i;
  long N = tf->head.vecSize;
  double nVal=(double)(nFrames-1);
  
#ifdef SAVE_DEBUG_DATA
  {
    double *hist = tf->vectors+(startVec*tf->head.vecSize);
    saveDoubleVector_csv("hist0.dat",hist,numIntervals,1);
  }
#endif

  // update parent transform (mean/variance/range)
  struct sTfData * tf0 = getTransformInitData();
  if ((!fixedRange)||(tf0->head.nVec == 0)) {
    cVectorMVN::updateTransformFloatAvg(tf,src,idxi);
  }

  double *mean = tf->vectors;
  double *stddev = NULL;
  if ((stdRange)&&(tf->head.typeID == TRFTYPE_MVN_HEQ)) {
    stddev = mean+tf->head.vecSize;
    for (i=0; i<tf->head.vecSize; i++) {
      if (stddev[i] <= 0.0) stddev[i] = STDDEV_FLOOR;
    }
  }

  /*
  update strategy:
  for each data element:
  update histogram sums
  histogram is stored as normalised values (-> pdf)

  check if histogram range has changed (inkl. tolerance)... if yes, then remap histogram before updating!
  */

  for (i=0; i<N; i++) {
    
    double X = src[i];

    if (!stdRange) {
      // check for range changes
      sHEQrange newRange = computeHEQrange(tf,i);
      if (curRange[i].isInit == 0) {
        HEQrangeAssign(curRange[i], newRange); // assign new range
      } else {
        if (HEQrangeChanged(newRange,curRange[i])) { // check for change within tolerance
          remapHistogram(tf, i, curRange[i], newRange); // TODO: <-- this code still seems buggy
          HEQrangeAssign(curRange[i], newRange); // assign new range
        } /*else { 
          printf("no range change\n");
          }*/
      }
    } else {
      // we must transform the current data ourselves ... cVectorMVN cannot handle this at that point
      if (tf->head.typeID == TRFTYPE_MVN_HEQ) {
        X = (X-(FLOAT_DMEM)mean[i])/(FLOAT_DMEM)stddev[i];
      }
    }

    // update histogram (a normalised pdf in our case...)
    double *hist = tf->vectors+(startVec*tf->head.vecSize);
    //double factor = nVal/(double)nFrames;
    long j;
    double s = 0.0;
    double s1 = 0.0;
    double a = 1.0 - (1.0-alpha)/(double)numIntervals;
    for (j=0; j<numIntervals; j++) {
      s += hist[i*numIntervals + j] * (1.0-a);
      hist[i*numIntervals + j] *= a;
      s1 += hist[i*numIntervals + j];
    }
    long iv = getIntervalIndex((double)X,curRange[i]);
    //printf("iv %i  x %f  %f - %f\n",iv,X,curRange[i].start,curRange[i].mean);
    if ((iv >= 0) && (iv < numIntervals)) 
      hist[i*numIntervals + iv] += 1.0;  // h*factor + 1 / nFrames
    if (iv==-1) {
      long x;
      for (x=0; x<numIntervals; x++) {
        hist[i*numIntervals + x] = 0.0; // (1.0) / (double)numIntervals;
      }
    }

    double factor = 1.0 / (s1+1.0);
    for (j=0; j<numIntervals; j++) {
      hist[i*numIntervals + j] *= factor;
    }

#ifdef SAVE_DEBUG_DATA
    if (i==0) saveDoubleVector_csv("hist.dat",hist,numIntervals,1);
#endif
  }

  return 1;
}


void cVectorHEQ::updateRingBuffer(const FLOAT_DMEM *src, long Nsrc) 
{
  if (stdRange) {
    double *mean = NULL; 
    double *stddev = NULL;

    struct sTfData * tf = getTransformData();
    mean = tf->vectors;
    stddev = mean+tf->head.vecSize;

    long i;
    if (tf->head.typeID == TRFTYPE_MVN_HEQ) {
      stddev = mean+tf->head.vecSize;
      for (i=0; i<tf->head.vecSize; i++) {
        if (stddev[i] <= 0.0) stddev[i] = STDDEV_FLOOR;
      }
      
      if (nAvgBuffer == fixedBufferFrames) {
        for (i=0; i<Nsrc; i++) {
          // store current input in ringbuffer at position of last input:
          buffer[i + Nsrc*wPtr] = (src[i]-(FLOAT_DMEM)mean[i])/(FLOAT_DMEM)stddev[i];
        }
      } else {
        for (i=0; i<Nsrc; i++) {
          // store current input in ringbuffer:
          buffer[i + Nsrc*wPtr] = (src[i]-(FLOAT_DMEM)mean[i])/(FLOAT_DMEM)stddev[i];
        }
        nAvgBuffer++;
      }
      wPtr++;
      if (wPtr >= fixedBufferFrames) wPtr = 0;
    }

  } else {
    cVectorMVN::updateRingBuffer(src,Nsrc);
  }
}

int cVectorHEQ::updateTransformFloatBuf(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long Nbuf, long wrPtr, int idxi)
{
  long i;
  long N = tf->head.vecSize;
  double nVal=(double)(nFrames-1);
  
#ifdef SAVE_DEBUG_DATA
  {
    double *hist = tf->vectors+(startVec*tf->head.vecSize);
    saveDoubleVector_csv("hist0.dat",hist,numIntervals,1);
  }
#endif

  // update parent transform (mean/variance/range)
  struct sTfData * tf0 = getTransformInitData();
  if ((!fixedRange)||(tf0->head.nVec == 0)) {
    cVectorMVN::updateTransformFloatAvg(tf,src,idxi);
  }

  /*
    TODO: when using stdRange the output seems to be broken,
    why? is this to be expected? is it due to chaning mean/variance?
  */

  for (i=0; i<N; i++) {
    
    double X = src[i];


    /* with this update method we do not need range updating and histogram remapping, 
       since the histogram is computed from scratch every time */

    sHEQrange newRange = computeHEQrange(tf,i);
    HEQrangeAssign(curRange[i], newRange); // assign new range

    /* update histogram (a normalised pdf in our case...) */
    double *hist = tf->vectors+(startVec*tf->head.vecSize);
    // we use the *exact* method here, and always compute the histogram over the full buffer length:
    long j;
    for (j=0; j<numIntervals; j++) {
      hist[i*numIntervals + j] = 0.0; 
    }
    for (j=0; j<Nbuf; j++) {
      long iv = getIntervalIndex((double)buf[j],curRange[i]);

      if ((iv >= 0) && (iv < numIntervals)) 
        hist[i*numIntervals + iv] += 1.0 / (double)Nbuf ;  
      if (iv==-1) {
        long x;
        for (x=0; x<numIntervals; x++) {
          hist[i*numIntervals + x] = 1.0 / (double)Nbuf;
        }
      }
    }

#ifdef SAVE_DEBUG_DATA
    if (i==0) saveDoubleVector_csv("hist.dat",hist,numIntervals,1);
#endif
  }

  return 1;
}




int cVectorHEQ::updateTransformFloatAvg(struct sTfData * tf, const FLOAT_DMEM *src, int idxi)
{
  long i;
  long N = tf->head.vecSize;
  double nVal=(double)(nFrames-1);
  
#ifdef SAVE_DEBUG_DATA
  {
    double *hist = tf->vectors+(startVec*tf->head.vecSize);
    saveDoubleVector_csv("hist0.dat",hist,numIntervals,1);
  }
#endif

  // update parent transform (mean/variance/range)
  struct sTfData * tf0 = getTransformInitData();
  if ((!fixedRange)||(tf0->head.nVec == 0)) {
    cVectorMVN::updateTransformFloatAvg(tf,src,idxi);
  }

  double *mean = tf->vectors;
  double *stddev = NULL;
  if ((stdRange)&&(tf->head.typeID == TRFTYPE_MVN_HEQ)) {
    stddev = mean+tf->head.vecSize;
    for (i=0; i<tf->head.vecSize; i++) {
      if (stddev[i] <= 0.0) stddev[i] = STDDEV_FLOOR;
    }
  }

  /*
  update strategy:
  for each data element:
  update histogram sums
  histogram is stored as normalised values (-> pdf)

  check if histogram range has changed (inkl. tolerance)... if yes, then remap histogram before updating!
  */

  for (i=0; i<N; i++) {
    
    double X = src[i];

    if (!stdRange) {
      // check for range changes
      if (curRange[i].isInit == 0) {
        sHEQrange newRange = computeHEQrange(tf,i);
        HEQrangeAssign(curRange[i], newRange); // assign new range
      } else if (!fixedRange) {
        sHEQrange newRange = computeHEQrange(tf,i);
        if (HEQrangeChanged(newRange,curRange[i])) { // check for change within tolerance
          remapHistogram(tf, i, curRange[i], newRange); // TODO: <-- this code still seems buggy
          HEQrangeAssign(curRange[i], newRange); // assign new range
        } /*else { 
          printf("no range change\n");
          }*/
      }
    } else {
      // we must transform the current data ourselves ... cVectorMVN cannot handle this at that point
      if (tf->head.typeID == TRFTYPE_MVN_HEQ) {
        X = (X-(FLOAT_DMEM)mean[i])/(FLOAT_DMEM)stddev[i];
      }
    }

    // update histogram (a normalised pdf in our case...)
    double *hist = tf->vectors+(startVec*tf->head.vecSize);
    double factor = nVal/(double)nFrames;
    long j;
    for (j=0; j<numIntervals; j++) {
      hist[i*numIntervals + j] *= factor;
    }
    long iv = getIntervalIndex((double)X,curRange[i]);
    //printf("iv %i  x %f  %f - %f\n",iv,X,curRange[i].start,curRange[i].mean);
    if ((iv >= 0) && (iv < numIntervals)) 
      hist[i*numIntervals + iv] += 1.0 / (double)nFrames ;  // h*factor + 1 / nFrames
    if (iv==-1) {
      long x;
      for (x=0; x<numIntervals; x++) {
        hist[i*numIntervals + x] = 1.0 / (double)nFrames;
      }
    }

#ifdef SAVE_DEBUG_DATA
    if (i==0) saveDoubleVector_csv("hist.dat",hist,numIntervals,1);
#endif
  }

  return 1;
}

/*
TODO: disable mvn update, enable heq update OR possibility of loading external MVN data directly into HEQ (as range)
*/

int cVectorHEQ::updateTransformFloatAvgI(struct sTfData * tf, const FLOAT_DMEM *src, FLOAT_DMEM *buf, long * _bufferNframes, long Nbuf, long wrPtr, int idxi)
{
  return updateTransformFloatAvg(tf,src,idxi);

  //return 1;
}

cVectorHEQ::~cVectorHEQ()
{
  if (gauss != NULL) free(gauss);
  if (lut != NULL) free(lut);
  if (curRange != NULL) free(curRange);
  if (cumhist != NULL) free(cumhist);
}

