/*
 *  SEMAINEFeatureMessage.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 17.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "SEMAINEFeatureMessage.h"

#include <typeinfo>
#include <sstream>
#include <string>

#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>

namespace semaine {
namespace cms {
namespace message {

	SEMAINEFeatureMessage::SEMAINEFeatureMessage(const Message * message)
	throw(semaine::cms::exceptions::MessageFormatException) :
		SEMAINEMessage(message)
	{
		const TextMessage * tm = dynamic_cast<const TextMessage *>(message);
		const BytesMessage * bm = dynamic_cast<const BytesMessage *>(message);
		if (tm == NULL && bm == NULL) {
			throw semaine::cms::exceptions::MessageFormatException(std::string("Expected either a text message or a bytes message, but got a ")+typeid(*message).name());
		}
	}


	SEMAINEFeatureMessage::~SEMAINEFeatureMessage()
	{
	}

	std::vector<float> SEMAINEFeatureMessage::getFeatureVector()
	throw(CMSException, semaine::cms::exceptions::MessageFormatException)
	{
		if (features.empty()) {
			const TextMessage * tm = dynamic_cast<const TextMessage *>(message);
			if (tm != NULL) {
				readFromTextMessage(tm);
			} else {
				const BytesMessage * bm = dynamic_cast<const BytesMessage *>(message);
				assert (bm != NULL);
				readFromBytesMessage(bm);
			}
		}
		assert(!features.empty()); //"features are still null";
		return features;
	}

	std::vector<std::string> SEMAINEFeatureMessage::getFeatureNames()
	throw(CMSException)
	{
		if (featureNames.empty()) {
			const TextMessage * tm = dynamic_cast<const TextMessage *>(message);
			if (tm != NULL) {
				readFromTextMessage(tm);
				assert (!featureNames.empty());
			}
		}
		return featureNames;
	}
	

	void SEMAINEFeatureMessage::readFromBytesMessage(const BytesMessage * bm)
	throw(CMSException, semaine::cms::exceptions::MessageFormatException)
	{
		throw semaine::cms::exceptions::MessageFormatException("byte message not yet implemented");
	}

	

	void SEMAINEFeatureMessage::readFromTextMessage(const TextMessage * tm)
	throw(CMSException)
	{
		std::string text = tm->getText();
		std::stringstream textstream(text);
		std::string line;
		while (!getline(textstream, line).eof()) {
			std::stringstream linestream(line);
			float f;
			std::string n;
			linestream >> f >> n;
			features.push_back(f);
			featureNames.push_back(n);
		}

	}



} // namespace message
} // namespace cms
} // namespace semaine



