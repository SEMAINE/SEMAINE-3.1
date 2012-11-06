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



#include <csvSink.hpp>

#define MODULE "cCsvSink"

/*Library:
sComponentInfo * registerMe(cConfigManager *_confman) {
  cDataSink::registerComponent(_confman);
}
*/

SMILECOMPONENT_STATICS(cCsvSink)

//sComponentInfo * cCsvSink::registerComponent(cConfigManager *_confman)
SMILECOMPONENT_REGCOMP(cCsvSink)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CCSVSINK;
  sdescription = COMPONENT_DESCRIPTION_CCSVSINK;

  // we inherit cDataSink configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSink")

  SMILECOMPONENT_IFNOTREGAGAIN_BEGIN
    ct->setField("filename","The CSV file to write to","smileoutput.csv");
    ct->setField("delimChar","The column delimiter character to use (usually ',' or ';') (NOTE: use '<space>' or '<tab>' for these special characters respectively)",';');
    ct->setField("lag","output data <lag> frames behind",0,0,0);
    ct->setField("append","1 = append to an existing file, or create a new file; 0 = overwrite an existing file, or create a new file",0);
    ct->setField("timestamp","1 = print a timestamp attribute for each output frame (1/0 = yes/no)",1);
    ct->setField("number","1 = print an instance number (= frameIndex) attribute for each output frame (1/0 = yes/no)",1);
    ct->setField("printHeader","1 = print a header line as the first line in the CSV file. This line contains the attribute names separated by the delimiter character.",1);

//    ct->setField("instanceBase","if not empty, print instance name attribute <instanceBase_Nr>","");
//    ct->setField("instanceName","if not empty, print instance name attribute <instanceName>","");

  SMILECOMPONENT_IFNOTREGAGAIN_END

  SMILECOMPONENT_MAKEINFO(cCsvSink);
}

SMILECOMPONENT_CREATE(cCsvSink)

//-----

cCsvSink::cCsvSink(const char *_name) :
  cDataSink(_name),
  filehandle(NULL),
  filename(NULL),
  printHeader(0),
  delimChar(';')
{
}

void cCsvSink::fetchConfig()
{
  cDataSink::fetchConfig();
  
  filename = getStr("filename");
  SMILE_IDBG(3,"filename = '%s'",filename);

  delimChar = getChar("delimChar");
  SMILE_IDBG(3,"delim = '%c'",delimChar);

  lag = getInt("lag");
  SMILE_IDBG(3,"lag = %i",lag);

  append = getInt("append");
  if (append) { SMILE_IDBG(3,"append to file is enabled"); }

  printHeader = getInt("printHeader");
  if (append) { SMILE_IDBG(3,"printing header with feature names"); }

  number = getInt("number");
  if (append) { SMILE_IDBG(3,"printing of frameIndex (number) to file is enabled"); }

  timestamp = getInt("timestamp");
  if (append) { SMILE_IDBG(3,"printing timestamp attribute (index 1) enabled"); }

/*
  instanceBase = getStr("instanceBase");
  SMILE_DBG(3,"instanceBase = '%s'",instanceBase);

  instanceName = getStr("instanceName");
  SMILE_DBG(3,"instanceName = '%s'",instanceName);
*/


}

/*
int cCsvSink::myConfigureInstance()
{
  int ret=1;
  ret *= cDataSink::myConfigureInstance();
  // ....
  //return ret;
}
*/


int cCsvSink::myFinaliseInstance()
{
  int ap=0;

  int ret = cDataSink::myFinaliseInstance();
  if (ret==0) return 0;
  
  if (append) {
    // check if file exists:
    filehandle = fopen(filename, "r");
    if (filehandle != NULL) {
      fclose(filehandle);
      filehandle = fopen(filename, "a");
      ap=1;
    } else {
      filehandle = fopen(filename, "w");
    }
  } else {
    filehandle = fopen(filename, "w");
  }
  if (filehandle == NULL) {
    COMP_ERR("Error opening file '%s' for writing (component instance '%s', type '%s')",filename, getInstName(), getTypeName());
  }
  
  if ((!ap)&&(printHeader)) {
    // write header ....
    if (number) {
      fprintf(filehandle, "frameIndex%c",delimChar);
    }
    if (timestamp) {
      fprintf(filehandle, "frameTime%c",delimChar);
    }

    long _N = reader->getLevelN();
    long i;
    for(i=0; i<_N-1; i++) {
      char *tmp = reader->getElementName(i);
      fprintf(filehandle, "%s%c",tmp,delimChar);
      free(tmp);
    }
    char *tmp = reader->getElementName(i);
    fprintf(filehandle, "%s%s",tmp,NEWLINE);
    free(tmp);
  }
  
  return ret;
}


int cCsvSink::myTick(long long t)
{
  if (filehandle == NULL) return 0;

  SMILE_DBG(4,"tick # %i, writing to CSV file (lag=%i):",t,lag);
  cVector *vec= reader->getFrameRel(lag);  //new cVector(nValues+1);
  if (vec == NULL) return 0;
  //else reader->nextFrame();

  long vi = vec->tmeta->vIdx;
  double tm = vec->tmeta->time;
  
/*
  if (prname==1) {
    fprintf(filehandle,"'%s',",instanceName);
  } else if (prname==2) {
    fprintf(filehandle,"'%s_%i',",instanceBase,vi);
  }
  */
  

  if (number) fprintf(filehandle,"%i%c",vi,delimChar);
  if (timestamp) fprintf(filehandle,"%f%c",tm,delimChar);

  // now print the vector:
  int i;
  for (i=0; i<vec->N-1; i++) {
    // print float as integer if its decimals are zero
    if (vec->dataF[i] == floor(vec->dataF[i])) {
      fprintf(filehandle,"%.0f%c",vec->dataF[i],delimChar);
    } else {
      fprintf(filehandle,"%e%c",vec->dataF[i],delimChar);
    }
  }
  if (vec->dataF[i] == floor(vec->dataF[i])) {
    fprintf(filehandle,"%0.f%s",vec->dataF[i],NEWLINE);
  } else {
    fprintf(filehandle,"%e%s",vec->dataF[i],NEWLINE);
  }

  // tick success
  return 1;
}


cCsvSink::~cCsvSink()
{
  fclose(filehandle);
}

