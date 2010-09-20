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

#ifndef _DECAF_UTIL_CONCURRENT_LOCK_H_
#define _DECAF_UTIL_CONCURRENT_LOCK_H_

#include <decaf/lang/Exception.h>
#include <decaf/util/concurrent/Synchronizable.h>
#include <decaf/util/Config.h>

namespace decaf{
namespace util{
namespace concurrent{

    /**
     * A wrapper class around a given synchronization mechanism that
     * provides automatic release upon destruction.
     * @author  Nathan Mittler
     */
    class Lock
    {
    private:

        /**
         * Flag to indicate whether or not this object has locked the
         * sync object.
         */
        bool locked;

        /**
         * The synchronizable object to lock/unlock.
         */
        Synchronizable* syncObject;

    public:

        /**
         * Constructor - initializes the object member and locks
         * the object if desired.
         * @param   object   The sync object to control
         * @param   intiallyLocked  If true, the object will automatically
         * be locked.
         */
        Lock( Synchronizable* object, const bool intiallyLocked = true )
        {
            try{
                syncObject = object;
                locked = false;

                if( intiallyLocked )
                {
                    lock();
                }
            }
            DECAF_CATCH_RETHROW( lang::Exception )
            DECAF_CATCHALL_THROW( lang::Exception )
        }

        /**
         * Destructor - Unlocks the object if it is locked.
         */
        virtual ~Lock()
        {
            try{
                if( locked )
                {
                  syncObject->unlock();
                }
            }
            DECAF_CATCH_RETHROW( lang::Exception )
            DECAF_CATCHALL_THROW( lang::Exception )
        }

        /**
         * Locks the object.
         */
        void lock()
        {
            try{
                syncObject->lock();
                locked = true;
            }
            DECAF_CATCH_RETHROW( lang::Exception )
            DECAF_CATCHALL_THROW( lang::Exception )
        }

        /**
         * Unlocks the object.
         */
        void unlock()
        {
            try{
                 if(locked)
                 {
                     syncObject->unlock();
                     locked = false;
                 }
            }
            DECAF_CATCH_RETHROW( lang::Exception )
            DECAF_CATCHALL_THROW( lang::Exception )
        }

        /**
         * Indicates whether or not the object is locked.
         * @return  true if the object is locked, otherwise false.
         */
        bool isLocked() const{ return locked; }
    };

}}}

#endif // _DECAF_UTIL_CONCURRENT_LOCK_H_
