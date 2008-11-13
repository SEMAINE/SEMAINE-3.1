//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
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

// GesturePhase.cpp: implementation of the GesturePhase class.
//
//////////////////////////////////////////////////////////////////////

#include "GesturePhase.h"
#include "IniManager.h"

extern IniManager inimanager;

/**@#-*/
using namespace std ;
using namespace GestureSpace;
/**@#+*/

extern FILE* data_log;
//define OUTPUT

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



GesturePhase::GesturePhase(GesturePhaseType t, float tm, float c, SideType s)
{
	v.clear();
	type		=t;
	time		=tm;
	certainty	=c;
	side		=s;
	output=false;
	currentHand	=NULL;
	currentArm	=NULL;
	currentWrist=NULL;
	mLoopType	=LOOP_NONE;
	transition	=NULL;
	PWR=-2.0f;
}

GesturePhase::~GesturePhase()
{
	currentHand	=NULL;
	currentArm	=NULL;
	currentWrist=NULL;
	/*
	GestureComponentVector::iterator iter,iter2;
	for(iter=v.begin();iter!=v.end();iter++)
	{
		GestureComponent *gc;

		gc=(*iter);

		for(iter2=v.begin();iter2!=v.end();iter2++)
		{
			if(((*iter)==(*iter2))&&((*iter)!=0))
				*iter2=0;
		}

		if(gc!=0)
			delete gc;

		(*iter)=0;
	}*/
	v.clear();
	delete transition;
}

void GesturePhase::GetAngles(BAPFrame &f)
{
	// Concretize call moved in here to minimize
	// non-transparent dependencies June 2004 Bjoern
	Concretize();

	GestureComponentVector::iterator iter;
	if(!v.empty())
		for(iter=v.begin(); iter!=v.end(); iter++)
		{
			(*iter)->GetAngles(f);
		}
	
}

void GesturePhase::Print()
{
	fprintf(data_log,"P GesturePhase::Print()\n");
	fprintf(data_log,"  AbsTime: %2.2f; time: %2.2f\n",mAbsTime,time);
	GestureComponentVector::iterator iter;
	for(iter=v.begin(); iter!=v.end(); iter++) {
		(*iter)->Print();
	}

}

void GesturePhase::Start()
{
	currentHand= new GestureSpace::HandShape;
	currentHand->side=side;
	currentHand->type=shape_default;
	currentHand->shape=NULL;

	currentArm = new GestureSpace::ArmPosition;
	currentArm->side=side;
		
	currentWrist = new GestureSpace::WristOrientation;
	currentWrist->side=side;
	currentNoise=NULL;
}

void GesturePhase::Add(ArmPositionX x, bool fixed)
{ currentArm->x=x; currentArm->fixedX=fixed;}
void GesturePhase::Add(ArmPositionY y, bool fixed)
{ currentArm->y=y; currentArm->fixedY=fixed;}
void GesturePhase::Add(ArmPositionZ z, bool fixed)
{ currentArm->z=z; currentArm->fixedZ=fixed;}

void GesturePhase::Add(HandShapeType t)
{
	currentHand->type=t;
	if( t==shape_form ) {
		BasicForm* pForm=new BasicForm;
		currentHand->shape=pForm;
		currentHand->shape->side=side;
	}
	else if (t==shape_symbol) {
		BasicSymbol* pSymbol=new BasicSymbol;
		currentHand->shape=pSymbol;
		currentHand->shape->side=side;
	}
}

void GesturePhase::Add(FormType t, bool fixed)
{
	if (currentHand->type == shape_default)
		Add(shape_form);
	
	currentHand->fixedForm=fixed;
	
	if (currentHand->type != shape_form) {
#ifdef OUTPUT
		fprintf(data_log,"! GesturePhase::Add(FormType t):\n  Current Hand is not of type shape_form!\n");
#endif
		return;
	}
	// expressivity - power: switch between open and fist handshapes
	// todo: add handling of fixed handshapes
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	//assert(this->PWR!=-2);
	//if((!mFixedHandForm) && (PWR >0.5f) && (t==form_open))
	//	t=form_fist;
	//else 
	//	if((!mFixedHandForm) && (PWR <-0.5f) && (t==form_fist))
	//		t=form_open;
#endif

	BasicForm* pForm = (BasicForm*) currentHand->shape;
	pForm->type=t;
}

