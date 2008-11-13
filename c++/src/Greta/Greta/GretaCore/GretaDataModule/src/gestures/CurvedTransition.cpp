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

// CurvedTransition.cpp: implementation of the CurvedTransition class.
//
//////////////////////////////////////////////////////////////////////

#include "CurvedTransition.h"


using namespace GestureSpace;
#define OUTPUT
//////////////////////////////////////////////////////////////////////
CurvedTransition::CurvedTransition(){
	clear();
	startFrame = 0;
	startPosition = alg3::vec3(0,0,0);
	endFrame = 1;
	endPosition = alg3::vec3(0,0,0);
	nbFrames = 1;
}
CurvedTransition::CurvedTransition(const CurvedTransition &rhs){
	startFrame				= rhs.startFrame;
	startPosition			= rhs.startPosition;
	endFrame				= rhs.endFrame;
	endPosition				= rhs.endPosition;
	nbFrames				= rhs.nbFrames;
	movement				= rhs.movement;
	startDirection			= rhs.startDirection;
	inverseWay				= rhs.inverseWay;
	orientation				= rhs.orientation;
	frequency[0]			= rhs.frequency[0];
	frequency[1]			= rhs.frequency[1];
	frequency[2]			= rhs.frequency[2];
	amplitude[0]			= rhs.amplitude[0];
	amplitude[1]			= rhs.amplitude[1];
	amplitude[2]			= rhs.amplitude[2];
	shift[0]				= rhs.shift[0];
	shift[1]				= rhs.shift[1];
	shift[2]				= rhs.shift[2];
	spatialVariation[0]		= rhs.spatialVariation[0];
	spatialVariation[1]		= rhs.spatialVariation[1];
	spatialVariation[2]		= rhs.spatialVariation[2];
	temporalVariation[0]	= rhs.temporalVariation[0]; 
	temporalVariation[1]	= rhs.temporalVariation[1]; 
	temporalVariation[2]	= rhs.temporalVariation[2]; 
}
//////////////////////////////////////////////////////////////////////
void CurvedTransition::instantiate(std::vector<string> params){
	bool alright = true;
	if(params.size()==16){ //custom
		// x params :
		float xt = 0;
		float xa = 0;
		float xs = 0;
		Variation xsv;
		Variation xtv;
		// y params :
		float yt = 0;
		float ya = 0;
		float ys = 0;
		Variation ysv;
		Variation ytv;
		// z params :
		float zt = 0;
		float za = 0;
		float zs = 0;
		Variation zsv;
		Variation ztv;

		alright =  is_of_type<float>(params[1],&xt)
				&& is_of_type<float>(params[2],&xa)
				&& is_of_type<float>(params[3],&xs)
				&& isVariationAndWhich(params[4],&xsv)
				&& isVariationAndWhich(params[5],&xtv)
				&& is_of_type<float>(params[6],&yt)
				&& is_of_type<float>(params[7],&ya)
				&& is_of_type<float>(params[8],&ys)
				&& isVariationAndWhich(params[9],&ysv)
				&& isVariationAndWhich(params[10],&ytv)
				&& is_of_type<float>(params[11],&zt)
				&& is_of_type<float>(params[12],&za)
				&& is_of_type<float>(params[13],&zs)
				&& isVariationAndWhich(params[14],&zsv)
				&& isVariationAndWhich(params[15],&ztv);
		if(alright) set(xt,yt,zt, xa,ya,za, xs,ys,zs, xsv,ysv,zsv, xtv,ytv,ztv);
	}
	if(params.size()==7){ //wave, circle and spirals
		Movement  m;
		float t = 0;
		float a = 0;
		Orientation o;
		Direction d;
		bool w = false;
		alright =  isMovementAndWhich(params[1],&m)
				&& is_of_type<float>(params[2],&t)
				&& is_of_type<float>(params[3],&a)
				&& isOrientationAndWhich(params[4],&o)
				&& isDirectionAndWhich(params[5],&d)
				&& is_of_type<bool>(params[6],&w);
		if(alright) set(m,t,a,o,d,w);
	}
	if(!alright) clear(); //this transition will not be active
}
//////////////////////////////////////////////////////////////////////
string CurvedTransition::toString(bool small){
	if(movement == CUSTOM)
		return "CURVE " +stringOf(frequency[0])+" "+stringOf(amplitude[0])+" "+stringOf(getShift(0))+" "+stringOf(spatialVariation[0],small)+" "+stringOf(temporalVariation[0],small)+" "
						+stringOf(frequency[1])+" "+stringOf(amplitude[1])+" "+stringOf(getShift(1))+" "+stringOf(spatialVariation[1],small)+" "+stringOf(temporalVariation[1],small)+" "
						+stringOf(frequency[2])+" "+stringOf(amplitude[2])+" "+stringOf(getShift(2))+" "+stringOf(spatialVariation[2],small)+" "+stringOf(temporalVariation[2],small)+"\n";
	else
		return "CURVE " +stringOf(movement,small)+" "+stringOf(getFrequencyOnPlane())+" "+stringOf(getAmplitudeOnPlane())+" "+stringOf(orientation,small)+" "+stringOf(startDirection)+" "+stringOf(inverseWay)+"\n";
}
//////////////////////////////////////////////////////////////////////
bool CurvedTransition::isActive(){
	if( amplitude[0] != 0 ||
		amplitude[1] != 0 ||
		amplitude[2] != 0 ) return true;
	return false;
}
//////////////////////////////////////////////////////////////////////
void CurvedTransition::clear(){
	set(0,0,0, 0,0,0, 0,0,0, NONE,NONE,NONE, NONE,NONE,NONE);
	startDirection	= UP;
	inverseWay		= false;
	orientation		= SAGITTAL;
}
//////////////////////////////////////////////////////////////////////
void CurvedTransition::set(float timesOnX, float timesOnY, float timesOnZ,
				 float amplitudeOnX, float amplitudeOnY, float amplitudeOnZ,
				 float shiftX, float shiftY, float shiftZ,
				 Variation spatialVariationX, Variation spatialVariationY, Variation spatialVariationZ,
				 Variation temporalVariationX, Variation temporalVariationY, Variation temporalVariationZ){
	movement				= CUSTOM;
	temporalVariation[0]	= temporalVariationX;
	temporalVariation[1]	= temporalVariationY;
	temporalVariation[2]	= temporalVariationZ;

	frequency[0]			= timesOnX;
	frequency[1]			= timesOnY;
	frequency[2]			= timesOnZ;

	amplitude[0]			= amplitudeOnX;
	amplitude[1]			= amplitudeOnY;
	amplitude[2]			= amplitudeOnZ;

	spatialVariation[0]		= spatialVariationX;
	spatialVariation[1]		= spatialVariationY;
	spatialVariation[2]		= spatialVariationZ;

	shift[0]				= shiftX*PIx2;
	shift[1]				= shiftY*PIx2;
	shift[2]				= shiftZ*PIx2;
}
//////////////////////////////////////////////////////////////////////
void CurvedTransition::set(Movement m, float times, float amplitude, Orientation o, Direction d, bool w){
	set(0,0,0, 0,0,0, 0,0,0, NONE,NONE,NONE, NONE,NONE,NONE); // initialisation of parameters
	movement		= m;
	startDirection	= d;
	inverseWay		= w; 
	orientation		= o;
	
	if(m == WAVE){
		if(o == SAGITTAL){
			this->amplitude[1] = amplitude;
			this->frequency[1] = times;
			if(d == DOWN)   shift[1] = PI;
		}
		if(o == TRANSVERSAL){
			this->amplitude[0] = amplitude;
			this->frequency[0] = times;
			if(d == DOWN)   shift[0] = PI;
		}
		if(o == FRONTAL){
			this->amplitude[2] = amplitude;
			this->frequency[2] = times;
			if(d == DOWN)   shift[2] = PI;
		}
	}
	else if(m != CUSTOM){ //circle and spirales
		int A,B;
		if(o == SAGITTAL){A=0;B=1;}
		if(o == TRANSVERSAL){A=0;B=2;}
		if(o == FRONTAL){A=2;B=1;}
		this->frequency[A] = times;
		this->frequency[B] = times;
		makeCircle(A,B,amplitude);
		if(m == SPIRAL_OUT) makeSpiral(A,B,GREATER,SMALLER);
		if(m == SPIRAL_IN) makeSpiral(A,B,SMALLER,GREATER);
	}
}
void CurvedTransition::makeCircle(int A, int B, float a){
	amplitude[A] = a;
	amplitude[B] = a;
	if(startDirection == UP) shift[A] = (3-2*inverseWay)*PIon2;
	if(startDirection == EXTERN) shift[B] = (1+2*inverseWay)*PIon2;
	if(startDirection == DOWN) { shift[A] = (1+2*inverseWay)*PIon2; shift[B] = PI; }
	if(startDirection == INTERN) { shift[A] = PI; shift[B] = (3-2*inverseWay)*PIon2; }
}
void CurvedTransition::makeSpiral(int A, int B, Variation space, Variation temps){
	spatialVariation[A] = space;
	spatialVariation[B] = space;
	temporalVariation[A] = temps;
	temporalVariation[B] = temps;
}
//////////////////////////////////////////////////////////////////////
float CurvedTransition::getAmplitudeOnPlane(Orientation o){
	int A,B;
	if(o == SAGITTAL){A=0;B=1;}
	if(o == TRANSVERSAL){A=0;B=2;}
	if(o == FRONTAL){A=2;B=1;}
	return amplitude[A]>amplitude[B]?amplitude[A]:amplitude[B]; //return the max amplitude on this plane
}
void CurvedTransition::setAmplitudeOnPlane(float a, Orientation o){
	int A,B;
	if(o == SAGITTAL){A=0;B=1;}
	if(o == TRANSVERSAL){A=0;B=2;}
	if(o == FRONTAL){A=2;B=1;}
	amplitude[A] = a;
	amplitude[B] = a;
}
float CurvedTransition::getFrequencyOnPlane(Orientation o){
	int A,B;
	if(o == SAGITTAL){A=0;B=1;}
	if(o == TRANSVERSAL){A=0;B=2;}
	if(o == FRONTAL){A=2;B=1;}
	return frequency[A]>frequency[B]?frequency[A]:frequency[B]; //return the max frequency on this plane
}
void CurvedTransition::setFrequencyOnPlane(float f, Orientation o){
	int A,B;
	if(o == SAGITTAL){A=0;B=1;}
	if(o == TRANSVERSAL){A=0;B=2;}
	if(o == FRONTAL){A=2;B=1;}
	frequency[A] = f;
	frequency[B] = f;
}
Variation CurvedTransition::doubleToVariation(double d){
	if(d<0) return SMALLER;
	if(d>0) return GREATER;
	return NONE;
}
//////////////////////////////////////////////////////////////////////
float CurvedTransition::S(float x, int axe){
	float s;
	if(spatialVariation[axe] == NONE) s = 1;
	if(spatialVariation[axe] == GREATER) s = x;
	if(spatialVariation[axe] == SMALLER) s = 1.0-x;
	return amplitude[axe] * s;
}
float CurvedTransition::T(float x, int axe){
	float t;
	if(temporalVariation[axe] == NONE) t = x;
	if(temporalVariation[axe] == GREATER) t = 1.0-sqrt(1.0-x); // quicken
	if(temporalVariation[axe] == SMALLER) t = sqrt(x); // slacken
	return frequency[axe]*PIx2*t+shift[axe];
}
//////////////////////////////////////////////////////////////////////
alg3::vec3 CurvedTransition::getPosition(int frame, SideType side){
	float relativeTime = nbFrames!=0 ? ((float)frame)/nbFrames : 0;
	if (relativeTime<0) relativeTime=0;
	alg3::vec3 position(0,0,0);
	position = (endPosition - startPosition) * relativeTime + startPosition;
	if(side==l)
		position += alg3::vec3(F(relativeTime,0),F(relativeTime,1),F(relativeTime,2));
	else
		position += alg3::vec3(-F(relativeTime,0),F(relativeTime,1),F(relativeTime,2));
	return position;
}
//////////////////////////////////////////////////////////////////////
double CurvedTransition::length(){
	// It's an approximation 
	alg3::vec3 curve ((PI*sqrt(2.f)*S(0.5,0)*frequency[0] + abs(startPosition[0]-endPosition[0])),
					  (PI*sqrt(2.f)*S(0.5,1)*frequency[1] + abs(startPosition[1]-endPosition[1])),
					  (PI*sqrt(2.f)*S(0.5,2)*frequency[2] + abs(startPosition[2]-endPosition[2])));
	return curve.length();
}
//////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
bool CurvedTransition::isMovementAndWhich(string s, Movement * m){
	if(s.compare("WAVE")==0 || s.compare("W")==0)		{(*m)=WAVE; return true;}
	if(s.compare("CIRCLE")==0 || s.compare("C")==0)		{(*m)=CIRCLE; return true;}
	if(s.compare("SPIRAL_OUT")==0 || s.compare("O")==0)	{(*m)=SPIRAL_OUT; return true;}
	if(s.compare("SPIRAL_IN")==0 || s.compare("I")==0)	{(*m)=SPIRAL_IN; return true;}
	return false;
}
string CurvedTransition::stringOf(Movement m, bool smallForm){
	if(m==WAVE){
		if(smallForm) return "W";
		else return "WAVE";
	}
	if(m==CIRCLE){
		if(smallForm) return "C";
		else return "CIRCLE";
	}
	if(m==SPIRAL_OUT){
		if(smallForm) return "O";
		else return "SPIRAL_OUT";
	}
	if(m==SPIRAL_IN){
		if(smallForm) return "I";
		else return "SPIRAL_IN";
	}
	if(m==CUSTOM){
		if(smallForm) return "U";
		else return "CUSTOM";
	}
	return "";
}
//--------------------------------------------------------------------
bool CurvedTransition::isOrientationAndWhich(string s, Orientation * o){
	if(s.compare("SAGITTAL")==0 || s.compare("YX")==0 || s.compare("XY")==0)	{(*o)=SAGITTAL; return true;}
	if(s.compare("TRANSVERSAL")==0 || s.compare("ZX")==0 || s.compare("XZ")==0)	{(*o)=TRANSVERSAL; return true;}
	if(s.compare("FRONTAL")==0 || s.compare("ZY")==0 || s.compare("YZ")==0)		{(*o)=FRONTAL; return true;}
	return false;
}
string CurvedTransition::stringOf(Orientation o, bool smallForm){
	if(o==SAGITTAL){
		if(smallForm) return "XY";
		else return "SAGITTAL";
	}
	if(o==TRANSVERSAL){
		if(smallForm) return "XZ";
		else return "TRANSVERSAL";
	}
	if(o==FRONTAL){
		if(smallForm) return "YZ";
		else return "FRONTAL";
	}
	return "";
}
//--------------------------------------------------------------------
bool CurvedTransition::isDirectionAndWhich(string s, Direction * d){
	if(s.compare("UP")==0)		{(*d)=UP; return true;}
	if(s.compare("DOWN")==0)	{(*d)=DOWN; return true;}
	if(s.compare("INTERN")==0)	{(*d)=INTERN; return true;}
	if(s.compare("EXTERN")==0)	{(*d)=EXTERN; return true;}
	return false;
}
string CurvedTransition::stringOf(Direction d, bool smallForm){
	if(d==UP)		return "UP";
	if(d==DOWN)		return "DOWN";
	if(d==INTERN)	return "INTERN";
	if(d==EXTERN)	return "EXTERN";
	return "";
}
//--------------------------------------------------------------------
bool CurvedTransition::isVariationAndWhich(string s, Variation * v){
	if(s.compare("SMALLER")==0 || s.compare("S")==0)	{(*v)=SMALLER; return true;}
	if(s.compare("NONE")==0 || s.compare("N")==0)		{(*v)=NONE; return true;}
	if(s.compare("GREATER")==0 || s.compare("G")==0)	{(*v)=GREATER; return true;}
	return false;
}
string CurvedTransition::stringOf(Variation v, bool smallForm){
	if(v==SMALLER){
		if(smallForm) return "S";
		else return "SMALLER";
	}
	if(v==NONE){
		if(smallForm) return "N";
		else return "NONE";
	}
	if(v==GREATER){
		if(smallForm) return "G";
		else return "GREATER";
	}
	return "";
}
//--------------------------------------------------------------------
//see on developpez.com and modify
string CurvedTransition::stringOf(float f, bool smallForm){
	std::ostringstream oss;
	oss << f;
	return oss.str();
}
string CurvedTransition::stringOf(bool b){
	std::ostringstream oss;
	oss << b;
	return oss.str();
}