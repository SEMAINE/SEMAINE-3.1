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

sort values in a vector

*/



#include <vectorSorter.hpp>

#define MODULE "cVectorSorter"


SMILECOMPONENT_STATICS(cVectorSorter)

SMILECOMPONENT_REGCOMP(cVectorSorter)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CVECTORSORTER;
  sdescription = COMPONENT_DESCRIPTION_CVECTORSORTER;

  // we inherit cVectorSorter configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
	// if you set description to NULL, the existing description will be used, thus the following call can
	// be used to update the default value:
    ct->setField("nameAppend",NULL,"sorted");

    ct->setField("descending","The default sort order is 'ascending', if you want to sort in descending order, set this variable to 1.",0);
    // this is an example for adding an integer option:
	//ct->setField("inverse","1 = perform inverse FFT",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cVectorSorter);
}

SMILECOMPONENT_CREATE(cVectorSorter)

//-----

cVectorSorter::cVectorSorter(const char *_name) :
  cVectorProcessor(_name)
{

}

void cVectorSorter::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  if (getInt("descending")) {
    descending = 1;
  } else {
    descending = 0;
  }
}



// a derived class should override this method, in order to implement the actual processing
int cVectorSorter::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  //TODO
  SMILE_IERR(1,"INT not yet implemented!");
  return 0;
}

// do the sorting
int cVectorSorter::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  long min = MIN(Nsrc,Ndst);
  if (dst != src) memcpy(dst,src,sizeof(FLOAT_DMEM)*min);
  smileUtil_quickSort_FLOATDMEM( dst, min );
  if (descending) {
    // ... I know it is probably more efficient to write an 'inverse' quicksort, however we don't want to go that far into optimisation...
    smileUtil_reverseOrder_FLOATDMEM( dst, min );
  }
  return 1;
}

cVectorSorter::~cVectorSorter()
{
}

