/*******************************************************************************
 * openSMILE
 *  - open Speech and Music Interpretation by Large-space Extraction -
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer, Bjoern Schuller
 * 
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
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
 *******************************************************************************/
 


/****

This is the main file of the smile SEMAINE component.
It handles the option parsing and creates an instance of
the semaine::components::smile::TumFeatureExtractor class
and runs the main loop in a separate thread.

****/


#include <iostream>
#include <stdio.h>
#include <list>

#include <semaine/config.h>

#include <semaine/util/XMLTool.h>
#include <semaine/components/Component.h>
#include <semaine/system/ComponentRunner.h>

#include "TumFeatureExtractor.h"
#undef MODULE
#define MODULE "featum_main"
#undef FUNCTION


/***************** options setup ***********************/
pOptions setupOptions( cOptionParser &parser ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_

  // when adding new parameters don't forget to define variables in the struct in option-parser.h !!
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
  parser.addPchar( "svmmodelI", 0, &(parser.opt.svmmodelI), NULL, "model for svm regression/classification (interest)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmscaleA", 0, &(parser.opt.svmscaleA), NULL, "scale for svm regression/classification (arousal)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmscaleV", 0, &(parser.opt.svmscaleV), NULL, "scale for svm regression/classification (valence)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "svmscaleI", 0, &(parser.opt.svmscaleI), NULL, "scale for svm regression/classification (interest)", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addBoolean( "sildet", 0, &(parser.opt.sildet), 0, "Enable silence detector for turn segmentation" );  
  parser.addBoolean( "HRF", 0, &(parser.opt.HRF), 0, "Use Hierarchical instead of full-turn functionals" );
  parser.addPchar( "svmpredfselA", 0, &(parser.opt.svmpredfselA), NULL, "Feature selection file for live svm prediction (arousal)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addPchar( "svmpredfselV", 0, &(parser.opt.svmpredfselV), NULL, "Feature selection file for live svm prediction (valence)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addPchar( "svmpredfselI", 0, &(parser.opt.svmpredfselI), NULL, "Feature selection file for live svm classification (interest)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addBoolean( "displayenergy", 'E', &(parser.opt.displayenergy), 0, "print log frame energy for debugging audio problems" );  
  #endif
  parser.addPchar( "waveout", 'w', &(parser.opt.waveout), NULL, "wave file output of pcm stream", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addPchar( "pipeaudio", 0, &(parser.opt.pipeaudio), NULL, "output wave data to a named pipe", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addFloat( "silthresh", 0, &(parser.opt.sil_thresh), 0.01, "Silence threshold", MANDATORY_ARG, OPTIONAL_PARAM );  

  // CMS on mfcc
  parser.addFloat( "cmsAlpha", 0, &(parser.opt.cmsAlpha), 0.995, "time constant alpha for running average CMS (mfccz)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addPchar( "cmsInitial", 0, &(parser.opt.cmsInitial), NULL, "initial mean vector for mfcc CMS (mfccz)", MANDATORY_ARG, OPTIONAL_PARAM );  

  // mfcc config:
  parser.addInt( "nMel", 0, &(parser.opt.nMel), 26, "number of mel frequency bands in filter bank", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addInt( "firstMFCC", 0, &(parser.opt.firstMFCC), 0, "first mel-frequency coefficient to compute", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addInt( "nMFCC", 0, &(parser.opt.nMFCC), 13, "number of mel-frequency coefficients to compute (starting from firstMFCC)", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addInt( "cepLifter", 0, &(parser.opt.cepLifter), 22, "cepLifter parameter", MANDATORY_ARG, OPTIONAL_PARAM );  
  parser.addBoolean( "usePower", 0, &(parser.opt.usePower), 1, "use power spectrum when computing MFCC");  


  #ifdef LIVE_REC
  #ifdef USE_PORTAUDIO  
  parser.addBoolean( "listdevices", 'l', &(parser.opt.listdevices), 0, "List portaudio devices" );  
  #endif
  parser.addInt( "bufsize", 0, &(parser.opt.buffersize), 4096, "audio buffer size", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "device", 'd', &(parser.opt.device), -1, "audio device id to record from (see -l) [-1 for system default]", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "samplerate", 'r', &(parser.opt.samplerate), 16000, "set recording sample rate", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "bits", 'b', &(parser.opt.bits), 16, "set bits/sample", MANDATORY_ARG, OPTIONAL_PARAM);
  parser.addInt( "channels", 'C', &(parser.opt.channels), 1, "set no. of channels", MANDATORY_ARG, OPTIONAL_PARAM);
  #endif
  
  //-----------------------

  pOptions opts = parser.doParse();
  if ((opts == NULL) || (opts->help)) {
    parser.showUsage();
    _FUNCTION_RETURN_(NULL); 
  } 
  _FUNCTION_RETURN_(opts); 
}
#undef FUNCTION
/****************************************/



int main (int argc, char *argv[]) {
#define FUNCTION "main"


	pOptions opts;

	try {

		/*** option parser *******/
		cOptionParser parser(argc, argv); 
		opts = setupOptions( parser );
		if (opts == NULL) {
			FEATUM_ERROR_FATAL(0,"Error parsing commandline options!");
			return exitApp(ERR_CMDLINE);
		}

		#ifndef LIVE_REC
		FEATUM_ERROR_FATAL(0,"This component only works, when live recording is enabled");
		return exitApp(ERR_CMDLINE);   
		#endif
 
		#ifdef LIVE_REC
		#ifdef USE_PORTAUDIO
		if (opts->listdevices) {
			printf("DEVICE LIST:\n"); fflush(stdout);
			liveInput_listDevices();
			return 0;
		}
 		#endif
		#endif
		
	} catch (int e) {}

	// start the actual component and its main loop
	try {
		semaine::util::XMLTool::startupXMLTools();

		std::list<semaine::components::Component *> comps;

		comps.push_back(new semaine::components::smile::TumFeatureExtractor(opts));
		
		semaine::system::ComponentRunner cr(comps);
		cr.go();
		cr.waitUntilCompleted();

		semaine::util::XMLTool::shutdownXMLTools();
	} catch (cms::CMSException & ce) {
		ce.printStackTrace();
	} catch (std::exception & e) {
		std::cerr << e.what();
	}
}
