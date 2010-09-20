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
 * $Id: VariableStore.hpp 551 2008-07-01 13:16:55Z jpcs $
 */

#ifndef _VARIABLESTORE_HPP
#define _VARIABLESTORE_HPP

#include <vector>

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/util/XercesDefs.hpp>

class Result;

/** The pure virtual base class for accessing variables at runtime. */
class XQILLA_API VariableStore
{
public:
  /** default destructor */
  virtual ~VariableStore() {};

  /** Looks up the value of a variable by namespace URI and localname pair. */
  virtual Result getVar(const XMLCh *namespaceURI, const XMLCh *name) const = 0;

  /** Fills the vector provided with the uri and localname of the variables in scope */
  virtual void getInScopeVariables(std::vector<std::pair<const XMLCh*, const XMLCh*> > &variables) const = 0;
};

#endif
