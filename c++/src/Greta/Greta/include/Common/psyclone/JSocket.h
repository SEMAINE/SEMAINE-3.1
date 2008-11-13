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


// JSocket.h: interface for the JSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKET_H__C0DFE1C7_F17D_4EA0_A890_F0C980247837__INCLUDED_)
#define AFX_SOCKET_H__C0DFE1C7_F17D_4EA0_A890_F0C980247837__INCLUDED_

#include "Object.h"
#include "JString.h"
#include "JTime.h"
#include "JThread.h"
#include "TCPLocation.h"
#include "Debug.h"
#include "TimeSeries.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define READTIMEOUT 10
#ifndef _WIN32_WCE
	#ifndef CYGWIN
		#define SD_BOTH 2
	#endif
#endif

#ifdef WIN32
	#define EINPROGRESS WSAEWOULDBLOCK
	// For best performance, BUFFERSIZE should be smaller
	// than SENDBUFFERSIZE
	#define BUFFERSIZE 10000
	#define SENDBUFFERSIZE 80000
//	#include <Iptypes.h>
	#if defined(CYGWIN)
		#include <windows.h>
	#endif
	#include <Iphlpapi.h>

#else
	// For best performance, BUFFERSIZE should be smaller
	// than SENDBUFFERSIZE
	#define BUFFERSIZE 10000
	#define SENDBUFFERSIZE 80000
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <net/if.h>
	#include <ifaddrs.h>

	#define NIPQUAD(addr) ((unsigned char *)&addr)[0], \
		((unsigned char *)&addr)[1], \
		((unsigned char *)&addr)[2], \
		((unsigned char *)&addr)[3]

	typedef int SOCKET;
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define WSAEWOULDBLOCK EAGAIN
	#define WSAEMSGSIZE 11
	// #define WSAECONNRESET 107 // 10054
	#define WSAENOTCONN 10057

/*	typedef struct in_addr {
		union {
			struct{
				unsigned char s_b1,
				s_b2,
				s_b3,
				s_b4;
			} S_un_b;
			struct {
				unsigned short s_w1,
				s_w2;
			} S_un_w;
			unsigned long S_addr;
		} S_un;
	} IN_ADDR;

	struct sockaddr_in {
			short   sin_family;
			u_short sin_port;
			struct  in_addr sin_addr;
			char    sin_zero[8];
	} SOCKADDR_IN;
*/
#endif // WIN32


namespace cmlabs {


//! Generic Socket class
/*!
  A Socket is one end of a network connection, to have communication
  you need a Socket in each end, connected to each other.

  When you create a Socket object you provide it with an address
  which can be blank, a port number and a protocol type. For a listener
  you need the address to be blank and then ask it to listen(). For a
  regular connection you say connect() and it will try to connect to a
  listener on the remote machine.
*/
class JSocket : public Object  
{
public:

	static bool isIPAddressPrivate(JString addr);

	//! Constructor
	/*!
	  Takes three parameters
      \param addr the ip address, ignored for listeners.
      \param portnumber the ip port, always needed.
      \param protocolname the protocol, default = tcp.
	*/
	JSocket(JString addr, int portnumber, JString protocolname = "");

	//! Constructor
	/*!
	  Takes one parameters
      \param loc a TCPLocation
	*/
	JSocket(const TCPLocation& loc, JString protocolname = "");

	//! Constructor
	/*!
	  Takes one parameters
      \param new_sock the OS socket to base this instance on.
	*/
	JSocket(SOCKET new_sock);

	virtual ~JSocket();

	Object* clone() const {return NULL;}
	//! Bind to port
	bool setUpListening();

	//! Wait for incoming connection. Returns NULL or a Socket*
	JSocket* waitForConnection(long timeout);
	
	//! Tries to connect to remote Socket
	bool connectNow();

	//! Under the hood connectNow()
	bool connectNow(const IPNumber& ip, int port);

	//! Tries to disconnect from remote Socket, called by terminate
	bool disconnectNow();

	//! Terminates the local Socket, will auto disconnectNow
	bool terminate();

	int flush();


	//! Read a number of bytes from socket
	/*!
	  \param buffer is the char* buffer
      \param maxcount max number of bytes to read.
	  \param timeout timeout in ms
	*/
	int read(char* buffer, int maxcount, long timeout);

	//! Read a String from socket
	/*!
      \param count number of bytes to read - no less!
	  \param timeout timeout in ms to wait for count bytes
	*/
	JString readString(int count, long timeout);

	//! Read a line (ending with <CR>) from socket
	/*!
	  \param timeout timeout in ms
	*/
	JString readln(long timeout);

	//! Write a string to socket
	/*!
      \param str string to write.
	*/
	int write(JString str);

