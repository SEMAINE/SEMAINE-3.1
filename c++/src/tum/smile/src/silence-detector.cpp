/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/


/*
 * silence/turn detector
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
                                      
 
