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


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This class does ....  the commandline option parsing and config file parsing
(config file parsing not yet implemented)

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 
#ifndef __OPTION_PARSER_HPP
#define __OPTION_PARSER_HPP

/********************************************************************************/
extern "C" {
  #include "option-parser.h"
}


/********************************************************************************/


class cOptionParser {

sOptionParser _data;

public:
       sOptions & opt; 
       
       /* Constructor: creates a class data object */
       cOptionParser(int argc, char *argv[], const char * configfile) : opt(_data.opt) {
         optionParser_create( &_data, argc, argv, configfile );
       }
       cOptionParser(int argc, char *argv[]) : opt(_data.opt)  {
         optionParser_create( &_data, argc, argv, NULL );
       }

       pOptions get() {
         return optionParser_get( &_data );
       }
       
       void addBoolean( const char *name, char abbr, int *val, int dflt, const char *info )
       {  optionParser_addBoolean( &_data, name, abbr, val, dflt, info ); }

       void addInt( const char *name, char abbr, int *val, int dflt, const char *info, int optional_arg, int mandatory  )
       {  optionParser_addInt( &_data, name, abbr, val, dflt, info, optional_arg, mandatory  ); }

       void addLONG_IDX( const char *name, char abbr, LONG_IDX *val, LONG_IDX dflt, const char *info, int optional_arg, int mandatory  )
       {  optionParser_addLONG_IDX( &_data, name, abbr, val, dflt, info, optional_arg, mandatory  ); }

       void addFloat( const char *name, char abbr, float *val, float dflt, const char *info, int optional_arg, int mandatory  )
       {  optionParser_addFloat( &_data, name, abbr, val, dflt, info, optional_arg, mandatory  ); }

       void addDouble( const char *name, char abbr, double *val, double dflt, const char *info, int optional_arg, int mandatory  )
       {  optionParser_addDouble( &_data, name, abbr, val, dflt, info, optional_arg, mandatory  ); }

       void addPchar( const char *name, char abbr, char **val, const char *dflt, const char *info, int optional_arg, int mandatory  )
       {  optionParser_addPchar( &_data, name, abbr, val, dflt, info, optional_arg, mandatory  ); }

       void addString( const char *name, char abbr, char *val, const char *dflt, const char *info, int len, int optional_arg, int mandatory  )
       {  optionParser_addString( &_data, name, abbr, val, dflt, info, len, optional_arg, mandatory  ); }


       pOptions doParse() {
         return optionParser_doParse( &_data );
       }

       void showUsage() {
         optionParser_showUsage( &_data );
       }

       void globaliseOptions() {
         optionParser_globaliseOptions( &_data );
       }
       
       pOptions getGlobalOpts() {
         return optionParser_getGlobalOpts();
       }

         
       ~cOptionParser() { }
     
};



#endif // #ifndef __OPTION_PARSER_HPP
