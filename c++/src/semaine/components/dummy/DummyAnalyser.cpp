/*
 *  DummyAnalyser.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 19.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "DummyAnalyser.h"

#include <cstdlib>
#include <sstream>
#include <decaf/lang/System.h>

#include <semaine/util/XMLTool.h>
#include <semaine/datatypes/xml/EMMA.h>
#include <semaine/datatypes/xml/SemaineML.h>

using namespace semaine::util;
using namespace semaine::datatypes::xml;

namespace semaine {
namespace components {
namespace dummy {

DummyAnalyser::DummyAnalyser() throw(CMSException) :
	Component("DummyAnalyser"),
	userIsSpeaking(false)
{
	featureReceiver = new FeatureReceiver("semaine.data.analysis.>");
	receivers.push_back(featureReceiver);
	//dialogStateReceiver = new DialogStateReceiver("semaine.data.state.dialog");
	//receivers.push_back(dialogStateReceiver);
	emmaSender = new EmmaSender("semaine.data.state.user", getName());
	senders.push_back(emmaSender);
}

DummyAnalyser::~DummyAnalyser()
{
	delete featureReceiver;
	//delete dialogStateReceiver;
	delete emmaSender;
}


void DummyAnalyser::react(SEMAINEMessage * m) throw(CMSException)
{
	SEMAINEFeatureMessage * fm = dynamic_cast<SEMAINEFeatureMessage *>(m);
	if (fm != NULL) {
		std::vector<float> features = fm->getFeatureVector();
		
		std::stringstream buf;
		buf << fm->getUsertime();
		std::string usertimeString = buf.str();

		
		if (features[0] < 0.002 && userIsSpeaking) { // some arbitrary condition
			// Simulate a turn-yielding event
			// Create and fill a simple EMMA document
			DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
			DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
			DOMElement * behaviour = XMLTool::appendChildElement(interpretation, SemaineML::E_BEHAVIOUR, SemaineML::namespaceURI);
			XMLTool::setAttribute(interpretation, EMMA::A_START, usertimeString);
			XMLTool::setAttribute(behaviour, SemaineML::A_NAME, "gaze-away");
			XMLTool::setAttribute(behaviour, SemaineML::A_INTENSITY, "0.9");
			// Now send it
			emmaSender->sendXML(document, fm->getUsertime());
		} else if (features[0] > 0.998 && !userIsSpeaking) {
			// Simulate a turn-request signal
			// Create and fill a simple EMMA document
			DOMDocument * document = XMLTool::newDocument(EMMA::E_EMMA, EMMA::namespaceURI, EMMA::version);
			DOMElement * interpretation = XMLTool::appendChildElement(document->getDocumentElement(), EMMA::E_INTERPRETATION);
			DOMElement * behaviour = XMLTool::appendChildElement(interpretation, SemaineML::E_BEHAVIOUR, SemaineML::namespaceURI);
			XMLTool::setAttribute(interpretation, EMMA::A_START, usertimeString);
			XMLTool::setAttribute(behaviour, SemaineML::A_NAME, "gaze-at-agent");
			XMLTool::setAttribute(behaviour, SemaineML::A_INTENSITY, "0.9");
			// Now send it
			emmaSender->sendXML(document, fm->getUsertime());
		}
	} else { // a DialogStateMessage
		/*SEMAINEDialogStateMessage sm = (SEMAINEDialogStateMessage) m;
		String speaker = sm.getState().getInfo().get("speaker");
		if (speaker != null) {
			if (speaker.equals("user")) 
				userIsSpeaking = true;
			else
				userIsSpeaking = false;
		}
		*/
	}

}


} // namespace dummy
} // namespace components
} // namespace semaine
