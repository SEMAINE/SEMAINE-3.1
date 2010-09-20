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
 * $Id: ContentSequenceFilter.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _CONTENTSEQUENCEFILTER_HPP
#define _CONTENTSEQUENCEFILTER_HPP

#include <xqilla/events/EventHandler.hpp>

class XQILLA_API ContentSequenceFilter : public EventFilter
{
public:
  ContentSequenceFilter(EventHandler *next);

  virtual void startDocumentEvent(const XMLCh *documentURI, const XMLCh *encoding);
  virtual void endDocumentEvent();
  virtual void endEvent();
  virtual void startElementEvent(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localname);
  virtual void piEvent(const XMLCh *target, const XMLCh *value);
  virtual void textEvent(const XMLCh *value);
  virtual void textEvent(const XMLCh *chars, unsigned int length);
  virtual void commentEvent(const XMLCh *value);
  virtual void attributeEvent(const XMLCh *prefix, const XMLCh *uri, const XMLCh *localname, const XMLCh *value,
                              const XMLCh *typeURI, const XMLCh *typeName);
  virtual void namespaceEvent(const XMLCh *prefix, const XMLCh *uri);
  virtual void atomicItemEvent(AnyAtomicType::AtomicObjectType type, const XMLCh *value, const XMLCh *typeURI,
                               const XMLCh *typeName);

private:
  bool lastWasAtomic_;
};

#endif
