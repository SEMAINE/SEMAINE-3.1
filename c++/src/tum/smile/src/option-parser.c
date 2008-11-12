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
 
 /*  IMPLEMENTATION of the class <classTemplate>
     See the corresponding header file (.h) for documentation!  */
 
 #define MODULE "option-parser"
 // commandline option parser and config file reader

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
//#include <stdlib.h>

#include "featum_common.h"
#include "featum_util.h"       //( optional )
#include "option-parser.h"

#undef FUNCTION

pOptions globalOpts = NULL;


//TODO::: warn when adding options twice !!

/* Destructor: frees the class data object and returns NULL */
//pOptionParser optionParser_destroy( pOptionParser obj );

/* Constructor: creates a class data object */
pOptionParser optionParser_create( pOptionParser obj, int argc, char *argv[], const char * configfile ) 
#define FUNCTION "optionParser_create"
{_FUNCTION_ENTER_
     if (obj == NULL) {
       obj = (pOptionParser)calloc(1,sizeof(sOptionParser));
       if (obj == NULL) {
         FEATUM_ERR_FATAL(1,"Error allocating memory!");
         _FUNCTION_RETURN_(NULL);
       }
     } else {
       memzero(obj, sizeof(sOptionParser));       
     }
     obj->argc = argc;
     obj->argv = argv;
     obj->configfile = (char *)configfile;
     obj->nOpt = 0;
       
     _FUNCTION_RETURN_( obj );
}
#undef FUNCTION


void optionParser_addBoolean( pOptionParser obj, const char *name, char abbr, int *val, int dflt, const char *info )
#define FUNCTION "optionParser_addBoolean"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_BOOLEAN;
    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = 1;
    obj->optionsToParse[obj->nOpt].mandatory = 0;
    obj->nOpt++;    
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION


void optionParser_addInt( pOptionParser obj, const char *name, char abbr, int *val, int dflt, const char *info, int optional_arg, int mandatory )
#define FUNCTION "optionParser_addBoolean"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_INT;
    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = optional_arg;
    obj->optionsToParse[obj->nOpt].mandatory = mandatory;    
    obj->nOpt++;    
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION


void optionParser_addLONG_IDX( pOptionParser obj, const char *name, char abbr, LONG_IDX *val, LONG_IDX dflt, const char *info, int optional_arg, int mandatory  )
#define FUNCTION "optionParser_addBoolean"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_LONG_IDX;
    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = optional_arg;
    obj->optionsToParse[obj->nOpt].mandatory = mandatory;     
    obj->nOpt++;
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

void optionParser_addFloat( pOptionParser obj, const char *name, char abbr, float *val, float dflt, const char *info, int optional_arg, int mandatory  )
#define FUNCTION "optionParser_addFloat"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR_FATAL(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR_FATAL(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_FLOAT;
    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = optional_arg;
    obj->optionsToParse[obj->nOpt].mandatory = mandatory;   
    obj->nOpt++;
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

void optionParser_addDouble( pOptionParser obj, const char *name, char abbr, double *val, double dflt, const char *info, int optional_arg, int mandatory  )
#define FUNCTION "optionParser_addBoolean"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_DOUBLE;
    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = optional_arg;
    obj->optionsToParse[obj->nOpt].mandatory = mandatory;   
    obj->nOpt++;
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

void optionParser_addPchar( pOptionParser obj, const char *name, char abbr, char **val, const char *dflt, const char *info, int optional_arg, int mandatory  )
#define FUNCTION "optionParser_addBoolean"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_PCHAR;
    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = optional_arg;    
    obj->optionsToParse[obj->nOpt].mandatory = mandatory;   
    obj->nOpt++;    
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION

//??????????   use memcopy??
void optionParser_addString( pOptionParser obj, const char *name, char abbr, char *val, const char *dflt, const char *info, int len, int optional_arg, int mandatory )
#define FUNCTION "optionParser_addBoolean"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    #ifdef COMPILE_CHECKS
    if (val == NULL) _FUNCTION_RETURNV_;
    if (name == NULL) _FUNCTION_RETURNV_;
    if (obj->nOpt == N_OPTIONS) {
      FEATUM_ERROR(0,"trying to add more than N_OPTIONS! please increase N_OPTIONS in option-parser.h");
      FEATUM_ERROR(0,"discarding option %s",name);
      _FUNCTION_RETURNV_;
    }
    #endif
    obj->optionsToParse[obj->nOpt].used = 1;
    obj->optionsToParse[obj->nOpt].name = name;
    obj->optionsToParse[obj->nOpt].abbr = abbr;
    obj->optionsToParse[obj->nOpt].type = OPT_TYPE_STRING;
    obj->optionsToParse[obj->nOpt].len = len;
