/*
 *  ComponentRunner.h
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */


#ifndef SEMAINE_SYSTEM_COMPONENTRUNNER_H
#define SEMAINE_SYSTEM_COMPONENTRUNNER_H

#include <list>

#include <semaine/config.h>

#include <semaine/components/Component.h>

using namespace semaine::components;

namespace semaine {
namespace system {

class ComponentRunner 
{
public:
	ComponentRunner(const std::list<Component *> & components);
	~ComponentRunner();

	/**
	 * Start each component in a separate thread, and return.
	 */
	void go();
	
	/**
	 * Wait for each component to complete. If the components do not terminate,
	 * this method blocks forever.
	 */
	void waitUntilCompleted();


private:
	std::list<Component *> components;

};

} // namespace system
} // namespace semaine


#endif

