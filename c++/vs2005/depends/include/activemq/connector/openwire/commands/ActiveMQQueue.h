/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ACTIVEMQ_CONNECTOR_OPENWIRE_COMMANDS_ACTIVEMQQUEUE_H_
#define _ACTIVEMQ_CONNECTOR_OPENWIRE_COMMANDS_ACTIVEMQQUEUE_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/connector/openwire/commands/ActiveMQDestination.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <decaf/lang/Exception.h>
#include <cms/Queue.h>
#include <vector>
#include <string>

namespace activemq{
namespace connector{
namespace openwire{
namespace commands{

    class ActiveMQQueue : public ActiveMQDestination,
                          public cms::Queue {
    public:

        const static unsigned char ID_ACTIVEMQQUEUE = 100;

    public:

        ActiveMQQueue();
        ActiveMQQueue( const std::string& name );
        virtual ~ActiveMQQueue() {}

        virtual unsigned char getDataStructureType() const;

        /**
         * Clone this object and return a new instance that the
         * caller now owns, this will be an exact copy of this one
         * @returns new copy of this object.
         */
        virtual ActiveMQQueue* cloneDataStructure() const {
            ActiveMQQueue* message = new ActiveMQQueue();
            message->copyDataStructure( this );
            return message;
        }

        /**
         * Copy the contents of the passed object into this objects
         * members, overwriting any existing data.
         * @return src - Source Object
         */
        virtual void copyDataStructure( const DataStructure* src ) {
            ActiveMQDestination::copyDataStructure( src );
        }

        /**
         * Returns a string containing the information for this DataStructure
         * such as its type and value of its elements.
         * @return formatted string useful for debugging.
         */
        virtual std::string toString() const {
            std::ostringstream stream;

            stream << "Begin Class = ActiveMQQueue" << std::endl;
            stream << ActiveMQDestination::toString();
            stream << "End Class = ActiveMQQueue" << std::endl;

            return stream.str();
        }

        /**
         * Compares the DataStructure passed in to this one, and returns if
         * they are equivalent.  Equivalent here means that they are of the
         * same type, and that each element of the objects are the same.
         * @returns true if DataStructure's are Equal.
         */
        virtual bool equals( const DataStructure* value ) const {
            return ActiveMQDestination::equals( value );
        }

        /**
         * @returns the cms::Destination interface pointer that the
         *          objects that derive from this class implement.
         */
        virtual const cms::Destination* getCMSDestination() const {
            return this;
        }

    public:   // CMS Destination

        /**
         * Retrieve the Destination Type for this Destination
         * @return The Destination Type
         */
        virtual cms::Destination::DestinationType getDestinationType(void) const {
            return cms::Destination::QUEUE;
        }

        /**
         * Converts the Destination to a String value representing the
         * Provider specific name fot this destination, which is not
         * necessarily equal to the User Supplied name of the Destination
         * @return Provider specific Name
         */
        virtual std::string toProviderString() const {
            return this->getPhysicalName();
        }

        /**
         * Creates a new instance of this destination type that is a
         * copy of this one, and returns it.
         * @returns cloned copy of this object
         */
        virtual cms::Destination* clone() const {
            return dynamic_cast<cms::Destination*>(
                this->cloneDataStructure() );
        }

        /**
         * Copies the contents of the given Destinastion object to this one.
         * @param source The source Destination object.
         */
        virtual void copy( const cms::Destination& source ) {
            this->copyDataStructure(
                dynamic_cast<const DataStructure*>( &source ) );
        }

        /**
         * Retrieve any properties that might be part of the destination
         * that was specified.  This is a deviation from the JMS spec
         * but necessary due to C++ restrictions.
         * @return const reference to a properties object.
         */
        virtual const cms::CMSProperties& getCMSProperties() const {
            return this->getOptions();
        }

    public:  // CMS Queue

        /**
         * Gets the name of this queue.
         * @return The queue name.
         */
        virtual std::string getQueueName() const
            throw( cms::CMSException ) {

            try{
                return this->getPhysicalName();
            }
            AMQ_CATCH_RETHROW( exceptions::ActiveMQException )
            AMQ_CATCH_EXCEPTION_CONVERT( decaf::lang::Exception, exceptions::ActiveMQException )
            AMQ_CATCHALL_THROW( exceptions::ActiveMQException )
        }

    };

}}}}

#endif /*_ACTIVEMQ_CONNECTOR_OPENWIRE_COMMANDS_ACTIVEMQQUEUE_H_*/

