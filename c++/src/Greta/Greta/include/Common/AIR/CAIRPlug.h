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



#if !defined(_CAIRPLUG_H_)
#define _CAIRPLUG_H_

namespace cmlabs {


typedef int (* MessageCallback)(char* from, char* to, char* type, char* language, char* content, char* whiteboard);

int CreatePlug(char* name, char* host, int port, MessageCallback func);
int ConnectPlugToServer(char* name);
int RegisterTrigger(char* name, char* whiteboard, char* msgType);
// int ReceiveMessage(char* from, char* to, char* type, char* language, char* content);
int ClearAllTriggers(char* name, char* whiteboard);
int IsConnectedToServer(char* name);
int PingServer(char* name);
int OpenTwoWayConnectionTo(char* name, char* whiteboard);
int PostAMessage(char* name, char* whiteboard, char* messageType, char* language, char* content);
int TerminatePlug(char* name);

} // namespace cmlabs

#endif // _CAIRPLUG_H_



