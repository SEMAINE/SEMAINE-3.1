/*
 * Copyright (c) 2001-2008
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2008
 *     Oracle. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Id: DynamicErrorException.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _DYNAMICERROREXCEPTION_HPP
#define _DYNAMICERROREXCEPTION_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/exceptions/XQException.hpp>
#include <xqilla/utils/XStr.hpp>

/** 
 * This means that an error was found while evaluating the expression
 */

class XQILLA_API DynamicErrorException : public XQException
{
public:
  DynamicErrorException(const XMLCh* const functionName, const XMLCh* const reason, const LocationInfo *info, const char *file, int line)
    : XQException(X("DynamicErrorException"), functionName, reason, info, file, line) {};
};

#endif // _DYNAMICERROREXCEPTION_HPP
