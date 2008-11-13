//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// 
//
//////////////////////////////////////////////////////////////////////

#include ".\FML-APML_AgentEngine.h"


#include "BML_AgentEngine.h"
#include "GretaXMLParser.h"

extern IniManager inimanager;
extern std::string ini_filename;
extern FileNames filenames;


extern FILE* gesture_log;
extern FILE* face_log;
extern RandomGen *randomgen;


FMLAPML_AgentEngine::FMLAPML_AgentEngine(void)
{
	if(!inimanager.initialized)
		inimanager.ReadIniFile(ini_filename);

	if(randomgen==0)
		randomgen = new RandomGen();

	apml=0;
	logfile=fopen("logs/fml-apml_engine_log.html","w");
	fprintf(logfile,"<html>\n<head>\n</head>\n<body>\n");
}

FMLAPML_AgentEngine::~FMLAPML_AgentEngine(void)
{
	fprintf(logfile,"</body>\n</html>\n");
	fclose(logfile);
	if(apml!=0)
		delete apml;
	if(randomgen!=0)
	{
		delete randomgen;
		randomgen=0;
	}
}

int FMLAPML_AgentEngine::loadBaseline(std::string baselinefilename,char *buffer)
{
	if (baselinefilename!="")
	{
		bl.Load(baselinefilename,"");
		return 1;
	}
	else
	{
		bl.Load("",buffer);
	}
}
int FMLAPML_AgentEngine::loadBQC(std::string bqcfilename,char *buffer)
{
	if (bqcfilename!="")
	{
		bqc.LoadQualifiersList(bqcfilename,"");
		return 1;
	}
	else
	{
		bqc.LoadQualifiersList("",buffer);		
		return 1;
	}
	return 0;
}

int FMLAPML_AgentEngine::loadMMS(std::string mmsfilename,char *buffer)
{
	if (mmsfilename!="")
	{
		mss.LoadSelectionLexicon(mmsfilename,"");
		return 1;
	}
	else
	{
		mss.LoadSelectionLexicon("",buffer);
		return 1;	
	}
	return 0;
}

int FMLAPML_AgentEngine::LoadFMLAPML(std::string FMLAPMLfilename,char *buffer)
{
	XMLGenericParser *p;
	std::list<XMLGenericTree*>::iterator iterch;
	std::list<XMLGenericTree*>::iterator iterbl;

	filenames.BuildFileNames(FMLAPMLfilename);

	p=new XMLGenericParser();

	p->SetValidating(true);
	if(FMLAPMLfilename!="")
		apml=p->ParseFile(FMLAPMLfilename);
	else
		apml=p->ParseBuffer(buffer);

	delete p;
	if(apml!=0)
	{
		// Recording of the speech parameters
		if(apml->FindNodeCalled("speech")!=0)
		{
			speech.StoreBML(apml->FindNodeCalled("speech"),inimanager);
			/*{
				delete apml;
				apml=0;
				return 0;
			}*/
			speech.TemporizeTimeMarkers();

			/*{
				delete apml;
				apml=0;
				return 0;
			}*/
			
			StoreCommunicativeIntentions(apml->FindNodeCalled("speech"),"pitchaccent");
			StoreCommunicativeIntentions(apml->FindNodeCalled("speech"),"boundary");

			speech.concretized=true;
		}
		StoreCommunicativeIntentions(apml->FindNodeCalled("fml"));
	}
	else
	{
		printf("Error parsing %s\n", FMLAPMLfilename.c_str());
		return 0;
	}

	delete apml;

	apml=0;

	return 1;
}

void FMLAPML_AgentEngine::StoreCommunicativeIntentions(XMLGenericTree *t,std::string tagsname)
{
	std::list<XMLGenericTree*>::iterator iter;

	if(t==0)
		return;

	for(iter=t->child.begin();iter!=t->child.end();iter++)
	{
		if((*iter)->name==tagsname)
		{
			MMSystemSpace::CommunicativeIntention ca;
			ca.id=(*iter)->GetAttribute("id");
			ca.name=(*iter)->name;
			if(ca.name=="emotion")
				ca.isemotion=true;
			if(ca.name=="emphasis")
				ca.isemphasis=true;
			if(ca.name=="pitchaccent")
				ca.ispitchaccent=true;
			ca.type=(*iter)->GetAttribute("type");
			ca.start_sym=(*iter)->GetAttribute("start");
			ca.dur_sym=(*iter)->GetAttribute("end");
			if((*iter)->HasAttribute("importance"))
				ca.importance=(*iter)->GetAttributef("importance");

			//radek
			if((*iter)->HasAttribute("intensity"))
				ca.SetIntensity(atof((*iter)->GetAttribute("intensity").c_str()));

			if((*iter)->HasAttribute("target"))
				ca.target=(*iter)->GetAttribute("target");
			commacts.push_back(ca);
		}
	}
}




