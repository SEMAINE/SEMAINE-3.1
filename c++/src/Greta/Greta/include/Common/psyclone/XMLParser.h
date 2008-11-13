/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/


// XMLParser.h: interface for the XMLParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPARSER_H__CD17EC42_A9BE_450C_BB84_C94D55772665__INCLUDED_)
#define AFX_XMLPARSER_H__CD17EC42_A9BE_450C_BB84_C94D55772665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "standard.h"
// #include <fstream>
#include <stdio.h>
// #include "include/cc++/url.h"
#include "Debug.h"	// Added by ClassView
#include "JString.h"	// Added by ClassView
#include "Object.h"
#include "ObjectCollection.h"	// Added by ClassView
#include "XMLNode.h"
#include "JFile.h"

namespace cmlabs {

// class XMLParser : public Object, URLStream, ifstream
class XMLParser : public Object //, std::ifstream
{
public:
	bool parseURL(const JString& url);
	void setXmlUrl(const JString& url);
	XMLParser();
	XMLParser(char url[]);
	virtual ~XMLParser();
	Object* clone() const {return NULL;}

	void deleteAllNodes();

	bool parse();
	bool parse(const JString& s);
	bool parseXML(const JString& s);
	JString verifyXML(const JString& xml);
	JString getErrorString();

	// Convenient relay methods for xmlNode
	ObjectCollection getAllNodes(const JString& childPath) const;
	XMLNode* getNodeWithAttrValue(Collection& attrPath, const JString& attrValue) const;
	ObjectCollection* getChildTags();

	XMLNode* getChildNode(const JString& childPath) const;
	XMLNode* getChildNode(Collection& childPath) const;

	ObjectCollection getChildNodes(const JString& childPath) const;
	ObjectCollection getChildNodes(Collection& childPath) const;

	JString findAttr(const JString& attr) const;
//	JString findAttr(Collection& attrPath) const;
	Dictionary* getAttributes();

	JString asXML();
	JString asXMLWithCRs();

	bool isUTF();

	XMLNode* getRootNode() const;
	void setRootNode(XMLNode* node);

	bool unitTest();

	bool getXmlUrl() const;

	bool parseComments;

private:
	bool parseLine(const JString& s);
	bool addXMLAttrAndValue(const JString& attr, const JString& value);
	bool addXMLNode(const JString& s, int line, int col);
	bool addXMLCommentNode(const JString& s, int line, int col);
	bool addXMLDirectiveNode(const JString& s, int line, int col);

	JString trimLeadingWhite(char *s) const;
	void printNodeStack(char *s);
	void reset();

	void pushNode(XMLNode* node);
	XMLNode* popNode();

	int line;
	int tagDepth;
	JString currLine;
	JString xml;
	XMLNode* xmlNode;
	XMLNode* currNode;

	ObjectCollection nodeStack;

	JString buf;
	JString attr;
	JString tag;

	bool singleQuoteOpen;
	bool doubleQuoteOpen;
	bool attrTag;
	bool tagOpen;
	bool tagClose;
	bool commentOpen;
	bool isCloseComment(const JString& s) const;

	bool isXMLUTF;
	JString xmlUrl;

	JString errorString;
};

} // namespace cmlabs

#endif // !defined(AFX_XMLPARSER_H__CD17EC42_A9BE_450C_BB84_C94D55772665__INCLUDED_)