//    *val = dflt;
    obj->optionsToParse[obj->nOpt].data = (void *)val;
    obj->optionsToParse[obj->nOpt].info = info;
    obj->optionsToParse[obj->nOpt].optional_arg = optional_arg;
    obj->optionsToParse[obj->nOpt].mandatory = mandatory;   
    obj->nOpt++;    
  }   
  _FUNCTION_RETURNV_;
}
#undef FUNCTION


// actually parse the options (config file first, then commandline)
pOptions optionParser_doParse( pOptionParser obj )
#define FUNCTION "optionParser_doParse"
{_FUNCTION_ENTER_
  int i,n; 
  if (obj != NULL) {
  
    if (obj->configfile) { // parse config file
                         
    }
    int parFound ;
    if ((obj->argv)&&(obj->argc > 1)) { // parse commandline
      for (n=1; n<obj->argc; n++) {
      if (obj->argv[n][0] == '-') { // begin of parameter
          FEATUM_DEBUG(10,"option (%i) = \"%s\"",n,obj->argv[n]);
          parFound = 0;
          for (i=0; i<obj->nOpt; i++) {
            if ( (obj->optionsToParse[i].used) && (obj->argv[n][0] == '-') &&
                ( ((obj->optionsToParse[i].abbr != 0)&&(obj->argv[n][1] == obj->optionsToParse[i].abbr) && ((obj->argv[n][2] == 0)||(obj->argv[n][2]==' ')) )  
                 || (!strcmp(obj->optionsToParse[i].name, &(obj->argv[n][1]))) ) ) {
               FEATUM_DEBUG(10,"found option \"%s\" on cmdline (n=%i, i=%i) ",obj->argv[n],n,i);
               parFound = 1;
               if (obj->optionsToParse[i].type != OPT_TYPE_BOOLEAN) { // more to parse
                 if (n < obj->argc-1) { // are there enough parameters left?
/*
                   if ((obj->argv[n+1][0] == '-') && !(obj->optionsToParse[i].optional_arg)) {
                     FEATUM_ERROR(1,"parameter \"%s\" requires an argument!",obj->optionsToParse[i].name);
                     _FUNCTION_RETURN_(NULL);
                   } else if (obj->argv[n+1][0] != '-') {
                          */
                     n++;     
                     switch(obj->optionsToParse[i].type) {
                        case OPT_TYPE_INT:
                          *(int *)(obj->optionsToParse[i].data) = atoi(obj->argv[n]);
                          break;                                    
                        case OPT_TYPE_LONG_IDX:
                          FEATUM_DEBUG(6,"read LONG_IDX parameter: %i (string: %s)",(LONG_IDX) atoi(obj->argv[n]),obj->argv[n]);
                          *(LONG_IDX *)(obj->optionsToParse[i].data) = (LONG_IDX)atoi(obj->argv[n]);
                          break;                                    
                        case OPT_TYPE_FLOAT:
                          FEATUM_DEBUG(6,"read float parameter: %f (string: %s)",atof(obj->argv[n]),obj->argv[n]);   
                          *(float *)(obj->optionsToParse[i].data) = (float)atof(obj->argv[n]);
                          break;                                    
                        case OPT_TYPE_DOUBLE:
                          *(double *)(obj->optionsToParse[i].data) = atof(obj->argv[n]);
                          break;                                    
                        case OPT_TYPE_FP:
                          #if FLOAT_TYPE == float
                          *(FLOAT_TYPE *)(obj->optionsToParse[i].data) = (FLOAT_TYPE)atof(obj->argv[n]);
                          #else
                           #if FLOAT_TYPE == double
                          *(FLOAT_TYPE *)(obj->optionsToParse[i].data) = (FLOAT_TYPE)atof(obj->argv[n]);
                           #else
                             #if FLOAT_TYPE == long double
                          *(FLOAT_TYPE *)(obj->optionsToParse[i].data) = (FLOAT_TYPE)strtold(obj->argv[n]);
                             #else
                          *(FLOAT_TYPE *)(obj->optionsToParse[i].data) = (FLOAT_TYPE)atof(obj->argv[n]);
                             #endif
                           #endif  
                          #endif
                          break;                                    
                       case OPT_TYPE_PCHAR:
                         *(char **)(obj->optionsToParse[i].data) = obj->argv[n];
                         break;
                       case OPT_TYPE_STRING:
                         FEATUM_ERR_FATAL(0,"(fixed length) STRING options not yet implemented!");
                         break;
                     }

                   //}     
                 } else if (!(obj->optionsToParse[i].optional_arg)) {
                   FEATUM_ERROR(1,"parameter \"%s\" requires an argument!",obj->optionsToParse[i].name);
                   _FUNCTION_RETURN_(NULL);
                 }
               } else {
                  *(int *)(obj->optionsToParse[i].data) = ! (*(int *)(obj->optionsToParse[i].data)); // toggle okay???
               } 
               obj->optionsToParse[i].given = 1;
            }
            
          } // end for(i=...)
          if (!parFound) {
            FEATUM_ERROR(1,"ignoring UNKNOWN parameter \"%s\" !",obj->argv[n]);   
            // _FUNCTION_RETURN_(NULL);            
          }
        } else {
          FEATUM_WARNING(1,"exess parameter \"%s\"",obj->argv[n]);
        }
         
      } // end for (n=...)
      
      
      
    }
    
    // now check if all mandatory parameters have been given
    for (i=0; i<obj->nOpt; i++) {
      if (obj->optionsToParse[i].used && obj->optionsToParse[i].mandatory && !(obj->optionsToParse[i].given)) {
        FEATUM_ERROR(1,"MANDATORY parameter \"%s\" not specified! aborting!",obj->optionsToParse[i].name);   
        _FUNCTION_RETURN_(NULL);                                      
      }
    }    
  }  
  _FUNCTION_RETURN_(&(obj->opt));
}
#undef FUNCTION


