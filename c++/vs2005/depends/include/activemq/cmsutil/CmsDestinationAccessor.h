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
#ifndef _ACTIVEMQ_CMSUTIL_CMSDESTINATIONACCESSOR_H_
#define _ACTIVEMQ_CMSUTIL_CMSDESTINATIONACCESSOR_H_

#include <activemq/util/Config.h>
#include <activemq/cmsutil/CmsAccessor.h>
#include <activemq/cmsutil/DynamicDestinationResolver.h>

namespace activemq {
namespace cmsutil {

    /**
     * Extends the <code>CmsAccessor</code> to add support for resolving destination names.
     *
     * <p>Not intended to be used directly.
     *
     * @see CmsTemplate
     * @see CmsAccessor
     */
    class AMQCPP_API CmsDestinationAccessor : public CmsAccessor {
    private:

        /**
         * The default destination resolver.
         */
        DynamicDestinationResolver defaultDestinationResolver;

        /**
         * The destination resolver to use.
         */
        DestinationResolver* destinationResolver;

        /**
         * Determines whether to use topics or queues by default.
         */
        bool pubSubDomain;

    public:

        CmsDestinationAccessor();

        virtual ~CmsDestinationAccessor();

        virtual bool isPubSubDomain() const {
            return this->pubSubDomain;
        }

        virtual void setPubSubDomain( bool pubSubDomain ) {
            this->pubSubDomain = pubSubDomain;
        }

        virtual DestinationResolver* getDestinationResolver() {
            return destinationResolver;
        }

        virtual const DestinationResolver* getDestinationResolver() const {
            return destinationResolver;
        }

        virtual void setDestinationResolver( DestinationResolver* destRes ) {
            this->destinationResolver = destRes;
        }

    protected:

        /**
         * Initializes the destination resolver.
         */
        virtual void init()
            throw ( cms::CMSException, decaf::lang::exceptions::IllegalStateException );

        /**
         * Calls destroy() on the destination resolver.
         */
        virtual void destroy()
            throw ( cms::CMSException, decaf::lang::exceptions::IllegalStateException );

        /**
         * Resolves the destination via the <code>DestinationResolver</code>.
         * @param session
         *      the session
         * @param destName
         *      the name of the destination.
         * @return the destination
         * @throws cms::CMSException if resolution failed.
         * @throws decaf::lang::exceptions::IllegalStateException if the destination
         *      resolver property is NULL.
         */
        virtual cms::Destination* resolveDestinationName(
            cms::Session* session,
            const std::string& destName )
                throw ( cms::CMSException, decaf::lang::exceptions::IllegalStateException );

        /**
         * Verifies that the destination resolver is valid.
         */
        virtual void checkDestinationResolver()
            throw ( decaf::lang::exceptions::IllegalStateException );

    };

}}

#endif /* _ACTIVEMQ_CMSUTIL_CMSDESTINATIONACCESSOR_H_ */
