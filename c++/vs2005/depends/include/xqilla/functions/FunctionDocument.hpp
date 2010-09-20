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
 * $Id: FunctionDoc.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _FUNCTIONDOCUMENT_HPP
#define _FUNCTIONDOCUMENT_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/XQFunction.hpp>

class QueryPathNode;

class XQILLA_API FunctionDocument : public XQFunction
{
public:
  static const XMLCh name[];
  static const unsigned int minArgs;
  static const unsigned int maxArgs;

  FunctionDocument(const VectorOfASTNodes &args, XPath2MemoryManager* memMgr);
  
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  Sequence createSequence(DynamicContext* context, int flags=0) const;

  QueryPathNode *getQueryPathTree() const { return queryPathTree_; }
  void setQueryPathTree(QueryPathNode *q) { queryPathTree_ = q; }

private:
  QueryPathNode *queryPathTree_;
};

#endif