void optionParser_showUsage( pOptionParser obj )
#define FUNCTION "optionParser_showUsage"
{_FUNCTION_ENTER_
  if (obj == NULL) {
    FEATUM_ERR_FATAL(1,"cannot print usage due to error with optionParser object (=NULL)");
    _FUNCTION_RETURNV_;
  }  
  
  printf("\nfeaTUM usage:\n=============\n\n");
  if (obj->argv && obj->argv[0])
    printf("%s -option <value> -option [optional value]  ...\n\n",obj->argv[0]);
  else
    printf("feaTUM -option <value> -option [optional value]  ...\n\n",obj->argv[0]);
  
  printf("Available options:\n-------------------\n\n");
  
  int i;
  for (i=0; i<obj->nOpt; i++) {
    if (obj->optionsToParse[i].used) {
      printf(" -%s ",obj->optionsToParse[i].name);
      if (obj->optionsToParse[i].abbr != 0) 
        printf("/ -%c",obj->optionsToParse[i].abbr);
      printf("  ");
      if (obj->optionsToParse[i].type != OPT_TYPE_BOOLEAN) {
        if (obj->optionsToParse[i].optional_arg)
          printf("[");
        else 
          printf("<");  
      }
      switch (obj->optionsToParse[i].type) {
        case OPT_TYPE_INT:
           printf("int");
           break;       
        case OPT_TYPE_BOOLEAN:
           //printf("boolean");
           break;       
        case OPT_TYPE_FLOAT:
           printf("real#");
           break;       
        case OPT_TYPE_DOUBLE:
           printf("real#");
           break;       
        case OPT_TYPE_FP:
           printf("real#");
           break;       
        case OPT_TYPE_PCHAR:
           printf("string");
           break;       
        case OPT_TYPE_STRING:
           printf("string(maxlen=%i)",obj->optionsToParse[i].len);
           break;       

      }
      if (obj->optionsToParse[i].type != OPT_TYPE_BOOLEAN) {
        if (obj->optionsToParse[i].optional_arg)
          printf("]");
        else 
          printf(">");
      }
      switch (obj->optionsToParse[i].type) {
        case OPT_TYPE_INT:
           printf(" -- default = %i",*(int *)(obj->optionsToParse[i].data));
           break;       
        case OPT_TYPE_BOOLEAN:
           if (*(int *)(obj->optionsToParse[i].data))
             printf(" -- default = ON");
           else  
             printf(" -- default = OFF");
           break;       
        case OPT_TYPE_FLOAT:
           printf(" -- default = %f",*(float *)(obj->optionsToParse[i].data));
           break;       
        case OPT_TYPE_DOUBLE:
           printf(" -- default = %f",*(double *)(obj->optionsToParse[i].data));
           break;       
        case OPT_TYPE_FP:
           printf(" -- default = %f",*(FLOAT_TYPE *)(obj->optionsToParse[i].data));
           break;       
        case OPT_TYPE_PCHAR:
           printf(" -- default = %s",*(char **)(obj->optionsToParse[i].data));
           break;       
        case OPT_TYPE_STRING:
           printf(" -- default = %s",(char *)(obj->optionsToParse[i].data));
           break;       
      }
      printf("\n");
      if (obj->optionsToParse[i].info) {
        printf("\t%s\n",  obj->optionsToParse[i].info);
      }  
      printf("\n");       
    }
  }
 _FUNCTION_RETURNV_;
}
#undef FUNCTION