void FMLAPML_AgentEngine::StoreCommunicativeIntentions(XMLGenericTree *t)
{
	std::list<XMLGenericTree*>::iterator iter;

	if(t==0)
		return;

	for(iter=t->child.begin();iter!=t->child.end();iter++)
	{
		MMSystemSpace::CommunicativeIntention ca;
		ca.id=(*iter)->GetAttribute("id");
		ca.name=(*iter)->name;
		if(ca.name=="emotion")
			ca.isemotion=true;
		if(ca.name=="emphasis")
			ca.isemphasis=true;
		if(ca.name=="pitchaccent")
			ca.ispitchaccent=true;
		ca.type=(*iter)->GetAttribute("type");
		ca.start_sym=(*iter)->GetAttribute("start");
		ca.dur_sym=(*iter)->GetAttribute("end");
		if((*iter)->HasAttribute("importance"))
			ca.importance=(*iter)->GetAttributef("importance");

		//radek
		if((*iter)->HasAttribute("intensity"))
			ca.SetIntensity(atof((*iter)->GetAttribute("intensity").c_str()));


		if((*iter)->HasAttribute("target"))
				ca.target=(*iter)->GetAttribute("target");
		ca.ref_type=(*iter)->GetAttribute("ref_type");
		ca.ref_id=(*iter)->GetAttribute("ref_id");
		ca.prop_type=(*iter)->GetAttribute("prop_type");
		ca.prop_value=(*iter)->GetAttribute("prop_value");
		commacts.push_back(ca);
		//std::cout<<" Communicative intention :"<<ca.name <<" This.start : "<< ca.start_sym<<" end : "<<ca.dur_sym << std::endl;
			
	}
}

int FMLAPML_AgentEngine::TemporizeCommunicativeIntentions(void)
{

	std::vector<MMSystemSpace::CommunicativeIntention>::iterator iter;
	int loopcount;

	if(commacts.empty()==true)
		return 1;

	for(iter=commacts.begin();iter!=commacts.end();iter++)
	{
		loopcount=0;
	
		if((*iter).Temporize(commacts,&speech,&loopcount)==0)
			return 0;
		//std::cout<<" Communicative intention :"<<(*iter).name <<" This.start : "<< (*iter).start<<" end : "<<(*iter).duration <<" This.startTM : "<< (*iter).start_sym<<" endTM : "<<(*iter).dur_sym << std::endl;
	}

	return 1;
}

void FMLAPML_AgentEngine::DefineCommunicativeContexts(void)
{
	std::vector<MMSystemSpace::CommunicativeIntention>::iterator iter;
	if(commacts.empty()==true)
		return;

	fprintf(logfile,"<table border='1'>\n");

	for(iter=commacts.begin();iter!=commacts.end();iter++)
	{
		if((*iter).concretized==true)
			(*iter).DefineContext(commacts);
		fprintf(logfile,"<tr><td>\n");
		(*iter).Print(logfile);
		fprintf(logfile,"</tr></td>\n");
	}

	fprintf(logfile,"</table>\n");

}

int FMLAPML_AgentEngine::reset()
{
	return 1;
}



