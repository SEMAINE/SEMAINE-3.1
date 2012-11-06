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

Input: candidates produced by a pitchBase descendant

*/


#ifndef __CFORMANTSMOOTHER_HPP
#define __CFORMANTSMOOTHER_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CFORMANTSMOOTHER "This component performs temporal formant smoothing. Input: candidates produced by a formant** component AND(!) - appended - an F0final or voicing field (which is 0 for unvoiced frames and non-zero for voiced frames). Output: Smoothed formant frequency contours."
#define COMPONENT_NAME_CFORMANTSMOOTHER "cFormantSmoother"

#define FPOSTSMOOTHING_NONE   0
#define FPOSTSMOOTHING_SIMPLE 1
#define FPOSTSMOOTHING_MEDIAN 2

#undef class
class DLLEXPORT cFormantSmoother : public cVectorProcessor {
  private:
    int firstFrame;
    int no0f0;
    int medianFilter0;
    int postSmoothing, postSmoothingMethod;
    int saveEnvs;

    long F0fieldIdx, formantFreqFieldIdx, formantBandwidthFieldIdx, formantFrameIntensField;
    long nFormantsIn;

    int nFormants, bandwidths, formants, intensity;
    //int onsFlag, onsFlagO;
    //int octaveCorrection;

    FLOAT_DMEM *median0WorkspaceF0cand;
    FLOAT_DMEM *lastFinal;

    FLOAT_DMEM *fbin;
    FLOAT_DMEM *fbinLastVoiced;
    // NOTE: difficulty when analysing data from multiple pdas : 
    // the scores and voicing probs. may be scaled differently, although they should all be in the range 0-1
    // thus, a direct comparion of these may not be feasible
    // We thus start comparison for each field (pda output) individually and then (TODO) merge information from multiple pdas

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    FLOAT_DMEM *voicingCutoff;

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFormantSmoother(const char *_name);
    
    virtual ~cFormantSmoother();
};




#endif // __CFORMANTSMOOTHER_HPP
