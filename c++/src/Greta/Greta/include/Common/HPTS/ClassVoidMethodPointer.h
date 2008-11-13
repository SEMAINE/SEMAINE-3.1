#ifndef _HPTS_ClassVoidMethodPointer
#define _HPTS_ClassVoidMethodPointer

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

namespace HPTS
{

  /** \brief Pointeur sur une méthode d'une instance d'objet retournant void.

  Cette classe permet de pointer sur une méthode d'un objet de type Based,
  ne prenant pas de paramètres et retournant une valeur de type void.
  De plus, elle conserve un pointeur sur l'instance d'une classe permettant
  d'appeler une méthode particulière d'une instance particulière.
  Il s'agit d'une spécialisation de la classe ClassMethodPointer pour certains
  compilateurs peut permissifs.

  \par 
  L'entête de méthode a donc la signature suivante:
  \par
  void Based::methodName()

  \param Based La classe de l'instance d'objet
  \param ReturnValue Le type de la valeur de retour de la méthode
   */
  template <class Based>
  class ClassVoidMethodPointer
  {
    /** \brief Le type de la méthode pointée */
    public: typedef void (Based::*MethodType)() ;
    
    /** \brief L'instance d'objet sur laquelle on souhaite appeler la méthode. */
    protected: Based * m_object ;
    /** \brief Le pointeur sur la methode. */
    protected: void (Based::*m_method)() ;
    
    /** \brief Constructeur 
	
    \param object L'instance d'objet sur laquelle l'appel de méthode est appliqué.
    \param method Le pointeur sur la méthode à appeler.
    */
    public: inline ClassVoidMethodPointer(Based * object, MethodType method) ;

    /** \brief Opérateur d'appel de méthode.
	
    Cet opérateur permet d'appeler la méthode pointée par m_method sur 
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

