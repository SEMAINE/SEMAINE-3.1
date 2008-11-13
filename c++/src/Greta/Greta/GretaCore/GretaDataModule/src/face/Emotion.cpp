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

// Emotion.cpp: implementation of the Emotion class.
//
//////////////////////////////////////////////////////////////////////

#include "Emotion.h"

std::string Emotion::getName()
{
	return this->name;
}

void Emotion::setName (std::string name1)
{
	this->name=name1;
}

float Emotion::getEvaluation()
{
	return this->evaluation;
}

void Emotion::setEvaluation(float evaluation1)
{
	this->evaluation=evaluation1;
}

float Emotion::getIntensity()
{
	return this->intensity;
}

void Emotion::setIntensity(float intensity1)
{
	this->intensity=intensity1;
}

Emotion ::Emotion(std::string name1,float evaluation1)
{
	this->name=name1;
	this->evaluation=evaluation1;
	this->intensity=1.0;
	this->type="felt";
}

Emotion::Emotion(std::string name1)
{
	this->name=name1; 
	this->evaluation=0;
	this->intensity=1.0;
	this->type="felt";
}

Emotion::Emotion()
{
	this->name=""; 
	this->evaluation=0;
	this->intensity=1.0;
	this->type="felt";
}

Emotion::~Emotion()
{
}


/**
* defines a type of emotion
* @param type a type
*/
void Emotion::setType(std::string type1)
{		    

	this->type=type1;

	for(unsigned int i=0;i<type.length();i++)
		type[i]=tolower(type[i]);	
}

/**
* returns a type of emotion
* @return type
*/

std::string Emotion::getType()
{
	return this->type;
}
