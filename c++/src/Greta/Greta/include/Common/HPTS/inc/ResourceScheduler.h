#ifndef _ResourceScheduler_H
#define _ResourceScheduler_H

#include <VirtualScheduler.h>

namespace ResourceScheduler
{
  /** \brief Classe d'ordonnanceur avec ressources. */
  class HPTS_DLL ResourceScheduler : public VirtualScheduler
  {
    template <class Type>
    class Chain
    {
      Protected Type    m_value ;
      Protected Chain * m_next ;
      
      Public Chain()
	: m_next(NULL)
      {}

      Public Chain(Type const & value)
	: m_value(value), m_next(NULL)
      {}
      
      Public Type const & value() const
      {
	return m_value ;
      }

      Public void setNext(Chain * ch)
      { m_next = ch ; }
      
      Public Chain * next()
      { return m_next ; }
    } ;
    
    // Differents types utiles
    Public typedef std::pair<Set,Set>                        StateResources ;
    Public typedef ResourceScheduler::Chain<MetaState*> StateList ;
    Public typedef std::pair<double, StateList*>             WeightStateList ;
    Public typedef std::map<StateResources, WeightStateList> ComputationMap ;
    Public typedef Chain<MetaState*> StateChain ;
    
    Private std::list<StateChain*>   m_chainList ;

    // Implementation de l'ordonnancement
    Public virtual void schedule() ;

    // Recuperation d'un nouveau chainon
    Protected StateChain * newChain() ;

    Protected StateChain * newChain(MetaState * value) ;

    // Liberation des chainons alloues
    Protected void freeChain() ;

    Protected void addConfiguration(ComputationMap & result,  std::pair<StateResources, WeightStateList> const & configuration,
				    MetaState * state) ;

    Protected void compute(ComputationMap & result, std::pair<StateResources, WeightStateList> const & configuration, 
			   VirtualScheduler::MetaStateSet const & stateSet, double expectedWeight) ;
    Protected void compute(ComputationMap & computationMap, 
			   VirtualScheduler::MetaStateSet const & stateSet,
			   double expectedWeight) ;

    Public virtual ~ResourceScheduler() ;
  } ;
}

#endif

