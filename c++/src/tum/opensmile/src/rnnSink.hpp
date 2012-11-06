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

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CRNNSINK_HPP
#define __CRNNSINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>
#include <rnn.hpp>

#define COMPONENT_DESCRIPTION_CRNNSINK "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CRNNSINK "cRnnSink"

#undef class




class DLLEXPORT cRnnSink : public cDataSink {
  private:
    FILE *outfile;
    FILE *outfileC;
    const char *netfile;
    const char *actoutput;
    const char *classoutput;
    char *classlabels;
    const char **classlabelArr;
    long nClasses;
    int ctcDecode;
    cNnRnn *rnn;
    cRnnNetFile net;
    FLOAT_NN *in;
    FLOAT_DMEM *out;
    int lasti;
    int printConnections;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    //cRnnWeightVector *createWeightVectorFromLine(char *line);
    //int loadNet(const char *filename);
    
//    int findPeepWeights(unsigned long id);
    //int findWeights(unsigned long idFrom, unsigned long idTo);
    //cNnLSTMlayer *createLstmLayer(int i, int idx, int dir=LAYER_DIR_FWD);
    // create a network from a successfully loaded net config file (loadNet function)
    //int createNet();
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cRnnSink(const char *_name);

    virtual ~cRnnSink();
};




#endif // __CRNNSINK_HPP
