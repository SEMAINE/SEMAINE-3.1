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
 * $Id: XQCommentConstructor.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef XQCOMMENTCONSTRUCTOR_HPP
#define XQCOMMENTCONSTRUCTOR_HPP

#include <xqilla/ast/XQDOMConstructor.hpp>

class EventHandler;

class XQILLA_API XQCommentConstructor : public XQDOMConstructor
{
public:
  XQCommentConstructor(ASTNode *value, XPath2MemoryManager* mm, bool xslt = false);

  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context,
                              bool preserveNS, bool preserveType) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  virtual const XMLCh* getNodeType() const;
  virtual ASTNode *getValue() const;
  bool isXSLT() const { return xslt_; }

  virtual void setValue(ASTNode *value);

private:
  ASTNode *m_value;
  bool xslt_;
};

#endif
