//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// ConstraintsContainer.cpp: implementation of the ConstraintsContainer class.
//
//////////////////////////////////////////////////////////////////////
// created on ...  by : Radek  (niewiadomski@iut.univ-paris8.fr)

//modifications:
//

#include "ConstraintsContainer.h"
#include "Constraint.h"
#include "MultimodalEmotionConstraint.h"
#include "XMLGenericParser.h"
#include <vector>

ConstraintsContainer::ConstraintsContainer () 
{
}


int ConstraintsContainer::init (std::string file_name)
{

	if(file_name=="")
	{
		printf("ConstraintsContainer: no filename given\n");		
		return 0;
		//throw Exception("ConstraintsContainer: no filename given",1);
	}

	XMLGenericParser p;
	XMLGenericTree *t;
	p.SetValidating(true);
	t=p.ParseFile(file_name);

	if(t==0)
	{
		printf("Error parsing file %s\n",file_name.c_str());		
		return 0;
		//throw Exception("Error parsing file",2);
	}


	if(!t==0) 
	{

		std::list<XMLGenericTree*>::iterator emotions_iter;

		for(emotions_iter=(t)->child.begin();emotions_iter!=(t)->child.end();emotions_iter++) //for each singal
		{						

			if((*emotions_iter)->attributes.empty()) 
			{

				// to ch.. bo musi byc nazwa emocji jako atrybut	

			} 

			else 

			{

				//read atributes
				std::string emotion_name = (*emotions_iter)->GetAttribute("emotion");

				std::list<XMLGenericTree*>::iterator iter;

				std::list<XMLGenericTree*> child = (*emotions_iter)->child;

				if(!child.empty()) 
				{

					MultimodalEmotionConstraint mec;		
					mec.setName(emotion_name);

					for(iter=child.begin();iter!=child.end();iter++) //for each child
					{
						if(strcmp((*iter)->name.c_str(),"signals")==0) 
						{

							std::list<XMLGenericTree*>::iterator signals_iter;

							for(signals_iter=(*iter)->child.begin();signals_iter!=(*iter)->child.end();signals_iter++) //for each singal
							{

								ConstraintSignal signal1;

								signal1.setProbability_end(atof((*signals_iter)->GetAttribute("probability_end").c_str()));

								signal1.setId(atoi((*signals_iter)->GetAttribute("id").c_str()));
								signal1.setOccurence(atof((*signals_iter)->GetAttribute("occurence").c_str()));
								signal1.setRepetivity(atof((*signals_iter)->GetAttribute("repetitivity").c_str()));
								signal1.setMax_duration(atof((*signals_iter)->GetAttribute("max_duration").c_str()));
								
								signal1.setMin_duration(atof((*signals_iter)->GetAttribute("min_duration").c_str()));

								signal1.setProbability_start(atof((*signals_iter)->GetAttribute("probability_start").c_str()));

								signal1.setName(((*signals_iter)->GetAttribute("name").c_str()));

								mec.addConstraintSignal(signal1);

							}//end for

						}//end if

						if ((strcmp((*iter)->name.c_str(),"cons")==0)&&(!(*iter)->child.empty())) 
						{

							std::list<XMLGenericTree*>::iterator con_iter;

							for(con_iter=(*iter)->child.begin();con_iter!=(*iter)->child.end();con_iter++) //for each child

							{

								//std::list<XMLAttribute>::iterator con_iter;

								//for(sig=(*con_iter)->attributes.begin();att_iter!=(*con_iter)->attributes.end();att_iter++)

								//{

								Constraint con1;

								//fill camps

								con1.setConstraintType((*con_iter)->GetAttribute("type"));

								con1.setConstraintRange( (*con_iter)->GetAttribute("range") );

								if (con1.getConstraintType()=="minus")
								{

									if((*con_iter)->child.empty()==false) 
									{

										std::list<XMLGenericTree*>::iterator altshapes;

										int	i=0;

										for(altshapes=(*con_iter)->child.begin();altshapes!=(*con_iter)->child.end();altshapes++)
										{

											if (i==0)
											{

												int id= atoi(((*altshapes)->GetAttribute("id")).c_str());
												std::string stype= (*altshapes)->GetAttribute("type");
												int itype=-1;
												if (stype=="stop") itype=2;
												if (stype=="start") itype=1;

												con1.setArg1(id,itype);

											}

											if (i==1)
											{
												int id= atoi(((*altshapes)->GetAttribute("id")).c_str());
												std::string stype= (*altshapes)->GetAttribute("type");
												int itype=-1;
												if (stype=="stop") itype=2;
												if (stype=="start") itype=1;
												con1.setArg2(id,itype);
											}

											if (i==2)
											{
												std::string stype = (*altshapes)->name.c_str();
												if (stype=="lessthan") con1.setLessThan();
												if (stype=="morethan") con1.setMoreThan();
												if (stype=="equal") con1.setEqual();

												float value = atof(((*altshapes)->GetAttribute("value")).c_str());

												con1.setValue(value);

											}
											i++;

										}//end of for po altdhad

									}// end if 

								}// end "minus"


								if (con1.getConstraintType()=="simple")
								{

									if((*con_iter)->child.empty()==false)
									{
										std::list<XMLGenericTree*>::iterator altshapes;

										int	i=0;

										for(altshapes=(*con_iter)->child.begin();altshapes!=(*con_iter)->child.end();altshapes++)
										{
											if (i==0)
											{

												int id= atoi(((*altshapes)->GetAttribute("id")).c_str());
												std::string stype= (*altshapes)->GetAttribute("type");
												int itype=-1;
												if (stype=="stop") itype=2;
												if (stype=="start") itype=1;

												con1.setArg1(id,itype);

											}

											if (i==1)
											{

												std::string stype = (*altshapes)->name.c_str();
												if (stype=="lessthan") con1.setLessThan();
												if (stype=="morethan") con1.setMoreThan();
												if (stype=="equal") con1.setEqual();

												float value = atof(((*altshapes)->GetAttribute("value")).c_str());
												con1.setValue(value);

											}

											i++;

										}//end for
									}//end if not


								}//end simple				

								//add to mec a con

								mec.addConstraint(con1);

							}//end  of for po con'ach

							//add mec to list

							this->mecs[mec.getName()]=mec;

						} //end of if cons


					}//end of childs


				}//end of if empty

			}

		}//end for emotions

	}//end of if==0

	return 1;

}//end of init


