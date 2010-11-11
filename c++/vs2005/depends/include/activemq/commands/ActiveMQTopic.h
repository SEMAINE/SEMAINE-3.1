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

#ifndef _ACTIVEMQ_COMMANDS_ACTIVEMQTOPIC_H_
#define _ACTIVEMQ_COMMANDS_ACTIVEMQTOPIC_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/util/Config.h>
#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/exceptions/ActiveMQException.h>
#include <decaf/lang/Exception.h>
#include <cms/Topic.h>
#include <vector>
#include <string>
#include <memory>

namespace activemq{
namespace commands{

    class AMQCPP_API ActiveMQTopic : public ActiveMQDestination,
                                     public cms::Topic {
    public:

        const static unsigned char ID_ACTIVEMQTOPIC = 101;

    public:

        ActiveMQTopic();
        ActiveMQTopic( const std::string& name );
        virtual ~ActiveMQTopic();

        virtual unsigned char getDataStructureType() const;

        /**
         * Clone this object and return a new instance that the
         * caller now owns, this will be an exact copy of this one
         * @returns new copy of this object.
         */
        virtual ActiveMQTopic* cloneDataStructure() const;

        /**
         * Copy the contents of the passed object into this objects
         * members, overwriting any existing data.
         * @return src - Source Object
         */
        virtual void copyDataStructure( const DataStructure* src );

        /**
         * Returns a string containing the information for this DataStructure
         * such as its type and value of its elements.
         * @return formatted string useful for debugging.
         */
        virtual std::string toString() const;

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
        virtual cms::Destination::DestinationType getDestinationType() const {
            return cms::Destination::TOPIC;
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
         * Copies the contents of the given Destination object to this one.
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

    public:  // CMS Topic

        /**
         * Gets the name of this topic.
         * @return The topic name.
         */
        virtual std::string getTopicName() const throw( cms::CMSException ) {
            return this->getPhysicalName();
        }

    };

}}

#endif /*#define _ACTIVEMQ_COMMANDS_ACTIVEMQTOPIC_H_*/
