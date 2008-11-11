/*
 *  ComponentRunner.cpp
 *  semaine
 *
 *  Created by Marc Schröder on 12.09.08.
 *  Copyright 2008 DFKI GmbH. All rights reserved.
 *
 */

#include "ComponentRunner.h"


namespace semaine {
namespace system {

ComponentRunner::ComponentRunner(const std::list<Component *> & components) :
components(components)
{
}

ComponentRunner::~ComponentRunner()
{
    std::list<Component *>::iterator it;
	for (it = components.begin(); it != components.end(); it++) {
		Component * c = *it;
		c->requestExit();
		c->join();
		delete c;
	}
}

void ComponentRunner::go()
{
    std::list<Component *>::iterator it;
	for (it = components.begin(); it != components.end(); it++) {
		Component * c = *it;
		c->start();
	}
}

void ComponentRunner::waitUntilCompleted()
{
    std::list<Component *>::iterator it;
	for (it = components.begin(); it != components.end(); it++) {
		Component * c = *it;
		c->join();
	}
}

} // namespace system
} // namespace semaine

