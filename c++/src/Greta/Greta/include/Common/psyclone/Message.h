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


#if !defined(_MESSAGE_)
#define _MESSAGE_

#include "Object.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "JString.h"
#include "Collections.h"
#include "JTime.h"
#include "DotString.h"
#include "XMLParser.h"

namespace cmlabs {

class Message;
class Reference : public Object {
public:
	//! to create empty Message
	Reference();
	//! to create from XML node
	Reference(XMLNode* node);
	//! to create from Message
	Reference(Message* msg);

	Object* clone() const;
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML(const JString& urlformat);
	JString toHTML();

	JString id;
	JString stored;
	JString from;
	JString to;
	JString cc;
	JString type;
	JTime postedTime;
	JTime receivedTime;
};

class Message : public Object {
 public:
	//! to create empty Message
	Message();
	//! to create from XML
	Message(const JString& xml);
	//! to create from XML node
	Message(XMLNode* node);
	//! to create from other message
	Message(const Message &msg);  
	//! Create from values
	Message(             
			const JString& from,
			const JString& to,
			const JString& type
	);
	//! Create from values with content and language
	Message(             
			const JString& from,
			const JString& to,
			const JString& type,
			const JString& content,
			const JString& language
	);
	//! Create from values with object
	Message(             
			const JString& from,
			const JString& to,
			const JString& type,
			Object* obj
	);
	~Message();

	bool init();
	bool unitTest();
	Object* clone() const;
	Message* shallowClone() const;
	Message* tinyClone() const;

	Message& operator=(const Message &msg);
	bool equals(const Object* o2) const;
	int compare(const Object* o2) const;


// ***************************************************************
// XML/HTML/Text Functions
// ***************************************************************

	//! Read XML node into this Message...
	bool fromXML(XMLNode* xmlNode);
	//! Convert Message to XML...
	JString toXML();          

	//! Nicely formatted HTML without <table></table>
	JString toHTMLTable(const JString& tags = "");

	//! Nicely formatted HTML for History field</table>
	JString getHistoryHTML(const JString& urlformat);

	//! Nicely formatted text print
	JString print();
	//! One line text print
	JString printShort();

// ***************************************************************
// Binary Functions
// ***************************************************************

	//! Get the size of binary data chunk, -1 for all chunks
	long getBinarySize(int chunk);
	//! Get the number of data chunks
	int getBinaryChunkCount();
	//! Write binary data to buffer at chunk number 'chunk'
	long toBinaryBuffer(int chunk, char* buffer, int maxlen);
	//! Set the data after separate reception
	bool fromBinaryBuffer(int chunk, char* buffer, long len);

// ***************************************************************
// Global Setters and Getters
// ***************************************************************

	//! Set a field in the Message
	bool set(const JString& name, const JString& value);
	//! Get a field in the Message
	JString get(const JString& name);

	//! Set an aux field in the Message
	bool setAux(const JString& name, const JString& value);
	//! Get an aux field in the Message
	JString getAux(const JString& name);

	//! Set a JTime field in the Message
	bool setTime(const JString& name, const JTime& t);
	//! Get a JTime field in the Message
	JTime getTime(const JString& name);

	//! Add a Message to the History field in the Message
	bool addReferenceToHistory(Message* msg, const JString& author);
	//! Add a Reference to the History field in the Message
	bool addReferenceToHistory(Reference* ref, const JString& author);
	//! Get the History collection in the Message
	ObjectDictionary* getAllHistory();
	//! Get the History collection in the Message from an Author
	ObjectCollection* getHistoryFromAuthor(const JString& author);
	//! Delete all History
	bool deleteHistory();


// ***************************************************************
// Interfaces
// ***************************************************************

	//! Set the attached Object for the Message
	bool setObject(Object* obj);
	//! Get the attached Object for the Message or NULL
	Object* getObject();
	//! Get the attached Object type (Class name) for the Message
	JString getObjectType();
	//! Delete the attached Object; NB: Will destroy object
	bool deleteObject();
	//! Takes away the attached object
	Object* takeObject();

