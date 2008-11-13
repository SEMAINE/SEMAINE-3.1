#ifndef _HPTS_Resource_H
#define _HPTS_Resource_H

#include <Presentation.h>
#include <MapStringId.h>

namespace HPTS
{
  /** \brief Classe de description des ressources.

  Cette classe représente une ressource fournie par les contrôleurs de HPTS++.
  \warning Elle ne doit jamais être construite par l'utilisateur mais seulement par un contrôleur.
  \sa HPTS::StateMachineController
   */
  class Resource
  {
    /** \brief L'identifiant correspondant à la ressource */
    MapStringId::Identifier m_identifier ;
    
    /** \brief Constructeur de ressource 
	
    Ce constructeur référence la ressource via son identifiant sous forme de chaîne
    de caractères dans la table des identifiants.
    \param mapStringId Table des identifiants dans laquelle les ressources sont référencées.
    \param name La chaîne de caractères identifiant la ressource.
    */
    Public Resource(MapStringId & mapStringId, std::string const & name)
      : m_identifier(mapStringId.reference(name)) 
    {}

    /** \brief Récupération de l'identifiant de la ressource */
    MapStringId::Identifier const & identifier() const
    { return m_identifier ; }

    /** \brief Récupération de la chaîne de caractères identifiant cette ressource. */
    std::string const & name() const
    { return m_identifier.value() ; }
  } ;
  
}

#endif

