/*
 *  DummyAnalyser.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "semaineplayer.h"

#include <cstdlib>
#include <sstream>
#include <decaf/lang/System.h>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/SemaineML.h>
//#include <semaine/cms/message/SEMAINEMessage.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;

namespace semaine {
namespace components {
namespace player {

semaineplayer::semaineplayer(FramesManager *fm) throw(CMSException) :
	Component("GretaPlayer"),
		fm(fm)
{
	fap_bap_Receiver = new Receiver("semaine.data.synthesis.lowlevel.>");
	receivers.push_back(fap_bap_Receiver);
	
}

semaineplayer::~semaineplayer()
{
	delete fap_bap_Receiver;
}


void semaineplayer::react(SEMAINEMessage * m) throw(CMSException)
{
	fm->pc->SetTime(0);
	std::string received;
	std::string twolines;
	std::string label;
	received=m->getText();
	if(received[received.size()-1]!='\n')
		received+="\n";
	label=received.substr(0,received.find_first_of("\n"));
	received=received.substr(received.find_first_of("\n")+1);
	while(received!="")
	{
		twolines="";
		twolines=received.substr(0,received.find_first_of("\n"))+"\n";
		received=received.substr(received.find_first_of("\n")+1);
		twolines+=received.substr(0,received.find_first_of("\n"))+"\n";
		received=received.substr(received.find_first_of("\n")+1);
		FAPFrame ff;
		ff.ReadFromBuffer((char*)twolines.c_str());
		ff.use_at=ff.framenumber;
		ff.id=label;
		fm->AddFAPFrame(ff);

	}
	

}


} // namespace player
} // namespace components
} // namespace semaine
