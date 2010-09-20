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

#ifndef _DECAF_INTERNAL_NIO_BUFFERFACTORY_H_
#define _DECAF_INTERNAL_NIO_BUFFERFACTORY_H_

#include <decaf/nio/ByteBuffer.h>
#include <decaf/nio/CharBuffer.h>
#include <decaf/nio/DoubleBuffer.h>
#include <decaf/nio/FloatBuffer.h>
#include <decaf/nio/LongBuffer.h>
#include <decaf/nio/IntBuffer.h>
#include <decaf/nio/ShortBuffer.h>

namespace decaf{
namespace internal{
namespace nio{

    /**
     * Factory class used by static methods in the decaf::nio package to
     * create the various default version of the NIO interfaces.
     */
    class DECAF_API BufferFactory {
    public:

        virtual ~BufferFactory() {}

        /**
         * Allocates a new byte buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated ByteBuffer which the caller owns.
         */
        static decaf::nio::ByteBuffer* createByteBuffer( std::size_t capacity );

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
        static decaf::nio::ByteBuffer* createByteBuffer( unsigned char* buffer,
                                                         std::size_t offset,
                                                         std::size_t length )
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
        static decaf::nio::ByteBuffer* createByteBuffer( std::vector<unsigned char>& buffer );

        /**
         * Allocates a new char buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated CharBuffer which the caller owns.
         */
        static decaf::nio::CharBuffer* createCharBuffer( std::size_t capacity );

        /**
         * Wraps the passed buffer with a new CharBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; that is, modifications
         * to the buffer will cause the array to be modified and vice versa. The new
         * buffer's capacity will be array.length, its position will be offset, its limit
         * will be offset + length, and its mark will be undefined. Its backing array
         * will be the given array, and its array offset will be zero.
         * @param buffer - The array that will back the new buffer
         * @param offset - The offset of the subarray to be used
         * @param length - The length of the subarray to be used
         * @returns a new CharBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::CharBuffer* createCharBuffer( char* buffer,
                                                         std::size_t offset,
                                                         std::size_t length )
            throw( lang::exceptions::NullPointerException );

        /**
         * Wraps the passed STL Byte Vector in a CharBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; modifications to the
         * buffer will cause the array to be modified and vice versa. The new buffer's
         * capacity and limit will be buffer.size(), its position will be zero, and its
         * mark will be undefined. Its backing array will be the given array, and its
         * array offset will be zero.
         * @param buffer - The vector that will back the new buffer, the vector must
         * have been sized to the desired size already by calling vector.resize( N ).
         * @returns a new CharBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::CharBuffer* createCharBuffer( std::vector<char>& buffer );

        /**
         * Allocates a new double buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated DoubleBuffer which the caller owns.
         */
        static decaf::nio::DoubleBuffer* createDoubleBuffer( std::size_t capacity );

        /**
         * Wraps the passed buffer with a new DoubleBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; that is, modifications
         * to the buffer will cause the array to be modified and vice versa. The new
         * buffer's capacity will be array.length, its position will be offset, its limit
         * will be offset + length, and its mark will be undefined. Its backing array
         * will be the given array, and its array offset will be zero.
         * @param buffer - The array that will back the new buffer
         * @param offset - The offset of the subarray to be used
         * @param length - The length of the subarray to be used
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::DoubleBuffer* createDoubleBuffer( double* buffer,
                                                             std::size_t offset,
                                                             std::size_t length )
            throw( lang::exceptions::NullPointerException );

        /**
         * Wraps the passed STL Double Vector in a DoubleBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; modifications to the
         * buffer will cause the array to be modified and vice versa. The new buffer's
         * capacity and limit will be buffer.size(), its position will be zero, and its
         * mark will be undefined. Its backing array will be the given array, and its
         * array offset will be zero.
         * @param buffer - The vector that will back the new buffer, the vector must
         * have been sized to the desired size already by calling vector.resize( N ).
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::DoubleBuffer* createDoubleBuffer( std::vector<double>& buffer );

        /**
         * Allocates a new float buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated DoubleBuffer which the caller owns.
         */
        static decaf::nio::FloatBuffer* createFloatBuffer( std::size_t capacity );

        /**
         * Wraps the passed buffer with a new FloatBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; that is, modifications
         * to the buffer will cause the array to be modified and vice versa. The new
         * buffer's capacity will be array.length, its position will be offset, its limit
         * will be offset + length, and its mark will be undefined. Its backing array
         * will be the given array, and its array offset will be zero.
         * @param buffer - The array that will back the new buffer
         * @param offset - The offset of the subarray to be used
         * @param length - The length of the subarray to be used
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::FloatBuffer* createFloatBuffer( float* buffer,
                                                           std::size_t offset,
                                                           std::size_t length )
            throw( lang::exceptions::NullPointerException );

        /**
         * Wraps the passed STL Float Vector in a FloatBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; modifications to the
         * buffer will cause the array to be modified and vice versa. The new buffer's
         * capacity and limit will be buffer.size(), its position will be zero, and its
         * mark will be undefined. Its backing array will be the given array, and its
         * array offset will be zero.
         * @param buffer - The vector that will back the new buffer, the vector must
         * have been sized to the desired size already by calling vector.resize( N ).
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::FloatBuffer* createFloatBuffer( std::vector<float>& buffer );

        /**
         * Allocates a new long long buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated DoubleBuffer which the caller owns.
         */
        static decaf::nio::LongBuffer* createLongBuffer( std::size_t capacity );

