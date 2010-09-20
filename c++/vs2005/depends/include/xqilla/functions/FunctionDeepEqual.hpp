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
 * $Id: FunctionDeepEqual.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

/*
  Deep-equal funcion
*/

#ifndef _FUNCTIONDEEPEQUAL_HPP
#define _FUNCTIONDEEPEQUAL_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ConstantFoldingFunction.hpp>
#include <xqilla/items/Node.hpp>

class Collation;

class XQILLA_API FunctionDeepEqual : public ConstantFoldingFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDeepEqual(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  Sequence createSequence(DynamicContext* context, int flags=0) const;

  static bool deep_equal(Sequence seq1, Sequence seq2, Collation* collation, DynamicContext* context, const LocationInfo *info);
  static bool node_deep_equal(const Node::Ptr &node1, const Node::Ptr &node2, Collation* collation, DynamicContext* context, const LocationInfo *info);

};

#endif // _FUNCTIONDEEPEQUAL_HPP