void GesturePhase::Add(FormThumbType t)
{
	if (currentHand->type != shape_form) {
#ifdef OUTPUT
		fprintf(data_log,"! GesturePhase::Add(FormThumbType t):\n  Current Hand is not of type shape_form!\n");
#endif
		return;
	}
	BasicForm* pForm = (BasicForm*) currentHand->shape;
	pForm->thumb=t;
}

void GesturePhase::Add(SymbolType t)
{
	if (currentHand->type == shape_default)
		Add(shape_symbol);

	if (currentHand->type != shape_symbol) {
#ifdef OUTPUT
		fprintf(data_log,"! GesturePhase::Add(SymbolType t):\n  Current Hand is not of type shape_symbol!\n");
#endif
		return;
	}
	BasicSymbol* pSymbol = (BasicSymbol*) currentHand->shape;
	pSymbol->type=t;

}

void GesturePhase::Add(SymbolOpenType t)
{
	if (currentHand->type != shape_symbol) {
		cout << "! GesturePhase::Add(SymbolType t):\n  Current Hand is not of type shape_symbol!\n";
		return;
	}
	BasicSymbol* pSymbol = (BasicSymbol*) currentHand->shape;
	pSymbol->opentype=t;
}

void GesturePhase::Add(SymbolClosedType t)
{
	if (currentHand->type != shape_symbol) {
#ifdef OUTPUT
		fprintf(data_log,"! GesturePhase::Add(SymbolType t):\n  Current Hand is not of type shape_symbol!\n");
#endif
		return;
	}
	BasicSymbol* pSymbol = (BasicSymbol*) currentHand->shape;
	pSymbol->closedtype=t;

}

void GesturePhase::Finalize()
{
	v.push_back(currentArm);
	v.push_back(currentHand);
	v.push_back(currentWrist);
	
	if (currentNoise!=NULL)
		v.push_back(currentNoise);
}

void GesturePhase::Add(FingerType f, BendType b)
{
	if (f==thumb) {
#ifdef OUTPUT
		fprintf(data_log,"! GesturePhase::Add(FingerType f, BendType b):\n   Cannot be used with thumb!\n");
#endif
		return;
	}
	FingerSpec* pFinger = new FingerSpec;
	pFinger->type=f;
	pFinger->bend=b;
	pFinger->side=side;
	currentHand->fingers.push_back(pFinger);
}

void GesturePhase::Add(BendType t) //refers to bend of a BasicForm!!
{
	if (currentHand->type != shape_form) {
#ifdef OUTPUT
		fprintf(data_log,"! Gesture::Add(BendType t):\n  Current Hand is not of type shape_form!\n");
#endif
		return;
	}
	BasicForm* pForm = (BasicForm*) currentHand->shape;
	pForm->bend=t;
}


void GesturePhase::ClearVector()
{
	v.clear();
}


