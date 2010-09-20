/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _DECAF_NIO_BYTEBUFFER_H_
#define _DECAF_NIO_BYTEBUFFER_H_

#include <decaf/nio/Buffer.h>
#include <decaf/lang/Comparable.h>
#include <decaf/lang/exceptions/NullPointerException.h>
#include <decaf/lang/exceptions/IndexOutOfBoundsException.h>
#include <decaf/nio/BufferUnderflowException.h>
#include <decaf/nio/BufferOverflowException.h>
#include <decaf/nio/ReadOnlyBufferException.h>
#include <decaf/internal/nio/ByteArrayPerspective.h>

namespace decaf{
namespace nio{

	class CharBuffer;
	class DoubleBuffer;
	class FloatBuffer;
	class ShortBuffer;
	class IntBuffer;
	class LongBuffer;

	/**
	 * This class defines six categories of operations upon byte buffers:
	 *
	 *  1. Absolute and relative get and put methods that read and write single bytes;
	 *  2. Relative bulk get methods that transfer contiguous sequences of bytes from
	 *     this buffer into an array;
	 *  3. Relative bulk put methods that transfer contiguous sequences of bytes from
	 *     a byte array or some other byte buffer into this buffer;
	 *  4. Absolute and relative get and put methods that read and write values of other
	 *     primitive types, translating them to and from sequences of bytes in a
	 *     particular byte order;
	 *  5. Methods for creating view buffers, which allow a byte buffer to be viewed as
	 *     a buffer containing values of some other primitive type; and
	 *  6. Methods for compacting, duplicating, and slicing a byte buffer.
	 *
	 * Byte buffers can be created either by allocation, which allocates space for the
	 * buffer's content, or by wrapping an existing byte array into a buffer.
	 *
	 * Access to binary data:
	 *
	 * This class defines methods for reading and writing values of all other primitive
	 * types, except boolean. Primitive values are translated to (or from) sequences of
	 * bytes according to the buffer's current byte order.
	 *
	 * For access to heterogeneous binary data, that is, sequences of values of
	 * different types, this class defines a family of absolute and relative get and
	 * put methods for each type. For 32-bit floating-point values, for example, this
	 * class defines:
	 *
	 *   float getFloat()
	 *   float getFloat(int index)
	 *   void  putFloat(float f)
	 *   void  putFloat(int index, float f)
	 *
	 * Corresponding methods are defined for the types char, short, int, long, and
	 * double. The index parameters of the absolute get and put methods are in terms
	 * of bytes rather than of the type being read or written.
	 *
	 * For access to homogeneous binary data, that is, sequences of values of the same
	 * type, this class defines methods that can create views of a given byte buffer.
	 * A view buffer is simply another buffer whose content is backed by the byte buffer.
	 * Changes to the byte buffer's content will be visible in the view buffer, and vice
	 * versa; the two buffers' position, limit, and mark values are independent. The
	 * asFloatBuffer method, for example, creates an instance of the FloatBuffer class
	 * that is backed by the byte buffer upon which the method is invoked. Corresponding
	 * view-creation methods are defined for the types char, short, int, long, and double.
	 *
	 * View buffers have two important advantages over the families of type-specific
	 * get and put methods described above:
	 *
	 *   A view buffer is indexed not in terms of bytes but rather in terms of the
	 *   type-specific size of its values;
	 *
	 *   A view buffer provides relative bulk get and put methods that can transfer
	 *   contiguous sequences of values between a buffer and an array or some other
	 *   buffer of the same type; and
	 *
	 */
	class DECAF_API ByteBuffer : public Buffer,
								 public lang::Comparable<ByteBuffer> {
	protected:

		/**
		 * Creates a ByteBuffer object that has its backing array allocated internally
		 * and is then owned and deleted when this object is deleted.  The array is
		 * initially created with all elements initialized to zero.
		 * @param capacity - size of the array, this is the limit we read and write to.
		 */
		ByteBuffer( std::size_t capacity );

	public:

		virtual ~ByteBuffer() {}

		/**
		 * @returns a std::string describing this object
		 */
		virtual std::string toString() const;

		/**
		 * Relative bulk get method.
		 * <p>
		 * This method transfers bytes from this buffer into the given destination
		 * vector. An invocation of this method of the form src.get(a) behaves in
		 * exactly the same way as the invocation.  The vector must be sized to the
		 * amount of data that is to be read, that is to say, the caller should call
		 * buffer.resize( N ) before calling this get method.
		 * @returns a reference to this Byte Buffer
		 * @throws BufferUnderflowException - If there are fewer than length bytes
		 * remaining in this buffer
		 */
		ByteBuffer& get( std::vector<unsigned char> buffer )
			throw ( BufferUnderflowException );

		/**
		 * Relative bulk get method.
		 * <p>
		 * This method transfers bytes from this buffer into the given destination array.
		 * If there are fewer bytes remaining in the buffer than are required to satisfy
		 * the request, that is, if length > remaining(), then no bytes are transferred
		 * and a BufferUnderflowException is thrown.
		 * <p>
		 * Otherwise, this method copies length bytes from this buffer into the given
		 * array, starting at the current position of this buffer and at the given offset
		 * in the array. The position of this buffer is then incremented by length.
		 * <p>
		 * @param buffer - pointer to an allocated buffer to fill
		 * @param offset - position in the buffer to start filling
		 * @param length - amount of data to put in the passed buffer
		 * @returns a reference to this Buffer
		 * @throws BufferUnderflowException - If there are fewer than length bytes
		 * remaining in this buffer
		 * @throws NullPointerException if the passed buffer is null.
		 */
		ByteBuffer& get( unsigned char* buffer,
						 std::size_t offset,
						 std::size_t length )
			throw( BufferUnderflowException,
				   lang::exceptions::NullPointerException );

		/**
		 * This method transfers the bytes remaining in the given source buffer into
		 * this buffer. If there are more bytes remaining in the source buffer than in
		 * this buffer, that is, if src.remaining() > remaining(), then no bytes are
		 * transferred and a BufferOverflowException is thrown.
		 * <p>
		 * Otherwise, this method copies n = src.remaining() bytes from the given
		 * buffer into this buffer, starting at each buffer's current position. The
		 * positions of both buffers are then incremented by n.
		 * @param src - the buffer to take bytes from an place in this one.
		 * @returns a reference to this buffer
		 * @throws BufferOverflowException - If there is insufficient space in this
		 * buffer for the remaining bytes in the source buffer
		 * @throws IllegalArgumentException - If the source buffer is this buffer
		 * @throws ReadOnlyBufferException - If this buffer is read-only
		 */
		ByteBuffer& put( ByteBuffer& src )
			throw( BufferOverflowException, ReadOnlyBufferException,
				   lang::exceptions::IllegalArgumentException );

		/**
		 * This method transfers bytes into this buffer from the given source array.
		 * If there are more bytes to be copied from the array than remain in this buffer,
		 * that is, if length > remaining(), then no bytes are transferred and a
		 * BufferOverflowException is thrown.
		 * <p>
		 * Otherwise, this method copies length bytes from the given array into this
		 * buffer, starting at the given offset in the array and at the current position
		 * of this buffer. The position of this buffer is then incremented by length.
		 * @param buffer - The array from which bytes are to be read
		 * @param offset - The offset within the array of the first byte to be read;
		 * @param length - The number of bytes to be read from the given array
		 * @returns a reference to this buffer
		 * @throws BufferOverflowException - If there is insufficient space in this buffer
		 * @throws ReadOnlyBufferException - If this buffer is read-only
		 * @throws NullPointerException if the passed buffer is null.
		 */
		ByteBuffer& put( const unsigned char* buffer,
						 std::size_t offset,
						 std::size_t length )
			throw( BufferOverflowException, ReadOnlyBufferException,
				   lang::exceptions::NullPointerException );

		/**
		 * This method transfers the entire content of the given source byte array into
		 * this buffer.  This is the same as calling put( &buffer[0], 0, buffer.size()
		 * @pparam buffer - The buffer whose contents are copied to this ByteBuffer
		 * @returns a reference to this buffer
		 * @throws BufferOverflowException - If there is insufficient space in this buffer
		 * @throws ReadOnlyBufferException - If this buffer is read-only
		 */
		ByteBuffer& put( std::vector<unsigned char>& buffer )
			throw( BufferOverflowException, ReadOnlyBufferException );

	public:   // Abstract Methods

		/**
		 * Tells whether or not this buffer is read-only.
		 * @returns true if, and only if, this buffer is read-only
		 */
		virtual bool isReadOnly() const = 0;

		/**
		 * Returns the byte array that backs this buffer
		 * <p>
		 * Modifications to this buffer's content will cause the returned array's
		 * content to be modified, and vice versa.
		 * <p>
		 * Invoke the hasArray method before invoking this method in order to ensure
		 * that this buffer has an accessible backing array.
		 * @returns The array that backs this buffer
		 * @throws ReadOnlyBufferException - If this buffer is backed by an array but
		 * is read-only
		 * @throws UnsupportedOperationException - If this buffer is not backed by an
		 * accessible array
		 */
		virtual unsigned char* array()
			throw( ReadOnlyBufferException,
				   lang::exceptions::UnsupportedOperationException ) = 0;

		/**
		 * Returns the offset within this buffer's backing array of the first element
		 * of the buffer.
		 * <p>
		 * If this buffer is backed by an array then buffer position p corresponds to
		 * array index p + arrayOffset().
		 * <p>
		 * Invoke the hasArray method before invoking this method in order to ensure
		 * that this buffer has an accessible backing array.
		 * @returns The offset within this buffer's array of the first element of
		 * the buffer
		 * @throws ReadOnlyBufferException - If this buffer is backed by an array but
		 * is read-only
		 * @throws UnsupportedOperationException - If this buffer is not backed by an
		 * accessible array
		 */
		virtual std::size_t arrayOffset() const
			throw( ReadOnlyBufferException,
				   lang::exceptions::UnsupportedOperationException ) = 0;

		/**
		 * Tells whether or not this buffer is backed by an accessible byte array.
		 * If this method returns true then the array and arrayOffset methods may safely
		 * be invoked.  Subclasses should override this method if they do not have a
		 * backing array as this class always returns true.
		 * @returns true if, and only if, this buffer is backed by an array and is not
		 * read-only
		 */
		virtual bool hasArray() const = 0;

		/**
		 * Creates a view of this byte buffer as a char buffer.
		 * <p>
		 * The content of the new buffer will start at this buffer's current position.
		 * Changes to this buffer's content will be visible in the new buffer, and vice
		 * versa; the two buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be
		 * the number of bytes remaining in this buffer, and its mark will be undefined.
		 * The new buffer will be read-only if, and only if, this buffer is read-only.
		 * @returns the new Char Buffer, which the caller then owns.
		 */
		virtual CharBuffer* asCharBuffer() const = 0;

		/**
		 * Creates a view of this byte buffer as a double buffer.
		 * <p>
		 * The content of the new buffer will start at this buffer's current position.
		 * Changes to this buffer's content will be visible in the new buffer, and vice
		 * versa; the two buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be
		 * the number of bytes remaining in this buffer divided by eight, and its mark
		 * will be undefined. The new buffer will be read-only if, and only if, this
		 * buffer is read-only.
		 * @returns the new double Buffer, which the caller then owns.
		 */
		virtual DoubleBuffer* asDoubleBuffer() const = 0;

		/**
		 * Creates a view of this byte buffer as a float buffer.
		 * <p>
		 * The content of the new buffer will start at this buffer's current position.
		 * Changes to this buffer's content will be visible in the new buffer, and vice
		 * versa; the two buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be
		 * the number of bytes remaining in this buffer divided by four, and its mark
		 * will be undefined. The new buffer will be read-only if, and only if, this
		 * buffer is read-only.
		 * @returns the new float Buffer, which the caller then owns.
		 */
		virtual FloatBuffer* asFloatBuffer() const = 0;

		/**
		 * Creates a view of this byte buffer as a int buffer.
		 * <p>
		 * The content of the new buffer will start at this buffer's current position.
		 * Changes to this buffer's content will be visible in the new buffer, and vice
		 * versa; the two buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be
		 * the number of bytes remaining in this buffer divided by four, and its mark
		 * will be undefined. The new buffer will be read-only if, and only if, this
		 * buffer is read-only.
		 * @returns the new int Buffer, which the caller then owns.
		 */
		virtual IntBuffer* asIntBuffer() const = 0;

		/**
		 * Creates a view of this byte buffer as a long buffer.
		 * <p>
		 * The content of the new buffer will start at this buffer's current position.
		 * Changes to this buffer's content will be visible in the new buffer, and vice
		 * versa; the two buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be
		 * the number of bytes remaining in this buffer divided by eight, and its mark
		 * will be undefined. The new buffer will be read-only if, and only if, this
		 * buffer is read-only.
		 * @returns the new long Buffer, which the caller then owns.
		 */
		virtual LongBuffer* asLongBuffer() const = 0;

		/**
		 * Creates a view of this byte buffer as a short buffer.
		 * <p>
		 * The content of the new buffer will start at this buffer's current position.
		 * Changes to this buffer's content will be visible in the new buffer, and vice
		 * versa; the two buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be
		 * the number of bytes remaining in this buffer divided by two, and its mark
		 * will be undefined. The new buffer will be read-only if, and only if, this
		 * buffer is read-only.
		 * @returns the new short Buffer, which the caller then owns.
		 */
		virtual ShortBuffer* asShortBuffer() const = 0;

		/**
		 * Creates a new, read-only byte buffer that shares this buffer's content.
		 * <p>
		 * The content of the new buffer will be that of this buffer. Changes to this
		 * buffer's content will be visible in the new buffer; the new buffer itself,
		 * however, will be read-only and will not allow the shared content to be
		 * modified. The two buffers' position, limit, and mark values will be
		 * independent.
		 * <p>
		 * If this buffer is itself read-only then this method behaves in exactly the
		 * same way as the duplicate method.
		 * <p>
		 * The new buffer's capacity, limit, position, and mark values will be
		 * identical to those of this buffer.
		 * @return The new, read-only byte buffer which the caller then owns.
		 */
		virtual ByteBuffer* asReadOnlyBuffer() const = 0;

		/**
		 * Compacts this buffer
		 * <p>
		 * The bytes between the buffer's current position and its limit, if any, are
		 * copied to the beginning of the buffer. That is, the byte at index
		 * p = position() is copied to index zero, the byte at index p + 1 is copied
		 * to index one, and so forth until the byte at index limit() - 1 is copied
		 * to index n = limit() - 1 - p. The buffer's position is then set to n+1 and
		 * its limit is set to its capacity. The mark, if defined, is discarded.
		 * <p>
		 * The buffer's position is set to the number of bytes copied, rather than to
		 * zero, so that an invocation of this method can be followed immediately by
		 * an invocation of another relative put method.
		 * @returns a reference to this ByteBuffer
		 * @throws ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& compact() throw( ReadOnlyBufferException ) = 0;

		/**
		 * Creates a new byte buffer that shares this buffer's content.
		 * <p>
		 * The content of the new buffer will be that of this buffer. Changes to this
		 * buffer's content will be visible in the new buffer, and vice versa; the two
		 * buffers' position, limit, and mark values will be independent.
		 * <p>
		 * The new buffer's capacity, limit, position, and mark values will be identical
		 * to those of this buffer. The new buffer will be read-only if, and only if,
		 * this buffer is read-only.
		 * @returns a new Byte Buffer which the caller owns.
		 */
		virtual ByteBuffer* duplicate() = 0;

		/**
		 * Relative get method. Reads the byte at this buffer's current position, and
		 * then increments the position.
		 * @returns The byte at the buffer's current position
		 * @throws BufferUnderflowException - If the buffer's current position is not
		 * smaller than its limit
		 */
		virtual unsigned char get() const throw( BufferUnderflowException ) = 0;

		/**
		 * Absolute get method. Reads the byte at the given index.
		 * @param index - the index in the Buffer where the byte is to be read
		 * @returns the byte that is located at the given index
		 * @throws IndexOutOfBoundsException - If index is not smaller than the
		 * buffer's limit
		 */
		virtual unsigned char get( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Reads the next byte at this buffer's current position, and then increments
		 * the position by one
		 * @returns the next char in the buffer..
		 * @throws BufferUnderflowException - If there are no more bytes remaining in
		 * this buffer, meaning we have reached the set limit.
		 */
		virtual char getChar() throw( BufferUnderflowException ) = 0;

		/**
		 * Reads one byte at the given index and returns it
		 * @param index - the index in the Buffer where the byte is to be read
		 * @returns the char at the given index in the buffer
		 * @throws IndexOutOfBoundsException - If index is not smaller than the
		 * buffer's limit
		 */
		virtual char getChar( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Reads the next eight bytes at this buffer's current position, and then
		 * increments the position by that amount.
		 * @returns the next double in the buffer..
		 * @throws BufferUnderflowException - If there are no more bytes remaining in
		 * this buffer, meaning we have reached the set limit.
		 */
		virtual double getDouble() throw( BufferUnderflowException ) = 0;

		/**
		 * Reads eight bytes at the given index and returns it
		 * @param index - the index in the Buffer where the bytes are to be read
		 * @returns the double at the given index in the buffer
		 * @throws IndexOutOfBoundsException - If there are not enough bytes
		 * remaining to fill the requested Data Type
		 */
		virtual double getDouble( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Reads the next four bytes at this buffer's current position, and then
		 * increments the position by that amount.
		 * @returns the next float in the buffer..
		 * @throws BufferUnderflowException - If there are no more bytes remaining in
		 * this buffer, meaning we have reached the set limit.
		 */
		virtual float getFloat() throw( BufferUnderflowException ) = 0;

		/**
		 * Reads four bytes at the given index and returns it
		 * @param index - the index in the Buffer where the bytes are to be read
		 * @returns the float at the given index in the buffer
		 * @throws IndexOutOfBoundsException - If there are not enough bytes
		 * remaining to fill the requested Data Type
		 */
		virtual float getFloat( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Reads the next eight bytes at this buffer's current position, and then
		 * increments the position by that amount.
		 * @returns the next long long in the buffer..
		 * @throws BufferUnderflowException - If there are no more bytes remaining in
		 * this buffer, meaning we have reached the set limit.
		 */
		virtual long long getLong() throw( BufferUnderflowException ) = 0;

		/**
		 * Reads eight bytes at the given index and returns it
		 * @param index - the index in the Buffer where the bytes are to be read
		 * @returns the long long at the given index in the buffer
		 * @throws IndexOutOfBoundsException - If there are not enough bytes
		 * remaining to fill the requested Data Type
		 */
		virtual long long getLong( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Reads the next four bytes at this buffer's current position, and then
		 * increments the position by that amount.
		 * @returns the next int in the buffer..
		 * @throws BufferUnderflowException - If there are no more bytes remaining in
		 * this buffer, meaning we have reached the set limit.
		 */
		virtual int getInt() throw( BufferUnderflowException ) = 0;

		/**
		 * Reads four bytes at the given index and returns it
		 * @param index - the index in the Buffer where the bytes are to be read
		 * @returns the int at the given index in the buffer
		 * @throws IndexOutOfBoundsException - If there are not enough bytes
		 * remaining to fill the requested Data Type
		 */
		virtual int getInt( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Reads the next two bytes at this buffer's current position, and then
		 * increments the position by that amount.
		 * @returns the next short in the buffer..
		 * @throws BufferUnderflowException - If there are no more bytes remaining in
		 * this buffer, meaning we have reached the set limit.
		 */
		virtual short getShort() throw( BufferUnderflowException ) = 0;

		/**
		 * Reads two bytes at the given index and returns it
		 * @param index - the index in the Buffer where the bytes are to be read
		 * @returns the short at the given index in the buffer
		 * @throws IndexOutOfBoundsException - If there are not enough bytes
		 * remaining to fill the requested Data Type
		 */
		virtual short getShort( std::size_t index ) const
			throw ( lang::exceptions::IndexOutOfBoundsException ) = 0;

		/**
		 * Writes the given byte into this buffer at the current position, and then
		 * increments the position.
		 * @param value - the byte value to be written
		 * @returns a reference to this buffer
		 * @throws BufferOverflowException - If this buffer's current position is not
		 * smaller than its limit
		 * @throws ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& put( unsigned char value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes the given byte into this buffer at the given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the byte to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& put( std::size_t index, unsigned char value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Writes one byte containing the given value, into this buffer at the
		 * current position, and then increments the position by one.
		 * @param value - The value to be written
		 * @returns a reference to this buffer
		 * @throw BufferOverflowException - If there are fewer than bytes remaining
		 * in this buffer than the size of the data to be written
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putChar( char value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes one byte containing the given value, into this buffer at the
		 * given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the value to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putChar( std::size_t index, char value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Writes eight bytes containing the given value, into this buffer at the
		 * current position, and then increments the position by eight.
		 * @param value - The value to be written
		 * @returns a reference to this buffer
		 * @throw BufferOverflowException - If there are fewer than bytes remaining
		 * in this buffer than the size of the data to be written
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putDouble( double value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes eight bytes containing the given value, into this buffer at the
		 * given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the value to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putDouble( std::size_t index, double value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Writes four bytes containing the given value, into this buffer at the
		 * current position, and then increments the position by eight.
		 * @param value - The value to be written
		 * @returns a reference to this buffer
		 * @throw BufferOverflowException - If there are fewer than bytes remaining
		 * in this buffer than the size of the data to be written
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putFloat( float value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes four bytes containing the given value, into this buffer at the
		 * given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the value to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putFloat( std::size_t index, float value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Writes eight bytes containing the given value, into this buffer at the
		 * current position, and then increments the position by eight.
		 * @param value - The value to be written
		 * @returns a reference to this buffer
		 * @throw BufferOverflowException - If there are fewer than bytes remaining
		 * in this buffer than the size of the data to be written
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putLong( long long value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes eight bytes containing the given value, into this buffer at the
		 * given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the value to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putLong( std::size_t index, long long value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Writes four bytes containing the given value, into this buffer at the
		 * current position, and then increments the position by eight.
		 * @param value - The value to be written
		 * @returns a reference to this buffer
		 * @throw BufferOverflowException - If there are fewer than bytes remaining
		 * in this buffer than the size of the data to be written
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putInt( int value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes four bytes containing the given value, into this buffer at the
		 * given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the value to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putInt( std::size_t index, int value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Writes two bytes containing the given value, into this buffer at the
		 * current position, and then increments the position by eight.
		 * @param value - The value to be written
		 * @returns a reference to this buffer
		 * @throw BufferOverflowException - If there are fewer than bytes remaining
		 * in this buffer than the size of the data to be written
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putShort( short value )
			throw( BufferOverflowException, ReadOnlyBufferException ) = 0;

		/**
		 * Writes two bytes containing the given value, into this buffer at the
		 * given index.
		 * @param index - position in the Buffer to write the data
		 * @param value - the value to write.
		 * @returns a reference to this buffer
		 * @throw IndexOutOfBoundsException - If index greater than the buffer's limit
		 * minus the size of the type being written.
		 * @throw ReadOnlyBufferException - If this buffer is read-only
		 */
		virtual ByteBuffer& putShort( std::size_t index, short value )
			throw( lang::exceptions::IndexOutOfBoundsException,
				   ReadOnlyBufferException ) = 0;

		/**
		 * Creates a new byte buffer whose content is a shared subsequence of this
		 * buffer's content.  The content of the new buffer will start at this buffer's
		 * current position. Changes to this buffer's content will be visible in the new
		 * buffer, and vice versa; the two buffers' position, limit, and mark values will
		 * be independent.
		 * <p>
		 * The new buffer's position will be zero, its capacity and its limit will be the
		 * number of bytes remaining in this buffer, and its mark will be undefined. The
		 * new buffer will be read-only if, and only if, this buffer is read-only.
		 * @returns the newly create ByteBuffer which the caller owns.
		 */
		virtual ByteBuffer* slice() const = 0;

	public:  // Comparable

		/**
		 * Compares this object with the specified object for order. Returns a
		 * negative integer, zero, or a positive integer as this object is less
		 * than, equal to, or greater than the specified object.
		 * @param value - the Object to be compared.
		 * @returns a negative integer, zero, or a positive integer as this
		 * object is less than, equal to, or greater than the specified object.
		 */
		virtual int compareTo( const ByteBuffer& value ) const;

		/**
		 * @return true if this value is considered equal to the passed value.
		 */
		virtual bool equals( const ByteBuffer& value ) const;

		/**
		 * Compares equality between this object and the one passed.
		 * @param value - the value to be compared to this one.
		 * @return true if this object is equal to the one passed.
		 */
		virtual bool operator==( const ByteBuffer& value ) const;

		/**
		 * Compares this object to another and returns true if this object
		 * is considered to be less than the one passed.  This
		 * @param value - the value to be compared to this one.
		 * @return true if this object is equal to the one passed.
		 */
		virtual bool operator<( const ByteBuffer& value ) const;

	public:

		/**
		 * Allocates a new byte buffer whose position will be zero its limit will
		 * be its capacity and its mark is not set.
		 * @param capacity - the internal buffer's capacity.
		 * @returns a newly allocated ByteBuffer which the caller owns.
		 */
		static ByteBuffer* allocate( std::size_t capacity );

		/**
		 * Wraps the passed buffer with a new ByteBuffer.
		 * <p>
		 * The new buffer will be backed by the given byte array; that is, modifications
		 * to the buffer will cause the array to be modified and vice versa. The new
		 * buffer's capacity will be array.length, its position will be offset, its limit
		 * will be offset + length, and its mark will be undefined. Its backing array
		 * will be the given array, and its array offset will be zero.
		 * @param buffer - The array that will back the new buffer
		 * @param offset - The offset of the subarray to be used
		 * @param length - The length of the subarray to be used
		 * @returns a new ByteBuffer that is backed by buffer, caller owns.
		 */
		static ByteBuffer* wrap( unsigned char* array, std::size_t offset, std::size_t length )
			throw( lang::exceptions::NullPointerException );

		/**
		 * Wraps the passed STL Byte Vector in a ByteBuffer.
		 * <p>
		 * The new buffer will be backed by the given byte array; modifications to the
		 * buffer will cause the array to be modified and vice versa. The new buffer's
		 * capacity and limit will be buffer.size(), its position will be zero, and its
		 * mark will be undefined. Its backing array will be the given array, and its
		 * array offset will be zero.
		 * @param buffer - The vector that will back the new buffer, the vector must
		 * have been sized to the desired size already by calling vector.resize( N ).
		 * @returns a new ByteBuffer that is backed by buffer, caller owns.
		 */
		static ByteBuffer* wrap( std::vector<unsigned char>& buffer );

	};

}}

#endif /*_DECAF_NIO_BYTEBUFFER_H_*/
