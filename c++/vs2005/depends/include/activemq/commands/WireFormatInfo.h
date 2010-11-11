/**
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

#ifndef _ACTIVEMQ_COMMANDS_WIREFORMATINFO_H_
#define _ACTIVEMQ_COMMANDS_WIREFORMATINFO_H_

#include <activemq/util/Config.h>
#include <activemq/commands/BaseCommand.h>
#include <activemq/util/PrimitiveMap.h>
#include <activemq/exceptions/ActiveMQException.h>

#include <vector>

namespace activemq{
namespace commands{

    class AMQCPP_API WireFormatInfo : public BaseCommand {
    private:

        std::vector<unsigned char> magic;
        std::vector<unsigned char> marshalledProperties;

        /**
         * WireFormatInfo Properties, unmarshaled from the marshaled
         * properties on use.
         */
        util::PrimitiveMap properties;

        /**
         * OpenWire Protocol Version
         */
        int version;

    public:

        const static unsigned char ID_WIREFORMATINFO = 1;

    public:

        WireFormatInfo();

        virtual ~WireFormatInfo();

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
        virtual DataStructure* cloneDataStructure() const;

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
        virtual bool equals( const DataStructure* value ) const;

        /**
         * Indicates that this command is aware of Marshaling, and needs
         * to have its Marshaling methods invoked.
         * @returns boolean indicating desire to be in marshaling stages
         */
        virtual bool isMarshalAware() const {
            return true;
        }

        /**
         * Allows a Visitor to visit this command and return a response to the
         * command based on the command type being visited.  The command will call
         * the proper processXXX method in the visitor.
         *
         * @return a Response to the visitor being called or NULL if no response.
         */
        virtual decaf::lang::Pointer<commands::Command> visit(
            activemq::state::CommandVisitor* visitor ) throw( exceptions::ActiveMQException );

        /**
         * Get the current Wireformat Version
         * @return int that identifies the version
         */
        int getVersion() const {
            return version;
        }

        /**
         * Set the current Wireformat Version
         * @param version - int that identifies the version
         */
        void setVersion( int version ) {
            this->version = version;
        }

        /**
         * Returns the currently configured Max Inactivity duration.
         * @return the set inactivity duration value.
         */
        long long getMaxInactivityDuration() const;

        /**
         * Sets the Max inactivity duration value.
         * @param maxInactivityDuration - max time a client can be inactive.
         */
        void setMaxInactivityDuration( long long maxInactivityDuration );

        /**
         * Returns the currently configured Max Inactivity Intial Delay duration.
         * @return the set inactivity duration initial delay value.
         */
        long long getMaxInactivityDurationInitalDelay() const;

        /**
         * Sets the Max inactivity initial delay duration value.
         * @param maxInactivityDurationInitalDelay - time before the inactivity delay is checked.
         */
        void setMaxInactivityDurationInitalDelay( long long maxInactivityDurationInitalDelay );

        /**
         * Checks if the stackTraceEnabled flag is on
         * @return true if the flag is on.
         */
        bool isStackTraceEnabled() const;

        /**
         * Sets if the stackTraceEnabled flag is on
         * @param stackTraceEnabled - ture to turn flag is on
         */
        void setStackTraceEnabled( bool stackTraceEnabled );

        /**
         * Checks if the tcpNoDelayEnabled flag is on
         * @return true if the flag is on.
         */
        bool isTcpNoDelayEnabled() const;

        /**
         * Sets if the tcpNoDelayEnabled flag is on
         * @param tcpNoDelayEnabled - ture to turn flag is on
         */
        void setTcpNoDelayEnabled( bool tcpNoDelayEnabled );

        /**
         * Checks if the cacheEnabled flag is on
         * @return true if the flag is on.
         */
        bool isCacheEnabled() const;

        /**
         * Sets if the cacheEnabled flag is on
         * @param cacheEnabled - true to turn flag is on
         */
        void setCacheEnabled( bool cacheEnabled );

        /**
         * Gets the Cache Size setting.
         * @return currently set cache size.
         */
        int getCacheSize() const;

        /**
         * Sets the Cache Size setting.
         * @param value - value to set to the cache size.
         */
        void setCacheSize( int value );

        /**
         * Checks if the tightEncodingEnabled flag is on
         * @return true if the flag is on.
         */
        bool isTightEncodingEnabled() const;

        /**
         * Sets if the tightEncodingEnabled flag is on
         * @param tightEncodingEnabled - true to turn flag is on
         */
        void setTightEncodingEnabled( bool tightEncodingEnabled );

        /**
         * Checks if the sizePrefixDisabled flag is on
         * @return true if the flag is on.
         */
        bool isSizePrefixDisabled() const;

        /**
         * Sets if the sizePrefixDisabled flag is on
         * @param sizePrefixDisabled - true to turn flag is on
         */
        void setSizePrefixDisabled( bool sizePrefixDisabled );

        /**
         * Get the Magic field
         * @return const reference to a std::vector<char>
         */
        const std::vector<unsigned char>& getMagic() const {
            return magic;
        }

        /**
         * Sets the value of the magic field
         * @param magic - const std::vector<char>
         */
        void setMagic( const std::vector<unsigned char>& magic ) {
            this->magic = magic;
        }

        /**
         * Get the marshalledProperties field
         * @return const reference to a std::vector<char>
         */
        const std::vector<unsigned char>& getMarshalledProperties() const {
            return marshalledProperties;
        }

        /**
         * Sets the value of the marshalledProperties field
         * @param marshalledProperties
         *      The Byte Array vector that contains the marshaled form of the Message
         *      properties, this is the data sent over the wire.
         */
        void setMarshalledProperties( const std::vector<unsigned char>& marshalledProperties ) {
            this->marshalledProperties = marshalledProperties;
        }

        /**
         * Gets the Properties for this Command
         * @return the Properties object for this Command.
         */
        virtual const util::PrimitiveMap& getProperties() const {
            return properties;
        }

        /**
         * Gets the Properties for this Command
         * @return the Properties object for this Command.
         */
        virtual util::PrimitiveMap& getProperties() {
            return properties;
        }

        /**
         * Sets the Properties for this Command
         * @param map - PrimtiveMap to copy
         */
        virtual void setProperties( const util::PrimitiveMap& map ) {
            this->properties.copy( map );
        }

        /**
         * Determines if we think this is a Valid WireFormatInfo command
         * @returns true if its valid.
         */
        bool isValid() const;

        /**
         * @returns answers true to the isWireFormatInfo query
         */
        virtual bool isWireFormatInfo() const {
            return true;
        }

    public:

        /**
         * Handles the marshaling of the objects properties into the
         * internal byte array before the object is marshalled to the
         * wire
         * @param wireFormat - the wire formatting controller
         */
        virtual void beforeMarshal( wireformat::WireFormat* wireFormat AMQCPP_UNUSED )
            throw ( decaf::io::IOException );

        /**
         * Called after unmarshaling is started to cleanup the object being
         * unmarshaled.
         * @param wireFormat - the wireformat object to control unmarshaling
         */
        virtual void afterUnmarshal( wireformat::WireFormat* wireFormat AMQCPP_UNUSED )
            throw ( decaf::io::IOException );

    };

}}

#endif /*_ACTIVEMQ_COMMANDS_WIREFORMATINFO_H_*/
