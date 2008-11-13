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


// Specs.h: interface for the Spec classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SPECS_H_)
#define _SPECS_H_

#include "DotString.h"
#include "Parameter.h"
#include "Collection.h"
#include "ObjectCollection.h"
#include "Dictionary.h"
#include "ObjectDictionary.h"
#include "XMLParser.h"
#include "TCPLocation.h"
#include "NetworkConnection.h"

namespace cmlabs {

class TriggerSpec : public Object {
public:
	TriggerSpec();
	TriggerSpec(const JString& xml, JString globalFrom = "");
	TriggerSpec(XMLNode* node, JString globalFrom = "");
	~TriggerSpec();
	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();
	JString toHTML();

//	bool isSystemTrigger();

	JString alias;
	JString from;
	DotString type;
	int delayMS;
};

class RetrieveSpec : public Object {
public:
	RetrieveSpec();
	RetrieveSpec(const JString& xml, JString globalFrom = "");
	RetrieveSpec(XMLNode* node, JString globalFrom = "");
	~RetrieveSpec();
	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();
	JString toHTML();

	static ObjectCollection* fromRetrievesXML(const JString& xml);
	static ObjectCollection* fromRetrievesXML(XMLNode* node);

	JString from;
	JString sender;
	JString id;
	DotString type;
	JString content;
	JString contentmatch;
	DotString beforeContext;
	DotString duringContext;
	DotString afterContext;

	JTime afterTime;
	JTime untilTime;

	int maxCount;
	int minCount;
	int latest;
	int lastMS;
};

class CrankSpec : public Object {
public:
	CrankSpec();
	CrankSpec(const JString& xml);
	CrankSpec(XMLNode* node);
	~CrankSpec();
	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();
	JString toHTML();

	bool continuous;
	JString name;
//	Dictionary* mediaStreams;
};

class PostSpec : public Object {
public:
	PostSpec();
	PostSpec(const JString& xml, JString globalTo = "");
	PostSpec(XMLNode* node, JString globalTo = "");
	~PostSpec();
	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();
	JString toHTML();

	JString alias;
	JString from;
	JString to;
	JString cc;
	JString phasechange;
	DotString type;
	JString content;
	JString language;
	double priority;
	bool useInReplyTo;
	bool noreply;
	Object* object;
};

class PhaseSpec : public Object {
public:
	PhaseSpec();
	PhaseSpec(const JString& xml);
	PhaseSpec(XMLNode* node);
	~PhaseSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();
	JString print();
	bool equals(const Object *o2);
	int compare(const Object *o2);
	bool isNewerThan(const PhaseSpec* spec);
	bool isOlderThan(const PhaseSpec* spec);

	JTime activationTime;
	JString name;
	JString pid;
	JString moduleName;
	JString context;
	bool allowSelfTriggering;
	JString lastTriggerName;
	ObjectDictionary* currentContexts;
	double priority;

	//! Contains TriggerSpecs
	ObjectCollection* triggers;

	//! Contains System TriggerSpecs
	// ObjectCollection* systriggers;

	//! Contains RetrieveSpecs
	ObjectCollection* retrieves;

	//! Contains CrankSpecs
	ObjectCollection* cranks;

	//! Contains PostSpecs
	ObjectCollection* posts;

	//! Contains StreamAccesses
	ObjectCollection* streams;

	bool containsGlobalTrigger();
};

class ContextSpec : public Object {
public:
	ContextSpec();
	ContextSpec(const JString& xml);
	ContextSpec(XMLNode* node);
	~ContextSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	bool reset();
	bool setCurrentContexts(ObjectDictionary* newContexts);
	bool isActive();
	ObjectCollection* getActivePhases();
	ObjectCollection* getAllPhases();
	bool switchPhase();
	bool calcCurrentPhases();

	JString name;
	JString moduleName;
	int currentPhaseNum;
	PhaseSpec* currentPhase;
	ObjectCollection* currentActivePhases;
	DotString ownContext;
	double priority;
	bool allowSelfTriggering;

	bool wasActive;
	//! Contains PhaseSpecs
	ObjectCollection* phases;
	//! Contains Current Contexts - do not delete!!!
	ObjectDictionary* currentContexts;
};


class ExecutableSpec;
class ModuleSpec : public Object {
public:
	ModuleSpec();
	ModuleSpec(const JString& xml);
	ModuleSpec(XMLNode* node);
	~ModuleSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();
	JString toHTMLCurrent();

	bool switchPhase();
	bool setCurrentContexts(ObjectDictionary* newContexts);

	bool isCurrentlyTriggeredBy(JString wb, JString type);
	bool hasCranks();

	ObjectCollection* getTriggersFor(JString wb, JString type);
	ObjectCollection* getActiveContexts();
	PhaseSpec* getFirstActivePhase();
	ObjectCollection* getActivePhases();
	ObjectCollection* getAllPhases();

