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
 * $Id: LetTuple.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef LETTUPLE_HPP
#define LETTUPLE_HPP

#include <xqilla/ast/TupleNode.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>

class ASTNode;
class SequenceType;

class XQILLA_API LetTuple : public TupleNode
{
public:
  LetTuple(TupleNode *parent, const XMLCh *varQName, ASTNode *expr, XPath2MemoryManager *mm);
  LetTuple(TupleNode *parent, const XMLCh *varURI, const XMLCh *varName, ASTNode *expr, XPath2MemoryManager *mm);

  const XMLCh *getVarURI() const { return varURI_; }
  void setVarURI(const XMLCh *uri) { varURI_ = uri; }
  const XMLCh *getVarName() const { return varName_; }
  void setVarName(const XMLCh *name) { varName_ = name; }

  ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }

  const StaticAnalysis &getVarSRC() const { return varSrc_; }

  virtual TupleNode *staticResolution(StaticContext *context);
  virtual TupleNode *staticTypingImpl(StaticContext *context);
  virtual TupleNode *staticTypingTeardown(StaticContext *context, StaticAnalysis &usedSrc);

  virtual TupleResult::Ptr createResult(DynamicContext* context) const;

  // Used during parsing
  SequenceType *seqType;

private:
  const XMLCh *varQName_, *varURI_, *varName_;
  StaticAnalysis varSrc_;
  ASTNode *expr_;
};

#endif
