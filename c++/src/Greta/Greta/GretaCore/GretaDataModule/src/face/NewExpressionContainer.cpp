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

// NewExpressionContainer.cpp: implementation of the  class.
//
//////////////////////////////////////////////////////////////////////
// created on 12 février 2008  by : Radek  (niewiadomski@iut.univ-paris8.fr)

//modifications:
//

#include "NewExpressionContainer.h"
#include "XMLGenericParser.h"
#include "XMLGenericTree.h"
#include <direntwin.h>

using namespace cmlabs;

NewExpressionContainer::NewExpressionContainer()
{
	//??	
}


int NewExpressionContainer::init(std::string directory)
{

	//read extended definitions

	DIR *pdir;

	struct dirent *pent;

	pdir=opendir(directory.c_str());

	if (!pdir) {

		//printf("Directory does not exist");
		expressionlist.clear();
		return 0;
		//throw Exception ("Directory does not exist", 1);
	}

	else

	{

		//errno=0;

		while ((pent=readdir(pdir))){

			//if (errno){  errno=0;continue;}

			std::string temp_filename=pent->d_name;            			             			

			//add other directories
			if (temp_filename.find_first_of("dtd")==0) continue;
			if (temp_filename.find_first_of("xmls")==0) continue;
			if (temp_filename.find_first_of(".")==0) continue;

			XMLGenericParser *xmlparser=new XMLGenericParser();

			XMLGenericTree *tree = xmlparser->ParseFile(directory+"\\"+temp_filename);

			if(!tree==0) {

				XMLGenericTree *head=tree;

				if(!head->attributes.empty()) {//the tag 'expression' should have one attribute name

					Expression* new_expression = new Expression();// if the object has not the name we  ignore it

					std::list<XMLAttribute>::iterator att_iter;

					std::string temp_name = "";

					for(att_iter=(*head).attributes.begin();att_iter!=(*head).attributes.end();att_iter++)
						if ((*att_iter).name.compare("name")==0) new_expression->setName((*att_iter).value);


					std::list<XMLGenericTree*> child = head->child;
					std::list<XMLGenericTree*>::iterator child_iter;

					if(!child.empty()) 
					{

						for(child_iter=child.begin();child_iter!=child.end();child_iter++) //for each expression (usually there is only one)
						{ //begining for

							std::list<XMLGenericTree*> nipote;

							//char
							if (((*child_iter)->name).compare("char")==0) 
							{

								nipote  =  (*child_iter)->child;

								std::list<XMLGenericTree*>::iterator nipote_iter;

								if (!nipote.empty()) 
								{

									for(nipote_iter=nipote.begin();nipote_iter!=nipote.end();nipote_iter++) 
									{

										if (((*nipote_iter)->name).compare("emotion")==0) 
										{

											std::list<XMLAttribute>::iterator nipote_att_iter;

											std::string temp_emotion="";

											for(nipote_att_iter=(*nipote_iter)->attributes.begin();nipote_att_iter!=(*nipote_iter)->attributes.end();nipote_att_iter++) 
											{

												if ((*nipote_att_iter).name.compare("name")==0) 
												{

													temp_emotion = (*nipote_att_iter).value.c_str();
												}

												new_expression->setEmotionName(temp_emotion);

											}//end for all atributes

										}//end if - emotion

										if ((*nipote_iter)->name.compare("profile")==0) 
										{

											std::list<XMLAttribute>::iterator nipote_att_iter;

											std::string temp_profile="";

											for(nipote_att_iter=(*nipote_iter)->attributes.begin();nipote_att_iter!=(*nipote_iter)->attributes.end();nipote_att_iter++) 
											{

												if ((*nipote_att_iter).name.compare("name")==0) 
												{

													temp_profile = (*nipote_att_iter).value.c_str();
												}

												new_expression->setProfileName(temp_profile);

											}//end for all atributes

										}//end if profile - profile

										if ((*nipote_iter)->name.compare("time")==0) 
										{

											std::list<XMLAttribute>::iterator nipote_att_iter;

											float temp_min = 0;
											float temp_max = 0;

											for(nipote_att_iter=(*nipote_iter)->attributes.begin();nipote_att_iter!=(*nipote_iter)->attributes.end();nipote_att_iter++)

											{

												if ((*nipote_att_iter).name.compare("min")==0) {

													temp_min = atof((*nipote_att_iter).value.c_str());
													new_expression->setTimeMin(temp_min);
												}

												if ((*nipote_att_iter).name.compare("max")==0) 
												{

													temp_max = atof((*nipote_att_iter).value.c_str());
													new_expression->setTimeMax(temp_max);
												}                                                                                                                                                


											}//end for all atributes

										}//end of if time

										if ((*nipote_iter)->name.compare("intensity")==0) 
										{
											std::list<XMLAttribute>::iterator nipote_att_iter;

											float int_min = 0;
											float int_max = 0;

											for(nipote_att_iter=(*nipote_iter)->attributes.begin();nipote_att_iter!=(*nipote_iter)->attributes.end();nipote_att_iter++)

											{

												if ((*nipote_att_iter).name.compare("min")==0) {

													int_min = atof((*nipote_att_iter).value.c_str());
													new_expression->setIntensityMin(int_min);
												}

												if ((*nipote_att_iter).name.compare("max")==0) 
												{

													int_max = atof((*nipote_att_iter).value.c_str());
													new_expression->setIntensityMax(int_max);

												}


											}//end of for all atributes


										}//end of if - intensity

									}//end for all children
								}//end of child is not empty

							}//end of char

							//face

							if (((*child_iter)->name).compare("face")==0) 
							{
								nipote = (*child_iter)->child;

								std::list<XMLGenericTree*>::iterator nipote_iter;

								int temp_faps [69];

								for (int i=0;i<69;i++) temp_faps[i]=0;

								if(!nipote.empty()) 
								{
									for(nipote_iter=nipote.begin();nipote_iter!=nipote.end();nipote_iter++) 
									{

										if (((*nipote_iter)->name).compare("fap")==0) 
										{

											std::list<XMLAttribute>::iterator nipote_att_iter;

											int temp_fap=0;
											int temp_value=0;

											for(nipote_att_iter=(*nipote_iter)->attributes.begin();nipote_att_iter!=(*nipote_iter)->attributes.end();nipote_att_iter++)

											{                                                

												if ((*nipote_att_iter).name.compare("num")==0) {

													temp_fap = atoi((*nipote_att_iter).value.c_str());
												}

												if ((*nipote_att_iter).name.compare("value")==0) {

													temp_value = atoi((*nipote_att_iter).value.c_str());
												}

												if ((temp_fap!=0)&&(temp_value!=0)) 
												{
													temp_faps[temp_fap]=temp_value;
													temp_fap=0;
													temp_value=0;
												}

											}//end of for all childs

										} //end of fap condition


									}//end of for all childs

									new_expression->setFaps(temp_faps);

								}//end of if child is not empty


							}//end of face

							//cons
							if (((*child_iter)->name).compare("cons")==0) 
							{
								//no cons at the moment
							}//end of cons                                                                                    

						}//end of for all childs in 

						expressionlist.push_back(new_expression);

					}//end if child is not empty

				}//end of if head not empty

				delete xmlparser;

			} //end of if tree!=0
			else 
			{ 
				//tree==0 - means no file inside
				return 0;
				//throw Exception ("The extended definition of emotion was not found", 2);

			} // end of else

		}//end of while - no more file

	}//end of else

	closedir(pdir);

	return 1;

} //end of constructor


