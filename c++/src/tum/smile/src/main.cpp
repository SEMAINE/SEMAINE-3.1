/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
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
 *******************************************************************************/


#define MODULE "main"
#include "featum_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "featum_util.h"       //( optional )
#ifdef LIVE_REC
#include "live-input.hpp"
#endif
#include "silence-detector.hpp"

#include "wave-input.hpp"
#include "wave-output.hpp"
#include "option-parser.hpp"
#include "input-framer.hpp"
#include "LLDex.hpp"
#include "LLDs.hpp"
#include "deltas.hpp"
#include "functionals.hpp"

#include "HTKoutput.hpp"
#include "CSVoutput.hpp"
#include "ARFFoutput.hpp"
#include "LSVMoutput.hpp"
#include "SVMpredict.hpp"

//int loglevel = 0;
#undef FUNCTION

// TODO: define all levels in ONE central location...!
// #define LLD_LEVEL0 0    // defined elsewhere!
// #define LLD_LEVEL1 1    // defined elsewhere!
// #define LLD_LEVEL2 2    // defined elsewhere!
#define LLD0_DE 3
#define LLD1_DE 4
#define LLD2_DE 5
#define LLD0_DEDE 6
#define LLD1_DEDE 7
#define LLD2_DEDE 8

#define LLD0_FUNC_L0 9
#define LLD1_FUNC_L0 10
#define LLD2_FUNC_L0 11
#define LLD0_DE_FUNC_L0 12
#define LLD1_DE_FUNC_L0 13
#define LLD2_DE_FUNC_L0 14
#define LLD0_DEDE_FUNC_L0 15
#define LLD1_DEDE_FUNC_L0 16
#define LLD2_DEDE_FUNC_L0 17

#define LLD0_FUNC_L1 18
#define LLD1_FUNC_L1 19
#define LLD2_FUNC_L1 20
#define LLD0_DE_FUNC_L1 21
#define LLD1_DE_FUNC_L1 22
#define LLD2_DE_FUNC_L1 23
#define LLD0_DEDE_FUNC_L1 24
#define LLD1_DEDE_FUNC_L1 25
#define LLD2_DEDE_FUNC_L1 26

#define LLD0_FUNC_L0f 27
#define LLD0_DE_FUNC_L0f 28
#define LLD0_DEDE_FUNC_L0f 29

#define ftMEM_nLevels 30


/************* put this in GENoutput.c ::: !!! ********************/
/*

*/
/*********************************/


