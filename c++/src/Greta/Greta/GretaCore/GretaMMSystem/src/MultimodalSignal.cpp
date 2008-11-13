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
#include "MultimodalSignal.h"
#include "RandomGen.h"
#include "NewExpressionContainer.h"
#include "ConstraintsContainer.h"

#include "DataContainer.h"
#include "MultimodalEmotionConstraint.h"

extern RandomGen *randomgen;
extern DataContainer *datacontainer;

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

MultimodalSignal::MultimodalSignal(void)
{
	preference=0;
}

MultimodalSignal::MultimodalSignal(const MultimodalSignal &rhs)
{
	std::vector<Signal>::const_iterator iter;
	for(iter=rhs.begin();iter!=rhs.end();iter++)
	{
		Signal s(*iter);
		this->push_back(s);
	}
	preference=rhs.preference;
}


MultimodalSignal::~MultimodalSignal(void)
{
}

bool MultimodalSignal::UseModality(std::string modalityname)
{
	bool found;
	std::vector<Signal>::iterator iter;
	found=false;
	for(iter=this->begin();iter!=this->end();iter++)
		if((*iter).modality==modalityname)
		{
			found=true;
			break;
		}
		return found;
}

float MultimodalSignal::ComputePreference(Dynamicline *dl)
{
	float p;
	std::vector<Signal>::iterator iter;
	p=0.0f;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		//if(dl->GetAttrValue((*iter).modality+".preference.value")>p)
		//	p=dl->GetAttrValue((*iter).modality+".preference.value");
		p=p+dl->GetAttrValue((*iter).modality+".preference.value");
	}
	this->preference=p;
	return p;
}

std::list<std::string> MultimodalSignal::UsedModalities()
{
	std::list<std::string> usedmodalities;
	std::vector<Signal>::iterator iter;
	usedmodalities.clear();
	for(iter=this->begin();iter!=this->end();iter++)
	{
		if((*iter).concretized)
			usedmodalities.push_back((*iter).modality);
	}
	return usedmodalities;
}

std::string MultimodalSignal::GetBML(int *uid)
{
	std::string bml;

	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		if((*iter).concretized)
			bml=bml+(*iter).GetBML(uid);
	}

	return bml;
}

void MultimodalSignal::Temporize(float start,float duration)
{
	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		(*iter).start=start;
		(*iter).strokes.clear();
		(*iter).duration=duration;
		(*iter).concretized=true;
	}
}

void MultimodalSignal::Temporize(float start,float stroke,float duration)
{
	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		(*iter).start=start;
		(*iter).duration=duration;
		(*iter).strokes.clear();
		(*iter).strokes.push_back(stroke);
		(*iter).concretized=true;
	}
}

void MultimodalSignal::Temporize(float start,std::vector<float> strokes,float duration)
{
	int strokesnum;
	strokesnum=strokes.size();

	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		(*iter).strokes.clear();
		(*iter).start=start;
		(*iter).duration=duration;
		if(strokes.empty())
		{
			(*iter).concretized=true;
			continue;
		}
		if((*iter).parameters.find("REP.value")==(*iter).parameters.end())
		{
			(*iter).strokes=strokes;
			(*iter).concretized=true;
			continue;
		}
		float rep=(*iter).parameters["REP.value"].GetValue();
		rep=(rep+1.0f)/2.0f;
		if(rep==0)
		{
			//(*iter).concretized=false;
			(*iter).strokes.clear();
			continue;
		}

		if((rep<=1.0)&&(rep>=0.86))
		{
			(*iter).strokes=strokes;
			(*iter).concretized=true;
			continue;
		}

		float strokeshighestnum=strokes.size();

		strokeshighestnum=rep*strokeshighestnum;

		if(strokeshighestnum<1.0f)
		{
			(*iter).strokes.push_back(strokes[strokes.size()/2]);
			(*iter).concretized=true;
			continue;
		}

		float strokefrequency;
		strokefrequency=duration/strokeshighestnum;
		float time;
		time=0;
		std::vector<float>::iterator stroke;
		for(stroke=strokes.begin();stroke!=strokes.end();stroke++)
		{
			if((*stroke)>time)
			{
				(*iter).strokes.push_back(float(*stroke));
				time+=strokefrequency;
			}
		}
		(*iter).concretized=true;
	}
}

void MultimodalSignal::AssignNameToSignals(std::string name)
{
	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		(*iter).id=name;
	}
}

void MultimodalSignal::AssignParameters(Baseline *bl)
{
	std::vector<Signal>::iterator iter;
	//printf("dynamicline:\n");
	//bl->Print();

	for(iter=this->begin();iter!=this->end();iter++)
	{
		std::map<std::string,EngineParameterSet>::iterator iter2;
		std::string modname;
		for(iter2=bl->begin();iter2!=bl->end();iter2++)
		{
			modname=(*iter2).second.name.substr(0,(*iter2).second.name.find_first_of("."));
			if(modname==(*iter).modality)
			{
				EngineParameters::iterator iter3;
				for(iter3=(*iter2).second.mp.begin();iter3!=(*iter2).second.mp.end();iter3++)
				{
					EngineParameter *mp;
					mp=new EngineParameter();
					mp->name=(*iter3).second.name;
					mp->SetValue((*iter3).second.GetValue());
					(*iter).parameters[mp->name]=(*mp);
				}
			}
		}
	}
}

bool MultimodalSignal::IsActivable(Dynamicline *dl,float importance)
{
	float oac;
	float *oacp;
	std::vector<Signal>::iterator iter;

	if(this->empty())
		return false;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		oacp=(*dl)[(*iter).modality].GetAttrAddress("OAC.value");
		if(oacp==0)
			return false;

		oac=*oacp;

		oac=oac+((randomgen->GetRand01()*0.3f)-0.15f);

		if(oac<=0)
			return false;

		if((1-importance)>(oac))
			return false;
	}
	return true;
}


