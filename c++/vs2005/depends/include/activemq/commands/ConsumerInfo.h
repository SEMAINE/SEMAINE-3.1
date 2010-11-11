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

#ifndef _ACTIVEMQ_COMMANDS_CONSUMERINFO_H_
#define _ACTIVEMQ_COMMANDS_CONSUMERINFO_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/commands/ActiveMQDestination.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/commands/BooleanExpression.h>
#include <activemq/commands/BrokerId.h>
#include <activemq/commands/ConsumerId.h>
#include <activemq/util/Config.h>
#include <decaf/lang/Pointer.h>
#include <string>
#include <vector>

namespace activemq{
namespace commands{

    using decaf::lang::Pointer;

    /*
     *
     *  Command code for OpenWire format for ConsumerInfo
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API ConsumerInfo : public BaseCommand {
    protected:

        Pointer<ConsumerId> consumerId;
        bool browser;
        Pointer<ActiveMQDestination> destination;
        int prefetchSize;
        int maximumPendingMessageLimit;
        bool dispatchAsync;
        std::string selector;
        std::string subscriptionName;
        bool noLocal;
        bool exclusive;
        bool retroactive;
        unsigned char priority;
        std::vector< decaf::lang::Pointer<BrokerId> > brokerPath;
        Pointer<BooleanExpression> additionalPredicate;
        bool networkSubscription;
        bool optimizedAcknowledge;
        bool noRangeAcks;
        std::vector< decaf::lang::Pointer<ConsumerId> > networkConsumerPath;

    public:

        const static unsigned char ID_CONSUMERINFO = 5;

    protected:

        ConsumerInfo( const ConsumerInfo& ) : BaseCommand() {};
        ConsumerInfo& operator= ( const ConsumerInfo& ) { return *this; };

    public:

        ConsumerInfo();

        virtual ~ConsumerInfo();

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
        virtual ConsumerInfo* cloneDataStructure() const;

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

        virtual const Pointer<ConsumerId>& getConsumerId() const;
        virtual Pointer<ConsumerId>& getConsumerId();
        virtual void setConsumerId( const Pointer<ConsumerId>& consumerId );

        virtual bool isBrowser() const;
        virtual void setBrowser( bool browser );

        virtual const Pointer<ActiveMQDestination>& getDestination() const;
        virtual Pointer<ActiveMQDestination>& getDestination();
        virtual void setDestination( const Pointer<ActiveMQDestination>& destination );

        virtual int getPrefetchSize() const;
        virtual void setPrefetchSize( int prefetchSize );

        virtual int getMaximumPendingMessageLimit() const;
        virtual void setMaximumPendingMessageLimit( int maximumPendingMessageLimit );

        virtual bool isDispatchAsync() const;
        virtual void setDispatchAsync( bool dispatchAsync );

        virtual const std::string& getSelector() const;
        virtual std::string& getSelector();
        virtual void setSelector( const std::string& selector );

        virtual const std::string& getSubscriptionName() const;
        virtual std::string& getSubscriptionName();
        virtual void setSubscriptionName( const std::string& subscriptionName );

        virtual bool isNoLocal() const;
        virtual void setNoLocal( bool noLocal );

        virtual bool isExclusive() const;
        virtual void setExclusive( bool exclusive );

        virtual bool isRetroactive() const;
        virtual void setRetroactive( bool retroactive );

        virtual unsigned char getPriority() const;
        virtual void setPriority( unsigned char priority );

        virtual const std::vector< decaf::lang::Pointer<BrokerId> >& getBrokerPath() const;
        virtual std::vector< decaf::lang::Pointer<BrokerId> >& getBrokerPath();
        virtual void setBrokerPath( const std::vector< decaf::lang::Pointer<BrokerId> >& brokerPath );

        virtual const Pointer<BooleanExpression>& getAdditionalPredicate() const;
        virtual Pointer<BooleanExpression>& getAdditionalPredicate();
        virtual void setAdditionalPredicate( const Pointer<BooleanExpression>& additionalPredicate );

        virtual bool isNetworkSubscription() const;
        virtual void setNetworkSubscription( bool networkSubscription );

        virtual bool isOptimizedAcknowledge() const;
        virtual void setOptimizedAcknowledge( bool optimizedAcknowledge );

        virtual bool isNoRangeAcks() const;
        virtual void setNoRangeAcks( bool noRangeAcks );

        virtual const std::vector< decaf::lang::Pointer<ConsumerId> >& getNetworkConsumerPath() const;
        virtual std::vector< decaf::lang::Pointer<ConsumerId> >& getNetworkConsumerPath();
        virtual void setNetworkConsumerPath( const std::vector< decaf::lang::Pointer<ConsumerId> >& networkConsumerPath );

        /**
         * @return an answer of true to the isConsumerInfo() query.
         */
        virtual bool isConsumerInfo() const {
            return true;
        }

        /**
         * Allows a Visitor to visit this command and return a response to the
         * command based on the command type being visited.  The command will call
         * the proper processXXX method in the visitor.
         * 
         * @return a Response to the visitor being called or NULL if no response.
         */
        virtual Pointer<Command> visit( activemq::state::CommandVisitor* visitor )
            throw( exceptions::ActiveMQException );

    };

}}

#endif /*_ACTIVEMQ_COMMANDS_CONSUMERINFO_H_*/
