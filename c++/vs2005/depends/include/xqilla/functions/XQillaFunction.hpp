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
 * $Id: XQillaFunction.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef XQILLAFUNCTION_HPP
#define XQILLAFUNCTION_HPP

#include <xqilla/ast/ConstantFoldingFunction.hpp>

class XQILLA_API XQillaFunction : public ConstantFoldingFunction
{
public:
  static const XMLCh XQillaPrefix[];
  static const XMLCh XMLChFunctionURI[];

protected:
  XQillaFunction(const XMLCh *name, unsigned int argsFrom, unsigned int argsTo,
    const char *paramDecl, const VectorOfASTNodes &args, XPath2MemoryManager *memMgr);
};

#endif
