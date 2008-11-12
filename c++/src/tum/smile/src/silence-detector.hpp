
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
