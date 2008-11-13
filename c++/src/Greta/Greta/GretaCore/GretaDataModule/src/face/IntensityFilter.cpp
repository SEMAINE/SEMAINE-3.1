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

// Intensityfilter.cpp: implementation of theIntensityfilter class.
//
//////////////////////////////////////////////////////////////////////
// created on 12 février 2008  by : Radek  (niewiadomski@iut.univ-paris8.fr)

//modifications:
//

#include "IntensityFilter.h"
#include "IniManager.h"

extern IniManager inimanager;



IntensityFilter::IntensityFilter()
{
}
	
	int IntensityFilter::init(std::string filename)
	{
	
	//read emotions;
	xmlparser=new XMLGenericParser();

	XMLGenericTree *tree = xmlparser->ParseFile(filename);

	if(!tree==0) 
	{

		XMLGenericTree *head=tree;

		if(head->attributes.empty()) 
		{

			std::list<XMLGenericTree*>::iterator iter;
			std::list<XMLGenericTree*> child =head->child;

			if(!child.empty()) 
			{

				for(iter=child.begin();iter!=child.end();iter++) //for each emotion
				{//begining for

					std::list<XMLAttribute>::iterator att_iter;

					std::string temp_name = "";
					float temp_evaluation = 0;

					for(att_iter=(*iter)->attributes.begin();att_iter!=(*iter)->attributes.end();att_iter++)
					{

						if ((*att_iter).name.compare("name")==0) 
						{

							temp_name = (*att_iter).value; 
						}

						if ((*att_iter).name.compare("evaluation")==0) 
						{ 

							temp_evaluation = atof((*att_iter).value.c_str()); 
						}

					}	

					if 	(temp_name != "")
					{

						Emotion *temp_emotion = new Emotion (temp_name,temp_evaluation);						
						emotionvector.push_back(temp_emotion);

					}//end if

				}//end for

			}//end if
		}//end if

	}//end if

	else 

	{ 
		//tree is empty
		return 0;
		//throw Exception ("Intensity Filter exception", 1);
	}

	delete xmlparser;

	return 1;
}




Emotion* IntensityFilter::findEmotion(std::string label)
{

	if ( (!(&emotionvector==0)) && (!(emotionvector.empty() ) )  )
	{

		std::list<Emotion*>::iterator emotion_iter;

		for(emotion_iter=emotionvector.begin();emotion_iter!=emotionvector.end();emotion_iter++)
		{

			std::string temp_name = ((*emotion_iter)->getName()).c_str();
			float temp_evaluation = (*emotion_iter)->getEvaluation();

			if (temp_name==label) { return (*emotion_iter); }

		}//end of for

	}//end of if

	return 0;
}


void IntensityFilter::printList()
{
	if ( (!(&emotionvector==0)) && (!(emotionvector.empty() ) )  )
	{
		std::list<Emotion*>::iterator emotion_iter;

		for(emotion_iter=emotionvector.begin();emotion_iter!=emotionvector.end();emotion_iter++)
		{
			std::string temp_name = ((*emotion_iter)->getName()).c_str();
			float temp_evaluation = (*emotion_iter)->getEvaluation();
		}

	} 
	else printf("List is empty\n");	
}

IntensityFilter::~IntensityFilter()
{
}


FaceExpression* IntensityFilter::modify(FaceExpression *fe1,float intensity, float probability)
{
	FaceExpression* new_expression = fe1->clone();

	/*
	//if the signal is neither "complex:" nor "fapfile:" search it in facelibrary or in EFE

	fe=this->FAPFileGenerator->FaceDictionary->GetExpression(comm_act->function,comm_act->value);
	if(fe!=0)
	{
	//if (desciption in EFE) use it;

	// OK if (no desciption in EFE) && (comm_act->getIntensity()>=0.5) use facelibrary;
	// OK if (no desciption in EFE) && (comm_act->getIntensity()<0.5) && (emotion is negative) use only upper face;
	// OK if (no desciption in EFE) && (comm_act->getIntensity()<0.5) && (emotion is positive or unknown) use only lower face;

	comm_act->face_expr_faps=fe->GetRawFaps(comm_act->prob);
	comm_act->fe=fe;
	comm_act->fe->GetMovement(comm_act->prob,&comm_act->head_m_type,&comm_act->head_m_amplitude,&comm_act->head_m_period,comm_act->expressivityREP,comm_act->expressivitySPC);

	*/

	if (inimanager.GetValueFloat("INTENSITY_PROB")>probability) 
	{

		if (intensity<=0.5f) 
		{
			//debug fe1->getInstance() !!!
			
			Emotion* temp_emotion = this->findEmotion(fe1->getInstance());

			if (temp_emotion!=0) 
			{

				float temp_evaluation = temp_emotion->getEvaluation();

				if (temp_evaluation>0) 
				{
					//use the lower face

					std::vector<FAPFrame> frames = new_expression->GetFrames();

					std::vector<FAPFrame>::iterator iter;

					for(iter=frames.begin();iter!=frames.end();iter++) //all frames of face expression from facelibrary
					{

						for(int i=0;i<69;i++) 
						{ 
							//all faps
							//raw_faps[0][i]=(*iter).FAPs[i].active;

							if (((i>=3)&&(i<=18))||((i>=40)&&(i<=47))||((i>=51)&&(i<=64))) 
								(*iter).SetFAP(i,(*iter).GetFAP(i)); 
							else (*iter).SetFAP(i,0);

						}//end for

					}//end for				

					new_expression->SetFrames(frames);

				} 
				else 
				{ 
					//use the upper face

					std::vector<FAPFrame> frames = new_expression->GetFrames();

					std::vector<FAPFrame>::iterator iter;

					for(iter=frames.begin();iter!=frames.end();iter++)//all frames of face expression from facelibrary
					{

						for(int i=0;i<69;i++) 
						{ 
							//raw_faps[0][i]=(*iter).FAPs[i].active;

							if (!(((i>=3)&&(i<=18))||((i>=40)&&(i<=47))||((i>=51)&&(i<=64))))  
								(*iter).SetFAP(i,(*iter).GetFAP(i)); 
							else (*iter).SetFAP(i,0);

						}//end for

					}//end for				

					new_expression->SetFrames(frames);

				}//end of else

			}//end: if no data for this emotion in xml file
			else 
			{ 
				new_expression -> rescaleIntensity(intensity);

			}

		}//end: if intensity...

	} else 
	{ //lower propability 
		new_expression -> rescaleIntensity(intensity);
	}

	return new_expression;
}

