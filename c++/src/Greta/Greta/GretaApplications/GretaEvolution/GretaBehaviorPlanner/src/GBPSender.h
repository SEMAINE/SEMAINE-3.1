#pragma once

#include <iostream>
#include <utility>
#include <string>
#include "JThread.h"
#include "Psydule.h"
#include "JString.h"
#include "Definitions.h"

using namespace cmlabs;

/**
 *  A GBPSender class. 
 *  Used to initialize and run the system.
 *  @author Jean Marie BERNARD => jean.M.bernard@gmail.com
 *  @date 2008
 */
class GBPSender : public JThread  
{
public:
	/**
	* Default constructor
	*/
	GBPSender();	
	/**
	* Constructor to use
	* @param Psydule *psydule the psydule Object adress
	*/
	GBPSender(Psydule *psydule);
	/**
	* Destructor
	*/
	~GBPSender();

	// Methods
	/**
	* The execution methods. Send message if the input buffer isn't empty.
	*/
	void run();
	/**
	* Add a message to send into the Message buffer.
	* @param std::string mes the message to send
	*/
	int addMessage(std::string mes);

private:
	// Attributes
	/**
	* The psydule interface adress
	*/
	Psydule *psydule;
	/**
	* The Sender list of messages
	*/
	std::list<std::string> messageList;
};