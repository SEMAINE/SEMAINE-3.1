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

// Ex[ression.cpp: implementation of the Expression class.
//
//////////////////////////////////////////////////////////////////////

//THIS CLASS WILL BE JOINTED WITH FaceExpression


#include "Expression.h"


Expression::Expression()
{
	time_min=0;
	time_max=0;
	name="";
	emotion="";
	profile="";
	intensity_min=0;
	intensity_max=1;

	//faps =(int*)malloc(sizeof(int)*69);
	//faps = (int*)malloc(sizeof(int)*69);

	//faps [69];
	for(int j=0;j<69;j++) faps[j]=0;

}

Expression::~Expression()
{	
	if(!faps==0) free(faps);
}

float Expression::getTimeMin()
{
	return this->time_min;
}
float Expression::getTimeMax(){return this->time_max;}
std::string Expression::getName(){return this->name;}
std::string Expression::getEmotionName(){return this->emotion;}
std::string Expression::getProfileName(){return this->profile;}
float Expression::getIntensityMin(){return this->intensity_min;}
float Expression::getIntensityMax(){return this->intensity_max;}
int* Expression::getFaps(){return this->faps;}
int Expression::getFapValue(int fap1){return this->faps[fap1];}


void Expression::setTimeMin(float timemin){this->time_min=timemin;}
void Expression::setTimeMax(float timemax){this->time_max=timemax;}
void Expression::setName(std::string label){this->name=label;}
void Expression::setEmotionName(std::string label){this->emotion=label;}
void Expression::setProfileName(std::string label){this->profile=label;}
void Expression::setIntensityMin(float intmin){this->intensity_min=intmin;}
void Expression::setIntensityMax(float intmax){this->intensity_max=intmax;}
void Expression::setFaps(int *faps1){
	for(int j=0;j<69;j++) this->faps[j]=faps1[j];}
void Expression::setFap(int fap1, int value1){this->faps[fap1]=value1;}



FaceExpression* Expression::getFaceExpression(){

	FaceExpression* new_expression = new FaceExpression();

	//name="";	
	new_expression->setInstance("affect");
	new_expression->setClassname(this->emotion);
	new_expression->setIntensity((this->intensity_max+this->intensity_min)/2);


	std::vector<FAPFrame> frames;
	FAPFrame new_frame;
	for(int i=0;i<69;i++) new_frame.SetFAP(i,faps[i],true);
	frames.push_back(new_frame);
	new_expression->SetFrames(frames);

	//FAPFrameVector animation;

	//set channels in new_expression??
	//channel *channels;

	return new_expression;	

}

//TO DO: check this method
void Expression::rescaleIntensity(float intensity)
{
if ((intensity>1.0f)||(intensity<0.0f)) return;
								
for(int i=0;i<69;i++) 
		this->faps[i]=(int) (this->faps[i]*intensity);				

				
}// end of rescaleIntensity