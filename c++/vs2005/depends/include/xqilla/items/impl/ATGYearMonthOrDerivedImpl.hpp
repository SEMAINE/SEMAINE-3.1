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
 * $Id: ATGYearMonthOrDerivedImpl.hpp 475 2008-01-08 18:47:44Z jpcs $
 */

#ifndef _ATGYEARMONTHORDERIVEDIMPL_HPP
#define _ATGYEARMONTHORDERIVEDIMPL_HPP

#include <xercesc/util/XercesDefs.hpp>
#include <xqilla/items/ATGYearMonthOrDerived.hpp>

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/Timezone.hpp>

class DynamicContext;

class XQILLA_API ATGYearMonthOrDerivedImpl : public ATGYearMonthOrDerived 
{

public:
  /* constructor */
  ATGYearMonthOrDerivedImpl(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context = 0);

  virtual void *getInterface(const XMLCh *name) const;

  /* Get the name of the primitive type (basic type) of this type
   * (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const;

  /* returns true if the two objects are equal
   * false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const;
  
  /** Returns less than 0 if this is less that other,
      0 if they are the same, and greater than 0 otherwise */
  virtual int compare(const ATGYearMonthOrDerived::Ptr &other, const DynamicContext *context) const;
 
  /** Returns true if a timezone is defined for this.  False otherwise.*/
  virtual bool hasTimezone() const;

  /** Sets the timezone to the given timezone.*/
  virtual ATGYearMonthOrDerived::Ptr setTimezone(const Timezone::Ptr &timezone, const DynamicContext* context) const;
  
  /* Get the primitive index associated with this type */
  static AnyAtomicType::AtomicObjectType getTypeIndex(); 

  /* Get the primitive type name */
  static const XMLCh* getPrimitiveName();

  /* Get the primitive index associated with this type */
  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex() const;

  static MAPM parseGYearMonth(const XMLCh* const value, const MAPM &implicitTimezone);

private:
  /* parse the gYearMonth */
  void setGYearMonth(const XMLCh* const value);

  MAPM buildReferenceDateTime(const DynamicContext *context) const;
  
  /*The values of this gYearMonth*/
  MAPM _MM;  // as nonNegativeInteger
  MAPM _YY;  // as integer

  /* whether this gYearMonth has a timezone value*/
  bool _hasTimezone;

  /* the timezone value, if it exist */
  Timezone::Ptr timezone_;

  /* the name of this type */
  const XMLCh* _typeName;

  /* the uri of this type */
  const XMLCh* _typeURI;
 
};

#endif // _ATGYEARMONTHORDERIVEDIMPL_HPP
