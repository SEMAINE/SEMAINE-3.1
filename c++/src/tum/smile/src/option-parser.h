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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This class does ....  the commandline option parsing and config file parsing
(config file parsing not yet implemented)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __OPTION_PARSER_H
#define __OPTION_PARSER_H

/********************************************************************************/
// max number of options
#define N_OPTIONS 99

// all option values go into this struct
typedef struct {
  int help;
  int sildet;
  int HRF;
  int displayenergy;

  float frameSize, frameStep;
  float preEmphasis;
  char *infile;
  char *htkout;
  char *csvout;
  char *arffout;
  char *arffoutHR;
  char *arffoutFull;
  char *arffoutFullA;
  char *arffoutFullV;
  char *arfflabel;
  char *arffclasses;
  char *arffclassesA;
  char *arffclassesV;
  char *waveout;
  char *waveout0;
  char *waveout1;
  char *waveout2;
  char *waveout3;
  char *pipeaudio;
  char *svmmodelA;
  char *svmmodelV;
  char *svmmodelI;
  char *svmscaleA;
  char *svmscaleV;
  char *svmscaleI;
  char *svmoutA;
  char *svmoutV;
  char *labelA;
  char *labelV;
  float sil_thresh;
  char *svmpredfselA;
  char *svmpredfselV;
  char *svmpredfselI;

  char *cmsInitial;
  float cmsAlpha;
    
  // audio recorder:
  int device;
  int listdevices;
  int buffersize;
  int channels;
  int samplerate;
  int bits;
  int useFloat32;   // use float32 instead of int32 samples
  int recordtime;
  int minrectime;
  int silwait;
  // sync:
  int triggerchan;
  char *triggerfile;
  LONG_IDX numtriggers;
  float rectime;
  float triggerfreq;
  
  int int1;
  LONG_IDX long1;
  float float1;
  
  // mfcc:
  int nMel;
  int nMFCC;
  int cepLifter;
  int firstMFCC;
  int usePower;
  
  // logging:
  int loglevel_debug;
} sOptions;
typedef sOptions *pOptions;


// these functions go into your main.c or whereever the options are parsed

/*
int setupOptions( pOptionParser obj ) 
#define FUNCTION "setupOptions"
{_FUNCTION_ENTER_
  if (obj == NULL) return;

  // add your parameters:::  (don't forget to define variables in the struct in option-parser.h !!)
  optionParser_addBoolean( obj, "help", 'h', &(obj->opt.help), 0, "Display this usage message" );
  optionParser_addFloat( obj, "frameSize", 's', &(obj->opt.frameSize), 0.02, "frame size in seconds", 0, 0);
  optionParser_addFloat( obj, "frameStep", 's', &(obj->opt.frameStep), 0.01, "frame step in seconds", 0, 0);
  
  //-----------------------

  pOptions opts = optionParser_doParse( options );
  if ((opts == NULL) || (opts->help)) {
    optionParser_showUsage(options);
    _FUNCTION_RETURN_(0); 
  } 
  _FUNCTION_RETURN_(1); 
}
#undef FUNCTION
*/

/*
void debug_printOptions( pOptions opts )
#define FUNCTION "debug_printOptions"
{_FUNCTION_ENTER_
  FEATUM_DEBUG(0, " opts->help = %i", opts->help);

  FEATUM_DEBUG(0, " opts->frameSize = %f", opts->frameSize);
  FEATUM_DEBUG(0, " opts->frameStep = %f", opts->frameStep);

}
#undef FUNCTION
*/

// Example code for option parsing:
   /*** option parser *******/
/*
  pOptionParser options =NULL;
  options = optionParser_create( options, argc, argv, NULL );
  pOptions opts = setupOptions( options );
  if (!opts) return exitApp(ERR_CMDLINE);
  optionParser_globaliseOptions( options );
  #ifdef ENABLE_PRINT_OPTIONS
  debug_printOptions( opts );
  #endif
  // option variables are now accessible via   opts->name
  //                           or globalOpts->name   in any module
 */          
           
/********************************************************************************/

// after the options have been parsed they will be available to all modules
// that are including "option-parser.h" via the global variable "globalOpts"
extern pOptions globalOpts;
// WARNING: globalOpts can be NULL, if there was an error initialising/parsing
// the options or if optionParser_globaliseOptions has not been called!


#define OPT_TYPE_BOOLEAN 1
#define OPT_TYPE_INT 2
#define OPT_TYPE_LONG_IDX 3
#define OPT_TYPE_FLOAT 4
#define OPT_TYPE_DOUBLE 5
#define OPT_TYPE_PCHAR 6
#define OPT_TYPE_STRING 7
#define OPT_TYPE_FP 8
 
typedef struct {
  int  used,given;
  char *name;
  char abbr;
  int type;
  int len;
  int optional_arg;
  int mandatory;
  void *data;
  char *info;
} sParseableOption;


typedef struct {
  sParseableOption optionsToParse[N_OPTIONS];
  sOptions  opt;
  int nOpt;
  int argc;
  char **argv;
  char *configfile;
} sOptionParser;
typedef sOptionParser *pOptionParser;
        
/* Constructor: creates a class data object */
pOptionParser optionParser_create( pOptionParser obj, int argc, char *argv[], const char * configfile );
pOptions optionParser_get( pOptionParser obj );


// for last parameter "mandatory"
#define MANDATORY_PARAM 1
#define OPTIONAL_PARAM  0

// for parameter "optional_arg"
#define OPTIONAL_ARG    1
#define MANDATORY_ARG   0

void optionParser_addBoolean( pOptionParser obj, const char *name, char abbr, int *val, int dflt, const char *info );
void optionParser_addInt( pOptionParser obj, const char *name, char abbr, int *val, int dflt, const char *info, int optional_arg, int mandatory  );
void optionParser_addLONG_IDX( pOptionParser obj, const char *name, char abbr, LONG_IDX *val, LONG_IDX dflt, const char *info,  int optional_arg, int mandatory  );
void optionParser_addFloat( pOptionParser obj, const char *name, char abbr, float *val, float dflt, const char *info,  int optional_arg, int mandatory  );
void optionParser_addDouble( pOptionParser obj, const char *name, char abbr, double *val, double dflt, const char *info,  int optional_arg, int mandatory  );
void optionParser_addPchar( pOptionParser obj, const char *name, char abbr, char **val, const char *dflt, const char *info,  int optional_arg, int mandatory  );
void optionParser_addString( pOptionParser obj, const char *name, char abbr, char *val, const char *dflt, const char *info, int len,  int optional_arg, int mandatory );

pOptions optionParser_doParse( pOptionParser obj );
void optionParser_showUsage( pOptionParser obj );

void optionParser_globaliseOptions( pOptionParser obj );
pOptions optionParser_getGlobalOpts( void );

/* Destructor: frees the class data object and returns NULL */
pOptionParser optionParser_destroy( pOptionParser obj );

#endif // #ifndef __OPTION_PARSER_H