ConstraintsContainer::~ConstraintsContainer ()
{
}

MultimodalEmotionConstraints ConstraintsContainer::getMultimodalEmotionConstraints()
{
	return this->mecs;
}

void ConstraintsContainer::printAll()
{

	if(mecs.empty()==false)
	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;
		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{
			printf("Name : %s \n", (*iter).first.c_str());	


			MultimodalEmotionConstraint mec=((*iter).second);

			std::vector<Constraint> *constraints = mec.getConstraints();

			std::vector<Constraint>::iterator constraint_iter;

			for(constraint_iter=constraints->begin();constraint_iter!=constraints->end();constraint_iter++)
			{

				printf("constraint type, %s \n", (*constraint_iter).getConstraintType().c_str());
				printf("arg1id, %i \n", (*constraint_iter).getArg1Id());
				printf("arg2id, %i \n", (*constraint_iter).getArg2Id());
				printf("arg1type, %i \n", (*constraint_iter).getArg1Type());
				printf("arg2type, %i \n", (*constraint_iter).getArg2Type());

			}//end of for						

			std::vector<ConstraintSignal> *signals = mec.getConstraintSignals();

			std::vector<ConstraintSignal>::iterator signal_iter;

			for(signal_iter=signals->begin();signal_iter!=signals->end();signal_iter++)
			{

				printf("signal name, %s \n", (*signal_iter).getName().c_str());
				printf("signal id, %i \n", (*signal_iter).getId());
				printf("signal max duration, %f \n", (*signal_iter).getMax_duration());
				printf("signal repetitivity, %f \n", (*signal_iter).getRepetivity());
				printf("signal prob start, %f \n", (*signal_iter).getProbability_start());
				printf("signal prob end, %f \n", (*signal_iter).getProbability_end());
				printf("signal occurence, %f \n", (*signal_iter).getOccurence());

			}//end of for						

		}//end of for
	}
}


