//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// DataContainer.h 
//
//////////////////////////////////////////////////////////////////////
// created by : Radek 

//modifications:
//

#pragma once

//do not change the order of includes!!!
#include "./gestures/Gestuary.h"
#include "./face/ComplexEmotionManager.h"
#include "./face/FaceExpressionDictionary.h"
#include "./face/NewExpressionContainer.h"
#include "./face/IntensityFilter.h"
#include "./face/LipData.h"
#include "./head/HeadGestuary.h"
#include "./torso/TorsoGestuary.h"
#include "./mmemotions/ConstraintsContainer.h"
#include "./listener/ListenerData.h"

using namespace GestureSpace;
using namespace HeadSpace;
using namespace TorsoSpace;


/**
* class :DataContainer
*
*/

class DataContainer
{

private:


	ConstraintsContainer *constraintscontainer;

	FaceExpressionDictionary *faceexpressiondictionary;
	NewExpressionContainer	*newexpressioncontainer;
	IntensityFilter * intfilt;
	ComplexEmotionManager *complexemotionmanager;
	LipData *lipdata;
	ListenerData *listenerdata;
	Gestuary *gestuary;
	HeadGestuary *headgestuary;

	TorsoGestuary *torsogestuary;
	TorsoMovementPoint restposition;

	//complex facial exptessions
	XMLGenericTree *cfe_lib;
	bool cfe_initialized;


public:

	/**
	* contructor 
	*
	*/

	DataContainer();

	/**
	* destructor 
	*/

	virtual ~DataContainer();

	//complexyes = 1 - use complex
	//complexyes = 0 - do not use complex


	/**
	* 
	*
	* @return 
	*/

	int initAll(std::string directory);


	/**
	*  
	* 
	*
	* @return 
	*/

	int initComplexFacialExpressions();	

	/**
	* 
	*
	* @return 
	*/

	int initBackChannel(std::string directory);

	/**
	* 
	*
	* @return 
	*/

	int initFMLEngine();

	/**
	*  
	* 
	*
	* @return 
	*/

	int initBMLEngine();

	/**
	* 
	*
	* @return 
	*/

	int initPlayer();

	bool backchannelInitialized;
	bool FMLInitialized;
	bool BMLInitialized;
	bool playerInitialized;
	bool complexExpressions;


	/**
	* 
	*
	* @return 
	*/

	FaceExpressionDictionary* getFaceExpressionDictionary();

	/**
	* 
	*
	* @return 
	*/

	IntensityFilter* getIntensityFilter();

	/**
	*  
	* 
	*
	* @return 
	*/

	NewExpressionContainer* getNewExpressionContainer();

	/**
	* 
	*
	* @return 
	*/

	ComplexEmotionManager* getComplexEmotionManager();

	/**
	* 
	*
	* @return 
	*/

	LipData* DataContainer::getLipData();

	/**
	*  
	* 
	*
	* @return 
	*/

	ListenerData* DataContainer::getListenerData();

	/**
	*  
	*
	* @return 
	*/

	Gestuary* DataContainer::getGestuary();

	/**
	*  
	*
	* @return 
	*/

	HeadGestuary* getHeadGestuary();

	/**
	* 
	*
	* @return 
	*/

	TorsoGestuary* getTorsoGestuary();

	/**
	* 
	*
	* @return 
	*/

	TorsoMovementPoint getRestPosition();

	/**
	* 
	*
	* @return 
	*/

	ConstraintsContainer* getContraints();

	/**
	* 
	*
	* @return 
	* @param  complexemotionlibraryfilename
	*/

	int ReadComplexFacialExpressionLibrary(std::string complexemotionlibraryfilename);

	/**
	* 
	*
	* @return 
	*/

	XMLGenericTree *getComplexFacialExpressionLibrary();

};
