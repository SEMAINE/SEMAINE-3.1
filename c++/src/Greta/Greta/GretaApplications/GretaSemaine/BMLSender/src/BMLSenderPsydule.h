// BMLSenderPsydule.h: interface for the BMLSenderPsydule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BMLSENDERPSYDULE_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_)
#define AFX_BMLSENDERPSYDULE_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include "Psydule.h"


struct emot{
	std::string name;
	int vote;
	int priority;
};


class BMLSenderPsydule : public Psydule
{
public:
	BMLSenderPsydule(JString name, JString host, int port);
	virtual ~BMLSenderPsydule();
	int ReadMessage(Message *triggerMessage);
	void MapParameters();
	float AccMin,AccMax,VelMin,VelMax,DIMin,DIMax,CIMin,CIMax;
	float Acc,Vel,DI,CI;
	float SPC,TMP,FLD,PWR;
	JString name,host;
	std::string emotionname;
	int port;

	FILE *logfile;
};

#endif // !defined(AFX_BMLSENDERPSYDULE_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_)
