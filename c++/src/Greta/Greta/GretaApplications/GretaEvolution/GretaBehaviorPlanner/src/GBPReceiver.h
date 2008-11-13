#pragma once

#include "Psydule.h"
#include "JThread.h"
#include "CentralClock.h"
#include "Definitions.h"
#include <iostream>
#include <utility>

using namespace cmlabs;
// Call to the engine class to allowed reference to it.
class GBPEngine ;

/**
 *  A GBPReceiver class. 
 *  Used to Receive all the needed message from the White board.(i.e. FML message, Baseline data, Behavior Data, Lexicon and clock)
 *  @author Jean Marie BERNARD => jean.M.bernard@gmail.com
 *  @date 2008
 */
class GBPReceiver : public JThread  
{
public:
	/**
	* The default constructor
	*/
	GBPReceiver();
	
	/**
	* The default to use.
	* @param Psydule *psydule the psydule Object adress
	* @param GBPEngine *gbpe the Module Engine Adress
	*/
	GBPReceiver(Psydule *psydule,GBPEngine *gbpe);
	
	/**
	* The destructor
	*/
	virtual ~GBPReceiver();
	/**
	* The thread execution method. Don't use run to launch this method, use start() instead.
	*/
	void run();
	// Getters
	/**
	* The GBPEngine adress getter.
	* @return the adress of the GBPEngine object.
	*/
	GBPEngine * getpGBPEngine();	
private :
	// Attributes
	/** 
	* The owner of this object (A GBPEngine object)
	*/
	GBPEngine *gbpengine;
	/** 
	* The adress of the psydule modules system.
	*/
	Psydule *psydule;
};
