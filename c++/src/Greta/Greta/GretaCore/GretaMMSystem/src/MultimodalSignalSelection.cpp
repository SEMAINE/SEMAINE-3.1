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

#include <math.h>
#include ".\multimodalsignalselection.h"
#include "XMLGenericParser.h"
#include "Implication.h"
#include "RandomGen.h"




extern RandomGen *randomgen;

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/
MultimodalSignalSelection::MultimodalSignalSelection()
{
	logfile=fopen("logs/multimodalsignsel_log.html","w");
	fprintf(logfile,"<html>\n<head>\n</head>\n<body>\n");
	max_num_signals=12;
}
MultimodalSignalSelection::MultimodalSignalSelection(std::string filepath)
{
	logfile=fopen("logs/multimodalsignsel_log.html","w");
	fprintf(logfile,"<html>\n<head>\n</head>\n<body>\n");
	max_num_signals=12;
	this->LoadSelectionLexicon(filepath,"");
}

MultimodalSignalSelection::~MultimodalSignalSelection(void)
{
	fprintf(logfile,"</body>\n</html>\n");
	fclose(logfile);
	BehaviorSets.clear();
}

int MultimodalSignalSelection::LoadSelectionLexicon(std::string filepath,char *buffer)
{
	XMLGenericParser p;
	XMLGenericTree *t;
	p.SetValidating(true);
	if(filepath!="")
	{
		t=p.ParseFileWithXSD(filepath,"mmsystem/xsd/behaviorsets.xsd");
	}
	else
	{
		t=p.ParseBufferWithXSD(buffer,"mmsystem/xsd/behaviorsets.xsd");
	}
	if(t==0)
	{
		printf("Error parsing file %s\n",filepath.c_str());
		fprintf(logfile,"Error parsing file %s\n",filepath.c_str());
		return 0;
	}
	std::list<XMLGenericTree*>::iterator sel;
	
	for(sel=(t->child).begin();sel!=(t->child).end();sel++)
	{
		BehaviorSet s;
		
		s.name=(*sel)->GetAttribute("name");

		//looks for the base signals associated to the BehaviorSet
		XMLGenericTree* basesignalsnode=(*sel)->FindNodeCalled("signals");

		std::list<XMLGenericTree*>::iterator sig;
		for(sig=basesignalsnode->child.begin();
			sig!=basesignalsnode->child.end();sig++)
		{
				Signal signal;
				signal.reference=(*sig)->GetAttribute("name");
				signal.modality=(*sig)->GetAttribute("modality");
				signal.id=(*sig)->GetAttribute("id");
				//just fills the vector of the base signals
				
				if((*sig)->child.empty()==false)
				{
					std::list<XMLGenericTree*>::iterator altshapes;
					altshapes=(*sig)->child.begin();
					//signal.reference=(*altshapes)->GetAttribute("name");
					//altshapes++;
					for(;altshapes!=(*sig)->child.end();altshapes++)
					{
						alternativeshape *as;
						as=new alternativeshape();
						as->name=(*altshapes)->GetAttribute("name");
						as->probability=(*altshapes)->GetAttributef("probability");
						signal.alternativeshapes.push_back(*as);
					}
				}

				s.basesimplesignals.push_back(signal);
		}
		
		//then checks if the number of the base signals exceeds the max threshold
		int size=(int)s.basesimplesignals.size();

		if(size>max_num_signals)
		{
			printf("MultimodalSignalSelection: the element %s has too many base signals (%d)\n",
				s.name.c_str(),size);
			fprintf(logfile,"<br/>MultimodalSignalSelection: the element %s has too many base signals (%d)<br/>\n",
				s.name.c_str(),size);
			delete t;
			return 0;
		}

		//creates a matrix with all the possibile combinations of the signals
		//and stores them in the list of multimodal signals
		for(int i=0;i<(size*size);i++)
		{
			MultimodalSignal v;
			s.multimodalsignals.push_back(v);
			for(int j=0;j<size;j++)
			{
				if(((i/(int)pow((float)2,(j)))%2)==0)
					s.multimodalsignals[i].push_back(s.basesimplesignals[j]);
			}
		}

		//then looks for the signals which are coresignals
		XMLGenericTree* coresignalsnode=(*sel)->FindNodeCalled("core");
		
		if(coresignalsnode!=0)
		{
			SignalVector coresignals;

			std::list<XMLGenericTree*>::iterator coreiter;
			for(coreiter=(coresignalsnode->child).begin();
				coreiter!=(coresignalsnode->child).end();coreiter++)
			{
					Signal signal;
					signal.id=(*coreiter)->GetAttribute("id");
					//puts them into a vector
					coresignals.push_back(signal);
			}

			//eliminates the multi modal signals that do not contain the base signals
			//which are coresignals

			s.ApplyCore(coresignals);

			coresignals.clear();
		}

		//stores the rules

		XMLGenericTree* rulesnode=(*sel)->FindNodeCalled("rules");
		
		if(rulesnode!=0)
		{
			ImplicationList implications;

			std::list<XMLGenericTree*>::iterator impl;
			for(impl=(rulesnode->child).begin();
				impl!=(rulesnode->child).end();impl++)
			{
				Implication implication;

				std::list<XMLGenericTree*>::iterator implitem;

				Signal ifpresent;
				
				ifpresent.id=(*impl)->FindNodeCalled("ifpresent")->GetAttribute("id");

				implication.ifpresent.push_back(ifpresent);

				for(implitem=(*impl)->child.begin();implitem!=(*impl)->child.end();implitem++)
				{
					if((*implitem)->name=="thenpresent")
					{
						Signal thenpresent;
						thenpresent.id=(*implitem)->GetAttribute("id");
						implication.thenpresent.push_back(thenpresent);
					}
					if((*implitem)->name=="thennotpresent")
					{
						Signal thennotpresent;
						thennotpresent.id=(*implitem)->GetAttribute("id");
						implication.thennotpresent.push_back(thennotpresent);
					}
				}

				implications.push_back(implication);
			}

			//eliminates the multi modal signals which do not respect the rules

			s.ApplyImplications(implications);

			implications.clear();
		}
		s.PrintMMSignals(logfile);

		this->BehaviorSets[s.name]=s;
	}

	delete t;

	return 1;

}

