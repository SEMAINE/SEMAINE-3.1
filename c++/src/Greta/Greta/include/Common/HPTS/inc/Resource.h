#ifndef _HPTS_Resource_H
#define _HPTS_Resource_H

#include <Presentation.h>
#include <MapStringId.h>

namespace HPTS
{
  /** \brief Classe de description des ressources.

  Cette classe repr�sente une ressource fournie par les contr�leurs de HPTS++.
  \warning Elle ne doit jamais �tre construite par l'utilisateur mais seulement par un contr�leur.
  \sa HPTS::StateMachineController
   */
  class Resource
  {
    /** \brief L'identifiant correspondant � la ressource */
    MapStringId::Identifier m_identifier ;
    
    /** \brief Constructeur de ressource 
	
    Ce constructeur r�f�rence la ressource via son identifiant sous forme de cha�ne
    de caract�res dans la table des identifiants.
    \param mapStringId Table des identifiants dans laquelle les ressources sont r�f�renc�es.
    \param name La cha�ne de caract�res identifiant la ressource.
    */
    Public Resource(MapStringId & mapStringId, std::string const & name)
      : m_identifier(mapStringId.reference(name)) 
    {}

    /** \brief R�cup�ration de l'identifiant de la ressource */
    MapStringId::Identifier const & identifier() const
    { return m_identifier ; }

    /** \brief R�cup�ration de la cha�ne de caract�res identifiant cette ressource. */
    std::string const & name() const
    { return m_identifier.value() ; }
  } ;
  
}

#endif

