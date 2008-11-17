/*
 *  ASR.cpp
 *  semaine
 *
 *  Created by Martin Wöllmer on 13.11.08
 */

#include "ASR.h"

#include <cstdlib>
#include <sstream>
#include <decaf/lang/System.h>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/SemaineML.h>

#include <string>

#include <tum/asr/ATKLib/AMonitor.h>
#include <tum/asr/ATKLib/ASource.h>
#include <tum/asr/ATKLib/ACode.h>
#include <tum/asr/ATKLib/ARec.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

namespace semaine {
namespace components {
namespace asr {

ASR::ASR() throw(CMSException) :
	Component("ASR"),
	p(NULL),
	auChan(NULL),
	feChan(NULL),
	ansChan(NULL),
	rman(NULL),
	ain(NULL),
	acode(NULL),
	arec(NULL),
	hset(NULL),
	dict(NULL),
	gram(NULL),
	ngram(NULL),
	amon(NULL)
{
	
	emmaSender = new EmmaSender("semaine.data.state.user", getName());
	senders.push_back(emmaSender);

	static const char * version="[ASR 11/11/08]";
	int argc=3;
	char* argv[3]={"ASR","-C","tum/asr/asr.cfg"};
      	if (InitHTK(argc,argv,version)<SUCCESS){
         	ReportErrors("Main",0); exit(-1);
      	}
}

ASR::~ASR()
{
	delete emmaSender;
 	//if(p!=NULL) delete p;
 	if(auChan!=NULL) delete auChan;
 	if(feChan!=NULL) delete feChan;
 	if(ansChan!=NULL) delete ansChan;
 	if(rman!=NULL) delete rman;
 	if(ain!=NULL) delete ain;
 	if(acode!=NULL) delete acode;
 	if(arec!=NULL) delete arec;
 	if(hset!=NULL) delete hset;
 	if(dict!=NULL) delete dict;
 	if(gram!=NULL) delete gram;
 	if(ngram!=NULL) delete ngram;
 	if(amon!=NULL) delete amon;
}

void ASR::customStartIO() throw(CMSException)
{
	//p = new APacket();

  	ConfParam *cParm[MAXGLOBS];       /* config parameters */
      	int numParm,i;
      	char ngramFN[100],buf[100];
      	ngramFN[0] = '\0';
      	// Read configuration parms for ANGRAM to see if NGram used
      	numParm = GetConfig("ANGRAM", TRUE, cParm, MAXGLOBS);
      	if (numParm>0){
         	if (GetConfStr(cParm,numParm,"NGRAMFILE",buf)) strcpy(ngramFN,buf);
      	}

      	// Create Buffers
      	auChan = new ABuffer("auChan");
      	feChan = new ABuffer("feChan");
      	ansChan = new ABuffer("ansChan");

      	// create a resource manager
      	rman = new ARMan();

      	// Create Audio Source and Coder
      	ain = new ASource("AIn",auChan);
      	acode = new ACode("ACode",auChan,feChan);
      	arec = new ARec("ARec",feChan,ansChan,rman,0);

      	// create global resources
      	hset = new AHmms("HmmSet"); // load info in config
      	dict = new ADict("ADict");
      	gram = new AGram("AGram");
      	rman->StoreHMMs(hset);
      	rman->StoreDict(dict);
      	rman->StoreGram(gram);

      	ResourceGroup *main = rman->NewGroup("main");
      	main->AddHMMs(hset);
      	main->AddDict(dict);
      	main->AddGram(gram);

      	if (strlen(ngramFN)>0){
         	ANGram * ngram  = new ANGram("ANGram");
         	rman->StoreNGram(ngram);
         	main->AddNGram(ngram);
      	}

	#ifdef WITHMON
      	// Create Monitor and Start it
      	amon = new AMonitor();
      	amon->AddComponent(ain);
      	amon->AddComponent(acode);
      	amon->AddComponent(arec);
      	amon->Start();
	#endif

      	// Start components executing
      	ain->Start();
      	acode->Start();
      	arec->Start();
      	arec->SendMessage("usegrp(main)");
      	arec->SendMessage("start()");

      	Boolean terminated = FALSE;

      	ain->SendMessage("start()");
}



void ASR::act() throw(CMSException)
{	
	const char * EXCL = "!SILENCE";
	string sentence="";
	bool newsentence=1;
	float sentstart=0.0;
	float sentend=0.0;
	float sentconf=0.0;
	int wordcounter=0;
      	Boolean terminated = FALSE;

      	while (!terminated) {
         	APacket p = ansChan->GetPacket();
         	if (p.GetKind() == StringPacket){
            		AStringData * sd = (AStringData *)p.GetData();
            		if (sd->data.find("TERMINATED") != string::npos) {
               		terminated = TRUE;
            		}
         	}
	 	if (p.GetKind() == PhrasePacket){
	    		HTime starttime=p.GetStartTime();
	    		HTime endtime=p.GetEndTime();
	    		APhraseData * pd = (APhraseData *)p.GetData();
			if((strcmp(pd->word.c_str(),EXCL)!=0)&&(starttime!=endtime)){
				if(newsentence==1){
					sentstart=p.GetStartTime()/10000;
					newsentence=0;
					sentence=sentence+(pd->word.c_str());
				}
				else{
					sentence=sentence+' '+(pd->word.c_str());
				}
			sentconf=sentconf+pd->confidence;
			wordcounter=wordcounter+1;
			}
			else if(strcmp(pd->word.c_str(),EXCL)==0){
				sentend=p.GetEndTime()/10000;
				terminated = TRUE;

				//num2str for start time
				char strtmp[50];
				sprintf(strtmp,"%.2f",sentstart);
				std::string startstring(strtmp);
				//num2str for end time
				char strtmp2[50];
				sprintf(strtmp2,"%.2f",sentend);
				std::string endstring(strtmp2);
				//num2str for confidence
				char strtmp3[50];
				sprintf(strtmp3,"%.2f",sentconf/wordcounter);
				std::string confstring(strtmp3);

				if(wordcounter!=0) {
					// Create and fill a simple EMMA document
					DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
					DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
					DOMElement * text = XMLTool::appendChildElement(interpretation, SemaineML::E_TEXT, SemaineML::namespaceURI);
					XMLTool::setAttribute(interpretation, EMMA::A_START, startstring);
					XMLTool::setAttribute(interpretation, EMMA::A_END, endstring);
					XMLTool::setAttribute(interpretation, EMMA::A_VERBAL, "true");
					XMLTool::setAttribute(interpretation, EMMA::A_MODE, "voice");
					XMLTool::setAttribute(interpretation, EMMA::A_CONFIDENCE, confstring);
					XMLTool::setAttribute(text, SemaineML::A_NAME, sentence);
					// Now send it
					emmaSender->sendXML(document, meta.getTime());
				}
			}
	 	}
      	}
}


} // namespace asr
} // namespace components
} // namespace semaine
