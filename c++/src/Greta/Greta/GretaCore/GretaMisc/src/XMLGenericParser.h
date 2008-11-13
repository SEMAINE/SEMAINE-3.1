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

// XMLGenericParser.h: interface for the XMLGenericParser class.
//
//////////////////////////////////////////////////////////////////////

//made by Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com

#if !defined(AFX_XMLGENERICPARSER_H__5461A1E1_215D_11D9_9C92_E689F9D58C74__INCLUDED_)
#define AFX_XMLGENERICPARSER_H__5461A1E1_215D_11D9_9C92_E689F9D58C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "XMLGenericTree.h"

/*! \brief generic xml parser
*
* this file defines a generic xml parser
* it is based on libxml2 (http://www.xmlsoft.org/)
* @see http://www.xmlsoft.org/
* @author Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com
*/

class XMLGenericParser  
{
public:
	/*! \brief class contructor
	*
	* creates and initializes the parser
	*/
	XMLGenericParser();
	/*! \brief class destructor
	* 
	*/
	virtual ~XMLGenericParser();
	/*! \brief parse an xml input file
	*
	* this function has to be called to get the parsed tree
	* @return the parsed tree
	*/
	XMLGenericTree *ParseFile(std::string name);

	XMLGenericTree *ParseFileWithXSD(std::string name,std::string xsdpath);

	XMLGenericTree *ParseBufferWithXSD(char *buffer,std::string xsdpath);

	XMLGenericTree *ParseBuffer(char *buffer);

	void SetValidating(bool v);
protected:
	//this tree is the result given by the ParseFile method
	XMLGenericTree *result_tree;
	//this is called by ParseFile
	XMLGenericTree *XMLGenericParser::CreateTree(void *readerpointer);
private:
	bool validating;
};

#endif // !defined(AFX_XMLGENERICPARSER_H__5461A1E1_215D_11D9_9C92_E689F9D58C74__INCLUDED_)
