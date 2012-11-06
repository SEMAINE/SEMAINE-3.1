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


/*  openSMILE component:

compute jitter from waveform and F0 estimate

This component has 2 readers (!)

*/


#include <pitchJitter.hpp>

#define MODULE "cPitchJitter"

SMILECOMPONENT_STATICS(cPitchJitter)

SMILECOMPONENT_REGCOMP(cPitchJitter)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CPITCHJITTER;
  sdescription = COMPONENT_DESCRIPTION_CPITCHJITTER;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")
  if (ct->setField("F0reader", "Configuration of the dataMemory reader sub-component which is used to read the F0 estimate from a pitch component output (e.g. cPitchShs).",
                  sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }
/*
if (ct->setField("exWriter", "dataMemory writer for pcm voice excitation signal",
                  sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE) == -1) {
     rA=1; // if subtype not yet found, request , re-register in the next iteration
  }
  */
  SMILECOMPONENT_IFNOTREGAGAIN(
    //ct->setField("F0dmLevel","data memory level to read f0 estimate from","pitch");
    ct->setField("F0field","The name of the field in 'F0reader.dmLevel' containing the F0 estimate (in Hz) (usually F0final or F0raw)","F0final");
    ct->setField("searchRangeRel","The relative search range for period deviations (Jitter): maxT0, minT0 = (1.0 +/- searchRangeRel)*T0",0.25);
    ct->setField("jitterLocal","1 = enable computation of F0 jitter (period length variations). jitterLocal = the average absolute difference between consecutive periods, divided by the average period length of all periods in the frame",0);
    ct->setField("jitterDDP","1 = enable computation of F0 jitter (period length variations). jitterDDP = the average absolute difference between consecutive differences between consecutive periods, divided by the average period length of all periods in the frame",0);
    ct->setField("jitterLocalEnv","1 = compute envelope of jitterLocal (i.e. fill jitter values in unvoiced frames with value of last voiced segment). Use this in conjunction with statistical functionals such as means.",0);
    ct->setField("jitterDDPEnv","1 = compute envelope of jitterDDP (i.e. fill jitter values in unvoiced frames with value of last voiced segment). Use this in conjunction with statistical functionals such as means.",0);
    
    ct->setField("shimmerLocal","1 = enable computation of F0 shimmer (amplitude variations). shimmerLocal = the average absolute difference between the interpolated peak amplitudes of consecutive periods, divided by the average peak amplitude of all periods in the frame",0);
    ct->setField("shimmerLocalEnv","1 = compute envelope of shimmerLocal (i.e. fill shimmer values in unvoiced frames with value of last voiced segment). Use this in conjunction with statistical functionals such as means.",0);

    // TODO: this option is not fully supported by the functionals component, for example.
    // winToVecProcessor cannot deal with variable frame rate, i.e. does not yet support collecting frames by looking at tmeta fields...
    ct->setField("onlyVoiced","1 = produce output only for voiced frames. I.e. do not output 0 jitter/shimmer values for unvoiced frames. WARNING: this option is not fully supported by the functionals component, yet.",0);

    ct->setField("periodLengths","1 = enable output of individual period lengths",0);
    ct->setField("periodStarts","1 = enable output of individual period start times",0);
  )
  
  SMILECOMPONENT_MAKEINFO(cPitchJitter);
}


SMILECOMPONENT_CREATE(cPitchJitter)

//-----

cPitchJitter::cPitchJitter(const char *_name) :
  cDataProcessor(_name), minF0(50.0), // <- TODO: read this from pitch field metadata
    lastIdx(0), lastMis(0), out(NULL), F0reader(NULL), lastT0(0.0), lastDiff(0.0),
    Nout(0), lastJitterLocal(0.0), lastJitterDDP(0.0), lastShimmerLocal(0.0),
    lastJitterLocal_b(0.0), lastJitterDDP_b(0.0), lastShimmerLocal_b(0.0)
{
  char *tmp = myvprint("%s.F0reader",getInstName());
  F0reader = (cDataReader *)(cDataReader::create(tmp));
  if (F0reader == NULL) {
    COMP_ERR("Error creating dataReader '%s'",tmp);
  }
  if (tmp!=NULL) free(tmp);
}

