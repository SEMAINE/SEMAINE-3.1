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

the SEMAINE text message sender for openSMILE

*/


#include <semaineTextOutput.hpp>
//#include <math.h>

#define MODULE "cSemaineTextOutput"

SMILECOMPONENT_STATICS(cSemaineTextOutput)

SMILECOMPONENT_REGCOMP(cSemaineTextOutput)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINETEXTOUTPUT;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINETEXTOUTPUT;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("keywordsCurrent", "whether to output keywords from current hypothesis", 0);
  ct->setField("keywordsPass1", "whether to output keywords from pass 1", 0);
  ct->setField("keywordsPass2", "whether to output keywords from pass 2", 1);
  ct->setField("outputFile", "text file where keywords should be written to", (const char*)NULL);
  ct->setField("emoOutputFile", "text file where the affect recognition and other classification results should be written to", (const char*)NULL);
  ct->setField("append", "whether to append to existing keyword files", 0);
  ct->setField("appendEmo", "whether to append to existing affect/classification files", 0);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineTextOutput);
}

SMILECOMPONENT_CREATE(cSemaineTextOutput)



//-----

cSemaineTextOutput::cSemaineTextOutput(const char *_name) :
  cSmileComponent(_name),
  gPtr(0), appendEmo(0), append(0),
  filehandle(NULL), emoFilehandle(NULL)
{
  // initialisation code...
  int i;
  for (i=0; i<N_GENDER_HISTORY; i++) {
    gender[i] = 0;
  }
}

  /*
void cSemaineTextOutput::mySetEnvironment()
{

}

int cSemaineTextOutput::myConfigureInstance()
{

  return 1;
}
*/
int cSemaineTextOutput::myFinaliseInstance()
{
  if (outputFileName != NULL) {
    if (append) {
      filehandle = fopen(outputFileName, "a");
    }
    else {
      filehandle = fopen(outputFileName, "w");
    }
    if (filehandle == NULL) {
      COMP_ERR("Error opening file '%s' for writing (component instance '%s', type '%s')", 
               outputFileName, getInstName(), getTypeName());
    }
  }

  if (emoOutputFileName != NULL) {
    if (appendEmo) {
      emoFilehandle = fopen(emoOutputFileName, "a");
    }
    else {
      emoFilehandle = fopen(emoOutputFileName, "w");
    }
    if (emoFilehandle == NULL) {
      COMP_ERR("Error opening file '%s' for writing (component instance '%s', type '%s')", 
               emoOutputFileName, getInstName(), getTypeName());
    }
  }

  return 1;
}

void cSemaineTextOutput::fetchConfig()
{
  kw[0] = getInt("keywordsCurrent");
  kw[1] = getInt("keywordsPass1");
  kw[2] = getInt("keywordsPass2");
  outputFileName = getStr("outputFile");
  emoOutputFileName = getStr("emoOutputFile");
  append = getInt("append");
  appendEmo = getInt("appendEmo");
}


void cSemaineTextOutput::sendArousalC( cComponentMessage *_msg )
{
  printf("sendArousalC: user time 1: %f; user time 2: %f; arousal: %f\n", 
         _msg->userTime1,
         _msg->userTime2,
         _msg->floatData[0]
         );

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f arousal:%.3f\n",
           _msg->userTime2,
           _msg->floatData[0]);
  }
}

void cSemaineTextOutput::sendValenceC( cComponentMessage *_msg )
{
  printf("sendValenceC: user time 1: %f; user time 2: %f; valence: %f\n", 
         _msg->userTime1,
         _msg->userTime2,
         _msg->floatData[0]
         );

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f valence:%.3f\n",
           _msg->userTime2,
           _msg->floatData[0]);
  }
}

void cSemaineTextOutput::sendPowerC( cComponentMessage *_msg )
{
  printf("sendPowerC: user time 1: %f; user time 2: %f; power: %f\n", 
         _msg->userTime1,
         _msg->userTime2,
         _msg->floatData[0]
         );

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f power:%.3f\n",
           _msg->userTime2,
           _msg->floatData[0]);
  }
}

void cSemaineTextOutput::sendIntensityC( cComponentMessage *_msg )
{
  printf("sendIntensityC: user time 1: %f; user time 2: %f; intensity: %f\n", 
         _msg->userTime1,
         _msg->userTime2,
         _msg->floatData[0]
         );

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f intensity:%.3f\n",
           _msg->userTime2,
           _msg->floatData[0]);
  }
}

void cSemaineTextOutput::sendAnticipationC( cComponentMessage *_msg )
{
  printf("sendAnticipationC: user time 1: %f; user time 2: %f; anticipation: %f\n", 
         _msg->userTime1,
         _msg->userTime2,
         _msg->floatData[0]
         );

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f anticipation:%.3f\n",
           _msg->userTime2,
           _msg->floatData[0]);
  }
}

void cSemaineTextOutput::sendInterestC( cComponentMessage *_msg )
{
  printf("sendInterestC: user time 1: %f; user time 2: %f;",
         _msg->userTime1,
         _msg->userTime2);
  for (int i = 0; i <= 2; ++i) {
    printf(" interest[%d]: %.3f;", i, ((double*)(_msg->custData))[i]);
  }
  printf("\n");

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f interest:%.3f,%.3f,%.3f\n",
           _msg->userTime2,
           ((double*)(_msg->custData))[0],
           ((double*)(_msg->custData))[1],
           ((double*)(_msg->custData))[2]);
  }
}

