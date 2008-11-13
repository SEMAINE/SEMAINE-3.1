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

// FaceExpressionDictionary.cpp: implementation of the FaceExpressionDictionary class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceExpressionDictionary.h"
#include "XMLGenericParser.h"
#ifdef _DEBUG
#include <crtdbg.h>
#include <stdlib.h>
#endif

extern FILE* data_log;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FaceExpressionDictionary::FaceExpressionDictionary()
{

}

FaceExpressionDictionary::~FaceExpressionDictionary()
{
	FExpressions.clear();
}

int FaceExpressionDictionary::Init()
{
	return Init("");
}

int FaceExpressionDictionary::Init(std::string facelibrarypath)
{
	// Get current flag
	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn on check function at every alloc/dealloc
	// tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

	// Set flag to the new value
	//_CrtSetDbgFlag( tmpFlag );
	//parse the face library

//	printf("reading the facelibrary... ");

	if(FExpressions.empty()==false)
		FExpressions.clear();

	XMLGenericParser *xmlparser;
	xmlparser=new XMLGenericParser();
	xmltree=xmlparser->ParseFile(facelibrarypath);
	if(xmltree==0)
		return 0;
	//if(xmltree!=0)
	//	xmltree->PrintWithAttributes("");
	StoreDictionary();
//	printf("done\n");
	//create a facial dictionary based on the parsed xml tree
	delete xmlparser;
	delete xmltree;
	return 1;
}


