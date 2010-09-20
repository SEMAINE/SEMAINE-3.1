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
 * $Id: ClosureResult.hpp 626 2008-08-26 20:26:44Z jpcs $
 */

#ifndef _CLOSURERESULT_HPP
#define _CLOSURERESULT_HPP

#include <xqilla/runtime/Result.hpp>
#include <xqilla/events/EventGenerator.hpp>
#include <xqilla/context/impl/VarStoreImpl.hpp>

class VariableStore;
class ASTNode;
class DocumentCache;

class XQILLA_API ClosureResult : public ResultImpl
{
public:
  virtual Item::Ptr next(DynamicContext *context);
  virtual Item::Ptr nextOrTail(Result &tail, DynamicContext *context);

  static Result create(const ASTNode *ast, DynamicContext *context, const VariableStore *varStore = 0);

private:
  ClosureResult(const ASTNode *ast, DynamicContext *context, const VariableStore *varStore);

  Item::Ptr contextItem_;
  unsigned int contextPosition_;
  unsigned int contextSize_;
  VarStoreImpl varStore_;
  DocumentCache *docCache_;
  Result result_;
};

class XQILLA_API ClosureEventGenerator : public ASTNodeEventGenerator
{
public:
  ClosureEventGenerator(const ASTNode *ast, DynamicContext *context, bool preserveNS, bool preserveType);

  virtual EventGenerator::Ptr generateEvents(EventHandler *events, DynamicContext *context);

protected:
  Item::Ptr contextItem_;
  unsigned int contextPosition_;
  unsigned int contextSize_;
  VarStoreImpl varStore_;
  DocumentCache *docCache_;
};

#endif