MultimodalSignal *MultimodalSignalSelection::SelectMultimodalSignal(std::vector<MultimodalSignal*> *possiblesignals,MMSystemSpace::CommunicativeIntention ca,Dynamicline *dl)
{
	//input: possible multimodal signals corresponding to the given communicative act
	//output: one multimodal signal

	std::vector<MultimodalSignal*>::iterator mmsptr;
	MultimodalSignal *mms_selected;
	int min,max;

	if(possiblesignals==0)
		return 0;
	if(possiblesignals->size()==0)
		return 0;

	min=99;
	max=0;


	//looks for the max and min multiplicity of qll the possible mm signals
	for(mmsptr=possiblesignals->begin();mmsptr!=possiblesignals->end();mmsptr++)
	{
		if((*mmsptr)->size()<min)
		{
			min=(*mmsptr)->size();
		}
		if((*mmsptr)->size()>max)
		{
			max=(*mmsptr)->size();
		}
	}
	
	int selectedmultiplicity;

	//compute the multiplicity of the final signal, using the communicative act importance

	if(min==max)
		selectedmultiplicity=min;
	else
		selectedmultiplicity=min+(max-min)*ca.importance;

	if(ca.importance==0)
		selectedmultiplicity=min;
	
	int counter;

	counter=0;

	int lookingformultiplicity;

	lookingformultiplicity=selectedmultiplicity;

	//now apply preference:

	//counts how many signals have the desired multiplicity
	while(counter==0)
	{
		for(mmsptr=possiblesignals->begin();mmsptr!=possiblesignals->end();mmsptr++)
		{
			if((*mmsptr)->size()==lookingformultiplicity)
				counter++;
		}
		//if there is no signal with that multiplicity, try another one
		if(counter==0)
		{
			if((max-selectedmultiplicity)<(selectedmultiplicity-min))
				lookingformultiplicity++;
			else
				lookingformultiplicity--;
		}
		//printf("*");
	}
	//printf("\n");

	std::vector<MultimodalSignal *>mmscandidates;
	float maxpref;
	float mmspref;
	maxpref=0;
	counter=0;
	//choose the signal with the desired multiplicity and highest preference
	for(mmsptr=possiblesignals->begin();mmsptr!=possiblesignals->end();mmsptr++)
	{
		mmspref=(*mmsptr)->ComputePreference(dl);
		if(((*mmsptr)->size()==lookingformultiplicity)&&((mmspref-maxpref)>0.1))
		{
			counter=1;
			mmscandidates.clear();
			maxpref=mmspref;
			mmscandidates.push_back((*mmsptr));
			continue;
		}
		if(((*mmsptr)->size()==lookingformultiplicity)&&(abs(mmspref-maxpref)<0.1))
		{
			counter++;
			mmscandidates.push_back((*mmsptr));
		}
	}

	int which;
	//if there is more than one signal with that multiplicity and highest preference, choose one randomly
	which=randomgen->GetRand01()*((float)counter);

	mms_selected=mmscandidates[which];
	
	return mms_selected;

}

