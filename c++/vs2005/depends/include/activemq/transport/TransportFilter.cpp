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

#include "TransportFilter.h"
#include <activemq/util/Config.h>

using namespace activemq;
using namespace activemq::transport;

////////////////////////////////////////////////////////////////////////////////
TransportFilter::TransportFilter( Transport* next, const bool own ) {

    this->next = next;
    this->own = own;

    commandlistener = NULL;
    exceptionListener = NULL;

    // Observe the nested transport for events.
    next->setCommandListener( this );
    next->setTransportExceptionListener( this );
}

////////////////////////////////////////////////////////////////////////////////
TransportFilter::~TransportFilter() {

    try{
        if( own ){
            delete next;
            next = NULL;
        }
    }
    AMQ_CATCHALL_NOTHROW()
}

////////////////////////////////////////////////////////////////////////////////
void TransportFilter::onTransportException(
    Transport* source AMQCPP_UNUSED,
    const decaf::lang::Exception& ex ) {

    fire( ex );
}
