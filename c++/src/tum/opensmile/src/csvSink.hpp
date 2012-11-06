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

Comma Separated Value file output (CSV)

*/


#ifndef __CCSVSINK_HPP
#define __CCSVSINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>

#define COMPONENT_DESCRIPTION_CCSVSINK "This component exports data in CSV (comma-separated-value) format used in many spreadsheet applications. As the first line of the CSV file a header line may be printed, which contains a delimiter separated list of field names of the output values."
#define COMPONENT_NAME_CCSVSINK "cCsvSink"

#undef class
class DLLEXPORT cCsvSink : public cDataSink {
  private:
    FILE * filehandle;
    const char *filename;
    char delimChar;
    int lag;
    int append, timestamp, number, printHeader;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL
    
    cCsvSink(const char *_name);

    virtual ~cCsvSink();
};




#endif // __CCSVSINK_HPP
