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

#ifndef _ACTIVEMQ_COMMANDS_ACTIVEMQMAPMESSAGE_H_
#define _ACTIVEMQ_COMMANDS_ACTIVEMQMAPMESSAGE_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/util/Config.h>
#include <activemq/commands/ActiveMQMessageTemplate.h>
#include <activemq/util/PrimitiveMap.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <cms/MapMessage.h>
#include <vector>
#include <string>
#include <memory>

namespace activemq{
namespace commands{

    class AMQCPP_API ActiveMQMapMessage :
        public ActiveMQMessageTemplate<cms::MapMessage> {

    private:

        // Map Structure to hold unmarshaled Map Data
        mutable std::auto_ptr<util::PrimitiveMap> map;

    public:

        const static unsigned char ID_ACTIVEMQMAPMESSAGE = 25;

    public:

        ActiveMQMapMessage();
        virtual ~ActiveMQMapMessage();

        virtual unsigned char getDataStructureType() const;

        /**
         * Determine if this object is aware of marshaling and should have
         * its before and after marshaling methods called.  Defaults to false.
         * @returns true if aware of marshaling
         */
        virtual bool isMarshalAware() const {
            return true;
        }

        /**
         * Clone this object and return a new instance that the
         * caller now owns, this will be an exact copy of this one
         * @returns new copy of this object.
         */
        virtual ActiveMQMapMessage* cloneDataStructure() const;

        /**
         * Copy the contents of the passed object into this objects
         * members, overwriting any existing data.
         * @return src - Source Object
         */
        virtual void copyDataStructure( const DataStructure* src );

        /**
         * Perform any processing needed before an marshal
         * @param wireFormat - the OpenWireFormat object in use.
         */
        virtual void beforeMarshal( wireformat::WireFormat* wireFormat )
            throw ( decaf::io::IOException );

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
         * Clears out the body of the message.  This does not clear the
         * headers or properties.
         */
        virtual void clearBody() throw( cms::CMSException );

    public:   // CMS Message

        /**
         * Clone this message exactly, returns a new instance that the
         * caller is required to delete.
         * @return new copy of this message
         */
        virtual cms::MapMessage* clone() const {
            return dynamic_cast<cms::MapMessage*>( this->cloneDataStructure() );
        }

    public:   // CMS MapMessage

        /**
         * Returns an Enumeration of all the names in the MapMessage
         * object.
         *
         * @return STL Vector of String values, each of which is the
         *         name of an item in the MapMessage
         *
         * @throws CMSException - if the operation fails due to an internal error.
         */
        virtual std::vector< std::string > getMapNames() const throw( cms::CMSException );

        /**
         * Indicates whether an item exists in this MapMessage object.
         *
         * @param name
         *      String name of the Object in question
         * @return boolean value indicating if the name is in the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         */
        virtual bool itemExists( const std::string& name ) const throw( cms::CMSException );

        /**
         * Returns the Boolean value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatExceptio - if this type conversion is invalid.
         */
        virtual bool getBoolean( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a boolean value with the specified name into the Map.
         *
         * @param name
         *      the name of the boolean
         * @param value
         *      the boolean value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setBoolean( const std::string& name, bool value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Byte value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual unsigned char getByte( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Byte value with the specified name into the Map.
         *
         * @param name
         *      the name of the Byte
         * @param value
         *      the Byte value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setByte( const std::string& name, unsigned char value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Bytes value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual std::vector<unsigned char> getBytes( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Bytes value with the specified name into the Map.
         *
         * @param name
         *      The name of the Bytes
         * @param value
         *      The Bytes value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setBytes( const std::string& name,
                               const std::vector<unsigned char>& value )
                                    throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Char value of the Specified name
         *
         * @param name
         *      name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual char getChar( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Char value with the specified name into the Map.
         *
         * @param name
         *      the name of the Char
         * @param value
         *      the Char value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setChar( const std::string& name, char value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Double value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual double getDouble( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Double value with the specified name into the Map.
         *
         * @param name
         *      The name of the Double
         * @param value
         *      The Double value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setDouble( const std::string& name, double value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Float value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual float getFloat( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Float value with the specified name into the Map.
         *
         * @param name
         *      The name of the Float
         * @param value
         *      The Float value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setFloat( const std::string& name, float value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Int value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual int getInt( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Int value with the specified name into the Map.
         *
         * @param name
         *      The name of the Int
         * @param value
         *      The Int value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setInt( const std::string& name, int value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Long value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual long long getLong( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Long value with the specified name into the Map.
         *
         * @param name
         *      The name of the Long
         * @param value
         *      The Long value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setLong( const std::string& name, long long value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the Short value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual short getShort( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a Short value with the specified name into the Map.
         *
         * @param name
         *      The name of the Short
         * @param value
         *      The Short value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setShort( const std::string& name, short value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Returns the String value of the Specified name
         *
         * @param name
         *      Name of the value to fetch from the map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageFormatException - if this type conversion is invalid.
         */
        virtual std::string getString( const std::string& name ) const
            throw( cms::MessageFormatException, cms::CMSException );

        /**
         * Sets a String value with the specified name into the Map.
         *
         * @param name
         *      The name of the String
         * @param value
         *      The String value to set in the Map
         *
         * @throws CMSException - if the operation fails due to an internal error.
         * @throws MessageNotWriteableException - if the Message is in Read-only Mode.
         */
        virtual void setString( const std::string& name, const std::string& value )
            throw( cms::MessageNotWriteableException, cms::CMSException );

    protected:

        /**
         * Fetches a reference to this objects PrimitiveMap, if one needs
         * to be created or unmarshaled, this will perform the correct steps.
         * @returns reference to a PrimtiveMap.
         */
        util::PrimitiveMap& getMap() throw ( decaf::lang::exceptions::NullPointerException );
        const util::PrimitiveMap& getMap() const throw ( decaf::lang::exceptions::NullPointerException );

        /**
         * Performs the unmarshal on the Map if needed, otherwise just returns
         */
        virtual void checkMapIsUnmarshalled() const
            throw ( decaf::lang::exceptions::NullPointerException );

    };

}}

#endif /*_ACTIVEMQ_COMMANDS_ACTIVEMQMAPMESSAGE_H_*/
