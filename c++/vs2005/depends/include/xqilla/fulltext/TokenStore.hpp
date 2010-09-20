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
 * $Id: TokenStore.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _TOKENSTORE_HPP
#define _TOKENSTORE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/framework/ReferenceCounted.hpp>
#include <xqilla/fulltext/TokenInfo.hpp>

class Tokenizer;

class XQILLA_API TokenStore : public ReferenceCounted
{
public:
  typedef RefCountPointer<const TokenStore> Ptr;

  virtual ~TokenStore() {}

  virtual TokenStream::Ptr findTokens(const XMLCh *searchString) const = 0;

  virtual unsigned int getStartTokenPosition() const = 0;
  virtual unsigned int getEndTokenPosition() const = 0;

protected:
  TokenStore() {}
};

#endif
