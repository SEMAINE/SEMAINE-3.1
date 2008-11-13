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

// DataContainer.cpp: implementation of the  DataContainer class.
//
//////////////////////////////////////////////////////////////////////
//created by:
//
//modifications:
//

#include "DataContainer.h"
#include "IniManager.h"
#include <stdio.h>

extern IniManager inimanager;
FILE* data_log=0;

using namespace GestureSpace;
using namespace HeadSpace;
using namespace MMSystemSpace;
using namespace TorsoSpace;


DataContainer::DataContainer()
{

	//inimanager.ReadIniFile("greta.ini");
	faceexpressiondictionary = NULL;
	newexpressioncontainer=NULL;
	intfilt=NULL;
	complexemotionmanager =NULL;

	lipdata=NULL;
	gestuary=NULL;
	headgestuary=NULL;
	torsogestuary=NULL;


	backchannelInitialized=false;
	FMLInitialized=false;
	BMLInitialized=false;
	playerInitialized=false;
	complexExpressions=false;


	std::string logname;
	logname=inimanager.Program_Path+"logs/datacontainer_log.txt";
	data_log=fopen(logname.c_str(),"w");


	cfe_initialized=false;
	cfe_lib=NULL;

	if(data_log==NULL)
	{
		printf("cannot creat face log file (probably the log folder is missing)\n");		
	}


}

DataContainer::~DataContainer()
{

	fclose(data_log);

	if (faceexpressiondictionary!=NULL) delete faceexpressiondictionary;

	if (intfilt!=NULL) delete intfilt;

	if (complexemotionmanager!=NULL) delete complexemotionmanager;

	if (newexpressioncontainer!=NULL) delete newexpressioncontainer;

	if (lipdata!=NULL) delete lipdata;

	if (gestuary!=NULL) delete gestuary;

	if (headgestuary!=NULL) delete headgestuary;

	if (torsogestuary!=NULL) delete torsogestuary;

	if (constraintscontainer!=NULL) delete constraintscontainer;

	if (cfe_lib!=NULL) delete cfe_lib;

}


// not used; java version is not used 

int DataContainer::initComplexFacialExpressions()
{

	//printf("Making connections with Psyclone \n");

	complexemotionmanager=new ComplexEmotionManager();
	if (complexemotionmanager->registered==false) 
	{
		//throw Exception ("Psyclone connection is not available", 1);
		return 0;
	}

	//
	complexemotionmanager->ReadLibrary(inimanager.Program_Path+".\\complexemotions.xml");
	if (complexemotionmanager->initialized==false) 
	{
		//throw Exception ("EARL list cannot be found", 2);
		return 0;
	}

	complexExpressions=true;
	return 1;
}



int DataContainer::initBackChannel(std::string directory)
{

	//analysisrules.xml
	//backchannellexicon.xml
	//initialstate.xml
	//rules.xml

	printf("Loading listener data... ");

	listenerdata=new ListenerData();
	listenerdata->LoadAll(directory);

	printf("done\n");

	backchannelInitialized=true;

	return 1;
}

int DataContainer::initFMLEngine()
{

	FMLInitialized=true;
	return 1;
}



