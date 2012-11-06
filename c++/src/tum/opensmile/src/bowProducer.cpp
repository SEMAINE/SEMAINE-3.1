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


/*  openSMILE component:

example dataSource
writes data to data memory...

*/


#include <bowProducer.hpp>
#define MODULE "cBowProducer"

SMILECOMPONENT_STATICS(cBowProducer)

SMILECOMPONENT_REGCOMP(cBowProducer)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CBOWPRODUCER;
  sdescription = COMPONENT_DESCRIPTION_CBOWPRODUCER;

  // we inherit cDataSource configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataSource")
  
  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("kwList","text file with list of keywords (one word per line) to use for BoW",(const char*)NULL);
    ct->setField("kwListPrefixFilter","keywords in kwList file are expected to have a prefix 'prefix'. Only those keywords will be loaded from the list, everything else will be ignored (i.e. line not beginning with 'prefix').",0);
    ct->setField("prefix","prefix to append to keywords to create feature names","BOW_");
    ct->setField("count","1=count frequency of keyword in input / 0=binary output only (keyword present/not present)",0);
    //ct->setField("useConf","1=use confidence instead of count or binary word presence",0);
  )

  SMILECOMPONENT_MAKEINFO(cBowProducer);
}

SMILECOMPONENT_CREATE(cBowProducer)

//-----

cBowProducer::cBowProducer(const char *_name) :
  cDataSource(_name),kwList(NULL),count(0),numKw(0),
  keywords(NULL), prefix(NULL), dataFlag(0)
{
}

void cBowProducer::fetchConfig()
{
  cDataSource::fetchConfig();
  
  kwList = getStr("kwList");
  SMILE_IDBG(2,"kwList = '%s'",kwList);
  kwListPrefixFilter = getInt("kwListPrefixFilter");
  SMILE_IDBG(2,"kwListPrefixFilter = '%i'",kwListPrefixFilter);

  count = getInt("count");
  SMILE_IDBG(2,"count = %i",count);
  prefix = getStr("prefix");
  SMILE_IDBG(2,"prefix = %i",prefix);
}

/*
int cBowProducer::myConfigureInstance()
{
  int ret = cDataSource::myConfigureInstance();
  // ...

  return ret;
}
*/

int cBowProducer::configureWriter(sDmLevelConfig &c)
{
  // configure your writer by setting values in &c
  c.T = 0; /* a-periodic level */
  return 1;
}

/*
int isEmptyLine(const char *line, int n)
{
  int i;
  int empty=1;
  if ((n==0)||(n>strlen(line))) n=strlen(line);
  for (i=0; i<n; i++) {
    if ( (line[i] != ' ')&&(line[i] != '\t')&&(line[i] != '\n') ) {
      empty = 0; break;
    }
  }
  return empty;
}
*/

int cBowProducer::loadKwList()
{
  if (kwList != NULL) {
    FILE * f = fopen(kwList,"r");
    if (f==NULL) {
      SMILE_IERR(1,"cannot open keyword list file '%s' (fopen failed)!",kwList);
      return 0;
    }
    int lineNr=0;
    int ret=1;
    char *line = NULL;
    size_t n,read; int len;

    do {
      read = getline(&line, &n, f);
      if ((read != (size_t)-1)&&(line!=NULL)) {
        lineNr++;
        len=(int)strlen(line);
        if (len>0) { if (line[len-1] == '\n') { line[len-1] = 0; len--; } }
        if (len>0) { if (line[len-1] == '\r') { line[len-1] = 0; len--; } }
        while (((line[0] == ' ')||(line[0] == '\t'))&&(len>=0)) { line[0] = 0; line++; len--; }
        if (len > 0) {
          if ((!kwListPrefixFilter)||(prefix == NULL)||(!strncmp(line,prefix,MIN(strlen(prefix),strlen(line))))) {
            numKw++;
            keywords = (char**)realloc(keywords,numKw*sizeof(char*));
            keywords[numKw-1] = strdup(line);
            SMILE_IDBG(4,"read keyword '%s' from keyword list",keywords[numKw-1]);
          }
        }
      } else { break; }
    } while (read != (size_t)-1);
    if (line != NULL) free(line);

    SMILE_IDBG(4,"read %i keywords from keyword list '%s'",numKw,kwList);
    fclose(f);
    return 1;
  }
  return 0;
}

// NOTE: nEl is always 0 for dataSources....
int cBowProducer::setupNewNames(long nEl)
{
  /* load names from keyword list file */
  if (!loadKwList()) COMP_ERR("failed loading keyword list '%s'",kwList);

  // configure dateMemory level, names, etc.
  int i;
  for (i=0; i<numKw; i++) {
    char *tmp;
    if ((kwListPrefixFilter)&&(prefix != NULL)) {
      writer->addField(keywords[i],1);
    } else {
      if (prefix != NULL) {
        tmp = myvprint("%s%s",prefix,keywords[i]);
      } else {
        tmp = myvprint("BOW_%s",keywords[i]);
      }
      writer->addField(tmp,1);
      free(tmp);
    }
  }

  namesAreSet=1;
  allocVec(numKw); //??
  return numKw;
}

/*
int cBowProducer::myFinaliseInstance()
{
  return cDataSource::myFinaliseInstance();
}
*/
#include <kwsjKresult.h>

int cBowProducer::buildBoW( cComponentMessage *_msg )
{
  // build bag of words vector in 'vec' based on keywords in message
  //Kresult *k = (Kresult *)(_msg->custData);
  juliusResult *k = (juliusResult *)(_msg->custData);
  int i,j;
    // find keyword in list
    // k->keyword[i]
    // set index in vector
  for (j=0; j<numKw; j++) {
    int found = 0;
    for (i=0; i<k->numW; i++) {
      // NOTE: keyword comparison is case INsensitive ! is that ok?
      if (!strcasecmp( k->word[i], keywords[j] )) {
        vec->dataF[j] = 1.0; found=1;
        break;
      }
    }
    if (found==0) vec->dataF[j] = 0.0;
  }
  return 0;
}

int cBowProducer::processComponentMessage( cComponentMessage *_msg )
{
  if (isMessageType(_msg,"turnFrameTime")) {
    // write current keyword status
    SMILE_IDBG(3,"received 'turnFrameTime' message");
    writer->setNextFrame(vec);
    dataFlag = 1;
    //return queNextFrameData(_msg->floatData[0], _msg->floatData[1], _msg->intData[0]);
  } else if (isMessageType(_msg,"asrKeywordOutput")) {  
    SMILE_IDBG(3,"received 'asrKeywordOutput' message");
    buildBoW(_msg);
    return 1;  // message was processed
  }
  return 0;
}


int cBowProducer::myTick(long long t)
{
  if (dataFlag) {
    dataFlag = 0;
    return 1;
  } else {
    return 0;
  }
}


cBowProducer::~cBowProducer()
{
  int i;
  if (keywords != NULL) {
    for (i=0; i<numKw; i++) {
      free(keywords[i]);
    } 
    free(keywords);
  }
  // ..
}
