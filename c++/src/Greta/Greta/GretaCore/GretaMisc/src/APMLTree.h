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

// APMLTree.h: interface for the APMLTree class.
//
//////////////////////////////////////////////////////////////////////

//made by Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com

#if !defined(AFX_APMLTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
#define AFX_APMLTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include "XMLGenericTree.h"

/*! \brief structure to hold the content of an attribute
	*
	*/
struct APMLAttribute{
	std::string name;
	std::string value;
};

	/*! \brief APML tree
	*
	* this class defines an object to store an APML tree
	* @see XMLGenericTree
	* @author Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com
	*/

class APMLTree : public XMLGenericTree
{
public:
	/*! \brief gives out an APML tree
	*
	* this function allows to get an APMLTree after using the ParseFile
	* function of the GenericParser object, which returns a GenericTree
	* @see XMLGenericTree
	* @return the APML tree
	*/
	static APMLTree* ConvertGenericToAPMLTree(XMLGenericTree* generic);
	/*! \brief assign the phoneme file name to a particular text leaf
	*
	*/
	void AssignPhonemesFileNames(std::string &programpath);
	/*! \brief calculates timing of the tree
	*
	*/
	void AssignTimings();
	/*! \brief duration of the tag
	*
	*/
	float duration;
	/*! \brief starting time of the tag
	*
	*/
	float start;
	/*! \brief adjust correct timings of the text leafs
	*
	* this function takes correct timings from the phonemes files (timings
	* in the file corresponding to the text tag sometimes are not correct)
	*/
	void MatchPhonemesTimingsForText(std::ifstream &globalphonemesfile,float &time);
	/*! \brief creates the script file for Festival
	*
	*/
	void WriteLinesToScriptFile(FILE *f);
	/*! \brief phonemes file name corresponding to the text tag
	*
	*/
	std::string PhonemesFileName;
	/*! \brief removes spaces and newlines before setting up the value of the node
	*
	*/
	void setValue(std::string s);
	/*! \brief prints the tree starting from this node
	*
	*/
	void PrintWithDurations(std::string s);
	/*! \brief list of the child nodes
	*
	*/
	std::list<APMLTree*> child;
	/*! \brief pointer to the parent node
	*
	*/
	APMLTree* parent;
	/*! \brief class contructor
	*
	*/
	APMLTree();
	/*! \brief class destructor
	*
	*/
	virtual ~APMLTree();
	/*! \brief list of the node's attributes
	*
	*/
	std::list<APMLAttribute> attributes;
	/*! \brief adds a new child to this node
	*
	*/
	void addChild(APMLTree *e);
private:
	//counter to create file names
	static unsigned int phonemesfilecount;
	//removes spaces and newlines
	void RemoveSpacesAndNewlines(std::string &s);
};

#endif // !defined(AFX_APMLTree_H__5B7B5963_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