void cPitchJitter::fetchConfig()
{
  cDataProcessor::fetchConfig();

  // load all configuration parameters you will later require fast and easy access to here:

  F0field = getStr("F0field");
  SMILE_IDBG(2,"F0field = '%s'",F0field);
  
  searchRangeRel = getDouble("searchRangeRel");
  SMILE_IDBG(2,"searchRangeRel = %f",searchRangeRel);

  onlyVoiced = getInt("onlyVoiced");
  SMILE_IDBG(2,"onlyVoiced = %i",onlyVoiced);

  jitterLocal = getInt("jitterLocal");
  SMILE_IDBG(2,"jitterLocal = %i",jitterLocal);
  jitterDDP = getInt("jitterDDP");
  SMILE_IDBG(2,"jitterDDP = %i",jitterDDP);

  jitterLocalEnv = getInt("jitterLocalEnv");
  SMILE_IDBG(2,"jitterLocalEnv = %i",jitterLocalEnv);
  jitterDDPEnv = getInt("jitterDDPEnv");
  SMILE_IDBG(2,"jitterDDPEnv = %i",jitterDDPEnv);

  shimmerLocal = getInt("shimmerLocal");
  SMILE_IDBG(2,"shimmerLocal = %i",shimmerLocal);
  shimmerLocalEnv = getInt("shimmerLocalEnv");
  SMILE_IDBG(2,"shimmerLocalEnv = %i",shimmerLocalEnv);

}




int cPitchJitter::configureReader() 
{
  // check if F0 level period and pcm input period match
  double Tf0 = F0reader->getLevelT();
  double T = reader->getLevelT();
  if (T == Tf0) {
    SMILE_IERR(1,"pcm level frame period must be << F0 level frame period! pcm data should be stream data and not frame data!");
  }
  if (T == 0.0) {
    SMILE_IERR(1,"pcm input level period is 0! Cannot compute sample rate!");
    T=1.0;
  }

  // set blocksize to 2*max pitch period (2.0/minF0)
  //blocksizeR = (long)ceil(2.0/(minF0*T));
  blocksizeR = (long)ceil(5.0/(minF0*T));
  int ret = cDataProcessor::configureReader();
  F0reader->setBlocksize(1);
  return ret;
}

void cPitchJitter::mySetEnvironment()
{
  cDataProcessor::mySetEnvironment();
  F0reader->setComponentEnvironment(getCompMan(), -1, this);
}

int cPitchJitter::myRegisterInstance(int *runMe)
{
  int ret = cDataProcessor::myRegisterInstance(runMe);
  ret *= F0reader->registerInstance();
  return ret;
}

int cPitchJitter::myConfigureInstance()
{
  if (!(F0reader->configureInstance())) return 0;
  if (!(F0reader->finaliseInstance())) return 0;

  int ret = cDataProcessor::myConfigureInstance();
  return ret;
}

int cPitchJitter::configureWriter(sDmLevelConfig &c) 
{
  const sDmLevelConfig *cfg = F0reader->getLevelConfig();
  c.T = cfg->T;
  c.frameSizeSec = cfg->frameSizeSec;
  c.basePeriod = cfg->basePeriod;
  c.growDyn = cfg->growDyn;
  c.isRb = cfg->isRb;
  c.lastFrameSizeSec = cfg->frameSizeSec;
  c.lenSec = cfg->lenSec;
  return 1; /* success */
}

/* You shouldn't need to touch this....
int cPitchJitter::myConfigureInstance()
{
  int ret = cDataProcessor::myConfigureInstance();
  return ret;
}
*/

/*
  Do what you like here... this is called after the input names and number of input elements have become available, 
  so you may use them here.
*/
/*
int cPitchJitter::dataProcessorCustomFinalise()
{
  
  return 1;
}
*/


/* 
  Use setupNewNames() to freely set the data elements and their names in the output level
  The input names are available at this point, you can get them via reader->getFrameMeta()
  Please set "namesAreSet" to 1, when you do set names
*/

int cPitchJitter::setupNewNames(long nEl) 
{
  // find pitch input field:
  const FrameMetaInfo * fmeta = F0reader->getFrameMetaInfo();
  int ri=0;
  long idx = fmeta->findFieldByPartialName( F0field , &ri );
  if (nEl <= 0) nEl = reader->getLevelN();
  if (idx < 0) {
    F0fieldIdx = 0;
    SMILE_IWRN(2,"Requested input field '*%s*' not found, defaulting to use 0th field!",F0field);
  } else {
    F0fieldIdx = fmeta->fieldToElementIdx( idx ) + ri;
  }
  
  int n=0;
  if (jitterLocal) { writer->addField("jitterLocal",1); n++; }
  if (jitterDDP) { writer->addField("jitterDDP",1); n++; }
  if (jitterLocalEnv) { writer->addField("jitterLocEnv",1); n++; }
  if (jitterDDPEnv) { writer->addField("jitterDEnv",1); n++; }
  if (shimmerLocal) { writer->addField("shimmerLocal",1); n++; }
  if (shimmerLocalEnv) { writer->addField("shimmerLocEnv",1); n++; }
  namesAreSet = 1;
  Nout = n;
  return n;
}