NewExpressionContainer::~NewExpressionContainer()
{
	//TO DO: free memory	

}


std::list<Expression*> NewExpressionContainer::getExpressions()
{
	return expressionlist;
}


Expression* NewExpressionContainer::findExpression(std::string label)
{

	if ( ( !expressionlist.empty() )  ) 
	{

		std::list<Expression*>::iterator emotion_iter;

		for(emotion_iter=expressionlist.begin();emotion_iter!=expressionlist.end();emotion_iter++){

			std::string temp_name = ((*emotion_iter)->getName()).c_str();

			if (temp_name==label) { return (*emotion_iter); }

		}//end of for

	}//end of of

	return 0;

}//end of findEmotion



/*
returns first expression that corresponds to to a given pattern (emotion name,intensity)
*/

//TO DO: a direcotry of expressions can contain more than one expression that suit to the pattern
//TO DO: a vector of expressions should be used

Expression* NewExpressionContainer::findExpressionByEmotionAndIntensity(std::string label, float intensity){

	if ( ( !expressionlist.empty() )  ) 
	{

		std::list<Expression*>::iterator emotion_iter;

		for(emotion_iter=expressionlist.begin();emotion_iter!=expressionlist.end();emotion_iter++)
		{

			std::string temp_emotion_name = ((*emotion_iter)->getEmotionName()).c_str();

			float temp_int_min = (*emotion_iter)->getIntensityMin();
			float temp_int_max = (*emotion_iter)->getIntensityMax();

			if ((temp_emotion_name==label)&&(temp_int_min<=intensity)&&(intensity<=temp_int_max)) 
			{ 
				return (*emotion_iter);
			}

		}//end of for

	}//end of if

	return 0;

}//end of findEmotion



void NewExpressionContainer::printList()
{

	if ( ( !expressionlist.empty() )  ) 
	{

		std::list<Expression*>::iterator emotion_iter;

		for(emotion_iter=expressionlist.begin();emotion_iter!=expressionlist.end();emotion_iter++)
		{

			std::string temp_name = ((*emotion_iter)->getName()).c_str();            

			printf("Name:%s\n", ((*emotion_iter)->getName()).c_str());
			printf("Emotion:%s\n", ((*emotion_iter)->getProfileName()).c_str());
			printf("Profile:%s\n", ((*emotion_iter)->getProfileName()).c_str());

			printf("Time min: %f, max: %f \n", ((*emotion_iter)->getTimeMin()),((*emotion_iter)->getTimeMax()));

			printf("Intensity min: %f, max: %f \n", ((*emotion_iter)->getIntensityMin()),((*emotion_iter)->getIntensityMax()));

			int *fap = (*emotion_iter)->getFaps();
			for (int i=0;i<69;i++)
			{
				if (fap[i]!=0) printf("Fap:%d, value: %d  \n",i,fap[i] );
			}

		}

	}

	else printf("List is empty \n");

}//end of printlist
