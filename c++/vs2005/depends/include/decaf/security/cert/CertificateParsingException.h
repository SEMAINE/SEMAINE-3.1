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

#ifndef _DECAF_SECURITY_CERT_CERTIFICATEPARSINGEXCEPTION_H_
#define _DECAF_SECURITY_CERT_CERTIFICATEPARSINGEXCEPTION_H_

#include <decaf/util/Config.h>
#include <decaf/security/cert/CertificateException.h>

namespace decaf{
namespace security{
namespace cert{

    /*
     * Certificate Parsing Exception. This is thrown whenever an invalid
     * DER-encoded certificate is parsed or unsupported DER features are found
     * in the Certificate.
     */
    class DECAF_API CertificateParsingException : public CertificateException
    {
    public:

        /**
         * Default Constructor
         */
        CertificateParsingException() throw() {};

        /**
         * Conversion Constructor from some other Exception
         * @param ex
         *      An exception that should become this type of Exception
         */
        CertificateParsingException(const Exception& ex) throw()
        : Exception()
        {
            *(Exception*)this = ex;
        }

        /**
         * Copy Constructor
         * @param ex
         *      An exception that should become this type of Exception
         */
        CertificateParsingException(const CertificateParsingException& ex) throw()
        : Exception()
        {
            *(Exception*)this = ex;
        }

        /**
         * Constructor - Initializes the file name and line number where
         * this message occured.  Sets the message to report, using an
         * optional list of arguments to parse into the message
         *
         * @param file
         *      name where exception occurs
         * @param lineNumber
         *      line number where the exception occurred.
         * @param msg
         *      message to report
         * @param ...
         *      list of primitives that are formatted into the message
         */
        CertificateParsingException( const char* file,
                               const int lineNumber,
                               const char* msg, ...) throw()
        : Exception()
        {
            va_list vargs;
            va_start(vargs, msg);
            buildMessage(msg, vargs);

            // Set the first mark for this exception.
            setMark(file, lineNumber);
        }

        /**
         * Clones this exception.  This is useful for cases where you need
         * to preserve the type of the original exception as well as the message.
         * All subclasses should override.
         *
         * @return A deep copy of this exception.
         */
        virtual CertificateParsingException* clone() const{
            return new CertificateParsingException(*this);
        }

        virtual ~CertificateParsingException() throw() {}

   };

}}}

#endif /*_DECAF_SECURITY_CERT_CERTIFICATEPARSINGEXCEPTION_H_*/