void GesturePhase::Add(string s)
{
#ifdef OUTPUT
	fprintf(data_log,"GesturePhase:Add - adding %s\n",s);
#endif
	//:TODO: - add remainind ":FIXED" appended cases (so far only arm position + some hand forms done)
	if (s.compare("SHAPE_DEFAULT")==0)
		Add(shape_default);
	else if (s.compare("SHAPE_FORM")==0)
		Add(shape_form);
	else if (s.compare("SHAPE_SYMBOL")==0)
		Add(shape_symbol);
	else if (s.compare("FORM_DEFAULT")==0)
		Add(form_default, false);
	else if (s.compare("FORM_DEFAULT:FIXED")==0)
		Add(form_default, true);
	else if (s.compare("FORM_FIST")==0)
		Add(form_fist, false);
	else if (s.compare("FORM_FIST:FIXED")==0)
		Add(form_fist, true);
	else if (s.compare("FORM_OPEN")==0)
		Add(form_open, false);
	else if (s.compare("FORM_OPEN:FIXED")==0)
		Add(form_open, true);
	else if (s.compare("FORM_POINT1")==0)
		Add(form_point1, false);
	else if (s.compare("FORM_POINT2")==0)
		Add(form_point2, false);
	else if (s.compare("FORM_2APART")==0)
		Add(form_2apart, false);
	else if (s.compare("FORM_OPENAPART")==0)
		Add(form_openapart, false);
	else if (s.compare("THUMB_DEFAULT")==0)
		Add(thumb_default);
	else if (s.compare("THUMB_AWAY")==0)
		Add(thumb_away);
	else if (s.compare("THUMB_OVER")==0)
		Add(thumb_over);
	else if (s.compare("BEND_DEFAULT")==0)
		Add(bend_default);
	else if (s.compare("BEND_STRAIGHT")==0)
		Add(bend_straight);
	else if (s.compare("BEND_ANGLED")==0)
		Add(bend_angled);
	else if (s.compare("BEND_CURVED")==0)
		Add(bend_curved);
	else if (s.compare("BEND_CLOSED")==0)
		Add(bend_closed);
	else if (s.compare("SYMBOL_DEFAULT")==0)
		Add(symbol_default);
	else if (s.compare("SYMBOL_1_OPEN")==0)
		Add(symbol_1_open);
	else if (s.compare("SYMBOL_2_OPEN")==0)
		Add(symbol_2_open);
	else if (s.compare("SYMBOL_3_OPEN")==0)
		Add(symbol_3_open);
	else if (s.compare("SYMBOL_1_CLOSED")==0)
		Add(symbol_1_closed);
	else if (s.compare("SYMBOL_2_CLOSED")==0)
		Add(symbol_2_closed);
	else if (s.compare("SYMBOL_3_CLOSED")==0)
		Add(symbol_3_closed);
	else if (s.compare("OPEN_DEFAULT")==0)
		Add(open_default);
	else if (s.compare("OPEN_THUMBOUT")==0)
		Add(open_thumbout);
	else if (s.compare("OPEN_THUMBIN")==0)
		Add(open_thumbin);
	else if (s.compare("CLOSED_DEFAULT")==0)
		Add(closed_default);
	else if (s.compare("CLOSED_STRAIGHT")==0)
		Add(closed_straight);
	else if (s.compare("CLOSED_INSIDE")==0)
		Add(closed_inside);
	else if (s.compare("CLOSED_TIGHT")==0)
		Add(closed_tight);
	////////////////////ARM//////////////////////////
	// X coordinate //
	else if (s.compare("XEP")==0)
		Add(XEP,false);
	else if (s.compare("XEP:FIXED")==0)
		Add(XEP,true);
	else if (s.compare("XP")==0)
		Add(XP,false);
	else if (s.compare("XP:FIXED")==0)
		Add(XP,true);
	else if (s.compare("XC")==0)
		Add(XC,false);
	else if (s.compare("XC:FIXED")==0)
		Add(XC,true);
	else if (s.compare("XCC")==0)
		Add(XCC,false);
	else if (s.compare("XCC:FIXED")==0)
		Add(XCC,true);
	else if (s.compare("XOPPC")==0)
		Add(XOppC,false);
	else if (s.compare("XOPPC:FIXED")==0)
		Add(XOppC,true);
	// Y coordinate //
	else if (s.compare("YUPPEREP")==0)
		Add(YUpperEP,false);
	else if (s.compare("YUPPEREP:FIXED")==0)
		Add(YUpperEP,true);
	else if (s.compare("YUPPERP")==0)
		Add(YUpperP,false);
	else if (s.compare("YUPPERP:FIXED")==0)
		Add(YUpperP,true);
	else if (s.compare("YUPPERC")==0)
		Add(YUpperC,false);
	else if (s.compare("YUPPERC:FIXED")==0)
		Add(YUpperC,true);
	else if (s.compare("YCC")==0)
		Add(YCC,false);
	else if (s.compare("YCC:FIXED")==0)
		Add(YCC,true);
	else if (s.compare("YLOWERC")==0)
		Add(YLowerC,false);
	else if (s.compare("YLOWERC:FIXED")==0)
		Add(YLowerC,true);
	else if (s.compare("YLOWERP")==0)
		Add(YLowerP,false);
	else if (s.compare("YLOWERP:FIXED")==0)
		Add(YLowerP,true);
	else if (s.compare("YLOWEREP")==0)
		Add(YLowerEP,false);
	else if (s.compare("YLOWEREP:FIXED")==0)
		Add(YLowerEP,true);
	// Y coordinate //
	else if (s.compare("ZNEAR")==0)
		Add(ZNear,false);
	else if (s.compare("ZNEAR:FIXED")==0)
		Add(ZNear,true);
	else if (s.compare("ZMIDDLE")==0)
		Add(ZMiddle,false);
	else if (s.compare("ZMIDDLE:FIXED")==0)
		Add(ZMiddle,true);
	else if (s.compare("ZFAR")==0)
		Add(ZFar,false);
	else if (s.compare("ZFAR:FIXED")==0)
		Add(ZFar,true);
	////////////////////PALM//////////////////////////
	else if (s.compare("PALMUP")==0)
		AddWrist("", "PALMUP", false);
	else if (s.compare("PALMDOWN")==0)
		AddWrist("", "PALMDOWN", false);
	else if (s.compare("PALMAWAY")==0)
		AddWrist("", "PALMAWAY", false);
	else if (s.compare("PALMTOWARDS")==0)
		AddWrist("", "PALMTOWARDS", false);
	else if (s.compare("PALMINWARDS")==0)
		AddWrist("", "PALMINWARDS", false);
	else if (s.compare("PALMOUTWARDS")==0)
		AddWrist("", "PALMOUTWARDS", false);
	else if (s.compare("PALMDEFAULT")==0)
		AddWrist("", "PALMDEFAULT", false);
	else if (s.compare("PALMUP:TO")==0)
		AddWrist("", "PALMUP", true);
	else if (s.compare("PALMDOWN:TO")==0)
		AddWrist("", "PALMDOWN", true);
	else if (s.compare("PALMAWAY:TO")==0)
		AddWrist("", "PALMAWAY", true);
	else if (s.compare("PALMTOWARDS:TO")==0)
		AddWrist("", "PALMTOWARDS", true);
	else if (s.compare("PALMINWARDS:TO")==0)
		AddWrist("", "PALMINWARDS", true);
	else if (s.compare("PALMOUTWARDS:TO")==0)
		AddWrist("", "PALMOUTWARDS", true);
	else if (s.compare("PALMDEFAULT:TO")==0)
		AddWrist("", "PALMDEFAULT", true);
	////////////////////FINGER BASE//////////////////////////
	else if (s.compare("FBUP")==0)
		AddWrist("FBUP", "", false);
	else if (s.compare("FBDOWN")==0)
		AddWrist("FBDOWN", "", false);
	else if (s.compare("FBAWAY")==0)
		AddWrist("FBAWAY", "", false);
	else if (s.compare("FBTOWARDS")==0)
		AddWrist("FBTOWARDS", "", false);
	else if (s.compare("FBINWARDS")==0)
		AddWrist("FBINWARDS", "", false);
	else if (s.compare("FBOUTWARDS")==0)
		AddWrist("FBOUTWARDS", "", false);
	else if (s.compare("FBDEFAULT")==0)
		AddWrist("FBDEFAULT", "", false);
	else if (s.compare("FBUP:TO")==0)
		AddWrist("FBUP", "", true);
	else if (s.compare("FBDOWN:TO")==0)
		AddWrist("FBDOWN", "", true);
	else if (s.compare("FBAWAY:TO")==0)
		AddWrist("FBAWAY", "", true);
	else if (s.compare("FBTOWARDS:TO")==0)
		AddWrist("FBTOWARDS", "", true);
	else if (s.compare("FBINWARDS:TO")==0)
		AddWrist("FBINWARDS", "", true);
	else if (s.compare("FBOUTWARDS:TO")==0)
		AddWrist("FBOUTWARDS", "", true);
	else if (s.compare("FBDEFAULT:TO")==0)
		AddWrist("FBDEFAULT", "", true);
	else {
#ifdef OUTPUT
		fprintf(data_log,"! GesturePhase::Add(): Unknown string '%s'\n");
#endif
	}
}

