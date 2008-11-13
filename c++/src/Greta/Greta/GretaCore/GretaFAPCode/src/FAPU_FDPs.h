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


#ifndef FAPU_FDPS_H
#define FAPU_FDPS_H

//Face Definition Parameters (feature points)
//Repsonible for defining the appearance of the	3D face model
//84 feature points, subdivided into groups and labelled with a number according to what region of
//the face they are in
//FDPs are used to transform the face into a particular face determined by shape
//Typically used only once at the beginning of the new session
//Example: FDP 2.1 is the jaw feature point
class FDPdata
{
public:
	FDPdata();
	~FDPdata();

	bool ReadFaceFDPs(const char *path, const float **vtxData);

	int FDPGroupsSize[11];	//the size of each group of feature points

	//3D array of the feature points themselves
	//indexed as featurePoints[i][j][k]
	// [i] is between i=2 and i<=11 
	// [j] is determined by FDPGroupsSize[i]
	// [k] is 0,1 or 2
	float ***featurePoints;
};


//Greta FAPU
#define GRETA_LH 239.963974   //Lip Height
#define GRETA_UL 479.682892   //Upper Lip Height
#define GRETA_LL 465.752380   //Lower Lip Height
#define GRETA_JH 166.034653	  //Jaw
#define GRETA_MNS 153.415222  //Distance from the nose
#define GRETA_LW 83.249252    //LIP Width
#define GRETA_ULP -340.974121 //Upper Lip protrusion
#define GRETA_LLP 753.426880  //Lower Lip protrusion

//The magnitude of the displacement is expressed by means of FAPU, Facial Animation Paramter Unit
//Represents a fraction of a specific distance on the human face (apart from FAPU for rotations)
class FAPUdata
{
public:
	FAPUdata();
	~FAPUdata();

	//calculate FAPUs using feature points
	void CalculateFAPUs(const FDPdata *fdps);
	void print(void);

	float MWFAPU;	//mouth width
	float MNSFAPU;	//mouth nose
	float ENSFAPU;	//eyes-nose separation
	float ESFAPU;	//eyes seperation
	float IRISDFAPU; //iris distance

	float IFAPU_MW;
	float IFAPU_MH;
	float IFAPU_JH;
	float IFAPU_ULP;
	float IFAPU_LLP;
	float IFAPU_FI;	 // Forehead Front Inclination
	float IFAPU_RCB; // Right Cheek Bump
	float IFAPU_LCB; // Left Cheek Bump

	float AFAPU_UL;
	float AFAPU_LL;
};

#endif