std::vector<MultimodalSignal*> *MultimodalSignalSelection::SelectMultimodalSignals(MMSystemSpace::CommunicativeIntention ca,
								Dynamicline *dl,
								std::list<std::string> &BusyModalities)
{

	BehaviorSet *s;
	s=FromCAtoBehaviorSet(ca);
	
	if(s==0)
		return 0;

	std::vector<MultimodalSignal*> *result;
	std::vector<MultimodalSignal>::iterator mms;
	std::list<std::string>::iterator bm;

	result=new std::vector<MultimodalSignal*>;
		
	//finds multi modal signals that do not use busy modalities
	
	
	for(mms=s->multimodalsignals.begin();mms!=s->multimodalsignals.end();mms++) //for each multimodalsignal
	{
		bool found;
		found=false;
		if((*mms).IsActivable(dl,ca.importance)==true)
		{
			//check if a given multimodalsignal uses the modality from the list of busymodalities
			for(bm=BusyModalities.begin();bm!=BusyModalities.end();bm++)
			{
				if((*mms).UseModality(*bm))
				{
					found=true;
					break;
				}
			}
			if(found==false)
				result->push_back(&(*mms));
		}
	}

	return result;
}

BehaviorSet *MultimodalSignalSelection::FromCAtoBehaviorSet(MMSystemSpace::CommunicativeIntention ca)
{
	std::string lookfor;

	lookfor=ca.name+"-"+ca.type;

	if(ca.target!="")
		lookfor+="-"+ca.target;

	if(BehaviorSets.find(lookfor)==BehaviorSets.end())
	{
		lookfor=ca.name+"-"+ca.ref_type+"-"+ca.ref_id+"-"+ca.prop_type+"-"+ca.prop_value;
		if(BehaviorSets.find(lookfor)==BehaviorSets.end())
		{
			lookfor=ca.name+"-"+ca.ref_type+"-"+ca.ref_id;
			if(BehaviorSets.find(lookfor)==BehaviorSets.end())
			{
				lookfor=ca.name+"-*";
				if(BehaviorSets.find(lookfor)==BehaviorSets.end())
				{
					printf("MultimodalSignalSelection: not found any behavior set for %s\n",(ca.name+"-"+ca.type).c_str());
					return 0;
				}
			}
		}
	}

	//printf("MultimodalSignalSelection: found the behavior set of %s\n",lookfor.c_str());
	
	return &(BehaviorSets[lookfor]);
}