	//! Write a string to socket
	/*!
      \param ch chars to write.
	  \param len number of chars to write
	*/
	int write(char* ch, int len);

	//! Write a string and a <CR> to socket
	/*!
      \param str string to write.
	*/
	int writeln(JString str);

	void setNonBlockingMode();
	void setBlockingMode();

	JString getLocalIPAddress();
	Collection getLocalIPAddresses();
	Dictionary getLocalInterfaceAddresses();
	JString getLocalHostname();
	JString getRemoteHostname();
	JString getRemoteIPAddress();
	JString getIPAddressFromName(JString name);
	IPNumber getIPNumberFromName(JString name);
	JString getHostnameFromIPAddress(JString ipaddr);

	// Own variables
	TCPLocation location;
	JString address;
	JString localhostName;
	int port;
	JString protocol;
	JString lastErrorMessage;
//	JString buffer;

//	ipnumber = new unsigned char[4];
//	charBuffer = new char[BUFFERSIZE+8];
//	unsigned char ipnumber[4];
	IPNumber ipnumber;
	char charBuffer[BUFFERSIZE+8];

	int charBufferCount;
	JString stringBuffer;

	bool connected;
	bool lastErrorRecoverable;
	bool listening;

	// *********** Tests ***********

	//! Is the Socket connected?
	bool isConnected(int timeout = 0);

	//! Wait timeout ms for data availability...
	bool waitForReadability(int timeout);

	//! Wait timeout ms for data to be written...
	bool waitForWriteability(int timeout);

	//! Is the Socket connected?
	bool didConnectionGoThrough(int timeout);

	//! Is the Socket connected?
	bool isListening();

	//! Is last error recoverable?
	bool isLastErrorRecoverable();

	//! Get the last error message
	JString getLastErrorMessage();

	//! Get the last error number, OS specific
	int getLastOSErrorNumber();

	//! Close down the socket, OS specific
	void osCloseSocket(SOCKET thisSocket);


	bool isSocketValid();
	bool setSocketNotValid();



	SOCKET mySocket;
	bool createSocket();
	void dealWithSocketError(int wsaError, JString intro = "");

	int peekIntoStream();
	int peekIntoStream(char* buffer, int maxcount);
	int readIntoBuffer();
	int readChar(char* ch);
	int writeString(JString text);
	int writeBuffer(char* buf, int length);

	double getSendProgress();
	int currentUnsentBufferSize;
	int currentSentBufferSize;

	double getReceiveProgress();
	int currentUnreceivedBufferSize;
	int currentReceivedBufferSize;

	long getSendBufferSize();
	long getReadBufferSize();

	double ioCounts[2];
	double* getIOCounts();
	bool resetIOCounts();

	#ifdef WIN32
		// Windows Specific Methods and Variables
	#else
		// Linux Specific Methods and Variables
	#endif
	
	Collection localIPAddresses;
	JString localIPAddress;

	JMutex mutex;
	bool isTerminating;
};







class SocketTester : public JThread, public Object
{
public:
	SocketTester(int port);
	virtual ~SocketTester();
	Object* clone() const {return NULL;}

	void run();
	bool unitTest();

	bool shouldContinue;
	int portnum;
	JSocket* server;
};


class ConnectionProfile : public Object {
public:
	ConnectionProfile(JString name = "");
	ConnectionProfile(const ConnectionProfile& profile);
	ConnectionProfile(XMLNode* node);
	Object* clone() const;
	bool equals(const Object* obj) const;
	bool fromXML(XMLNode* node);
	JString toXML();
	JString print();
	JString toHTML();

	const ConnectionProfile& operator = (const ConnectionProfile& profile);

	bool setSpeed(long upbps, long downbps);
	bool addDataStat(long upbytes, long downbytes, const JTime& time = JTime());

	long getSpeedUpload();
	long getSpeedDownload();

	long getAvgUpload();
	long getAvgDownload();

	long getMaxAvgUpload();
	long getMaxAvgDownload();

	double getTotalUpload();
	double getTotalDownload();

	int maxConnectionHistory;
	bool isDirectlyConnected;

	JString getSpeedString();
	JString getRateString();
	JString getMaxRateString();
	JString getTotalString();

	JTime getLastActivity();

	JString name;

private:

	bool recalc();

	long speedUploadBps;
	long speedDownloadBps;

	long avgUploadBps;
	long avgDownloadBps;

	long maxAvgUploadBps;
	long maxAvgDownloadBps;

	double totalUpload;
	double totalDownload;

	TimeSeries uploadHistory;
	TimeSeries downloadHistory;
};

} // namespace cmlabs

#endif // !defined(AFX_SOCKET_H__C0DFE1C7_F17D_4EA0_A890_F0C980247837__INCLUDED_)