/***************** options setup ***********************/
pOptions setupOptions( cOptionParser &parser ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_

  // add your parameters:::  (don't forget to define variables in the struct in option-parser.h !!)
  parser.addBoolean( "help", 'h', &(parser.opt.help), 0, "Display this usage message" );
  parser.addFloat( "frameSize", 'S', &(parser.opt.frameSize), 0.032, "frame size in seconds", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addFloat(  "frameStep", 's', &(parser.opt.frameStep), 0.01, "frame step in seconds", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addFloat(  "preEmphasis", 'p', &(parser.opt.preEmphasis), 0.97, "1st order pre emphasis filter coefficient", MANDATORY_ARG, OPTIONAL_PARAM);
  #ifndef LIVE_REC
  parser.addPchar(  "infile", 'i', &(parser.opt.infile), NULL, "input WAVE file", MANDATORY_ARG, MANDATORY_PARAM);
  #else
  parser.addPchar(  "infile", 'i', &(parser.opt.infile), NULL, "input WAVE file", MANDATORY_ARG, OPTIONAL_PARAM);
  #endif
  parser.addPchar(  "htkout", 'o', &(parser.opt.htkout), NULL, "output HTK file (mfcc only)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "csvout", 'c', &(parser.opt.csvout), NULL, "output CSV file (low-level)", MANDATORY_ARG, OPTIONAL_PARAM);
  #ifdef ENABLE_FUNCTIONALS
  parser.addPchar(  "arffout", 'a', &(parser.opt.arffout), NULL, "output ARFF file (functionals over 0.5s frames)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arffoutFull", 'f', &(parser.opt.arffoutFull), NULL, "output ARFF file (functionals over full input file)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arffoutFullA", 0, &(parser.opt.arffoutFullA), NULL, "output ARFF file (functionals over full input file), arousal", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arffoutFullV", 0, &(parser.opt.arffoutFullV), NULL, "output ARFF file (functionals over full input file), valence", MANDATORY_ARG, OPTIONAL_PARAM);

  parser.addPchar(  "arffoutHR", 'F', &(parser.opt.arffoutHR), NULL, "output ARFF file (hierarchical functionals over full input file)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arfflabel", 'L', &(parser.opt.arfflabel), NULL, "label for -f and -h options", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arffclasses", 0, &(parser.opt.arffclasses), NULL, "list of nominal classes {...} or \"numeric\"", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arffclassesA", 0, &(parser.opt.arffclassesA), NULL, "list of nominal classes {...} or \"numeric\" (arousal)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "arffclassesV", 0, &(parser.opt.arffclassesV), NULL, "list of nominal classes {...} or \"numeric\" (valence)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "labelA", 0, &(parser.opt.labelA), NULL, "label for -svmoutA", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar(  "labelV", 0, &(parser.opt.labelV), NULL, "label for -svmoutV", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmoutA", 'A', &(parser.opt.svmoutA), NULL, "LibSVM output file (append mode)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmoutV", 'V', &(parser.opt.svmoutV), NULL, "LibSVM output file (append mode)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmmodelA", 0, &(parser.opt.svmmodelA), NULL, "model for svm regression/classification (arousal)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmmodelV", 0, &(parser.opt.svmmodelV), NULL, "model for svm regression/classification (valence)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmscaleA", 0, &(parser.opt.svmscaleA), NULL, "scale for svm regression/classification (arousal)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmscaleV", 0, &(parser.opt.svmscaleV), NULL, "scale for svm regression/classification (valence)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addBoolean( "sildet", 0, &(parser.opt.sildet), 0, "Enable silence detector for turn segmentation" );  
  parser.addBoolean( "HRF", 0, &(parser.opt.HRF), 0, "Use Hierarchical instead of full-turn functionals" );
  #ifdef LIVE_REC
  #ifdef USE_PORTAUDIO  
  parser.addBoolean( "listdevices", 'l', &(parser.opt.listdevices), 0, "List portaudio devices" );  
  #endif
  #endif
  parser.addFloat( "silthresh", 0, &(parser.opt.sil_thresh), 0.01, "Silence threshold", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addPchar( "svmpredfselA", 0, &(parser.opt.svmpredfselA), NULL, "Feature selection file for live svm prediction (arousal)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addPchar( "svmpredfselV", 0, &(parser.opt.svmpredfselV), NULL, "Feature selection file for live svm prediction (valence)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addBoolean( "displayenergy", 'E', &(parser.opt.displayenergy), 0, "print log frame energy for debugging audio problems" );  


/*
  parser.addInt( "int1", 0, &(parser.opt.int1), 1, "int1", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addLONG_IDX( "long1", 0, &(parser.opt.long1), 1, "long1", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addFloat( "float1", 0, &(parser.opt.float1), 1, "float1", MANDATORY_ARG, OPTIONAL_PARAM );  
*/

  #endif
  parser.addPchar( "waveout", 'w', &(parser.opt.waveout), NULL, "wave file output of pcm stream", MANDATORY_ARG, OPTIONAL_PARAM);

  #ifdef LIVE_REC
  parser.addInt( "bufsize", 0, &(parser.opt.buffersize), 4096, "audio buffer size", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "device", 'd', &(parser.opt.device), -1, "audio device id to record from (see -l) [-1 for system default]", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "samplerate", 'r', &(parser.opt.samplerate), 16000, "set recording sample rate", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "bits", 'b', &(parser.opt.bits), 16, "set bits/sample", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "channels", 'C', &(parser.opt.channels), 1, "set no. of channels", MANDATORY_ARG, OPTIONAL_PARAM);
  #endif
  
//void optionParser_addString( pOptionParser obj, char *name, char abbr, char *val, char *dflt, char *info, int len,  int optional_arg, int mandatory );  
  //-----------------------

  pOptions opts = parser.doParse();
  if ((opts == NULL) || (opts->help)) {
    parser.showUsage();
    _FUNCTION_RETURN_(NULL); 
  } 
  _FUNCTION_RETURN_(opts); 
}
#undef FUNCTION

void debug_printOptions( pOptions opts )
#define FUNCTION "debug_printOptions"
{_FUNCTION_ENTER_
  FEATUM_DEBUG(0, " opts->help = %i", opts->help);

  FEATUM_DEBUG(0, " opts->frameSize = %f", opts->frameSize);
  FEATUM_DEBUG(0, " opts->frameStep = %f", opts->frameStep);
  FEATUM_DEBUG(0, " opts->preEmphasis = %f", opts->preEmphasis);

  if (opts->infile != NULL) FEATUM_DEBUG(0, " opts->infile = %s", opts->infile);
  if (opts->htkout != NULL) FEATUM_DEBUG(0, " opts->htkout = %s", opts->htkout);
  if (opts->csvout != NULL) FEATUM_DEBUG(0, " opts->csvout = %s", opts->csvout);
  if (opts->arffout != NULL) FEATUM_DEBUG(0, " opts->arffout = %s", opts->arffout);  
}
#undef FUNCTION
/****************************************/





/************** Ctrl+C signal handler **/
int featum_quit = 0;

#include  <signal.h>

void INThandler(int);

void  INThandler(int sig)
{
     char  c;

     signal(sig, SIG_IGN);
     featum_quit = 1;
     signal(SIGINT, INThandler);
}
/*******************************************/

/*     
     printf("OUCH, did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y')
          exit(0);
     else
          signal(SIGINT, INThandler);
     */

//#define REDUCE

int main(int argc, char *argv[])
#define FUNCTION "main"
{_FUNCTION_ENTER_


  #ifdef ENABLE_PROFILER
  timeval profiler;
  profiler_init(&profiler);
  #endif
 
  /*** option parser *******/

  cOptionParser parser(argc, argv); //options = optionParser_create( NULL, argc, argv, NULL );
  pOptions opts = setupOptions( parser );
  if (!opts) return exitApp(ERR_CMDLINE);
//  optionParser_globaliseOptions( options );

  #ifdef ENABLE_PRINT_OPTIONS
  debug_printOptions( opts );
  #endif

  

/*
sOptions oppp;
pOptions opts = &oppp;
memzero(opts,sizeof(sOptions));
*/
/*
opts->infile  = "edA01.wav";
opts->svmmodelA = "models/sal2/arousal.model";
opts->svmscaleA = "models/sal2/arousal.scale";  
opts->svmpredfselA = "models/sal2/arousal.fsel";  
opts->sildet = 1;
*/
  //char *inputfile = "great.wav";
  //char *htkOutputFile = "great.htk";
  char *inputfile = opts->infile;
  #ifndef LIVE_REC
  if (inputfile == NULL) {
    FEATUM_ERROR_FATAL(0,"Live recording not supported, you must specify an input file using the -i option!");
    return exitApp(ERR_CMDLINE);   
  }
  #endif
  #ifdef LIVE_REC
  #ifdef USE_PORTAUDIO
  if (opts->listdevices) {
    liveInput_listDevices();
    return 0;
  }
  #endif
  #endif


  char *htkOutputFile = opts->htkout;
  char *csvOutputFile = opts->csvout;
  #ifdef ENABLE_FUNCTIONALS
  char *arffOutputFile = opts->arffout;  
  #endif


  /***************************** wave file input *****************************/    
  cWaveInput *waveIn=NULL;

//  pWaveInput waveIn=NULL;
//  pLiveInput liveIn=NULL;

  #ifdef LIVE_REC
  cLiveInput *liveIn=NULL;
  if (opts->infile == NULL) { // setup live audio input
    FEATUM_DEBUG(4,"opening live input audio: %i Hz, %i bit, %i channel(s)",opts->samplerate, opts->bits, opts->channels);
//    cWaveParameters recParam(opts->samplerate, opts->channels, opts->bits) ;
//    liveIn = new cLiveInput(NULL, opts->device, opts->buffersize, recParam, LI_STREAM_STARTED);
    liveIn = new cLiveInput(NULL, opts->device, opts->buffersize, opts->samplerate, opts->bits, opts->channels, LI_STREAM_STARTED);

//    if (!(liveIn = liveInput_create( liveIn, NULL, opts->device, opts->buffersize, opts->samplerate, pcmBitsToSampleType(opts->bits,BYTEORDER_LE,opts->useFloat32), opts->channels, 1))) {
//      return exitApp(ERR_INPUT);
//    }
  } else {
  #endif
    FEATUM_DEBUG(4,"opening wave file %s as input",opts->infile);  //PCM_PREBUFFER, PCM_POSTBUFFER
    waveIn = new cWaveInput(opts->infile, opts->buffersize);
//    waveIn = new cWaveInput("great.wav", 0 );
//    if (!(waveIn = waveInput_create( waveIn, opts->infile,  opts->buffersize  ))) {
//      return exitApp(ERR_INPUT);
//    }

    FEATUM_DEBUG(4,"wave file %s was sucessfully opened",opts->infile);
    FEATUM_DEBUG(5,"sample rate of wave file: %i\n",waveIn->getSampleRate());
  #ifdef LIVE_REC
  }
  #endif
  /*------------------------------------end--------------------------------------*/


  /***************************** create input framer *****************************/    
  cInputFramer * framedInput = NULL;
  
  if (waveIn != NULL) {
    framedInput = new cInputFramer(*waveIn);
  } 
  #ifdef LIVE_REC
  else {
    framedInput = new cInputFramer(*liveIn); //inputFramer_create(audioStream_create(liveIn,TYPE_LIVEINPUT));
  }
  #endif
  framedInput->setAudioParameters(1, WORK_SAMPLETYPE, MEMORGA_INTERLV);
  int n_ll_levels = 0;   // low level levels (see below)
  //int nExtraLevels = 3;  // levels for functionals, deltas, etc.

  // add framer clients:
  int id1 = framedInput->addClientSecStep(opts->frameSize, opts->frameStep, 1); n_ll_levels++;
  framedInput->setPreEmphasis(id1, opts->preEmphasis);
//printf("h1 %i %i %i\n",(long)framedInput, framedInput->_data.client, framedInput->_data.client[id1]); fflush(stdout);  
  FEATUM_DEBUG(3,"frame size id0 = %i",framedInput->_data.client[id1]->frameLength);
//  int id2 = inputFramer_addClient(framedInput, 64, 2000);

  #ifdef DEBUG_SILDET
  int id2 = framedInput->addClientSecStep(opts->frameStep, opts->frameStep, 0); //n_ll_levels++;
  framedInput->setWindowing(id2,WIN_NONE);
  #endif

  /*------------------------------------end--------------------------------------*/
//printf("h2\n"); fflush(stdout);  
  /***************************** create LLDex ************************************/
  // -> temporary extractor data struct
  int *nHist = (int*)calloc(1,sizeof(int)*n_ll_levels);
  int i;
  for (i=0; i<n_ll_levels; i++) {
    nHist[i] = 10;  // TODO: autocreate from constants? frameLength/etc..?
  }
  cLLDex lldex (*framedInput,n_ll_levels,nHist);
  free(nHist);
  /*------------------------------------end--------------------------------------*/


  #ifdef DEBUG_SILDET
  /***** wave output for debugging purposes.... ****/
  cWaveOutput *waveout = NULL; 
  /*************************************************/
  #endif
  
  /***************************** create LLDs (main lld config) *******************/
     // -> main lld config for automated processing of enabled llds
  // init LLDs (pass LLDex)
    
  cLLDs llds(lldex, 1);

  // setup LLDs : enable, NULL obj (to auto create)

  llds.setupLLD("energy",1,NULL,LLD_LEVEL0);
  llds.setupLLD("fft",1,NULL,LLD_LEVEL0);
  llds.setupLLD("pitch",1,NULL,LLD_LEVEL0);
  llds.setupLLD("mfcc",1,NULL,LLD_LEVEL0);
  llds.setupLLD("time",1,NULL,LLD_LEVEL0);
//  llds.setupLLD("vq",1,NULL,LLD_LEVEL0);  // voice quality
  llds.setupLLD("spectral",1,NULL,LLD_LEVEL0); // spectral features
  
//  llds.setupLLD("cepstrum",1,NULL,LLD_LEVEL0);
//  pLLDcepstralF0 lld_cepstralF0 = (pLLDcepstralF0)LLDs_setupLLD(llds,"cepstralF0",1,NULL,LLD_LEVEL0);
//  pLLDlpc lld_lpc = (pLLDlpc)LLDs_setupLLD(llds,"lpc",1,NULL,LLD_LEVEL0);
//    #ifndef REDUCE

//  #endif
  
//  pLLDlpc lld_lpc = (pLLDlpc)LLDs_setupLLD(llds,"lpc",1,NULL,LLD_LEVEL0);
  
   // ... more ...

  // check dependencies of extractors
  if (llds.checkDependencies() == 0) {
    FEATUM_ERR_FATAL(0,"LLD extractor dependency check failed");
    return exitApp(ERR_CONFIG);
  }
  llds.countEnabled();

  FEATUM_DEBUG(1, "ft_lld_energy = %i",ft_lld_energy);
  FEATUM_DEBUG(1, "ft_lld_fft = %i",ft_lld_fft);
  FEATUM_DEBUG(1, "ft_lld_pitch = %i",ft_lld_pitch);
  FEATUM_DEBUG(1, "ft_lld_mfcc = %i",ft_lld_mfcc);
  FEATUM_DEBUG(1, "ft_lld_time = %i",ft_lld_time);

  /*------------------------------------end--------------------------------------*/   

  /***************************** create featureMemory ****************************/
  // TODO: segfault if capacity >= 19988 !!! ????
  LONG_IDX cap[] = {100000,100000}; // TODO: function to automatically compute capacity based on: a) time (sec.),  b) whole file
  int ids[] = {id1,0};    // -> todo ?  auto assign from framer object
  
//  pFeatureMemory ftMem = (pFeatureMemory)featureMemory_create(NULL, framedInput, ids, (n_ll_levels+nExtraLevels), cap);
//  cFeatureMemory ftMem(FTMEM_FIXED_BUFFER, ftMEM_nLevels);//(n_ll_levels+nExtraLevels));
  cFeatureMemory ftMem(FTMEM_RING_BUFFER, ftMEM_nLevels);//(n_ll_levels+nExtraLevels));
  
/*  if (ftMem == NULL) {
    FEATUM_ERR_FATAL(0,"Cannot create feature memory!");
    return exitApp(ERR_MEMORY);          
  }
*/
  // setup the levels and allocate memory for each level  
  llds.setupFeatureMemLevels( ftMem, *framedInput, ids, cap );
  llds.configureFeatureMemoryNames( ftMem, LLD_LEVEL0 );


//  LLDs_configureFeatureMemoryNames( llds, ftMem, LLD_LEVEL1 );
    
  // llds->nExtractors_enabled;
/*
  LONG_IDX *nelems;
  nelems = (LONG_IDX *)calloc(1,sizeof(LONG_IDX)*(n_ll_levels+nExtraLevels));
  for (i=0; i<n_ll_levels; i++) { //auto create for n ll_levels
    nelems[i] = llds->nExtractors_flushable;  //TODO: true number of flushable extractors for this level  
    FEATUM_DEBUG(3,"nelems[%i]=%i",i,nelems[i]);
  }
  if (featureMemory_memAlloc( ftMem, nelems ) == 0) return;
  free(nelems);
*/
   
  /*------------------------------------end--------------------------------------*/    


  /************************ create differentiator object for LLD *****************/
  cDeltas deltas( 2 ); // two ids for delta and delta delta
//  ( pDeltas obj, pFeatureMemory mem, int id, int olevel, int ilevel, int nFramesContext );

  if (!deltas.setupID( ftMem, 0, LLD0_DE, LLD_LEVEL0, 3)) {
    FEATUM_ERR_FATAL(0,"Failed setting up Delta extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!deltas.setupID( ftMem, 1, LLD0_DEDE, LLD0_DE, 3)) {
    FEATUM_ERR_FATAL(0,"Failed setting up DeltaDelta extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  /*------------------------------------end--------------------------------------*/   

  /***************************** create functionals object ***********************/
  #ifdef ENABLE_FUNCTIONALS
  cFunctionals functs( 9 );
  

  if (!opts->HRF) 
  {
                  // enable all functionals for all hierarchies (>180.000 features!)
  functs.enable("maxmin",NULL);
  functs.enable("mean",NULL);
  functs.enable("nzmean",NULL);
  functs.enable("mmmdist",NULL);
  functs.enable("quartile",NULL);
  functs.enable("percentile",NULL);
  functs.enable("qmean",NULL);
  functs.enable("moments",NULL);
  functs.enable("centroid",NULL);
  functs.enable("regression",NULL);
  functs.enable("peaks",NULL);
  functs.enable("times",NULL);
  functs.enable("segments",NULL);
  functs.enable("dct",NULL);
  } else {

  int functsL0[] = {0,1,2,6,7,8, -1};
  int functsL1[] = {3,4,5, -1};
  
  // enbale the functionals required on the first hierarchy...
  //functs.enable("maxmin",functsL0);
  functs.enable("percentile",NULL);
  functs.enable("mean",functsL0);
  //functs.enable("mmmdist",functsL0);
  //functs.enable("quartile",functsL0);
  functs.enable("moments",functsL0);
  functs.enable("centroid",functsL0);
  functs.enable("regression",functsL0);
  //functs.enable("times",functsL0);
  //functs.enable("segments",functsL0);
  functs.enable("dct",functsL0);

  // enbale the functionals required on the second hierarchy...
  functs.enable("maxmin",functsL1);
  functs.enable("mean",functsL1);
  functs.enable("moments",functsL1);
  functs.enable("quartile",functsL0);
//  functs.enable("centroid",functsL1);
//  functs.enable("regression",functsL1);
  //functs.enable("dct",functsL1);

  }
//  functs.enable("zcr",NULL);
//  functs.enable("mcr",NULL);

    
  // basic functionals
  float funcL0_T = 0.5;
  float funcL0_fs = 0.5;
  //functionals_setupID( pFunctionals obj, pFeatureMemory mem, int id, int olevel, int ilevel, float T, float computeframeSize, int capacity );
  if (!functs.setupID( ftMem, 0, LLD0_FUNC_L0, LLD_LEVEL0, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 0!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functs.setupID( ftMem, 1, LLD0_DE_FUNC_L0, LLD0_DE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 1!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functs.setupID( ftMem, 2, LLD0_DEDE_FUNC_L0, LLD0_DEDE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 2!");
    return exitApp(ERR_MEMORY);                      
  } 

  // hierarchical functionals:
  if (!functs.setupID( ftMem, 3, LLD0_FUNC_L1, LLD0_FUNC_L0, 4.0, 16.0, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functs.setupID( ftMem, 4, LLD0_DE_FUNC_L1, LLD0_DE_FUNC_L0, 4.0, 16.0, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functs.setupID( ftMem, 5, LLD0_DEDE_FUNC_L1, LLD0_DEDE_FUNC_L0, 4.0, 16.0, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor!");
    return exitApp(ERR_MEMORY);                      
  } 

  // full turn, non-hierarchical:
  if (!functs.setupID( ftMem, 6, LLD0_FUNC_L0f, LLD_LEVEL0, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 0!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functs.setupID( ftMem, 7, LLD0_DE_FUNC_L0f, LLD0_DE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 1!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functs.setupID( ftMem, 8, LLD0_DEDE_FUNC_L0f, LLD0_DEDE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 2!");
    return exitApp(ERR_MEMORY);                      
  } 

  // do initialisation of names and count enabled functionals
  functs.finalizeConfiguration( );

  #endif
  /*------------------------------------end--------------------------------------*/   
  
  
  
  
  // TODO: for outputs: for higher level outputs the number of features increases
     // e.g. pitch max, min, etc. 
     // implement a way of configuring higher levels with respect to features in the lower levels (LLD levels are enough)
     
     // requires more info in feature Memory....
     // TODO: redesign of feature Memory:
     /*
        add links between levels, information about data organisation in each level
        timing support
        generalised interface  (no direct access to the memory data!)
          -> two versions: online stream (ringbuffer) and offline (full buffer)
          both version share data structures, only ringbuffer management variables are added for stream version
          
     
     
     */
  /*************************** create output objects *****************************/

  cHtkOutput * htkOutp= NULL;
  if (opts->htkout != NULL) {
    htkOutp = new cHtkOutput(ftMem, htkOutputFile);
    #define nHTKels 6
    sElID htkEls[nHTKels];
    // mfccs:
    htkEls[0] = outputObject(LLD_LEVEL0, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[1] = outputObject(LLD_LEVEL0, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[2] = outputObject(LLD0_DE, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[3] = outputObject(LLD0_DE, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[4] = outputObject(LLD0_DEDE, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[5] = outputObject(LLD0_DEDE, ft_lld_energy, 1, 1, 0, EL_ENABLED);

/*
    htkEls[0] = outputObject(LLD_LEVEL0, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[1] = outputObject(LLD_LEVEL0, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[2] = outputObject(LLD0_DE, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[3] = outputObject(LLD0_DE, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[4] = outputObject(LLD0_DEDE, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[5] = outputObject(LLD0_DEDE, ft_lld_energy, 1, 1, 0, EL_ENABLED);
*/

/*    htkEls[0] = outputObject(LLD_LEVEL0, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[1] = outputObject(LLD_LEVEL0, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[2] = outputObject(LLD_LEVEL0, ft_lld_pitch, 1, 1, 0, EL_ENABLED); // pitch strength
    htkEls[3] = outputObject(LLD0_DE, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[4] = outputObject(LLD0_DE, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[5] = outputObject(LLD0_DE, ft_lld_pitch, 1, 1, 0, EL_ENABLED); // pitch strength
    htkEls[6] = outputObject(LLD0_DEDE, ft_lld_mfcc, 1, -1, 0, EL_ENABLED);
    htkEls[7] = outputObject(LLD0_DEDE, ft_lld_energy, 1, 1, 0, EL_ENABLED);
    htkEls[8] = outputObject(LLD0_DEDE, ft_lld_pitch, 1, 1, 0, EL_ENABLED); // pitch strength
*/
    htkOutp->configure(htkEls, nHTKels);
    
  }


  cCsvOutput * csvOutp = NULL;
  if (opts->csvout != NULL) {
    csvOutp = new cCsvOutput(ftMem, csvOutputFile);
    #define nCSVels 15
//    #define nCSVels 15
    sElID csvEls[nCSVels];
    // energy:
    csvEls[0] = outputObject(LLD_LEVEL0, ft_lld_energy, 0, -1, 0, EL_ENABLED);
    csvEls[1] = outputObject(LLD0_DE, ft_lld_energy, 0, -1, 0, EL_ENABLED);
    csvEls[2] = outputObject(LLD0_DEDE, ft_lld_energy, 0, -1, 0, EL_ENABLED);
    // pitch:
    csvEls[3] = outputObject(LLD_LEVEL0, ft_lld_pitch, 0, -1, 0, EL_ENABLED);
    csvEls[4] = outputObject(LLD0_DE, ft_lld_pitch, 0, -1, 0, EL_ENABLED);
    csvEls[5] = outputObject(LLD0_DEDE, ft_lld_pitch, 0, -1, 0, EL_ENABLED);
    // MFCC:
    csvEls[6] = outputObjectFull(LLD_LEVEL0, ft_lld_mfcc);
    csvEls[7] = outputObjectFull(LLD0_DE, ft_lld_mfcc);    
    csvEls[8] = outputObjectFull(LLD0_DEDE, ft_lld_mfcc);
    // time domain features (zero crossings, etc.)
    csvEls[9] = outputObjectFull(LLD_LEVEL0, ft_lld_time);
    csvEls[10] = outputObjectFull(LLD0_DE, ft_lld_time);
    csvEls[11] = outputObjectFull(LLD0_DEDE, ft_lld_time);

    // spectral
    csvEls[12] = outputObjectFull(LLD_LEVEL0, ft_lld_spectral);
    csvEls[13] = outputObjectFull(LLD0_DE, ft_lld_spectral);
    csvEls[14] = outputObjectFull(LLD0_DEDE, ft_lld_spectral);

    // lpc:
/*
    csvEls[12] = outputObjectFull(LLD_LEVEL0, ft_lld_lpc);
    csvEls[13] = outputObjectFull(LLD0_DE, ft_lld_lpc);
    csvEls[14] = outputObjectFull(LLD0_DEDE, ft_lld_lpc);
*/
   
    csvOutp->configure(csvEls, nCSVels);
    
  }


  #ifdef ENABLE_FUNCTIONALS
// TODO:: bug here... buildNames does not properly build the functional's names, or the level linking is broken!
  cArffOutput * arffOutp = NULL;
  if (opts->arffout != NULL) {
    arffOutp = new cArffOutput(ftMem, arffOutputFile);
    #define nARFFelsA 12
    sElID arffEls[nARFFelsA];
    arffEls[0] = outputObjectFull(LLD0_FUNC_L0, ft_lld_energy);
    arffEls[1] = outputObjectFull(LLD0_DE_FUNC_L0, ft_lld_energy);
    arffEls[2] = outputObjectFull(LLD0_DEDE_FUNC_L0, ft_lld_energy);

    arffEls[3] = outputObjectFull(LLD0_FUNC_L0, ft_lld_pitch);
    arffEls[4] = outputObjectFull(LLD0_DE_FUNC_L0, ft_lld_pitch);
    arffEls[5] = outputObjectFull(LLD0_DEDE_FUNC_L0, ft_lld_pitch);

    arffEls[6] = outputObjectFull(LLD0_FUNC_L0, ft_lld_mfcc);
    arffEls[7] = outputObjectFull(LLD0_DE_FUNC_L0, ft_lld_mfcc);
    arffEls[8] = outputObjectFull(LLD0_DEDE_FUNC_L0, ft_lld_mfcc);

    arffEls[9] = outputObjectFull(LLD0_FUNC_L0, ft_lld_time);
    arffEls[10] = outputObjectFull(LLD0_DE_FUNC_L0, ft_lld_time);
    arffEls[11] = outputObjectFull(LLD0_DEDE_FUNC_L0, ft_lld_time);
        
    arffOutp->configure(arffEls, nARFFelsA);
    
  }
  
  // hierarchical functionals:
  cArffOutput * arffOutpHR = NULL;  
  if (opts->arffoutHR != NULL) {
    arffOutpHR = new cArffOutput(ftMem, opts->arffoutHR, 1);
    #define nARFFelsB 15
    sElID arffEls[nARFFelsB];
    // mfccs:
    
    arffEls[0] = outputObjectFull(LLD0_FUNC_L1, ft_lld_energy);
    arffEls[1] = outputObjectFull(LLD0_DE_FUNC_L1, ft_lld_energy);
    arffEls[2] = outputObjectFull(LLD0_DEDE_FUNC_L1, ft_lld_energy);

    arffEls[3] = outputObjectFull(LLD0_FUNC_L1, ft_lld_pitch);
    arffEls[4] = outputObjectFull(LLD0_DE_FUNC_L1, ft_lld_pitch);
    arffEls[5] = outputObjectFull(LLD0_DEDE_FUNC_L1, ft_lld_pitch);

    arffEls[6] = outputObjectFull(LLD0_FUNC_L1, ft_lld_mfcc);
    arffEls[7] = outputObjectFull(LLD0_DE_FUNC_L1, ft_lld_mfcc);
    arffEls[8] = outputObjectFull(LLD0_DEDE_FUNC_L1, ft_lld_mfcc);

    arffEls[9] = outputObjectFull(LLD0_FUNC_L1, ft_lld_time);
    arffEls[10] = outputObjectFull(LLD0_DE_FUNC_L1, ft_lld_time);
    arffEls[11] = outputObjectFull(LLD0_DEDE_FUNC_L1, ft_lld_time);

    arffEls[12] = outputObjectFull(LLD0_FUNC_L1, ft_lld_spectral);
    arffEls[13] = outputObjectFull(LLD0_DE_FUNC_L1, ft_lld_spectral);
    arffEls[14] = outputObjectFull(LLD0_DEDE_FUNC_L1, ft_lld_spectral);
        
    arffOutpHR->configure(arffEls, nARFFelsB);
    arffOutpHR->setClassesList(opts->arffclasses);
  }

  // full turn, non hierarchical:
  cArffOutput * arffOutpFull = NULL;
  cArffOutput * arffOutpFullA = NULL;
  cArffOutput * arffOutpFullV = NULL;
  cLsvmOutput * svmOutputA = NULL;
  cLsvmOutput * svmOutputV = NULL;
  cSvmPredict * svmPredA = NULL;
  cSvmPredict * svmPredV = NULL;

    #define nARFFelsC 15
    sElID arffEls[nARFFelsC];
    
    int l0=LLD0_FUNC_L0f;
    int l1=LLD0_DE_FUNC_L0f;
    int l2=LLD0_DEDE_FUNC_L0f;
    
    if (opts->HRF) {
     l0 =    LLD0_FUNC_L1;
     l1 =    LLD0_DE_FUNC_L1;      
     l2 =    LLD0_DEDE_FUNC_L1;        
    }
                   
    arffEls[0] = outputObjectFull(l0, ft_lld_energy);
    arffEls[1] = outputObjectFull(l1, ft_lld_energy);
    arffEls[2] = outputObjectFull(l2, ft_lld_energy);

    arffEls[3] = outputObjectFull(l0, ft_lld_pitch);
    arffEls[4] = outputObjectFull(l1, ft_lld_pitch);
    arffEls[5] = outputObjectFull(l2, ft_lld_pitch);

    arffEls[6] = outputObjectFull(l0, ft_lld_mfcc);
    arffEls[7] = outputObjectFull(l1, ft_lld_mfcc);
    arffEls[8] = outputObjectFull(l2, ft_lld_mfcc);

    arffEls[9] = outputObjectFull(l0, ft_lld_time);
    arffEls[10] = outputObjectFull(l1, ft_lld_time);
    arffEls[11] = outputObjectFull(l2, ft_lld_time);

    // spectral
    arffEls[12] = outputObjectFull(l0, ft_lld_spectral);
    arffEls[13] = outputObjectFull(l1, ft_lld_spectral);
    arffEls[14] = outputObjectFull(l2, ft_lld_spectral);

    if (opts->arffoutFull != NULL) {
      arffOutpFull = new cArffOutput(ftMem, opts->arffoutFull, 1);
      arffOutpFull->configure(arffEls, nARFFelsC);
      arffOutpFull->setClassesList(opts->arffclasses);
    }
    if (opts->arffoutFullA != NULL) {
      arffOutpFullA = new cArffOutput(ftMem, opts->arffoutFullA, 1);
      arffOutpFullA->loadSelection(opts->svmpredfselA);      
      arffOutpFullA->configure(arffEls, nARFFelsC);
      arffOutpFullA->setClassesList(opts->arffclassesA);
    }
    if (opts->arffoutFullV != NULL) {
      arffOutpFullV = new cArffOutput(ftMem, opts->arffoutFullV, 1);
      arffOutpFullV->loadSelection(opts->svmpredfselV);
      arffOutpFullV->configure(arffEls, nARFFelsC);
      arffOutpFullV->setClassesList(opts->arffclassesV);
    }

    if (opts->svmmodelA != NULL) {
        printf("loading svm model for arousal ...\n"); fflush(stdout);
        svmPredA = new cSvmPredict( ftMem, opts->svmmodelA, opts->svmscaleA );
        svmPredA->loadSelection(opts->svmpredfselA);
        svmPredA->configure(arffEls, nARFFelsC);
    }
    if (opts->svmmodelV != NULL) {
        printf("loading svm model for valence ...\n"); fflush(stdout);
        svmPredV = new cSvmPredict( ftMem, opts->svmmodelV, opts->svmscaleV );
        svmPredV->loadSelection(opts->svmpredfselV);
        svmPredV->configure(arffEls, nARFFelsC);
    }
    if (opts->svmoutA != NULL) {
        svmOutputA = new cLsvmOutput ( ftMem, opts->svmoutA, 1 );
        svmOutputA->loadSelection(opts->svmpredfselA);        
        svmOutputA->configure(arffEls, nARFFelsC);
    }
    if (opts->svmoutV != NULL) {
        svmOutputV = new cLsvmOutput ( ftMem, opts->svmoutV, 1 );
        svmOutputV->loadSelection(opts->svmpredfselV);
        svmOutputV->configure(arffEls, nARFFelsC);
    }

  //}
  #endif


/*  
  pWaveOutput waveOut = NULL;
  if (opts->waveout) {
//   pWaveOutput waveOutput_create( pWaveOutput obj, char *filename, long sampleRate, int bits, int channels )
    waveOut = waveOutput_create(waveOut, opts->waveout, framedInput->sampleRate, framedInput->sampleType, framedInput->nChan);
  }
  */                   

  /*------------------------------------end--------------------------------------*/

  FEATUM_DEBUG(4,"entering processing loop");
  FEATUM_MESSAGE(2,"entering processing loop\n");
  signal(SIGINT, INThandler);
  
  cPcmBuffer * frame1 = NULL;
  cPcmBuffer * frame2 = NULL;
  cSilenceDetector silDet(lldex,opts->sil_thresh);
  
  #ifdef DEBUG_SILDET
   if (!opts->sildet) if (waveout == NULL) waveout = new cWaveOutput( "dbg_out.wav", 16000, SAMPLETYPE_I16, 1 );
  #endif 
  
  int okay = 1;
  int fl = 0; int fnr  = 0;
  int turntime = 0;
  int speaking = 0;
  cFtIdentifier meanE(LLD0_FUNC_L0f);
  cFtIdentifier meanF0(LLD0_FUNC_L0f); 
  cFtIdentifier meanF0n(LLD0_FUNC_L0f); 
  cFtIdentifier meanF0s(LLD0_FUNC_L0f);  
  cFtIdentifier frameE(LLD_LEVEL0);
  
  while( okay ) {
    int flag = 0;

    FEATUM_DEBUG(9,"calling getFrame (id1=%i)",id1);
    if (framedInput->getFrame(id1, &frame1)) {
      flag = 1;         
    }
    #ifdef DEBUG_SILDET
    framedInput->getFrame(id2, &frame2);
    #endif
    
    if (frame1 == NULL) FEATUM_DEBUG(9,"FRAME ==  NULL\n");

      
   
    // add frame to short term ringbuffer
    if (lldex.addFrame(LLD_LEVEL0, *frame1 )) { 
                             
      // call the extractors on all levels.... //+timestamps...
      // TODO: consider extractors priority... i.e. call those with lower numerical value for priority first
      llds.callExtractors(LLD_LEVEL0);  //level
      //TODO:: Flush condition + FlushToFTMem if true
      llds.flushToFtMem(ftMem,LLD_LEVEL0); //level

      deltas.autoCompute();
      int flag = 0;
      #ifdef ENABLE_FUNCTIONALS
      flag += functs.computeCurrentOnly( 0 );
      flag += functs.computeCurrentOnly( 1 );
      flag += functs.computeCurrentOnly( 2 );
      #endif
      
      if (htkOutp != NULL) htkOutp->saveFrame();
      if (csvOutp != NULL) csvOutp->saveFrame();
    
      #ifdef ENABLE_FUNCTIONALS
      if (flag)
        if (arffOutp != NULL) arffOutp->saveFrameExt(NULL,opts->infile);
 
      if (opts->displayenergy) {
        if (!frameE.found()) { ftMem.findFeature( "logEnergy", frameE ); }
        FLOAT_TYPE_FTMEM *fen = ftMem.getFeaturesByID( frameE, 0 );
        printf("frameE = %f\n",*fen);
      }
      
      // live energy thresholding...
      //#ifdef LIVE_REC
//        printf("here9\n"); fflush(stdout);
      int sres = silDet.process(LLD_LEVEL0);
      if ((opts->sildet)&&(sres == 2)) { // begin of turn (10 frames before current position)
        printf("** detected turn start [t=%f]--\n",(*frame1)._data.timestamp); fflush(stdout);
          // ++AMQ++ send turn start
        if (opts->HRF) {
          functs.setFrameStart( 3, -10 );
          functs.setFrameStart( 4, -10 );
          functs.setFrameStart( 5, -10 );
        } else {
          functs.setFrameStart( 6, -10 );
          functs.setFrameStart( 7, -10 );
          functs.setFrameStart( 8, -10 );
        }        
        turntime = 0;
        speaking = 1;
        #ifdef DEBUG_SILDET
        // open output wave file
        if (waveout == NULL) waveout = new cWaveOutput( "dbg_out.wav", 16000, SAMPLETYPE_I16, 1 );
        #endif
      }
      
      #ifdef DEBUG_SILDET
      // save pcm frame  (non overlap...! non pre emph, non windowed!!!)
      if (waveout != NULL) waveout->writeFrame(*frame2);
      #endif
      if (speaking == 1) turntime++;
                                          
      if ((opts->sildet)&&(( sres == 1 )||(turntime>1000))) {  // ||(turnlength > 10s)
        printf("-- detected turn end [t=%f] [l=%i]--\n",(*frame1)._data.timestamp,turntime); fflush(stdout);
        
        #ifdef DEBUG_SILDET
        // close output wave file, increment counter
        delete waveout; waveout = NULL;
        #endif
        if (opts->HRF) {
          functs.computeNow(  0, 1 );
          functs.computeNow(  1, 1 );
          functs.computeNow(  2, 1 );
          functs.computeNow(  3, 1 );
          functs.computeNow(  4, 1 );
          functs.computeNow(  5, 1 );
        } else {        
          functs.computeNow(  6, 1 );
          functs.computeNow(  7, 1 );
          functs.computeNow(  8, 1 );
        }
        speaking = 0;
        
        
        // get voicing and energy parameters + mean pitch
           // warning: the names are not unique, they occur on other levels (0.5s functionals...)
        if (!meanE.found()) { ftMem.findFeature( "logEnergy-max", meanE ); }
        if (!meanF0.found()) { ftMem.findFeature( "F0freq-nzmean", meanF0 ); }
        if (!meanF0n.found()) { ftMem.findFeature( "F0freq-nzN", meanF0n ); }
        if (!meanF0s.found()) { ftMem.findFeature( "F0strength-percentile95", meanF0s ); }
//        printf("h4\n"); fflush(stdout);
        FLOAT_TYPE_FTMEM *en = ftMem.getFeaturesByID( meanE, 0 );
        FLOAT_TYPE_FTMEM *f0 = ftMem.getFeaturesByID( meanF0, 0 );
        FLOAT_TYPE_FTMEM *f0n = ftMem.getFeaturesByID( meanF0n, 0 );
        FLOAT_TYPE_FTMEM *f0s = ftMem.getFeaturesByID( meanF0s, 0 );
        if (en != NULL) printf(" MAX E = %f\n",*en); fflush(stdout);
        if (f0 != NULL) printf(" NZMEAN F0 = %f\n",*f0 * MAX_PITCH); fflush(stdout);
        if (f0n != NULL) printf(" voiced \% = %f\n",*f0n);fflush(stdout);
        if (f0s != NULL) printf(" voicing = %f\n",*f0s);fflush(stdout);

       
        if ( (f0 != NULL)&&(f0n != NULL)&&(en!= NULL)&&(f0s!=NULL)
             &&(turntime > 100)
           //&& (*f0* MAX_PITCH > 40.0) 
           && (*f0* MAX_PITCH < 495.0) 
            && 
            ( 
               //(*f0n > 0.001) && 
               (*f0s > 0.40) && (*en > -11.0)  )
            || ( (*en > -9.0) && (*f0s > 0.5) ) 
            ) {


          printf("\n !VOICE!\n");
          // classify:
          if (svmPredA != NULL) {
            svmPredA->setIdxToLastFrame();
            printf(">>>> arousal: "); fflush(stdout);
            svmPredA->processNextFrame(); fflush(stdout);
          }
          if (svmPredV != NULL) {
            svmPredV->setIdxToLastFrame();
            printf(">>>> valence: "); fflush(stdout);
            svmPredV->processNextFrame(); fflush(stdout);
          }
          printf("\n");
          float a,v;
          a = svmPredA->getLastResult();
          v = svmPredV->getLastResult();

          printf("<emotion>\n");
          printf("  <dimensions set=\"valenceArousalPotency\">\n");
          printf("    <arousal value=\"%f\">\n",a);
          printf("    <valence value=\"%f\">\n",v);
          printf("    <potency value=\"%f\">\n",0.0);
          printf("  </dimensions>\n");
          printf("</emotion>\n\n");
          fflush(stdout);
/*
          <emotion>

    <dimensions set="valenceArousalPotency">

        <arousal value="0.3"/>

        <valence value="0.9"/>

        <potency value="0.8"/>

    </dimensions>

</emotion>
*/
        }    else {
          // TODO: skip frame...
        }                 
    
        turntime = 0;
        if (svmOutputA != NULL) {
          svmOutputA->saveFrameExt(opts->labelA);
        }
        if (svmOutputV != NULL) {
          svmOutputV->saveFrameExt(opts->labelV);
        }
        if (arffOutpFull != NULL) {
          arffOutpFull->saveFrameExt(opts->arfflabel,opts->infile);
        }
        if (arffOutpFullA != NULL) {
          arffOutpFullA->saveFrameExt(opts->labelA,opts->infile);
        }
        if (arffOutpFullV != NULL) {
          arffOutpFullV->saveFrameExt(opts->labelV,opts->infile);
        }


      }
      #endif
      //#endif
      
    }
    
    //functionals_computeCurrentOnly( functs, 0 );
    
    #ifdef LIVE_REC
    if (featum_quit && (liveIn != NULL)) {
      if (!fl) printf("Stopped recording from device ... waiting for data in buffer to be written to disk.\n"); 
      fl = 1;
      liveIn->setStatus(0);
      // stop recording, then wait for buffer clear
    } else 
    #endif
    if (featum_quit) {
      flag = 0;  // exit immediately       
    }
    
    okay = flag;
    FEATUM_DEBUG(8,"LOOP featum_quit = %i",featum_quit);
  }
  //waveOutput_destroy(waveOut);
  #ifdef LIVE_REC
  delete liveIn;
  #endif

  // compute deltas at the end properly....
  deltas.autoComputeLast();
  // save last frames
  if (htkOutp != NULL) {
    while (htkOutp->saveFrame()) {}
  }
  
  #ifdef DEBUG_SILDET
  if (!opts->sildet) { delete waveout; waveout = NULL; }
  #endif
    
  #ifdef ENABLE_FUNCTIONALS
  functs.computeNow(  0, 2 );
  functs.computeNow(  1, 2 );
  functs.computeNow(  2, 2 );
  functs.computeNow(  3, 1 );
  functs.computeNow(  4, 1 );
  functs.computeNow(  5, 1 );
  functs.computeNow(  6, 1 );
  functs.computeNow(  7, 1 );
  functs.computeNow(  8, 1 );
  
  //functionals_computeNow( functs, 1, 1 );
  //  FEATUM_DEBUG(1,"0: %i, 1: %i",(long)ftMem->level[2].ft[0][0], (long)ftMem->level[2].ft[0][1]);

  if (arffOutpHR != NULL) arffOutpHR->saveFrameExt(opts->arfflabel,opts->infile);
  if (!opts->sildet) if (arffOutpFull != NULL) arffOutpFull->saveFrameExt(opts->arfflabel,opts->infile);
  if (!opts->sildet) if (arffOutpFullA != NULL) arffOutpFullA->saveFrameExt(opts->labelA,opts->infile);
  if (!opts->sildet) if (arffOutpFullV != NULL) arffOutpFullV->saveFrameExt(opts->labelV,opts->infile);

  if (svmOutputA != NULL) {
    if (!opts->sildet) svmOutputA->saveFrameExt(opts->labelA);
    delete svmOutputA;
  }
  if (svmOutputV != NULL) {
    if (!opts->sildet) svmOutputV->saveFrameExt(opts->labelV);
    delete svmOutputV;
  }

  if (svmPredA != NULL) {
    if (!opts->sildet) printf("arousal: ");
    if (!opts->sildet) svmPredA->processNextFrame();
    delete svmPredA;
  } 
  if (svmPredV != NULL) {
    if (!opts->sildet) printf("valence: ");
    if (!opts->sildet) svmPredV->processNextFrame();
    delete svmPredV;
  }

  if (arffOutp != NULL) delete arffOutp;
  if (arffOutpHR != NULL) delete arffOutpHR;
  if (arffOutpFull != NULL) delete arffOutpFull;
  if (arffOutpFullA != NULL) delete arffOutpFullA;
  if (arffOutpFullV != NULL) delete arffOutpFullV;
  #endif  
  
//  frame2 = pcmBufferFree(frame2,0);
  
  if (htkOutp != NULL) delete htkOutp;
  if (csvOutp != NULL) delete csvOutp;

//  if (frame1 != NULL) delete frame1;
  
// free LLDex, LLDs


  
//FOR TESTING: read out featureMem and print to screen

/*
  int n;
  for (n=0; n< ftMem->level[LLD_LEVEL0].curIdx; n++) {
    pFtMemFrame el = NULL;
    el = featureMemory_getFrame(ftMem, LLD_LEVEL0, (LONG_IDX)n  ); //FT_LLD_ENERGY
    if (el != NULL) {
      if (el->frame[ft_lld_pitch]->values[1] >= 0.1) 
        printf("F0(%i, %i, %f): %f\n",n,el->timestamp,el->timestampSec,el->frame[ft_lld_pitch]->values[0]);
    } else
      printf("NULL\n");  
    //for (i=0; i<el->nValues; i++) {
  }
 */ 
  
/*
  printf(" IDX level 1: %i \n",ftMem->level[1].curIdx);
  for (n=0; n< ftMem->level[1].curIdx; n++) {
    pFtMemFrame el = NULL;
    el = featureMemory_getFrame(ftMem, 1, (LONG_IDX)n  ); //FT_LLD_ENERGY
    if (el != NULL) 
      printf("MAX(%i, %i, %f): %f  MIN: %f\n",n,el->timestamp,el->timestampSec,el->frame[ft_lld_energy]->values[functs->en_max-1],el->frame[ft_lld_energy]->values[functs->en_min-1]);
    else
      printf("NULL\n");  
    //for (i=0; i<el->nValues; i++) {
  }*/
  
  
  #ifdef ENABLE_FUNCTIONALS
  #endif

// TESTING: print featureMem names:
  //printf("Feature0: %s\n",featureMemory_getFeatureName(ftMem,0,ft_lld_energy,0));
  //printf("Feature1: %s\n",featureMemory_getFeatureName(ftMem,0,ft_lld_energy,1));

               // + featurenames...
               // STEP2: implement output module

  #ifdef ENABLE_PROFILER
  profiler_printTime(&profiler);
  #endif
  
// free featureMem

  FEATUM_DEBUG(5,"destroying inputFramer");  
  delete framedInput;
  FEATUM_DEBUG(5,"destroying wave file input (%s)",inputfile);  
  delete waveIn;
  

/* ---end of test code---- */


  // parse options (commandline + config file)
  
  
  // read and process feature selection file
  
  // set up data input (frames, wavefile, streams etc.)
  
  // register feature extractors
  
  // loop while data is available
//  while (isDataAvail)
//  {
        // REQD:  ftex struct history
        //        2d  ftex struct -> history and LLD framesize
        
          // read frame & clear ftext struct
          
          // process frame, level 1 (FFT, zero x-ing, energy, etc.)
          
          // proces frame, level 2 (pitch, mfcc, spectral)
          
          // process frame, level 3 (formant, lp, plp, etc.)
          
          // (process, level 4  [generation of new features, e.g. log/sin/exp, etc.])

          // add to global feature memory (LARGE!!)
          // -> global feature memory is responsible for differentials
          //   -> so update them every time the add method is called
          
          // check for complete frames in glob. ft. mem and save raw features
          // **** -> out LLD  [interface to real time post processors & data saving to file]
          
          // check output framing & compute functionals from LLD at frame end
          // (functionals class that uses global featuremem as input)
          
          // save output functionals frame
          
          // *** -> out functionals.. functional frame rate should not be fixed!
          //                           -> an input stream (or file) should be able to control the frame borders on-the-fly
//  }

  // cleanup        
  return exitApp(NO_ERROR);
}
#undef FUNCTION

