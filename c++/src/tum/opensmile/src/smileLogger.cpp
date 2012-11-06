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


/******************************************************************************/

 /*

 openSMILE Message Logging

 */


#include <time.h>

//#include <exceptions.hpp>
#include <smileLogger.hpp>

// Global logger variable (used by exception classes for automatically logging...)
DLLEXPORT cSmileLogger SMILE_LOG_GLOBAL(1);

#define MODULE "smileLogger"

/********************* class implementation ***********************************/

cSmileLogger::cSmileLogger(int _loglevel, const char * _logfile, int _append) :
  logf(NULL),
  msg(NULL),
  silence(0),
  _enableLogPrint(1)
{
  if (_logfile != NULL) {
    logfile = strdup(_logfile);
    stde = 0;
    openLogfile(_append);
  }
  else { stde = 1; logfile = NULL; }

  if (_loglevel >= 0) {
    ll_msg = ll_wrn = ll_err = ll_dbg = _loglevel;
  } else {
    ll_msg = ll_wrn = ll_err = ll_dbg = 0;
  }
  smileMutexCreate(logmsgMtx);
}

cSmileLogger::cSmileLogger(int loglevel_msg, int loglevel_wrn, int loglevel_err, int loglevel_dbg, const char *_logfile, int _append):
  logf(NULL),
  msg(NULL),
  silence(0),
  _enableLogPrint(1)
{
  if (_logfile != NULL) {
    logfile = strdup(_logfile);
    stde = 0;
    openLogfile(_append);
  }
  else { stde = 1; logfile = NULL; }

  ll_msg = loglevel_msg;
  ll_wrn = loglevel_wrn;
  ll_err = loglevel_err;
  ll_dbg = loglevel_dbg;
  smileMutexCreate(logmsgMtx);
}

cSmileLogger::~cSmileLogger()
{
  smileMutexLock(logmsgMtx);
  if (msg != NULL) free(msg);
  closeLogfile();
  if (logfile != NULL) free(logfile);
  smileMutexUnlock(logmsgMtx);
  smileMutexDestroy(logmsgMtx);
}

/* opens the logfile */
void cSmileLogger::openLogfile(int append)
{
  if (logfile == NULL) return;
  if (logf) { fclose(logf); logf=NULL; }
  if (append) {
    logf = fopen(logfile,"a");
  } else {
    logf = fopen(logfile,"w");
  }
  if (logf == NULL) {
    throw(IOException(FMT("cannot open logfile for writing!"),0,MODULE));
  }
}

void cSmileLogger::closeLogfile()
{
  if (logf != NULL) { fclose(logf); logf = NULL; }
}

void cSmileLogger::setLogLevel(int _type, int level)
{
  switch(_type) {
    case LOG_ALL:
         ll_msg = ll_wrn = ll_err = ll_dbg = level;
         break;
    case LOG_MESSAGE: ll_msg = level; break;
    case LOG_WARNING: ll_wrn = level; break;
    case LOG_ERROR:   ll_err = level; break;
    case LOG_DEBUG:   ll_dbg = level; break;
    default:
         throw(ComponentException(FMT("invalid log level type (%i) specified in call to setLogLevel",level),MODULE));
  }
}

void cSmileLogger::setLogLevel(int level)
{
  setLogLevel(LOG_ALL,level);
}

void cSmileLogger::setLogFile(char *file, int _append, int _stde)
{
  if (file != NULL) {
    if (logfile) {
      free(logfile); logfile = NULL;
    }
    logfile = strdup(file);
    stde = _stde;
    openLogfile(_append);
  }
}

void cSmileLogger::setLogFile(const char *file, int _append, int _stde)
{
  if (file != NULL) {
    if (logfile) {
      free(logfile); logfile = NULL;
    }
    logfile = strdup(file);
    stde = _stde;
    openLogfile(_append);
  }
}

// formating of log message, save result in msg
void cSmileLogger::fmtLogMsg(const char *type, char *t, int level, const char *m)
{
  if (t == NULL) return;
  if (msg != NULL) {
    free(msg); msg = NULL;
  }
  if ((type != NULL)&&(m!=NULL)) {
    msg = myvprint("(%s) [%i] in %s : %s",type,level,m,t);
  } else if (type != NULL) {
    msg = myvprint("(%s) [%i] : %s",type,level,t);
  } else if (m != NULL) {
    msg = myvprint("(MSG) [%i] in %s : %s",level,m,t);
  } else {
    msg = myvprint("(MSG) [%i] : %s",level,t);
  }
}

// main log message dispatcher
void cSmileLogger::logMsg(int itype, char *s, int level, const char *m)
{
  if (!silence) {
    // check loglevel and type
    const char *type=NULL;
    switch (itype) {
       case LOG_PRINT :
            if (level > ll_msg) { free(s); return; }
            type=NULL;
            break;
       case LOG_MESSAGE :
            if (level > ll_msg) { free(s); return; }
            type=NULL;
            break;
       case LOG_ERROR :
            if (level > ll_err) { free(s); return; }
            type="ERROR";
            break;
       case LOG_WARNING :
            if (level > ll_wrn) { free(s); return; }
            type="WARN";
            break;
       case LOG_DEBUG :
            if (level > ll_dbg) { free(s); return; }
            type="DBG";
            break;
       default: return;
    }

    smileMutexLock(logmsgMtx);
    // format log message
    if (itype == LOG_PRINT) {
      if (msg != NULL) {
        free(msg); msg = NULL;
      }
      msg = myvprint("%s",s);
      if (_enableLogPrint) {
        // write to file
        writeMsgToFile(1);
      }

    } else {
      fmtLogMsg(type,s,level,m);
      // write to file
      writeMsgToFile();
    }
    free(s);
    

    // print to console
    if ((stde)||(logf == NULL)||(itype==LOG_PRINT))
      printMsgToConsole();

    smileMutexUnlock(logmsgMtx);
  }
}

// print message to console , without a timestamp
void cSmileLogger::printMsgToConsole()
{
  if (msg != NULL) {
    fprintf(stderr,"%s\n",msg);
    fflush(stderr);
    #ifdef __MINGW32
    fflush(stderr);
    #endif
  }
}

// write a log message in *msg to current logfile (if open)
// add a date- and timestamp to message
void cSmileLogger::writeMsgToFile(int pr)
{
  if ((logf != NULL)&&(msg != NULL)) {
    if (pr==0) {
      // date string
      time_t t;
      time(&t);
      struct tm *ti;
      ti = localtime(&t);
      fprintf(logf,"[ %.2i.%.2i.%.4i - %.2i:%.2i:%.2i ]\n    ",
        ti->tm_mday, ti->tm_mon+1, ti->tm_year+1900,
        ti->tm_hour, ti->tm_min, ti->tm_sec
      );
    }
    // log message
    fprintf(logf,"%s\n",msg);
    fflush(logf);
  }
}

