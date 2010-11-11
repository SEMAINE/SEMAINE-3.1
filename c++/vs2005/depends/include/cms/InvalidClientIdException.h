/**
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

#ifndef _CMS_INVALICLIENTIDEXCEPTION_H_
#define _CMS_INVALICLIENTIDEXCEPTION_H_

#include <cms/Config.h>
#include <cms/CMSException.h>

namespace cms{

    /**
     * This exception must be thrown when a client attempts to set a connection's
     * client ID to a value that is rejected by a provider.
     *
     * @since 1.3
     */
    class CMS_API InvalidClientIdException : public CMSException {
    public:

        InvalidClientIdException() throw();

        InvalidClientIdException( const InvalidClientIdException& ex ) throw();

        InvalidClientIdException( const std::string& message,
                                  const std::exception* cause ) throw();

        InvalidClientIdException( const std::string& message,
                                  const std::exception* cause,
                                  const std::vector< std::pair< std::string, int> >& stackTrace ) throw();

        virtual ~InvalidClientIdException() throw();

    };

}

#endif /*_CMS_INVALICLIENTIDEXCEPTION_H_*/
