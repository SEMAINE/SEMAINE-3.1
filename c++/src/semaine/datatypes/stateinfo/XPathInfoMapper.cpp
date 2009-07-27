/*
 * StateInfo.cpp
 *
 *  Created on: 25.07.2009
 *      Author: marc
 */

#include <semaine/datatypes/stateinfo/XPathInfoMapper.h>


using namespace XERCES_CPP_NAMESPACE;
using namespace semaine::util;

namespace semaine {

namespace datatypes {

namespace stateinfo {



XPathInfoMapper::XPathInfoMapper(const std::map<std::string, std::string> & prefixes2NamespaceURIs,
	const std::map<std::string, std::string> & names2XpathExpressions)
	: names2exprString(names2XpathExpressions),
	namespaceContext(prefixes2NamespaceURIs)
{
}










}

}

}
