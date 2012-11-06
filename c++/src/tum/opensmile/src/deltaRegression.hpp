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


/*  openSMILE component: delta coefficients

compute delta regression using regression formula

*/



#ifndef __CDELTAREGRESSION_HPP
#define __CDELTAREGRESSION_HPP

#include <smileCommon.hpp>
#include <windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CDELTAREGRESSION "This component computes delta regression coefficients using the regression equation from the HTK book."
#define COMPONENT_NAME_CDELTAREGRESSION "cDeltaRegression"

#undef class
class DLLEXPORT cDeltaRegression : public cWindowProcessor {
private:
  int halfWaveRect, absOutput;
  int deltawin;
  FLOAT_DMEM norm;

protected:
  SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();

  //virtual int configureWriter(const sDmLevelConfig *c);
  //virtual int setupNamesForField(int i, const char*name, long nEl);

  // buffer must include all (# order) past samples
  virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );


public:
  SMILECOMPONENT_STATIC_DECL

    cDeltaRegression(const char *_name);

  virtual ~cDeltaRegression();
};


#endif // __CDELTAREGRESSION_HPP
