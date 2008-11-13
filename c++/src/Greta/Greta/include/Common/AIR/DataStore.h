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


// DataStore.h: interface for the DataStore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DATASTORE_H_)
#define _DATASTORE_H_

#include "Communicator.h"

namespace cmlabs {

class DataStoreEntry : public Object {
public:
	DataStoreEntry() { object = NULL; }
	~DataStoreEntry() { delete(object); }
	Object* clone() const {
		DataStoreEntry* entry = new DataStoreEntry();
		entry->timestamp = timestamp;
		entry->comment = comment;
		entry->object = object->clone();
		return entry;
	}

	JString printHTML();
	JString print() ;
	
	JTime timestamp;
	JString comment;
	Object* object;
};

class DataStore : public Communicator  
{
public:
	DataStore(const TCPLocation& cnsServer, JString storeFile = "");
	virtual ~DataStore();

	void run();

	Message* receiveMsg(Message* msg);

	HTMLPage* serveHTTPRequest(const JString& origin, HTTPRequest* request);
	Message* serveTelnetInput(const JString& id, const JString& origin, const JString& text);

private:

	bool readFromFile();
	bool saveToFile();

	Object* retrieveFirstAfter(JString from, JTime timestamp);
	Object* retrieveList(JString from);
	Object* retrieveLast(JString from);
	
	JString filename;
	ObjectDictionary entries;
};

} // namespace cmlabs

#endif // !defined(_DATASTORE_H_)
