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
 * $Id: XercesConfiguration.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef XERCESCONFIGURATION_H
#define XERCESCONFIGURATION_H

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/simple-api/XQillaConfiguration.hpp>
#include <xqilla/items/Node.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMNode;
XERCES_CPP_NAMESPACE_END

class XQILLA_API XercesConfiguration : public XQillaConfiguration
{
public:

  /**
   * The "Xerces" DOMNode node interface.
   * Use this as the parameter when you call Item::getInterface()
   * to have a xerces DOMNode returned, if the Node is of the
   * correct type. If it is not of the correct type, the method
   * will return 0.
   */
  static const XMLCh gXerces[];

  virtual DocumentCache *createDocumentCache(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);

  virtual SequenceBuilder *createSequenceBuilder(const DynamicContext *context);

  virtual ItemFactory *createItemFactory(DocumentCache *cache,
                                         XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);

  virtual UpdateFactory *createUpdateFactory(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);

  virtual URIResolver *createDefaultURIResolver(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *memMgr);

  virtual Node::Ptr createNode(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const DynamicContext *context) const;
};

#endif
