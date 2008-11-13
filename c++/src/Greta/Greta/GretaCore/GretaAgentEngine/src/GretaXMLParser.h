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

// GretaXMLParser.h: interface for the GretaXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "APMLCommunicativeAct.h"
#include "BC_CommunicativeAct.h"
#include "FaceEngineGlobals.h"
#include <string>


/**
*
* class :GretaXMLParser
*
*/

class GretaXMLParser  
{
public:

/**
* contructor 
*
*/

	GretaXMLParser();

/**
 * this method 
 * 
 *
* @return 
* @param  filename
* @param  &root
* @param first_comm_act
* @param last_comm_act
*/

	int GretaXMLParser::ParseXMLFile(std::string filename,xmlNodePtr &root,CommunicativeAct **first_comm_act,CommunicativeAct **last_comm_act);

/**
* destructor 
*/

	virtual ~GretaXMLParser();
private:
	//! The Facial Display Dictionary 
	//fap_id_list *dictionary;

/**
 * this method 
 * 
 *
* @param  node
*/

	void GretaXMLParser::DFS(xmlNodePtr node);

/**
 * this method 
 * 
 *
* @param  node
*/

	void GretaXMLParser::CreateCommActsFromXML(xmlNodePtr node);

/**
 * this method 
 * 
 *
* @return 
* @param s1
*/

	char *GretaXMLParser::new_string(const char *s1);
	//char* GretaXMLParser::getFunction(xmlNodePtr node);
	//char* GretaXMLParser::class_exists(char *class_type);

/**
 * this method 
 * 
 *
* @param  error_code
* @param s1
* @param s2
*/

	void GretaXMLParser::ParseError(int error_code,char *s1,char *s2);

/**
 * this method 
 * 
 *
* @param comm_act
*/

	void GretaXMLParser::InsertCommAct(CommunicativeAct *comm_act);

/**
 * this method 
 * 
 *
* @param comm_act
* @param  node
*/

	void GretaXMLParser::BuildTextDep(CommunicativeAct *comm_act,xmlNodePtr node);

/**
 * this method 
 * 
 *
* @return 
* @param  node
*/

	int GretaXMLParser::blank_text(xmlNodePtr node);

/**
 * this method 
 * 
 *
* @param  target
*/

	void GretaXMLParser::InvertSlashes(std::string target);

/**
 * this method 
 * 
 *
* @param comm_act
*/

	void GretaXMLParser::InitExprPar(CommunicativeAct *comm_act);


/**
 * this method 
 * 
 *
* @param bc_comm_act
*/

	void GretaXMLParser::InsertBackChannelCommAct(BC_CommunicativeAct *bc_comm_act);

	//float GretaXMLParser::GetRand01();
	xmlNodePtr treeroot;
	xmlDocPtr doc;
	CommunicativeAct *first_comm_act;
	CommunicativeAct *last_comm_act;
	BC_CommunicativeAct *bc_first_comm_act;
	BC_CommunicativeAct *bc_last_comm_act;


	void* engine;
	int tmp_file_index;
	FILE *textfile;
	int rheme_or_theme;
};
