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


#ifndef __CPITCHSMOOTHER_HPP
#define __CPITCHSMOOTHER_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CPITCHSMOOTHER "This component performs temporal pitch smoothing. Input: candidates produced by a pitchBase descendant (e.g. cPitchSHS). The voicing cutoff threshold is inherited from the input component, thus this smoother component does not provide its own threshold option."
#define COMPONENT_NAME_CPITCHSMOOTHER "cPitchSmoother"

#define POSTSMOOTHING_NONE   0
#define POSTSMOOTHING_SIMPLE 1
#define POSTSMOOTHING_MEDIAN 2

#undef class
class DLLEXPORT cPitchSmoother : public cVectorProcessor {
  private:
    int firstFrame;
    int no0f0;
    int medianFilter0;
    int postSmoothing, postSmoothingMethod;
    int onsFlag, onsFlagO;
    int octaveCorrection;

    int F0final, F0finalEnv, voicingFinalClipped, voicingFinalUnclipped;
    int F0raw, voicingC1, voicingClip;

    int nInputLevels; // number of input fields called F0cand (= number of input pdas (algos))
    int voicing, scores; // are candVoicing and candScores fields also present? (1/0 flag)
    
    FLOAT_DMEM lastVoice;
    FLOAT_DMEM pitchEnv;

    // per frame data:
    int *nCands; // array holding number of candidates for each pda (current)
    // global:
    int totalCands;
    int *nCandidates; // array holding max. number of candidates for each pda; array of size nInputLevels;
    FLOAT_DMEM *f0cand, *candVoice, *candScore;

    // index lookup:
    int *f0candI, *candVoiceI, *candScoreI;  // array of size nInputLevels;
    int *F0rawI, *voicingClipI, *voicingC1I;

    FLOAT_DMEM *median0WorkspaceF0cand;
    FLOAT_DMEM *lastFinal;
    FLOAT_DMEM lastFinalF0;

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
    
    cPitchSmoother(const char *_name);
    
    virtual ~cPitchSmoother();
};




#endif // __CPITCHSMOOTHER_HPP
