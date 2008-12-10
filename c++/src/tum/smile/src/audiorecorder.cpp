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
#define LIVE_REC
#include "live-input.hpp"
#include "silence-detector.hpp"

#include "wave-input.hpp"
#include "wave-output.hpp"
#include "option-parser.hpp"

#undef FUNCTION

/***************** options setup ***********************/
pOptions setupOptions( cOptionParser &parser ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_

  // add your parameters:::  (don't forget to define variables in the struct in option-parser.h !!)
  parser.addBoolean( "help", 'h', &(parser.opt.help), 0, "Display this usage message" );
  parser.addFloat( "frameSize", 'S', &(parser.opt.frameSize), 0.032, "frame size in seconds", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addBoolean( "sildet", 0, &(parser.opt.sildet), 0, "Enable silence detector for turn segmentation" );  
  #ifdef USE_PORTAUDIO  
  parser.addBoolean( "listdevices", 'l', &(parser.opt.listdevices), 0, "List portaudio devices" );  
  #endif
  parser.addFloat( "silthresh", 0, &(parser.opt.sil_thresh), 0.01, "Silence threshold", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addBoolean( "displayenergy", 'E', &(parser.opt.displayenergy), 0, "print log frame energy for debugging audio problems" );  
  parser.addPchar( "waveout", 'w', &(parser.opt.waveout), NULL, "wave file output of pcm stream", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "bufsize", 0, &(parser.opt.buffersize), 4096, "audio buffer size", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "device", 'd', &(parser.opt.device), -1, "audio device id to record from (see -l) [-1 for system default]", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "samplerate", 'r', &(parser.opt.samplerate), 44100, "set recording sample rate", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "bits", 'b', &(parser.opt.bits), 16, "set bits/sample", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "channels", 'C', &(parser.opt.channels), 2, "set no. of channels", MANDATORY_ARG, OPTIONAL_PARAM);

  // default: record infinitely until killed
  // rectime: record only rectime (exact)
  parser.addInt( "recordtime", 't', &(parser.opt.recordtime), 1, "exact recording time", MANDATORY_ARG, OPTIONAL_PARAM);

  // minrectime: minimum rectime, calibrate silence at beginning, record at least minrectime, abort if silence is detected for longer than 10sec.
  parser.addInt( "minrectime", 'm', &(parser.opt.minrectime), 1, "minimum recording time", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "silwait", 'W', &(parser.opt.channels), 1, "time to wait for silence before stopping recording", MANDATORY_ARG, OPTIONAL_PARAM);
  
  //-----------------------

  pOptions opts = parser.doParse();
  if ((opts == NULL) || (opts->help)) {
    parser.showUsage();
    _FUNCTION_RETURN_(NULL); 
  } 
  _FUNCTION_RETURN_(opts); 
}
#undef FUNCTION


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


  #ifdef ENABLE_PROFILER
  timeval profiler;
  profiler_init(&profiler);
  #endif
 
  /*** option parser *******/

  cOptionParser parser(argc, argv); //options = optionParser_create( NULL, argc, argv, NULL );
  pOptions opts = setupOptions( parser );
  if (!opts) return exitApp(ERR_CMDLINE);

  #ifdef USE_PORTAUDIO
  if (opts->listdevices) {
    liveInput_listDevices();
    return 0;
  }
  #endif

  if (opts->waveout == NULL) {
    FEATUM_ERROR_FATAL(0,"You must specify the output file!");
    return exitApp(ERR_CMDLINE);   
  }

  int sampleType = pcmBitsToSampleType( opts->bits, BYTEORDER_LE, 0 );

  /***** wave file output ****/
  cWaveOutput *waveout = new cWaveOutput( opts->waveout, opts->samplerate, sampleType , opts->channels );
  /*************************************************/


  /***************************** sound card input *****************************/    
  cLiveInput *liveIn = new cLiveInput(NULL, opts->device, opts->buffersize, opts->samplerate, opts->bits, opts->channels, LI_STREAM_STARTED);
  FEATUM_DEBUG(4,"opened live input audio: %i Hz, %i bit, %i channel(s)",opts->samplerate, opts->bits, opts->channels);
  /*------------------------------------end--------------------------------------*/
  

  /*------------------------------------end--------------------------------------*/

  FEATUM_DEBUG(4,"entering processing loop");
  FEATUM_MESSAGE(2,"entering recording loop\n");
  signal(SIGINT, INThandler);  // setup Ctrl+C handler
  
  cPcmBuffer frame1(opts->buffersize/4, opts->channels, sampleType, MEMORGA_INTERLV, opts->samplerate);
  // cSilenceDetector silDet(lldex,opts->sil_thresh);
 
  int turntime = 0;
  int fl = 0;
  int okay = 1;
  
  while( okay ) {
    int flag = 0;

    FEATUM_DEBUG(9,"calling getFrame");
    if (liveIn->readFrame(frame1)) flag = 1; 
    waveout->writeFrame(frame1);
printf("flag = %i\n",flag);
    if (featum_quit && (liveIn != NULL)) {
      if (!fl) printf("Stopped recording from device ... waiting for data in buffer to be written to disk.\n"); 
      fl = 1;
      liveIn->setStatus(0);
      // stop recording, then wait for buffer clear
    } else if (featum_quit) {
      flag = 0;  // exit immediately       
    }
    
    okay = flag;
    FEATUM_DEBUG(8,"LOOP featum_quit = %i",featum_quit);
  }
  if (liveIn != NULL) { delete liveIn; liveIn = NULL; }
  if (waveout != NULL) { delete waveout; waveout = NULL; }

  // cleanup        
  return exitApp(NO_ERROR);
}
#undef FUNCTION

