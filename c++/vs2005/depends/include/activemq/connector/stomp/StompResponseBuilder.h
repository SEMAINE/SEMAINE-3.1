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

#ifndef _ACTIVEMQ_CONNECTOR_STOMP_STOMPRESPONSEBUILDER_H_
#define _ACTIVEMQ_CONNECTOR_STOMP_STOMPRESPONSEBUILDER_H_

#include <activemq/transport/MockTransport.h>
#include <activemq/util/Config.h>

namespace activemq{
namespace connector{
namespace stomp{

    class AMQCPP_API StompResponseBuilder :
        public transport::MockTransport::ResponseBuilder{
    public:

        StompResponseBuilder() {}
        virtual ~StompResponseBuilder() {}

        virtual transport::Response* buildResponse( const transport::Command* cmd );
        virtual void buildIncomingCommands(
            const transport::Command* cmd, decaf::util::Queue<transport::Command*>& queue );

    };

}}}

#endif /*_ACTIVEMQ_CONNECTOR_STOMP_STOMPRESPONSEBUILDER_H_*/
