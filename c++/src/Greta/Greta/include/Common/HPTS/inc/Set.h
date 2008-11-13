#ifndef _Set_H
#define _Set_H

#include <assert.h>
#include <iostream>


class Set
{
  // Attributes
protected:
  int * _set ;
  int _size ;
  int _intSize ;

  // Constructors
public:
  Set(int nbElem=1)
    : _set(new int[(nbElem-1)/(sizeof(int)*8)+1]), _size(nbElem), _intSize((nbElem-1)/(sizeof(int)*8)+1)
  {
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { _set[cpt]=0 ; }
  }

  Set(Set const & s)
    : _set(new int[(s._size)/(sizeof(int)*8)+1]), _size(s._size), _intSize(s._intSize)
  {
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { _set[cpt] = s._set[cpt] ; }
  }

  ~Set()
  {
    delete[] _set ;
  }

protected:
  // dec & co
  int dec(int elem) const
  { return elem%(sizeof(int)*8) ; }

  int offset(int elem) const
  { return elem/(sizeof(int)*8) ; }

public:
  void clear()
  {
    for(int cpt=0; cpt<_intSize ; cpt++)
      {
	_set[cpt]=0 ;
      }
  }

  // Accessors
  bool in(int elem) const
  {
    assert((elem>=0) & (elem<_size)) ;
    return (bool)(_set[offset(elem)] & (1<<dec(elem))) ;
  }

  Set & add(int elem)
  {
    assert((elem>=0) & (elem<_size)) ;
    _set[offset(elem)] |= 1<<dec(elem) ;
    return *this ;
  }

  Set & sub(int elem)
  {
    assert((elem>=0) & (elem<_size)) ;
    _set[offset(elem)] &= ~(1<<dec(elem)) ;
    return *this ;
  }

  bool empty() const
  {
    int result=0 ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { result |= _set[cpt] ; }
    return result==0 ;
  }

  int nbElem() const
  {
    int result=0 ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { 
	int value=_set[cpt] ;
	for(int bitCount=0 ; bitCount<sizeof(int)*8 ; bitCount++, value>>1)
	  {
	    result += value & 1 ;
	  }
      }
    return result ;
  }

  int size() const
  {
    return _size ;
  }

  // operators
  Set operator*(Set const & s) const
  {
    assert(s._size==_size) ;
    Set result(*this) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { result._set[cpt] &= s._set[cpt] ; }
    return result ;
  }

  Set & operator*=(Set const & s)
  {
    assert(s._size==_size) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { _set[cpt] &= s._set[cpt] ; }
    return (*this) ;
  }

  Set operator+(Set const & s) const
  {
    assert(s._size==_size) ;
    Set result(*this) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { result._set[cpt] |= s._set[cpt] ; }
    return result ;
  }

  Set & operator+=(Set const & s)
  {
    assert(s._size==_size) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { _set[cpt] |= s._set[cpt] ; }
    return (*this) ;
  }

  Set operator-(Set const & s) const
  {
    assert(s._size==_size) ;
    Set result(*this) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { result._set[cpt] &= ~s._set[cpt] ; }
    return result ;
  }

  Set operator-=(Set const & s)
  {
    assert(s._size==_size) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { _set[cpt] &= ~s._set[cpt] ; }
    return (*this) ;
  }

  Set operator!() const
  {
    Set result(*this) ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { result._set[cpt] = ~result._set[cpt] ; }
    result._set[_intSize-1] &= ~(-1<<dec(_intSize)) ;
    return result ;
  }

  Set & operator= (Set const & s)
  {
    if(_size<s._size) 
      { 
	delete[] _set ;
	_intSize = s._intSize ;
	_set = new int[_intSize] ;
      }
    _size    = s._size ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { _set[cpt] = s._set[cpt] ; }
    return *this ;
  }

  bool operator== (Set const & s) const
  {
    if(size()!=s.size()) { return false ; }
    int result = 0 ;
    for(int cpt=0 ; cpt<_intSize ; cpt++)
      { result |= _set[cpt]^s._set[cpt] ; }
    return result==0 ;
  }

  bool operator!=(Set const & s) const
  { return !(*this)==s ; }

  bool operator< (Set const & s) const
  {
    if(size()!=s.size())
      { return size()<s.size() ; }
    int cpt = 0 ;
    for(cpt=0 ; cpt<_intSize && _set[cpt]==s._set[cpt] ; cpt++)
      { }
    if(cpt==_intSize) return false ;
    return _set[cpt]<s._set[cpt] ; 
  }
} ;

inline std::ostream & operator<<(std::ostream & o, Set const & s)
{
  o<<"{" ;
  for(int cpt=0 ; cpt<s.size() ; cpt++)
    {
      if(s.in(cpt))
	{ o<<1 ; }
      else
	{ o<<0 ; }
    }
  o<<"}" ;
  return o ;
}

#endif

