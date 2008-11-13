#ifndef _ResourceScheduler_ResourceScheduler_H
#define _ResourceScheduler_ResourceScheduler_H

#include <HPTS/VirtualScheduler.h>

namespace ResourceScheduler
{
  /** \brief Classe d'ordonnanceur avec ressources. */
  class HPTS_DLL ResourceScheduler : public VirtualScheduler
  {
    template <class Type>
    class Chain
    {
      protected: Type    m_value ;
      protected: Chain * m_next ;
      
      public: Chain()
	: m_next(NULL)
      {}

      public: Chain(Type const & value)
	: m_value(value), m_next(NULL)
      {}
      
      public: Type const & value() const
      {
	return m_value ;
      }

      public: void setNext(Chain * ch)
      { m_next = ch ; }
      
      public: Chain * next()
      { return m_next ; }
    } ;
    
    // Differents types utiles
    public: typedef std::pair<Set,Set>                        StateResources ;
    public: typedef ResourceScheduler::Chain<MetaState*> StateList ;
    public: typedef std::pair<double, StateList*>             WeightStateList ;
    public: typedef std::map<StateResources, WeightStateList> ComputationMap ;
    public: typedef Chain<MetaState*> StateChain ;
    
    private: std::list<StateChain*>   m_chainList ;

    // Implementation de l'ordonnancement
    public: virtual void schedule() ;

    // Recuperation d'un nouveau chainon
    protected: StateChain * newChain() ;

    protected: StateChain * newChain(MetaState * value) ;

    // Liberation des chainons alloues
    protected: void freeChain() ;

    protected: void addConfiguration(ComputationMap & result,  std::pair<StateResources, WeightStateList> const & configuration,
				    MetaState * state) ;

    protected: void compute(ComputationMap & result, std::pair<StateResources, WeightStateList> const & configuration, 
			   VirtualScheduler::MetaStateSet const & stateSet, double expectedWeight) ;
    protected: void compute(ComputationMap & computationMap, 
			   VirtualScheduler::MetaStateSet const & stateSet,
			   double expectedWeight) ;

    public: virtual ~ResourceScheduler() ;
  } ;
}

#endif

