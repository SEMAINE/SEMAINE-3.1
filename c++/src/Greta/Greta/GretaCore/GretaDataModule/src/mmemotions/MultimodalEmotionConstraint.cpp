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

// MultimodalEmotionConstraint.cpp: implementation of the MultimodalEmotionConstraint class.
//
//////////////////////////////////////////////////////////////////////

#include "MultimodalEmotionConstraint.h"


MultimodalEmotionConstraint::MultimodalEmotionConstraint (){
	signals = new std::vector<ConstraintSignal>;
	constraints = new std::vector<Constraint>;

}
MultimodalEmotionConstraint::~MultimodalEmotionConstraint ()
{
	//delete signals;
	//delete constraints;
}

std::string MultimodalEmotionConstraint::getName(){
	return name;
}
void MultimodalEmotionConstraint::setName(std::string name1){
	this->name=name1;
}
void MultimodalEmotionConstraint::addConstraint(Constraint  constraint1){
	constraints->push_back(constraint1);
}
std::vector<Constraint>* MultimodalEmotionConstraint::getConstraints(){
	return constraints;
}


int MultimodalEmotionConstraint::getIdOfSignal(std::string name){

int id=-1;

	//find id of the signal
	std::vector<ConstraintSignal>::iterator signal_iter;
	
	for(signal_iter=signals->begin();signal_iter!=signals->end();signal_iter++) {

		if (strcmp((*signal_iter).getName().c_str(),name.c_str())==0) 
			id=(*signal_iter).getId();
			
	}

return id;

}

std::vector<Constraint>* MultimodalEmotionConstraint::getConstraintsOfOneSignal(std::string name){

	int id=getIdOfSignal(name);

	//find relevant cons	
	std::vector<Constraint> *good_cons;
	good_cons = new std::vector<Constraint>;

	std::vector<Constraint>::iterator cons_iter;
	for(cons_iter=constraints->begin();cons_iter!=constraints->end();cons_iter++) {
		
		if (((*cons_iter).getArg1Id()==id)||((*cons_iter).getArg2Id()==id)) 
		
			good_cons->push_back(*cons_iter);
	}
	return  good_cons;
}

void MultimodalEmotionConstraint::addConstraintSignal(ConstraintSignal constraintsignal){
	signals->push_back(constraintsignal);
}

void MultimodalEmotionConstraint::addConstraint(Constraint *constraint) {
	constraints->push_back(*constraint);
}
	
std::vector<ConstraintSignal>* MultimodalEmotionConstraint::getConstraintSignals(){
	return signals;
}

ConstraintSignal *MultimodalEmotionConstraint::getConstraintSignal(std::string name)
{
	std::vector<ConstraintSignal>::iterator signal_iter;
	for(signal_iter=signals->begin();signal_iter!=signals->end();signal_iter++) 
	{
		if (strcmp((*signal_iter).getName().c_str(),name.c_str())==0) 
			return &((ConstraintSignal)(*signal_iter));
	} //end of for

	return NULL;
}

