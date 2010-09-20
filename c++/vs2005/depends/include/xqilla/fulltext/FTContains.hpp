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
 * $Id: FTContains.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef _FTCONTAINS_HPP
#define _FTCONTAINS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/SingleResult.hpp>

class FTSelection;

class XQILLA_API FTContains : public ASTNodeImpl
{
public:
  FTContains(ASTNode *argument, FTSelection *selection, ASTNode *ignore, XPath2MemoryManager *memMgr);
  ~FTContains();

  virtual ASTNode *staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);
  virtual Result createResult(DynamicContext *context, int flags=0) const;

  ASTNode *getArgument() const { return argument_; }
  FTSelection *getSelection() const { return selection_; }
  ASTNode *getIgnore() const { return ignore_; }

  void setArgument(ASTNode *a) { argument_ = a; }
  void setSelection(FTSelection *s) { selection_ = s; }
  void setIgnore(ASTNode *a) { ignore_ = a; }

private:
  class XQILLA_API FTContainsResult : public SingleResult
  {
  public:
    FTContainsResult(const FTContains *parent);
    Item::Ptr getSingleResult(DynamicContext *context) const;
    std::string asString(DynamicContext *context, int indent) const;

  private:
    const FTContains *parent_;
  };

private:
  ASTNode *argument_;
  FTSelection *selection_;
  ASTNode *ignore_;
};

#endif

