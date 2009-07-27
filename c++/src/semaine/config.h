/*
 *  config.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CONFIG_H
#define SEMAINE_CONFIG_H

#ifdef _MSC_VER
// On Visual C++, avoid warnings of the type:
// warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning( disable : 4290 )
#endif

#ifdef HAVE_CONFIG_H
// config.h created by autoconf
#include <config.h>
#endif


#include <string>
#include <stdlib.h>
#include <cassert>
#include <stdlib.h>

inline std::string _determine_CMS_URL()
{
	const char * url = getenv("CMS_URL");
	if (url == NULL) url = "tcp://localhost:61616";
	return std::string(url);
}

inline std::string _determine_CONFIG_STATEINFO()
{
	const char * conf = getenv("CONFIG_STATEINFO");
	if (conf == NULL) conf = "stateinfo.config";
	return std::string(conf);
}

const std::string DEFAULT_CMS_URL = _determine_CMS_URL();
const std::string DEFAULT_CMS_USER = "";
const std::string DEFAULT_CMS_PASSWORD = "";

const std::string CONFIG_STATEINFO = _determine_CONFIG_STATEINFO();



#endif

