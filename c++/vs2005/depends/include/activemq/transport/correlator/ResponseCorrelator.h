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

#ifndef _ACTIVEMQ_TRANSPORT_CORRELATOR_RESPONSECORRELATOR_H_
#define _ACTIVEMQ_TRANSPORT_CORRELATOR_RESPONSECORRELATOR_H_

#include <activemq/util/Config.h>
#include <activemq/transport/TransportFilter.h>
#include <activemq/transport/correlator/FutureResponse.h>
#include <activemq/commands/Command.h>
#include <activemq/commands/Response.h>
#include <decaf/util/concurrent/Mutex.h>
#include <decaf/util/concurrent/Concurrent.h>
#include <decaf/util/concurrent/atomic/AtomicInteger.h>
#include <map>
#include <stdio.h>

namespace activemq{
namespace transport{
namespace correlator{

    using decaf::lang::Pointer;
    using activemq::commands::Command;
    using activemq::commands::Response;

    /**
     * This type of transport filter is responsible for correlating
     * asynchronous responses with requests.  Non-response messages
     * are simply sent directly to the CommandListener.  It owns
     * the transport that it
     */
    class AMQCPP_API ResponseCorrelator : public TransportFilter {
    private:

        /**
         * The next command id for sent commands.
         */
        decaf::util::concurrent::atomic::AtomicInteger nextCommandId;

        /**
         * Map of request ids to future response objects.
         */
        std::map<unsigned int, Pointer<FutureResponse> > requestMap;

        /**
         * Sync object for accessing the request map.
         */
        decaf::util::concurrent::Mutex mapMutex;

        /**
         * Flag to indicate the closed state.
         */
        bool closed;

    public:

        /**
         * Constructor.
         * @param next the next transport in the chain
         */
        ResponseCorrelator( const Pointer<Transport>& next );

        virtual ~ResponseCorrelator();

        /**
         * Sends a one-way command.  Does not wait for any response from the
         * broker.
         * @param command the command to be sent.
         * @throws IOException if an exception occurs during writing of
         * the command.
         * @throws UnsupportedOperationException if this method is not implemented
         * by this transport.
         */
        virtual void oneway( const Pointer<Command>& command )
            throw( decaf::io::IOException,
                   decaf::lang::exceptions::UnsupportedOperationException );

        /**
         * Sends the given request to the server and waits for the response.
         * @param command The request to send.
         * @return the response from the server.
         * @throws IOException if an error occurs with the request.
         */
        virtual Pointer<Response> request( const Pointer<Command>& command )
            throw( decaf::io::IOException,
                   decaf::lang::exceptions::UnsupportedOperationException );

        /**
         * Sends the given request to the server and waits for the response.
         * @param command The request to send.
         * @param timeout The time to wait for a response.
         * @return the response from the server.
         * @throws IOException if an error occurs with the request.
         */
        virtual Pointer<Response> request( const Pointer<Command>& command, unsigned int timeout )
            throw( decaf::io::IOException,
                   decaf::lang::exceptions::UnsupportedOperationException );

        /**
         * This is called in the context of the nested transport's
         * reading thread.  In the case of a response object,
         * updates the request map and notifies those waiting on the
         * response.  Non-response messages are just delegated to
         * the command listener.
         * @param command the received from the nested transport.
         */
        virtual void onCommand( const Pointer<Command>& command );

        /**
         * Starts this transport object and creates the thread for
         * polling on the input stream for commands.  If this object
         * has been closed, throws an exception.  Before calling start,
         * the caller must set the IO streams and the reader and writer
         * objects.
         * @throws IOException if an error occurs or if this transport
         * has already been closed.
         */
        virtual void start() throw( decaf::io::IOException );

        /**
         * Stops the polling thread and closes the streams.  This can
         * be called explicitly, but is also called in the destructor. Once
         * this object has been closed, it cannot be restarted.
         * @throws IOException if errors occur.
         */
        virtual void close() throw( decaf::io::IOException );

        /**
         * Event handler for an exception from a command transport.
         * @param source The source of the exception
         * @param ex The exception.
         */
        virtual void onTransportException( Transport* source,
                                           const decaf::lang::Exception& ex );

    };

}}}

#endif /*_ACTIVEMQ_TRANSPORT_CORRELATOR_RESPONSECORRELATOR_H_*/
