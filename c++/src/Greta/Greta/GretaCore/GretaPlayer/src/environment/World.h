#pragma once

#include <vector>
#include "WorldObject.h"
#include "../AgentAspect.h"

/**
*
* class :World
*
*/

class World
{
public:

	/**
	*
	* contructor 
	*
	*/

	World(void);

	/**
	*
	* destructor 
	*/

	~World(void);

	/**
	*  
	* 
	*
	*/

	void draw();

	/**
	*  
	* 
	*
	* @param  name
	* @param  objectshape
	*/

	void AddWorldObject(std::string name,std::string objectshape);

	/**
	*  
	* 
	*
	* @param  name
	* @param  character
	*/

	void AddAgent(std::string name,std::string character);

	/**
	*  
	* 
	*
	* @param  filename
	*/

	void LoadWorld(std::string filename);

	/**
	*  
	* 
	*
	* @return 
	* @param  name
	*/

	WorldObject *FindWorldObject(std::string name);

	/**
	*  
	* 
	*
	* @return 
	* @param  name
	*/

	AgentAspect *FindAgent(std::string name);
	std::vector<WorldObject *> objects;
	std::vector<AgentAspect *> agents;
	typedef std::vector<WorldObject *>::iterator objectsiterator;
	typedef std::vector<AgentAspect *>::iterator agentsiterator;

};
