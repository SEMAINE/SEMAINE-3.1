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


// API.h: interface for the API class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_API_H__E5AACF74_0444_465A_B249_FA401893C217__INCLUDED_)
#define AFX_API_H__E5AACF74_0444_465A_B249_FA401893C217__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Debug.h"
#include "ObjectRequestQueue.h"
#include "ObjectCollection.h"
#include "JThread.h"

namespace cmlabs {


//! API service with Threading
/*!
This class should be inherited by any class wanting an API interface.
Is has a separate JThread and its run will query the inQueue for new
requests every timerMaintenance milliseconds, given as a parameter to
the constructor.

If a new request was detected this will be forwarded to the apiCall()
function which should return the reply to the request as a JString.

After having waited for a new request it will always call the
doMaintenance() function which can be programmed to do regular clean
up work and other tasks.
*/
class API : 
	public ObjectRequestQueue, 
	public JThread  
{
public:
	//! Constructor
	/*!
      \param timerMaintenanceMS cycle in ms to call doMaintenance()
	*/
//	API(long timerMaintenanceMS = 1000);

	API(JString nm = JString("AN API"), long timerMaintenanceMS =50);
	virtual ~API();

	//! Will accept requests and route them to apiCall()
	void run();

	//! Terminates the thread and resourses nicely
	bool shutdown();

	JString getName();

	JString name;

protected:	

	//! bool to determine when not to run anymore
	bool shouldContinue;

	//! JTime interval for running maintenance
	long timerMaintenance;

	//! When request is detected this method is called to reply - must be overwritten
	virtual Object* apiCall(Object* request);

	//! For super class regular Maintenance and clean up
	virtual void doMaintenance();

};

} // namespace cmlabs

#endif // !defined(AFX_API_H__E5AACF74_0444_465A_B249_FA401893C217__INCLUDED_)
