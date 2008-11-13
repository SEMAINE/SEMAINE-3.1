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


#include "Collections.h"
#include "Specs.h"
#include "Message.h"

namespace cmlabs {

#if !defined(_MESSAGEPROCESSOR_H_)
#define _MESSAGEPROCESSOR_H_

class MessageProcessor {
public:
	//! Is msg a Wakeup Message
	static bool isWakeupMessage(Message* msg);
	//! Get the trigger message from msg
	static Message* getTriggerMessage(Message* msg);
	//! Get the retrieved message collection from msg
	static ObjectCollection* getRetrievedMessages(Message* msg);
	//! Get the PhaseSpec from msg
	static PhaseSpec* getPhaseSpec(Message* msg);
	//! Get the PhaseSpec from msg
	static PhaseSpec* takePhaseSpec(Message* msg);

	//! Get an attached message from msg
	static Message* getMessage(Message* msg, int pos);
};

#endif // _MESSAGEPROCESSOR_H_
} // namespace cmlabs



