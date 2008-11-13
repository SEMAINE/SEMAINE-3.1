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

// ComplexEmotionManager.h: interface for the ComplexEmotionManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "XMLGenericParser.h"
#include "Psydule.h"
#include "../../../include/Common/definitions.h"

class ComplexEmotionManager  
{
protected:
	XMLGenericTree *lib;

private:

	Psydule *psy;
	JString GretaName;

public:
	ComplexEmotionManager();
	virtual ~ComplexEmotionManager();

	int ReadLibrary(std::string complexemotionlibraryfilename);

	int **GetExpression(std::string classname,std::string instancename);

	int CreateInputForComplexExpressionModule(XMLGenericTree *tree,std::string complexexpressionfilename);
	void CreateInputBufferForComplexExpressionModule(XMLGenericTree *tree,std::string &outstring);

	int **RunComplexExpressionModule(std::string complexexpressionfilename,std::string buffer);

	bool registered;
	bool initialized;

};