	Collection getContextNames();

	bool resetCache();

	//! Each contain a ContextSpec
	ObjectDictionary* contexts;

	JString moduleName;
	JString description;
	JString type;
	JString remote;
	JString share;
	double priority;
	bool allowSelfTriggering;
	JString plugFunction;
	int verbose;

	// For caching of searches
	//! Cache of lookups of context+whiteboard+type
	ObjectDictionary* getTriggersCache;
	//! Cache of lookups of the "context" above
	JString currentContextsCacheString;

	//! Contains Current Contexts - do not delete!!!
	ObjectDictionary* currentContexts;
	ObjectDictionary* parameters;

	ExecutableSpec* exec;
};


class WhiteboardSpec : public Object {
public:
	WhiteboardSpec();
	WhiteboardSpec(const JString& xml);
	WhiteboardSpec(XMLNode* node);
	~WhiteboardSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	Collection getStreamNames();

	//! Each contain a StreamSpec
	ObjectDictionary* streamSpecs;

	//! Each contain a CatalogSpec
	ObjectDictionary* catalogSpecs;

	JString whiteboardName;
	JString description;
//	long maxCount;
	ObjectDictionary* parameters;
	JString remote;
	double priority;
	bool useLocalSystemInfo;
};

class StreamSpec : public Object {
public:
	StreamSpec();
	StreamSpec(const JString& xml);
	StreamSpec(XMLNode* node);
	~StreamSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	JString streamName;
	JString description;
	long maxSize;
	long maxCount;
	JString parent;
	JString remote;
	bool allowMultipleWriters;
};

class StreamAccess : public Object {
public:
	StreamAccess();
	StreamAccess(const JString& xml);
	StreamAccess(XMLNode* node);
	~StreamAccess();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	JString alias;
	JString source;
	JString mode;
};


class CatalogSpec : public Object {
public:
	CatalogSpec();
	CatalogSpec(const JString& xml);
	CatalogSpec(XMLNode* node);
	~CatalogSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	JString name;
	JString remote;
	JString description;
	JString parent;
	JString queryhandler;
	JString plug;
	ObjectDictionary parameters;
};

class ExecutableSpec : public Object {
public:
	ExecutableSpec();
	ExecutableSpec(const JString& xml);
	ExecutableSpec(XMLNode* node);
	~ExecutableSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	JString getMatchingCommandLine();
	JString getMatchingCommandLine(SysInfo* sysInfo);

	JString name;
	ObjectDictionary sysSpecs;
	Dictionary commandLines;
	bool consoleOutput;
};

class SatelliteSpec : public Object {
public:
	SatelliteSpec();
	SatelliteSpec(const JString& xml);
	SatelliteSpec(XMLNode* node);
	~SatelliteSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	TCPLocation getTCPLocation();

	JString name;
	JString host;
	int port;
};


class ActionSpec : public Object {
public:
	ActionSpec();
	ActionSpec(JString name, JString act, JString value, JString target = "", Object* obj = NULL, JString content = "");
	ActionSpec(const JString& xml);
	ActionSpec(XMLNode* node);
	~ActionSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();
	bool setParameter(JString name, JString value);
	JString getParameter(JString name);

	JString name;
	int delay;
	JString state;
	JString target;
	JString act;
	JString value;
	JString content;
	Object* object;
	Object* answer;
	Dictionary params;
};

class ActionPerformer : public Object {
public:
	virtual bool performAction(ActionSpec* action) = 0;
	virtual bool performAction(JString name, JString act, JString content = "", JString target = "", Object* object = NULL, JString value = "") = 0;
	virtual bool performActions(ObjectCollection* actions) = 0;
};

class ClientProfile : public Object {
public:
	ClientProfile();
	ClientProfile(const JString& xml);
	ClientProfile(XMLNode* node);
	~ClientProfile();

	Object* clone() const;
	bool equals(const Object* otherProfile) const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();
	JString print();

	JString getCapability(const JString& category, const JString& capability);
	bool setCapability(const JString& category, const JString& capability, const JString& value);
	bool hasCapability(const JString& category, const JString& capability);

	JString name;
	ObjectDictionary capabilities;
	ConnectionProfile connectionProfile;
	JString thisConvID;
	JString lastConvID;
};


class ServiceSpec : public Object {
public:
	ServiceSpec();
	ServiceSpec(const JString& xml);
	ServiceSpec(XMLNode* node);
	~ServiceSpec();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	JString toHTML();

	JString name;
	JString service;
	JString type;
	JString remote;
	int maxConv;
	int timeout;
	double priority;
	JString provider;
//	JString whiteboard;
	TCPLocation location;
	JString setup;

	ModuleSpec* moduleSpec;
};

} // namespace cmlabs


#endif // _SPECS_H_
