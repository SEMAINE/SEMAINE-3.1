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

// CurvedTransition.h: interface for the CurvedTransition class.
//
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <sstream>  
#include "algebra3.h"
#include "BAPframe.h"

const float PI = 3.141592654f;
const float PIx2 = (PI*2);
const float PIon2 = (PI/2);
const float PIon2x3 = (3*PIon2);

enum Direction{UP,DOWN,INTERN,EXTERN};
enum Movement{WAVE,CIRCLE,SPIRAL_OUT,SPIRAL_IN,CUSTOM};
enum Orientation{SAGITTAL,TRANSVERSAL,FRONTAL}; //plane
enum Variation{SMALLER=-1,NONE,GREATER};

namespace GestureSpace {
	class CurvedTransition{
	public :
		
		CurvedTransition(void);
		CurvedTransition(const CurvedTransition &rhs);
		void instantiate(std::vector<std::string> params);
		bool isActive(void);

		/* function to set a custom movement */
		void set(float timesOnX,				float timesOnY,					float timesOnZ,
				 float amplitudeOnX,			float amplitudeOnY,				float amplitudeOnZ,
				 float shiftX,					float shiftY,					float shiftZ,
				 Variation spatialVariationX,	Variation spatialVariationY,	Variation spatialVariationZ,
				 Variation temporalVariationX,	Variation temporalVariationY,	Variation temporalVariationZ);

		/* function to set wave or circle or spiral movements */
		void set(Movement m, float times, float amplitude, Orientation o, Direction d, bool w);

		/* sets an empty movement (not active) */
		void clear(void);

		/* the param frame isn't the absolute frame number */
		alg3::vec3 getPosition(int frame, SideType side);

		/* return the length of the curve between 2 positions */
		double length(); // It's juste an approximation. for the right length we may use an elliptic integral

		/* get the coresponding string */
		string toString(bool smallForm=false);

		/* getters and setters */		
		void		setStartFrame(int n)						{startFrame = n; nbFrames = endFrame - startFrame;}
		void		setStartPosition(alg3::vec3 position)		{startPosition = position;}
		void		setEndFrame(int n)							{endFrame = n; nbFrames = endFrame - startFrame;}
		void		setEndPosition(alg3::vec3 position)			{endPosition = position;}
		void		setRealStartPosition(alg3::vec3 position, SideType side){startPosition += (position-(getPosition(0,side)));} //adjusts the startPosition in order to return this position with getPosition(0)
		Movement	getMovement(void)							{return movement;}
		void		setMovement(Movement m)						{movement = m;}
		Direction	getStartDirection(void)						{return startDirection;}
		void		setStartDirection(Direction d)				{startDirection = d;}
		bool		getInverseWay(void)							{return inverseWay;}
		void		setInverseWay(bool b)						{inverseWay = b;}
		Orientation	getOrientation(void)						{return orientation;}
		void		setOrientation(Orientation o)				{orientation = o;}
		Variation	getTemporalVariation(int axe)				{return temporalVariation[axe];}
		void		setTemporalVariation(Variation v, int axe)	{temporalVariation[axe] = v;}
		void		setTemporalVariation(double d, int axe)		{temporalVariation[axe] = doubleToVariation(d);}
		Variation	getSpatialVariation(int axe)				{return spatialVariation[axe];}
		void		setSpatialVariation(Variation v, int axe)	{spatialVariation[axe] = v;}
		void		setSpatialVariation(double d, int axe)		{spatialVariation[axe] = doubleToVariation(d);}
		float		getShift(int axe)							{return shift[axe]/PIx2;}
		void		setShift(float s, int axe)					{shift[axe] = s*PIx2;}
		float		getAmplitude(int axe)						{return amplitude[axe];}
		void		setAmplitude(float a, int axe)				{amplitude[axe] = a;}
		float		getFrequency(int axe)						{return frequency[axe];}
		void		setFrequency(float f, int axe)				{frequency[axe] = f;}
		float		getAmplitudeOnPlane(void)					{return getAmplitudeOnPlane(orientation);}
		void		setAmplitudeOnPlane(float a)				{setAmplitudeOnPlane(a, orientation);}
		float		getFrequencyOnPlane(void)					{return getFrequencyOnPlane(orientation);}
		void		setFrequencyOnPlane(float f)				{setFrequencyOnPlane(f, orientation);}
		float		getAmplitudeOnPlane(Orientation o);
		void		setAmplitudeOnPlane(float a, Orientation o);
		float		getFrequencyOnPlane(Orientation o);
		void		setFrequencyOnPlane(float f, Orientation o);

	private:

		//
		int			startFrame;
		alg3::vec3	startPosition;
		int			endFrame;
		alg3::vec3	endPosition;
		int			nbFrames;

		//Params of movement
		Movement	movement; //movement type
		Direction	startDirection; //not use for a CUSTOM movement, only UP and DOWN for WAVE (else, it's like UP)
		bool		inverseWay; //not use for CUSTOM and WAVE movements
		Orientation	orientation; //not use for a CUSTOM movement
		float		frequency[3];
		float		amplitude[3];
		float		shift[3];
		Variation	spatialVariation[3];
		Variation	temporalVariation[3]; //temporal variation. SMALLER is slacken, GREATER is quicken

		//private functions
		void makeCircle(int A, int B, float a);
		void makeSpiral(int A, int B, Variation space, Variation temps);
		Variation doubleToVariation(double d);
		//wave form is F(x)=S(x)*sin(T(x))
		float S(float x, int axe); //Computes the Spatial variation
		float T(float x, int axe); //Computes the Temporal variation
		float F(float x, int axe){return (S(x,axe)*sin(T(x,axe)));} //Computes the wave forme on one axe

		//////////////////////////////////////////////////////////////////////
		///// STRING TOOLS ///////////////////////////////////////////////////
		//--------------------------------------------------------------------
		bool isMovementAndWhich(string s, Movement * m);
		string stringOf(Movement m, bool smallForm=false);
		//--------------------------------------------------------------------
		bool isOrientationAndWhich(string s, Orientation * o);
		string stringOf(Orientation o, bool smallForm=false);
		//--------------------------------------------------------------------
		bool isDirectionAndWhich(string s, Direction * d);
		string stringOf(Direction d, bool smallForm=false);
		//--------------------------------------------------------------------
		bool isVariationAndWhich(string s, Variation * v);
		string stringOf(Variation v, bool smallForm=false);
		//--------------------------------------------------------------------
		//see on developpez.com and modify
		string stringOf(float f, bool smallForm=false);
		string stringOf(bool b);
		template<typename T>
		bool is_of_type( const std::string & Str, T * tmp ){
			// créer un flux à partir de la chaîne donnée
			std::istringstream iss( Str );
			// tenter la conversion et
			// vérifier qu'il ne reste plus rien dans la chaîne
			return ( iss >> (* tmp) ) && ( iss.eof() );
		}
	};
}