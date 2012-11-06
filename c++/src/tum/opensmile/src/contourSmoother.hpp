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


/*  openSMILE component: contour smoother

smooth data contours by moving average filter

*/



#ifndef __CCONTOURSMOOTHER_HPP
#define __CCONTOURSMOOTHER_HPP

#include <smileCommon.hpp>
#include <windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CCONTOURSMOOTHER "This component smooths data contours by applying a moving average filter of configurable length."
#define COMPONENT_NAME_CCONTOURSMOOTHER "cContourSmoother"

#undef class
class DLLEXPORT cContourSmoother : public cWindowProcessor {
  private:
    int smaWin;
    int noZeroSma;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();

    //virtual int configureWriter(const sDmLevelConfig *c);
    //virtual int setupNamesForField(int i, const char*name, long nEl);

    // buffer must include all (# order) past samples
    virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );
    
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cContourSmoother(const char *_name);

    virtual ~cContourSmoother();
};


#endif // __CCONTOURSMOOTHER_HPP
