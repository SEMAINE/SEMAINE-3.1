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


// DMAModule.h: interface for the DMAModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DMAMODULE_H__80F05D17_6E01_41FD_9078_5DBB60D0AE14__INCLUDED_)
#define AFX_DMAMODULE_H__80F05D17_6E01_41FD_9078_5DBB60D0AE14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Component.h"
#include "Queue.h"
#include "BinaryBuffer.h"

namespace cmlabs {


//! Super class for all DMAModules
/*!
	Includes all basic DMAModule functionality
*/
class DMAModule :
	public Component  
{
public:
	DMAModule(JString name, TCPLocation cnsHost, BinaryBuffer* inBuffer, BinaryBuffer* outBuffer);
	virtual ~DMAModule();

	void run(); 

	virtual void interpretMessage(Message* msg) = 0;
	virtual Object* workOnObject(Object* object) = 0;

	bool postMessage(JString to, JString type);
	bool postMessage(JString to, JString type, JString content, JString language);

	JString statusText;

	// Values
	int waittimeMessages;
	bool getLastObject;

	int movementThreshold;
	int differenceThreshold;

protected:
	BinaryBuffer* inputBuffer;
	BinaryBuffer* outputBuffer;

	JString binaryObjectInterest;
	JTime lastTime;
	JTime thisTime;

};

} // namespace cmlabs

#endif // !defined(AFX_DMAMODULE_H__80F05D17_6E01_41FD_9078_5DBB60D0AE14__INCLUDED_)
