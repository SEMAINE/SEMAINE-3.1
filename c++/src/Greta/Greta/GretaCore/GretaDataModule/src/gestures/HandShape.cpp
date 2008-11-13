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

// HandShape.cpp: implementation of the HandShape class.
//
//////////////////////////////////////////////////////////////////////
#include "HandShape.h"
/**@#-*/
using namespace std ;
using namespace GestureSpace ;
/**@#+*/
//////////////////////////////////////////////////////////////////////	
// class FingerSpec implementation
//////////////////////////////////////////////////////////////////////

FingerSpec::FingerSpec() { type=finger_default; bend=bend_default; 	fileFolder=fileFolder+"handshapes\\";}
FingerSpec::~FingerSpec() {}
void FingerSpec::GetAngles(BAPFrame& f) 
{
	string fileName;
	if (output) cout << "FingerSpec::GetAngles(BAPFrame& f)\n";
		
	// STEP 1: read basic hand form
	if (type== index) fileName="index_";
	if (type== middle) fileName="middle_";
	if (type== ring) fileName="ring_";
	if (type== pinky) fileName="pinky_";
	
	if ((bend== bend_default)||(bend == bend_straight)) fileName=fileName+"straight.txt";
	if (bend== bend_angled) fileName=fileName+"angled.txt";
	if (bend== bend_curved) fileName=fileName+"curved.txt";
	if (bend== bend_closed) fileName=fileName+"closed.txt";
	fileName = fileFolder + filePrefix + fileName;
	if (output) cout << "   Reading file: "<<fileName<<endl;
	
	// read basic hand form into BAPFrame:
	//f.ReadPose(fileName, false);
	if (side == r) 
		f.ReadPose(fileName, false);
	else 
		f.ReadPose(fileName,false,true);

}
void FingerSpec::Print()
{
	cout << "FingerSpec::Print()\n"; 
	cout << "  +---FingerType   : "<<type<<endl; 
	cout << "  +---BendType     : "<<bend<<endl; 
}



//////////////////////////////////////////////////////////////////////	
// class BasicShape implementation
//////////////////////////////////////////////////////////////////////
BasicShape::BasicShape() 
{
	fileFolder=fileFolder+"handshapes\\";
}
BasicShape::~BasicShape() {}
void BasicShape::GetAngles(BAPFrame& f) 
{}
void BasicShape::Print()
{ cout << "BasicShape::Print()\n"; }

//////////////////////////////////////////////////////////////////////	
// class BasicForm implementation
//////////////////////////////////////////////////////////////////////
BasicForm::BasicForm() { thumb=thumb_default; bend=bend_default; status=abstract;}
BasicForm::~BasicForm() {}
void BasicForm::GetAngles(BAPFrame& f) 
{
	string fileName;
	if (output) cout << "s BasicForm::GetAngles(BAPFrame& f) SIDE: "<<side<<"\n";
		
	// STEP 1: read basic hand form
	if (type!= form_default) {
		// set filename according to basic hand form:
		if (type==form_fist) fileName="form_fist.txt";
		if (type==form_open) fileName="form_open.txt";
		if (type==form_point1) fileName="form_point1.txt";
		if (type==form_point2) fileName="form_point2.txt";
		if (type==form_2apart) fileName="form_2apart.txt";
		if (type==form_openapart) fileName="form_openapart.txt";
		fileName = fileFolder + filePrefix + fileName;
		if (output) cout << ". Reading file: "<<fileName<<endl;
	
		// read basic hand form into BAPFrame:
		if (side == r) 
			f.ReadPose(fileName, false);
		else 
			f.ReadPose(fileName,false,true);

		//f.ReadPose(fileName, false);
	}
	
	// STEP 2: bending - only defined for form_open for now !!!
	if ((bend != bend_default) && (type=form_open)) {
		
		// set filename according to bend type 
		if (bend == bend_angled) fileName="bend_angled.txt";
		if (bend == bend_curved) fileName="bend_curved.txt";
		if (bend == bend_closed) fileName="bend_closed.txt";
		fileName = fileFolder + filePrefix + fileName;
		if (output) cout << ". Reading file: "<<fileName<<endl;
		// read basic hand form into BAPFrame:
		if (side == r) 
			f.ReadPose(fileName, false);
		else 
			f.ReadPose(fileName,false,true);

		//f.ReadPose(fileName);
	}

	//STEP 3: thumb position
	if (thumb != thumb_default) {
		enum FormThumbType 		{thumb_default, thumb_away, thumb_over};
		if (thumb == thumb_away) fileName="thumb_away.txt";
		if (thumb == thumb_over) fileName="thumb_over.txt";
		fileName = fileFolder + filePrefix + fileName;
		if (output) cout << ". Reading file: "<<fileName<<endl;
		// read basic hand form into BAPFrame:
		if (side == r) 
			f.ReadPose(fileName, false);
		else 
			f.ReadPose(fileName,false,true);

		//f.ReadPose(fileName, false);
	}
}