// start = 1: speaking status changed to start, start = 0: status changed to stop
void cSemaineTextOutput::sendSpeakingStatus( cComponentMessage *_msg, int start )
{
  const char *strtmp = "stop";
  if (start) strtmp = "start";
  printf("sendSpeakingStatus: user time 1: %f; %s\n",
         _msg->userTime1,
         strtmp);
}

void cSemaineTextOutput::sendGender( cComponentMessage *_msg )
{
  const char *strtmp = "female";

  // manage gender result history buffer
  if (_msg->msgtext[0] == 'M') {
    gender[gPtr] = GENDER_M;
  } else {
    gender[gPtr] = GENDER_F;
  }

  gPtr++; 
  if (gPtr >= N_GENDER_HISTORY) gPtr = 0;

  // count # male/female in gender result history
  int m=0,f=0;
  int i;
  for (i=0; i<N_GENDER_HISTORY; i++) {
    if (gender[i] == GENDER_M) m++;
    else if (gender[i] == GENDER_F) f++;
  }

  // majority vote on gender
  if (m>f) { strtmp = "male"; }
  printf("sendGender: user time 1: %f; gender (majority vote): %s\n", _msg->userTime1, strtmp);

  if (emoFilehandle != NULL) {
    fprintf(emoFilehandle, 
    "%f gender:%s\n",
           _msg->userTime2,
           strtmp);
  }
}

void cSemaineTextOutput::sendPitchDirection( cComponentMessage *_msg )
{
  const char *strtmp = "rise"; // _msg->intData[0] == 0
  if (_msg->intData[0] == 1) strtmp = "fall";
  else if (_msg->intData[0] == 2) strtmp = "rise-fall";
  else if (_msg->intData[0] == 3) strtmp = "fall-rise";
  printf("sendPitchDirection: user time 1: %f; direction: %s\n", strtmp);
}


#include <kwsjKresult.h>

void cSemaineTextOutput::sendKeywords( cComponentMessage *_msg )
{
  if (_msg->intData[0] >= 0 && _msg->intData[0] <= 2 && !kw[_msg->intData[0]])
    return;

/*  if (filehandle == NULL) {
    if (append) {
      filehandle = fopen(outputFileName, "a");
    }
    else {
      filehandle = fopen(outputFileName, "w");
    }
    if (filehandle == NULL) {
      COMP_ERR("Error opening file '%s' for writing (component instance '%s', type '%s')", 
               outputFileName, getInstName(), getTypeName());
    }
  }*/
  /*printf("sendKeywords: user time 1: %f; user time 2: %f; keywords:\n",
         _msg->userTime1, 
         _msg->userTime2);*/

  if (filehandle != NULL) {
    int i;
    juliusResult *k = (juliusResult *)(_msg->custData);
    if (k->numW) {
      fprintf(filehandle, "--pass%d--\n", _msg->intData[0]);
      fprintf(filehandle, "#%f\n", _msg->userTime1);
      for (i=0; i<k->numW; i++) {
        fprintf(filehandle, 
          "%f %s %.3f\n",
          _msg->userTime1 + k->start[i],
          k->word[i],
          k->conf[i]);
      }
      fprintf(filehandle, "$%f\n", _msg->userTime2);
    }
  }
}


int cSemaineTextOutput::processComponentMessage( cComponentMessage *_msg ) 
{ 
  if (isMessageType(_msg,"classificationResult")) {  
    // determine origin by message's user-defined name, which can be set in the config file
    SMILE_IDBG(3,"received 'classificationResult' message");
    if (!strcmp(_msg->msgname,"arousal")) sendArousalC(_msg);
    else if (!strcmp(_msg->msgname,"valence")) sendValenceC(_msg);
    else if (!strcmp(_msg->msgname,"interest")) sendInterestC(_msg);
    else if (!strcmp(_msg->msgname,"gender")) sendGender(_msg);
    else if (!strcmp(_msg->msgname,"power")) sendPowerC(_msg);
    else if (!strcmp(_msg->msgname,"intensity")) sendIntensityC(_msg);
    else if (!strcmp(_msg->msgname,"anticipation")) sendAnticipationC(_msg);
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"pitchDirection")) {  
    SMILE_IDBG(3,"received 'pitchDirection' message");
    sendPitchDirection(_msg);
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"asrKeywordOutput")) {  
    SMILE_IDBG(3,"received 'asrKeywordOutput' message");
    sendKeywords(_msg);
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"turnSpeakingStatus")) {
    SMILE_IDBG(2,"received 'turnSpeakingStatus' message: %i",_msg->intData[0]);
    sendSpeakingStatus(_msg, _msg->intData[0]);
    return 1; // message was processed
  }

  return 0; // if message was not processed
}  

int cSemaineTextOutput::myTick(long long t) 
{

  // return 1;  // tick did succeed!

  return 0; // tick did not succeed, i.e. nothing was processed or there was nothing to process
}

cSemaineTextOutput::~cSemaineTextOutput()
{
  // cleanup code...

  if (filehandle != NULL) {
    fclose(filehandle);
  }
  if (emoFilehandle != NULL) {
    fclose(emoFilehandle);
  }
}


