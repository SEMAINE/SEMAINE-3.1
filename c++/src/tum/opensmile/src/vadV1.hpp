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

voice activity detection based on LSF and Pitch features + smoothing

*/


#ifndef __CVADV1_HPP
#define __CVADV1_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CVADV1 "A voice activity detector based on Line-Spectral-Frequencies, Mel spectra and energy + smoothing. This component requires input of the following type in the following order: MelSpec;lsf;energy. See vadV1.hpp for an example config!"
#define COMPONENT_NAME_CVADV1 "cVadV1"

/* what we have to read from the input level:

LSF
Pitch F0 (raw)
Energy

*********** example config ***********

[enV:cEnergy]
reader.dmLevel=frame
writer.dmLevel=energy
nameAppend=energy
rms=1
log=1

[lpc:cLpc]
reader.dmLevel=frames
saveRefCoeff=0
writer.dmLevel=lpc
p=10

[mspecV:cMelspec]
reader.dmLevel=fftmagnitude
writer.dmLevel=mspec
htkcompatible = 0
usePower = 0
nBands = 14
lofreq = 50
hifreq = 4000

[lsp:cLsp]
reader.dmLevel=lpc
writer.dmLevel=lsp

[vad:cVadV1]
reader.dmLevel=mspec;lsp;energy
writer.dmLevel=vad11
writer.levelconf.noHang=1
debug=\cm[vaddebug{0}:1=debug vad]
threshold=\cm[threshold{-13}:VAD threshold]
disableDynamicVAD=\cm[disableDynamicVAD{0}:disable dynamic threshold vad, instead use energy based vad only, the energy threshold can be set via the 'threshold' option]
;threshold=\cm[threshold{-13.0}:VAD energy threshold, minimum energy for dynamic vad, can be very small, it is used only as a backup; real threshold if disableDynamicVAD is set, in that case you should set the threshold to approx.]

*/
 
#define NINIT  50  // NINIT must be < FTBUF!!
#define FTBUF  100
#define FUZBUF 10

#undef class
class DLLEXPORT cVadV1 : public cDataProcessor {
  private:
    int debug, disableDynamicVAD;

    long specIdx, specN, lsfN;
    long F0rawIdx, voiceProbIdx, eIdx;

    FLOAT_DMEM *spec;
    int t0histIdx; int vadBin;
    FLOAT_DMEM t0hist[8];
    FLOAT_DMEM div0;
    FLOAT_DMEM turnSum, turnN;

    // history for smooting:
    FLOAT_DMEM f0v_0, ent_0, E_0;
    FLOAT_DMEM ar0, ar1, arU, arV;

    // adaptive thresholds:
    long nInit;
    FLOAT_DMEM uF0v, uEnt, uE;
    FLOAT_DMEM vF0v, vEnt, vE;
    FLOAT_DMEM tuF0v, tuEnt, tuE;
    FLOAT_DMEM tvF0v, tvEnt, tvE;
    FLOAT_DMEM F0vH[FTBUF], entH[FTBUF], EH[FTBUF];
    int F0vHidx, entHidx, EHidx;
    FLOAT_DMEM tF0vH[FTBUF], tentH[FTBUF], tEH[FTBUF];
    int tF0vHidx, tentHidx, tEHidx;
    int nInitT, nInitN;
    
    FLOAT_DMEM vadFuzH[FUZBUF];
    int vadFuzHidx;

    FLOAT_DMEM minE, minEn, maxEn;


    FLOAT_DMEM threshold;
    int nPost, nPre;
    int useRMS;
    int turnState, actState;
    long startP, startP0, endP0;
    double endSmileTime, startSmileTime;
   
    int cnt1, cnt2;

    const char *recFramer, *recComp, *statusRecp;
    
    void findInputMapping();
    //FLOAT_DMEM specEntropy(FLOAT_DMEM *lsf, int N);
    FLOAT_DMEM pitchVariance(FLOAT_DMEM curF0raw);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    //virtual int isVoice(FLOAT_DMEM *src);
    //virtual void updateThreshold(FLOAT_DMEM eRmsCurrent);

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    //virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);
    //virtual void configureField(int idxi, long __N, long nOut);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
//    virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
//    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cVadV1(const char *_name);

    virtual ~cVadV1();
};




#endif // __CVADV1_HPP
