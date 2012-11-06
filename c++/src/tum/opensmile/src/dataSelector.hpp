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

example for dataProcessor descendant

*/


#ifndef __CDATASELECTOR_HPP
#define __CDATASELECTOR_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CDATASELECTOR "This component copies data from one level to another, thereby selecting frame fields and elements by their element/field name."
#define COMPONENT_NAME_CDATASELECTOR "cDataSelector"

typedef struct {
  long eIdx; // element index

// actually these two are not needed... ??
  long fIdx; // field index
  long aIdx; // array index, if field is an array, else 0
  long N;
} sDataSelectorSelData;

#undef class
class DLLEXPORT cDataSelector : public cDataProcessor {
  private:
    int elementMode, selectionIsRange;
    cVector *vecO;

    const char * selFile;
    int nSel; // number of names in names array
    char **names;
    
    int nElSel, nFSel;  // actual number of elements(!) or fields selected (size of mapping array)
    sDataSelectorSelData *mapping; // actual data selection map
    long *idxSelected;

    // for loading files...
    int fselType;

    int loadSelection();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int setupNewNames(long nEl);
    //virtual int configureWriter(const sDmLevelConfig *c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDataSelector(const char *_name);

    virtual ~cDataSelector();
};




#endif // __CDATASELECTOR_HPP
