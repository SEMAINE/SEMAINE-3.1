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

// IdleMovments.h: implementation for the IdleMovments class.
//
//////////////////////////////////////////////////////////////////////

//generate the continuation of the facial expressions 
//used in the realtime version


#include "IdleMovements.h"
#include "FAPFrame.h"
#include "IniManager.h"
#include "RandomGen.h"

extern IniManager inimanager;
extern RandomGen *randomgen;


IdleMovements::IdleMovements()
{
	randomgen = new RandomGen();	
}


IdleMovements::~IdleMovements()
{

}

void IdleMovements::generate(FAPFrame lastframe, float animationlength) 
{

	//lastframe.SetFAP(48,0,true);
	//lastframe.SetFAP(49,0,true);
	//lastframe.SetFAP(50,0,true);
	//lastframe.SetFAP(19,0,true);
	//lastframe.SetFAP(20,0,true);
	//lastframe.SetFAP(21,0,true);
	//lastframe.SetFAP(22,0,true);

	//printf( "idle insidelastframe 48 %i, 49 %i, 50 %i \n",lastframe.FAPs[48].value,lastframe.FAPs[49].value,lastframe.FAPs[50].value);

	FramesTotalNumber= (int) ceil (animationlength*FRAMERATE);

	FaceInterp=new FaceInterpolator(FramesTotalNumber,NULL);

	FaceInterp->InitKeyFrames();
	InitializeFaps();

	fapframevector->resize(0);

	FAPFrame one=lastframe.clone();
	one.ActivateAllFAPs();
	fapframevector->push_back(one);	

	int frameind=2;

	for(frameind;frameind<(int)FramesTotalNumber/4;frameind++){
		FAPFrame newone;
		for(int iii=0;iii<69;iii++){
			if ((iii!=48)&&(iii!=49)&&(iii!=50))  newone.SetFAP(iii,0,false);				
			else newone.SetFAP(iii,lastframe.GetFAP(iii),false);
		}
		newone.id=frameind+1;
		fapframevector->push_back(newone);
	}

	//TO DO: randomly max *+-10% 
	FAPFrame two = lastframe.clone();
	two.ActivateAllFAPs();
	for(int iii=0;iii<69;iii++){
		if ((iii!=48)&&(iii!=49)&&(iii!=50)&&(iii!=20)&&(iii!=19)&&(iii!=21)&&(iii!=22)) {
			two.SetFAP(iii, (int)(two.GetFAP(iii)*( 1 - (0.1*randomgen->GetRand01() ) ) ) ,true);						
		}
	}
	fapframevector->push_back(two);


	for(frameind;frameind<(int)FramesTotalNumber/2;frameind++){
		FAPFrame newone;
		for(int iii=0;iii<69;iii++){
			if ((iii!=48)&&(iii!=49)&&(iii!=50))  newone.SetFAP(iii,0,false);				
			else newone.SetFAP(iii,lastframe.GetFAP(iii),false);
		}
		newone.id=frameind+1;
		fapframevector->push_back(newone);
	}


	//TO DO: randomly max *+-10% 
	FAPFrame three= lastframe.clone();
	three.ActivateAllFAPs();
	for(int iii=0;iii<69;iii++){
		if ((iii!=48)&&(iii!=49)&&(iii!=50)&&(iii!=20)&&(iii!=19)&&(iii!=21)&&(iii!=22)) {
			three.SetFAP(iii, (int)( three.GetFAP(iii)*( 1 - (0.1*randomgen->GetRand01() ) ) ) ,true);						
		}
	}
	fapframevector->push_back(three);


	for(frameind;frameind<FramesTotalNumber-2;frameind++){
		FAPFrame newone;
		for(int iii=0;iii<69;iii++){
			if ((iii!=48)&&(iii!=49)&&(iii!=50))  newone.SetFAP(iii,0,false);				
			else newone.SetFAP(iii,lastframe.GetFAP(iii),false);
		}
		newone.id=frameind+1;
		fapframevector->push_back(newone);
	}


	FAPFrame four = lastframe.clone();	
	four.ActivateAllFAPs();
	fapframevector->push_back(four);

	FaceInterp->InterpolateFaps_NoHead(fapframevector, PWR->GetValue(), FLD->GetValue());

	//FaceInterp->InterpolateHead(fapframevector, faps_head, fapmask_head, faps_head_ns, fapmask_head_ns, PWR->GetValue(), FLD->GetValue());

	//int index=1;
	//std::vector<FAPFrame>::iterator iter;
	//for(iter=fapframevector->begin();iter!=fapframevector->end();iter++)  (*iter).framenumber=index++;

	fapframevector->resize(fapframevector->size()-2);

	//save a copy of facial expressions
	fapframevectorwithoutnoise->resize(0);

	std::vector<FAPFrame>::iterator iter;
	for(iter=fapframevector->begin();iter!=fapframevector->end();iter++) 
		(*fapframevectorwithoutnoise).push_back(iter->clone());


	ApplyBioBlink();

	if (inimanager.GetValueInt("HEAD_NOISE")!=0)
	{
		AddNoise((int)((FramesTotalNumber+5)/4)); 
	}

}