void MultimodalSignal::SaveCSV(float time,FILE *preferencefile,FILE *gesturefile,FILE *torsofile,FILE *headfile,FILE *facefile,FILE *gazefile)
{
	FILE *f;
	float gestp,torsp,facep,headp,gazep;
	char store[255];
	int i;

	gestp=-999;
	torsp=-999;
	facep=-999;
	headp=-999;
	gazep=-999;

	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		if((*iter).modality=="gesture")
		{
			f=gesturefile;
			gestp=(*iter).parameters["preference.value"].GetValue();
		}
		if((*iter).modality=="torso")
		{
			f=torsofile;
			torsp=(*iter).parameters["preference.value"].GetValue();
		}
		if((*iter).modality=="head")
		{
			f=headfile;
			headp=(*iter).parameters["preference.value"].GetValue();
		}
		if((*iter).modality=="face")
		{
			f=facefile;
			facep=(*iter).parameters["preference.value"].GetValue();
		}
		if((*iter).modality=="gaze")
		{
			f=gazefile;
			gazep=(*iter).parameters["preference.value"].GetValue();
		}

		sprintf(store,"%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
			(*iter).start,
			(*iter).parameters["preference.value"].GetValue(),
			(*iter).parameters["OAC.value"].GetValue(),
			(*iter).parameters["SPC.value"].GetValue(),
			(*iter).parameters["TMP.value"].GetValue(),
			(*iter).parameters["FLD.value"].GetValue(),
			(*iter).parameters["PWR.value"].GetValue(),
			(*iter).parameters["REP.value"].GetValue());

		fprintf(f,"%s",store);
		sprintf(store,"%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
			(*iter).start+(*iter).duration,
			(*iter).parameters["preference.value"].GetValue(),
			(*iter).parameters["OAC.value"].GetValue(),
			(*iter).parameters["SPC.value"].GetValue(),
			(*iter).parameters["TMP.value"].GetValue(),
			(*iter).parameters["FLD.value"].GetValue(),
			(*iter).parameters["PWR.value"].GetValue(),
			(*iter).parameters["REP.value"].GetValue());

		fprintf(f,"%s",store);
	}

	sprintf(store,"%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
		(*iter).start,gestp,torsp,headp,facep,gazep);
	for(i=0;i<strlen(store);i++)
	{
		if(store[i]=='.')
		{
			store[i]=',';
		}
	}
	fprintf(preferencefile,"%s",store);
	sprintf(store,"%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n",
		(*iter).start+(*iter).duration,gestp,torsp,headp,facep,gazep);
	for(i=0;i<strlen(store);i++)
	{
		if(store[i]=='.')
		{
			store[i]=',';
		}
	}
	fprintf(preferencefile,"%s",store);
}

void MultimodalSignal::ShuffleShapes()
{
	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		(*iter).ShuffleShape();
	}
}

void MultimodalSignal::AssignStrokesFromCommAct(MMSystemSpace::CommunicativeIntention *ca)
{
	MMSystemSpace::CommunicativeIntention *contextaccent;

	contextaccent=ca->GetPitchAccentFromContext();

	if(contextaccent!=0)
	{
		float stroke;
		stroke=contextaccent->start-ca->start+(contextaccent->duration*0.40);
		this->Temporize(ca->start,stroke,ca->duration);
	}
	else
	{
		this->Temporize(ca->start,ca->duration);
	}
}

void MultimodalSignal::AssignStrokesFromStressVector(MMSystemSpace::CommunicativeIntention *ca,std::vector<float> *stressvector)
{
	std::vector<float> strokes;
	std::vector<float>::iterator iter;
	float stresstime;
	float prevstresstime=ca->start+0.3;
	for(iter=stressvector->begin();iter!=stressvector->end();iter++)
	{
		stresstime=(*iter)-0.2;
		if(stresstime<ca->start)
			continue;
		if(stresstime>(ca->start+ca->duration))
			continue;

		if((stresstime-prevstresstime)>0.2)
		{
			strokes.push_back(stresstime-ca->start);
			prevstresstime=stresstime;
		}
	}
	if(strokes.empty()==false)
		this->Temporize(ca->start,strokes,ca->duration);
	else
		this->Temporize(ca->start,ca->duration);
}


void MultimodalSignal::AddIntensity(float intensity1)
{
	std::vector<Signal>::iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		(*iter).SetIntensity(intensity1);
	}

}

