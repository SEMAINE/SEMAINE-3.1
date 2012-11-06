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

Signal source. Generates various noise types and pre-defined signals.

*/


#ifndef __CSIGNALGENERATOR_HPP
#define __CSIGNALGENERATOR_HPP

#include <smileCommon.hpp>
#include <dataSource.hpp>

#define COMPONENT_DESCRIPTION_CSIGNALGENERATOR "This component provides a signal source. This source generates various noise types and pre-defined signals and value patterns. See the configuration documentation for a list of currently implemented types."
#define COMPONENT_NAME_CSIGNALGENERATOR "cSignalGenerator"

#define NOISE_WHITE   0   // white gaussian noise 'white'
#define SIGNAL_SINE   1   // sinusodial signal (single frequency) 'sine'
#define SIGNAL_CONST  2   // constant value 'const'
#define SIGNAL_RECT   3   // rectangular periodic signal 'rect'
//...

#undef class
class DLLEXPORT cSignalGenerator : public cDataSource {
  private:
    long nValues;
    int randSeed;
    FLOAT_DMEM stddev, mean;
    FLOAT_DMEM constant;
    double signalPeriod, phase;
    double myt ; // current time
    double samplePeriod;
    double lastP;
    double scale;
    double val;

    int * nElements;
    char ** fieldNames;
    int nFields;
    
    int noiseType;
    long lengthFrames;
    long curT;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cSignalGenerator(const char *_name);

    virtual ~cSignalGenerator();
};




#endif // __CSIGNALGENERATOR_HPP