std::string FMLAPML_AgentEngine::Execute(char *buffer)
{
	dl=MMSystemSpace::Dynamicline();	
	apml = new XMLGenericTree();
	std::string bml;

	XMLGenericParser *p;
	p=new XMLGenericParser();
	p->SetValidating(true);
	apml=p->ParseBuffer(buffer);
	delete p;

	commacts = std::vector<MMSystemSpace::CommunicativeIntention>() ;
	speech = MMSystemSpace::Signal();
	
	if(LoadFMLAPML("",buffer)==0)
		return "";

	
	if(TemporizeCommunicativeIntentions()==0)
		return "";
		
	std::sort(commacts.begin(),commacts.end());
	DefineCommunicativeContexts();
	
	bml="";

	int uid;

	uid=0;
	
	bml="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	bml=bml+"<!DOCTYPE bml SYSTEM \"bml.dtd\" []>\n";
	bml=bml+"<bml>\n";

	bml=bml+speech.GetBML(&uid);

	//TODO: check the following instruction for bugs
	//bl.SaveCSV(-1,preferencefile,csvgesturefile,csvtorsofile,csvheadfile,csvfacefile,csvgazefile);

	MMSystemSpace::CommunicativeIntention *contextact;

	std::vector<MMSystemSpace::MultimodalSignal*> *mmsignals;
	MMSystemSpace::MultimodalSignal *mmsignal;

	std::vector<MMSystemSpace::CommunicativeIntention>::iterator iter;

	//for each commact
	for(iter=commacts.begin();iter!=commacts.end();iter++)
	{
		dl=bqc.ComputeDynamicline(&(*iter),bl);

		contextact=(*iter).GetActFromContext("emotion");

		if(contextact!=0)
			dl=bqc.ComputeDynamicline(contextact,dl);

		contextact=(*iter).GetActFromContext("emphasis");
		if(contextact!=0)
			dl=bqc.ComputeDynamicline(contextact,dl);

		contextact=(*iter).GetActFromContext("rheme");
		if(contextact!=0)
			dl=bqc.ComputeDynamicline(contextact,dl);

		//dl.SaveCSV((*iter).start,preferencefile,csvgesturefile,csvtorsofile,csvheadfile,csvfacefile,csvgazefile);

		//choose a list of available mms for a commact - the last parameter is a list of busymodalities
		mmsignals=mss.SelectMultimodalSignals((*iter),
			&dl,
			(*iter).GetContextUsedModalities());

		//chose one mms from the previous list 
		mmsignal=mss.SelectMultimodalSignal(mmsignals,(*iter),&dl);
	
	if(mmsignal!=0)
		{
			
			//std::cout <<"."<< std::ends;	
			mmsignal->AssignNameToSignals((*iter).name);
			mmsignal->AddIntensity( (*iter).GetIntensity()   );
			mmsignal->AssignParameters(&dl);
			mmsignal->ShuffleShapes();
			mmsignal->AssignStrokesFromStressVector(&(*iter),&speech.stressingpoints);
			(*iter).UsedModalities=mmsignal->UsedModalities();
		
				if ((*iter).isemotion==true) 
				{
	
				//there is one mmsignal chosen with different modalities (all modalities)
				//time constraints should be considered and modalities should be ordered
				// if we want use two different signal on the same modality SelectMultimodalSignals should allow this before
				//thus we suppose we have a mmsignal that can contain different signals even for the same modality
				//now we can order them according to the time-constraints
				//each signal has its own start and end time
				//before using "addTimeConstraints" method all signals had the same start and end (see: temporize() )
					
					if(inimanager.GetValueInt("MULTIMODAL_EMOTIONS")==1)
								mmsignal->addTimeConstraints((*iter).type.c_str());
			}
		
			(*iter).instantiated=true;
			
			//each signal of a given multimodal signal is changed to bml separately
			
			//inviarlo su psyclone attaccando l'header
			bml=bml+mmsignal->GetBML(&uid);
		}
	}
	bml=bml+"</bml>\n";
	//std::cout <<"A calcule : "<< bml <<std::endl;
	FILE *f=fopen(filenames.FMLAPML_to_BML_File.c_str(),"w");
	//FILE *f=fopen((inimanager.Program_Path+"bml/from-fml-apml.xml").c_str(),"w");
	if(f!=0)
	{
		fprintf(f,"%s",bml.c_str());
		fclose(f);
	}
		
	
	return bml;
}

