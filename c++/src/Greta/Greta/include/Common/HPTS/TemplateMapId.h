#ifndef _TemplateMapId_H
#define _TemplateMapId_H

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>
#include <map>
#include <list>
#include <vector>
#include <assert.h>

template <class Type>
class TemplateMapId
{
  protected: std::map<Type, int>   m_TypeId ;
  protected: std::vector<Type>     m_idType ;
  protected: std::vector<int>      m_idRef ;
  protected: std::list<int>        m_freed ;

  public: class Identifier
  {
    protected: int             m_id ;
    protected: TemplateMapId * m_TypeId ; 
    
    public:    inline Identifier() 
	  : m_id(-1), m_TypeId(NULL)
	{}

    
	protected: inline Identifier(TemplateMapId<Type> * TypeId, int id) 
	  : m_id(id), m_TypeId(TypeId)
	{
		if(m_id!=-1)
			m_TypeId->m_idRef[id]++ ;
	}

    public:    inline Identifier(Identifier const & id) 
	  : m_id(id.m_id), m_TypeId(id.m_TypeId)
	{
		if(m_id!=-1)
		m_TypeId->m_idRef[m_id]++ ;
	}

    public:    inline ~Identifier() 
	{ 
	  if(m_id!=-1)
		{ m_TypeId->dereference(m_id) ; }
	}


    public:    inline int operator() () const 
	{ return m_id ; }

    public:    inline Type const & value() const 
	{ return m_TypeId->m_idType[m_id] ; }

    public:    inline bool operator<(Identifier const & id) const 
	{
		assert(m_TypeId==id.m_TypeId) ;
		return m_id<id.m_id ;
	}

    public:    inline bool operator==(Identifier const & id) const 
	{
		assert(m_TypeId==id.m_TypeId) ;
		return m_id==id.m_id ;
	}

    public:    inline bool operator!=(Identifier const & id) const 
	{
		assert(m_TypeId==id.m_TypeId) ;
		return m_id!=id.m_id ;
	}

    public:    inline Identifier & operator=(Identifier const & id) 
	{
		if(m_id!=-1)
			{ m_TypeId->dereference(m_id) ; }
		m_id=id.m_id ;
		m_TypeId=id.m_TypeId ;
		if(m_id!=-1)
			{ m_TypeId->m_idRef[m_id]++ ; }
		return *this ;
	}

    friend class TemplateMapId<Type> ;
  } ;

  //
  // Classe permettant d'appeler une fonction de call back lors du dereferencement complet
  // d'un identifiant.
  //
  public: class CallBack
  {
    public: virtual void action(int id) = 0 ;
  } ;

  protected: CallBack * m_callBack ;


  public: TemplateMapId() : m_callBack(NULL) {}
  public: TemplateMapId(CallBack * callBack) : m_callBack(callBack) {}

  public:    inline Identifier reference(Type const & str) ;
  public:    inline bool contains(Identifier const & id) ;
  public:    inline int  size() const ;
  public:    inline bool exist(Type const & str) const ;

  protected: inline void dereference(int id) ;

  friend class Identifier ;
} ;
  
// --------------
// Implementation
// --------------
/*
template <class Type>
TemplateMapId<Type>::Identifier::Identifier()
  : m_id(-1), m_TypeId(NULL)
{}
*/

/*
template <class Type>
TemplateMapId<Type>::Identifier::Identifier(TemplateMapId<Type> * TypeId, int id) 
  : m_id(id), m_TypeId(TypeId)
{
  if(m_id!=-1)
    m_TypeId->m_idRef[id]++ ;
}
*/

/*
template <class Type>
TemplateMapId<Type>::Identifier::Identifier(Identifier const & id) 
  : m_id(id.m_id), m_TypeId(id.m_TypeId)
{
  if(m_id!=-1)
    m_TypeId->m_idRef[m_id]++ ;
}
*/


//template <class Type>
//TemplateMapId<Type>::Identifier::~Identifier() 
//{ 
//  if(m_id!=-1)
//    { m_TypeId->dereference(m_id) ; }
//}

/*
template <class Type>
int TemplateMapId<Type>::Identifier::operator() () const
{ return m_id ; }
*/

/*
template <class Type>
Type const & TemplateMapId<Type>::Identifier::value() const 
{ return m_TypeId->m_idType[m_id] ; }
*/
/*
template <class Type>
bool TemplateMapId<Type>::Identifier::operator<(Identifier const & id) const 
{
  assert(m_TypeId==id.m_TypeId) ;
  return m_id<id.m_id ;
}
*/
/*
template <class Type>
bool TemplateMapId<Type>::Identifier::operator==(Identifier const & id) const 
{
  assert(m_TypeId==id.m_TypeId) ;
  return m_id==id.m_id ;
}*/
/*
template <class Type>
bool TemplateMapId<Type>::Identifier::operator!=(Identifier const & id) const 
{
  assert(m_TypeId==id.m_TypeId) ;
  return !((*this)==id) ;
}*/
/*
template <class Type>
typename TemplateMapId<Type>::Identifier & TemplateMapId<Type>::Identifier::operator=(Identifier const & id) 
{
  if(m_id!=-1)
    { m_TypeId->dereference(m_id) ; }
  m_id=id.m_id ;
  m_TypeId=id.m_TypeId ;
  if(m_id!=-1)
    { m_TypeId->m_idRef[m_id]++ ; }
  return *this ;
}*/

template <class Type>
typename TemplateMapId<Type>::Identifier TemplateMapId<Type>::reference(Type const & str) 
{
  typename std::map<Type, int>::iterator it=m_TypeId.find(str) ;
  if(it!=m_TypeId.end())
    {
      int id=(*it).second ;
      return Identifier(this, id) ;
    }
  if(m_freed.size()==0)
    {
      int id = m_idRef.size() ;
      m_idRef.push_back(0) ;
      m_idType.push_back(str) ;
      m_TypeId[str]=id ;
      return Identifier(this, id) ;
    }
  int id=m_freed.front() ;
  m_freed.erase(m_freed.begin()) ;
  m_idType[id]=str ;
  m_TypeId[str]=id ;
  return Identifier(this,id) ;
}

template <class Type>
bool TemplateMapId<Type>::exist(Type const & str) const
{
  return m_TypeId.find(str)!=m_TypeId.end() ;
}


template <class Type>
bool TemplateMapId<Type>::contains(Identifier const & id)
{ return id.m_TypeId==this ; }

template <class Type>
void TemplateMapId<Type>::dereference(int id) 
{
  m_idRef[id]-- ;
  if(m_idRef[id]==0)
    {
      typename std::map<Type, int>::iterator f = m_TypeId.find(m_idType[id]) ;
      if(f!=m_TypeId.end())
	{
	  m_freed.push_back(id) ;
	  m_TypeId.erase(m_TypeId.find(m_idType[id])) ;
	}
      if(m_callBack!=NULL) { m_callBack->action(id) ; }
    }
}

template <class Type>
int TemplateMapId<Type>::size() const
{
  return m_idRef.size() ;
}

#endif
