/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * This module has been created by Benedikt Gollan for ADMIRE
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

example vectorProcessor descendant

*/

#undef class

#include <string>
#include <math.h>


#ifndef __CHROMAPERFRAME_HPP
#define __CHROMAPERFRAME_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CCHROMAPERFRAME "Chroma-based features per frame."
#define COMPONENT_NAME_CCHROMAPERFRAME "cChromaPerFrame"

class cChromaPerFrame : public cVectorProcessor {
  private:
	  void print(int,int);
	  void update(int);


  FLOAT_DMEM *dstcopy;

  double BeatPos [2000][5];
  double sum;
  double key[12];
  double tone[12];
  double tone_per_frame[12];
  int m,p,indexkey,indexcount;
  int indexkeycadence;
  int Tracknumber;
  double chordmaj[12];
  double chordmin[12];
  int n,loopnum,l,Chord_Key;
  int curbar;
  long playedSamples;
  long processedsamples;
  int framedone;
  double getnextFramelength();
  double framelength;
  FILE * filehandle;
  double frameSize,frameStep;
  int count[4];
  int svm_idx;
  int HTM_SVM;
   char chord_svm_ch[100];
  char *chord_svm_p;
  int lookupindex(char*);
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    //virtual int myTick(long long t);
	virtual int processComponentMessage( cComponentMessage *_msg );

  //  virtual int configureWriter(const sDmLevelConfig *c);

    virtual void configureField(int idxi, long __N, long nOut);
    virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cChromaPerFrame(const char *_name);

    virtual ~cChromaPerFrame();
};




#endif // __EXAMPLEVECTORPROCESSOR_HPP
