#pragma once

#include "Psydule.h"
#include <iostream>
#include <utility>
#include <string>
#include <map>
#include "FML-APML_AgentEngine.h"

using namespace cmlabs;
// Call to the engine class to allox reference to it.
class GBPEngine;


/**
 *  A GBPTranscoderThread class. 
 *  Used to Convert FML to BML. This object use An FML_Apml Agent engine..
 *  @author Jean Marie BERNARD => jean.M.bernard@gmail.com
 *  @date 2008
 */
class GBPTranscoderThread 
{
public:
	/** 
	* Default constructor
	*/
	GBPTranscoderThread();
	/** 
	* Constructor to use
	* @param GBPEngine *gbpe the Module Engine Adress
	*/
	GBPTranscoderThread(GBPEngine *gbpe);
	/**
	* Destructor
	*/
	virtual ~GBPTranscoderThread();
	/**
	* Execution Methods.
	* When a message is dropped into the message list, this methods call the FML/Apml agent engine to convert the Received FML into BML, and send it into the sender buffer.
	*/
	void run();
	/**
	* Add an FML message into the FML message list (buffer)
	* @param JString mes a Jstring message in the FML format
	*/
	int addMessage(std::string mes);
	/**
	* Load the baseline into the FML APML Agent engine. Store baseline filename or buffer into the memory. Load the buffer if the baselinefilename="".
	* @param std::string baselinefilename The Baseline File Name
	* @param char * buffer The baseline buffer
	*/
	int loadBaseline(std::string baselinefilename,char *buffer);
	/**
	* Load the Behavior Qualiifiers into the FML APML Agent engine. Store Behavior Qualiifiers  filename or buffer into the memory. Load the buffer if the bqcfilename="".
	* @param std::string bqcfilename The Behavior Qualiifiers File Name
	* @param char * buffer The Behavior Qualiifiers buffer
	*/
	int loadBQC(std::string bqcfilename,char *buffer);
	/**
	* Load the lexicon into the FML APML Agent engine. Store lexicon  filename or buffer into the memory. Load the buffer if the mmsfilename="".
	* @param std::string mmsfilename The lexicon File Name
	* @param char * buffer The lexicon buffer
	*/
	int loadMMS(std::string mmsfilename,char *buffer);

private:
	// Attributes
	/**
	* The module Engine
	*/
	GBPEngine *gbpengine;
	/**
	* The FML message buffer 
	*/
	std::list<std::string> messageList;	
	/**
	* The Baseline loading state (1 = Baseline loaded / 0= Baseline empty)
	*/
	int baselineLoaded;
	/**
	* The Behavior Qualiifiers loading state (1 = Behavior Qualiifiers loaded / 0= Behavior Qualiifiers empty)
	*/
	int bqcLoaded;
	/**
	* The lexicon loading state (1 = lexicon loaded / 0= lexicon empty)
	*/
	int mssLoaded;
	/**
	* The FML_APML Agent engine adress
	*/
	FMLAPML_AgentEngine *AAE;
	/**
	* The Baseline Buffer
	*/
	JString baselineBuffer;
	/**
	* The Baseline File name
	*/
	JString baselineFilename;
	/**
	* The Behavior Qualiifiers Buffer
	*/
	JString bqcBuffer;
	/**
	* The Behavior Qualiifiers File name
	*/
	JString bqcFilename;
	/**
	* The lexicon Buffer
	*/
	JString mssBuffer;
	/**
	* The lexicon File name
	*/
	JString mssFilename;
	/**
	* The cutting method
	*/
	int Chunk(std::string buffer);
	std::list<std::string> taglist;	
};