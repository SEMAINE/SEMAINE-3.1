#ifndef _HPTS_ClassMethodPointer
#define _HPTS_ClassMethodPointer

//#include <Presentation.h>
#include <HPTS/hpts_dll.h>

namespace HPTS
{

  /** \brief Pointeur sur une méthode d'une instance d'objet.

  Cette classe permet de pointer sur une méthode d'un objet de type Based,
  ne prenant pas de paramètres et retournant une valeur de type ReturnValue.
  De plus, elle conserve un pointeur sur l'instance d'une classe permettant
  d'appeler une méthode particulière d'une instance particulière.

  \par 
  L'entête de méthode a donc la signature suivante:
  \par
  ReturnValue Based::methodName()

  \param Based La classe de l'instance d'objet
  \param ReturnValue Le type de la valeur de retour de la méthode
   */
  template <class Based, class ReturnValue>
  class ClassMethodPointer
  {
    /** \brief Le type de la méthode pointée */
    public: typedef ReturnValue (Based::*MethodType)() ;
    
    /** \brief L'instance d'objet sur laquelle on souhaite appeler la méthode. */
    protected: Based * m_object ;
    /** \brief Le pointeur sur la methode. */
    protected: ReturnValue (Based::*m_method)() ;
    
    /** \brief Constructeur 
	
    \param object L'instance d'objet sur laquelle l'appel de méthode est appliqué.
    \param method Le pointeur sur la méthode à appeler.
    */
    public: inline ClassMethodPointer(Based * object, MethodType method) ;

    /** \brief Opérateur d'appel de méthode
	
    Cet opérateur permet d'appeler la méthode pointée par m_method sur 
    l'objet pointe par m_object.
     */
    public: inline ReturnValue operator() () const ;
  } ;
  
  template <class Based, class ReturnValue>
  ClassMethodPointer<Based, ReturnValue>::ClassMethodPointer(Based * object, MethodType method)
    : m_object(object), m_method(method)
  {
  }
  
  template <class Based, class ReturnValue>
  ReturnValue ClassMethodPointer<Based, ReturnValue>::operator() () const
  {
    return (m_object->*m_method)() ;
  }
}

#endif

