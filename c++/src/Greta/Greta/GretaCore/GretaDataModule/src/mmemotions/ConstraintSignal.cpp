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

// ConstraintSignal.cpp: implementation of the ConstraintSignal class.
//
//////////////////////////////////////////////////////////////////////
#include "ConstraintSignal.h"

ConstraintSignal ConstraintSignal::clone()
{

	ConstraintSignal newone;

	newone.setId(this->getId());
	newone.setMax_duration(this->getMax_duration());
	newone.setMin_duration(this->getMin_duration());
	newone.setName(this->getName());
	newone.setOccurence(this->getOccurence());
	newone.setProbability_end(this->getProbability_end());
	newone.setProbability_start(this->getProbability_start());

	return newone;
}

ConstraintSignal::ConstraintSignal(){
	
	int id=0;
	std::string name="";
	
	float repetivity=0.0f;
	float probability_start=0.0f;
	float probability_end=0.0f;
	float occurence=0.0f;
	float max_duration=0.0f;
	float min_duration=0.0f;
}

ConstraintSignal::~ConstraintSignal(){}

	std::string ConstraintSignal::getName(){
		return this->name;
	}
	int ConstraintSignal::getId(){return this->id;}
	float ConstraintSignal::getRepetivity(){return this->repetivity;}
	float ConstraintSignal::getProbability_start(){return this->probability_start;}
	float ConstraintSignal::getProbability_end(){return this->probability_end;}
	float ConstraintSignal::getOccurence(){return this->occurence;}
	float ConstraintSignal::getMax_duration(){return this->max_duration;}
	float ConstraintSignal::getMin_duration(){return this->min_duration;}

	void ConstraintSignal::setName(std::string name1){this->name=name1;}
	void ConstraintSignal::setId(int id1){this->id=id1;}
	void ConstraintSignal::setRepetivity(float repetivity1){this->repetivity=repetivity1;}
	void ConstraintSignal::setProbability_start(float probability_start1){
		this->probability_start=probability_start1;
	}
	void ConstraintSignal::setProbability_end(float probability_end1){
		this->probability_end=probability_end1;
	}
	void ConstraintSignal::setOccurence(float occurence1){this->occurence=occurence1;}
	void ConstraintSignal::setMin_duration(float min_duration1){this->min_duration=min_duration1;}
	void ConstraintSignal::setMax_duration(float max_duration1){this->max_duration=max_duration1;}