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

#ifndef _ACTIVEMQ_COMMANDS_ACTIVEMQBYTESMESSAGE_H_
#define _ACTIVEMQ_COMMANDS_ACTIVEMQBYTESMESSAGE_H_

// Turn off warning message for ignored exception specification
#ifdef _MSC_VER
#pragma warning( disable : 4290 )
#endif

#include <activemq/util/Config.h>
#include <activemq/commands/ActiveMQMessageTemplate.h>
#include <decaf/io/ByteArrayOutputStream.h>
#include <decaf/io/DataInputStream.h>
#include <decaf/io/DataOutputStream.h>
#include <cms/BytesMessage.h>
#include <vector>
#include <string>
#include <memory>

namespace activemq{
namespace commands{

    class AMQCPP_API ActiveMQBytesMessage :
        public ActiveMQMessageTemplate< cms::BytesMessage > {
    private:

        /**
         * OutputStream that wraps around the command's content when in
         * write-only mode.
         */
        std::auto_ptr<decaf::io::ByteArrayOutputStream> bytesOut;

        /**
         * DataInputStream wrapper around the input stream.
         */
        mutable std::auto_ptr<decaf::io::DataInputStream> dataIn;

        /**
         * DataOutputStream wrapper around the output stream.
         */
        std::auto_ptr<decaf::io::DataOutputStream> dataOut;

        /**
         * Tracks the actual length of the Message when compressed.
         */
        mutable std::size_t length;

    public:

        const static unsigned char ID_ACTIVEMQBYTESMESSAGE = 24;

    public:

        ActiveMQBytesMessage();
        virtual ~ActiveMQBytesMessage();

        virtual unsigned char getDataStructureType() const;

        /**
         * Clone this object and return a new instance that the
         * caller now owns, this will be an exact copy of this one
         * @returns new copy of this object.
         */
        virtual ActiveMQBytesMessage* cloneDataStructure() const;

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
         *
         * @param value
         *      The Command to compare to this one.
         *
         * @returns true if DataStructure's are Equal.
         */
        virtual bool equals( const DataStructure* value ) const;

    public:   // CMS Message

        /**
         * Clone this message exactly, returns a new instance that the
         * caller is required to delete.
         * @return new copy of this message
         */
        virtual cms::BytesMessage* clone() const {
            return dynamic_cast<cms::BytesMessage*>( this->cloneDataStructure() );
        }

        /**
         * Clears out the body of the message.  This does not clear the
         * headers or properties.
         */
        virtual void clearBody() throw( cms::CMSException );

        /**
         * Store the Data that was written to the stream before a send
         */
        virtual void onSend();

    public:   // CMS BytesMessage

        /**
         * sets the bytes given to the message body.
         *
         * @param buffer
         *      Byte Buffer to copy
         * @param numBytes
         *      Number of bytes in Buffer to copy
         *
         * @throws CMSException - If an internal error occurs.
         * @throws MessageNotWriteableException - if in Read Only Mode.
         */
        virtual void setBodyBytes( const unsigned char* buffer, std::size_t numBytes )
            throw( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Gets the bytes that are contained in this message, user should
         * copy this data into a user allocated buffer.  Call
         * <code>getBodyLength</code> to determine the number of bytes
         * to expect.
         *
         * @return const pointer to a byte buffer
         *
         * @throws CMSException - If an internal error occurs.
         * @throws MessageNotReadableException - If the message is in Write Only Mode.
         */
        virtual unsigned char* getBodyBytes() const
            throw( cms::MessageNotReadableException, cms::CMSException );

        /**
         * Returns the number of bytes contained in the body of this message.
         *
         * @return number of bytes.
         *
         * @throws CMSException - If an internal error occurs.
         * @throws MessageNotReadableException - If the message is in Write Only Mode.
         */
        virtual std::size_t getBodyLength() const
            throw( cms::MessageNotReadableException, cms::CMSException );

        /**
         * Puts the message body in read-only mode and repositions the stream
         * of bytes to the beginning.
         *
         * @throws CMSException - If the provider fails to perform the reset operation.
         * @throws MessageFormatException - If the Message has an invalid format.
         */
        virtual void reset() throw ( cms::MessageFormatException, cms::CMSException );

        /**
         * Reads a Boolean from the Bytes message stream
         * @returns boolean value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual bool readBoolean() const throw ( cms::MessageEOFException,
                                                 cms::MessageNotReadableException,
                                                 cms::CMSException );

        /**
         * Writes a boolean to the bytes message stream as a 1-byte value.
         * The value true is written as the value (byte)1; the value false
         * is written as the value (byte)0.
         *
         * @param value
         *      boolean to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeBoolean( bool value ) throw ( cms::MessageNotWriteableException,
                                                        cms::CMSException );

        /**
         * Reads a Byte from the Bytes message stream
         *
         * @returns unsigned char value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual unsigned char readByte() const throw ( cms::MessageEOFException,
                                                       cms::MessageNotReadableException,
                                                       cms::CMSException );

        /**
         * Writes a byte to the bytes message stream as a 1-byte value
         *
         * @param value
         *      byte to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeByte( unsigned char value ) throw ( cms::MessageNotWriteableException,
                                                              cms::CMSException );

        /**
         * Reads a byte array from the bytes message stream.
         *
         * If the length of vector value is less than the number of bytes
         * remaining to be read from the stream, the vector should be filled. A
         * subsequent call reads the next increment, and so on.
         *
         * If the number of bytes remaining in the stream is less than the
         * length of vector value, the bytes should be read into the vector. The
         * return value of the total number of bytes read will be less than the
         * length of the vector, indicating that there are no more bytes left to
         * be read from the stream. The next read of the stream returns -1.
         *
         * @param value
         *      buffer to place data in
         * @returns the total number of bytes read into the buffer, or -1 if
         *          there is no more data because the end of the stream has
         *          been reached
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual std::size_t readBytes( std::vector<unsigned char>& value ) const
            throw ( cms::MessageEOFException,
                    cms::MessageNotReadableException,
                    cms::CMSException );

        /**
         * Writes a byte array to the bytes message stream using the vector
         * size as the number of bytes to write.
         *
         * @param value
         *      bytes to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeBytes( const std::vector<unsigned char>& value )
            throw ( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Reads a portion of the bytes message stream.
         *
         * If the length of array value is less than the number of bytes
         * remaining to be read from the stream, the array should be filled. A
         * subsequent call reads the next increment, and so on.
         *
         * If the number of bytes remaining in the stream is less than the
         * length of array value, the bytes should be read into the array. The
         * return value of the total number of bytes read will be less than the
         * length of the array, indicating that there are no more bytes left to
         * be read from the stream. The next read of the stream returns -1.
         *
         * If length is negative, or length is greater than the length of the
         * array value, then an IndexOutOfBoundsException is thrown. No bytes
         * will be read from the stream for this exception case.
         *
         * @param buffer
         *      the buffer into which the data is read
         * @param length
         *      the number of bytes to read; must be less than or equal to
         *      value.length
         * @returns the total number of bytes read into the buffer, or -1 if
         *          there is no more data because the end of the stream has
         *          been reached
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual std::size_t readBytes( unsigned char* buffer, std::size_t length ) const
            throw ( cms::MessageEOFException,
                    cms::MessageNotReadableException,
                    cms::CMSException );

        /**
         * Writes a portion of a byte array to the bytes message stream.
         * size as the number of bytes to write.
         *
         * @param value
         *      bytes to write to the stream
         * @param offset
         *      the initial offset within the byte array
         * @param length
         *      the number of bytes to use
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeBytes( const unsigned char* value, std::size_t offset, std::size_t length )
            throw ( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Reads a Char from the Bytes message stream
         *
         * @returns char value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual char readChar() const throw ( cms::MessageEOFException,
                                              cms::MessageNotReadableException,
                                              cms::CMSException );

        /**
         * Writes a char to the bytes message stream as a 1-byte value
         *
         * @param value
         *      char to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeChar( char value ) throw ( cms::MessageNotWriteableException,
                                                     cms::CMSException );

        /**
         * Reads a 32 bit float from the Bytes message stream
         *
         * @returns double value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual float readFloat() const throw ( cms::MessageEOFException,
                                                cms::MessageNotReadableException,
                                                cms::CMSException );

        /**
         * Writes a float to the bytes message stream as a 4 byte value
         * @param value
         *      float to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeFloat( float value ) throw ( cms::MessageNotWriteableException,
                                                       cms::CMSException );

        /**
         * Reads a 64 bit double from the Bytes message stream
         *
         * @returns double value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual double readDouble() const throw ( cms::MessageEOFException,
                                                  cms::MessageNotReadableException,
                                                  cms::CMSException );

        /**
         * Writes a double to the bytes message stream as a 8 byte value
         * @param value
         *      double to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeDouble( double value ) throw ( cms::MessageNotWriteableException,
                                                         cms::CMSException );

        /**
         * Reads a 16 bit signed short from the Bytes message stream
         *
         * @returns short value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual short readShort() const throw ( cms::MessageEOFException,
                                                cms::MessageNotReadableException,
                                                cms::CMSException );

        /**
         * Writes a signed short to the bytes message stream as a 2 byte value
         *
         * @param value
         *      signed short to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeShort( short value ) throw ( cms::MessageNotWriteableException,
                                                       cms::CMSException );

        /**
         * Reads a 16 bit unsigned short from the Bytes message stream
         *
         * @returns unsigned short value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual unsigned short readUnsignedShort() const throw ( cms::MessageEOFException,
                                                                 cms::MessageNotReadableException,
                                                                 cms::CMSException );

        /**
         * Writes a unsigned short to the bytes message stream as a 2 byte value
         *
         * @param value
         *      unsigned short to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeUnsignedShort( unsigned short value )
            throw ( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Reads a 32 bit signed integer from the Bytes message stream
         *
         * @returns int value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual int readInt() const throw ( cms::MessageEOFException,
                                            cms::MessageNotReadableException,
                                            cms::CMSException );

        /**
         * Writes a signed int to the bytes message stream as a 4 byte value
         *
         * @param value
         *      signed int to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeInt( int value ) throw ( cms::MessageNotWriteableException,
                                                   cms::CMSException );

        /**
         * Reads a 64 bit long from the Bytes message stream
         *
         * @returns long long value from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual long long readLong() const throw ( cms::MessageEOFException,
                                                   cms::MessageNotReadableException,
                                                   cms::CMSException );

        /**
         * Writes a long long to the bytes message stream as a 8 byte value
         *
         * @param value
         *      signed long long to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeLong( long long value ) throw ( cms::MessageNotWriteableException,
                                                          cms::CMSException );

        /**
         * Reads an ASCII String from the Bytes message stream
         *
         * @returns String from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual std::string readString() const throw ( cms::MessageEOFException,
                                                       cms::MessageNotReadableException,
                                                       cms::CMSException );

        /**
         * Writes an ASCII String to the Bytes message stream
         *
         * @param value
         *      String to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to write the message due to
         *                        some internal error.
         * @throws MessageNotWriteableException - if the message is in read-only mode.
         */
        virtual void writeString( const std::string& value )
            throw ( cms::MessageNotWriteableException, cms::CMSException );

        /**
         * Reads an UTF String from the BytesMessage stream
         *
         * @returns String from stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual std::string readUTF() const throw ( cms::MessageEOFException,
                                                    cms::MessageNotReadableException,
                                                    cms::CMSException );

        /**
         * Writes an UTF String to the BytesMessage stream
         *
         * @param value
         *      String to write to the stream
         *
         * @throws CMSException - if the CMS provider fails to read the message due to
         *                        some internal error.
         * @throws MessageEOFException - if unexpected end of bytes stream has been reached.
         * @throws MessageNotReadableException - if the message is in write-only mode.
         */
        virtual void writeUTF( const std::string& value )
            throw ( cms::MessageNotWriteableException, cms::CMSException );

    private:

        void storeContent();

        void initializeReading() const;

        void initializeWriting();

    };

}}

#endif /*_ACTIVEMQ_COMMANDS_ACTIVEMQBYTESMESSAGE_H_*/
