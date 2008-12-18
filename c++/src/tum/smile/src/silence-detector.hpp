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

#ifndef __SILENCE_DETECTOR_HPP
#define __SILENCE_DETECTOR_HPP


#include "LLDex.hpp"
#include "pcm-data.hpp"

class cSilenceDetector {
  
  private:
      float run_max;
      float run_min;
      double run_mean_sum;
      double run_mean_n;
      double run_mean;
      
      int silCnt; int silCntThresh;
      int actCnt; int actCntThresh;
      
      float thresh;
      int status;    // 0=silence, 1=activity
  
      cLLDex &lldex;
      
      void objZero();
      int _process(float energy);
          
  
  public:
      cSilenceDetector(cLLDex &_lldex);
      cSilenceDetector(cLLDex &_lldex, float _thresh);
            
      int process(int id);  // use energy from lldex struct
      int process(cPcmBuffer &buf);  // compute energy from pcm buffer
      int process(float energy);   // energy value is given directly
      
      ~cSilenceDetector();
      
};

                                     
#endif //__SILENCE_DETECTOR_HPP
