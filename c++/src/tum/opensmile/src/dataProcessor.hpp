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


/*  openSMILE component: cDataProcessor 

This is an abstract base class for all components which read data from the data memory and write new data to the data memory.

*/


#ifndef __DATA_PROCESSOR_HPP
#define __DATA_PROCESSOR_HPP

#include <smileCommon.hpp>
#include <dataWriter.hpp>
#include <dataReader.hpp>

#define COMPONENT_DESCRIPTION_CDATAPROCESSOR "This is an abstract base class for all components which read data from the data memory and write new data to the data memory."
#define COMPONENT_NAME_CDATAPROCESSOR "cDataProcessor"

#undef class
class DLLEXPORT cDataProcessor : public cSmileComponent {
  private:
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    int nInput, inputStart;
    const char *inputName;

    cDataWriter *writer;
    cDataReader *reader;
    int namesAreSet;
    
    double buffersize_sec; /* buffersize of write level, as requested by config file (in seconds)*/
    double blocksizeR_sec, blocksizeW_sec; /* blocksizes for block processing (in seconds)*/
    long buffersize; /* buffersize of write level, as requested by config file (in frames)*/
    long blocksizeR, blocksizeW; /* blocksizes for block processing (in frames)*/
    
    int copyInputName; /* 1=copy input name and append "nameAppend" (if != NULL) , 0 = do not copy and set name only to "nameAppend" */
    const char * nameAppend; /* suffix to append to element names */
    void addNameAppendField(const char*base, const char*append, int _N=1, int arrNameOffset=0);
    void addNameAppendFieldAuto(const char*base, const char *customFixed, int _N=1, int arrNameOffset=0);

    // get the size of input frames in seconds
    double getFrameSizeSec();

    // get the period of the base level (e.g. sampling rate for wave input)
    double getBasePeriod();

    //
    // find a field in the input level by a part of its name or its full name
    // set the internal variables nInput, inputStart, and inputName
    //
    // nEl specifies the maximum number of input elements (for checking valid range of field index)
    // (optional) fullName = 1: match full field name instead of part of name
    long findInputField(const char *namePartial, int fullName=0, long nEl=-1);

    //
    // find a field in the input level by a part of its name or its full name
    //
    // nEl specifies the maximum number of input elements (for checking valid range of field index)
    // *N , optional, a pointer to variable (long) that will be filled with the number of elements in the field
    // **_fieldName : will be set to a pointer to the name of the field
    // (optional) fullName = 1: match full field name instead of part of name
    long findField(const char *namePartial, int fullName=0, long *_N=NULL, const char **_fieldName=NULL, long nEl=-1, int *more=NULL);

    //
    // search for an element by its partial name (if multiple matches are found, only the first is returned)
    // if fullName==1 , then do only look for exact name matches
    // the return value will be the element(!) index , i.e. the index of the element in the data vector
    //
    long findElement(const char *namePartial, int fullName, long *_N, const char **_fieldName, int *more);

    // get data from input field (previously found by findInputField())
    // stores nInput elements in **_in, the memory is allocated if necessary (i.e. *_in == NULL)
    int getInputFieldData(const FLOAT_DMEM *src, long Nsrc, FLOAT_DMEM **_in);

    virtual void fetchConfig();
    virtual void mySetEnvironment();

    virtual int myRegisterInstance(int *runMe=NULL);
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    
    //---- *runMe return value for component manager : 0, don't call my tick of this component, 1, call myTick
    virtual int runMeConfig() { return 1; }

    //---- HOOKs during configure (for setting level parameters, buffersize/type, blocksize requirements)
    // overwrite this, to configure the writer AFTER the reader config is available
    // possible return values (others will be ignored): 
    // -1 : configureWriter has overwritten c->nT value for the buffersize, myConfigureInstance will not touch nT !
    virtual int configureWriter(sDmLevelConfig &c) { return 1; }
    
    // configure reader, i.e. setup matrix reading, blocksize requests etc...
    virtual int configureReader();

    //---- HOOKs during finalise (for setting names)
    virtual int dataProcessorCustomFinalise() { return 1; }

    virtual int setupNewNames(long nEl);

    // clone the input field info of field 'sourceIdx' to target field 'targetIdx' (can be -1 for last field..)
    // force = 1: overwrite existing data (which was set by custom code, for example)
    virtual int cloneInputFieldInfo(int sourceFidx, int targetFidx=-1, int force = 0);

    //virtual int cloneInputFieldInfo(int sourceFidx, const char *_name) {
    //
    //}

    virtual int setupNamesForField(int i, const char*name, long nEl);
    virtual void configureField(int idxi, long __N, long _nOut) {}
	  //virtual int setupNamesForField(int i, const char*name, long nEl, int arrNameOffset);


    virtual void setEOI() {
      cSmileComponent::setEOI();
      if (reader!=NULL) reader->setEOI();
      if (writer!=NULL) writer->setEOI();
    }

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDataProcessor(const char *_name);
    virtual ~cDataProcessor();
};




#endif // __DATA_PROCESSOR_HPP
