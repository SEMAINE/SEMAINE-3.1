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

#ifndef _ACTIVEMQ_WIREFORMAT_OPENWIRE_MARSAHAL_V3_JOURNALQUEUEACKMARSHALLER_H_
#define _ACTIVEMQ_WIREFORMAT_OPENWIRE_MARSAHAL_V3_JOURNALQUEUEACKMARSHALLER_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/wireformat/openwire/marshal/BaseDataStreamMarshaller.h>

#include <decaf/io/DataInputStream.h>
#include <decaf/io/DataOutputStream.h>
#include <decaf/io/IOException.h>
#include <activemq/util/Config.h>
#include <activemq/commands/DataStructure.h>
#include <activemq/wireformat/openwire/OpenWireFormat.h>
#include <activemq/wireformat/openwire/utils/BooleanStream.h>

namespace activemq{
namespace wireformat{
namespace openwire{
namespace marshal{
namespace v3{

    /**
     * Marshaling code for Open Wire Format for JournalQueueAckMarshaller
     *
     *  NOTE!: This file is auto generated - do not modify!
     *         if you need to make a change, please see the Java Classes
     *         in the activemq-openwire-generator module
     */
    class AMQCPP_API JournalQueueAckMarshaller : public BaseDataStreamMarshaller {
    public:

        JournalQueueAckMarshaller() {}
        virtual ~JournalQueueAckMarshaller() {}

        /**
         * Creates a new instance of this marshalable type.
         *
         * @return new DataStructure object pointer caller owns it.
         */
        virtual commands::DataStructure* createObject() const;

        /**
         * Get the Data Structure Type that identifies this Marshaler
         *
         * @return byte holding the data structure type value
         */
        virtual unsigned char getDataStructureType() const;

        /**
         * Un-marshal an object instance from the data input stream.
         *
         * @param wireFormat - describes the wire format of the broker.
         * @param dataStructure - Object to be un-marshaled.
         * @param dataIn - BinaryReader that provides that data.
         * @param bs - BooleanStream stream used to unpack bits from the wire.
         *
         * @throws IOException if an error occurs during the unmarshal.
         */
        virtual void tightUnmarshal( OpenWireFormat* wireFormat,
                                     commands::DataStructure* dataStructure,
                                     decaf::io::DataInputStream* dataIn,
                                     utils::BooleanStream* bs ) throw( decaf::io::IOException );

        /**
         * Write the booleans that this object uses to a BooleanStream
         *
         * @param wireFormat - describes the wire format of the broker
         * @param dataStructure - Object to be marshaled
         * @param bs - BooleanStream stream used to pack bits from the wire.
         * @returns int value indicating the size of the marshaled object.
         *
         * @throws IOException if an error occurs during the marshal.
         */
        virtual int tightMarshal1( OpenWireFormat* wireFormat,
                                   commands::DataStructure* dataStructure,
                                   utils::BooleanStream* bs ) throw( decaf::io::IOException );

        /**
         * Write a object instance to data output stream
         *
         * @param wireFormat - describes the wire format of the broker
         * @param dataStructure - Object to be marshaled
         * @param dataOut - BinaryReader that provides that data sink
         * @param bs - BooleanStream stream used to pack bits from the wire.
         *
         * @throws IOException if an error occurs during the marshal.
         */
        virtual void tightMarshal2( OpenWireFormat* wireFormat,
                                    commands::DataStructure* dataStructure,
                                    decaf::io::DataOutputStream* dataOut,
                                    utils::BooleanStream* bs ) throw( decaf::io::IOException );

        /**
         * Un-marshal an object instance from the data input stream
         *
         * @param wireFormat - describes the wire format of the broker
         * @param dataStructure - Object to be marshaled
         * @param dataIn - BinaryReader that provides that data source
         *
         * @throws IOException if an error occurs during the unmarshal.
         */
        virtual void looseUnmarshal( OpenWireFormat* wireFormat,
                                     commands::DataStructure* dataStructure,
                                     decaf::io::DataInputStream* dataIn ) throw( decaf::io::IOException );

        /**
         * Write a object instance to data output stream
         *
         * @param wireFormat - describs the wire format of the broker
         * @param dataStructure - Object to be marshaled
         * @param dataOut - BinaryWriter that provides that data sink
         *
         * @throws IOException if an error occurs during the marshal.
         */
        virtual void looseMarshal( OpenWireFormat* wireFormat,
                                   commands::DataStructure* dataStructure,
                                   decaf::io::DataOutputStream* dataOut ) throw( decaf::io::IOException );

    };

}}}}}

#endif /*_ACTIVEMQ_WIREFORMAT_OPENWIRE_MARSAHAL_V3_JOURNALQUEUEACKMARSHALLER_H_*/