// call by greta Modular
std::string FMLAPML_AgentEngine::Execute(std::string FMLAPMLfilename,std::string baselinefilename,char *buffer, int realtime)
{
	if(LoadFMLAPML(FMLAPMLfilename,buffer)==0)
		return "";

	std::string bml;

	BML_AgentEngine bae;
	
	if(TemporizeCommunicativeIntentions()==0)
		return "";

	std::sort(commacts.begin(),commacts.end());
	DefineCommunicativeContexts();

	this->loadBaseline(baselinefilename,"");

	bqc=MMSystemSpace::BehaviorQualityComputation("mmsystem/behaviorqualifiers.xml");
	mss=MMSystemSpace::MultimodalSignalSelection("mmsystem/lexicon.xml");
	
	bml="";

	int uid;

	uid=0;
	
	bml="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
	if (realtime==0)
		bml=bml+"<!DOCTYPE bml SYSTEM \"bml.dtd\" []>\n";
	else
		bml=bml+"<!DOCTYPE bml SYSTEM \"bml/bml.dtd\" []>\n";
	bml=bml+"<bml>\n";

	if(speech.concretized==true)
		bml=bml+speech.GetBML(&uid);

	//commentare questi file!!!
/*	FILE *csvgesturefile;
	FILE *csvtorsofile;
	FILE *csvheadfile;
	FILE *csvfacefile;
	FILE *csvgazefile;
	FILE *preferencefile;
	csvgesturefile=fopen("logs/gesture.csv","w");
	csvtorsofile=fopen("logs/torso.csv","w");
	csvheadfile=fopen("logs/head.csv","w");
	csvfacefile=fopen("logs/face.csv","w");
	csvgazefile=fopen("logs/gaze.csv","w");
	preferencefile=fopen("logs/preference.csv","w");*/

	//TODO: check the following instruction for bugs
	//bl.SaveCSV(-1,preferencefile,csvgesturefile,csvtorsofile,csvheadfile,csvfacefile,csvgazefile);

	MMSystemSpace::CommunicativeIntention *contextact;

	std::vector<MMSystemSpace::MultimodalSignal*> *mmsignals;
	MMSystemSpace::MultimodalSignal *mmsignal;

	std::vector<MMSystemSpace::CommunicativeIntention>::iterator iter;

	//for each commact
	for(iter=commacts.begin();iter!=commacts.end();iter++)
	{
		dl=bqc.ComputeDynamicline(&(*iter),bl);

		contextact=(*iter).GetActFromContext("emotion");

		if(contextact!=0)
			dl=bqc.ComputeDynamicline(contextact,dl);

		contextact=(*iter).GetActFromContext("emphasis");
		if(contextact!=0)
			dl=bqc.ComputeDynamicline(contextact,dl);

		contextact=(*iter).GetActFromContext("rheme");
		if(contextact!=0)
			dl=bqc.ComputeDynamicline(contextact,dl);

		//dl.SaveCSV((*iter).start,preferencefile,csvgesturefile,csvtorsofile,csvheadfile,csvfacefile,csvgazefile);

		//choose a list of available mms for a commact - the last parameter is a list of busymodalities
		mmsignals=mss.SelectMultimodalSignals((*iter),
			&dl,
			(*iter).GetContextUsedModalities());

		//chose one mms from the previous list 
		mmsignal=mss.SelectMultimodalSignal(mmsignals,(*iter),&dl);
			
	
	if(mmsignal!=0)
		{
			mmsignal->AssignNameToSignals((*iter).name);
			mmsignal->AddIntensity( (*iter).GetIntensity()   );
			mmsignal->AssignParameters(&dl);
			mmsignal->ShuffleShapes();
			mmsignal->AssignStrokesFromStressVector(&(*iter),&speech.stressingpoints);
			//mmsignal->SaveCSV((*iter).start,preferencefile,csvgesturefile,csvtorsofile,csvheadfile,csvfacefile,csvgazefile);
			(*iter).UsedModalities=mmsignal->UsedModalities();
		
				if ((*iter).isemotion==true) 
				{
	
				//there is one mmsignal chosen with different modalities (all modalities)
				//time constraints should be considered and modalities should be ordered
				// if we want use two different signal on the same modality SelectMultimodalSignals should allow this before
				//thus we suppose we have a mmsignal that can contain different signals even for the same modality
				//now we can order them according to the time-constraints
				//each signal has its own start and end time
				//before using "addTimeConstraints" method all signals had the same start and end (see: temporize() )
					
					if(inimanager.GetValueInt("MULTIMODAL_EMOTIONS")==1)
								mmsignal->addTimeConstraints((*iter).type.c_str());
			}
		
			(*iter).instantiated=true;
			
			//each signal of a given multimodal signal is changed to bml separately
			bml=bml+mmsignal->GetBML(&uid);
		}
	}

	/*fclose(csvgesturefile);
	fclose(csvtorsofile);
	fclose(csvheadfile);
	fclose(csvfacefile);
	fclose(csvgazefile);
	fclose(preferencefile);*/

	bml=bml+"</bml>\n";

	if(realtime==1)
		return bml;

	FILE *f=fopen(filenames.FMLAPML_to_BML_File.c_str(),"w");
	//FILE *f=fopen((inimanager.Program_Path+"bml/from-fml-apml.xml").c_str(),"w");
	if(f!=0)
	{
		fprintf(f,"%s",bml.c_str());
		fclose(f);
	}

	//if(bae.Execute(inimanager.Program_Path+"bml/from-fml-apml.xml",0,1)==0)
	if(bae.Execute(filenames.FMLAPML_to_BML_File,0,1)==0)
	{
		printf("FMLAPML Engine:: Error running the BML engine\n");
		return "";
	}

	return bml;
}