	//! Compare type including star notation
	bool doesTypeMatch(const JString& match);

	// Getters

	//! Get a copy of ID
	JString getID();       
	//! Get a copy of in-reply-to
	Reference* getInReplyTo();
	//! Get a copy of from
	JString getFrom();        
	//! Get a copy of to
	JString getTo();          
	//! Get a copy of cc
	JString getCC();          
    //! Get a copy of type
	JString getType();		  
	//! Get a copy of content
	JString getContent();     
	//! Get a copy of origin
	JString getOrigin();	  
	//! Get the comment for the Message
	JString getComment();
	//! Get the Message priority
	double getPriority();		
	//! Get the Message priority
	double getTimeToLive();		
	//! Get Message language
	JString getLanguage();
	//! Get Message stored
	JString getStored();
	
	//! Get Message creation time
	JTime getTimestamp();
	//! Get Message posted time
	JTime getPostedTime();
	//! Get Message received time
	JTime getReceivedTime();

	// Setters

	//! Set Message ID, autogenerated if not set
	bool setID(const JString& id);             
	//! Set in-reply-to
	bool setInReplyTo(Message* msg);
	bool setInReplyTo(Reference* ref);
	//! Set from
	bool setFrom(const JString& frm);          
	//! Set to
	bool setTo(const JString& t);              
	//! Set cc
	bool setCC(const JString& cc);              
	//! Set type
	bool setType(const JString& t);            
	//! Set content and language
	bool setContent(const JString& cont, const JString& language);      
	//! Set content and language without parsing XML
	bool setContentNoParse(const JString& cont, const JString& language);      
	//! Set origin
	bool setOrigin(const JString& orig);		
	//! Set human readable comment field
	bool setComment(const JString& str);
	//! Set Message priority
	bool setPriority(double val);	
	//! Set Message timeToLive
	bool setTimeToLive(double val);	
	//! Set Message language
	bool setLanguage(const JString& language);
	//! Set Message stored
	bool setStored(const JString& stored);

	//! Set Message creation time
	bool setTimestamp(const JTime& t);
	//! Set Message posted time
	bool setPostedTime(const JTime& t);
	//! Set Message received time
	bool setReceivedTime(const JTime& t);

	//! Dictionary of generic text entries
	// Dictionary primDict;
	//! Dictionary of generic text entries
	Dictionary* auxDict;
	//! Dictionary of Timestamps
	ObjectDictionary* timeDict;
	//! Other Messages in the past, id = timestamp
	ObjectDictionary* history;

	Object* object;

	//! Which entries to print in HTML view
	Collection* htmlCol;


	JString from;
	JString to;
	JString type;
	JString id;
	JString cc;
	JString origin;
	JString comment;
	double priority;
	double timeToLive;
	JString stored;
	Reference* inReplyTo;
	JString isResponse;
	JString noreply;

	JTime postedTime;
	JTime receivedTime;
//	JTime postedTime;

	JString language;
	JString content;

	ObjectCollection* attachedMsgs;

};


} // namespace cmlabs

#include "TCPLocation.h"

namespace cmlabs {

class MessageSender {
public:
	//! Send Message to name, get Message back -- WILL DESTROY Message
	virtual Message* sendReceiveMsgTo(const JString& name, Message* msg, int timeout = MSGTIMEOUT) = 0;
	//! Send Message to name, confirm delivery -- WILL DESTROY Message
	virtual bool sendMsgTo(const JString& name, Message* msg, int timeout = MSGTIMEOUT) = 0;
	//! Send text to Logger.
	/*!
	  Adds a log entry to Logger. It will log compName, the text str and the type, which can
	  be used to filter when looking at the Logger later.
      \param str the text to be added to logger.
      \param type a text to filter by in the Logger
	*/
	virtual bool log(int level, char *format, ...) = 0;
	virtual bool setStatus(const JString& name, const JString& value) = 0;
	virtual JString getStatus(const JString& name) = 0;
	virtual TCPLocation resolve(const JString& name, bool forceCNSLookup = false) = 0;
};

} // namespace cmlabs

#endif

