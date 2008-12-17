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


#define TRIGGER_FREQ_MIN  45
#define TRIGGER_FREQ      50
#define TRIGGER_FREQ_MAX  55

#define BUFFERSIZE 48000


/*

Cut and resample multi chanenl wave files with trigger signal in N-th channel

*/

#define MODULE "tumsync"
#include "featum_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#undef LIVE_REC
#include "featum_util.h"       //( optional )
#include "wave-output.h"
#include "wave-input.h"
#include "pcm-process.h"
#include "option-parser.h"

#undef FUNCTION

/***************** options setup ***********************/
pOptions setupOptions( pOptionParser obj ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_
  if (obj == NULL) return;

  // add your parameters:::  (don't forget to define variables in the struct in option-parser.h !!)
  optionParser_addBoolean( obj, "help", 'h', &(obj->opt.help), 0, "Display this usage message" );
  optionParser_addPchar( obj, "infile", 'i', &(obj->opt.infile), NULL, "input file or device", MANDATORY_ARG, MANDATORY_PARAM);
  optionParser_addPchar( obj, "waveout0", '0', &(obj->opt.waveout0), NULL, "wave file output of pcm stream (ch1)", MANDATORY_ARG, MANDATORY_PARAM);
  optionParser_addPchar( obj, "waveout1", '1', &(obj->opt.waveout1), NULL, "wave file output of pcm stream (ch2)", MANDATORY_ARG, MANDATORY_PARAM);
  optionParser_addPchar( obj, "waveout2", '2', &(obj->opt.waveout2), NULL, "wave file output of pcm stream (ch3)", MANDATORY_ARG, MANDATORY_PARAM);
  optionParser_addPchar( obj, "waveout3", '3', &(obj->opt.waveout3), NULL, "wave file output of pcm stream (ch4)", MANDATORY_ARG, MANDATORY_PARAM);
  optionParser_addInt( obj, "triggerchan", 'c', &(obj->opt.triggerchan), 5, "channel with trigger signal", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addLONG_IDX( obj, "numtriggers", 'n', &(obj->opt.numtriggers), 0, "number of trigger signals during recording time", MANDATORY_ARG, OPTIONAL_PARAM);
  optionParser_addFloat( obj, "rectime", 't', &(obj->opt.rectime), 0.0, "recording time in seconds", MANDATORY_ARG, MANDATORY_PARAM);
  optionParser_addPchar( obj, "triggerfile", 'f', &(obj->opt.triggerfile), 0, "output .csv file with trigger times", MANDATORY_ARG, MANDATORY_PARAM);

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

typedef struct {
  SAMPLE32i prev;
  SAMPLE32i thresh;
  SAMPLE32i max;
  long long lastPeak;
  long long lastSeenTrigger;
  long long lastWantedTrigger;
  double triggerPeriodMean;
  long sampleRate;
  long long nTriggers;
  double startTime;
  double lastTime;  // these times are audio times...
  double nTime;   // audio time of numtriggers
  LONG_IDX firstTrigger;
  long firstTriggerLocal;
  long long nForced;
  long long nForcedGrN;
  long long lastPeakMean;
} sMemory;
        


LONG_IDX printTriggerRiseTimes(FILE *triggertimes, long long numtriggers, SAMPLE32i *data, LONG_IDX nBlocks, long long sampleIndex, sMemory *wisdom)
#define FUNCTION "printTriggerRiseTimes"
{
  LONG_IDX i;
  SAMPLE32i thresh = wisdom->thresh;
  SAMPLE32i dataI1 = 0;
  wisdom->firstTriggerLocal = 0;
  
  FEATUM_DEBUG(7,"printTRT nBlocks: %i , sampleIndex: %i",nBlocks,sampleIndex);
  for (i=0; i<nBlocks; i++)
  {
    if (wisdom->firstTrigger) wisdom->lastPeak--;
    if ((wisdom->lastPeak < (wisdom->sampleRate / TRIGGER_FREQ_MAX)*(-1)) || (!wisdom->firstTrigger)) {
      //if (i) dataI1 = data[i-1];
      //else dataI1 = wisdom->prev;
      if (data[i] > thresh) {
         wisdom->lastTime =   (double)sampleIndex / (double)wisdom->sampleRate;
         //printf("trigger: %i  [%f sec.]\n",sampleIndex, wisdom->lastTime);
         fprintf(triggertimes,"%i,%f\n",sampleIndex,wisdom->lastTime);
         if (!wisdom->firstTrigger) {
           wisdom->startTime =   wisdom->lastTime;                         
         }
         if (wisdom->nTriggers <= numtriggers) {
           wisdom->nTime = wisdom->lastTime-wisdom->startTime;                      
           wisdom->lastWantedTrigger = sampleIndex;
         }
         wisdom->nTriggers++;
         if (wisdom->firstTrigger == 0) 
         { 
           wisdom->firstTrigger = sampleIndex;
           if (wisdom->firstTriggerLocal == 0) wisdom->firstTriggerLocal = i;
         }
         // TODO: tigger period mean....
         wisdom->lastPeakMean += wisdom->lastPeak;
         wisdom->lastPeak = 0;
         wisdom->lastSeenTrigger = sampleIndex;
      }
    }
    if ((wisdom->lastPeak < (wisdom->sampleRate / TRIGGER_FREQ_MIN)*(-1)) && (wisdom->firstTrigger)) {
       fprintf(stderr,"trigger: %i  [%f sec.]   [missed -> FORCED!!] \n",sampleIndex, (double)sampleIndex / (double)wisdom->sampleRate);
       if (wisdom->nTriggers < numtriggers) {
         fprintf(triggertimes,"%i,%f\n",sampleIndex,(double)sampleIndex / (double)wisdom->sampleRate);
       } else {
         fprintf(stderr," skipping (>numtriggers)\n");
         wisdom->nForcedGrN++;
       }
       wisdom->lastPeak = 0;
       wisdom->nForced++;
    }
    sampleIndex++;
  }
  wisdom->prev = data[nBlocks-1];
  return sampleIndex;
}
#undef FUNCTION


LONG_IDX getTriggerRiseLevel(SAMPLE32i *data, LONG_IDX nBlocks, long long sampleIndex, sMemory *wisdom)
#define FUNCTION "getTriggerRiseLevel"
{
  LONG_IDX i;
    
  for (i=0; i<nBlocks; i++)
  {
    if (sampleIndex > 10) {
      if (data[i] > wisdom->max) wisdom->max = data[i];
      //printf("data[i]=%i\n",data[i]);
    }
    sampleIndex++;
  }
  wisdom->thresh = wisdom->max/3;
  return sampleIndex;
}
#undef FUNCTION



int main(int argc, char *argv[])
#define FUNCTION "main"
{_FUNCTION_ENTER_

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
  
  loglevel_print_debug = 0;
  
  /***************************** create wave input *****************************/    
  pWaveInput waveIn=NULL;
  FEATUM_DEBUG(4,"opening wave file %s as input",opts->infile); 
  if (!(waveIn = waveInput_create( waveIn, opts->infile,  0 ))) {
    return exitApp(ERR_INPUT);
  }

  FEATUM_DEBUG(4,"wave file %s was sucessfully opened",opts->infile);
  FEATUM_DEBUG(5,"sample rate of wave file: %i",waveInput_getSampleRate(waveIn));
  FEATUM_DEBUG(5,"# of channels in wave file: %i",waveIn->parameters.nChan);
  FEATUM_DEBUG(5,"nBPS in wave file: %i",waveIn->parameters.nBPS);

  /*------------------------------------end--------------------------------------*/

  if (opts->triggerchan < waveIn->parameters.nChan) {
    FEATUM_ERROR_FATAL(0,"triggerchan must be the last channel of input wave file! other channels are not yet supported!\n");
    waveInput_destroy(waveIn);                        
    return exitApp(-1); 
  }
  opts->triggerchan = waveIn->parameters.nChan-1;  //  channel indicies start at 0
  

  /***************************** create wave output *****************************/    
  if (waveIn->parameters.nChan != 5) {
    FEATUM_WARNING(1,"number of input channels is != 5 !!\n Please modify the source in main_sync.c to support your custom number of channels (%i)\n",waveIn->parameters.nChan);
  }

////*** manually added 4 channel mono output to separate files:

  pWaveOutput waveOut0 = NULL;
  pWaveOutput waveOut1 = NULL;
  pWaveOutput waveOut2 = NULL;
  pWaveOutput waveOut3 = NULL;

//    waveOut = waveOutput_create(waveOut, opts->waveout, waveIn->parameters.sampleRate, waveIn->parameters.sampleType, waveIn->parameters.nChan-1);

//    waveOut = waveOutput_create(waveOut, opts->waveout, waveIn->parameters.sampleRate, SAMPLETYPE_I24, waveIn->parameters.nChan-1);

  if ((opts->waveout0)&&(opts->waveout1)&&(opts->waveout2)&&(opts->waveout3)) {
    waveOut0 = waveOutput_create(waveOut0, opts->waveout0, waveIn->parameters.sampleRate, waveIn->parameters.sampleType, 1);
    waveOut1 = waveOutput_create(waveOut1, opts->waveout1, waveIn->parameters.sampleRate, waveIn->parameters.sampleType, 1);
    waveOut2 = waveOutput_create(waveOut2, opts->waveout2, waveIn->parameters.sampleRate, waveIn->parameters.sampleType, 1);
    waveOut3 = waveOutput_create(waveOut3, opts->waveout3, waveIn->parameters.sampleRate, waveIn->parameters.sampleType, 1);

    if (waveOut0 == NULL) {
      return exitApp(-1);            
    }
    if (waveOut1 == NULL) {
      return exitApp(-1);            
    }
    if (waveOut2 == NULL) {
      return exitApp(-1);            
    }
    if (waveOut3 == NULL) {
      return exitApp(-1);            
    }

  } else {
    return -1;       
  }
  /*------------------------------------end--------------------------------------*/
  
  // install Ctrl+C signal handler
  signal(SIGINT, INThandler);

  FEATUM_DEBUG(5,"entering main loop");
  
  // allocate sample copy buffers
  // A: input buffer (with input parameters)
  pPcmBuffer bufA = pcmBufferAllocate(NULL, BUFFERSIZE, waveIn->parameters.nChan, waveIn->parameters.sampleType, MEMORGA_INTERLV, waveIn->parameters.sampleRate);
  // B: output buffer (with output parameters)
//  pPcmBuffer bufB = pcmBufferAllocate(NULL, BUFFERSIZE, waveIn->parameters.nChan-1, waveIn->parameters.sampleType, MEMORGA_INTERLV, waveIn->parameters.sampleRate);
  pPcmBuffer bufT = pcmBufferAllocate(NULL, BUFFERSIZE, 1, SAMPLETYPE_I32/*waveIn->parameters.sampleType*/, MEMORGA_INTERLV, waveIn->parameters.sampleRate);

//***** for mono channel output support:
  pPcmBuffer bufB = pcmBufferAllocate(NULL, BUFFERSIZE, 1, waveIn->parameters.sampleType, MEMORGA_INTERLV, waveIn->parameters.sampleRate);

  pPcmConversion chConv = pcmConversion_create(NULL, waveIn->parameters.nChan, 1, 0);
  pcmConversion_setRoute(chConv, waveIn->parameters.nChan-1, 0, 1.0);

  pPcmConversion chConv0 = pcmConversion_create(NULL, waveIn->parameters.nChan, 1, 0);
  pcmConversion_setRoute(chConv0, 0, 0, 1.0);
  pPcmConversion chConv1 = pcmConversion_create(NULL, waveIn->parameters.nChan, 1, 0);
  pcmConversion_setRoute(chConv1, 1, 0, 1.0);
  pPcmConversion chConv2 = pcmConversion_create(NULL, waveIn->parameters.nChan, 1, 0);
  pcmConversion_setRoute(chConv2, 2, 0, 1.0);
  pPcmConversion chConv3 = pcmConversion_create(NULL, waveIn->parameters.nChan, 1, 0);
  pcmConversion_setRoute(chConv3, 3, 0, 1.0);

  
  FILE *triggertimes = NULL;
  triggertimes = fopen( opts->triggerfile, "w");
  sMemory triggerMem;
  triggerMem.sampleRate = waveInput_getSampleRate(waveIn);
  triggerMem.firstTrigger = 0;
  triggerMem.lastPeak = 0;
  triggerMem.prev = 0;
  triggerMem.thresh = -1;
  triggerMem.nForced = 0;
  triggerMem.nForcedGrN = 0;
  triggerMem.nTriggers = 0;
  triggerMem.startTime = 0.0;
  triggerMem.lastPeakMean = 0;
  
  int running = 1;
  long long sampleIndex = 0;
  long long samplesWritten = 0;
  long long samplesToWrite = (long long)round((double)(opts->rectime)*(double)waveInput_getSampleRate(waveIn));
  FEATUM_MESSAGE(1,"samplesToWrite: %i, samplesWritten: %i (rectime:%f)",samplesToWrite,samplesWritten, opts->rectime);
  while( running < 3 ) {
    int flag = 0;
    pcmBuffer_clear(bufA); //pcmBuffer_clear(bufB); 
    pcmBuffer_clear(bufT);
    FEATUM_DEBUG(9,"calling getFrame");
    if (waveInput_getDataSequential(waveIn, bufA)) {

      flag = 1;
      LONG_IDX copied = 0;
      
      switch (running) {
        case 1:
          copied = pcmBuffer_copy_ext(bufA, bufT, 0, -1, 0, chConv);
          // analyze trigger channel:
          sampleIndex = getTriggerRiseLevel(bufT->data, copied, sampleIndex, &triggerMem);
          sampleIndex += copied;
//          waveOutput_writeDataSequential(waveOut, bufT);
          break;
          
        case 2:
          copied = pcmBuffer_copy_ext(bufA, bufT, 0, -1, 0, chConv);
          // find triggers:
          sampleIndex = printTriggerRiseTimes(triggertimes, opts->numtriggers, bufT->data, copied, sampleIndex, &triggerMem);
     
          if ((triggerMem.firstTrigger)) { //&&(triggerMem.nTriggers < opts->numtriggers)) {
            LONG_IDX s1=0, s2=0;
            LONG_IDX lng = -1;
            LONG_IDX skip = triggerMem.firstTriggerLocal;
//                                                    printf("-- %i\n",bufA->nBlocks);
            if (samplesToWrite > samplesWritten) {
            if (samplesToWrite - samplesWritten < bufA->nBlocks) { lng = samplesToWrite-samplesWritten; }
            else {lng = -1; }
//                                                        printf("-->%i\n",lng);
            pcmBuffer_clear(bufB); s1=pcmBuffer_copy_ext(bufA, bufB, skip, lng, 0, chConv0);  
            //printf("c0: %i  %i  \n",((int32_t*)(bufA->data))[0],((int32_t*)(bufB->data))[0]);
            waveOutput_writeDataSequential(waveOut0, bufB);
            pcmBuffer_clear(bufB); s2=pcmBuffer_copy_ext(bufA, bufB, skip, lng, 0, chConv1);  
            if (s2 != s1) { FEATUM_WARNING(1,"number of samples copied for ch1 and ch0 differ!"); }
            waveOutput_writeDataSequential(waveOut1, bufB);
            pcmBuffer_clear(bufB); s2=pcmBuffer_copy_ext(bufA, bufB, skip, lng, 0, chConv2);  
            if (s2 != s1) { FEATUM_WARNING(1,"number of samples copied for ch2 and ch0 differ!"); }
            waveOutput_writeDataSequential(waveOut2, bufB);
            pcmBuffer_clear(bufB); s2=pcmBuffer_copy_ext(bufA, bufB, skip, lng, 0, chConv3);  
            if (s2 != s1) { FEATUM_WARNING(1,"number of samples copied for ch3 and ch0 differ!"); }
            waveOutput_writeDataSequential(waveOut3, bufB);
            samplesWritten += s1;
            }
          }
     
          break;

        default:
          break;
      }
        
    }
    if (flag == 0) {
      running++;
      // reset file position to beginning of file
      waveInput_setPos( waveIn, 0);
      if (running==2) FEATUM_MESSAGE(1,"first pass finished.");
      FEATUM_DEBUG(5,"levels: max=%i  thresh=%i",triggerMem.max,triggerMem.thresh);
      sampleIndex = 0;
    }
    // TODO: maybe use getch() to watch for "q" input to quit without Ctrl+C ??
    
    if (featum_quit) {
      printf("CTRL+C pressed (SIGINT).\n"); 
      running = 99;
    } 
    FEATUM_DEBUG(8,"LOOP featum_quit = %i",featum_quit);
  }
  
  // include last trigger (full frame)
  triggerMem.nTime += 1.0/TRIGGER_FREQ;
 
  FEATUM_MESSAGE(1,"samplesToWrite: %i, samplesWritten: %i",samplesToWrite,samplesWritten);
  
  FEATUM_MESSAGE(1,"number of trigger positions missed: %i",triggerMem.nForced);
  FEATUM_MESSAGE(1,"number of detected triggers: %i  (numtriggers requested = %i)",triggerMem.nTriggers,opts->numtriggers);
  FEATUM_MESSAGE(1,"audio time of %i requested triggers is: %f seconds",opts->numtriggers,triggerMem.nTime);
  FEATUM_MESSAGE(1,"DRIFT: audio time - video time = %f seconds",triggerMem.nTime-opts->rectime);
  FEATUM_MESSAGE(1,"Mean trigger frequency = %f",triggerMem.sampleRate/(((double)triggerMem.lastPeakMean/(double)triggerMem.nTriggers)));
  
  if (samplesWritten < samplesToWrite) {
     LONG_IDX lng = samplesToWrite - samplesWritten;
     FEATUM_MESSAGE(1," ... padding %i samples with 0s",lng);
     while (lng > 0) {
       lng -= pcmBuffer_zeroBuf(bufB, lng);
       waveOutput_writeDataSequential(waveOut0, bufB);
       waveOutput_writeDataSequential(waveOut1, bufB);
       waveOutput_writeDataSequential(waveOut2, bufB);              
       waveOutput_writeDataSequential(waveOut3, bufB);  
    }
  } 

  LONG_IDX trigLng =  (LONG_IDX)round(triggerMem.nTime * (double)waveInput_getSampleRate(waveIn));
  if (trigLng > samplesWritten)  {
    FEATUM_MESSAGE(1," ... removed %i samples at the end",trigLng-samplesWritten);
  }
  
  // cleanup...
  if (triggertimes != NULL) fclose(triggertimes);
  pcmConversion_destroy(chConv);

  waveInput_destroy(waveIn);
  pcmBufferFree(bufA,0);
  pcmBufferFree(bufB,0);
  pcmBufferFree(bufT,0);
  waveOutput_destroy(waveOut0);
  waveOutput_destroy(waveOut1);
  waveOutput_destroy(waveOut2);
  waveOutput_destroy(waveOut3);

  pcmConversion_destroy(chConv0);
  pcmConversion_destroy(chConv1);
  pcmConversion_destroy(chConv2);
  pcmConversion_destroy(chConv3);
  
  
  // exit     
  int ret = exitApp(NO_ERROR);  _FUNCTION_RETURN_(ret);
}
#undef FUNCTION

