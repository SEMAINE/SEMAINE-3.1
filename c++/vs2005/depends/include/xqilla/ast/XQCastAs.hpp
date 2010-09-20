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
 * $Id: XQCastAs.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef _XQCASTAS_HPP
#define _XQCASTAS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class SequenceType;

class XQILLA_API XQCastAs : public ASTNodeImpl
{
public:
  XQCastAs(ASTNode* expr, SequenceType* exprType, XPath2MemoryManager* memMgr);
  XQCastAs(ASTNode* expr, SequenceType* exprType, bool isPrimitive, AnyAtomicType::AtomicObjectType typeIndex, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  ASTNode *getExpression() const;
  SequenceType *getSequenceType() const;
  bool getIsPrimitive() const { return _isPrimitive; }
  AnyAtomicType::AtomicObjectType getTypeIndex() const { return _typeIndex; }  

  void setExpression(ASTNode *item);

  AnyAtomicType::Ptr cast(const AnyAtomicType::Ptr &in, DynamicContext *context) const;
  AnyAtomicType::Ptr cast(const XMLCh *value, DynamicContext *context) const;

protected:
  class CastAsResult : public SingleResult
  {
  public:
    CastAsResult(const XQCastAs *di);

    Item::Ptr getSingleResult(DynamicContext *context) const;

  private:
    const XQCastAs *_di;
  };

  ASTNode* _expr;
  SequenceType* _exprType;
  bool _isPrimitive;
  AnyAtomicType::AtomicObjectType _typeIndex;
};

#endif
