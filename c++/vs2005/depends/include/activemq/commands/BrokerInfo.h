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

#ifndef _ACTIVEMQ_COMMANDS_BROKERINFO_H_
#define _ACTIVEMQ_COMMANDS_BROKERINFO_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/commands/BaseCommand.h>
#include <activemq/commands/BrokerId.h>
#include <activemq/commands/BrokerInfo.h>
#include <activemq/util/Config.h>
#include <decaf/lang/Pointer.h>
#include <string>
#include <vector>

namespace activemq{
namespace commands{

    using decaf::lang::Pointer;

    /*
     *
     *  Command code for OpenWire format for BrokerInfo
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-cpp-openwire-generator module
     *
     */
    class AMQCPP_API BrokerInfo : public BaseCommand {
    protected:

        Pointer<BrokerId> brokerId;
        std::string brokerURL;
        std::vector< decaf::lang::Pointer<BrokerInfo> > peerBrokerInfos;
        std::string brokerName;
        bool slaveBroker;
        bool masterBroker;
        bool faultTolerantConfiguration;
        bool duplexConnection;
        bool networkConnection;
        long long connectionId;
        std::string brokerUploadUrl;
        std::string networkProperties;

    public:

        const static unsigned char ID_BROKERINFO = 2;

    protected:

        BrokerInfo( const BrokerInfo& ) : BaseCommand() {};
        BrokerInfo& operator= ( const BrokerInfo& ) { return *this; };

    public:

        BrokerInfo();

        virtual ~BrokerInfo();

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
        virtual BrokerInfo* cloneDataStructure() const;

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

        virtual const Pointer<BrokerId>& getBrokerId() const;
        virtual Pointer<BrokerId>& getBrokerId();
        virtual void setBrokerId( const Pointer<BrokerId>& brokerId );

        virtual const std::string& getBrokerURL() const;
        virtual std::string& getBrokerURL();
        virtual void setBrokerURL( const std::string& brokerURL );

        virtual const std::vector< decaf::lang::Pointer<BrokerInfo> >& getPeerBrokerInfos() const;
        virtual std::vector< decaf::lang::Pointer<BrokerInfo> >& getPeerBrokerInfos();
        virtual void setPeerBrokerInfos( const std::vector< decaf::lang::Pointer<BrokerInfo> >& peerBrokerInfos );

        virtual const std::string& getBrokerName() const;
        virtual std::string& getBrokerName();
        virtual void setBrokerName( const std::string& brokerName );

        virtual bool isSlaveBroker() const;
        virtual void setSlaveBroker( bool slaveBroker );

        virtual bool isMasterBroker() const;
        virtual void setMasterBroker( bool masterBroker );

        virtual bool isFaultTolerantConfiguration() const;
        virtual void setFaultTolerantConfiguration( bool faultTolerantConfiguration );

        virtual bool isDuplexConnection() const;
        virtual void setDuplexConnection( bool duplexConnection );

        virtual bool isNetworkConnection() const;
        virtual void setNetworkConnection( bool networkConnection );

        virtual long long getConnectionId() const;
        virtual void setConnectionId( long long connectionId );

        virtual const std::string& getBrokerUploadUrl() const;
        virtual std::string& getBrokerUploadUrl();
        virtual void setBrokerUploadUrl( const std::string& brokerUploadUrl );

        virtual const std::string& getNetworkProperties() const;
        virtual std::string& getNetworkProperties();
        virtual void setNetworkProperties( const std::string& networkProperties );

        /**
         * @return an answer of true to the isBrokerInfo() query.
         */
        virtual bool isBrokerInfo() const {
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

#endif /*_ACTIVEMQ_COMMANDS_BROKERINFO_H_*/