float ConstraintsContainer::getRepetivity(std::string emotion, std::string signalname)
{

	if(mecs.empty()==false)

	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;

		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{

			std::string current_emotion_name = (*iter).first;

			MultimodalEmotionConstraint * mec = &(mecs[iter->first]);

			std::vector<ConstraintSignal> *signals = mec->getConstraintSignals();

			std::vector<ConstraintSignal>::iterator signals_iter;

			for(signals_iter=signals->begin();signals_iter!=signals->end();signals_iter++){

				std::string current_signal_name=(*signals_iter).clone().getName();

				if((current_emotion_name.compare(emotion)==0)  &&( current_signal_name.compare(signalname)==0 ) ){

					return (*signals_iter).getRepetivity();				
				}

			}//end of for						

		}//end of for
	}

	return 0.0f;

}


float ConstraintsContainer::getProbability_start(std::string emotion, std::string signalname)
{

	if(mecs.empty()==false)

	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;

		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{

			std::string current_emotion_name = (*iter).first;

			MultimodalEmotionConstraint * mec = &(mecs[iter->first]);

			std::vector<ConstraintSignal> *signals = mec->getConstraintSignals();

			std::vector<ConstraintSignal>::iterator signals_iter;

			for(signals_iter=signals->begin();signals_iter!=signals->end();signals_iter++)
			{

				std::string current_signal_name=(*signals_iter).clone().getName();

				if((current_emotion_name.compare(emotion)==0)  && ( current_signal_name.compare(signalname)==0 ) )
				{

					return (*signals_iter).getProbability_start();				
				}

			}//end of for						

		}//end of for
	}

	return 0.0f;

}

float ConstraintsContainer::getProbability_end(std::string emotion, std::string signalname)
{

	if(mecs.empty()==false)

	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;

		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{

			std::string current_emotion_name = (*iter).first;

			MultimodalEmotionConstraint * mec = &(mecs[iter->first]);

			std::vector<ConstraintSignal> * signals = mec->getConstraintSignals();

			std::vector<ConstraintSignal>::iterator signals_iter;

			for(signals_iter=signals->begin();signals_iter!=signals->end();signals_iter++)
			{

				std::string current_signal_name=((*signals_iter).clone()).getName();

				if( ( current_emotion_name.compare(emotion)==0 ) &&( current_signal_name.compare(signalname)==0 ) )
				{

					return (*signals_iter).getProbability_end();				
				}

			}//end of for						

		}//end of for
	}

	return 0.0f;

}

float ConstraintsContainer::getOccurence(std::string emotion, std::string signalname)	
{

	if(mecs.empty()==false)

	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;

		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{

			std::string current_emotion_name = (*iter).first;

			MultimodalEmotionConstraint * mec = &(mecs[iter->first]);

			std::vector<ConstraintSignal> *signals = mec->getConstraintSignals();

			std::vector<ConstraintSignal>::iterator signals_iter;

			for(signals_iter=signals->begin();signals_iter!=signals->end();signals_iter++)
			{

				std::string current_signal_name=(*signals_iter).clone().getName();

				if((current_emotion_name.compare(emotion)==0) &&( current_signal_name.compare(signalname)==0 ) )
				{

					return (*signals_iter).getOccurence();				
				}

			}//end of for						

		}//end of for
	}

	return 0.0f;

}

float ConstraintsContainer::getMax_duration(std::string emotion, std::string signalname)
{

	if (signalname.compare("")==0) return 0.0;

	if(mecs.empty()==false)

	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;

		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{

			std::string current_emotion_name = (*iter).first;

			MultimodalEmotionConstraint * mec = &(mecs[iter->first]);

			std::vector<ConstraintSignal> *signals = mec->getConstraintSignals();

			std::vector<ConstraintSignal>::iterator signals_iter;

			for(signals_iter=signals->begin();signals_iter!=signals->end();signals_iter++)
			{

				std::string current_signal_name=(*signals_iter).clone().getName();

				if((current_emotion_name.compare(emotion)==0)  &&( current_signal_name.compare(signalname)==0 ) )
				{

					return (*signals_iter).getMax_duration();				
				}

			}//end of for						

		}//end of for
	}

	return 0.0f;

}


MultimodalEmotionConstraint* ConstraintsContainer::getMultimodalEmotionConstraint(std::string emotion)	
{

	if(mecs.empty()==false)

	{
		std::map<std::string,MultimodalEmotionConstraint>::const_iterator iter;

		for(iter=mecs.begin();iter!=mecs.end();iter++)
		{

			std::string current_emotion_name = (*iter).first;

			MultimodalEmotionConstraint * mec = &(mecs[iter->first]);

			if (current_emotion_name.compare(emotion)==0) return mec;

		}

	}

	return NULL;
}
