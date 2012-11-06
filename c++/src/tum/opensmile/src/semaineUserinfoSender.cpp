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

the SEMAINE Emma message sender for openSMILE

*/


#include <semaineUserinfoSender.hpp>
//#include <math.h>

#define MODULE "cSemaineUserinfoSender"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cSemaineUserinfoSender)

SMILECOMPONENT_REGCOMP(cSemaineUserinfoSender)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINEUSERINFOSENDER;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINEUSERINFOSENDER;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("dummy","nothing",0);
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineUserinfoSender);
}

SMILECOMPONENT_CREATE(cSemaineUserinfoSender)



//-----

cSemaineUserinfoSender::cSemaineUserinfoSender(const char *_name) :
  cSemaineEmmaSender(_name),
  gPtr(0)
{
  // initialisation code...
  int i;
  for (i=0; i<N_GENDER_HISTORY; i++) {
    gender[i] = 0;
  }
}


// start = 1: speaking status changed to start, start = 0: status changed to stop
void cSemaineUserinfoSender::sendSpeakingStatus( cComponentMessage *_msg, int start )
{
  const char *strtmp = "stop";
  if (start) strtmp = "start";
  
  std::string statusStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * speaking = XMLTool::appendChildElement(interpretation, SemaineML::E_SPEAKING, SemaineML::namespaceURI);
  XMLTool::setAttribute(speaking, SemaineML::A_STATUS_CHANGE, strtmp);
  XMLTool::setPrefix(speaking, "semaine");

  // Now send it
  sendDocument(document);
}


void cSemaineUserinfoSender::sendGender( cComponentMessage *_msg )
{
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:confidence="0.3">

      <semaine:gender name="female" xmlns:semaine="http://www.semaine-project.eu/semaineml"/>

    </emma:interpretation>
</emma:emma>
*/
  
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


  // fill XML document
  std::string genderStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * gender = XMLTool::appendChildElement(interpretation, SemaineML::E_GENDER, SemaineML::namespaceURI);
  XMLTool::setAttribute(gender, SemaineML::A_NAME, strtmp);
  XMLTool::setPrefix(gender, "semaine");

  // Now send it
  sendDocument(document);
}

void cSemaineUserinfoSender::sendPitchDirection( cComponentMessage *_msg )
{
/*
<emma:emma version="1.0" xmlns:emma="http://www.w3.org/2003/04/emma">
    <emma:interpretation  emma:offset-to-start="12345" emma:duration="444" emma:confidence="0.3">

        <semaine:pitch xmlns:semaine="http://www.semaine-project.eu/semaineml" direction="rise"/>

    </emma:interpretation>
</emma:emma>
*/

  const char *strtmp = "rise"; // _msg->intData[0] == 0
  if (_msg->intData[0] == 1) strtmp = "fall";
  else if (_msg->intData[0] == 2) strtmp = "rise-fall";
  else if (_msg->intData[0] == 3) strtmp = "fall-rise";
  std::string pitchStr(strtmp);

  char strtmp2[50];
  sprintf(strtmp2,"%ld",smileTimeToSemaineTime(_msg->userTime1));
  std::string startTm(strtmp2);

  // Create and fill a simple EMMA EmotionML document
  XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
  XMLTool::setPrefix(document->getDocumentElement(), "emma");

  XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
  XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, startTm);
  XMLTool::setAttribute(interpretation, EMMA::A_DURATION, "0");
  XMLTool::setPrefix(interpretation, "emma");

  XERCESC_NS::DOMElement * pitchDir = XMLTool::appendChildElement(interpretation, SemaineML::E_PITCH, SemaineML::namespaceURI);
  XMLTool::setAttribute(pitchDir, SemaineML::A_DIRECTION, strtmp);
  XMLTool::setPrefix(pitchDir, "semaine");

  // Now send it
  sendDocument(document);
}



int cSemaineUserinfoSender::processComponentMessage( cComponentMessage *_msg ) 
{ 
  if (isMessageType(_msg,"classificationResult")) {  
     if (!strcmp(_msg->msgname,"gender")) sendGender(_msg);
  }
  else if (isMessageType(_msg,"pitchDirection")) {  
    SMILE_IDBG(3,"received 'pitchDirection' message");
    sendPitchDirection(_msg);
    return 1;  // message was processed
  }
  else if (isMessageType(_msg,"turnSpeakingStatus")) {
    SMILE_IDBG(3,"received 'turnSpeakingStatus' message: %i",_msg->intData[0]);
    sendSpeakingStatus(_msg, _msg->intData[0]);
    return 1; // message was processed
  }
  return 0; // if message was not processed
}  




#endif //HAVE_SEMAINEAPI