void BasicForm::Print()
{ 
	cout << "s. BasicForm::Print()\n"; 
	cout << "   +---FormType     : "<<type<<endl; 
	cout << "   +---FormThumbType: "<<thumb<<endl; 
	cout << "   +---FormBendType : "<<bend<<endl; 
}

//////////////////////////////////////////////////////////////////////	
// class BasicSymbol implementation
//////////////////////////////////////////////////////////////////////
BasicSymbol::BasicSymbol() { opentype=open_default; closedtype=closed_default; status=abstract;}
BasicSymbol::~BasicSymbol() {}
void BasicSymbol::GetAngles(BAPFrame& f) 
{
//enum SymbolType 		{symbol_default, symbol_1_open, symbol_1_closed, symbol_2_open, symbol_2_closed, symbol_3_open, symbol_3_closed};
	string fileName;
	if (output) cout << "s BasicForm::GetAngles(BAPFrame& f)\n";
		
	// STEP 1: read basic hand form
	if (type!= symbol_default) {
		// set filename according to basic hand form:
		if (type==symbol_1_open) fileName="symbol_1_open.txt";
		if (type==symbol_1_closed) fileName="symbol_1_closed.txt";
		if (type==symbol_2_open) fileName="symbol_2_open.txt";
		if (type==symbol_2_closed) fileName="symbol_2_closed.txt";
		if (type==symbol_3_open) fileName="symbol_3_open.txt";
		if (type==symbol_3_closed) fileName="symbol_3_closed.txt";
		fileName = fileFolder + filePrefix + fileName;
		if (output) cout << ". Reading file: "<<fileName<<endl;
//		f.ReadPose(fileName, false);
		if (side == r) 
			f.ReadPose(fileName, false);
		else 
			f.ReadPose(fileName,false,true);

	}
	
	// optional thumb position for open symbols
	if (((type==symbol_1_open) ||
		 (type==symbol_2_open) ||
		 (type==symbol_3_open)) && (opentype != open_default) )
	{
		if (opentype == open_thumbout) fileName="open_thumbout.txt";
		if (opentype == open_thumbin) fileName="open_thumbin.txt";
		fileName = fileFolder + filePrefix + fileName;
		if (output) cout << "  Reading file: "<<fileName<<endl;
//		f.ReadPose(fileName, false);
		if (side == r) 
			f.ReadPose(fileName, false);
		else 
			f.ReadPose(fileName,false,true);

	}
	
	// optional thumb/index position for closed symbols
	if (((type==symbol_1_closed) ||
		 (type==symbol_2_closed) ||
		 (type==symbol_3_closed)) && (closedtype != closed_default) )
	{
		if (closedtype == closed_straight) fileName="closed_straight.txt";
		if (closedtype == closed_inside) fileName="closed_inside.txt";
		if (closedtype == closed_tight) fileName="closed_tight.txt";
		fileName = fileFolder + filePrefix + fileName;
		if (output) cout << "  Reading file: "<<fileName<<endl;
//		f.ReadPose(fileName, false);
		if (side == r) 
			f.ReadPose(fileName, false);
		else 
			f.ReadPose(fileName,false,true);
	
	}
	

}
void BasicSymbol::Print()
{ 
	cout << "s BasicSymbol::Print()\n"; 
	cout << "  +---SymbolType       : "<<type<<endl; 
	cout << "  +---SymbolOpenType   : "<<opentype<<endl; 
	cout << "  +---SymbolClosedType : "<<closedtype<<endl; 
}


//////////////////////////////////////////////////////////////////////	
// class HandShape implementation
//////////////////////////////////////////////////////////////////////

HandShape::HandShape() {
	type=shape_default;
	fingers.clear();
	shape=0;
	status=abstract;
}

HandShape::~HandShape(){
	fingers.clear();
}


void HandShape::Print()
{
	cout << "HandShape::Print()\n"; 
	cout << "  HandShapeType : "<<type<<endl; 
	
	// first print basic hand shape
	shape->Print();
	
	// then print fingers
	vector<FingerSpec *>::iterator iter;
	for(iter=fingers.begin(); iter!=fingers.end(); iter++) {
		(*iter)->Print();
	}

}

void HandShape::GetAngles(BAPFrame &f) 
{
	if (output) cout << "s HandShape::GetAngles(BAPFrame &f)\n";
	
	if (status=abstract) {
		if (output) cout << "~ HandShape::GetAngles: Position still ABSTRACT. Returning."<<endl;
		return;
	}
	// first load basic hand shape
	if (shape!=0)
		shape->GetAngles(f);
	
	// next load fingers 1 by 1
	vector<FingerSpec *>::iterator iter;
	for(iter=fingers.begin(); iter!=fingers.end(); iter++) {
		(*iter)->GetAngles(f);
	}
	
}

void HandShape::Concretize()
{
	status = concrete;
}

void HandShape::SetSide(SideType s)
{
	side=s;
	if (shape!=0)
		shape->SetSide(s);
	vector<FingerSpec *>::iterator iter;
	for(iter=fingers.begin(); iter!=fingers.end(); iter++) {
		(*iter)->SetSide(s);
	}

	
}