//
//THIS PART OF CODE IS STILL UNDER DEVELOPPEMENT!!!
//IT IS NOT USED BY GRETA
//
void MultimodalSignal::addTimeConstraints(std::string emotion )
{

	/*
the signals have only the reference defined , it needs to be parsered
	
	if((*iter).reference!="")
	{
		classname=(*iter).reference.substr(0,(*iter).reference.find_first_of("="));
		instancename=(*iter).reference.substr((*iter).reference.find_first_of("=")+1);
	}
	
	*/
	
	//clone this
	MultimodalSignal clone = this->clone();
	
	//cancel the list of signals of this
	this->clear();
	
	NewExpressionContainer *newexpressions = datacontainer->getNewExpressionContainer();
	ConstraintsContainer *constraintscontainer = datacontainer->getContraints();

	MultimodalEmotionConstraint *mec = constraintscontainer->getMultimodalEmotionConstraint(emotion);
	//contr

	//each signal should have time duration at the probability of the occurence, and of repetition
	//if the signal is not present in exe it should have additional constraints defined in te contraint file

	std::vector<Signal>::iterator iter;

	//decide the nunber of n-units on the base of baseline and time of commact

	float start = 0;
	float stop = 0;

	//what about the timemarkers????
	//find time constraints	
	for(iter=clone.begin();iter!=clone.end();iter++)
	{
		if (start==0) start =(*iter).start;
		else if (start> (*iter).start ) start =(*iter).start;
		if (stop < (*iter).start + (*iter).duration ) stop =(*iter).start + (*iter).duration;		
	}

	float time = stop - start;

	//at the moment we use n = 1 sec
	int n = floor(time);

	//turn number
	int turn = 0;

	//time of one turn - at the moment it is about 1 sec
	float time_unit= time/n;


	// cancel the signals that are shorter than one unit and create probability list

	//to do

	//create vectors of probabilities
	//+1 becaese of index from 1 to clone.size
	
	//if empty expression is not added
	int probabilities_size = clone.size()+1;

	//std::map<std::string,probabilities>::iterator iter2;

	std::map<std::string,float> probabilities_start;
	std::map<std::string, float> probabilities_end;
	std::map<std::string, float> current_probabilities;

	//vector<int> probabilities_start(probabilities_size);
	//vector<int> probabilities_end (probabilities_size);
	//vector<int> current_probabilities (probabilities_size);

	//probabilities [0][y] - is not used
	//float probabilities [probabilities_size][1];	

	/*
	for (int i=0; i < probabilities_size ;i++)
	{
		probabilities_start[i]=0.0f;
		probabilities_end[i]=0.0f;		
		current_probabilities[i]=0.0f;		
	}
	*/

	int index1 = 1;

	std::vector<Signal>::iterator iterxxx;

	for(iterxxx=clone.begin();iterxxx!=clone.end();iterxxx++)
	{
		std::string temp = iterxxx->reference;		

		float max_duration=constraintscontainer->getMax_duration(emotion,temp);

		//check it
		if (max_duration<time_unit) 
		{
			//check the index
			clone.erase(iterxxx);

		}
		else 
		{

			//update the start and ending probabilities

			std:string temp_name= findName(temp);			
		
			probabilities_start[temp_name] = constraintscontainer->getProbability_start(emotion,temp);

			probabilities_end[temp_name] = constraintscontainer->getProbability_end(emotion,temp);

			current_probabilities[temp_name] =  constraintscontainer->getProbability_start(emotion,temp);

			//attention: some probabilities [x][y] are 0 as we cannot not use corresponding signal x
		} //end of else

	} //end of for

	/*
	//add the empty signal
	Signal empty;
	empty.modality="face";
	empty.type="neutral";
	empty.intensity=0;	
	empty.reference="affect=neutral";
	clone.push_back(empty);

	//choose the probability for the empty signal according to the expressivity parameters
	probabilities_start["neutral"]=0.5f;
	probabilities_end["neutral"]=0.5f;		
	current_probabilities["neutral"]=0.5f;		

	ConstraintSignal emptycs;
	int neutral_id= ( *(mec->getConstraintSignals()) ).size() + 1;
	printf("neutral_id %i \n", neutral_id);
	emptycs.setId(((*mec->getConstraintSignals())).size()+1); 
	emptycs.setMax_duration(2.0); //should be modified then
	emptycs.setMin_duration(1.0); //should be modified then
	emptycs.setRepetivity(1);
	emptycs.setName("affect=neutral");

	Constraint *emptycon = new Constraint();
	emptycon->setArg1(neutral_id,1);
	emptycon->setArg2(neutral_id,2);
	emptycon->setConstraintType("minus");
	emptycon->setValue(0);
	emptycon->setLessThan();

	mec->addConstraintSignal(emptycs);
	mec->addConstraint(emptycon);
	*/

	//clone is a full list of signal at disposal
	//temporal is like clone but with some signals cancelled (avoid infinite loop)

	//generate a  temporal list of signals
	MultimodalSignal temporal=clone.clone();

	//start of the algortithm
	int singals_in_turn=0;
	
	Signal startsignal;
	startsignal.reference="affect=neutral";
	//startsignal.value="neutral";
	startsignal.type="face";
	//startsignal.function="affect";

	//add something at the begining
	startsignal.start=0;
	startsignal.duration = 1;
	startsignal.id="neutral";
	//startsignal.concretized=true;	
	//not needed
	startsignal.strokes.push_back( 0.1 );								
				
	this->push_back(startsignal);

	//turn++; 
	
	while (turn<n) 
	{

		bool success=false;

		//A. choose the signal (consider the probability of occurence)

		//choose from temporal

		//if (!(temporal.empty())) it is always true because of empty signals with no constraints
		//thus at list empty can be chosen

		float result=randomgen->GetRand01();

		MultimodalSignal short_list;

			for(iter=temporal.begin();iter!=temporal.end();iter++)	
		{ //for all signals in temporal

			//printf("temP %s \n", (*iter).reference.c_str() );
			//printf("prob %f '\n", current_probabilities[findName(iter->reference)] );

			if (current_probabilities[findName(iter->reference)] > result ) 
			{
				///printf(" added %s \n", (*iter) .reference.c_str());
				short_list.push_back( *((*iter).clone() ) );
			}//end of if
		}//end of for

		if (short_list.size()==0) 
		{
			turn++; 
		}
		else 
		{

			result=randomgen->GetRand01();

			Signal *chosen;

			for (int i=1; i<=short_list.size(); i++) 
			{

				if  ( ( ( ( (float) 1/short_list.size() ) * (float) (i-1) ) < result) && (result < ( ( (float) 1/short_list.size() ) * (float) i) ) ) 
				{									
					chosen = new Signal ( (Signal) short_list[i-1]); 
					i=short_list.size()+1; //exit from for
				}
			} //end of for
	
			float result=randomgen->GetRand01();
			
			float begin_time = (turn +result) * time_unit;

			//check the contraints with "this" (does the signal can be the first one or it has to precede the other signal)
			if (checkStartCons(chosen->reference, begin_time,  mec )==true) 
			{
				//yes			
				//choose the duration xn - generate start and end for this signal - check contraints again
				
				float max_duration =  (mec->getConstraintSignal(chosen->reference))->getMax_duration();
				float min_duration =  (mec->getConstraintSignal(chosen->reference))->getMin_duration();

				float distance = max_duration - min_duration;

				float result=randomgen->GetRand01();
				result = sqrt(result);

				float x1 =  min_duration + (result*distance) /2.0f;
				float x2 =  max_duration - (result*distance) /2.0f;

				//printf("min %f, max %f, x1, %f, x2 %f \n", min_duration, max_duration, begin_time + x1, begin_time + x2);

				float end_time= 0.0f;
				result=randomgen->GetRand01();
				if (result>0.5f) end_time=x1; else end_time=x2;

				//for (int i = (int) max_duration; i > min_duration; i-= time_unit) 
				//{ 
				//end_time=i;

				if ( ( checkStopCons (chosen->reference, begin_time, end_time, mec ) == true) && ( begin_time + end_time <= time ) )
				{

					//set start and stop
					chosen->start=begin_time;
					//chosen.start_sym
					chosen->duration = end_time;
					//chosen.duration_sym
					//do I need to fill some other properties??
					chosen->concretized=true;
					
					//find strokes
					//printf("before size %i \n", chosen->strokes.size());					
					//printf("signal %s \n", chosen->reference.c_str());					

					if(chosen->strokes.empty()==false)
						{
							std::vector<float>::iterator iter;
							for(iter=chosen->strokes.begin();iter!=chosen->strokes.end();)
							{
								//if stroke is included
								float f=(*iter);
								//printf("stokes is :%f ", f);
								if ( (f<=begin_time) || (f >= end_time) )
								{
								//printf(".. cancelled \n");
								chosen->strokes.erase(iter);

								}
								else 
								{
									//DEBUG IT
									(*iter)=(*iter)-begin_time;
									iter++;
									//printf("\n");
								}
							}
						}
						
						//if we cancelled all strokes
						if ((chosen->strokes.empty()==true) || (chosen->strokes.size()==0)) 
						{
							chosen->strokes.resize(0);
							float stroke_time= 0.1*end_time;
							chosen->strokes.push_back(stroke_time);								
						}
					
						//printf("after size %i \n", chosen->strokes.size());					
						this->push_back(*chosen);
						singals_in_turn++;

					//update probabilities
					//update the probabilities (consider the possibility of repetition, (???of the continuation!!!!,) change the probabilities of occurence)												

					float factor =  (float) ( (float)  (turn+1)   / (float) n ) ;
				
					if ( (factor < 1.0f) && (factor >= 0.0f) )
					{	
						for(iter=clone.begin();iter!=clone.end();iter++)	//for all signals in clone 
						{	
							std::string temp_name = findName(iter->reference);
							
							float current_repetivity = constraintscontainer->getRepetivity(emotion, iter->reference ) ; 			

							//if it is an emement that can not be repeated - I put 0 as a probability
							if  (iter->reference.compare(chosen->reference) == 0 )
								if (current_repetivity==0.0f)  
								{							
									
									probabilities_start[temp_name]=0.0f;
									probabilities_end[temp_name]=0.0f;
								}

								//three cases:
								//start=end
								if (probabilities_start[temp_name]==probabilities_end[temp_name])
									current_probabilities[temp_name] = probabilities_start[temp_name] ;
								//start < end
								if (probabilities_start[temp_name] < probabilities_end[temp_name]) 
								{						
									float distance = probabilities_end[temp_name]-probabilities_start[temp_name] ;
									current_probabilities[temp_name] = probabilities_start[temp_name] + distance * factor ;
								}
								//start > end										
								if (probabilities_start[temp_name] > probabilities_end[temp_name])
								{						
									float distance = probabilities_start[temp_name] - probabilities_end[temp_name];
									current_probabilities[temp_name] = probabilities_start[temp_name] - distance * factor ;
								}

								if (current_probabilities[temp_name]>1.0f) current_probabilities[temp_name]=1;
								if (current_probabilities[temp_name]<0.0f) current_probabilities[temp_name]=0;
								

						}//end of for

					}//end of if factor

					//after adding a signal and updating the prob
					//do we want to add a new signal in this turn 

					//printf(" OAC %f ",  chosen->parameters["OAC.value"].GetValue() );
					//printf(" SPC %f ",  chosen->parameters["SPC.value"].GetValue() );
					//printf(" TMP %f ",  chosen->parameters["TMP.value"].GetValue() );
					//printf(" FLD %f ",  chosen->parameters["FLD.value"].GetValue() );
					//printf(" PWR %f ", chosen->parameters["PWR.value"].GetValue() );
					//printf(" REP %f \n",  chosen->parameters["REP.value"].GetValue() );

					//which parameters should be used
					float overall = (*chosen).parameters["OAC.value"].GetValue();
					
					//5 - becouse max 5 signals at the moment possible - true ?
					//supposing that the OAC is from the interval -1,1

					if (((float)(overall+1)*(float)(5 - singals_in_turn))>4.0f) 
					{
						//debug
						//do not use it again in this turn						
						std::string temp_name = findName(chosen->reference);
						current_probabilities[temp_name] = 0.0f ;

						//i=min_duration-1;//exit from for 
						success=true;
						//continue; // I DO NOT EXIT to while as I wish to ...
					}
					else 
					{
						//all is possible again??
						temporal=clone.clone();
						turn++; //or more if the expressivity is low
						//i=min_duration-1;//exit from for
						success=true;
						//continue; // I DO NOT EXIT to while as I wish to ...
					}//end of else

				}//end of if stop

				else 
				{ //if stop condition is false

				}
				//}//end of for 

				//if we are here it means that chosen is not a good signal becouse it cannot be finshed in time
				//so cancel a from the proposal list and signal again

				/*
				if (success==false)  {

				std::vector<Signal>::iterator iter;
				for(iter=temporal.begin();iter!=temporal.end();iter++)	{ //for all signals in temporal
				if ( strcmp( (*iter).reference.c_str() , chosen->reference.c_str() ) == 0 ) temporal.erase(iter);		
				}

				//continue;
				}//end if success = false

				*/

			}  // end of start
			else 
			{
				//if we are here it means that chosen is not a good signal becouse it cannot be started in time
				//so cancel a from the proposal list and choose a signal again					

				//do not use it again in this turn						
				std::string temp_name = findName(chosen->reference);
				current_probabilities[temp_name] = 0.0f ;				

			}//end of else

		}//end of else if short list is empty

	}// and of while

	//add the empty singal at the end if it is necessary	

	if (turn*time_unit<time)
	{
		//create empty signal
		Signal empty;
		empty.id="neutral";
		empty.start=turn*time_unit;
		empty.duration=time - turn*time_unit;
		empty.reference="affect=neutral";
		empty.concretized=true;
		empty.strokes.push_back( 0.1 *empty.duration );			
		this->push_back(empty);

		
	}//end of if


/*
	printf("\n\n Results: \n\n");

	for(iter=this->begin();iter!=this->end();iter++)	
	{			
		printf("Name %s \n", 	iter->reference.c_str() );
		printf("start %f \n", 	iter->start );
		printf("duration %f \n", 	iter->duration );
	    printf("stroke %f \n", 	iter->strokes.at(0) );
	}

*/

}//end of addTimecontstraints




