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


/*

wave or stream in
   |
   v
   
framing (multiple framesizes possible)
(ftex modules register with frames unit?)
   |
   v
   




*/
#define MODULE "main"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "featum_common.h"
#ifndef LIVE_REC
#warning compiling main_recorder.c without LIVE_REC support enabled!
#endif
#include "featum_util.h"       //( optional )
#include "wave-output.h"
#include "wave-input.h"
#include "live-input.h"
#include "pcm-process.h"
#include "option-parser.h"
#include "audio-stream.h"

#undef FUNCTION

/***************** options setup ***********************/
pOptions setupOptions( pOptionParser obj ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_
  if (obj == NULL) return;

  // add your parameters:::  (don't forget to define variables in the struct in option-parser.h !!)
  optionParser_addBoolean( obj, "help", 'h', &(obj->opt.help), 0, "Display this usage message" );
  optionParser_addBoolean( obj, "listdevices", 'l', &(obj->opt.listdevices), 0, "List available audio devices" );
  optionParser_addInt( obj, "buffersize", 's', &(obj->opt.buffersize), 8192, "buffer size in bytes", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addPchar( obj, "infile", 'i', &(obj->opt.infile), NULL, "input file or device", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addPchar( obj, "waveout", 'w', &(obj->opt.waveout), NULL, "wave file output of pcm stream", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addInt( obj, "device", 'd', &(obj->opt.device), -1, "audio device id to record from (see -l) [-1 for system default]", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addInt( obj, "samplerate", 'r', &(obj->opt.samplerate), 48000, "set recording sample rate", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addInt( obj, "bits", 'b', &(obj->opt.bits), 16, "set bits/sample", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addBoolean( obj, "float32", 'f', &(obj->opt.useFloat32), 0, "prefer float32 when -b 32 is used");
  optionParser_addInt( obj, "channels", 'c', &(obj->opt.channels), 2, "set no. of channels", MANDATORY_ARG, OPTIONAL_PARAM);

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


int main(int argc, char *argv[])
#define FUNCTION "main"
{_FUNCTION_ENTER_

#ifndef LIVE_REC
  FEATUM_ERROR_FATAL(0,"please recompile the recorder with \"#define LIVE_REC\" enabled in featum_common.h ...");
  FEATUM_ERROR_FATAL(0,"a recording application without sound device recording support is useless!");
#else

  #ifdef ENABLE_PROFILER
  timeval profiler;
  profiler_init(&profiler);
  #endif
    


  /*** option parser *******/
  pOptionParser options =NULL;
  options = optionParser_create( options, argc, argv, NULL );
  pOptions opts = setupOptions( options );
  if (!opts) return exitApp(ERR_CMDLINE);
  optionParser_globaliseOptions( options );

  #ifdef ENABLE_PRINT_OPTIONS
  debug_printOptions( opts );
  #endif
  
  //char *inputfile = opts->infile;
  if ((opts->waveout == NULL)&&(opts->listdevices == 0)) {
    FEATUM_ERROR_FATAL(0,"You must specify either the -w or -l option (use -h for details!)");
    return exitApp(ERR_CMDLINE);   
  }
  
  if (opts->listdevices) {
    liveInput_listDevices();
  }
  
  if (opts->waveout != NULL) {  // do recording
  
  
  /***************************** live input *****************************/    
  pLiveInput liveIn=NULL;
  pWaveInput waveIn=NULL;

  #ifdef LIVE_REC
  if (opts->infile == NULL) { // setup live audio input
    FEATUM_DEBUG(4,"opening live input audio: %i Hz, %i bit, %i channel(s)",opts->samplerate, opts->bits, opts->channels);
    if (!(liveIn = liveInput_create( liveIn, NULL, opts->device, opts->buffersize, opts->samplerate, pcmBitsToSampleType(opts->bits,BYTEORDER_LE,opts->useFloat32), opts->channels, 0))) {
      return exitApp(ERR_INPUT);
    }
  } else {
  #endif
    FEATUM_DEBUG(4,"opening wave file %s as input",opts->infile);  //PCM_PREBUFFER, PCM_POSTBUFFER
    if (!(waveIn = waveInput_create( waveIn, opts->infile,  opts->buffersize  ))) {
      return exitApp(ERR_INPUT);
    }

    FEATUM_DEBUG(4,"wave file %s was sucessfully opened",opts->infile);
    FEATUM_DEBUG(5,"sample rate of wave file: %i\n",waveInput_getSampleRate(waveIn));
  #ifdef LIVE_REC
  }
  #endif
  /*------------------------------------end--------------------------------------*/
  
  

  /***************************** create audio stream *****************************/    
  pAudioStream inputStream = NULL;
  if (waveIn != NULL) {
    inputStream = audioStream_create(waveIn,TYPE_WAVEINPUT);
  } 
  #ifdef LIVE_REC
  else {
    inputStream = audioStream_create(liveIn,TYPE_LIVEINPUT);
  }
  #endif


  /***************************** create wave output *****************************/    

  pWaveOutput waveOut = NULL;
  if (opts->waveout) {
    waveOut = waveOutput_create(waveOut, opts->waveout, inputStream->sampleRate, inputStream->sampleType, inputStream->nChan);
    if (waveOut == NULL) {
      return exitApp(-1);            
    }
  } else {
    return -1;       
  }
  /*------------------------------------end--------------------------------------*/
  

  FEATUM_DEBUG(4,"entering main loop");
  // install Ctrl+C signal handler
  signal(SIGINT, INThandler);
  
  pPcmBuffer buf = pcmBufferAllocate(NULL, opts->buffersize/2, inputStream->nChan, inputStream->sampleType, MEMORGA_INTERLV, inputStream->sampleRate);
  int running = 1;
  int fl = 0;
  //waveOutput_setStartTime(waveOut, ....);
  liveInput_setStatus(liveIn, 1);
  printf("Started recording from device %i ... press Ctrl+C to stop.\n",opts->device);
// stop recording, then wait for buffer clear
  while( running  ) {
    int flag = 0;
    pcmBuffer_clear(buf);
    FEATUM_DEBUG(9,"calling getFrame (%i)",buf->nBlocks);
    if (audioStream_getData(inputStream, buf)) {
      flag = 1;             
      //waveOutput_setStopTime(waveOut, ...);
      waveOutput_writeDataSequential(waveOut, buf);
    }
    running = flag;
    // TODO: maybe use getch() to watch for "q" input to quit without Ctrl+C ??
    
    if (featum_quit) {
      if (!fl) printf("Stopped recording from device ... waiting for data in buffer to be written to disk.\n"); 
      fl = 1;
      liveInput_setStatus(liveIn, 0);
    } // stop recording, then wait for buffer clear
    FEATUM_DEBUG(8,"LOOP featum_quit = %i",featum_quit);
  }
 
  // cleanup...
  liveInput_destroy(liveIn);
  waveInput_destroy(waveIn);
  pcmBufferFree(buf,0);
  waveOutput_destroy(waveOut);
  audioStream_destroy(inputStream);
  
  }
 
#endif
 
  // exit     
  int ret = exitApp(NO_ERROR);  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION

