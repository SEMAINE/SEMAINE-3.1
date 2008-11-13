/***************************** License ************************************

  Copyright (c) 2005 by Communicative Machines
  http://www.cmlabs.com   All rights reserved.
  
  This library is released under a 
  Communicative Machines Public Binary Library (PBL) License version 1.0
  which should be included with the distribution that included this file.

  The library is free software in the sense that you can use and 
  redistribute it under the terms of the PBL as published by Communicative 
  Machines; http://www.cmlabs.com/licenses/pbl/

  Information about OpenAIR is available at
  http://www.mindmakers.org/openair

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**************************************************************************/


// Communicator.h: interface for the Communicator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMUNICATOR_H__ECF02601_8B39_45AB_9CF9_B21F62F5177B__INCLUDED_)
#define AFX_COMMUNICATOR_H__ECF02601_8B39_45AB_9CF9_B21F62F5177B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef AIRVERSION
	#include "airversion.h"
#endif


#include "Debug.h"
#include "types.h"
#include "TCPLocation.h"
#include "Message.h"
#include "Dictionary.h"
#include "ObjectDictionary.h"
#include "Parameter.h"
#include "UninstantiatedXMLObject.h"

#include "Network.h"
#include "JTime.h"
#include "TimerManager.h"
#include "XMLParser.h"
#include "InfoItem.h"

#include <stdarg.h>

namespace cmlabs {

#ifndef AIRVERSION
	#define AIRVERSION "Psyclone CMLabs 2007(c)Communicative Machines"
	#define BINVERSION "1.2dev"
	#define LICENSENAME "Communicative Machines Only License"
	#define LICENSEVERSION "1.0"
	#define LICENSETYPE "DEV"
	#define LICENSEKEY ""
	#define LICENSEPERIOD "Unlimited"
	#define CUSTOMERNAME "Communicative Machines"
	#define CUSTOMERSHORT "CMLabs"
	#define CUSTOMERADDRESS1 ""
	#define CUSTOMERADDRESS2 ""
	#define PRODUCTNAME ""
	#define PRODUCTID ""
	#define TEXTLINE1 "This software is for internal testing only and may not be used outside CMLabs"
	#define TEXTLINE2 ""
	#define TEXTLINE3 ""
	#define LICENSOR "Communicative Machines Incorporated"
	#define BUILDDATE __DATE__
	#define BUILDTIME __TIME__
	#define BUILDSYSTEM ""
#endif

class ComponentState : public Object
{
public:
	ComponentState(const JString& name) { compName = name; };
	virtual ~ComponentState() {};
	Object* clone() const {return NULL;}

	JString compName;
	PerfStat perfStat;
	Dictionary statusDictionary;
	ObjectTable recentMessages;
	ObjectDictionary parameters;
};


class Communicator;

//! Super class for TCP/IP and Threading
/*!
	Super class for all objects needing to send/receive Message via TCP/IP
	and that needs a run() method run by a separate thread
*/
class Communicator : public JThread, public TCPReceiver, public MessageSender, public Object
{
public:
	//! Constructor
	/*!
	  Takes two, three or four parameters
      \param name the name of the component.
      \param cnsAddr the machine name on which CNS resides.
      \param priority the priority of the JThread created, default = 0.
      \param shouldInit should we run the init(), default = true.
	*/
	Communicator(const JString& name, TCPLocation cnsAddr, int priority = 0);
	Communicator(const JString& name, const JString& host, int port, int priority = 0);

	//! Destructor
	virtual ~Communicator();
	Object* clone() const {return NULL;}

	//! Do initialization
	/*!
	  This will start up the Network and add this object as a TCPReceiver.
	  It will also create the cnsCache.
	*/
	virtual bool init();

	//! Do initialization
	/*!
	  This will start up the Network and add this object as a TCPReceiver.
	  It will also create the cnsCache. NoListen forces communication to
	  go via the CNS, so nobody will create their own connections to this
	  communicator.
	*/
	virtual bool initNoListen();

	//! Initialize object from a stored profile
	/*!
		The Communicator will initialize from the DataStore
		and try to continue running as if it was never shut down.
	*/
	virtual bool initFromDataStore();

	//! Send text to Logger.
	/*!
	  Adds a log entry to Logger. It will log compName, the text str and the type, which can
	  be used to filter when looking at the Logger later.
      \param str the text to be added to logger.
      \param type a text to filter by in the Logger
	*/
	virtual bool logString(int level, const JString& str);
	//! Send text to Logger.
	/*!
	  Adds a log entry to Logger. It will log compName, the text str and the type, which can
	  be used to filter when looking at the Logger later.
      \param str the text to be added to logger.
      \param type a text to filter by in the Logger
	*/
	virtual bool log(int level, char *format, ...);

	//! Terminates the thread and resourses nicely
	virtual bool shutdown();

// ************** JThread Methods ****************
	//! Main Run, do not overwrite, use run()
//	void Run();

	//! Overwritable run method
	/*!
	  This is the main function for this JThread, overwrite this to run anything
	  in your object.
	*/
	virtual void run();

	//! The Network callback function
	/*!
	  This will be called whenever this object receives a Message from another Component.
	  Should return a reply message, even if it is only an OK.
      \param msg the Message that was received via the Network.
      \return the return Message, could be just an OK or an actual reply
	*/
	virtual Message* receiveMsg(Message* msg);

	virtual HTMLPage* getHTMLFromComponent(const JString& comp, HTTPRequest* request, const JString& origin = "");
	virtual HTMLPage* serveHTTPRequest(const JString& origin, HTTPRequest* request);
	virtual Message* serveTelnetInput(const JString& id, const JString& origin, const JString& text);


// ************** TCPReceiver Methods ****************
	void tcpInput(const JString& str, NetworkConnection* con);
	Message* msgInput(Message* msg);
	JString printTCPInputStat();
	Message* netObjectReceive(Message* msg, NetworkConnection* con);
	JString getName();
	virtual bool connectNotify(const JString& name, NetworkConnection* con);
	virtual bool disconnectNotify(const JString& name, NetworkConnection* con);
	virtual bool primaryReconnectNotify();
	virtual bool primaryDisconnectNotify();
	bool addConnectionNotification(const JString& name);

// ************** XML Parser Methods ****************
/*	XMLNode* getRootNode(void);
	bool parseURL(JString xmlUrl);
	bool parseXML(JString xml);
	JString asXML(bool withCRs);

	// Convenient relay methods for xmlNode
	ObjectCollection getAllNodes(JString childPath);
	XMLNode* getNodeWithAttrValue(Collection& attrPath, JString attrValue);
	ObjectCollection getChildTags();

	XMLNode* getChildNode(JString childPath);
	XMLNode* getChildNode(Collection& childPath);

	ObjectCollection getChildNodes(JString childPath);
	ObjectCollection getChildNodes(Collection& childPath);

	JString findAttr(JString attr);
	JString findAttr(Collection& attrPath);
	Dictionary getAttributes();
*/
// ************** Own Methods ****************
	JString getLocalHostname();
	JString getLocalIPAddress();
	Collection getLocalIPAddresses();

	// Test to see if the CNS is there...
	bool isCNSAvailable();
	bool isConnectedTo(const JString& comp);
	bool ping(const JString& component);
	bool ping(const TCPLocation& loc);
	bool isSameExecutable(const JString& comp);
	bool isSameExecutable(const TCPLocation& loc);

	JString retrieveCNSOverview();
	Dictionary* retrieveStatusDictionaryFromComponent(const JString& comp);
	JString retrieveStatusFromComponent(const JString& comp);
	virtual Dictionary* generateStatusDictionary(const JString& comp);

// ************** Catalog Methods ****************

	InfoItem* queryCatalog(const JString& catalog, Message* query);

// ************** DataStore Methods ****************

