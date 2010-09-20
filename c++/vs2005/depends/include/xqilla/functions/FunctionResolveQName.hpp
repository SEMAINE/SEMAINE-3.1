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
 * $Id: FunctionResolveQName.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

/*
 * resolve-QName function
 */

#ifndef _FUNCTIONRESOLVEQNAME_HPP
#define _FUNCTIONRESOLVEQNAME_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>

/** resolve-QName function. */
class XQILLA_API FunctionResolveQName : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionResolveQName(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  /** Returns an QName with the lexical form given in the first argument. The 
   *  prefix is resolved using the in-scope namespaces for a given element. **/
  Sequence createSequence(DynamicContext* context, int flags=0) const;

};

#endif // _FUNCTIONRESOLVEQNAME_HPP
