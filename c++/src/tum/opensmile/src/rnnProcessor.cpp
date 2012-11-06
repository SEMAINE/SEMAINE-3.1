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

 example of a vector processor

*/



#include <rnnProcessor.hpp>

#define MODULE "cRnnProcessor"


SMILECOMPONENT_STATICS(cRnnProcessor)

SMILECOMPONENT_REGCOMP(cRnnProcessor)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CRNNPROCESSOR;
  sdescription = COMPONENT_DESCRIPTION_CRNNPROCESSOR;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
    //ct->setField("nameAppend",NULL,"processed");
    ct->setField("netfile","The file which contains the trained network created by rnnlib",(const char *)NULL);
    ct->setField("classlabels","Give a string of comma separated (NO spaces allowed!!) class names (e.g. class1,class2,class3) for a classification or transcription task",(const char*)NULL);
    ct->setField("printConnections","1/0 = yes/no : print human readable information on the network layers on connections",0);
    //ct->setField("printInputStats","1/0 = yes/no : print input weight sums (can be used for feature selection...)",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cRnnProcessor);
}

SMILECOMPONENT_CREATE(cRnnProcessor)

//-----

cRnnProcessor::cRnnProcessor(const char *_name) :
  cDataProcessor(_name), frameO(NULL), nClasses(0)
{

}

void cRnnProcessor::fetchConfig()
{
  cDataProcessor::fetchConfig();
  
  netfile = getStr("netfile");
  SMILE_IDBG(2,"netfile = '%s'",netfile);

  const char * _classlabels = getStr("classlabels");
  SMILE_IDBG(2,"classlabels = '%s'",_classlabels);
  if (_classlabels != NULL) {
    classlabels = strdup(_classlabels);
    // count number of classes:
    nClasses = 1;
    char *cl = classlabels;
    while(1) {
      char *x = strchr(cl,',');
      if (x!=NULL) {
        cl=x+1;
        nClasses++;
      } else {
        break;
      }
    }

    // allocate memory for classes array
    classlabelArr = (const char **)calloc(1,sizeof(const char *)*nClasses);

    // parse class names
    long i=0;
    cl = classlabels;
    while(1) {
      char *x = strchr(cl,',');
      if (x!=NULL) {
        *x=0;
        classlabelArr[i] = (const char *)cl;
        if (*(x+1) == 0) { nClasses--; break; }
        cl = x+1;
        i++;
      } else {
        classlabelArr[i] = (const char *)cl;
        i++;
        break;
      }
    }
   
  }
  
  printConnections = getInt("printConnections");
  SMILE_IDBG(2,"printConnections = %i",printConnections);
}


int cRnnProcessor::myConfigureInstance()
{
  int ret = cDataProcessor::myConfigureInstance();
  if (ret) {
    ret = smileRnn_loadNet(netfile,net);
    //TODO: create individual nets for each idxi, or only one net if "processArrayFields=0"
    // thus: move this code to  setupNamesForField
  }
  return ret;
}


/*
int cRnnProcessor::configureWriter(sDmLevelConfig &c)
{
  c.N = 
  // you must return 1, in order to indicate configure success (0 indicated failure)
  return 1;
}
*/

// optional
/*
int cRnnProcessor::setupNamesForField(int i, const char*name, long nEl)
{
  return cVectorProcessor::setupNamesForField(i,name,nEl);
}
*/

int cRnnProcessor::setupNewNames(long nEl)
{
  if ((net.task == NNTASK_CLASSIFICATION)||(net.task == NNTASK_TRANSCRIPTION)) {
    long i;
   if (net.outputSize) printf("outputsize: %i\n",net.outputSize);
   if (nClasses) printf("classes: %i\n",nClasses);
    for (i=0; i<MIN(nClasses,net.outputSize); i++) {
      addNameAppendField("RNNoutpAct",classlabelArr[i],1);
    }
    if (net.outputSize > nClasses) {
      if (net.task == NNTASK_TRANSCRIPTION) {
        for (i=nClasses; i<net.outputSize-1; i++) {
          addNameAppendField("RNNoutpAct","unlabelled",1);
        }
        addNameAppendField("RNNoutpAct","ctcOther",1);
      } else {
        for (i=nClasses; i<net.outputSize; i++) {
          addNameAppendField("RNNoutpAct","unlabelled",1);
        }
      }
    }
  } else {
    addNameAppendField("RNNoutpAct",nameAppend,net.outputSize);
  }
	namesAreSet=1;
  return 1;
}



int cRnnProcessor::myFinaliseInstance()
{
  int ret = cDataProcessor::myFinaliseInstance();

  if (ret) {
    /*
    if (getInt("printInputStats")) {
      FLOAT_NN *wg = NULL;
      long N = smileRnn_getInputSelection(net, &wg);
      long i;
      if (wg != NULL) {
        SMILE_PRINT("input weighting:");
        for (i=0; i<N; i++) {
          SMILE_PRINT("%i: %f",i,wg[i]);
        }
      } else {
        SMILE_IERR(1,"input weighting information is not available");
      }
    }
    */
    ret = smileRnn_createNet(net,rnn,printConnections);
    if (ret) {
      in = (FLOAT_NN*)malloc(sizeof(FLOAT_NN)*net.inputSize);
      out = (FLOAT_DMEM*)malloc(sizeof(FLOAT_DMEM)*net.outputSize);
    }
    frameO = new cVector(net.outputSize);
  }

  return ret;
}


// a derived class should override this method, in order to implement the actual processing
int cRnnProcessor::myTick(long long t)
{
  cVector * frame = reader->getNextFrame();
  if (frame == NULL) return 0;

  // pass current vector to net
  long i, _N=0;
  for (i=0; i<MIN(frame->N,net.inputSize); i++) { in[i] = (FLOAT_NN)(frame->dataF[i]); }
  rnn->forward(in, MIN(frame->N,net.inputSize));
  
  // get output
  const FLOAT_NN *outp = rnn->getOutput(&_N);
  
  //copy to *dst;
  for (i=0; i<MIN(frameO->N,_N); i++) {
    frameO->dataF[i] = (FLOAT_DMEM)outp[i];
  }

  writer->setNextFrame(frameO);

  return 1;
}

cRnnProcessor::~cRnnProcessor()
{
  if (in != NULL) free(in);
  if (out != NULL) free(out);
  if (rnn != NULL) delete rnn;
  if (classlabels != NULL) free(classlabels);
  if (classlabelArr != NULL) free(classlabelArr);
  if (frameO != NULL) delete frameO;
}

