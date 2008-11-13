// XMLGenericParser.h: interface for the XMLGenericParser class.
//
//////////////////////////////////////////////////////////////////////

//made by Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com

//this file defines a generic xml parser
//it is based on libxml2 (http://www.xmlsoft.org/)
#if !defined(AFX_XMLGENERICPARSER_H__5461A1E1_215D_11D9_9C92_E689F9D58C74__INCLUDED_)
#define AFX_XMLGENERICPARSER_H__5461A1E1_215D_11D9_9C92_E689F9D58C74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "XMLGenericTree.h"

class XMLGenericParser  
{
public:
	//creates and initializes the parser
	XMLGenericParser();
	virtual ~XMLGenericParser();
	//this function has to be called to get the parsed tree
	XMLGenericTree *ParseFile(std::string name);

protected:
	//this tree is the result given by the ParseFile method
	XMLGenericTree *result_tree;
	//this is called by ParseFile
	XMLGenericTree *XMLGenericParser::CreateTree(void *readerpointer);
};

#endif // !defined(AFX_XMLGENERICPARSER_H__5461A1E1_215D_11D9_9C92_E689F9D58C74__INCLUDED_)
