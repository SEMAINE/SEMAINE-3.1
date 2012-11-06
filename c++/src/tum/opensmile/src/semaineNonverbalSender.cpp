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


#include <semaineNonverbalSender.hpp>
//#include <math.h>

#define MODULE "cSemaineNonverbalSender"

#ifdef HAVE_SEMAINEAPI

SMILECOMPONENT_STATICS(cSemaineNonverbalSender)

SMILECOMPONENT_REGCOMP(cSemaineNonverbalSender)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CSEMAINENONVERBALSENDER;
  sdescription = COMPONENT_DESCRIPTION_CSEMAINENONVERBALSENDER;

  // configure your component's configType:

  SMILECOMPONENT_CREATE_CONFIGTYPE
  //ct->setField("reader", "dataMemory interface configuration (i.e. what slot to read from)", sconfman->getTypeObj("cDataReader"), NO_ARRAY, DONT_FREE);
  ct->setField("dummy","nothing",0);
  ct->setField("laughterString","laughter symbol as output by the ASR engine","*laugh*");
  ct->setField("sighString","sigh symbol as output by the ASR engine","*sigh*");
  ct->setField("breathString","breath symbol as output by the ASR engine","*breathe*");
  SMILECOMPONENT_IFNOTREGAGAIN( {} )

  SMILECOMPONENT_MAKEINFO(cSemaineNonverbalSender);
}

SMILECOMPONENT_CREATE(cSemaineNonverbalSender)



//-----

cSemaineNonverbalSender::cSemaineNonverbalSender(const char *_name) :
  cSemaineEmmaSender(_name)
{
}

void cSemaineNonverbalSender::fetchConfig() 
{
  cSemaineEmmaSender::fetchConfig();

  laughterString = getStr("laughterString");
  sighString = getStr("sighString");
  breathString = getStr("breathString");
}

// this includes the juliusResult struct/class definition
#include <kwsjKresult.h>

void cSemaineNonverbalSender::sendNonverbals( cComponentMessage *_msg )
{
  int i;
  juliusResult *k = (juliusResult *)(_msg->custData);
  if (k==NULL) return;

  int nNv = 0;
  for (i=0; i<k->numW; i++) {
    // check for non-verbals.... and remove them, only preceed if words are left
    if (k->word[i][0] == '*') nNv++;
  }
  if (nNv == 0) return;

  char strtmp[150];
  sprintf(strtmp,"%.2f",_msg->floatData[0]);
  std::string valStr(strtmp);
  long long startTime = smileTimeToSemaineTime(_msg->userTime1);
  sprintf(strtmp,"%ld",startTime);
  std::string startTm(strtmp);
  sprintf(strtmp,"%ld",(long long)round((_msg->userTime2 - _msg->userTime1)*1000.0));
  std::string duration(strtmp);

  for (i=0; i<k->numW; i++) {
    if (k->word[i][0] == '*') {

      // Create and fill a simple EMMA document
      XERCESC_NS::DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
      XMLTool::setPrefix(document->getDocumentElement(), "emma");

      /*
      XERCESC_NS::DOMElement * sequence = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_SEQUENCE);
      XMLTool::setAttribute(sequence, EMMA::A_OFFSET_TO_START, startTm);
      XMLTool::setAttribute(sequence, EMMA::A_DURATION, duration);
      XMLTool::setPrefix(sequence, "emma");
    */
      sprintf(strtmp,"%ld",startTime + (long long)round((k->start[i])*1000.0));
      std::string offs(strtmp);
      sprintf(strtmp,"%.3f",k->conf[i]);
      std::string confidence(strtmp);

      XERCESC_NS::DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
      XMLTool::setAttribute(interpretation, EMMA::A_OFFSET_TO_START, offs);
      XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, confidence);
      XMLTool::setPrefix(interpretation, "emma");
      XERCESC_NS::DOMElement * vocal = XMLTool::appendChildElement(interpretation, SemaineML::E_VOCALIZATION, SemaineML::namespaceURI);

      if ((laughterString!=NULL)&&(!strcmp(k->word[i],laughterString))) {
        XMLTool::setAttribute(vocal, SemaineML::A_NAME, SemaineML::V_LAUGHTER);
      } else if ((sighString!=NULL)&&(!strcmp(k->word[i], sighString))) {
        XMLTool::setAttribute(vocal, SemaineML::A_NAME, SemaineML::V_SIGH);
      } else if ((breathString!=NULL)&&(!strcmp(k->word[i], breathString))) {
        XMLTool::setAttribute(vocal, SemaineML::A_NAME, SemaineML::V_BREATH);
      }
      XMLTool::setPrefix(vocal, "semaine");

      // Now send it
      sendDocument(document);
    }
  }


 
}




int cSemaineNonverbalSender::processComponentMessage( cComponentMessage *_msg ) 
{ 
  if (isMessageType(_msg,"asrKeywordOutput")) {  
    SMILE_IDBG(3,"received 'asrKeywordOutput' message");
    sendNonverbals(_msg);
    return 1;  // message was processed
  }
  
  return 0; // if message was not processed
}  




#endif //HAVE_SEMAINEAPI

