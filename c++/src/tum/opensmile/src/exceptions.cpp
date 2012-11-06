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


/*

  SMILE exception classes
  ===========================
  
  Exception base class:
    cSMILException:
      + int getCode()
      + int getType()
        valid type constants:
          EX_GENERIC, EX_MEMORY, EX_COMPONENT, EX_IO, EX_CONFIG, EX_USER
      + getText()  : get human readable error message

  GenericException: (cSMILException) [Internal errors, usually not displayed or loggged, only informative purpose]
    > GenericException(int code)
    - int getCode()
    
  MemoryException: (cSMILException) [Memory allocation errors, etc.]
    > MemoryException(int code)
    - int getCode()

  ----------------------------------------------------------------------------
  ComponentException: (cSMILException) [Any error in a smile component]
    > ComponentException([const char* module], const char* text)
    > ComponentException([const char* module], FMT(const char* fmt, ...))
    + getText()  : get full verbose text of error message
    - int getCode()


  ConfigException: (ComponentException) [Any configuration error]
    > ConfigException(int origin, [const char *module], const char* text)
    > ConfigException(int origin, [const char *module], FMT(const char* fmt, ...))
    + int getOrigin()
   origin: (exception raised when...)
          a) errors occur during config file parsing (origin = CE_PARSER)
          b) a module detects a bogus configuration, invalid value, etc.  (origin = CE_INVALID)
          c) mandatory parameters are not specified, etc. (origin = CE_MANAGER)

  IOException: (ComponentException) [Any I/O error, drive, network, sound, ...]
    > IOException([const char *module], [int code], const char* text, )
    > IOException([const char *module], [int code], FMT(const char* fmt, ...))

  UserException: (ComponentException) [Any other error, user defined...]
    > UserException([const char *module], int code, const char* text, )
    > UserException([const char *module], int code, FMT(const char* fmt, ...))
  
 */
/*
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define MODULE "exceptionHandler"
*/

