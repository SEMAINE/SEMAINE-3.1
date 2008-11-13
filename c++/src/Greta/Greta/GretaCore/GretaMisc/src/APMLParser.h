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

// APMLParser.h: interface for the APMLParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APMLPARSER_H__5B7B5962_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
#define AFX_APMLPARSER_H__5B7B5962_2060_11D9_9C92_EE4D16C4357B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "APMLTree.h"
#include "XMLGenericParser.h"
#include "IniManager.h"	// Added by ClassView

	/*! \brief APML parser
	*
	* this file defines an object which is an APML parser;
	* it is an extension of a generic parser object;
	* this object can be used to parse  files written in APML
	* and if timings calculation is enabled it calls the speech
	* synthesizer Festival and assigns the correct timings to
	* the leafs of the APML parsed tree
	* @see XMLGenericParser
	* @author Maurizio Mancini m.mancini@iut.univ-paris8.fr manmau@yahoo.com
	*/

class APMLParser : public XMLGenericParser
{
public:
	/*! \brief disables timings calculation through Festival
	*
	*/
	void DisableTimingsCalculation();
	/*! \brief enables timings calculation through Festival
	*
	*/
	void EnableTimingsCalculation();
	/*! \brief parses APML file
	*
	*  call this to parse your APML file
	*  @return the parsed tree
	*/
	APMLTree *APMLParser::ParseFile(std::string name);
	/*! \brief class contructor
	*
	*  called as object is created
	*/

	APMLParser();
	/*! \brief resulting tree
	*
	*  this tree is given as result by the ParseFile function
	*/
	APMLTree *result_tree;
	/*! \brief class destructor
	*
	*/
	int CalculateTimings();
	/*! \brief script file name
	*
	*  used only if Festival has to be called (timings enabled)
	*/
	std::string scriptfilename;
	/*! \brief xml file name
	*
	*  used only if Festival has to be called (timings enabled)
	*/
	std::string xmlfilename;
	/*! \brief phonemes file name
	*
	*  used only if Festival has to be called (timings enabled)
	*/
	std::string phonemesfilename;
	/*! \brief wave file name
	*
	*  used only if Festival has to be called (timings enabled)
	*/
	std::string wavfilename;
	virtual ~APMLParser();

private:
	
	/*! \brief executes Festival
	*
	*  this function is internally called only if timings are enabled
	*/
	int APMLParser::CallFestival();
	/*! \brief calculates text nodes timings
	*
	*  this function is internally called only if timings are enabled
	*/
	void APMLParser::CalculateTextNodesTimings(void);
	/*! \brief enavles/disables timings calculation
	*
	*  this flag is managed by EnableTimingsCalculation() and DisableTimingsCalculation()
	*  @see EnableTimingsCalculation DisableTimingsCalculation
	*/
	bool calculate_timings;
	/*! \brief creates proper file names
	*
	*  this function is internally called only if timings are enabled
	*  @see xmlfilename phonemesfilename wavfilename scriptfilename
	*/
	void APMLParser::MakeFilesNames(std::string s);
};

#endif // !defined(AFX_APMLPARSER_H__5B7B5962_2060_11D9_9C92_EE4D16C4357B__INCLUDED_)
