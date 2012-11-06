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

*/


#ifndef __CSPECSCALE_HPP
#define __CSPECSCALE_HPP

#include <smileCommon.hpp>
#include <vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CSPECSCALE "This component performs linear/non-linear axis scaling of FFT magnitude spectra with spline interpolation."
#define COMPONENT_NAME_CSPECSCALE "cSpecScale"

/*
#define SPECTSCALE_LINEAR   0
#define SPECTSCALE_LOG      1
#define SPECTSCALE_BARK     2
#define SPECTSCALE_MEL      3
#define SPECTSCALE_SEMITONE 4
#define SPECTSCALE_BARK_SCHROED     5
#define SPECTSCALE_BARK_SPEEX       6
*/

#define INTERP_NONE       0
#define INTERP_LINEAR     1
#define INTERP_SPLINE     2

#undef class
class DLLEXPORT cSpecScale : public cVectorProcessor {
  private:
    int scale; // target scale
    int sourceScale;
    int interpMethod;
    int specSmooth, specEnhance;
    int auditoryWeighting;
    double logScaleBase, logSourceScaleBase;
    double minF, maxF, fmin_t, fmax_t;
    long nPointsTarget;
    double firstNote, param;

    long nMag, magStart;
    double fsSec;
    double deltaF, deltaF_t;

    double *f_t;
    double *spline_work;
    double *y, *y2;
    double *audw;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
	  virtual int setupNewNames(long nEl);
    virtual int dataProcessorCustomFinalise();
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

    //virtual double scaleTransfFwd(double x);
    //virtual double scaleTransfInv(double x);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSpecScale(const char *_name);
    
    virtual ~cSpecScale();
};




#endif // __CSPECSCALE_HPP
