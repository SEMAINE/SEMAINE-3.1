/*
 * featum silence/turn detector
 * first , simple, energy based version
 * (C) 2008 by Florian Eyben, TUM. All rights reserved.
 *
 */

#include "featum_common.h"

#include <stdio.h>
#include "silence-detector.hpp"

void cSilenceDetector::objZero() {
  run_max = 0;                                   
  run_min = 0;
  run_mean_sum = 0;
  run_mean_n = 0;
  silCnt = 0; actCnt = 0;
  status = 0;
}

cSilenceDetector::cSilenceDetector(cLLDex &_lldex) : lldex(_lldex) {
  objZero();
  silCntThresh = 30;
  actCntThresh = 10;
  thresh = 0.005;
}

cSilenceDetector::cSilenceDetector(cLLDex &_lldex, float _thresh) : lldex(_lldex), thresh(_thresh) {
  objZero();
  silCntThresh = 30;
  actCntThresh = 10;
}

int cSilenceDetector::_process(float energy) {
  int retval=0;                                 

  if (energy > run_max) run_max = energy;
  if (energy < run_min) run_min = energy;
  run_mean_sum += energy;
  run_mean_n++;
  run_mean = run_mean_sum / run_mean_n;
    
  if (status) {  // actitivity
    if (energy < thresh) {
      silCnt++;       
    } else {
//      silCnt = 0;
      silCnt -= 1; if(silCnt <0) silCnt = 0;  // experimental code
    }
    if (silCnt > silCntThresh) {
      status = 0;
      retval = 1;  // silence begin
      actCnt = 0;        
    }
  } else {
    if (energy > thresh) {
      actCnt++;
    } else {
     // actCnt = 0;       
      actCnt -= 1; if(actCnt <0) actCnt = 0;  // experimental code
    }
    if (actCnt > actCntThresh) {
      status = 1;
      retval = 2;  // turn begin
      silCnt = 0;           
    }
  }
  
  return retval; 
}


int cSilenceDetector::process(float energy) {
  return _process(energy);               
}

int cSilenceDetector::process(cPcmBuffer &buf) {
  float energy = 0.0;
  energy = buf.rmsEnergy();
  return _process(energy);               
}

int cSilenceDetector::process(int id) {
  if ((lldex._data.current != NULL)&&(lldex._data.current[id]->energy != NULL)) {
    int retval;
    retval =  _process(lldex._data.current[id]->energy->rmsEnergy);
    //printf("  ## ret: %i  , energy: %f\n",retval,lldex._data.current[id]->energy->rmsEnergy );
    return retval;
  } else {
    return 0;       
  }
} 


cSilenceDetector::~cSilenceDetector() { }
                                      
 