	//! Store an object in DataStore NB: obj will be destroyed, give copy!
	bool storeData(JTime timestamp, Object* obj, const JString& comment);
	//! Get my list of stored objects in DataStore
	ObjectCollection* retrieveDataStoreList();
	//! Get my last stored object in DataStore
	Object* retrieveLastDataStoreObject();
	//! Get one of my stored object in DataStore
	Object* retrieveFistDataStoreObjectAfter(JTime timestamp);


// ************** High Level Msg ****************

	// Expect an answer back

	//! Send Message to loc, get Message back -- WILL DESTROY Message
	virtual Message* sendReceiveMsgTo(const TCPLocation& loc, Message* msg, int timeout = MSGTIMEOUT);
	//! Send Message to name, get Message back -- WILL DESTROY Message
	virtual Message* sendReceiveMsgTo(const JString& name, Message* msg, int timeout = MSGTIMEOUT);
	
	// Expect an OK back
	
	//! Send Message to name, confirm delivery -- WILL DESTROY Message
	virtual bool sendMsgTo(const JString& name, Message* msg, int timeout = MSGTIMEOUT);
	//! Send Message to loc, confirm delivery -- WILL DESTROY Message
	virtual bool sendMsgTo(const TCPLocation& loc, Message* msg, int timeout = MSGTIMEOUT);

	// Send via CNS
	//! Send Message to name via CNS, get Message back -- WILL DESTROY Message
	Message* sendReceiveMsgViaCNS(const JString& name, Message* msg, int timeout = MSGTIMEOUT);

	bool trainNetworkProfile(const JString& comp, ConnectionProfile* profile);
	bool trainNetworkProfile(const TCPLocation& loc, ConnectionProfile* profile);

	//! Binds a location with CNS and returns the modified location
	TCPLocation cnsBindAnotherName(const TCPLocation& loc);
	//! Retrieves an entry from the config file. If name = "", this means yourself, "all" is everything
	bool getGlobalConfiguration();
	JString getConfiguration(const JString& name = "");
	virtual bool parseConfiguration();

	TCPLocation resolve(const JString& name, bool forceCNSLookup = false);

	PerfStat* getPerfStat();
	ComponentState* compState;
	ObjectDictionary compStates;
	bool switchComponentState(const JString& name);
	virtual bool cnsUnbind();
	bool cnsUnbind(const JString& name);
	// PerfStat perfStat;

// ************** Parameter Methods ****************
/*	bool setParameterOn(JString rec, JString name, JString value);
	JString getParameterOn(JString rec, JString name);
	
	bool existParameter(JString name);
	JString getParameter(JString name);
	bool setParameter(JString name, JString value);
	bool incrementParameter(JString name, int steps = 1);
	bool decrementParameter(JString name, int steps = 1);
	bool resetParameter(JString name);
*/

	ObjectDictionary* getParameterSpecs(const JString& module);
	Parameter* getParameterSpec(const JString& module, const JString& parameter);
	JString getParameterString(const JString& module, const JString& parameter);
	int getParameterInteger(const JString& module, const JString& parameter);
	double getParameterDouble(const JString& module, const JString& parameter);
	JString getParameter(const JString& module, const JString& parameter);

	bool setParameterString(const JString& module, const JString& parameter, const JString& value);
	bool setParameterInteger(const JString& module, const JString& parameter, int value);
	bool setParameterDouble(const JString& module, const JString& parameter, double value);
	bool setParameter(const JString& module, const JString& parameter, const JString& value);
	bool increaseParameter(const JString& module, const JString& parameter, int steps);
	bool decreaseParameter(const JString& module, const JString& parameter, int steps);
	bool resetParameter(const JString& module, const JString& parameter);
	bool addParameterItem(const JString& module, const JString& parameter, const JString& value);
	bool removeParameterItem(const JString& module, const JString& parameter, const JString& value);

