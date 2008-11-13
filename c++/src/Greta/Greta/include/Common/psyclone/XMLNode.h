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


// XMLNode.h: interface for the XMLNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLNODE_H__DE034BFC_CE07_424E_8385_13977121E936__INCLUDED_)
#define AFX_XMLNODE_H__DE034BFC_CE07_424E_8385_13977121E936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "Dictionary.h"
#include "Collection.h"
#include "ObjectCollection.h"

#define	JSTRINGNULL	JString("")
#define	ISJSTRINGNULL(js)	(js == JSTRINGNULL)

namespace cmlabs {

class XMLNode  : public Object
{
public:
	XMLNode* getNodeWithAttrValue(Collection& attrPath, const JString& attrValue);
	XMLNode(const JString& s);
	Object* clone() const;

	void deleteAllChildNodes();

	JString asXMLChildrenOnly();
	JString asXMLWithCRs();
	JString toXML();
	JString toXMLChildrenOnly();
	JString toHTML();

	JString asXMLOpenTag();
	JString asXMLCloseTag();

	Collection getPath() const;

	XMLNode * getParent() const;
	void setParent(XMLNode* p);

	ObjectCollection* getChildTags();
	ObjectCollection getAllNodes(const JString& childPath);

	XMLNode* getFirstChildNode();
	XMLNode* getChildNode(const JString& childPath);
	XMLNode* getChildNode(Collection& childPath);

	ObjectCollection getChildNodes(const JString& childPath);
	ObjectCollection getChildNodes(Collection& childPath);

	JString findAttr(const JString& attr) const;
//	JString findAttr(Collection& attrPath) const;
	Dictionary* getAttributes();
	//! Checks for attribute attr in tag <tag attr="xxx" />
	bool hasAttribute(const JString& attr) const;
	//! Returns the attribute value in tag <tag attr="xxx" />
	JString getAttribute(const JString& attr) const;

	//! Returns text content, if content is XML returns "" (use getTextContent() then)
	JString getText() const;
	void setText(const JString& t);
	//! Returns text content even if content is XML format
	JString getTextContent();

	bool isEmpty() const;

	bool setComment(const JString& t);
	bool setPreComment(const JString& t);
	bool setPostComment(const JString& t);
	JString getComment() const;
	JString getPreComment() const;
	JString getPostComment() const;
	bool isCommentNode() const;

	bool setDirective(const JString& t);
	JString getDirective() const;
	bool isDirectiveNode() const;

	void			setTag(const JString& t);
	JString			getTag() const;

					XMLNode();
	virtual			~XMLNode();

	bool isUTF();

	// Adding attributes and child tags
	void			addAttribute(const JString& attr, const JString& attrValue);
	void			addChildTag(Object* childValue);

	// Formatting XML as JString
	JString			indent(int nestLevel);
	JString			asXML(int nestLevel, bool withCRs);
	JString			asXML();

	XMLNode*		parent;
	JString			tag;
	JString			text;
	// ####### expensive to create and destroy #########
	Dictionary*		attributes;
	ObjectCollection*	childTags;
	JString			preComment;
	JString			postComment;
	JString			directive;

	bool isXMLUTF;
	int lineno;
	int charno;
};

#define XMLNODE(n)  ((XMLNode*) n)
#define XMLNODENULL XMLNODE(NULL)

} // namespace cmlabs

#endif // !defined(AFX_XMLNODE_H__DE034BFC_CE07_424E_8385_13977121E936__INCLUDED_)