/*
  If you don't use setupNewNames() you may set the names for each input field by overwriting the following method:
*/
/*
int cPitchJitter::setupNamesForField( TODO )
{
  // DOC TODO...
}
*/

int cPitchJitter::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();
  if (ret) {
    // do all custom init stuff here... 
    // e.g. allocating and initialising memory (which is not used before, e.g. in setupNames, etc.),
    // loading external data, 
    // etc.

    // ...

  }
  return ret;
}

double cPitchJitter::crossCorr(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny)
{
  long N = MIN(Nx,Ny);
  long i;
  double cc = 0.0;
  double mx = 0.0;
  double my = 0.0;
  double nx = 0;
  double ny = 0;
  for (i=0; i<N; i++) {
    mx += x[i];
    my += y[i];
  }
  mx /= (double)N;
  my /= (double)N;
  for (i=0; i<N; i++) {
    cc += (x[i]-mx)*(y[i]-my);
    nx += (x[i]-mx)*(x[i]-mx);
    ny += (y[i]-mx)*(y[i]-mx);
  }
  cc /= sqrt(nx)*sqrt(ny);
  //cc /= (double)N;
  return cc;
}

// get difference of peak amplitude in the two given frames
// save interpolated index of max peak in first frame in *maxI0 if != NULL
// save interpolated index of max peak in second frame in *maxI1 if != NULL
FLOAT_DMEM cPitchJitter::amplitudeDiff(FLOAT_DMEM *x, long Nx, FLOAT_DMEM *y, long Ny, double *maxI0, double *maxI1, FLOAT_DMEM *_A0, FLOAT_DMEM *_A1)
{
  long i;
  long N = MIN(Nx,Ny);
  double A0=1.0, A1=1.0;
  // analyse first frame:
  long mI=1; FLOAT_DMEM max=x[1];
  for (i=1; i<Nx-1; i++) {
    if (x[i] > max) { max=x[i]; mI = i; }
  }
  double mi = smileMath_quadFrom3pts((double)(mI-1),x[mI-1],(double)(mI),x[mI],(double)(mI+1),x[mI+1],&A0,NULL);
  //printf("A0 %f  x[%i] %f  Nx=%i\n",A0,mI,x[mI],Nx);
  if (maxI0 != NULL) *maxI0 = mi;
  // analyse second frame:
  mI=1; max=y[1];
  for (i=1; i<Ny-1; i++) {
    if (y[i] > max) { max=y[i]; mI = i; }
  }
  mi = smileMath_quadFrom3pts((double)(mI-1),y[mI-1],(double)(mI),y[mI],(double)(mI+1),y[mI+1],&A1,NULL);
  if (maxI1 != NULL) *maxI1 = mi;
  //printf("A1 %f  x[%i] %f  Ny=%i\n",A1,mI,y[mI],Ny);
  
  // save amplitudes
  if (_A0 != NULL) *_A0 = (FLOAT_DMEM)A0;
  if (_A1 != NULL) *_A1 = (FLOAT_DMEM)A1;
  
  // compute relative amplitude difference:
  return (FLOAT_DMEM)fabs(A0-A1);
}