	JString getStateParameterNotify(const JString& state, const JString& name);
	Parameter* getStateParameter(const JString& state, const JString& name);
	bool addStateParameter(const JString& state, const JString& paramname, const JString& xml);
	bool hasStateParameter(const JString& state, const JString& name);
	JString getStateParameterString(const JString& state, const JString& param);
	int getStateParameterInteger(const JString& state, const JString& param);
	double getStateParameterDouble(const JString& state, const JString& param);
	bool setStateParameterString(const JString& state, const JString& param, const JString& value);
	bool setStateParameterInteger(const JString& state, const JString& param, int value);
	bool setStateParameterDouble(const JString& state, const JString& param, double value);
	bool resetStateParameter(const JString& state, const JString& param);
	bool increaseStateParameter(const JString& state, const JString& param, int steps);
	bool decreaseStateParameter(const JString& state, const JString& param, int steps);
	bool addStateParameterItem(const JString& state, const JString& param, const JString& value);
	bool removeStateParameterItem(const JString& state, const JString& param, const JString& value);
	Collection getStateParameterItems(const JString& state, const JString& param);

	Parameter* getParameter(const JString& name);
	bool addParameter(const JString& paramname, const JString& xml);
	bool hasParameter(const JString& name);
	JString getParameterString(const JString& param);
	int getParameterInteger(const JString& param);
	double getParameterDouble(const JString& param);
	bool setParameterString(const JString& param, const JString& value);
	bool setParameterInteger(const JString& param, int value);
	bool setParameterDouble(const JString& param, double value);
	bool resetParameter(const JString& param);
	bool increaseParameter(const JString& param, int steps);
	bool decreaseParameter(const JString& param, int steps);
	bool addParameterItem(const JString& param, const JString& value);
	bool removeParameterItem(const JString& param, const JString& value);
	Collection getParameterItems(const JString& param);

	//! Reading OS information...
	Dictionary getOSInfo();

	bool setPriority(double pri);
	double getPriority();

	Collection getNameAliases();
	bool addNameAlias(const JString& alias);

// ************** Own Variables ****************
	bool shouldContinue;
//	bool running;
	bool useDirectCommunication;
	bool verifyReceiverName;

	JString compName;
	Collection aliases;
	int ownPort;
	TCPLocation cnsLocation;
	TCPLocation ownLocation;
	JString localHostname;
	int verbose;
	double priority;

	NetworkConnection* owncon;

	static JString encryptString(const JString& str);
	static JString decryptString(const JString& str);

	JTime createTime;

	//! Setting a status text
	bool setStatus(const JString& name, const JString& value);

	//! Getting a status text value
	JString getStatus(const JString& name);

	// Now contained in ComponentState
	//! ObjectDictionary of parameters
	// ObjectDictionary* parameters;

protected:
	ObjectDictionary* cnsCache;

	//! Dictionary global strings
	Dictionary globalInfo;

	//! Allow only one input string to be processed at a time
	JMutex inputMutex;

	//! Thread statistics in running TCP inputs
	ThreadStat inputStat;

	//! Dictionary with arbitrary named values
//	Dictionary* statusDictionary;

	Network* network;
	TimerManager* timerManager;

// ************** XML Parser ****************

	//! Holds the component's XML initialization string
	JString configuration;
	XMLParser* configXML;
	JString description;

// ************** Msg Level TCP ****************

	bool listenOnPort;
	bool cnsBind();
	bool cnsRebindOnPort(int newport);

	TCPLocation cnsResolve(const JString& name);

	Message* runLowLevelCommand(const JString& cmd, Message* inMsg);

	bool addSelfManagedReceivingConnection(const JString& com);
	bool addSelfManagedReceivingConnection(const TCPLocation& loc);
	bool canConnectTo(const JString& comp);
	bool canConnectTo(const TCPLocation& loc);

// ************** Low Level TCP ****************

	NetworkConnection* makeTCPConnectionTo(const JString& addr, int port, const JString& name);
	NetworkConnection* makeTCPConnectionTo(const TCPLocation& loc);
	bool endTCPConnection(NetworkConnection* tcp);

	Collection notifyOnConnect;

// ************** Only for Debug ****************

	Collection debugTestComponents;

};

} // namespace cmlabs

#endif // !defined(AFX_COMMUNICATOR_H__ECF02601_8B39_45AB_9CF9_B21F62F5177B__INCLUDED_)