void optionParser_globaliseOptions( pOptionParser obj )
#define FUNCTION "optionParser_globaliseOptions"
{_FUNCTION_ENTER_
  if (obj != NULL) {
    globalOpts = &(obj->opt);
  } else {
    globalOpts = NULL;     
  }
}
#undef FUNCTION 


inline pOptions optionParser_getGlobalOpts( void )
#define FUNCTION "optionParser_getGlobalOpts"
{_FUNCTION_ENTER_
  _FUNCTION_RETURN_( globalOpts );
}
#undef FUNCTION    


/* Destructor: frees the class data object */
pOptionParser optionParser_destroy( pOptionParser obj )
#define FUNCTION "optionParser_destroy"
{_FUNCTION_ENTER_
        
          
     if (obj != NULL)
       free(obj);
        
     _FUNCTION_RETURN_( NULL );
}
#undef FUNCTION




/*

    //
    // Get values from command line
    //
    while(len != -1)
    {        
        char **opts = (char**)&options;
        char **vals = (char**)&sub_values;

        // get main option (with -)
        len = get_arg_opt(argc, arg, &pos, &option_name, &option_type, opts);
        sub_len = 0;
        sub_pos = 0;
        
        switch(option_name)
        {
            // ---------------------------------------------
            // -con
            // ---------------------------------------------
            case OPT_CON:
                sub_len = get_sub_opt(len, opts, &sub_pos, sub_name, vals, 1);
                if(sub_len != -1) {
                    if(isnumeric(sub_values[0]))
                        sh_actions->connection = atoi(sub_values[0]);   
                }
                break;

            // ---------------------------------------------
            // -play
            // ---------------------------------------------
            case OPT_PLAY:
                // -playthread, -playstreamthread ? set thread on!
                if(option_type == 1 || option_type == 3)
                    *play_thread = -1;
                
                while(sub_len != -1)
                {
                    sub_len = get_sub_opt(len, opts, &sub_pos, sub_name, vals, 1);
                    if(sub_len != -1)
                    {                        
                        // "", file=
                        if(!strcmp(sub_name, "") || !strcmp(sub_name, "file"))
                        {   
                            // help for -play
                            if(!strcmp(sub_values[0], "help") || !strcmp(sub_values[0], "?"))
                            {
                                print_usage_play();
                                goto usage_end;
                            }
                            else
                            {                                
                                strncpy(sh_actions->play_rec.play_file, sub_values[0], sizeof(sh_actions->play_rec.play_file));

                                // WAVE as default
                                sh_actions->play_rec.wave = 1;
                                sh_actions->play_rec.raw = 0;                                
                                sh_actions->play_rec.speex = 0;
                                sh_actions->play_rec.rtp = 0;
                            }

                            // -playstream, -playstreamthread ? set clockrate!
                            if(option_type == 2 || option_type == 3 && sub_len == 2) {
                                if(isnumeric(sub_values[1]))
                                    sh_actions->play_rec.stream_clockrate = atoi(sub_values[1]);
                                else
                                    sh_actions->play_rec.stream_clockrate = 8000;

                                 // RAW
                                 sh_actions->play_rec.raw = 1;
                                 sh_actions->play_rec.speex = 0;                                 
                                 sh_actions->play_rec.wave = 0;                                    
                                 sh_actions->play_rec.rtp = 0;
                            }                                                        
                        }
                        // hz=
                        if(!strcmp(sub_name, "hz"))
                        {
                            if(isnumeric(sub_values[0]))
                                sh_actions->play_rec.stream_clockrate = atoi(sub_values[0]);
                            else
                                sh_actions->play_rec.stream_clockrate = 8000;

                            if(sh_actions->play_rec.stream_clockrate <= 0 || sh_actions->play_rec.stream_clockrate > 44100)
                                sh_actions->play_rec.stream_clockrate = 8000;

                             // RAW
                             sh_actions->play_rec.raw = 1;
                             sh_actions->play_rec.speex = 0;                                 
                             sh_actions->play_rec.wave = 0;                                    
                             sh_actions->play_rec.rtp = 0;
                        }                        
                        // mode=
                        if(!strcmp(sub_name, "mode"))
                        {
                            for(i=0; i<sub_len; i++)
                            {
                                if(!strcmp(sub_values[i], "thread"))
                                    *play_thread = -1;
                                
                                if(!strcmp(sub_values[i], "stream") || !strcmp(sub_values[i], "raw"))
                                {
                                    if(sh_actions->play_rec.stream_clockrate == 0)
                                        sh_actions->play_rec.stream_clockrate = 8000;

                                    // RAW
                                    sh_actions->play_rec.raw = 1;
                                    sh_actions->play_rec.wave = 0;
                                    sh_actions->play_rec.speex = 0;
                                    sh_actions->play_rec.rtp = 0;
                                } 

                                if(!strcmp(sub_values[i], "speex")) 
                                {
                                    // SPEEX
                                    sh_actions->play_rec.speex = 1;
                                    sh_actions->play_rec.raw = 0;
                                    sh_actions->play_rec.wave = 0;                                    
                                    sh_actions->play_rec.rtp = 0;
                                }

                                if(!strcmp(sub_values[i], "rtp")) 
                                {
                                    // RTP/RTCP
                                    sh_actions->play_rec.rtp = 1;
                                    sh_actions->play_rec.speex = 0;
                                    sh_actions->play_rec.raw = 0;
                                    sh_actions->play_rec.wave = 0;
                                }
                            }
                        }
                        
                        // rtp=
                        if(!strcmp(sub_name, "rtp") || strcmp(sub_name, "rtp_port"))
                        {
                            if(isnumeric(sub_values[0]))
                                sh_actions->play_rec.rtp_port = atoi(sub_values[0]);
                        }

                        // bufsize=
                        if(!strcmp(sub_name, "buffersize") || !strcmp(sub_name, "bufsize"))
                        {
                            if(isnumeric(sub_values[0]))
                                sh_actions->play_rec.stream_buffersize = atol(sub_values[0]);
                            else
                                sh_actions->play_rec.stream_buffersize = 0;
                        }

                        // wait_start=
                        if(!strcmp(sub_name, "wait_start") || !strcmp(sub_name, "waitstart"))
                        {
                            if(isnumeric(sub_values[0]))
                                sh_actions->play_rec.wait_start = atoi(sub_values[0]);
                            else
                                if(!strcmp(sub_values[0], "endless"))
                                    sh_actions->play_rec.wait_start = -1;
                                else
                                    sh_actions->play_rec.wait_start = 0;
                        }

                        // wait_end=
                        if(!strcmp(sub_name, "wait_end") || !strcmp(sub_name, "waitend"))
                        {
                            if(isnumeric(sub_values[0]))
                                sh_actions->play_rec.wait_end = atoi(sub_values[0]);
                            else
                                if(!strcmp(sub_values[0], "endless"))
                                    sh_actions->play_rec.wait_end = -1;
                                else
                                    sh_actions->play_rec.wait_end = 0;
                        }

                        // timeout= (both: waitstart= and waitend=)
                        if(!strcmp(sub_name, "wait") || !strcmp(sub_name, "timeout"))
                        {
                            if(isnumeric(sub_values[0])) {
                                sh_actions->play_rec.wait_start = atoi(sub_values[0]);
                                sh_actions->play_rec.wait_end = atoi(sub_values[0]);
                            }
                            else {
                                if(!strcmp(sub_values[0], "endless")) {
                                    sh_actions->play_rec.wait_start = -1;
                                    sh_actions->play_rec.wait_end  = -1;
                                }
                                else {
                                    sh_actions->play_rec.wait_start = 0;
                                    sh_actions->play_rec.wait_end = 0;

                                }
                            }
                        }
                    }
                }                
                break;

                
            // ---------------------------------------------
            // -record
            // ---------------------------------------------
*/

