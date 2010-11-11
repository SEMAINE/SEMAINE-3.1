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

#ifndef _ACTIVEMQ_COMMANDS_SUBSCRIPTIONINFO_H_
#define _ACTIVEMQ_COMMANDS_SUBSCRIPTIONINFO_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/BaseDataStructure.h>
#include <activemq/util/Config.h>
#include <decaf/lang/Pointer.h>
#include <string>
#include <vector>

namespace activemq{
namespace commands{

    using decaf::lang::Pointer;

    /*
     *
     *  Command code for OpenWire format for SubscriptionInfo
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API SubscriptionInfo : public BaseDataStructure {
    protected:

        std::string clientId;
        Pointer<ActiveMQDestination> destination;
        std::string selector;
        std::string subcriptionName;
        Pointer<ActiveMQDestination> subscribedDestination;

    public:

        const static unsigned char ID_SUBSCRIPTIONINFO = 55;

    protected:

        SubscriptionInfo( const SubscriptionInfo& ) : BaseDataStructure() {};
        SubscriptionInfo& operator= ( const SubscriptionInfo& ) { return *this; };

    public:

        SubscriptionInfo();

        virtual ~SubscriptionInfo();

        /**
         * Get the unique identifier that this object and its own
         * Marshaler share.
         * @returns new DataStructure type copy.
         */
        virtual unsigned char getDataStructureType() const;

        /**
         * Clone this object and return a new instance that the
         * caller now owns, this will be an exact copy of this one
         * @returns new copy of this object.
         */
        virtual SubscriptionInfo* cloneDataStructure() const;

        /**
         * Copy the contents of the passed object into this object's
         * members, overwriting any existing data.
         * @param src - Source Object
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
        virtual bool equals( const DataStructure* value ) const;

        virtual const std::string& getClientId() const;
        virtual std::string& getClientId();
        virtual void setClientId( const std::string& clientId );

        virtual const Pointer<ActiveMQDestination>& getDestination() const;
        virtual Pointer<ActiveMQDestination>& getDestination();
        virtual void setDestination( const Pointer<ActiveMQDestination>& destination );

        virtual const std::string& getSelector() const;
        virtual std::string& getSelector();
        virtual void setSelector( const std::string& selector );

        virtual const std::string& getSubcriptionName() const;
        virtual std::string& getSubcriptionName();
        virtual void setSubcriptionName( const std::string& subcriptionName );

        virtual const Pointer<ActiveMQDestination>& getSubscribedDestination() const;
        virtual Pointer<ActiveMQDestination>& getSubscribedDestination();
        virtual void setSubscribedDestination( const Pointer<ActiveMQDestination>& subscribedDestination );

    };

}}

#endif /*_ACTIVEMQ_COMMANDS_SUBSCRIPTIONINFO_H_*/
