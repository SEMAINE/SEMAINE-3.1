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

#ifndef _DECAF_UTIL_STLSET_H_
#define _DECAF_UTIL_STLSET_H_

#include <set>
#include <vector>
#include <memory>
#include <decaf/lang/exceptions/NoSuchElementException.h>
#include <decaf/util/concurrent/Synchronizable.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/Iterator.h>
#include <decaf/util/AbstractSet.h>

namespace decaf{
namespace util{

    /**
     * Set template that wraps around a std::set to provide
     * a more user-friendly interface and to provide common
     * functions that do not exist in std::set.
     */
    template <typename E>
    class StlSet : public decaf::util::AbstractSet<E> {
    private:

        std::set<E> values;

    private:

        class SetIterator : public Iterator<E> {
        private:

            typename std::set<E>::iterator current;
            typename std::set<E>::iterator previous;
            typename std::set<E>* set;

        public:

            SetIterator( typename std::set<E>* set ) {
                this->current = set->begin();
                this->previous = set->end();
                this->set = set;
            }
            virtual ~SetIterator() {}

            virtual E next() throw( lang::exceptions::NoSuchElementException ){
                if( this->current == set->end() ) {
                    throw lang::exceptions::NoSuchElementException(
                        __FILE__, __LINE__,
                        "Set::Iterator::next - No more elements to return" );
                }

                this->previous = this->current;
                return *( this->current++ );
            }

            virtual bool hasNext() const {
                return ( this->current != set->end() );
            }

            virtual void remove() throw ( lang::exceptions::IllegalStateException,
                                          lang::exceptions::UnsupportedOperationException ){
                if( this->previous == set->end() ) {
                    throw lang::exceptions::IllegalStateException(
                        __FILE__, __LINE__,
                        "Set::Iterator::remove - Invalid State to call remove" );
                }

                this->set->erase( this->previous );
                this->previous = this->set->end();
            }
        };

        class ConstSetIterator : public Iterator<E> {
        private:

            typename std::set<E>::const_iterator current;
            typename std::set<E>::const_iterator previous;
            const typename std::set<E>* set;

        public:

            ConstSetIterator( const typename std::set<E>* set ) {
                this->current = set->begin();
                this->previous = set->end();
                this->set = set;
            }
            virtual ~ConstSetIterator() {}

            virtual E next() throw( lang::exceptions::NoSuchElementException ){
                if( this->current == set->end() ) {
                    throw lang::exceptions::NoSuchElementException(
                        __FILE__, __LINE__,
                        "Set::Iterator::next - No more elements to return" );
                }

                this->previous = this->current;
                return *( this->current++ );
            }

            virtual bool hasNext() const {
                return ( this->current != set->end() );
            }

            virtual void remove() throw ( lang::exceptions::IllegalStateException,
                                          lang::exceptions::UnsupportedOperationException ){
                throw lang::exceptions::UnsupportedOperationException(
                    __FILE__, __LINE__,
                    "Set::Iterator::remove - Not Valid on a Const Iterator" );
            }
        };

    public:

        /**
         * Default constructor - does nothing.
         */
        StlSet() {}

        /**
         * Copy constructor - copies the content of the given set into this
         * one.
         * @param source The source set.
         */
        StlSet( const StlSet& source ){
            copy( source );
        }

        /**
         * Copy constructor - copies the content of the given set into this
         * one.
         * @param source The source set.
         */
        StlSet( const Collection<E>& source ){
            copy( source );
        }

        virtual ~StlSet() {}

        /**
         * {@inheritDoc}
         */
        Iterator<E>* iterator() {
            return new SetIterator( &values );
        }
        Iterator<E>* iterator() const {
            return new ConstSetIterator( &values );
        }

        /**
         * {@inheritDoc}
         */
        virtual bool equals( const StlSet& source ) const {
            return this->values == source.values;
        }

        /**
         * {@inheritDoc}
         */
        virtual void copy( const StlSet& source ) {
            this->values.clear();
            this->values = source.values;
        }

        /**
         * {@inheritDoc}
         */
        virtual void clear() throw ( lang::exceptions::UnsupportedOperationException ) {
            values.clear();
        }

        /**
         * {@inheritDoc}
         */
        virtual bool contains( const E& value ) const throw ( lang::Exception ) {
            typename std::set<E>::const_iterator iter;
            iter = values.find( value );
            return iter != values.end();
        }

        /**
         * @return if the set contains any element or not, TRUE or FALSE
         */
        virtual bool isEmpty() const {
            return values.empty();
        }

        /**
         * @return The number of elements in this set.
         */
        virtual std::size_t size() const {
            return values.size();
        }

        /**
         * {@inheritDoc}
         */
        virtual bool add( const E& value )
            throw ( lang::exceptions::UnsupportedOperationException,
                    lang::exceptions::IllegalArgumentException,
                    lang::exceptions::IllegalStateException ) {

            return values.insert( value ).second;
        }

        /**
         * {@inheritDoc}
         */
        virtual bool remove( const E& value )
            throw ( lang::exceptions::UnsupportedOperationException,
                    lang::exceptions::IllegalArgumentException ) {

            return values.erase( value ) != 0;
        }

    };

}}

#endif /*_DECAF_UTIL_STLSET_H_*/
