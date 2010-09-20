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
 * $Id: SequenceBuilder.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _SEQUENCEBUILDER_HPP
#define _SEQUENCEBUILDER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/events/EventHandler.hpp>

#include <xercesc/util/XMemory.hpp>

class DynamicContext;
class Sequence;

class XQILLA_API SequenceBuilder : public EventHandler, public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
  virtual ~SequenceBuilder() {}

  virtual Sequence getSequence() const = 0;
};

#endif
