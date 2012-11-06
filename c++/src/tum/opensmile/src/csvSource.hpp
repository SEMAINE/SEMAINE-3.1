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

CSV (Comma seperated value) file reader. 
This component reads all columns as attributes into the data memory. One line thereby represents one frame or sample. The first line may contain a header with the feature names (see header=yes/no/auto option).

*/


#ifndef __CCSVSOURCE_HPP
#define __CCSVSOURCE_HPP

#include <smileCommon.hpp>
#include <dataSource.hpp>

#define COMPONENT_DESCRIPTION_CCSVSOURCE "This component reads CSV (Comma seperated value) files. It reads all columns as attributes into the data memory. One line represents one frame. The first line may contain a header with the feature names (see header=yes/no/auto option)."
#define COMPONENT_NAME_CCSVSOURCE "cCsvSource"

#undef class
class DLLEXPORT cCsvSource : public cDataSource {
  private:
    //int nAttr;
    FILE *filehandle;
    const char *filename;

    int *field;
    //int fieldNalloc;
    int nFields, nCols;
    int eof;

    int header;
    char delimChar;

    long lineNr;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    void setGenericNames(int nDelim);
    void setNamesFromCSVheader(char *line, int nDelim);

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int setupNewNames(long nEl=0);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cCsvSource(const char *_name);

    virtual ~cCsvSource();
};




#endif // __CCSVSOURCE_HPP
