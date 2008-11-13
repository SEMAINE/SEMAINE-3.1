// XMLGenericTree.h: interface for the XMLGenericTree class.
//
//////////////////////////////////////////////////////////////////////

//made by Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com

//this file describes a generic tree object
//which is the result of the parsing of an xml file

#if !defined(AFX_XMLGenericTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
#define AFX_XMLGenericTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include <list>
#include <iostream>
#include <fstream>

//generic attribute
struct XMLAttribute{
	std::string name;
	std::string value;
};

class XMLGenericTree  
{
public:
	//this function adds an attribute to the node
	void addAttribute(std::string n,std::string v);
	//prints the tree under this node
	void PrintWithAttributes(std::string s);
	//adds a child to this node
	void addChild(XMLGenericTree *e);
	//node name
	std::string name;
	//node's children
	std::list<XMLGenericTree*> child;
	//node's parent node
	XMLGenericTree* parent;
	//constructor
	XMLGenericTree();
	virtual ~XMLGenericTree();
	//node's value
	std::string value;
	//node's attributes
	std::list<XMLAttribute> attributes;
};

#endif // !defined(AFX_XMLGenericTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