int FaceExpressionDictionary::StoreDictionary()
{
	XMLGenericTree *t;
	t=xmltree->FindNodeCalled("implementation");
	std::list<XMLGenericTree*>::iterator iter;
		if(!(t->child).empty())
			for(iter=(t->child).begin();iter!=(t->child).end();iter++)
			{
				std::string classname,instance;
				classname=(*iter)->GetAttribute("class");
				instance=(*iter)->GetAttribute("instance");
				this->stringToLower(classname);
				this->stringToLower(instance);
				if((classname!="")&&(instance!=""))
				{
					fprintf(data_log,"expression class %s instance %s\n",classname.c_str(),instance.c_str());
					FaceExpression e;
					e.setClassname(classname);
					e.setInstance(instance);
					std::list<XMLGenericTree*>::iterator children;
					children=((*iter)->child).begin();
					
					FAPFrame lastframe;

					if((*children)->name=="action")
					{
						FAPFrame frame;
						if(BuildFAPFrame(*iter,frame,1)==0)
						{
							fprintf(data_log,"error, action undefined\n");
							return 0;
						}
						
						FAPFrameVector temp = 	e.GetFrames();
						temp.push_back(frame);
						e.SetFrames(temp);	

						lastframe=frame;
					}
					if((*children)->name=="probability")
					{
						while((children!=(*iter)->child.end())&&((*children)->name=="probability"))
						{
							FAPFrame frame;
							frame.probability=atof((*children)->GetAttribute("value").c_str());
							if(BuildFAPFrame(*children,frame,1)==0)
							{
								fprintf(data_log,"error, action undefined\n");
								return 0;
							}
							
							
							FAPFrameVector temp = 	e.GetFrames();
							temp.push_back(frame);
							e.SetFrames(temp);	

							//(e.getFrames()).push_back(frame); //Add element at the end of vector
							
							lastframe=frame;
							children++;
						}
					}// e.getFrames() is a vector of expressions becouse one expression can have different definitions with the weights (or probabilities accocieted with)


					
					children=((*iter)->child).begin(); // check the rest of the data
					while(children!=(*iter)->child.end())
					{
						if((*children)->name=="channel") fprintf(data_log,"I ignore channels. Do not use them in facelibrary.\n");
						children++;
					}


					// find channels automatically					
					//FAPFrame temp_frame = (FAPFrame)(*((FAPFrameVector)e.getFrames()).begin());
					
					FAPFrame temp_frame = lastframe;

/* Per Radek: visto che hai detto che non funziona ho commentato il tuo codice, altrimenti il mio non funziona più
					//RAISING
					if ((temp_frame.GetFAP(31)>0)&&
						(temp_frame.GetFAP(32)>0)&&
						(temp_frame.GetFAP(33)>0)&&
						(temp_frame.GetFAP(34)>0)&&
						(temp_frame.GetFAP(35)>0)&&
						(temp_frame.GetFAP(36)>0)&&
						(temp_frame.GetFAP(33)>temp_frame.GetFAP(31))&&
						(temp_frame.GetFAP(34)>temp_frame.GetFAP(32))&&
						(temp_frame.GetFAP(33)>temp_frame.GetFAP(35))&&
						(temp_frame.GetFAP(34)>temp_frame.GetFAP(36))						
						){
							(e.getChannels())->eyebrows=RAISING;

					}

					//UP_DOWN -> like in sadness
					if ((temp_frame.GetFAP(31)>0)&&
						(temp_frame.GetFAP(32)>0)&&
						(temp_frame.GetFAP(33)>0)&&
						(temp_frame.GetFAP(34)>0)&&
						(temp_frame.GetFAP(35)>0)&&
						(temp_frame.GetFAP(36)>0)&&
						(temp_frame.GetFAP(37)>0)&&
						(temp_frame.GetFAP(38)>0)				
						){
							(e.getChannels())->eyebrows=UP_DOWN;
					}

					//FROWN -> 
					if ((temp_frame.GetFAP(31)<0)&&
						(temp_frame.GetFAP(32)<0)&&
						(temp_frame.GetFAP(33)<0)&&
						(temp_frame.GetFAP(34)<0)&&
						(temp_frame.GetFAP(35)<0)&&
						(temp_frame.GetFAP(36)<0)&&
						(temp_frame.GetFAP(37)>0)&&
						(temp_frame.GetFAP(38)>0)				
						){
							(e.getChannels())->eyebrows=FROWN;

					}
					//LOOK_AT
					if ((abs(temp_frame.GetFAP(23))<=3000)&&
						(abs(temp_frame.GetFAP(24))<=3000)&&
						(abs(temp_frame.GetFAP(25))<=3000)&&
						(abs(temp_frame.GetFAP(26))<=3000)
						){
							(e.getChannels())->gaze=LOOK_AT;
					}
					//LOOK_AWAY
					if ((abs(temp_frame.GetFAP(23))>3000)&&
						(abs(temp_frame.GetFAP(24))>3000)&&
						(abs(temp_frame.GetFAP(25))>3000)&&
						(abs(temp_frame.GetFAP(26))>3000)
						){
							(e.getChannels())->gaze=LOOK_AWAY;

					}
					//LIP_CORNER_UP
					if ((temp_frame.GetFAP(59)>0)&&
						(temp_frame.GetFAP(60)>0)
						){
							(e.getChannels())->mouth=LIP_CORNER_UP;

					}
					//LIP_CORNER_DOWN
					if ((temp_frame.GetFAP(59)<0)&&
						(temp_frame.GetFAP(60)<0)
						){
							(e.getChannels())->mouth=LIP_CORNER_DOWN;

					}
					//TENSE
					if ((temp_frame.GetFAP(51)>0)&&
						(temp_frame.GetFAP(52)>0)&&
						(temp_frame.GetFAP(55)>0)&&
						(temp_frame.GetFAP(56)>0)&&
						(temp_frame.GetFAP(57)>0)&&
						(temp_frame.GetFAP(58)>0)){
							(e.getChannels())->mouth=TENSE;

					}
					//ASIDE
					if (abs((temp_frame.GetFAP(50))>1000)){
							(e.getChannels())->head_direction=ASIDE;

					}
					//LOOK_DOWN
					if ((temp_frame.GetFAP(48)<-3000)){
							(e.getChannels())->head_direction=LOOK_DOWN;

					}
					//LOOK_UP
					if (temp_frame.GetFAP(48)>3000){
							(e.getChannels())->head_direction=LOOK_UP;

					}
					//HEAD_NOD
					if (abs((temp_frame.GetFAP(48))<3000)&&
							((temp_frame.GetFAP(48)>1000)||
							(temp_frame.GetFAP(48)<-1000))
					){
							(e.getChannels())->head_movement=HEAD_NOD;
					}
					
					//HEAD_SHAKE
					if (abs(temp_frame.GetFAP(49))>1000){
							(e.getChannels())->head_movement=HEAD_SHAKE;
					}
					*/
					
					
					children=((*iter)->child).begin();
					while(children!=(*iter)->child.end())
					{
						if((*children)->name=="channel")
						{
							if((*children)->GetAttribute("name")=="raising")
								(e.getChannels())->eyebrows=RAISING;
							else
							if((*children)->GetAttribute("name")=="updown")
								(e.getChannels())->eyebrows=UP_DOWN;
							else
							if((*children)->GetAttribute("name")=="frown")
								(e.getChannels())->eyebrows=FROWN;
							else
							if((*children)->GetAttribute("name")=="look_at")
								(e.getChannels())->gaze=LOOK_AT;
							else
							if((*children)->GetAttribute("name")=="look_away")
								(e.getChannels())->gaze=LOOK_AWAY;
							else
							if((*children)->GetAttribute("name")=="lip_corner_up")
								(e.getChannels())->mouth=LIP_CORNER_UP;
							else
							if((*children)->GetAttribute("name")=="lip_corner_down")
								(e.getChannels())->mouth=LIP_CORNER_DOWN;
							else
							if((*children)->GetAttribute("name")=="tense")
								(e.getChannels())->mouth=TENSE;
							else
							if((*children)->GetAttribute("name")=="head_aside")
								(e.getChannels())->head_direction=ASIDE;
							else
							if((*children)->GetAttribute("name")=="head_down")
								(e.getChannels())->head_direction=LOOK_DOWN;
							else
							if((*children)->GetAttribute("name")=="head_up")
								(e.getChannels())->head_direction=LOOK_UP;
							else
							if((*children)->GetAttribute("name")=="head_nod")
								(e.getChannels())->head_movement=HEAD_NOD;
							else
							if((*children)->GetAttribute("name")=="head_shake")
								(e.getChannels())->head_movement=HEAD_SHAKE;
							else
								printf("facelibrary parser: unknown channel %s for expression %s=%s\n",(*children)->GetAttribute("name").c_str(),e.getClassname().c_str(),e.getInstance().c_str());
						}
						children++;
					}


					AddFaceExpression(e);
					//e.Print();
				}
				else
				{
					fprintf(data_log,"error reading facial expressions from xml tree\n");
					return 0;
				}
			}
	return 1;
}




