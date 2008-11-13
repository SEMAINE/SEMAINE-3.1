#ifndef _SET_OPERATORS_H
#define _SET_OPERATORS_H

#include <vector>
#include <list>
#include <set>

/** \brief Passage d'un ensemble à un vecteur */
template <class T, class Cmp>
inline std::vector<T> & operator+=(std::vector<T> & v, std::set<T, Cmp> const & s)
{
  v.resize(s.size()) ;
  int cpt=0 ;
  for(typename std::set<T, Cmp>::const_iterator it=s.begin() ; it!=s.end()  ; it++, cpt++)
    {
      v[cpt]=(*it) ;
    }
  return v ;
}



// ---------------------------
// Operations sur les elements
// ---------------------------

//
// Verification de la presence d'un element dans un ensemble
//
/** \brief Vérification de la présence d'un élément dans un ensemble */
template <class T, class Cmp>
inline bool in(T const & e, std::set<T, Cmp> const & s)
{
  return (s.find(e)!=s.end()) ;
}

//
// Ajout d'un element
//
/** \brief Ajout d'un élément dans un ensemble */
template <class T, class Cmp>
inline std::set<T, Cmp> & operator+=(std::set<T, Cmp> & s, T const & e)
{
  typename std::set<T, Cmp>::iterator pos=s.find(e) ;
  if(pos==s.end())
    { s.insert(e) ; }
  return s ;
}

//
// Supression d'un element
//
/** \brief Suppression d'un élément dans un ensemble */
template <class T, class Cmp>
inline std::set<T, Cmp> & operator-=(std::set<T, Cmp> & s, T const & e)
{
  typename std::set<T, Cmp>::iterator pos=s.find(e) ;
  if(pos!=s.end())
    { s.erase(pos) ; }
  return s ;
}

// ----------------------------
// Operations sur les ensembles
// ----------------------------

//
/** \brief Ajout d'une liste d'éléments dans un ensemble */
template <class T, class Cmp>
std::set<T, Cmp> & operator+=(std::set<T, Cmp> & s, std::list<T> const & l)
{
  for(typename std::list<T>::const_iterator it=l.begin() ; it!=l.end() ; it++)
    {
      s+=(*it) ;
    }
  return s ;
}

/** Suppression d'une liste d'éléments d'un ensemble */
template <class T1, class T2, class Cmp>
std::set<T1, Cmp> & operator-=(std::set<T1, Cmp> & s, std::list<T2> const & l)
{
  for(typename std::list<T2>::const_iterator it=l.begin() ; it!=l.end() ; it++)
    {
      s-=(*it) ;
    }
  return s ;
}

/** Ajout d'un tableau d'elements dans un ensemble. */
template <class T, class Cmp>
std::set<T, Cmp> & operator+=(std::set<T, Cmp> & s, std::vector<T> const & l)
{
  for(typename std::vector<T>::const_iterator it=l.begin() ; it!=l.end() ; it++)
    {
      s+=(*it) ;
    }
  return s ;
}

/** Supression d'un vecteur d'éléments dans un ensemble */
template <class T1, class T2, class Cmp>
std::set<T1, Cmp> & operator-=(std::set<T1, Cmp> & s, std::vector<T2> const & l)
{
  for(typename std::vector<T2>::const_iterator it=l.begin() ; it!=l.end() ; it++)
    {
      s-=(*it) ;
    }
  return s ;
}

/** \brief Union d'ensembles */
template <class T, class Cmp>
std::set<T, Cmp> & operator+=(std::set<T, Cmp> & s1, std::set<T, Cmp> const & s2)
{
  for(typename std::set<T, Cmp>::const_iterator it=s2.begin() ; it!=s2.end() ; it++)
    {
      s1+=*it ;
    }
  return s1 ;
}

/** Soustraction d'ensembles */
template <class T, class Cmp>
std::set<T, Cmp> & operator-=(std::set<T, Cmp> & s1, std::set<T, Cmp> const & s2)
{
  for(typename std::set<T, Cmp>::const_iterator it=s2.begin() ; it!=s2.end() ; it++)
    {
      s1-=*it ;
    }
  return s1 ;
}

/** Intersection d'ensembles */
template <class T, class Cmp>
std::set<T, Cmp> & operator*=(std::set<T, Cmp> & s1, std::set<T, Cmp> const & s2)
{
  for(typename std::set<T, Cmp>::iterator it=s1.begin() ; it!=s1.end() ; it++)
    {
      if(!in(*it, s2))
	{
	  s1.erase(it) ;
	}
    }
  return s1 ; 
}

// ---------------------------------------------------
// Operations entre ensembles avec resultat temporaire
// ---------------------------------------------------

/** \brief Union d'ensembles */
template <class T, class Cmp>
inline std::set<T, Cmp> operator+(std::set<T, Cmp> const & s1, std::set<T, Cmp> const & s2)
{
  if(s1.size()<s2.size())
    return s2+s1 ;

  std::set<T, Cmp> result(s1) ;
  for(typename std::set<T, Cmp>::const_iterator it=s2.begin() ; it!=s2.end() ; it++)
    {
      if(s1.find(*it)==s1.end())
	result.insert(*it) ;
    }
  return result ;
}

/** \brief  Différence d'ensembles */
template <class T, class Cmp>
inline std::set<T, Cmp> operator-(std::set<T, Cmp> const & s1, std::set<T, Cmp> const & s2)
{
  std::set<T, Cmp> result(s1) ;

  for(typename std::set<T, Cmp>::const_iterator it=s2.begin() ; it!=s2.end() ; it++)
    {
      if(s1.find(*it)!=s1.end())
	result.erase(*it) ;
    }
  return result ;
}

/** \brief Intersection d'ensembles */
template <class T, class Cmp>
inline std::set<T, Cmp> operator*(std::set<T, Cmp> const & s1, std::set<T, Cmp> const & s2)
{
  std::set<T, Cmp> result ;

  if(s2.size()<s1.size())
    return s2*s1 ;

  for(typename std::set<T, Cmp>::const_iterator it=s1.begin() ; it!=s1.end() ; it++)
    {
      if(s2.find(*it)!=s2.end())
	result.insert(*it) ;
    }
  return result ;
}
		  
/** \brief xor de deux ensembles */
template <class T, class Cmp>
inline std::set<T, Cmp> operator^(std::set<T, Cmp> const & s1, std::set<T, Cmp> const & s2)
{
  if(s2.size()<s1.size())
    return s2^s1 ;

  std::set<T, Cmp> result(s2) ;
  for(typename std::set<T, Cmp>::const_iterator it=s1.begin() ; it!=s1.end() ; it++)
    {
      if(s2.find(*it)==s2.end())
	{ result.insert(*it) ; }
      else
	{ result.erase(*it) ; }
    }
  
  return result ;
}

/** \brief Affichage d'un ensemble sur un flot. */
template <class Type>
inline std::ostream & operator<<(std::ostream & out, const std::set<Type> & s)
{
  out<<"{ " ;
  for(std::set<int>::const_iterator it=s.begin() ; it!=s.end() ; it++)
    {
      out<<(*it)<<" " ;
    }
  out<<"}" ;
}

#endif

