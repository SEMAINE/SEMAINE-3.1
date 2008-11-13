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


// Catalog.h: interface for the Catalog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CATALOG_H__317E41EC_E64D_4E7E_BBF7_15FFA37E4BA0__INCLUDED_)
#define AFX_CATALOG_H__317E41EC_E64D_4E7E_BBF7_15FFA37E4BA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Debug.h"
#include "Component.h"
#include "XMLParser.h"
#include "CatalogStorage.h"
#include "DynamicLoader.h"

typedef int (* DLLCatalogRun)(JString name, Component* component, ObjectRequestQueue* queries);
typedef int (* DLLCatalogQuery)(Message* query, ObjectDictionary* parameters, ObjectDictionary* answers);

//! Super class for all Catalogs
/*!
	Includes all basic Catalog functionality
*/
class Catalog :
	public Component  
{
public:
	Catalog(JString name, TCPLocation cnsHost);
	virtual ~Catalog();

	bool init();
	void run();

	Message* acceptMessage(Message* msg);
	DynamicLoader* loader;
	CatalogSpec* catalogSpec;

protected:

	ObjectRequestQueue receivedQueries;
};

int FileCatalogRun(JString name, Component* component, ObjectRequestQueue* queries);
int FileCatalogQuery(Message* query, ObjectDictionary* parameters, ObjectDictionary* answers);

#endif // !defined(AFX_CATALOG_H__317E41EC_E64D_4E7E_BBF7_15FFA37E4BA0__INCLUDED_)