        /**
         * Wraps the passed buffer with a new LongBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; that is, modifications
         * to the buffer will cause the array to be modified and vice versa. The new
         * buffer's capacity will be array.length, its position will be offset, its limit
         * will be offset + length, and its mark will be undefined. Its backing array
         * will be the given array, and its array offset will be zero.
         * @param buffer - The array that will back the new buffer
         * @param offset - The offset of the subarray to be used
         * @param length - The length of the subarray to be used
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::LongBuffer* createLongBuffer( long long* buffer,
                                                           std::size_t offset,
                                                           std::size_t length )
            throw( lang::exceptions::NullPointerException );

        /**
         * Wraps the passed STL Long Vector in a LongBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; modifications to the
         * buffer will cause the array to be modified and vice versa. The new buffer's
         * capacity and limit will be buffer.size(), its position will be zero, and its
         * mark will be undefined. Its backing array will be the given array, and its
         * array offset will be zero.
         * @param buffer - The vector that will back the new buffer, the vector must
         * have been sized to the desired size already by calling vector.resize( N ).
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::LongBuffer* createLongBuffer( std::vector<long long>& buffer );

        /**
         * Allocates a new int buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated DoubleBuffer which the caller owns.
         */
        static decaf::nio::IntBuffer* createIntBuffer( std::size_t capacity );

        /**
         * Wraps the passed buffer with a new IntBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; that is, modifications
         * to the buffer will cause the array to be modified and vice versa. The new
         * buffer's capacity will be array.length, its position will be offset, its limit
         * will be offset + length, and its mark will be undefined. Its backing array
         * will be the given array, and its array offset will be zero.
         * @param buffer - The array that will back the new buffer
         * @param offset - The offset of the subarray to be used
         * @param length - The length of the subarray to be used
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::IntBuffer* createIntBuffer( int* buffer,
                                                       std::size_t offset,
                                                       std::size_t length )
            throw( lang::exceptions::NullPointerException );

        /**
         * Wraps the passed STL int Vector in a IntBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; modifications to the
         * buffer will cause the array to be modified and vice versa. The new buffer's
         * capacity and limit will be buffer.size(), its position will be zero, and its
         * mark will be undefined. Its backing array will be the given array, and its
         * array offset will be zero.
         * @param buffer - The vector that will back the new buffer, the vector must
         * have been sized to the desired size already by calling vector.resize( N ).
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::IntBuffer* createIntBuffer( std::vector<int>& buffer );

        /**
         * Allocates a new short buffer whose position will be zero its limit will
         * be its capacity and its mark is not set.
         * @param capacity - the internal buffer's capacity.
         * @returns a newly allocated DoubleBuffer which the caller owns.
         */
        static decaf::nio::ShortBuffer* createShortBuffer( std::size_t capacity );

        /**
         * Wraps the passed buffer with a new ShortBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; that is, modifications
         * to the buffer will cause the array to be modified and vice versa. The new
         * buffer's capacity will be array.length, its position will be offset, its limit
         * will be offset + length, and its mark will be undefined. Its backing array
         * will be the given array, and its array offset will be zero.
         * @param buffer - The array that will back the new buffer
         * @param offset - The offset of the subarray to be used
         * @param length - The length of the subarray to be used
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::ShortBuffer* createShortBuffer( short* buffer,
                                                           std::size_t offset,
                                                           std::size_t length )
            throw( lang::exceptions::NullPointerException );

        /**
         * Wraps the passed STL Short Vector in a ShortBuffer.
         * <p>
         * The new buffer will be backed by the given byte array; modifications to the
         * buffer will cause the array to be modified and vice versa. The new buffer's
         * capacity and limit will be buffer.size(), its position will be zero, and its
         * mark will be undefined. Its backing array will be the given array, and its
         * array offset will be zero.
         * @param buffer - The vector that will back the new buffer, the vector must
         * have been sized to the desired size already by calling vector.resize( N ).
         * @returns a new DoubleBuffer that is backed by buffer, caller owns.
         */
        static decaf::nio::ShortBuffer* createShortBuffer( std::vector<short>& buffer );

    };

}}}

#endif /*_DECAF_INTERNAL_NIO_BUFFERFACTORY_H_*/