std::string MultimodalSignal::findName(std::string reference) 
{
	std::string new_name;
	if( reference!="" )
	{							
		if ( reference.find_first_of("=")!=0 ) 
			new_name = reference.substr (reference.find_first_of("=")+1, reference.length()- reference.find_first_of("=") - 1);
		else new_name=reference;
	}
	
	return new_name;
}


//
//THIS PART OF CODE IS STILL UNDER DEVELOPPEMENT!!!
//IT IS NOT USED BY GRETA
//

//time in seconds (and not in time-units)	
bool MultimodalSignal::checkStartCons(std::string reference, float begin_time, MultimodalEmotionConstraint *mec)
{

	std::vector<Constraint>* good_cons = mec->getConstraintsOfOneSignal(reference);
	
	//good_cons = mec->getConstraints();	
	
	int id = mec->getIdOfSignal (reference);

	
	//for all relevant cons
	std::vector<Constraint>::iterator cons_iter;
	for(cons_iter=good_cons->begin();cons_iter!=good_cons->end();cons_iter++) {

		//and all signals chosen for the moment
		std::vector<Signal>::iterator signals_iter;
		for(signals_iter=this->begin();signals_iter!=this->end();signals_iter++) {

			
			int signal_id = mec->getIdOfSignal( (*signals_iter).reference );

			
			if (
				( (signal_id==cons_iter->getArg1Id()) &&(id==cons_iter->getArg2Id()) ) 
				|| 
				( (signal_id==cons_iter->getArg2Id()) &&(id==cons_iter->getArg1Id()) ) 
				)
			{

				//suppose that the first argument = a candicate

				/*
				a cons is of type 1 

				<con type="minus">
				<arg id="this" type="start"/> 
				<arg id="other" type="start"/> 
				<lessthan value="0"/> 
				</con>  

				start(this)-start(other) < x
				start(this) < x + start(other)

				*/

				/*
				if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isLessThan()==true) &&
				(cons_iter->getArg1Type()==1) &&
				(cons_iter->getArg1Id()==id)&&
				(cons_iter->getArg2Type()==1)&&
				(cons_iter->getArg2Id()==signal_id)
				) 
				{

				if ( !( begin_time <  (*signals_iter).start) ) return false;

				}// end of condition 1

				*/

				/*
				a cons is of type 2 

				<con type="minus">
				<arg id="this" type="start"/> 
				<arg id="other" type="start"/> 
				<equal value="0"/> 
				</con>  

				start(this)-start(other) = x
				start(this) =  start(other)

				*/

				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isEqual()==true) &&
					(cons_iter->getArg1Type()==1) &&
					(cons_iter->getArg1Id()==id)&&
					(cons_iter->getArg2Type()==1)&&
					(cons_iter->getArg2Id()==signal_id)
					) 
				{
					//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start ); 

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) ) 
					{
						if (
							!(begin_time == (*signals_iter).start ) 
							) return false;
					}
				}// end of condition 2
				/*
				a cons is of type 3 

				<con type="minus">
				<arg id="this" type="start"/> 
				<arg id="other" type="start"/> 
				<morethan value="0"/> 
				</con>  

				start(this)-start(other) > x
				start(this) > x + start(other)
				*/

				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isMoreThan()==true) &&
					(cons_iter->getArg1Type()==1) &&
					(cons_iter->getArg1Id()==id)&&
					(cons_iter->getArg2Type()==1)&&
					(cons_iter->getArg2Id()==signal_id)
					) 
				{
					//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start ); 

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
						if ( 
							( 
							!( begin_time >  (*signals_iter).start ) 
							) 
							)
							return false;

				}// end of condition 3

				/*
				a cons is of type 4

				<con type="minus">
				<arg id="this" type="start"/> 
				<arg id="other" type="stop"/> 
				<lessthan value="0"/> 
				</con>  

				start(this)-stop(other) < x
				start(this) < x + stop(other)
				*/

				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isLessThan()==true) &&
					(cons_iter->getArg1Type()==1) &&
					(cons_iter->getArg1Id()==id)&&
					(cons_iter->getArg2Type()==2)&&
					(cons_iter->getArg2Id()==signal_id)
					) 
				{
					//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start +  (*signals_iter).duration ); 

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
						if (
							!(begin_time <  (*signals_iter).start +  (*signals_iter).duration) 
							) return false;

				}// end of condition 4

				/*
				a cons is of type 5

				<con type="minus">
				<arg id="this" type="start"/> 
				<arg id="other" type="stop"/> 
				<equal value="0"/> 
				</con>  

				start(this)-stop(other) = x
				start(this) = x + stop(other)

				*/

				//if a cons is of type 5
				//or it is a global con
				//or it is a local con and so the intersection has to be true
				//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
				
				//contraditory conditions
				//if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
					if (
						(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
						(cons_iter->isEqual()==true) &&
						(cons_iter->getArg1Type()==1) &&
						(cons_iter->getArg1Id()==id)&&
						(cons_iter->getArg2Type()==2)&&
						(cons_iter->getArg2Id()==signal_id)
						) 
					{
						//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start +  (*signals_iter).duration ); 

						//or it is a global con
						//or it is a local con and so the intersection has to be true
						//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
						if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
							if (
								!(begin_time ==  (*signals_iter).start +  (*signals_iter).duration) 
								) return false;

					}// end of condition 5


					/*
					a cons is of type 6

					<con type="minus">
					<arg id="this" type="start"/> 
					<arg id="other" type="stop"/> 
					<morethan value="0"/> 
					</con>  

					start(this)-stop(other) > x
					start(this) > x + stop(other)
					*/

					if (
						(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
						(cons_iter->isMoreThan()==true) &&
						(cons_iter->getArg1Type()==1) &&
						(cons_iter->getArg1Id()==id)&&
						(cons_iter->getArg2Type()==2)&&
						(cons_iter->getArg2Id()==signal_id)
						) 
					{
						//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start +  (*signals_iter).duration ); 
						
						//or it is a global con
						//or it is a local con and so the intersection has to be true
						//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
						
						//the condition is not valid signals cannot be intersected
						//if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
							if (
								!(begin_time >  (*signals_iter).start +  (*signals_iter).duration) 
								) return false;

					}// end of condition 6

					//suppose that the second argument = a  candicate

					/*
					a cons is of type 7

					<con type="minus">
					<arg id="other" type="start"/> 
					<arg id="this" type="start"/> 						
					<lessthan value="0"/> 
					</con>  

					start(other) - start(this) < x
					start(this) > start(other)

					*/

					//if a cons is of type 7
					if (
						(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
						(cons_iter->isLessThan()==true) &&
						(cons_iter->getArg2Type()==1) &&
						(cons_iter->getArg2Id()==id)&&
						(cons_iter->getArg1Type()==1)&&
						(cons_iter->getArg1Id()==signal_id)
						) 
					{
						
						//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start ); 

						//or it is a global con
						//or it is a local con and so the intersection has to be true
						//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
						if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
							if ( !(begin_time >  (*signals_iter).start) ) return false;

					}// end of condition 7

					/*
					a cons is of type 8

					<con type="minus">
					<arg id="other" type="start"/> 
					<arg id="this" type="start"/> 						
					<equalthan value="0"/> 
					</con>  

					start(other) - start(this) = 0
					start(this) = start(other)

					*/
					//if a cons is of type 8
					if (
						(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
						(cons_iter->isEqual()==true) &&
						(cons_iter->getArg2Type()==1) &&
						(cons_iter->getArg2Id()==id)&&
						(cons_iter->getArg1Type()==1)&&
						(cons_iter->getArg1Id()==signal_id)
						) {
							
							//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start  ); 

							//or it is a global con
							//or it is a local con and so the intersection has to be true
							//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
							if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
								if (
									!(begin_time == (*signals_iter).start ) 
									) return false;

						}// end of condition 8


						/*
						a cons is of type 9

						<con type="minus">
						<arg id="other" type="start"/> 
						<arg id="this" type="start"/> 						
						<morethan value="0"/> 
						</con>  

						start(other)-start(this) > x
						start(this) < start(other)

						*/

						/*
						//if a cons is of type 9
						if (
						(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
						(cons_iter->isMoreThan()==true) &&
						(cons_iter->getArg2Type()==1) &&
						(cons_iter->getArg2Id()==id)&&
						(cons_iter->getArg1Type()==1)&&
						(cons_iter->getArg1Id()==signal_id)
						) 
						{

						if ( !(begin_time <  (*signals_iter).start) ) return false;

						}// end of condition 9
						*/

						/*
						a cons is of type 7

						<con type="minus">
						<arg id="other" type="stop"/> 
						<arg id="this" type="start"/> 						
						<lessthan value="0"/> 
						</con>  

						stop(other) - start(this) < x
						start(this) > stop(other)

						*/
						//if a cons is of type 10
						if (
							(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
							(cons_iter->isLessThan()==true) &&
							(cons_iter->getArg2Type()==1) &&
							(cons_iter->getArg2Id()==id)&&
							(cons_iter->getArg1Type()==2)&&
							(cons_iter->getArg1Id()==signal_id)
							)
						{
							//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start +  (*signals_iter).duration ); 

							//or it is a global con
							//or it is a local con and so the intersection has to be true
							//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
							
							//again contraditory conditions
							//if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
								if (
									!(begin_time >  (*signals_iter).start +  (*signals_iter).duration) 
									) return false;

						}// end of condition 10

						/*
						a cons is of type 7

						<con type="minus">
						<arg id="other" type="stop"/> 
						<arg id="this" type="start"/> 						
						<equal value="0"/> 
						</con>  

						stop(other) - start(this) = x
						start(this) = stop(other)

						*/
						//if a cons is of type 11
						if (
							(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
							(cons_iter->isEqual()==true) &&
							(cons_iter->getArg2Type()==1) &&
							(cons_iter->getArg2Id()==id)&&
							(cons_iter->getArg1Type()==2)&&
							(cons_iter->getArg1Id()==signal_id)
							) 
						{
							//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start +  (*signals_iter).duration ); 

							//or it is a global con
							//or it is a local con and so the intersection has to be true
							//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
							
							//contradotory condition
							if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
								if (
									!(begin_time ==  (*signals_iter).start +  (*signals_iter).duration) 
									) return false;

						}// end of condition 11
						/*
						a cons is of type 7

						<con type="minus">
						<arg id="other" type="stop"/> 
						<arg id="this" type="start"/> 						
						<morethan value="0"/> 
						</con>  

						stop(other) - start(this) > x
						start(this) < stop(other)

						*/
						//if a cons is of type 12
						if (
							(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
							(cons_iter->isMoreThan()==true) &&
							(cons_iter->getArg2Type()==1) &&
							(cons_iter->getArg2Id()==id)&&
							(cons_iter->getArg1Type()==2)&&
							(cons_iter->getArg1Id()==signal_id)
							)
						{
							//printf(" begin_time %f , signal time %f \n", begin_time, (*signals_iter).start +  (*signals_iter).duration ); 

							//or it is a global con
							//or it is a local con and so the intersection has to be true
							//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
							if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time) ) )
								if (
									!(begin_time <  (*signals_iter).start +  (*signals_iter).duration) 
									) return false;

						}// end of condition 12

			}//end if

		}//end of for

	}//end of for


	return true;
}//end of checkStartCons


