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

// Constraint.cpp: implementation of the Constraint class.
//
//////////////////////////////////////////////////////////////////////

#include "Constraint.h"

Constraint::Constraint()
{

	this->type="";

	this->arg1_id=-1;
	this->arg2_id=-1;

	this->arg1_type=-1;
	this->arg2_type=-1;

	this->lessthan=false;
	this->morethan=false;
	this->equal=false;

	this->value=0;
}

Constraint::~Constraint()
{
}

void Constraint::setConstraintType(std::string type1)
{
	this->type=type1;
}
	
std::string Constraint::getConstraintType()
{
	return this->type;
}

void Constraint::setConstraintRange(std::string range1)
{
	this->range=range1;
}
	
std::string Constraint::getConstraintRange()
{
	return this->range;
}

void Constraint::setArg1(int id1,int type1)
{
this->arg1_id=id1;
this->arg1_type=type1;
}

void Constraint::setArg2(int id1,int type1)
{
this->arg2_id=id1;
this->arg2_type=type1;
}

int Constraint::getArg1Id()
{
	return this->arg1_id;
}

int Constraint::getArg2Id()
{
	return this->arg2_id;
}
	
int Constraint::getArg1Type()
{
	return this->arg1_type;
}
int Constraint::getArg2Type()
{
	return this->arg2_type;
}

void Constraint::setLessThan()
{
	this->lessthan=true;
	this->morethan=false;
	this->equal=false;
}

void Constraint::setMoreThan()
{
	this->lessthan=false;
	this->morethan=true;
	this->equal=false;
}

void Constraint::setEqual()
{
	this->lessthan=false;
	this->morethan=false;
	this->equal=true;
}


bool Constraint::isLessThan()
{
	return this->lessthan;
}

bool Constraint::isMoreThan()
{
	return this->morethan;
}

bool Constraint::isEqual()
{
	return this->equal;
}

void Constraint::setValue(float value1)
{
	this->value=value1;
}

float Constraint::getValue()
{
	return this->value;
}
