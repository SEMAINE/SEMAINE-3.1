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

#pragma once
#include "xmlgenerictree.h"

class ERLTree :
	public XMLGenericTree
{
public:
	ERLTree(void);
	~ERLTree(void);
	/*! \brief gives out an APML tree
	*
	* this function allows to get an APMLTree after using the ParseFile
	* function of the GenericParser object, which returns a GenericTree
	* @see XMLGenericTree
	* @return the APML tree
	*/
	static ERLTree* ConvertGenericToERLTree(XMLGenericTree* generic);
	//removes spaces and newlines
	void RemoveSpacesAndNewlines(std::string &s);
	/*! \brief assign the phoneme file name to a particular text leaf
	*
	*/
	void AssignPhonemesFileNames(std::string &programpath);
	/*! \brief phonemes file name corresponding to the text tag
	*
	*/
	std::string PhonemesFileName;
	/*! \brief list of the child nodes
	*
	*/
	std::list<ERLTree*> child;
	/*! \brief pointer to the parent node
	*
	*/
	ERLTree* parent;
	/*! \brief duration of the tag
	*
	*/
	float duration;
	/*! \brief starting time of the tag
	*
	*/
	float start;
	/*! \brief class contructor
	*
	*/
	void MatchPhonemesTimingsForText(std::ifstream &globalphonemesfile,float &time);
	void addChild(ERLTree *e);
	void AssignTimings();
	void SaveToFile(std::string s,FILE *outputfile);
	void SaveWithAttributes(std::string filename);
private:
	static unsigned int erlphonemesfilecount;
	FILE *outputfile;
};