int FaceExpressionDictionary::BuildFAPFrame(XMLGenericTree *t, FAPFrame &f,float multiplied)
{
	float mult;
	std::list<XMLGenericTree*>::iterator iter;
	if(!t->child.empty())
		for(iter=t->child.begin();iter!=t->child.end();iter++)
		{
			mult=multiplied;
			if((*iter)->name=="fap")
			{
				float num=atof((*iter)->GetAttribute("num").c_str());
				float value=atof((*iter)->GetAttribute("value").c_str())*mult;
				f.SetFAP(num,value);
			}
			if((*iter)->name=="eyesdirection")
			{
				f.eyesdir.active=true;
				float h=atof((*iter)->GetAttribute("h").c_str());
				float v=atof((*iter)->GetAttribute("v").c_str());
				f.eyesdir.h=h*mult;
				f.eyesdir.v=v*mult;
				f.SetFAP(23,h/180.0f*3.14*100000);
				f.SetFAP(25,v/180.0f*3.14*100000);
				f.SetFAP(24,h/180.0f*3.14*100000);
				f.SetFAP(26,v/180.0f*3.14*100000);
				/*
				if(h==0)
				{
					f.FAPs[23].active=false;
					f.FAPs[24].active=false;
				}
				if(v==0)
				{
					f.FAPs[25].active=false;
					f.FAPs[26].active=false;
				}
				*/
			}
			if((*iter)->name=="headdirection")
			{
				f.headdir.active=true;
				float h=atof((*iter)->GetAttribute("h").c_str())*mult;
				float v=atof((*iter)->GetAttribute("v").c_str())*mult;
				float t=atof((*iter)->GetAttribute("t").c_str())*mult;
				f.headdir.h=h*mult;
				f.headdir.v=v*mult;
				f.headdir.t=t*mult;
				f.SetFAP(48,v/180.0f*3.14*100000);
				f.SetFAP(49,h/180.0f*3.14*100000);
				f.SetFAP(50,t/180.0f*3.14*100000);
				
				if(v==0)
					f.FAPs[48].active=false;
				if(h==0)
					f.FAPs[49].active=false;
				if(t==0)
					f.FAPs[50].active=false;
					
			}
			if((*iter)->name=="headmovement")
			{
				f.headmov.active=true;
				std::string type=(*iter)->GetAttribute("type");
				float amplitude=atof((*iter)->GetAttribute("amplitude").c_str());
				float period=atof((*iter)->GetAttribute("period").c_str());
				f.headmov.type=type;
				f.headmov.amplitude=amplitude*mult;
				f.headmov.period=period;
			}
			
			if((*iter)->name=="action")
			{	
				XMLGenericTree *res;
				float m=atof((*iter)->GetAttribute("multiplied").c_str());
				if(m!=0)
					mult=mult*m;
				else
					mult=1.0f;
				res=xmltree->FindNodeCalled("declaration");
				if(res!=0)
				{
					std::list<XMLGenericTree*>::iterator act_iter;
					if(!res->child.empty())
					{
						bool found=false;
						for(act_iter=res->child.begin();act_iter!=res->child.end();act_iter++)
						{
							//printf("vedo se %s è uguale a %s\n",GetAttribute((*act_iter),"name").c_str(),GetAttribute((*iter),"name").c_str());
							if((*act_iter)->GetAttribute("name")==(*iter)->GetAttribute("name"))
							{
								if(BuildFAPFrame(*act_iter,f,mult)==0)
									return 0;
								found=true;
								break;
							}
						}
						if(found==false)
						{
							fprintf(data_log,"error, action not defined %s\n",(*iter)->GetAttribute("name").c_str());
							return 0;
						}
					}
				}
			}
		}
		return 1;
}


