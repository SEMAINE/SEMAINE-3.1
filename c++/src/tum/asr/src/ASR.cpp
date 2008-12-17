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

#include <AMonitor.h>
#include <ARec.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;
using namespace XERCES_CPP_NAMESPACE;

#define HARDC_SAMPR 10

namespace semaine {
namespace components {
namespace asr {

ASR::ASR(char * configfile) throw(CMSException) :
	Component("ASR"),
	p(NULL),
	feChan(NULL),
	ansChan(NULL),
	rman(NULL),
	arec(NULL),
	hset(NULL),
	dict(NULL),
	gram(NULL),
	ngram(NULL),
	amon(NULL),
	wasSpeaking(0),
	nLag(10)
{
	featureReceiver = new FeatureReceiver("semaine.data.analysis.>");
	receivers.push_back(featureReceiver);
	
	emmaSender = new EmmaSender("semaine.data.state.user.emma", getName());
	senders.push_back(emmaSender);

	sentence="";
	newsentence=1;
	sentstart=0.0;
	sentend=0.0;
	sentconf=0.0;
	wordcounter=0;
      	terminated = FALSE;
	featureIndex=NULL;
	nFeaturesSelected = 0;
	curTime = 0;
	speakingIndex = 0;

	prevSpeakingIndex = 0.0;
	thisSpeakingIndex = 0.0;
	countdown = 0;

	static const char * version="[ASR 11/11/08]";
	int argc=3;
	char* argv[3] = {"ASR","-C",NULL};
        if (configfile != NULL) 
          argv[2]=configfile;
        else 
          argv[2]="asr.cfg";

      	if (InitHTK(argc,argv,version)<SUCCESS){
         	ReportErrors("Main",0); exit(-1);
      	}
}

ASR::~ASR()
{
	delete emmaSender;
 	if(feChan!=NULL) delete feChan;
 	if(ansChan!=NULL) delete ansChan;
 	if(rman!=NULL) delete rman;
 	if(arec!=NULL) delete arec;
 	if(hset!=NULL) delete hset;
 	if(dict!=NULL) delete dict;
 	if(gram!=NULL) delete gram;
 	if(ngram!=NULL) delete ngram;
 	if(amon!=NULL) delete amon;

	if(featureIndex!=NULL) delete [] featureIndex;  // free(..) ??
}

void ASR::customStartIO() throw(CMSException)
{

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
      	feChan = new ABuffer("feChan");
      	ansChan = new ABuffer("ansChan");

      	// create a resource manager
      	rman = new ARMan();

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
//      	amon->AddComponent(ain);
//      	amon->AddComponent(acode);
      	amon->AddComponent(arec);
      	amon->Start();
	#endif

      	arec->Start();
      	arec->SendMessage("usegrp(main)");
      	arec->SendMessage("start()");

      	Boolean terminated = FALSE;

//      	ain->SendMessage("start()");
}

// search all names for the given one, return index of found feature or -1 else
int ASR::findFeature(std::string name, std::vector<std::string> names)
{
	int i;
	for (i=0; i<names.size(); i++) {
		if (name == names[i]) return i;
	}
	return -1;
}

void ASR::setupFeatureNameLookup(SEMAINEFeatureMessage *fm)
{
	if ((nFeaturesSelected > 0)&&(featureIndex != NULL)) return;

	std::vector<std::string> featureNames = fm->getFeatureNames();
	int n = 39; 
	int i;

	nFeaturesSelected = n;
	featureIndex = new int[n];
	featureIndex[0] = findFeature("mfccz1",featureNames);
	featureIndex[1] = findFeature("mfccz2",featureNames);
	featureIndex[2] = findFeature("mfccz3",featureNames);
	featureIndex[3] = findFeature("mfccz4",featureNames);
	featureIndex[4] = findFeature("mfccz5",featureNames);
	featureIndex[5] = findFeature("mfccz6",featureNames);
	featureIndex[6] = findFeature("mfccz7",featureNames);
	featureIndex[7] = findFeature("mfccz8",featureNames);
	featureIndex[8] = findFeature("mfccz9",featureNames);
	featureIndex[9] = findFeature("mfccz10",featureNames);
	featureIndex[10] = findFeature("mfccz11",featureNames);
	featureIndex[11] = findFeature("mfccz12",featureNames);
	featureIndex[12] = findFeature("mfccz0",featureNames);


	featureIndex[13] = findFeature("mfccz1-De",featureNames);
	featureIndex[14] = findFeature("mfccz2-De",featureNames);
	featureIndex[15] = findFeature("mfccz3-De",featureNames);
	featureIndex[16] = findFeature("mfccz4-De",featureNames);
	featureIndex[17] = findFeature("mfccz5-De",featureNames);
	featureIndex[18] = findFeature("mfccz6-De",featureNames);
	featureIndex[19] = findFeature("mfccz7-De",featureNames);
	featureIndex[20] = findFeature("mfccz8-De",featureNames);
	featureIndex[21] = findFeature("mfccz9-De",featureNames);
	featureIndex[22] = findFeature("mfccz10-De",featureNames);
	featureIndex[23] = findFeature("mfccz11-De",featureNames);
	featureIndex[24] = findFeature("mfccz12-De",featureNames);
	featureIndex[25] = findFeature("mfccz0-De",featureNames);

	featureIndex[26] = findFeature("mfccz1-De-De",featureNames);
	featureIndex[27] = findFeature("mfccz2-De-De",featureNames);
	featureIndex[28] = findFeature("mfccz3-De-De",featureNames);
	featureIndex[29] = findFeature("mfccz4-De-De",featureNames);
	featureIndex[30] = findFeature("mfccz5-De-De",featureNames);
	featureIndex[31] = findFeature("mfccz6-De-De",featureNames);
	featureIndex[32] = findFeature("mfccz7-De-De",featureNames);
	featureIndex[33] = findFeature("mfccz8-De-De",featureNames);
	featureIndex[34] = findFeature("mfccz9-De-De",featureNames);
	featureIndex[35] = findFeature("mfccz10-De-De",featureNames);
	featureIndex[36] = findFeature("mfccz11-De-De",featureNames);
	featureIndex[37] = findFeature("mfccz12-De-De",featureNames);
	featureIndex[38] = findFeature("mfccz0-De-De",featureNames);


	speakingIndex = findFeature("speaking",featureNames);
}


// get features and put into packet
APacket ASR::makeFeaturePacket(SEMAINEFeatureMessage *fm)
{
	std::vector<float> features = fm->getFeatureVector();
	int i;

	BufferInfo info;
	info.srcPK = MFCC + HASZEROC + HASDELTA + HASACCS + HASZEROM ;
//	info.srcFF = 0; //WAVEFILE;
	info.srcSampRate = 625;
	info.frSize = 39*4;
	info.frRate = 100000;
	info.tgtPK = MFCC + HASZEROC + HASDELTA + HASACCS + HASZEROM ;
	info.tgtSampRate = 100000;
	info.tgtVecSize = nFeaturesSelected;
	info.spDetSil = 0;
	info.chPeak = 0;
	info.spDetSp = 0;
	info.spDetSNR = 0;
	info.spDetThresh = 0;
	info.curVol = 0;
	info.matTranFN = NULL;

	AObsData * o = new AObsData(&info, 1);

//	o->data.vq[0] = (features[speakingIndex]==0.0)?0:1;

	prevSpeakingIndex = thisSpeakingIndex;
	thisSpeakingIndex = features[speakingIndex];
	if (prevSpeakingIndex==1.0 && thisSpeakingIndex==0.0) {
		printf("turn end detected, now waiting 20 frames to compute best word hypothesis\n");
		countdown=20;
 	}
	if (countdown > 0) {
		countdown=countdown-1;
		if ((countdown == 0)&&(features[speakingIndex]!=1.0))
		  printf(" stopping decoder. recognition result will appear soon.\n");
 	}
	if ((countdown > 0) || (features[speakingIndex]==1.0)) {
		o->data.vq[0] = 1.0;
	} else {
		o->data.vq[0] = 0.0;
	}
	if (prevSpeakingIndex==0.0 && thisSpeakingIndex==1.0) {
		printf("turn start detected.\n");		
	}

//if ((features[speakingIndex]==0.0)&&(wasSpeaking==1)) { o->data.vq[0] = 1; wasSpeaking=0; }
//if ((features[speakingIndex]==1.0)) { wasSpeaking=1; }
//	o->data.fv[1][0] = (float)nFeaturesSelected;

	for (i=0; i<nFeaturesSelected; i++) {
		if (featureIndex[i] >= 0) {
			o->data.fv[1][i+1] = features[featureIndex[i]];
		} else
			o->data.fv[1][i+1] = 0.0;

	}
	APacket pkt(o);

	// TODO: USE SEMAINE TIMES HERE::: 
	pkt.SetStartTime(curTime);
	curTime += HARDC_SAMPR * 10000; //info.tgtSampRate;
	pkt.SetEndTime(curTime);
	// 
	return pkt;
}

void ASR::react(SEMAINEMessage * m) throw(CMSException)
{
	SEMAINEFeatureMessage * fm = dynamic_cast<SEMAINEFeatureMessage *>(m);
	if (fm != NULL) {
		setupFeatureNameLookup(fm);
		APacket p = makeFeaturePacket(fm);
		feChan->PutPacket(p);
	}

}


void ASR::act() throw(CMSException)
{	
	const char * EXCL = "!SILENCE";
/*	
	string sentence="";
	bool newsentence=1;
	float sentstart=0.0;
	float sentend=0.0;
	float sentconf=0.0;
	int wordcounter=0;
      	Boolean terminated = FALSE;
*/
      	//while (!terminated) {
        if (ansChan->DataAvailable()) {

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
				cerr << sentence << "\n";
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
					XMLTool::appendChildTextNode(text, sentence);
					// Now send it
					emmaSender->sendXML(document, meta.getTime());
				}
				sentence="";
				newsentence=1;
			}
	 	}
      	}
}


} // namespace asr
} // namespace components
} // namespace semaine
