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
#include "wave-output.h"
#include "wave-input.h"
#ifdef LIVE_REC
#include "live-input.h"
#endif
#include "pcm-process.h"
#include "input-framer.h"
#include "option-parser.h"
#include "LLDex.h"
#include "LLDs.h"
#include "functionals.h"
#include "deltas.h"
#include "HTKoutput.h"
#include "CSVoutput.h"
#include "ARFFoutput.h"

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
#define EL_ENABLED 1
#define EL_DISABLED 0
sElID outputObject( int level, int el, int nStart, int nEnd, int n, int enabled )
{
  sElID ret;
  ret.level = level;
  ret.el = el;
  ret.nStart = nStart;
  ret.nEnd = nEnd;
  ret.n = n;
  ret.enabled = enabled;  
  return ret;
}

sElID outputObjectFull( int level, int el )
{
  sElID ret;
  ret.level = level;
  ret.el = el;
  ret.nStart = 0;
  ret.nEnd = -1;
  ret.n = 0;
  ret.enabled = 1;  
  return ret;
}
*/
/*********************************/



/***************** options setup ***********************/
pOptions setupOptions( pOptionParser obj ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_
  if (obj == NULL) return;

  // add your parameters:::  (don't forget to define variables in the struct in option-parser.h !!)
  optionParser_addBoolean( obj, "help", 'h', &(obj->opt.help), 0, "Display this usage message" );
  optionParser_addFloat( obj, "frameSize", 'S', &(obj->opt.frameSize), 0.032, "frame size in seconds", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addFloat( obj, "frameStep", 's', &(obj->opt.frameStep), 0.01, "frame step in seconds", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addFloat( obj, "preEmphasis", 'p', &(obj->opt.preEmphasis), 0.97, "1st order pre emphasis filter coefficient", MANDATORY_ARG, OPTIONAL_PARAM);
  #ifndef LIVE_REC
  optionParser_addPchar( obj, "infile", 'i', &(obj->opt.infile), NULL, "input WAVE file", MANDATORY_ARG, MANDATORY_PARAM);
  #else
  optionParser_addPchar( obj, "infile", 'i', &(obj->opt.infile), NULL, "input WAVE file", MANDATORY_ARG, OPTIONAL_PARAM);
  #endif
  optionParser_addPchar( obj, "htkout", 'o', &(obj->opt.htkout), NULL, "output HTK file (mfcc only)", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addPchar( obj, "csvout", 'c', &(obj->opt.csvout), NULL, "output CSV file (low-level)", MANDATORY_ARG, OPTIONAL_PARAM);
  #ifdef ENABLE_FUNCTIONALS
  optionParser_addPchar( obj, "arffout", 'a', &(obj->opt.arffout), NULL, "output ARFF file (functionals over 0.5s frames)", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addPchar( obj, "arffoutFull", 'f', &(obj->opt.arffoutFull), NULL, "output ARFF file (functionals over full input file)", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addPchar( obj, "arffoutHR", 'F', &(obj->opt.arffoutHR), NULL, "output ARFF file (hierarchical functionals over full input file)", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addPchar( obj, "arfflabel", 'L', &(obj->opt.arfflabel), NULL, "label for -f and -h options", MANDATORY_ARG, OPTIONAL_PARAM);
  #endif
  optionParser_addPchar( obj, "waveout", 'w', &(obj->opt.waveout), NULL, "wave file output of pcm stream", MANDATORY_ARG, OPTIONAL_PARAM);
  #ifdef LIVE_REC
  optionParser_addInt( obj, "device", 'd', &(obj->opt.device), -1, "audio device id to record from (see -l) [-1 for system default]", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addInt( obj, "samplerate", 'r', &(obj->opt.samplerate), 16000, "set recording sample rate", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addInt( obj, "bits", 'b', &(obj->opt.bits), 16, "set bits/sample", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addInt( obj, "channels", 'c', &(obj->opt.channels), 1, "set no. of channels", MANDATORY_ARG, OPTIONAL_PARAM);
  #endif
  
//void optionParser_addString( pOptionParser obj, char *name, char abbr, char *val, char *dflt, char *info, int len,  int optional_arg, int mandatory );  
  //-----------------------

  pOptions opts = optionParser_doParse( obj );
  if ((opts == NULL) || (opts->help)) {
    optionParser_showUsage( obj );
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
  pOptionParser options = optionParser_create( NULL, argc, argv, NULL );
  pOptions opts = setupOptions( options );
  if (!opts) return exitApp(ERR_CMDLINE);
  optionParser_globaliseOptions( options );

  #ifdef ENABLE_PRINT_OPTIONS
  debug_printOptions( opts );
  #endif
  
  //  char *inputfile = "great.wav";
  //  char *htkOutputFile = "great.htk";
  char *inputfile = opts->infile;
  #ifndef LIVE_REC
  if (inputfile == NULL) {
    FEATUM_ERROR_FATAL(0,"Live recording not supported, you must specify an input file using the -i option!");
    return exitApp(ERR_CMDLINE);   
  }
  #endif
  
  char *htkOutputFile = opts->htkout;
  char *csvOutputFile = opts->csvout;
  #ifdef ENABLE_FUNCTIONALS
  char *arffOutputFile = opts->arffout;  
  #endif
  
 
  /***************************** wave file input *****************************/    
  pWaveInput waveIn=NULL;

  #ifdef LIVE_REC
  pLiveInput liveIn=NULL;
  if (opts->infile == NULL) { // setup live audio input
    FEATUM_DEBUG(4,"opening live input audio: %i Hz, %i bit, %i channel(s)",opts->samplerate, opts->bits, opts->channels);
    if (!(liveIn = liveInput_create( liveIn, NULL, opts->device, opts->buffersize, opts->samplerate, pcmBitsToSampleType(opts->bits,BYTEORDER_LE,opts->useFloat32), opts->channels, 1))) {
      return exitApp(ERR_INPUT);
    }
  } else {
  #endif
    FEATUM_DEBUG(4,"opening wave file %s as input",opts->infile);  //PCM_PREBUFFER, PCM_POSTBUFFER
        printf("bs: %i\n",opts->buffersize);
    if (!(waveIn = waveInput_create( waveIn, opts->infile,  opts->buffersize  ))) {
      return exitApp(ERR_INPUT);
    }

    FEATUM_DEBUG(4,"wave file %s was sucessfully opened",opts->infile);
    FEATUM_DEBUG(5,"sample rate of wave file: %i\n",waveInput_getSampleRate(waveIn));
  #ifdef LIVE_REC
  }
  #endif
  /*------------------------------------end--------------------------------------*/
  

  /***************************** create input framer *****************************/    
  pInputFramer framedInput = NULL;
  if (waveIn != NULL) {
    framedInput = inputFramer_create(audioStream_create(waveIn,TYPE_WAVEINPUT));
  } 
  #ifdef LIVE_REC
  else {
    framedInput = inputFramer_create(audioStream_create(liveIn,TYPE_LIVEINPUT));
  }
  #endif
  inputFramer_setAudioParameters(framedInput, 1, WORK_SAMPLETYPE, MEMORGA_INTERLV);
  int n_ll_levels = 0;   // low level levels (see below)
  //int nExtraLevels = 3;  // levels for functionals, deltas, etc.

  // add framer clients:
  int id1 = inputFramer_addClientSecStep(framedInput, opts->frameSize, opts->frameStep, 1); n_ll_levels++;
  inputFramer_setPreEmphasis(framedInput, id1, opts->preEmphasis);
  FEATUM_DEBUG(3,"frame size id0 = %i",framedInput->client[0]->frameLength);
//  int id2 = inputFramer_addClient(framedInput, 64, 2000);

  /*------------------------------------end--------------------------------------*/
  //return;
  


  /***************************** create LLDex ************************************/
  // -> temporary extractor data struct
  int *nHist = (int*)calloc(1,sizeof(int)*n_ll_levels);
  int i;
  for (i=0; i<n_ll_levels; i++) {
    nHist[i] = 10;  // TODO: autocreate from constants? frameLength/etc..?
  }
  pLLDex lldex = (pLLDex)LLDex_create(NULL,framedInput,n_ll_levels,nHist);
  free(nHist);
  /*------------------------------------end--------------------------------------*/
  
  
  /***************************** create LLDs (main lld config) *******************/
     // -> main lld config for automated processing of enabled llds
  // init LLDs (pass LLDex)
  pLLDs llds = (pLLDs)LLDs_create(NULL, lldex, 1, 1);
  
  // setup LLDs : enable, NULL obj (to auto create)
  pLLDenergy lld_energy = (pLLDenergy)LLDs_setupLLD(llds,"energy",1,NULL,LLD_LEVEL0);
  pLLDfft lld_fft = (pLLDfft)LLDs_setupLLD(llds,"fft",1,NULL,LLD_LEVEL0);
  pLLDpitch lld_pitch = (pLLDpitch)LLDs_setupLLD(llds,"pitch",1,NULL,LLD_LEVEL0);
  pLLDmfcc lld_mfcc = (pLLDmfcc)LLDs_setupLLD(llds,"mfcc",1,NULL,LLD_LEVEL0);
  pLLDtime lld_time = (pLLDtime)LLDs_setupLLD(llds,"time",1,NULL,LLD_LEVEL0);
  pLLDcepstrum lld_cepstrum = (pLLDcepstrum)LLDs_setupLLD(llds,"cepstrum",1,NULL,LLD_LEVEL0);
//  pLLDcepstralF0 lld_cepstralF0 = (pLLDcepstralF0)LLDs_setupLLD(llds,"cepstralF0",1,NULL,LLD_LEVEL0);
//  pLLDlpc lld_lpc = (pLLDlpc)LLDs_setupLLD(llds,"lpc",1,NULL,LLD_LEVEL0);
//    #ifndef REDUCE

//  #endif
  
//  pLLDlpc lld_lpc = (pLLDlpc)LLDs_setupLLD(llds,"lpc",1,NULL,LLD_LEVEL0);
  
   // ... more ...
    
  // check dependencies of extractors
  if (LLDs_checkDependencies( llds ) == 0) {
    FEATUM_ERR_FATAL(0,"LLD extractor dependency check failed");
    return exitApp(ERR_CONFIG);
  }
  LLDs_countEnabled( llds );
        FEATUM_DEBUG(1, "ft_lld_energy = %i",ft_lld_energy);
  FEATUM_DEBUG(1, "ft_lld_fft = %i",ft_lld_fft);
    FEATUM_DEBUG(1, "ft_lld_pitch = %i",ft_lld_pitch);
      FEATUM_DEBUG(1, "ft_lld_mfcc = %i",ft_lld_mfcc);
        FEATUM_DEBUG(1, "ft_lld_time = %i",ft_lld_time);

  /*------------------------------------end--------------------------------------*/   
   
   
  /***************************** create featureMemory ****************************/
  // TODO: segfault if capacity >= 19988 !!! ????
  LONG_IDX cap[] = {200000,20000}; // TODO: function to automatically compute capacity based on: a) time (sec.),  b) whole file
  int ids[] = {id1,0};    // -> todo ?  auto assign from framer object
  
//  pFeatureMemory ftMem = (pFeatureMemory)featureMemory_create(NULL, framedInput, ids, (n_ll_levels+nExtraLevels), cap);
  pFeatureMemory ftMem = (pFeatureMemory)featureMemory_create(NULL, FTMEM_FIXED_BUFFER, ftMEM_nLevels);//(n_ll_levels+nExtraLevels));
  if (ftMem == NULL) {
    FEATUM_ERR_FATAL(0,"Cannot create feature memory!");
    return exitApp(ERR_MEMORY);          
  }

  // setup the levels and allocate memory for each level  

  LLDs_setupFeatureMemLevels( llds, ftMem, framedInput, ids, cap );
  LLDs_configureFeatureMemoryNames( llds, ftMem, LLD_LEVEL0 );
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
  pDeltas deltas = deltas_create( NULL, 2 ); // two ids for delta and delta delta
//  ( pDeltas obj, pFeatureMemory mem, int id, int olevel, int ilevel, int nFramesContext );
  if (!deltas_setupID( deltas, ftMem, 0, LLD0_DE, LLD_LEVEL0, 3)) {
    FEATUM_ERR_FATAL(0,"Failed setting up Delta extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!deltas_setupID( deltas, ftMem, 1, LLD0_DEDE, LLD0_DE, 3)) {
    FEATUM_ERR_FATAL(0,"Failed setting up DeltaDelta extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  /*------------------------------------end--------------------------------------*/   

  
  /***************************** create functionals object ***********************/
  #ifdef ENABLE_FUNCTIONALS
  pFunctionals functs = functionals_create( NULL, 9 );
  // enbale the functionals required...
  functionals_enable( functs, "maxmin", NULL );
  functionals_enable( functs, "median", NULL );
  functionals_enable( functs, "mean", NULL );
  functionals_enable( functs, "stddev", NULL );

  // do initialisation of names and count enabled functionals
  functionals_finalizeConfiguration( functs );
  
  // basic functionals
  float funcL0_T = 0.5;
  float funcL0_fs = 0.5;
  //functionals_setupID( pFunctionals obj, pFeatureMemory mem, int id, int olevel, int ilevel, float T, float computeframeSize, int capacity );
  if (!functionals_setupID( functs, ftMem, 0, LLD0_FUNC_L0, LLD_LEVEL0, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 0!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functionals_setupID( functs, ftMem, 1, LLD0_DE_FUNC_L0, LLD0_DE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 1!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functionals_setupID( functs, ftMem, 2, LLD0_DEDE_FUNC_L0, LLD0_DEDE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 2!");
    return exitApp(ERR_MEMORY);                      
  } 

  // hierarchical functionals:
  if (!functionals_setupID( functs, ftMem, 3, LLD0_FUNC_L1, LLD0_FUNC_L0, 4.0, 16.0, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functionals_setupID( functs, ftMem, 4, LLD0_DE_FUNC_L1, LLD0_DE_FUNC_L0, 4.0, 16.0, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functionals_setupID( functs, ftMem, 5, LLD0_DEDE_FUNC_L1, LLD0_DEDE_FUNC_L0, 4.0, 16.0, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor!");
    return exitApp(ERR_MEMORY);                      
  } 

  // full turn, non-hierarchical:
  if (!functionals_setupID( functs, ftMem, 6, LLD0_FUNC_L0f, LLD_LEVEL0, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 0!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functionals_setupID( functs, ftMem, 7, LLD0_DE_FUNC_L0f, LLD0_DE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 1!");
    return exitApp(ERR_MEMORY);                      
  } 
  if (!functionals_setupID( functs, ftMem, 8, LLD0_DEDE_FUNC_L0f, LLD0_DEDE, funcL0_T, funcL0_fs, -1)) {
    FEATUM_ERR_FATAL(0,"Failed setting up functional extractor 2!");
    return exitApp(ERR_MEMORY);                      
  } 
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

  pHtkOutput htkOutp = NULL;
  if (opts->htkout != NULL) {
    htkOutp = htkOutput_create(htkOutp, ftMem, htkOutputFile);
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
    htkOutput_configure(htkOutp, htkEls, nHTKels);
    
  }


  pCsvOutput csvOutp = NULL;
  if (opts->csvout != NULL) {
    csvOutp = csvOutput_create(csvOutp, ftMem, csvOutputFile);
    #define nCSVels 12
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
    // lpc:
/*
    csvEls[12] = outputObjectFull(LLD_LEVEL0, ft_lld_lpc);
    csvEls[13] = outputObjectFull(LLD0_DE, ft_lld_lpc);
    csvEls[14] = outputObjectFull(LLD0_DEDE, ft_lld_lpc);
*/
   
    csvOutput_configure(csvOutp, csvEls, nCSVels);
    
  }
  
  #ifdef ENABLE_FUNCTIONALS
// TODO:: bug here... buildNames does not properly build the functional's names, or the level linking is broken!
  pArffOutput arffOutp = NULL;
  if (opts->arffout != NULL) {
    arffOutp = arffOutput_create(arffOutp, ftMem, arffOutputFile, 0);
    #define nARFFels 12
    sElID arffEls[nARFFels];
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
        
    arffOutput_configure(arffOutp, arffEls, nARFFels);
    
  }
  
  // hierarchical functionals:
  pArffOutput arffOutpHR = NULL;  
  if (opts->arffoutHR != NULL) {
    arffOutpHR = arffOutput_create(arffOutp, ftMem, opts->arffoutHR, 0);
    #define nARFFels 12
    sElID arffEls[nARFFels];
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
        
    arffOutput_configure(arffOutpHR, arffEls, nARFFels);
  }

  // full turn, non hierarchical:
  pArffOutput arffOutpFull = NULL;
  if (opts->arffoutFull != NULL) {
    arffOutpFull = arffOutput_create(arffOutpFull, ftMem, opts->arffoutFull, 0);
    #define nARFFels 12
    sElID arffEls[nARFFels];
    // mfccs:
    
    arffEls[0] = outputObjectFull(LLD0_FUNC_L0f, ft_lld_energy);
    arffEls[1] = outputObjectFull(LLD0_DE_FUNC_L0f, ft_lld_energy);
    arffEls[2] = outputObjectFull(LLD0_DEDE_FUNC_L0f, ft_lld_energy);

    arffEls[3] = outputObjectFull(LLD0_FUNC_L0f, ft_lld_pitch);
    arffEls[4] = outputObjectFull(LLD0_DE_FUNC_L0f, ft_lld_pitch);
    arffEls[5] = outputObjectFull(LLD0_DEDE_FUNC_L0f, ft_lld_pitch);

    arffEls[6] = outputObjectFull(LLD0_FUNC_L0f, ft_lld_mfcc);
    arffEls[7] = outputObjectFull(LLD0_DE_FUNC_L0f, ft_lld_mfcc);
    arffEls[8] = outputObjectFull(LLD0_DEDE_FUNC_L0f, ft_lld_mfcc);

    arffEls[9] = outputObjectFull(LLD0_FUNC_L0f, ft_lld_time);
    arffEls[10] = outputObjectFull(LLD0_DE_FUNC_L0f, ft_lld_time);
    arffEls[11] = outputObjectFull(LLD0_DEDE_FUNC_L0f, ft_lld_time);
        
    arffOutput_configure(arffOutpFull, arffEls, nARFFels);
    
  }
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
  signal(SIGINT, INThandler);
  
  pPcmBuffer frame1 = NULL;
  pPcmBuffer frame2 = NULL;
  int okay = 1;
  //waveOutput_setStartTime(waveOut, ....);
  int fl = 0; int ret;
  while( okay ) {
    int flag = 0;

    FEATUM_DEBUG(9,"calling getFrame (id1=%i)",id1);
    ret = inputFramer_getFrame(framedInput, id1, &frame1);
    if ((frame1 != NULL)&&(frame1->nBlocks == 0)) { pcmBufferFree(frame1, 0); frame1 = NULL; }
    if (ret) {
//      if (frame1 != NULL) 
//        printf( "[t=%f]  E1: %f\n", frame1->timestampSec,pcmProcess_energy(frame1));
      flag = 1;             
//      waveOutput_setStopTime(waveOut, ...);                           ;
//      waveOutput_writeDataSequential(waveOut, frame1);
    }

    
    if (frame1 == NULL) FEATUM_DEBUG(9,"FRAME ==  NULL\n");

      
   
    // add frame to short term ringbuffer
    if (LLDex_addFrame(lldex, LLD_LEVEL0, frame1 )) { 
                              
      // call the extractors on all levels.... //+timestamps...
      // TODO: consider extractors priority... i.e. call those with lower numerical value for priority first
      LLDs_callExtractors(llds,LLD_LEVEL0);  //level
      //TODO:: Flush condition + FlushToFTMem if true
      LLDs_flushToFtMem(llds,ftMem,LLD_LEVEL0); //level
      deltas_autoCompute(deltas);
      int flag = 0;
      #ifdef ENABLE_FUNCTIONALS
      flag += functionals_computeCurrentOnly( functs, 0 );
      flag += functionals_computeCurrentOnly( functs, 1 );
      flag += functionals_computeCurrentOnly( functs, 2 );
      #endif
      
      htkOutput_saveFrame(htkOutp);
      csvOutput_saveFrame(csvOutp);
    
      #ifdef ENABLE_FUNCTIONALS
      if (flag)
        arffOutput_saveFrame(arffOutp);
      #endif

    }
    
    //functionals_computeCurrentOnly( functs, 0 );
    
    #ifdef LIVE_REC
    if (featum_quit && (liveIn != NULL)) {
      if (!fl) printf("Stopped recording from device ... waiting for data in buffer to be written to disk.\n"); 
      fl = 1;
      liveInput_setStatus(liveIn, 0);
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
  liveIn = liveInput_destroy(liveIn);
  #endif
  // compute deltas at the end properly....
  deltas_autoComputeLast( deltas );
  // save last frames
  while (htkOutput_saveFrame(htkOutp)) {}
  
  #ifdef ENABLE_FUNCTIONALS
  functionals_computeNow( functs, 0, 2 );
  functionals_computeNow( functs, 1, 2 );
  functionals_computeNow( functs, 2, 2 );
  functionals_computeNow( functs, 3, 1 );
  functionals_computeNow( functs, 4, 1 );
  functionals_computeNow( functs, 5, 1 );
  functionals_computeNow( functs, 6, 1 );
  functionals_computeNow( functs, 7, 1 );
  functionals_computeNow( functs, 8, 1 );
  
  //functionals_computeNow( functs, 1, 1 );
  //  FEATUM_DEBUG(1,"0: %i, 1: %i",(long)ftMem->level[2].ft[0][0], (long)ftMem->level[2].ft[0][1]);

  if (opts->arfflabel != NULL) {
    arffOutput_saveFrameExt(arffOutpHR, opts->arfflabel, NULL);
    arffOutput_saveFrameExt(arffOutpFull, opts->arfflabel, NULL);
  } else {
    arffOutput_saveFrame(arffOutpHR);
    arffOutput_saveFrame(arffOutpFull);
  }

  arffOutp = arffOutput_destroy( arffOutp);
  arffOutpHR = arffOutput_destroy( arffOutpHR);
  arffOutpFull = arffOutput_destroy( arffOutpFull);
  #endif  
  
  frame1 = pcmBufferFree(frame1,0);
  frame2 = pcmBufferFree(frame2,0);
  
  htkOutp = htkOutput_destroy( htkOutp );
  csvOutp = csvOutput_destroy( csvOutp );
  
  
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
  functs = functionals_destroy( functs );
  #endif
  deltas = deltas_destroy( deltas );

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
  framedInput = inputFramer_destroyWithInput(framedInput);
  FEATUM_DEBUG(5,"destroying wave file input (%s)",inputfile);  
  waveIn = waveInput_destroy(waveIn);
  
  options = optionParser_destroy( options );

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

