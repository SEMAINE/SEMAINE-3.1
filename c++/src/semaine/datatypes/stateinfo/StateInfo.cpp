/*
 * StateInfo.cpp
 *
 *  Created on: 25.07.2009
 *      Author: marc
 */

#include <semaine/datatypes/stateinfo/StateInfo.h>

#include <fstream>
#include <string>
#include <list>

using namespace XERCES_CPP_NAMESPACE;
using namespace semaine::util;
using namespace semaine::cms::exceptions;

namespace semaine {

namespace datatypes {

namespace stateinfo {


//////////////////// Static stuff ///////////////////////

// XERCES does not fully support XPath, so we need to simplify some of the expressions:
#define XERCES_XPATH_SIMPLIFICATION 1

const std::string StateInfo::APIVersion = "0.2";

const StateInfo::Type StateInfo::Types[] = { StateInfo::AgentState, StateInfo::DialogState, StateInfo::UserState, StateInfo::ContextState, StateInfo::SystemState };
const int StateInfo::numTypes = 5;
const std::string StateInfo::TypeNames[] = {"AgentState", "DialogState", "UserState", "ContextState", "SystemState" };

void trim(std::string& str, const std::locale& loc = std::locale()) {
	std::string::size_type pos = 0;
	while (pos < str.size() && isspace(str[pos], loc)) pos++;
	str.erase(0, pos);
	pos = str.size();
	while (pos > 0 && isspace(str[pos - 1], loc)) pos--;
	str.erase(pos);
}

std::vector<std::string> tokenize(const std::string& str,
                      const std::string& delimiters = " ")
{
	std::vector<std::string> tokens;

    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return tokens;
}


XPathInfoMapper * getXPathExpressions(StateInfo::Type & type, const std::list<std::string> & configSection)
throw(SystemConfigurationException)
{
	const char * delims = "\t =:";
	std::map<std::string, std::string> namespacePrefixes;
	std::map<std::string, std::string> shortNames;

	bool readNamespacePrefixes = false;
	bool readShortNames = false;
	for (std::list<std::string>::const_iterator it = configSection.begin(); it != configSection.end(); ++it) {
		std::string line = *it;
		if (line == "[namespace prefixes]") {
			readNamespacePrefixes = true;
		} else if (line == "[short names]") {
			readShortNames = true;
			readNamespacePrefixes = false;
			if (namespacePrefixes.empty()) {
				throw SystemConfigurationException("Namespace prefixes must be defined before short names");
			}
		} else if (readNamespacePrefixes) {
			int sepStart = line.find_first_of(delims);
			int secondStart = line.find_first_not_of(delims, sepStart);
			if (sepStart == std::string::npos || secondStart == std::string::npos) {
				throw SystemConfigurationException("Expected namespace prefix definition, got '"+line+"'");
			}
			const std::string prefix = line.substr(0, sepStart);
			const std::string namespaceURI = line.substr(secondStart);
			namespacePrefixes[prefix] = namespaceURI;
		} else if (readShortNames) {
			int sepStart = line.find_first_of(delims);
			int secondStart = line.find_first_not_of(delims, sepStart);
			if (sepStart == std::string::npos || secondStart == std::string::npos) {
				throw new SystemConfigurationException("Expected short name to XPath definition, got '"+line+"'");
			}
			std::string shortName = line.substr(0, sepStart);
			std::string xpathExpr = line.substr(secondStart);
			shortNames[shortName] = xpathExpr;
		}
	}

	return new XPathInfoMapper(namespacePrefixes, shortNames);
}

const std::map<StateInfo::Type, XPathInfoMapper *> getInfosByType()
{
	try {
		std::map<StateInfo::Type, std::list<std::string> > configSections;

		std::ifstream configFile(CONFIG_STATEINFO.c_str());
		if (configFile.fail())
			throw SystemConfigurationException("Problem opening stateinfo config file "+CONFIG_STATEINFO);
		std::string line;
		std::list<std::string> currentSection;
		const StateInfo::Type * currentType = NULL;
		while (!configFile.eof()) {
			getline(configFile, line);
			trim(line);
			if (line == "" || line[0] == '#') {
				continue;
			}
			if (line[0] == '[') {
				for (int i=0; i<StateInfo::numTypes; i++) {
					if (line == "["+StateInfo::TypeNames[i]+"]") {
						if (currentType != NULL) {
							configSections[*currentType] = currentSection; // copy constructor used
						}
						currentSection.clear();
						currentType = &(StateInfo::Types[i]);
						break;
					}
				}
			}
			currentSection.push_back(line);
		}
		if (currentType != NULL) {
			configSections[*currentType] = currentSection;
		}
		configFile.close();

		std::map<StateInfo::Type, XPathInfoMapper *> t2m;
		for (int i=0; i<StateInfo::numTypes; i++) {
			StateInfo::Type t = StateInfo::Types[i];
			std::list<std::string> configSection = configSections[t];
			if (configSection.size() == 0)
				throw SystemConfigurationException("No config section for "+StateInfo::TypeNames[t]);
			t2m[t] = getXPathExpressions(t, configSection);
		}
		return t2m;

	} catch (SystemConfigurationException & exc) {
		std::cerr << "SystemConfigurationException: Cannot initialise state info configuration: " << std::endl
			<< exc.what() << std::endl;
		throw exc;
	}

}


const std::map<StateInfo::Type, XPathInfoMapper *> StateInfo::infosByType = getInfosByType();


/////////////////////// The actual StateInfo class //////////////////////


StateInfo::StateInfo(const std::map<std::string, std::string> & infoItems, const std::string & whatState, Type type)
throw(SystemConfigurationException)
: stateName(whatState), type(type), info(infoItems), doc(NULL)
{
    log = semaine::cms::CMSLogger::getLog(stateName);
	// Verify that we can handle these info items:
	const std::map<std::string, std::string> info2expr = infosByType.find(type)->second->getExpressionMap();
	for (std::map<std::string, std::string>::const_iterator it = infoItems.begin(); it != infoItems.end(); ++it) {
		std::string key = it->first;
		if (info2expr.find(key) == info2expr.end()) {
			throw SystemConfigurationException("Don't know how to handle info item '"+key+"' -- something seems to be out of sync");
		}
	}
}


StateInfo::StateInfo(DOMDocument * doc, const std::string & whatState, const std::string & rootName, const std::string & rootNamespace, Type type)
throw(MessageFormatException, SystemConfigurationException)
	: doc(doc), stateName(whatState), type(type)
{
    log = semaine::cms::CMSLogger::getLog(stateName);
    analyseDocument(rootName, rootNamespace);
}


StateInfo::~StateInfo()
{
}

void StateInfo::setInfo(const std::string & name, const std::string & value)
throw(SystemConfigurationException)
{
	// Verify that we can handle these info items:
	const std::map<std::string, std::string> info2expr = infosByType.find(type)->second->getExpressionMap();
	if (info2expr.find(name) == info2expr.end()) {
		throw SystemConfigurationException("Don't know how to handle info item '"+name+"' -- something seems to be out of sync");
	}
	info[name] = value;
	doc = NULL; // if it was there, it's now out of date
}

DOMDocument * StateInfo::getDocument()
throw(SystemConfigurationException)
{
	if (doc == NULL) {
		createDocumentFromInfo();
	}
	return doc;
}



void StateInfo::createDocumentFromInfo()
throw(SystemConfigurationException)
{
	doc = NULL;
	XPathInfoMapper * xpathInfoMapper = infosByType.find(type)->second;
	const std::map<std::string, std::string> infoNames = xpathInfoMapper->getExpressionMap();
	const NamespaceContext * namespaceContext = xpathInfoMapper->getNamespaceContext();

	for(std::map<std::string, std::string>::iterator it = info.begin(); it != info.end(); ++it) {
		std::string shortName = it->first;
		std::string value = it->second;
		std::cout << "Now creating doc representation for " << shortName << " = " << value << std::endl;
		if (value == "") continue;
		DOMElement * currentElt = NULL;
		std::map<std::string, std::string>::const_iterator xpathLookupIt = infoNames.find(shortName);
		if (xpathLookupIt == infoNames.end()) {
			throw SystemConfigurationException("No info entry for '"+shortName+"' -- something seems to be out of sync");
		}
		std::string expr = xpathLookupIt->second;
		std::vector<std::string> parts = tokenize(expr, "/");
		// Now go through all parts except the last one:
		for (int i=0, max=parts.size()-1; i<max; i++) {
			std::string part = parts[i];
			// if there is more than one slash, indicating arbitrary substructures, we treat it like a single slash (direct child)
			if (part == "") continue;
			std::vector<std::string> prefixAndName = tokenize(part, ":");
			if (prefixAndName.size() > 2)
				throw SystemConfigurationException("Erroneous XPath expression for info '"+shortName+"': part '"+part+"'");
			std::string prefix = prefixAndName.size() == 2 ? prefixAndName[0] : "";
			std::string namespaceURI = prefix != "" ? namespaceContext->getNamespaceURI(prefix) : "";
			std::string localNameExpr = prefixAndName.back();
			std::string localName;
			std::string attName;
			bool haveAttName = false;
			std::string attValue;
			bool haveAttValue = false;
			if (localNameExpr.find("[") != std::string::npos) { // attribute-based selection of nodes
				int openB = localNameExpr.find("[");
				int closeB = localNameExpr.find("]");
				if (closeB < openB)
					throw SystemConfigurationException("Erroneous XPath expression for info '"+shortName+"': part '"+part+"'");
				localName = localNameExpr.substr(0, openB);
				std::string attValPair = localNameExpr.substr(openB+1, closeB-openB-1);
				if (attValPair[0] != '@') {
					throw SystemConfigurationException("Cannot generate document from XPath expression for info '"+shortName+"': part '"+part+"'");
				}
				int iEqualSign = attValPair.find("=");
				if (iEqualSign != std::string::npos) {
					attName = attValPair.substr(1, iEqualSign-1);
					haveAttName = true;
					attValue = attValPair.substr(iEqualSign+1);
					haveAttValue = true;
					if (!(attValue[0] == '\'' && attValue[attValue.length()-1] == '\'')
							|| (attValue[0] == '"' && attValue[attValue.length()-1] == '"')) {
						throw SystemConfigurationException("For info '"+shortName+"', unexpected attribute value "+attValue+" found in part '"+part+"' -- not quoted?");
					}
					attValue = attValue.substr(1, attValue.length()-2);
				} else {
					attName = attValPair.substr(1);
					haveAttName = true;
					haveAttValue = false;
				}
			} else {
				localName = localNameExpr;
			}
			// Now traverse to or create element defined by prefix, localName and namespaceURI
			if (currentElt == NULL) { // at top level
				if (doc == NULL) { // create a new document
					try {
						doc = XMLTool::newDocument(localName, namespaceURI);
					} catch (DOMException de) {
						throw SystemConfigurationException("For info '"+shortName+"', cannot create document for localname '"+localName+"' and namespaceURI '"+namespaceURI+"'");
					}
					currentElt = doc->getDocumentElement();
					XMLTool::setPrefix(currentElt, prefix);
				} else {
					currentElt = doc->getDocumentElement();
					if (XMLTool::getLocalName(currentElt) != localName) {
						throw SystemConfigurationException("Incompatible root node specification: expression for '"+shortName+"' requests '"+
								localName+"', but previous expressions have requested '"+XMLTool::getLocalName(currentElt)+"'!");
					}
					std::string currentNamespaceURI = XMLTool::getNamespaceURI(currentElt);
					if (currentNamespaceURI != namespaceURI) {
						throw SystemConfigurationException("Incompatible root namespace specification: expression for '"+shortName+"' requests '"+
								namespaceURI+"', but previous expressions have requested '"+currentNamespaceURI+"'!");
					}
				}
			} else { // somewhere in the tree
				// First check if the requested child already exists
				std::list<DOMElement *> * sameNameChildren = XMLTool::getChildrenByLocalNameNS(currentElt, localName, namespaceURI);
				bool found = false;
				if (!haveAttName) {
					if (sameNameChildren->size() > 0) {
						currentElt = sameNameChildren->front();
						found = true;
					}
				} else {
					for (std::list<DOMElement *>::iterator it = sameNameChildren->begin(); it != sameNameChildren->end(); ++it) {
						DOMElement * c = *it;
						if (XMLTool::hasAttribute(c, attName)) {
							if (!haveAttValue || attValue == XMLTool::getAttribute(c, attName)) {
								currentElt = c;
								found = true;
								break;
							}
						}
					}
				}
				if (!found) { // need to create it
					currentElt = XMLTool::appendChildElement(currentElt, localName, namespaceURI);
					if (prefix != "") XMLTool::setPrefix(currentElt, prefix);
					if (haveAttName) {
						XMLTool::setAttribute(currentElt, attName, haveAttValue ? attValue : "");
					}
				}
			}
		} // for all parts except the last one
		if (currentElt == NULL)
			throw SystemConfigurationException("No elements or no final part created for info '"
					+shortName+"' from XPath expression '"+expr+"'");
		std::string lastPart = parts.back();
		trim(lastPart);
		if (lastPart == "text()") { // textual content of currentElt
			XMLTool::setTextContent(currentElt, value);
		} else if (lastPart[0] == '@') {
			std::string attName = lastPart.substr(1);
			XMLTool::setAttribute(currentElt, attName, value);
		} else {
			throw new SystemConfigurationException("Do not know how to assign value for info '"+shortName+"' from last part of xpath expression '"
					+lastPart+"' -- expected either 'text()' or '@attributeName'");
		}
	}
}

void StateInfo::analyseDocument(const std::string & rootName, const std::string & rootNamespace)
throw(MessageFormatException, SystemConfigurationException)
{
	if (doc == NULL) {
		throw SystemConfigurationException("Received NULL document");
	}
	DOMElement * root = doc->getDocumentElement();
	if (XMLTool::getLocalName(root) != rootName) {
		throw MessageFormatException("XML document should have root node '"+
							rootName+"', but has '"+XMLTool::getLocalName(root)+"'!");
	}
	if (XMLTool::getNamespaceURI(root) != rootNamespace) {
		throw MessageFormatException("root node should have namespace '"+
				rootNamespace+"' but has '"+XMLTool::getNamespaceURI(root)+"'!");
	}

	// Now extract the information from the document using the XPath expressions given in a config file
	XPathInfoMapper * xpathInfoMapper = infosByType.find(type)->second;
	const std::map<std::string, std::string> infoNames = xpathInfoMapper->getExpressionMap();
	const NamespaceContext * namespaceContext = xpathInfoMapper->getNamespaceContext();
	for (std::map<std::string, std::string>::const_iterator it = infoNames.begin(); it != infoNames.end(); ++it) {
		std::string shortName = it->first;
		std::string xpathExpr = it->second;
#ifdef XERCES_XPATH_SIMPLIFICATION
		if (xpathExpr.find("[@") != std::string::npos) { // attribute-based selection of nodes not supported in Xerces
			std::string simplified = xpathExpr;
			while (simplified.find("[@") != std::string::npos) {
				int start = simplified.find("[@");
				int end = simplified.find("]", start);
				if (end == std::string::npos) {
					throw SystemConfigurationException("Failed to simplify XPath expression for Xerces' limited XPath support: no closing square bracket found in "+simplified);
				}
				simplified.erase(start, end-start+1);
			}
			std::cerr << "Simplifying XPath expression for Xerces' limited XPath support." << std::endl
				<< "Original:   " << xpathExpr << std::endl
				<< "Simplified: " << simplified << std::endl;
			xpathExpr = simplified;
		}
#endif
		try {
			XMLCh * xmlXpathExpr = XMLString::transcode(xpathExpr.c_str());
			// TODO: the following throws an exception because String-based lookup is not implemented in Xerces:
			DOMXPathResult * result = (DOMXPathResult *) doc->evaluate(xmlXpathExpr, doc, namespaceContext, DOMXPathResult::STRING_TYPE, NULL);
			std::string resultString = XMLTool::transcode(result->getStringValue());
			if (resultString != "") {
				info[shortName] = resultString;
				log->debug("Read info: "+shortName+" = "+resultString);
			}
		} catch (DOMXPathException & xee) {
			char * err = XMLString::transcode(xee.msg);
			std::cerr << err << std::endl;
			throw MessageFormatException("Problem analysing value of '"+shortName+"': "+err);
		} catch (DOMException & xe) {
			char * err = XMLString::transcode(xe.getMessage());
			std::cerr << err << std::endl;
			throw MessageFormatException("Problem analysing value of '"+shortName+"': "+err);
		}
	}

}










}

}

}
