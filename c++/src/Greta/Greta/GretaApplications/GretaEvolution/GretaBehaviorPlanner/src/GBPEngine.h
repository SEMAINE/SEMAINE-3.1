#pragma once

#include "GBPReceiver.h"
#include "GBPSender.h"
#include "GBPTranscoderThread.h"
#include "Definitions.h"
#include "LogDefinitions.h"
#include "ErrorDefinition.h"
#include "Psydule.h"
#include "CentralClock.h"
#include <conio.h>
#include <stdio.h>
#include <iostream>
#include <utility>
/**
 *  A GBPEngine class. 
 *  Used to initialize and run the system.
 *  @author Jean Marie BERNARD => jean.M.bernard@gmail.com
 *  @date 2008
 */
class GBPEngine  
{
public:
	/**
	* The GBPEngine Constructor Class. Convention : 
	* getObject to obtain a copy of the object
	* getpObject to obtain the adress of the object
	* @param iniFileName : the name of the module's initialisation file 
	*/
	GBPEngine(std::string iniFileName);
	/**
	* The GBPEngine Destructor Class.
	*/
	~GBPEngine();

	// Methods :
	/**
	* Initialise the application. Register the module to psyclone.
	* @return 1 if the initialisation is successful,
	0 in other case.
	*/
	int initApplication();
	/**
	* Initialise the receiver. Register to receive message from the psyclone module
	* @return 1 if the registration is successful,
	0 in other case.
	*/
	int initReceiver();
	/**
	* Initialise the sender. Register to send message to the psyclone module
	* @return 1 if the registration is successful,
	0 in other case.
	*/
	int initSender();
	/**
	* Initialise the transcoder thread. Create and start the transcoding process.
	* @return 1 if the initialisation is successful,
	0 in other case.
	*/
	int initTranscoderThread();
	/**
	* The execution methods.
	* @return 1 if the execution is successful,
	0 in other case.
	*/
	int run();
	/**
	* The stopping methods.
	* @return 1 if the stopping is successful,
	0 in other case.
	*/
	int quit();
	// Getters : 
	// Central Clock's getters
	/**
	* The central clock getter.
	* @return a copy of the central clock object.
	*/
	CentralClock getCentralClock();
	/**
	* The central clock adress getter.
	* @return the adress of the central clock object.
	*/
	CentralClock  * getpCentralClock();
	// Transcoder getters
	/**
	* The Transcoder getter.
	* @return a copy of the Transcoder object.
	*/
	GBPTranscoderThread getGBPTranscoderThread();
	/**
	* The Transcoder adress getter.
	* @return the adress of the Transcoder object.
	*/
	GBPTranscoderThread * getpGBPTranscoderThread();

	// sender getter
	/**
	* The Sender adress getter.
	* @return the adress of the Sender object.
	*/
	GBPSender * getpGBPSender();
    
private:
	// Attributes :
	/** The FML receiver */
	GBPReceiver *fmlRec ;
	/** The whiteboard module adress */	
	std::string psyduleHost;
	/** The whiteboard module port */	
	int psydulePort;
	/** The Psydule interface */
	Psydule *psydule;
	/** Computer central clock */
	CentralClock pc;
	/** The BML sender */
	GBPSender *gbpsender;
	/** The iniFileName of the module */
	std::string iniFileName;
	/** The FML to BML Transcoder system */
	GBPTranscoderThread  gbptranscoderThread ;

};