/////////////////////////////////////////////////////////////////////
void GesturePhase::Add(ArmX x, bool fixed)
{ currentArm->AbstractX=x; currentArm->fixedX=fixed;}
void GesturePhase::Add(ArmY y, bool fixed)
{ currentArm->AbstractY=y; currentArm->fixedY=fixed;}
void GesturePhase::Add(ArmZ z, bool fixed)
{ currentArm->AbstractZ=z; currentArm->fixedZ=fixed;}


/////////////////////////////////////////////////////////////////////
void GesturePhase::Add(PalmType t)
{ 
	currentWrist->SetPalm(t);
	//currentWrist->palm=t;
}

/////////////////////////////////////////////////////////////////////
void GesturePhase::Add(FingerBaseType t)
{ 
	currentWrist->SetFingerBase(t);
	//currentWrist->fingerbase=t;
}
/////////////////////////////////////////////////////////////////////
void GesturePhase::AddWrist(string sFb, string sP, bool to)
{
	PalmType p; 
	FingerBaseType f;
	bool skippalm = false;
	bool skipfingerbase = false;

	////////////////////PALM//////////////////////////
	if (sP.compare("PALMUP")==0)
		p=PalmUp;
	else if (sP.compare("PALMDOWN")==0)
		p=PalmDown;
	else if (sP.compare("PALMAWAY")==0)
		p=PalmAway;
	else if (sP.compare("PALMTOWARDS")==0)
		p=PalmTowards;
	else if (sP.compare("PALMINWARDS")==0)
		p=PalmInwards;
	else if (sP.compare("PALMOUTWARDS")==0)
		p=PalmOutwards;
	else if (sP.compare("PALMDEFAULT")==0)
		p=PalmDefault;
	else
		skippalm = true;
	////////////////////FINGER BASE//////////////////////////
	if (sFb.compare("FBUP")==0)
		f=FBUp;
	else if (sFb.compare("FBDOWN")==0)
		f=FBDown;
	else if (sFb.compare("FBAWAY")==0)
		f=FBAway;
	else if (sFb.compare("FBTOWARDS")==0)
		f=FBTowards;
	else if (sFb.compare("FBINWARDS")==0)
		f=FBInwards;
	else if (sFb.compare("FBOUTWARDS")==0)
		f=FBOutwards;
	else if (sFb.compare("FBDEFAULT")==0)
		f=FBDefault;
	else
		skipfingerbase = true;
	//WRITE VALUES//
	if (to) {
		if (!skipfingerbase)
			currentWrist->SetFingerBase(f);
		if (!skippalm)
			currentWrist->SetPalm(p);
	} else {
		if (!skipfingerbase)
			currentWrist->SetFromFingerBase(f);
		if (!skippalm)
			currentWrist->SetFromPalm(p);
	}
}