int cPitchJitter::myTick(long long t)
{
  /* actually process data... */

  SMILE_IDBG(4,"tick # %i, processing value vector",t);

  if (isEOI()) return 0; // TODO: check if we need to flush jitter data OR we can still read from input....?

  long i;

  // get next pitch frame:
  cVector *fvec = F0reader->getNextFrame();
  if (fvec != NULL) {

    FLOAT_DMEM F0 = 0.0;
    if (F0fieldIdx < fvec->N) F0 = fvec->dataF[F0fieldIdx];
    long lenF = fvec->tmeta->lengthFrames;

    double T = reader->getLevelT(); // sample period
    long startVidx = (long)round(fvec->tmeta->time / T);
    double pitchT = fvec->tmeta->period; // pitch frame period

    // frame overlap (in samples):
    long ppLen = (long)ceil(pitchT/T);
    long ovl = lenF - ppLen;

    // maxmimum number of frames we may read...
    long toRead = ppLen+lastMis; /*lenF*/
    long maxRead = lastMis+lenF;
    if (maxRead < toRead) toRead = maxRead;

    // check start idx:
    if (startVidx - lastMis != lastIdx) {
      SMILE_IWRN(3,"discontinuity in input indicies... %i != %i  (v: %i, lastm: %i, tr %i, mr %i)\n",startVidx-lastMis,lastIdx,startVidx,lastMis,toRead,maxRead);
      lastIdx = startVidx; 
    //  toRead = lenF;
      // resync...
    }
    
    // get pcm data
    cMatrix *mat = reader->getMatrix(lastIdx,maxRead);

    if (mat == NULL) {
      //lastIdx += maxRead;
      return 0;
    }

    // jitter computation variables:
    FLOAT_DMEM nPeriodsLocal = 0;
    FLOAT_DMEM nPeriodsDDP = 0;
    FLOAT_DMEM nPeriods = 0;
    FLOAT_DMEM avgPeriod = 0.0;
    FLOAT_DMEM JitterDDP=0.0;// Praat: ddp
    FLOAT_DMEM JitterLocal=0.0;
    FLOAT_DMEM JitterPPQ=0.0;
    // shimmer computation variables
    FLOAT_DMEM avgAmp=0.0;
    FLOAT_DMEM avgAmpDiff=0.0;

    
    // start the waveform matching for voiced segements
    long start = 0;
    if (F0 > 0.0) { // voiced frame

      double T0 = 1.0/F0; // pitch period
      double Tf = T0/T; // f0 period in frames (samples)
      long T0f = (long)round(Tf); // .. as integer

      // upper / lower T0 bounds:
      double T0min = (1.0-searchRangeRel)*T0;
      double T0max = (1.0+searchRangeRel)*T0;
      long T0minF = (long)floor(T0min/T);
      long T0maxF = (long)ceil(T0max/T);

      // waveform matching  (TODO: sync to amplitude max.)
      double *cc = (double*)calloc(1,sizeof(double)*(T0maxF-T0minF+1));

      long os = start;
      //printf("read N %i  2*T0maxF %i\n",mat->nT,2*T0maxF);
      // TODO: multi-channel (mat->N > 1) support!
      while (( start< mat->nT - 2*T0maxF ) && (start < toRead)) {
        long _Tf;
        for (_Tf = T0minF; _Tf <= T0maxF; _Tf++) {
          long mid = start+_Tf;
          long end = start+2*_Tf;
          //if (end < vec->nT) {
          cc[_Tf-T0minF] = crossCorr(mat->dataF+start,_Tf,mat->dataF+mid,_Tf);
          //} 
        }

        // peak picking
        double max=cc[T0f-T0minF]; long maxI=-1;
        //printf("T0maxF %i T0minF %i T0 %f, F0 %f, T0min %f , T0max %f T %f\n",T0maxF,T0minF,T0,F0,T0min,T0max,T);
        for (i=1; i<T0maxF-T0minF-1; i++) {
          //printf("cc[%i] = %f\n",i,cc[i]);
          if ( (cc[i-1] < cc[i]) && (cc[i] > cc[i+1]) ) { // peak
            if (maxI == -1) {
              maxI = i; max = cc[i];
            } else {
              if (cc[i] > max) {
                maxI = i; max = cc[i];
              }
            }
          }
        }

        // assign new start
        os = start;
        long pp = T0minF+maxI;
        
        // get amplitude differences for shimmer computation:
        

        // compute Jitter and Shimmer
        if (maxI >= 0) {
          //printf("jitter: %i  (maxI: %i)\n",pp-T0f,maxI);
          start += pp;

          // get amplitude differences for shimmer computation:        
          double max0 = 0;
          FLOAT_DMEM _a0=0;
          FLOAT_DMEM ad=0; 
          ad = amplitudeDiff(mat->dataF+os,pp,mat->dataF+start,pp,&max0,NULL,&_a0,NULL);

          // parabolic interpolation of maxI
          double conf = 0.0;
          double ccI = 0.0;
          double maxId = fabs(  ( (double)T0minF + smileMath_quadFrom3pts((double)(maxI-1),cc[maxI-1],(double)(maxI),cc[maxI],(double)(maxI+1),cc[maxI+1],&ccI,&conf) ) ) * T;
          
          if (ccI > 0.5) { // detect valid period only if CC > 0.8 ! TODO: make this configurable

          avgPeriod += (FLOAT_DMEM)maxId;
          nPeriods += 1.0;

          if (lastT0 > 0.0) {
            FLOAT_DMEM diff = (FLOAT_DMEM)fabs(lastT0-maxId);
            JitterLocal += diff; 
            nPeriodsLocal += 1.0;
            if (lastDiff > 0.0) {
              JitterDDP += fabs(lastDiff-diff);
              nPeriodsDDP += 1.0;
            }
            lastDiff = diff;
          }
          lastT0 = (FLOAT_DMEM)maxId;

          // debug output (TODO: save wave data properly to a pcm level, and save period start/end times..
          //saveFloatDmemVectorWlen_bin("dataJ.dat",mat->dataF+os,start-os,1);  

          // Shimmer:
          avgAmp += _a0;
          avgAmpDiff += ad;

          }
        } else {
          start += T0f;
          //printf("no match\n");
        }
        //if (start < toRead) {
          
        //  printf("W: %i - %i  (%i)\n",lastIdx+os,lastIdx+start,start-os);
        //}

      }
      //if (start > toRead) start = os; // <-- should never happen..?
      lastMis = toRead - start;

      free(cc);

    } else { // for unvoiced frames:
      start = toRead;
      lastMis = 0;
      // reset memory:
      lastT0 = 0.0; lastDiff=0.0;
      lastJitterDDP = 0.0; lastJitterLocal = 0.0;
      lastShimmerLocal = 0.0;
      //saveFloatDmemVectorWlen_bin("dataJ.dat",mat->dataF,toRead,1);  
    }
    
    lastIdx += start;

    // build output vector:
    if (Nout == 0) return 0;

    if (onlyVoiced && (F0==0.0)) { return 0; }

    long n=0;
    if (out == NULL) out = new cVector(Nout,DMEM_FLOAT);
    if (out == NULL) OUT_OF_MEMORY;

    
    if ((nPeriods>0.0)&&(nPeriodsLocal > 0.0)&&(F0>0.0)) {
      JitterLocal /= nPeriodsLocal;
      lastJitterLocal_b = lastJitterLocal = JitterLocal / (avgPeriod/nPeriods);
    }
    if (jitterLocal) {
      if ((nPeriods>0.0)&&(nPeriodsLocal > 0.0)&&(F0>0.0)) {
        //JitterLocal /= nPeriodsLocal;
        out->dataF[n] = lastJitterLocal; // = JitterLocal / (avgPeriod/nPeriods);
      } else {
        if ((nPeriods == 0.0)&&(F0>0.0)) {
          out->dataF[n] = lastJitterLocal;
        } else {
          out->dataF[n] = 0.0;
        }
      }
      n++;
    }
    if (jitterLocalEnv) {
      out->dataF[n] = lastJitterLocal_b;
      n++;
    }

    if ((nPeriods>0.0)&&(nPeriodsDDP > 0.0)&&(F0>0.0)) {
      JitterDDP /= nPeriodsDDP;
      lastJitterDDP_b = lastJitterDDP = JitterDDP / (avgPeriod/nPeriods);
    }
    if (jitterDDP) {
      if ((nPeriods>0.0)&&(nPeriodsDDP > 0.0)&&(F0>0.0)) {
        //JitterDDP /= nPeriodsDDP;
        out->dataF[n] = lastJitterDDP;
      } else {
        if ((nPeriods == 0.0)&&(F0>0.0)) {
          out->dataF[n] = lastJitterDDP;
        } else {
          out->dataF[n] = 0.0;
        }
      }
      n++;
    }
    if (jitterDDPEnv) {
      out->dataF[n] = lastJitterDDP_b;
      n++;
    }
    
    if ((nPeriods>0.0)&&(F0>0.0)) {
      if (avgAmp > 0.0) {
        lastShimmerLocal_b = lastShimmerLocal = (avgAmpDiff/avgAmp);
      } else {
        lastShimmerLocal=0.0;
      }
    }
    if (shimmerLocal) { // shimmer local
      if ((nPeriods>0.0)&&(F0>0.0)) {
        out->dataF[n++] = lastShimmerLocal;
      } else {
        if ((nPeriods == 0.0)&&(F0>0.0)) {
          out->dataF[n] = lastShimmerLocal;
        } else {
          out->dataF[n] = 0.0;
        }
      }
      n++;
    }
    if (shimmerLocalEnv) {
      out->dataF[n] = lastShimmerLocal_b;
      n++;
    }


    out->tmetaReplace(fvec->tmeta);
    writer->setNextFrame(out);

    return 1;
  }


////////////////////////////////////

#if 0
    double startVidx = fvec->tmeta->time / fvec->tmeta->period;
    
    double pitchT = fvec->tmeta->period; // pitch frame period
    double T = reader->getLevelT(); // sample period

    double T0 = 1.0/F0; // pitch period

    long toRead = (long)ceil((pitchT+T0)/T)+lastMis;
    //double len = fvec->tmeta->lengthSec;

    double Tf = T0/T; // f0 period in frames (samples)
    long T0f = (long)round(Tf);

    // upper / lower T0 bounds:
    double T0min = (1.0-searchRangeRel)*T0;
    double T0max = (1.0+searchRangeRel)*T0;
    long T0minF = (long)floor(T0min/T);
    long T0maxF = (long)ceil(T0max/T);

    // maxmimum number of frames we may read...
    long maxAvail = lenF+lastMis;

    // get pcm data
    long nRead = maxAvail; // MIN(maxAvail,toRead);
    cMatrix *mat = reader->getMatrix(lastIdx,nRead);

    lastIdx += nRead;
    if (mat == NULL) { 
      // frame not available..? If F0 frame was available this can only mean we have to catch up...
      
      // TODO : write NULL jitter/shimmer frame for proper sync?
      //...
      printf("Not avail\n");
      return 0; 
    }

    if (F0 > 0.0) {
      // waveform matching  (TODO: sync to amplitude max.)
      double *cc = (double*)calloc(1,sizeof(double)*(T0maxF-T0minF+1));

      long start = 0;
      //printf("read N %i  2*T0maxF %i\n",mat->nT,2*T0maxF);
      // TODO: multi-channel (mat->N > 1) support!
      while (start<mat->nT - 2*T0maxF) {
        long _Tf;
        for (_Tf = T0minF; _Tf <= T0maxF; _Tf++) {
          long mid = start+_Tf;
          long end = start+2*_Tf;
          //if (end < vec->nT) {
          cc[_Tf-T0minF] = crossCorr(mat->dataF+start,_Tf,mat->dataF+mid,_Tf);
          //} 
        }

        // peak picking and (TODO: parabolic interpolation)
        double max=cc[T0f-T0minF]; long maxI=-1;
        //printf("T0maxF %i T0minF %i T0 %f, F0 %f, T0min %f , T0max %f T %f\n",T0maxF,T0minF,T0,F0,T0min,T0max,T);
        for (i=1; i<T0maxF-T0minF-1; i++) {
          printf("cc[%i] = %f\n",i,cc[i]);
          if ( (cc[i-1] < cc[i]) && (cc[i] > cc[i+1]) ) { // peak
            if (maxI == -1) {
              maxI = i; max = cc[i];
            } else {
              if (cc[i] > max) {
                maxI = i; max = cc[i];
              }
            }
          }
        }
        // TODO: parabolic interpolation of maxI for jitter comp. ??

        // assign new start
        long os = start;
        long pp = T0minF+maxI;
        if (maxI >= 0) {
          printf("jitter: %i  (maxI: %i)\n",pp-T0f,maxI);
          start += pp;
        } else {
          start += T0f;
          printf("no match\n");
        }
        saveFloatDmemVectorWlen_bin("dataJ.dat",mat->dataF+os,start-os,1);  


        // save period start...
      }



      free(cc);

      lastMis = nRead - start;

    } else {
      // compute lastMis (i.e.: maxAvail - lastPeriodEnd)
      lastMis = 0;
      //lastIdx += start;
    }
    
     // zero jitter/shimmer for unvoiced frames ?
  // lastIdx += lenF;
  // printf("unvoiced \n");
  //}
    
  // build output vector

  // save output
    //writer->setNextFrame(vecO);
    return 1;  
  } 
#endif
  return 0;

}


cPitchJitter::~cPitchJitter()
{
  // cleanup...
  if (out != NULL) delete out;
  if (F0reader != NULL) delete F0reader;
}

