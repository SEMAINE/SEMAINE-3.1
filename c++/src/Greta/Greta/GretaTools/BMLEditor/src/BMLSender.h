// BMLSenderPsydule.h: interface for the BMLSenderPsydule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMLSENDER_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_)
#define AFX_BMLSENDER_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <fstream>
//#include <Windows.h>
#include <string>
#include "Communicator.h"
#include "Component.h"
#include "CppAIRPlug.h"

using namespace cmlabs;

class BMLSender : public CppAIRPlug
{
public:
	BMLSender(JString name, JString host, int port);
	virtual ~BMLSender();
	int Register();
	int PostFile(std::string BMLFileName);
	int PostString(std::string BML);
	int ReceiveMessage();
	JString name,host;
	std::string emotionname;
	int port;
};

#endif // !defined(AFX_BMLSENDERPSYDULE_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_)
