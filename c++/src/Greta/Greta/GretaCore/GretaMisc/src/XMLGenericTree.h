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


/*! \brief generic xml attribute
*
*/
struct XMLAttribute{
	std::string name;
	std::string value;
};

/*! \brief xml generic tree
*
* this class describes a generic tree object
* which is the result of the parsing of an xml file
* @see APMLTree
* @author Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com
*/

class XMLGenericTree  
{
public:
	/*! \brief add an attribute
	*
	* this function adds an attribute to the node
	* @param n the name of the attribute
	* @param v the value of the attribute
	*/
	//this function adds an attribute to the node
	void addAttribute(std::string n,std::string v);
	/*! \brief prints the tree under this node, complete with all attributes
	*
	* @param s is a string that is prefixed to each printed line
	* and for each recursive iteration of the function the string
	* becomes longer so the final output has a tree appearence;
	* s should be initialized to empty string
	*/
	void PrintWithAttributes(std::string s);
	void SaveWithAttributes(std::string filename);
	/*! \brief adds a child to this node
	*
	* @param e is the tree to be added
	*/
	void addChild(XMLGenericTree *e);
	/*! \brief returns the node called with some name
	*
	* @param n the name of the wanted node
	* @return the pointer to the wanted node or 0 if not found
	*/
	XMLGenericTree* FindNodeCalled(std::string n);
	/*! \brief returns the attribute with the given name
	*
	* @param n the name of the wanted attribute
	* @return the pointer to the wanted attribute or 0 if not found
	*/
	std::string GetAttribute(std::string n);
	bool HasAttribute(std::string n);
	float GetAttributef(std::string n);
	void SetAttribute(std::string n, std::string value);
	/*! \brief returns number of child node of the node
	*
	*/
	int GetNumberOfChildren();
	/*! \brief node name
	*
	*/
	std::string name;
	/*! \brief node's list of the children
	*
	*/
	std::list<XMLGenericTree*> child;
	/*! \brief node's parent node
	*
	*/
	XMLGenericTree* parent;
	/*! \brief class constructor
	*
	*/
	XMLGenericTree();
	/*! \brief class destructor
	*
	*/
	virtual ~XMLGenericTree();
	/*! \brief node's value
	*
	*/
	std::string value;
	/*! \brief node's list of attributes
	*
	*/
	std::list<XMLAttribute> attributes;
	std::string ToString();
private:
	void SaveToFile(std::string s,FILE *outputfile);
	FILE *outputfile;
};

#endif // !defined(AFX_XMLGenericTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