//
//THIS PART OF CODE IS STILL UNDER DEVELOPPEMENT!!!
//IT IS NOT USED BY GRETA
//

//time in seconds (and not in time-units)	
bool MultimodalSignal::checkStopCons(std::string reference, float begin_time, float end_time, MultimodalEmotionConstraint *mec){

	std::vector<Constraint>* good_cons = mec->getConstraintsOfOneSignal(reference);
	int id = mec->getIdOfSignal (reference);

	//for all relevant cons
	std::vector<Constraint>::iterator cons_iter;
	for(cons_iter=good_cons->begin();cons_iter!=good_cons->end();cons_iter++) {

		//and all signals chosen for the moment
		std::vector<Signal>::iterator signals_iter;
		for(signals_iter=this->begin();signals_iter!=this->end();signals_iter++) {

			int signal_id = mec->getIdOfSignal( (*signals_iter).reference );

			//suppose that the first argument= a  candicate

			/*
			a cons is of type 1

			<con type="minus">
			<arg id="this" type="stop"/> 						
			<arg id="other" type="stop"/> 

			<lessthan value="0"/> 
			</con>  

			stop(this) - stop(other) < x
			stop(this) < stop(other)

			*/
			//if a cons is of type 1
			if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isLessThan()==true) &&
				(cons_iter->getArg1Type()==2) &&
				(cons_iter->getArg1Id()==id)&&
				(cons_iter->getArg2Type()==2)&&
				(cons_iter->getArg2Id()==signal_id)
				) 
			{

				//or it is a global con
				//or it is a local con and so the intersection has to be true
				//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
				
				//this condition is always true as if stop(x)<stop(y) -> x and y intersect
				if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
					if ( ! (end_time <  ( (*signals_iter).start +  (*signals_iter).duration) ) ) return false;

			}// end of condition 1

			/*
			a cons is of type 1

			<con type="minus">
			<arg id="this" type="stop"/> 						
			<arg id="other" type="stop"/> 

			<equal value="0"/> 
			</con>  

			stop(this) - stop(other) = x
			stop(this) = stop(other)

			*/
			//if a cons is of type 2
			if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isEqual()==true) &&
				(cons_iter->getArg1Type()==2) &&
				(cons_iter->getArg1Id()==id)&&
				(cons_iter->getArg2Type()==2)&&
				(cons_iter->getArg2Id()==signal_id)
				) {

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if (
							!(
							end_time ==( (*signals_iter).start +  (*signals_iter).duration ) 
							) 
							) return false;

				}// end of condition 2

				/*
				a cons is of type 1

				<con type="minus">
				<arg id="this" type="stop"/> 						
				<arg id="other" type="stop"/> 

				<morethan value="0"/> 
				</con>  

				stop(this) - stop(other) > x
				stop(this) > stop(other)

				*/
				//if a cons is of type 3
				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isMoreThan()==true) &&
					(cons_iter->getArg1Type()==2) &&
					(cons_iter->getArg1Id()==id)&&
					(cons_iter->getArg2Type()==2)&&
					(cons_iter->getArg2Id()==signal_id)
					)
				{

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if ( 
							! (
							end_time > ( (*signals_iter).start +  (*signals_iter).duration )
							) 
							) return false;

				}// end of condition 3


				/*

				<con type="minus">
				<arg id="this" type="stop"/> 
				<arg id="other" type="start"/> 
				<lessthan value="0"/> 
				</con>  

				stop(this)-start(other) < x
				stop(this) < x + start(other)

				*/

				/*			
				if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isLessThan()==true) &&
				(cons_iter->getArg1Type()==2) &&
				(cons_iter->getArg1Id()==id)&&
				(cons_iter->getArg2Type()==1)&&
				(cons_iter->getArg2Id()==signal_id)
				) 

				{


				if (
				! (
				end_time <  (*signals_iter).start 
				)
				) return false;

				}// end of condition 4
				*/


				//if a cons is of type 5
				/*

				<con type="">
				<arg id="this" type="stop"/> 
				<arg id="other" type="start"/> 
				<equal value="0"/> 
				</con>  

				stop(this)-start(other) = x
				stop(this) = x + start(other)

				*/

				/*
				if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isEqual()==true) &&
				(cons_iter->getArg1Type()==2) &&
				(cons_iter->getArg1Id()==id)&&
				(cons_iter->getArg2Type()==1)&&
				(cons_iter->getArg2Id()==signal_id)
				) 
				{

				//check the condition: a < b + x, x = 0 or x = cons_iter->value
				if (
				!(
				end_time ==  (*signals_iter).start  
				)
				) 
				return false;

				}// end of condition 5
				*/

				/*

				<con type="">
				<arg id="this" type="stop"/> 
				<arg id="other" type="start"/> 
				<morethan value="0"/> 
				</con>  

				stop(this)-start(other) > x
				stop(this) > x + start(other)

				*/

				//if a cons is of type 6
				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isMoreThan()==true) &&
					(cons_iter->getArg1Type()==2) &&
					(cons_iter->getArg1Id()==id)&&
					(cons_iter->getArg2Type()==1)&&
					(cons_iter->getArg2Id()==signal_id)
					) 
				{


					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if (
							!(
							end_time >  (*signals_iter).start 
							)
							) return false;

				}// end of condition 6

				//suppose that the second argument = a  candicate
				/*

				<con type="">
				<arg id="other" type="stop"/> 
				<arg id="this" type="stop"/> 
				<lessthan value="0"/> 
				</con>  

				stop(other)-stop(this) < x
				stop(this) > x + stop(other)

				*/
				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isLessThan()==true) &&
					(cons_iter->getArg2Type()==2) &&
					(cons_iter->getArg2Id()==id)&&
					(cons_iter->getArg1Type()==2)&&
					(cons_iter->getArg1Id()==signal_id)
					) 
				{

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if ( 
							!(
							end_time > ( (*signals_iter).start +  (*signals_iter).duration )
							) 
							) return false;

				}// end of condition 7

				/*
				<con type="">
				<arg id="other" type="stop"/> 
				<arg id="this" type="stop"/> 
				<equal value="0"/> 
				</con>  

				stop(other)-stop(this) = x
				stop(this) = x + stop(other)

				*/
				//if a cons is of type 8
				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isEqual()==true) &&
					(cons_iter->getArg2Type()==2) &&
					(cons_iter->getArg2Id()==id)&&
					(cons_iter->getArg1Type()==2)&&
					(cons_iter->getArg1Id()==signal_id)
					)
				{

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if (
							!(
							end_time == ( (*signals_iter).start +  (*signals_iter).duration )
							) 
							) return false;

				}// end of condition 8

				/*
				<con type="">
				<arg id="other" type="stop"/> 
				<arg id="this" type="stop"/> 
				<morethan value="0"/> 
				</con>  

				stop(other)-stop(this) > x
				stop(this) < x + stop(other)

				*/
				//if a cons is of type 9
				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isMoreThan()==true) &&
					(cons_iter->getArg2Type()==2) &&
					(cons_iter->getArg2Id()==id)&&
					(cons_iter->getArg1Type()==2)&&
					(cons_iter->getArg1Id()==signal_id)
					)
				{

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)

					//this condition is always true as if stop(x)<stop(y) -> x and y intersect
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if (
							!(
							end_time <  ( (*signals_iter).start +  (*signals_iter).duration )
							)
							) return false;

				}// end of condition 9

				/*

				<con type="">
				<arg id="other" type="start"/> 
				<arg id="this" type="stop"/> 
				<lessthan value="0"/> 
				</con>  

				start(other)-stop(this) < x
				stop(this) > x + start(other)

				*/
				//if a cons is of type 10
				if (
					(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
					(cons_iter->isLessThan()==true) &&
					(cons_iter->getArg2Type()==2) &&
					(cons_iter->getArg2Id()==id)&&
					(cons_iter->getArg1Type()==1)&&
					(cons_iter->getArg1Id()==signal_id)
					) 
				{

					//or it is a global con
					//or it is a local con and so the intersection has to be true
					//if it is not a global con and the interesection is false then do not check the condition (true by default as con cannot be applied in this case)
					if ( (strcmp(cons_iter->getConstraintRange().c_str(),"global")==0) || ( (*signals_iter).intersect(begin_time,end_time) ) )
						if (
							!(
							end_time >  (*signals_iter).start
							) 
							) return false;

				}// end of condition 10

				/*			
				<con type="minus">
				<arg id="other" type="start"/> 
				<arg id="this" type="stop"/> 
				<equal value="0"/> 
				</con>  

				start(other) - stop(this)= 0
				stop(this) =  start(other)

				*/


				/*

				//if a cons is of type 11
				if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isEqual()==true) &&
				(cons_iter->getArg2Type()==2) &&
				(cons_iter->getArg2Id()==id)&&
				(cons_iter->getArg1Type()==1)&&
				(cons_iter->getArg1Id()==signal_id)
				) {

				if (
				!(
				end_time ==  (*signals_iter).start 
				)
				) return false;

				}// end of condition 11
				*/
				/*			
				<con type="minus">
				<arg id="other" type="start"/> 
				<arg id="this" type="stop"/> 
				<morethan value="0"/> 
				</con>  

				start(other) - stop(this) > 0
				stop(this) <  start(other)

				*/
				/*
				//if a cons is of type 12
				if (
				(strcmp(cons_iter->getConstraintType().c_str(), "minus")==0)&&
				(cons_iter->isMoreThan()==true) &&
				(cons_iter->getArg2Type()==2) &&
				(cons_iter->getArg2Id()==id)&&
				(cons_iter->getArg1Type()==1)&&
				(cons_iter->getArg1Id()==signal_id)
				) 
				{

				if (
				!(
				end_time <  (*signals_iter).start 
				)
				) return false;

				}// end of condition 12
				*/


		}//end of for

	}//end of for


	return true;
}//end of checkEndCons




MultimodalSignal MultimodalSignal::clone ()
{

	//check it
	MultimodalSignal cloned;

	std::vector<Signal>::const_iterator iter;
	for(iter=this->begin();iter!=this->end();iter++)
	{
		Signal s(*iter);
		cloned.push_back(s);
	}
	cloned.preference=this->preference;

	return cloned;
}

