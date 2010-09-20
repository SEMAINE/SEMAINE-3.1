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
 * $Id: XQTreatAs.hpp 659 2008-10-06 00:11:22Z jpcs $
 */

#ifndef _XQTREATAS_HPP
#define _XQTREATAS_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/ast/ASTNodeImpl.hpp>
#include <xqilla/runtime/LazySequenceResult.hpp>

class SequenceType;

class XQILLA_API XQTreatAs : public ASTNodeImpl
{
public:
  XQTreatAs(ASTNode* expr, SequenceType *exprType, XPath2MemoryManager* memMgr, const XMLCh *errorCode = 0);
  XQTreatAs(ASTNode* expr, SequenceType *exprType, const XMLCh *errorCode, bool doTypeCheck, bool doCardinalityCheck,
            ASTNode *funcConvert, const StaticType &treatType, bool isExact, XPath2MemoryManager* memMgr);

  virtual Result createResult(DynamicContext* context, int flags=0) const;
  virtual ASTNode* staticResolution(StaticContext *context);
  virtual ASTNode *staticTypingImpl(StaticContext *context);

  ASTNode *getExpression() const;
  ASTNode *getFuncConvert() const { return _funcConvert; }
  SequenceType *getSequenceType() const;
  bool getDoTypeCheck() const { return _doTypeCheck; }
  bool getDoCardinalityCheck() const { return _doCardinalityCheck; }
  const XMLCh *getErrorCode() const { return _errorCode; }
  const StaticType &getTreatType() const { return _treatType; }
  bool getIsExact() const { return _isExact; }

  void setExpression(ASTNode *item);
  void setFuncConvert(ASTNode *item) { _funcConvert = item; }

  static const XMLCh err_XPDY0050[];
  static const XMLCh err_XPTY0004[];
  static const XMLCh funcVarName[];

protected:
  ASTNode* _expr;
  SequenceType *_exprType;
  const XMLCh *_errorCode;
  bool _doTypeCheck;
  bool _doCardinalityCheck;
  ASTNode *_funcConvert;
  StaticType _treatType;
  bool _isExact;
};

#endif
