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
 * $Id: XQDOMConstructor.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef XQDOMCONSTRUCTOR_HPP
#define XQDOMCONSTRUCTOR_HPP

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

#include <xercesc/framework/XMLBuffer.hpp>

class EventHandler;
class QueryPathNode;

class XQILLA_API XQDOMConstructor : public ASTNodeImpl
{
public:
  XQDOMConstructor(XPath2MemoryManager* mm);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context,
                              bool preserveNS, bool preserveType) const = 0;

  virtual const XMLCh* getNodeType() const = 0;
  virtual ASTNode *getName() const { return 0; }
  virtual const VectorOfASTNodes *getAttributes() const { return 0; }
  virtual const VectorOfASTNodes *getChildren() const { return 0; }
  virtual ASTNode *getValue() const { return 0; }

  virtual void setName(ASTNode *name) {}
  virtual void setValue(ASTNode *value) {}

  static bool getStringValue(const VectorOfASTNodes* m_children, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &value,
                             DynamicContext *context);
  static bool getStringValue(const ASTNode *child, XERCES_CPP_NAMESPACE_QUALIFIER XMLBuffer &value,
                             DynamicContext *context);

  QueryPathNode *getQueryPathTree() const { return queryPathTree_; }
  void setQueryPathTree(QueryPathNode *q) { queryPathTree_ = q; }

private:
  QueryPathNode *queryPathTree_;
};

class XQILLA_API XQContentSequence : public ASTNodeImpl
{
public:
  XQContentSequence(ASTNode *expr, XPath2MemoryManager* mm);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context,
                              bool preserveNS, bool preserveType) const;

  ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }

private:
  ASTNode *expr_;
};

class XQILLA_API XQDirectName : public ASTNodeImpl
{
public:
  XQDirectName(const XMLCh *qname, bool isAttr, XPath2MemoryManager* mm);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  const XMLCh *getQName() const { return qname_; }
  bool getIsAttr() const { return isAttr_; }

private:
  const XMLCh *qname_;
  bool isAttr_;
};

class XQILLA_API XQNameExpression : public ASTNodeImpl
{
public:
  XQNameExpression(ASTNode *expr, XPath2MemoryManager* mm);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  ASTNode *getExpression() const { return expr_; }
  void setExpression(ASTNode *expr) { expr_ = expr; }

private:
  class NameExpressionResult : public SingleResult
  {
  public:
    NameExpressionResult(const XQNameExpression *ast)
      : SingleResult(ast), ast_(ast) {}

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const
    { return ""; }

  private:
    const XQNameExpression *ast_;
  };

  ASTNode *expr_;
};

class XQILLA_API XQSimpleContent : public ASTNodeImpl
{
public:
  XQSimpleContent(VectorOfASTNodes *children, XPath2MemoryManager* mm);

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext* context, int flags=0) const;

  const VectorOfASTNodes *getChildren() const { return children_; }

private:
  class SimpleContentResult : public SingleResult
  {
  public:
    SimpleContentResult(const XQSimpleContent *ast)
      : SingleResult(ast), ast_(ast) {}

    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const
    { return ""; }

  private:
    const XQSimpleContent *ast_;
  };

  VectorOfASTNodes *children_;
};

#endif
