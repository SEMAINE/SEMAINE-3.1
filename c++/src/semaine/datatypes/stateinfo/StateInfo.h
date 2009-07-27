/*
 * StateInfo.h
 *
 *  Created on: 25.07.2009
 *      Author: marc
 */

#ifndef SEMAINE_DATATYPES_STATEINFO_H
#define SEMAINE_DATATYPES_STATEINFO_H

#include <string>
#include <map>

#include <xercesc/dom/DOM.hpp>

#include <semaine/util/XMLTool.h>

#include <semaine/cms/CMSLogger.h>
#include <semaine/cms/exceptions/SystemConfigurationException.h>
#include <semaine/cms/exceptions/MessageFormatException.h>

#include <semaine/datatypes/stateinfo/XPathInfoMapper.h>

namespace semaine {

namespace datatypes {

namespace stateinfo {


class StateInfo {
public:
	// Static stuff:
	static const std::string APIVersion;
	enum Type { AgentState, DialogState, UserState, ContextState, SystemState };
	static const Type Types[];
	static const int numTypes;
	static const std::string TypeNames[];
	static const std::map<Type, XPathInfoMapper *> infosByType;
	// The actual class:
	StateInfo(const std::map<std::string, std::string> & infoItems, const std::string & whatState, Type type)
		throw(semaine::cms::exceptions::SystemConfigurationException);
	virtual ~StateInfo();
	/**
	 * Get the type of state info of this object: AgentState, DialogState, UserState, ContextState, or SystemState.
	 * @return
	 */
	Type getType() { return type; }
	/**
	 * Provide a read-only access to the information in this message.
	 * The map contains as keys all information that can be known according
	 * to the API version, and as non-null values the values taken from the message.
	 * Values for information items not contained in the message will be null.
	 * @return a map with string keys and string values.
	 */
	const std::map<std::string, std::string> & getInfos()
	{
		return info;
	}

	/**
	 * Indicate whether the current info set contains an entry for the named information.
	 * @param name
	 * @return
	 */
	bool hasInfo(const std::string & name)
	{
		return info.find(name) != info.end();
	}

	/**
	 * Get the named information, or the empty string if there is no such information.
	 * To distinguish whether the info is present and has the empty string as its value
	 * or whether the info is not present, use #hasInfo().
	 * @param name
	 * @return
	 */
	const std::string getInfo(const std::string & name)
	{
		std::map<std::string, std::string>::const_iterator it = info.find(name);
		if (it != info.end()) return it->second;
		return "";
	}

	void setInfo(const std::string & name, const std::string & value)
		throw(semaine::cms::exceptions::SystemConfigurationException);

	XERCES_CPP_NAMESPACE::DOMDocument * getDocument()
		throw(semaine::cms::exceptions::SystemConfigurationException);

	const std::string & toString()
	{
		return stateName;
	}

	const std::string & getAPIVersion()
	{
		return APIVersion;
	}







protected:
	std::map<std::string, std::string> info;
	XERCES_CPP_NAMESPACE::DOMDocument * doc;
	semaine::cms::CMSLogger * log;
	const std::string stateName;
	Type type;
    StateInfo(XERCES_CPP_NAMESPACE::DOMDocument * doc, const std::string & whatState, const std::string & rootName, const std::string & rootNamespace, Type type)
		throw(semaine::cms::exceptions::MessageFormatException, semaine::cms::exceptions::SystemConfigurationException);
    virtual void createDocumentFromInfo()
		throw(semaine::cms::exceptions::SystemConfigurationException);
    virtual void analyseDocument(const std::string & rootName, const std::string & rootNamespace)
		throw(semaine::cms::exceptions::MessageFormatException, semaine::cms::exceptions::SystemConfigurationException);
};




}

}

}

#endif /* SEMAINE_DATATYPES_STATEINFO_H */
