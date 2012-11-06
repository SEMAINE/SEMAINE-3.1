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


#ifndef COMMANDLINE_PARSER_HPP
#define COMMANDLINE_PARSER_HPP

#include <smileCommon.hpp>

#ifdef __WINDOWS
#ifndef __MINGW32
// windows stdlib.h does not seem to define those, even though msdn library says it would ???
#define EINVAL 22
#define ERANGE 34
#else
#include <errno.h>
#endif
#else
#include <errno.h>
#endif

#define N_ALLOC_BLOCK 50
#define STRLEN_NAME 255

#define CMDOPT_BOOLEAN  0
#define CMDOPT_INT      1
#define CMDOPT_DOUBLE   2
#define CMDOPT_STR      3

struct sCmdlineOpt {
  char name[STRLEN_NAME];
  char abbr;
  char *description;
  int type;
  int dfltInt;
  double dfltDouble;
  char *dfltStr;
  int argMandatory;
  int isMandatory;
  int isSet;
};

#undef class
class DLLEXPORT cCommandlineParser {

  private:
    int argc;
    char **argv;
    int N,Nalloc;
    int active;
    struct sCmdlineOpt * opt;

    int getWrIdx();
    int addOpt( const char *name, char abbr, const char *description, int argMandatory, int isMandatory);

    /* find index of option "name", return index upon success, else return -1 */
    int findOpt( const char *name ) const;

  public:

    cCommandlineParser(int _argc, char ** _argv);

    int addBoolean( const char *name, char abbr, const char *description , int dflt=0, int argMandatory=0, int isMandatory=0 );
    int addInt( const char *name, char abbr, const char *description , int dflt=0, int argMandatory=1, int isMandatory=0 );
    int addDouble( const char *name, char abbr, const char *description, double dflt=0.0, int argMandatory=1, int isMandatory=0 );
    int addStr( const char *name, char abbr, const char *description, const char *dflt=NULL, int argMandatory=1, int isMandatory=0 );

    void showUsage(const char *binname=NULL);
    int doParse(int ignDupl=0, int ignUnknown=1);  // return value: 0 on normal parse, -1 if usage was requested with '-h' (in this case the application should terminate after doParse() has finished

    int getBoolean( const char *name ) const;
    int getInt( const char *name ) const;
    int isSet( const char *name ) const;

    double getDouble( const char *name ) const;
    const char * getStr( const char *name ) const;

    ~cCommandlineParser();
};

#endif // COMMANDLINE_PARSER_HPP