void FaceExpressionDictionary::AddFaceExpression(FaceExpression &e)
{
	FExpressions.push_back(e);
	std::list<std::string>::iterator iter;
	bool found=false;
	if(!classes.empty())
		for(iter=classes.begin();iter!=classes.end();iter++)
		{
			if(strcmp((*iter).c_str(),e.getClassname().c_str())==0)
			{
				found=true;
				break;
			}
		}
	if(found==false)
	{
		std::string s=e.getClassname();
		classes.push_back(s);
	}
	
}

FaceExpression* FaceExpressionDictionary::GetExpression(std::string c, std::string i)
{
	bool found=false;
	std::list<FaceExpression>::iterator iter;

	iter=FExpressions.begin();
	
	while((found==false)&&(iter!=FExpressions.end()))
	{
		if(((*iter).getClassname()==c)&&((*iter).getInstance()==i))
		{
			found=true;
			break;
		}
		iter++;
	}

	if(found==true)
		return &(*iter);
	return 0;
}

void FaceExpressionDictionary::Mix2FAPFrames(FAPFrame &result,FAPFrame* f1,float factor1,FAPFrame* f2,float factor2)
{
	for(int i=1;i<NUMBER_OF_FAPS;i++)
	{
		//FAP is active in both the frames
		if((f1->FAPs[i].active)&&(f2->FAPs[i].active))
		{
			if((factor1!=0)&&(factor2!=0))
			{
				//sign of the FAP is discordant
				if(((f1->FAPs[i].value<0)&&(f2->FAPs[i].value>0))||((f1->FAPs[i].value>0)&&(f2->FAPs[i].value<0)))
					result.FAPs[i].active=0;
				else
				{
					//only one of the two is zero
					if((f1->FAPs[i].value==0)&&(f2->FAPs[i].value!=0))
						result.SetFAP(i,f2->FAPs[i].value*factor2);
					else
					if((f1->FAPs[i].value!=0)&&(f2->FAPs[i].value==0))
						result.SetFAP(i,f1->FAPs[i].value*factor1);
					else
						//they have same sign
						result.SetFAP(i,(f1->FAPs[i].value*factor1+f2->FAPs[i].value*factor2)/2);
				}
			}
			else
			{
				if(factor1!=0)
					result.SetFAP(i,f1->FAPs[i].value*factor1);
				if(factor2!=0)
					result.SetFAP(i,f2->FAPs[i].value*factor2);
			}
		}
		else
		{
			if((f1->FAPs[i].active)&&(factor1!=0))
				result.SetFAP(i,f1->FAPs[i].value*factor1);
			if((f2->FAPs[i].active)&&(factor2!=0))
				result.SetFAP(i,f2->FAPs[i].value*factor2);
		}
	}
}

void FaceExpressionDictionary::Mix3FAPFrames(FAPFrame &result,FAPFrame* f1,float factor1,FAPFrame* f2,float factor2,FAPFrame* f3,float factor3)
{
	FAPFrame result1;
	this->Mix2FAPFrames(result1,f1,factor1,f2,factor2);
	this->Mix2FAPFrames(result,&result1,1,f3,factor3);
}

void FaceExpressionDictionary::stringToLower(std::string &s)
{
	for(int i=0;i<s.length();i++)
		s[i]=tolower(s[i]);
}

void FaceExpressionDictionary::stringToLower(char *s)
{
	for(int i=0;i<strlen(s);i++)
		s[i]=tolower(s[i]);
}