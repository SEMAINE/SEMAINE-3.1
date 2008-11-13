#ifndef _HPTS_ClassVoidMethodPointer
#define _HPTS_ClassVoidMethodPointer

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

namespace HPTS
{

  /** \brief Pointeur sur une m�thode d'une instance d'objet retournant void.

  Cette classe permet de pointer sur une m�thode d'un objet de type Based,
  ne prenant pas de param�tres et retournant une valeur de type void.
  De plus, elle conserve un pointeur sur l'instance d'une classe permettant
  d'appeler une m�thode particuli�re d'une instance particuli�re.
  Il s'agit d'une sp�cialisation de la classe ClassMethodPointer pour certains
  compilateurs peut permissifs.

  \par 
  L'ent�te de m�thode a donc la signature suivante:
  \par
  void Based::methodName()

  \param Based La classe de l'instance d'objet
  \param ReturnValue Le type de la valeur de retour de la m�thode
   */
  template <class Based>
  class ClassVoidMethodPointer
  {
    /** \brief Le type de la m�thode point�e */
    public: typedef void (Based::*MethodType)() ;
    
    /** \brief L'instance d'objet sur laquelle on souhaite appeler la m�thode. */
    protected: Based * m_object ;
    /** \brief Le pointeur sur la methode. */
    protected: void (Based::*m_method)() ;
    
    /** \brief Constructeur 
	
    \param object L'instance d'objet sur laquelle l'appel de m�thode est appliqu�.
    \param method Le pointeur sur la m�thode � appeler.
    */
    public: inline ClassVoidMethodPointer(Based * object, MethodType method) ;

    /** \brief Op�rateur d'appel de m�thode.
	
    Cet op�rateur permet d'appeler la m�thode point�e par m_method sur 
    l'objet pointe par m_object.
     */
    public: inline void operator() () const ;
  } ;
  
  template <class Based>
  ClassVoidMethodPointer<Based>::ClassVoidMethodPointer(Based * object, MethodType method)
    : m_object(object), m_method(method)
  {
  }
  
  template <class Based>
  void ClassVoidMethodPointer<Based>::operator() () const
  {
    (m_object->*m_method)() ;
  }
}

#endif