int DataContainer::initBMLEngine()
{

	printf("Loading facelibrary... ");

	faceexpressiondictionary=new FaceExpressionDictionary();

	int code=faceexpressiondictionary->Init((inimanager.Program_Path+inimanager.GetValueString("FACELIBRARY_PATH")).c_str());

	if (code==0) 
	{
		//throw Exception ("FaceLibrary Exception", 1);
		return 0;
	}
	printf("done\n");


	printf("Loading extended definition of facial expressions... ");

	newexpressioncontainer = new NewExpressionContainer();
	code = newexpressioncontainer->init(inimanager.Program_Path+".\\fd\\efe");			

	if (code==0) 
	{
		//throw Exception ("FaceLibrary Exception", 1);
		return 0;
	}

	intfilt = new IntensityFilter ();
	code = intfilt->init (inimanager.Program_Path+".\\fd\\efe\\xmls\\D_emotions.xml");
	if (code==0) 
	{
		//throw Exception ("FaceLibrary Exception", 1);
		return 0;
	}
	printf("done\n");


	printf("Loading gestures... ");

	gestuary = new Gestuary();
	gestuary ->LoadLibrary();

	printf("done\n");

	printf("Loading head movements... ");

	headgestuary = new HeadGestuary();

	code=headgestuary ->LoadGestuary(inimanager.Program_Path+".\\head\\gestuary.xml");
	if (code==0) 
	{
		//throw Exception ("HeadGestuary Exception", 1);
		return 0;
	}
	printf("done\n");


	printf("Loading torso movements... ");

	restposition.LoadPose("rest.txt");
	restposition.compensatehead=1;
	restposition.compensateshoulders=1;

	torsogestuary = new TorsoGestuary();
	code=torsogestuary ->LoadGestuary(inimanager.Program_Path+".\\torso\\gestuary.xml",restposition);
	if (code==0) 
	{
		//throw Exception ("TorsoGestuary Exception", 1);
		return 0;
	}
	printf("done\n");

	///////////////////////////////////////////////

	printf("Loading lips movements... ");

	lipdata=new LipData();
	lipdata->ReadPhonemes();

	printf("done\n");


	///////////////////////////////////////////////

	printf("Loading constraints... ");

	constraintscontainer=new ConstraintsContainer();

	code=constraintscontainer->init(inimanager.Program_Path+".\\fd\\mfe\\example.xml");

	if (code==0) 
	{
		//throw Exception ("ConstraintsContainer Exception", 1);
		return 0;
	}

	printf("done\n");

	///////////////////////////////////////////////

	//load short version of complex facial expressions
	printf("Loading CFE-EARL library... ");

	code=this->ReadComplexFacialExpressionLibrary(inimanager.Program_Path+".\\complexemotions.xml");
	if (code==0) 
	{
		//throw Exception ("CFE EARL list cannot be found", 1);
		return 0;
	}

	printf("done\n");

	/////////////////////////////////////////////////////

	BMLInitialized=true;
	return 1;

}

int DataContainer::initPlayer()
{

	playerInitialized=true;
	return 1;
}

// INFO;	
//GretaModular - all
//BMLRealtimePsydule - bml 
//IntenstionPlaner - all
//bmlpsydule - bml
//behaviour planeer - all
//bmleditor - bml
//facelibrayvewer - bml
//gesturetester - bml
//gestureeditror - bml


int DataContainer::initAll(std::string directory)
{

	int code = 1;

	code*= this->initBackChannel(directory);
	code*= this->initBMLEngine();
	code*= this->initFMLEngine();

	//java version is not used 
	// this->initComplexFacialExpressions();

	code*= this->initPlayer();

	if (code==0) {
		//throw Exception ("Data Cannot Be Initialized", 1);
		return 0;
	}

	return 1;

}



FaceExpressionDictionary *DataContainer::getFaceExpressionDictionary()
{
	return faceexpressiondictionary;
}

IntensityFilter* DataContainer::getIntensityFilter()
{
	return intfilt;
}

NewExpressionContainer* DataContainer::getNewExpressionContainer()
{
	return newexpressioncontainer;
}

ComplexEmotionManager* DataContainer::getComplexEmotionManager()
{
	if (complexemotionmanager!=NULL) return complexemotionmanager;
	return NULL;
}


LipData* DataContainer::getLipData()
{
	return lipdata;
}

ListenerData* DataContainer::getListenerData()
{
	return listenerdata;
}

Gestuary* DataContainer::getGestuary() 
{
	return gestuary;
}

HeadGestuary* DataContainer::getHeadGestuary() 
{
	return headgestuary;
}

TorsoGestuary* DataContainer::getTorsoGestuary() 
{
	return torsogestuary;
}

TorsoMovementPoint DataContainer::getRestPosition() 
{
	return restposition;
}

ConstraintsContainer* DataContainer::getContraints()
{
	return constraintscontainer;
}



XMLGenericTree *DataContainer::getComplexFacialExpressionLibrary()
{
	if (cfe_initialized==true) return cfe_lib;
	return NULL;
}


int DataContainer::ReadComplexFacialExpressionLibrary(std::string complexemotionlibraryfilename)
{
	XMLGenericParser p;
	p.SetValidating(false);
	this->cfe_lib=p.ParseFile(complexemotionlibraryfilename);
	if(this->cfe_lib==0)
	{
		cfe_initialized=false;		
		return 0;
	}
	cfe_initialized=true;
	return 1;
}
