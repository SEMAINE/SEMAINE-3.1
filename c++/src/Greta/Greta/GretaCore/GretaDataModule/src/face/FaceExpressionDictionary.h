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

// FaceExpressionDictionary.h: interface for the FaceExpressionDictionary class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable:4786)

#include "FaceExpression.h"
#include "XMLGenericTree.h"
#include <list>
#include <string>



class FaceExpressionDictionary  
{
public:
	std::list<std::string> classes;
	std::list<FaceExpression> FExpressions;


	FaceExpression* GetExpression(std::string c,std::string i);
	void AddFaceExpression(FaceExpression &e);	
	int Init();
	int Init(std::string facelibrarypath);
	FaceExpressionDictionary();
	virtual ~FaceExpressionDictionary();	

	/*! \brief mixes togheter 3 FAP frames using Greeks' algorithm
	*
	*/
	void Mix3FAPFrames(FAPFrame &result,FAPFrame* f1,float factor1,FAPFrame* f2,float factor2,FAPFrame* f3,float factor3);
	/*! \brief mixes togheter 2 FAP frames using Greeks' algorithm
	*
	*/
	void Mix2FAPFrames(FAPFrame &result,FAPFrame* f1,float factor1,FAPFrame* f2,float factor2);

private:

	XMLGenericTree *xmltree;

	int BuildFAPFrame(XMLGenericTree *t,FAPFrame &f,float mult);
	int StoreDictionary();
	void stringToLower(std::string &s);
	void stringToLower(char *s);
};
