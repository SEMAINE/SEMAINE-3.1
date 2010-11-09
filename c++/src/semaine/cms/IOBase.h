/*
 *  iobase.h
 *  semaine
 *
 *  Created by Marc Schröder on 29.08.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#ifndef SEMAINE_CMS_IOBASE_H
#define SEMAINE_CMS_IOBASE_H

#include <semaine/config.h>

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/CMSException.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <iostream>

#include <semaine/cms/event.h>

#include "CMSLogger.h"

using namespace cms;

namespace semaine { 
	namespace cms {

    class ExcListener;

	//////////////////// IOBase //////////////////////

		
		/**
		 * This class handles the low-level interaction with the JMS provider for
		 * the components of the SEMAINE system.
		 * 
		 * @author marc
		 *
		 */
		class IOBase {
		protected:
			const std::string cmsUrl;
			const std::string cmsUser;
			const std::string cmsPassword;
			Connection * connection;
			Session * session;
			Topic * topic;
			bool isConnectionStarted;
			const CMSException * exception;
			
		public:
			/**
			 * Create a new CMS IOBase connection with the given topic on the default JMS server.
			 * The default JMS server is determined as follows:
			 * <ul>
			 * <li>jms URL: default value <code>tcp://localhost:61616</code>;</li>
			 * <li>jms User: default value empty;</li>
			 * <li>jms Password: default value empty.</li>
			 * </ul>
			 * @param topicName the name of the topic to send to.
			 * @throws CMSException
			 */
			IOBase(const std::string & topicName)
			throw (CMSException);
			
			virtual ~IOBase();
			
			/**
			 * Create a new CMS IOBase connection with the given topic on the given JMS server.
			 * @param cmsUrl the url where to contact the JMS server
			 * @param cmsUser the username to use (can be an empty string)
			 * @param cmsPassword the password to use (can be an empty string)
			 * @param topicName the name of the topic to send to.
			 * @throws CMSException
			 */
			IOBase(const std::string & cmsUrl, const std::string & cmsUser, const std::string & cmsPassword, const std::string & topicName)
			throw (CMSException);
			
			/**
			 * Get the name of the topic.
			 * @return a string containing the topic name.
			 * @throws JMSException
			 */
			std::string getTopicName()
			throw (CMSException);
			
			void startConnection()
			throw (CMSException);
			
			/**
			 * Get the URL of the JMS server connected to.
			 * @return a string containing the url
			 */
			std::string getCMSUrl()
			{
				return cmsUrl;
			}
			
			Connection* getConnection()
			{
				return connection;
			}
			
			Session* getSession()
			{
				return session;
			}
			
			Topic* getTopic()
			{
				return topic;
			}
			
			
			
		friend class ExcListener;

		private:
			void initialise(const std::string & topicName) throw (CMSException);
			void setException(const CMSException & ex);
			ExcListener * excListener;

		}; // class IOBase
		
		//////////////// Exception listener ///////////////

		class ExcListener : public ExceptionListener {
		public:
			ExcListener(IOBase * iobase) :
				iobase(iobase)
			{}
			virtual void onException(const CMSException & ex) {
			 	CMSLogger::getLog("Connection")->error(std::string("Exception Listener: "), &ex);
		        iobase->setException(ex);
			}
		private:
			IOBase * iobase;
		};


		
	} // namespace cms
} // namespace semaine
#endif


