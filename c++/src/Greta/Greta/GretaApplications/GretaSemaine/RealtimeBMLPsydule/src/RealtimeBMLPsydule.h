// GretaBMLRealtimePsydule.h: interface for the GretaBMLRealtimePsydule class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <time.h>

#include "Signal.h"
#include "MotorPlanner.h"
#include "Psydule.h"
#include "FapFrame.h"
#include "BapFrame.h"
#include "FaceEngine.h"
#include "IniManager.h"
#include "CentralClock.h"
#include "BML_AgentEngine.h"
#include "../../../include/common/definitions.h"


class RealtimeBMLPsydule : public Psydule
{
public:
	RealtimeBMLPsydule(JString name, JString host, int port,JString GretaName);
	virtual ~RealtimeBMLPsydule();

	int ReadMessage(Message *msg);
	int SendIdle();
	void MainLoop();
	void SendFAPsBAPs(int start_frame_time,int &lastfapframetime,int &lastbapframetime);
	//void GretaBMLRealtimePsydule::SendFAP();
	//void GretaBMLRealtimePsydule::SendBAP();

	std::list<JString> datatypes;
	std::vector<MMSystemSpace::Signal> signals;

	//Psydule *psydule;
	FaceEngine *faceengine;
	GestureSpace::MotorPlanner *mplanner;
	FAPFrameVector *fapframes;
	
	//for infinite movements we need another copy of the vector of frames
	FAPFrameVector *fapframeswithoutnoise;
	BAPFrameVector *bapframes;
	CentralClock pc;

	//std::vector<FAPFrame> *all_fapframes;
	std::vector<BAPFrame> *all_bapframes;

	BML_AgentEngine bmlengine;

	float FPS;

	bool NoEndFaceSignals;
	int last_fap_frame_time;
	int last_bap_frame_time;

	
	bool send_sound;
	std::string sound_file_name;
	void  findCurrentFrame(FAPFrameVector *frames);
	FAPFrame last_fap_frame;
	BAPFrame *last_bap_frame;

private:
	JString GretaName;

};

