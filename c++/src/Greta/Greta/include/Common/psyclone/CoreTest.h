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


// CoreTest.cpp : Defines the entry point for the console application.
//

#if !defined(_CORETEST_H_)
#define _CORETEST_H_

#include "standard.h"

#include "Queue.h"
#include "ObjectRequestQueue.h"
#include "XMLParser.h"
#include "Collections.h"
#include "Parameter.h"
#include "JSocket.h"
#include "TimerManager.h"
#include "Network.h"
#include "DynamicLoader.h"
#include "JFile.h"
#include "MediaServer.h"
#include "MediaConnection.h"
#include "OSCommand.h"

namespace cmlabs {

JString runTest(int c);
JString testText(Object* obj);
JString testTextThread(JThread* obj);
JString testTextNetwork(Network* network, int testport);
JString testTextSockets(SocketTester* obj);
JString testTextMediaServer(MediaServer* obj, int testport);

void runTests(int cnsport = 2000, JString testlib = "");
void test(Object* obj);
void runXMLParseTest();
void testSockets(SocketTester* obj);
void testNetwork(Network* network, int testport);
void testJThread(JThread* obj);
bool runSystemTest(int testport = 2000);
void testMediaServer(MediaServer* obj, int testport);

} // namespace cmlabs

#endif // _CORETEST_H_

