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


// Config.h: interface for the Config class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Config_H__865B2C7A_6423_4C1E_A11D_2D8AF78EC0F1__INCLUDED_)
#define AFX_Config_H__865B2C7A_6423_4C1E_A11D_2D8AF78EC0F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Communicator.h"
#include "JString.h"	// Added by ClassView

namespace cmlabs {

#define CURRENTPSYSPECVERSION 1.2
#define OLDESTCOMPATIBLEPSYSPECVERSION 1.2

class Config : public Communicator  
{
public:
	Config(JString name, JString filename, TCPLocation cnsloc);
	virtual ~Config();

	bool init();
	void run();
	Message* receiveMsg(Message* msg);
	Message* lookupEntry(Message* msg);

	JString getComponentXml(JString name);
	Dictionary getGlobalInfo();
	bool addToGlobalInfo(JString key, JString value);

	bool setConfigurationXML(JString xml);

private:
	JString dnaLocation;
	Dictionary* infoStore;
	JString staticConfigText;
	Dictionary globalInfo;
};

} // namespace cmlabs

#endif // !defined(AFX_Config_H__865B2C7A_6423_4C1E_A11D_2D8AF78EC0F1__INCLUDED_)
