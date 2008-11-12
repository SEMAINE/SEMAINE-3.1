

#ifndef __WAVE_COMMON_H
#define __WAVE_COMMON_H


#define BYTEORDER_LE 0
#define BYTEORDER_BE 1

typedef struct {
  long sampleRate;
  int sampleType;
  int nChan;
  int blockSize;
  int nBPS;       // bytes per sample 
  int byteOrder;  // BYTEORDER_LE or BYTEORDER_BE
  int memOrga;    // MEMORGA_INTERLV  or MEMORGA_SEPCH
  LONG_IDX nBlocks;  // nBlocks in buffer
} sWaveParameters;
typedef sWaveParameters* pWaveParameters;


#endif  // __WAVE_COMMON_H  