void GesturePhase::AddWristMagnitude(float m)
{ 
	currentWrist->SetWristMagnitude(m);
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void GesturePhase::Concretize()
{
	GestureComponentVector::iterator iter;
	if(!v.empty())
		for(iter=v.begin(); iter!=v.end(); iter++)
		{
			(*iter)->Concretize();
		}
}

/////////////////////////////////////////////////////////////////////
void GesturePhase::SetSide(SideType s)
{
	GestureComponentVector::iterator iter;
	for(iter=v.begin(); iter!=v.end(); iter++) {
		(*iter)->SetSide(s);
	}
	side = s;

}

/////////////////////////////////////////////////////////////////////
void GesturePhase::SetType(string s)
{

#ifdef OUTPUT
	fprintf(data_log,". Set phase type %s\n",s.c_str());
#endif

	if (s.compare("STROKE")==0) {
		type=STROKE;
	} else if (s.compare("STROKE_START")==0) {
		type=STROKE_START;
	} else if (s.compare("STROKE_END")==0) {
		type=STROKE_END;
	} else if (s.compare("PREPARATION")==0) {
		type=PREPARATION;
	} else if (s.compare("PRE_STROKE_HOLD")==0) {
		type=PRE_STROKE_HOLD;
	} else if (s.compare("POST_STROKE_HOLD")==0) {
		type=POST_STROKE_HOLD;
	} else if (s.compare("RETRACTION")==0) {
		type=RETRACTION;
	} else {
#ifdef OUTPUT
		fprintf(data_log,". Set phase type default\n");
#endif
		type=PHASETYPE_DEFAULT;
	}
}

///////////////////////////////////////////////////////////////
void GesturePhase::Add(string finger, string bend)
{
	FingerType f=finger_default; 
	BendType b=bend_default;
	if (finger.compare("THUMB")==0) f=thumb;
	else if (finger.compare("INDEX")==0) f=index;
	else if (finger.compare("MIDDLE")==0) f=middle;
	else if (finger.compare("RING")==0) f=ring;
	else if (finger.compare("PINKY")==0) f=pinky;
	if (bend.compare("BEND_STRAIGHT")==0) b=bend_straight;
	else if (bend.compare("BEND_ANGLED")==0) b=bend_angled;
	else if (bend.compare("BEND_CURVED")==0) b=bend_curved;
	else if (bend.compare("BEND_CLOSED")==0) b=bend_closed;
	Add(f,b);
}


void GesturePhase::AddStoredPose(string file)
{
	bool found;
	found=false;
	GestureComponentVector::const_iterator iter;
	for (iter=v.begin();iter!=v.end();iter++)
	{
		if (typeid(**iter) == typeid(StoredComponent))
		{
			(*((StoredComponent *)*iter)).SetFilename(file);
			found=true;
			break;
		}
	}
	if(found==false)
	{
		StoredComponent* pComp = new StoredComponent;
		pComp->SetFilename(file);
		v.push_back(pComp);
	}
}

void GesturePhase::RemoveStoredPose()
{
	GestureComponentVector::iterator iter;
	for (iter=v.begin();iter!=v.end();iter++)
	{
		if (typeid(**iter) == typeid(StoredComponent))
		{
			v.erase(iter);
			break;
		}
	}
}

string GesturePhase::GetStoredPose()
{
	GestureComponentVector::const_iterator iter;
	
	for (iter=v.begin();iter!=v.end();iter++)
	{
		if (typeid(**iter) == typeid(StoredComponent))
		{
			return (*((StoredComponent *)*iter)).GetFilename();
		}
	}
	return "";
}

void GesturePhase::PrintTimes()
{

#ifdef OUTPUT
	fprintf(data_log,"  AbsTime: %2.2f; time : %2.2f\n",mAbsTime, time);
#endif	
}

void GesturePhase::PrintTimesOnScreen()
{
	printf("  AbsTime: %2.2f; time: %2.2f side: %d type: %d currentArm: %d\n",
		mAbsTime, time, side, type, currentArm);
}

GesturePhase::GesturePhase(const GesturePhase &rhs)
{
	certainty		= rhs.certainty;
	mAbsTime		= rhs.mAbsTime;
	mLoopType		= rhs.mLoopType;
	output			= rhs.output;
	time			= rhs.time;
	type			= rhs.type;

	GestureComponentVector::const_iterator iter;
	v.clear();
	currentArm=NULL;
	currentHand=NULL;
	currentWrist=NULL;
	transition = NULL;
	if(rhs.transition!=NULL)
		transition = new CurvedTransition(*(rhs.transition));

	// :KLUDGE: :TRICKY: real time type checking
	// can be avoided by using iterators instead of pointers?
	
	for (iter=rhs.v.begin();iter!=rhs.v.end();iter++)
	{
		if (typeid(**iter) == typeid(ArmPosition))
		{
			ArmPosition *pA = new ArmPosition(*((ArmPosition *)*iter));
			currentArm = pA;
			v.push_back(pA);
		} 
		else if (typeid(**iter) == typeid(WristOrientation))
		{
			WristOrientation *pW = new WristOrientation(*((WristOrientation *)*iter));
			currentWrist = pW;
			v.push_back(pW);
		}
		else if (typeid(**iter) == typeid(HandShape))
		{
			HandShape *pH = new HandShape(*((HandShape *)*iter));
			currentHand = pH;
			v.push_back(pH);
		}
		else if (typeid(**iter) == typeid(StoredComponent))
		{
			StoredComponent *pS = new StoredComponent(*((StoredComponent *)*iter));
			v.push_back(pS);		
		}
		else if (typeid(**iter) == typeid(NoiseComponent))
		{
			NoiseComponent *pN = new NoiseComponent(*((NoiseComponent *)*iter));
			v.push_back(pN);		
		}
		else
		{
			fprintf(data_log,"GesturePhase: found unexpected TYPEID: %s",typeid(**iter).name());
			//GestureComponent *pC= new GestureComponent(**iter);
			//v.push_back(pC);
			v.push_back(*iter);
		}
	}
	this->SetSide(rhs.side);
}

void GesturePhase::SetSPC(float SPC)
{
	GestureComponentVector::const_iterator iter;
	for (iter=this->v.begin();iter!=this->v.end();iter++)
		if (typeid(**iter) == typeid(ArmPosition))
		{
			((ArmPosition *)*iter)->expressivitySPC=SPC;
			((ArmPosition *)*iter)->ComputeScale();
		}
}

void GesturePhase::SetPWR(float PWR)
{
	this->PWR=PWR;
}

float GesturePhase::GetPWR()
{
	//assert(this->PWR!=-2);
	return this->PWR;
}

void GesturePhase::AddNoise()
{
	NoiseComponent* pN=new NoiseComponent();
	currentNoise=pN;
}

void GesturePhase::AddTweenComponent(GesturePhase* from, GesturePhase* to, float t)
{
	TweenComponent* pT = new TweenComponent;
	pT->SetPhases(from, to);
	pT->SetT(t);
	v.push_back(pT);
}
