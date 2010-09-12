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


const std::string StateInfo::APIVersion = "0.2";

const StateInfo::Type StateInfo::Types[] = { StateInfo::AgentState, StateInfo::DialogState, StateInfo::UserState, StateInfo::ContextState, StateInfo::SystemState };
const int StateInfo::numTypes = 5;
const std::string StateInfo::TypeNames[] = {"AgentState", "DialogState", "UserState", "ContextState", "SystemState" };


//////////////// Helper functions ////////////////////

void trim(std::string& str, const std::locale& loc = std::locale()) {
#ifdef _MSC_VER
#define MY_ISSPACE(a,b) isspace(a)
#else
#define MY_ISSPACE(a,b) isspace(a,b)
#endif
	std::string::size_type pos = 0;
	while (pos < str.size() && MY_ISSPACE(str[pos], loc)) pos++;
	str.erase(0, pos);
	pos = str.size();
	while (pos > 0 && MY_ISSPACE(str[pos - 1], loc)) pos--;
	str.erase(pos);
#undef MY_ISSPACE
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


/**
 * The given string is interpreted as a limited subset of XPath expressions and split into parts.
 * Each part except the last one is expected to follow precisely the following form:
 * <code>"/" ( prefix ":" ) ? localname ( "[" "@" attName "=" "'" attValue "'" "]" ) ?</code>
 * The last part must be either
 * <code> "/" "text()"</code>
 * or
 * </code> "/" "@" attributeName </code>.
 * @param expr the string to be split as an xpath expression
 * @return an array of string arrays. All except the last entry in this array is guaranteed to have
 * four elements, with the meaning:
 * <ul>
 * <li>[0]: prefix (can be the empty string)</li>
 * <li>[1]: localname (guaranteed not to be empty)</li>
 * <li>[2]: attributeName (can be empty)</li>
 * <li>[3]: attributeValue (is guaranteed to be empty when attributeName is empty;
 *  when attributeName is non-empty, and attributeValue is empty,
 *  then the attribute must be present but there are no constraints about its value)</li>
 * </ul>
 * The last element in the returned array has either length 1 or length 0. If it is of length 0, then the XPath expression ended in "text()",
 * i.e. the value to be referenced is the text content of the enclosing Element; if it is of length 1, the String
 * contained is guaranteed to be non-null and represents the name of the attribute to be referenced on the enclosing Element.
 *
 * @throws SystemConfigurationException if expr does not match the expected format.
 */
std::vector< std::vector<std::string> > splitXPathIntoParts(const std::string & expr)
throw(SystemConfigurationException) {
	std::vector< std::vector<std::string> > parts;
	if (expr[0] != '/') {
		throw SystemConfigurationException("XPath expression does not start with a slash: "+expr);
	}
	int pos = 1;
	// Structure of each part except the last part:
	// "/" ( prefix ":" ) ? localname ( "[" "@" attName "=" "'" attValue "'" "]" ) ?
	// Avoid regular expression code so it is easier to port to C++.
	// Strategy:
	// find first of ":[/";
	// - if ":", detach prefix, find first of "[/", and continue in next line:
	// - if "/", there is no attribute;
	// - if "[", require "@", find "=" followed by "'", then find next "'", require "]" and "/".
	while (true) { // We loop until we cannot match a '/' anymore, because last part is different
		std::string prefix = "";
		std::string localname = "";
		std::string attName = "";
		std::string attValue = "";
		int nextColonPos = expr.find(':', pos);
		int nextSlashPos = expr.find('/', pos);
		int nextOpenSqB = expr.find('[', pos);
		if (nextSlashPos == std::string::npos) {
			break; // pos is start of last part
		}
		if (nextColonPos != -1 && nextColonPos < nextSlashPos) {
			prefix = expr.substr(pos, nextColonPos-pos);
			pos = nextColonPos + 1;
		}
		// Attributes?
		if (nextOpenSqB != std::string::npos && nextOpenSqB < nextSlashPos) {
			if (nextOpenSqB <= pos) {
				throw SystemConfigurationException("Wrong square bracket location in XPath expression "+expr);
			}
			localname = expr.substr(pos, nextOpenSqB-pos);
			if (expr[nextOpenSqB+1] != '@') {
				throw SystemConfigurationException("Expected '@' character after '[' in XPath expression "+expr);
			}
			int equalPos = expr.find('=', nextOpenSqB);
			int nextCloseSqB = expr.find(']', nextOpenSqB);
			if (equalPos != std::string::npos && equalPos+2<nextCloseSqB) {
				// we have an attribute value
				attName = expr.substr(nextOpenSqB+2, equalPos-(nextOpenSqB+2));
				if (expr[equalPos+1] != '\'' || expr[nextCloseSqB-1] != '\'') {
					throw SystemConfigurationException("Attribute value for attribute '"+attName+"' must be in single quotes, in XPath expression "+expr);
				}
				attValue = expr.substr(equalPos+2, nextCloseSqB-1-(equalPos+2));
			} else { // only attribute name, no value
				attName = expr.substr(nextOpenSqB+2, nextCloseSqB-(nextOpenSqB+2));
			}
			nextSlashPos = nextCloseSqB+1;
			if (nextSlashPos >= expr.length() || expr[nextSlashPos] != '/') {
				throw SystemConfigurationException("XPath expression seems malformed: no slash after closed square bracket: "+expr);
			}
		} else { // no attribute
			localname = expr.substr(pos, nextSlashPos-pos);
			// attName and attValue stay empty
		}
		std::vector<std::string> part;
		part.push_back(prefix);
		part.push_back(localname);
		part.push_back(attName);
		part.push_back(attValue);
		parts.push_back(part);
		pos = nextSlashPos + 1;
	}
	// Last part: expect either 'text()' or '@attributeName'
	if (pos >= expr.length()) {
		throw SystemConfigurationException("XPath expression is expected to contain as final part either 'text()' or '@attributeName': "+expr);
	}
	if (expr[pos] == '@') {
		std::string attName = expr.substr(pos+1);
		std::vector<std::string> part;
		part.push_back(attName);
		parts.push_back(part);
	} else if (expr.substr(pos) == "text()") {
		std::vector<std::string> part; // vector with 0 elements
		parts.push_back(part);
	} else {
		throw SystemConfigurationException("XPath expression is expected to contain as final part either 'text()' or '@attributeName': "+expr);
	}
	return parts;
}



XPathInfoMapper * getXPathExpressions(StateInfo::Type & type, const std::list<std::string> & configSection)
throw(SystemConfigurationException)
{
	const char * delims = "\t =:";
	std::map<std::string, std::string> namespacePrefixes;
	std::map<std::string, std::string> shortNames;
	// Temporary map to make sure no two short names have the same expression:
	std::map<std::string, std::string> expr2shortname;

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
			std::string::size_type sepStart = line.find_first_of(delims);
			std::string::size_type secondStart = line.find_first_not_of(delims, sepStart);
			if (sepStart == std::string::npos || secondStart == std::string::npos) {
				throw SystemConfigurationException("Expected namespace prefix definition, got '"+line+"'");
			}
			const std::string prefix = line.substr(0, sepStart);
			const std::string namespaceURI = line.substr(secondStart);
			namespacePrefixes[prefix] = namespaceURI;
		} else if (readShortNames) {
			std::string::size_type sepStart = line.find_first_of(delims);
			std::string::size_type secondStart = line.find_first_not_of(delims, sepStart);
			if (sepStart == std::string::npos || secondStart == std::string::npos) {
				throw SystemConfigurationException("Expected short name to XPath definition, got '"+line+"'");
			}
			std::string shortName = line.substr(0, sepStart);
			std::string xpathExpr = line.substr(secondStart);
			// Fail-early strategy: we want to verify at load time whether all entries match our expected format:
			splitXPathIntoParts(xpathExpr);
			if (shortNames.find(shortName) != shortNames.end()) {
				throw SystemConfigurationException("In section "+StateInfo::TypeNames[type]
				    +", short name '"+shortName+"' occurs more than once");
			}
			if (expr2shortname.find(xpathExpr) != expr2shortname.end()) {
				throw SystemConfigurationException("In section "+StateInfo::TypeNames[type]
				    +", short names '"+shortName+"' and '"+expr2shortname[xpathExpr]+"' have the same XPath expression");
			}
			shortNames[shortName] = xpathExpr;
			expr2shortname[xpathExpr] = shortName;
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
		if (value == "") continue;
		DOMElement * currentElt = NULL;
		std::map<std::string, std::string>::const_iterator xpathLookupIt = infoNames.find(shortName);
		if (xpathLookupIt == infoNames.end()) {
			throw SystemConfigurationException("No info entry for '"+shortName+"' -- something seems to be out of sync");
		}
		std::string expr = xpathLookupIt->second;


		std::vector< std::vector<std::string> > parts = splitXPathIntoParts(expr);
		for (int i=0; i<parts.size()-1; i++) {
			std::vector<std::string> part = parts[i];
			std::string prefix = part[0];
			std::string localName = part[1];
			std::string attName = part[2];
			std::string attValue = part[3];
			std::string namespaceURI = prefix != "" ? namespaceContext->getNamespaceURI(prefix) : "";

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
				if (attName.empty()) {
					if (sameNameChildren->size() > 0) {
						currentElt = sameNameChildren->front();
						found = true;
					}
				} else {
					for (std::list<DOMElement *>::iterator it = sameNameChildren->begin(); it != sameNameChildren->end(); ++it) {
						DOMElement * c = *it;
						if (XMLTool::hasAttribute(c, attName)) {
							if (attValue.empty() || attValue == XMLTool::getAttribute(c, attName)) {
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
					if (!attName.empty()) {
						XMLTool::setAttribute(currentElt, attName, attValue);
					}
				}
			}
		} // for all parts except the last one

		if (currentElt == NULL)
			throw SystemConfigurationException("No elements or no final part created for info '"
					+shortName+"' from XPath expression '"+expr+"'");
		std::vector<std::string> lastPart = parts.back();

		if (lastPart.empty()) { // textual content of currentElt
			XMLTool::setTextContent(currentElt, value);
		} else {
			std::string attName = lastPart[0];
			XMLTool::setAttribute(currentElt, attName, value);
		}




/*
		std::vector<std::string> parts = tokenize(expr, "/");
		// Now go through all parts except the last one:
		for (std::string::size_type i=0, max=parts.size()-1; i<max; i++) {
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
				std::string::size_type openB = localNameExpr.find("[");
				std::string::size_type closeB = localNameExpr.find("]");
				if (closeB < openB)
					throw SystemConfigurationException("Erroneous XPath expression for info '"+shortName+"': part '"+part+"'");
				localName = localNameExpr.substr(0, openB);
				std::string attValPair = localNameExpr.substr(openB+1, closeB-openB-1);
				if (attValPair[0] != '@') {
					throw SystemConfigurationException("Cannot generate document from XPath expression for info '"+shortName+"': part '"+part+"'");
				}
				std::string::size_type iEqualSign = attValPair.find("=");
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
		*/
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
		try {
			XMLCh * xmlXpathExpr = XMLString::transcode(xpathExpr.c_str());
			DOMXPathResult * result = (DOMXPathResult *) doc->evaluate(xmlXpathExpr, doc, namespaceContext, DOMXPathResult::FIRST_RESULT_TYPE, NULL);
			if (result->isNode()) {
				std::string resultString = XMLTool::transcode(result->getStringValue());
				if (resultString != "") {
					info[shortName] = resultString;
					log->debug("Read info: "+shortName+" = "+resultString);
				}
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
