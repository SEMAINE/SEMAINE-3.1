/*
 *  cmslogger.h
 *  semaine
 *
 *  Created by Marc Schröder on 09.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */
#ifndef SEMAINE_CMS_CMSLOGGER_H
#define SEMAINE_CMS_CMSLOGGER_H

#include <map>
#include <exception>
#include <string>

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/MessageProducer.h>

#include <semaine/cms/loglevel.h>
#include <semaine/config.h>

using namespace cms;

namespace semaine { 
	namespace cms {
		/**
		 * A relatively lightweight class logging messages to a CMS topic if possible,
		 * and to the local log system if the JMS connection fails. All instances of 
		 * CMSLogger use the same CMS connection and the same session.
		 * @author marc
		 *
		 */
		class CMSLogger {
			
		public:
			static CMSLogger * getLog(const std::string& source);
			
			~CMSLogger();

			void error(const std::string & message, const std::exception * exc = NULL);
			void warn(const std::string & message, const std::exception * exc = NULL);
			void info(const std::string & message, const std::exception * exc = NULL);
			void debug(const std::string & message, const std::exception * exc = NULL);
			
			bool isDebugEnabled();
			
			
		private:
			static Connection * connection;
			static Session * session;
			static std::map<const std::string, CMSLogger *> loggers;
			
			CMSLogger(const std::string & source);
			
			MessageProducer * errorMP;
			MessageProducer * warnMP;
			MessageProducer * infoMP;
			MessageProducer * debugMP;
			const std::string source;
			
			void log(MessageProducer * mp, const std::string & level, const std::string & message, const std::exception * exc);
			
		}; // class CMSLogger
    } // namespace cms
} // namespace semaine

#endif

