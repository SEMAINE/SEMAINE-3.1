#ifndef _INTERVALL_
#define _INTERVALL_

#include <Exception.h>
#include <Presentation.h>
#include <algorithm>

template <class Based>
class Intervall
{

  // Exceptions 
  Public EXCEPTION(IntervallException, std::exception, "") ;
  Public EXCEPTION(InvalidIntervall, IntervallException, "Invalid intervall") ;
  Public EXCEPTION(DisjointIntervall, IntervallException, 
		   "Disjoint intervall not supported") ;
  Public EXCEPTION(EmptyIntervall, IntervallException, "Empty intervall") ;

  // Attributes
  Protected Based _first ;
  Protected Based _second ;
  Protected bool  _empty ;

  // Methods
  Public Intervall(Based const & a, Based const & b)
    throw (InvalidIntervall)
    : _first(a), _second(b), _empty(false) 
    {
      VERIFY(_first<=_second, InvalidIntervall) ;
    }

  Public Intervall()
    : _empty(true) {}
    
  /* Operators */
  // Union
  Public Intervall<Based> operator+ (Intervall<Based> const & i) const
    throw (DisjointIntervall)
    {
	  if(_empty)   return Intervall<Based>(i) ;
	  if(i._empty) return Intervall<Based>(*this) ;
	  VERIFY(i._first<=_second, DisjointIntervall) ;
	  return Intervall<Based>(std::min(_first,i._first), 
				  std::max(_second, i._second)) ;
    }

  // Intersection
  Public Intervall<Based> operator* (Intervall<Based> const & i) const
    {
      if(_empty | i._empty) return Intervall<Based>() ;
      if(i._first>_second) return Intervall<Based>() ;
      return Intervall<Based>(std::max(_first,i._first), std::min(_second,i._second)) ;
    }

  Public bool empty() const
    { return _empty ; }
  
  Public Based const & first() const 
    throw (EmptyIntervall)
    { 
      VERIFY(!_empty, EmptyIntervall) ;
      return _first ; 
    }

  Public Based const & second() const
    throw (EmptyIntervall) 
    {
      VERIFY(!_empty, EmptyIntervall) ;
      return _second ; 
    }

  Public bool in(Based const & v) const
    {
      return (v>=_first) & (v<=_second) ;
    }
} ;

#endif






