// PerlinNoise.h: interface for the PerlinNoise class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERLINNOISE_H__F5E11E1A_6C4F_4875_AE74_3F7CB15C61BB__INCLUDED_)
#define AFX_PERLINNOISE_H__F5E11E1A_6C4F_4875_AE74_3F7CB15C61BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GestureEngineGlobals.h"

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

/** Perlin noise function in 1,2,3 dimensions.
* Ported to C++ from Perlin's C source code on his website.
*/
class PerlinNoise  
{
public:
	/** 3D Perlin noise */
	float noise3(float vec[3]);

	/** 2D Perlin noise */
	float noise2(float vec[2]);

	/** 1D Perlin noise - 0.1 is a good timestep between frames */
	double noise1(double arg);

	/**
	* constructor 
	*
	*/

	PerlinNoise();

	/**
	* destructor
	*
	*/
	virtual ~PerlinNoise();

private:
	int p[B + B + 2];
	float g3[B + B + 2][3];
	float g2[B + B + 2][2];
	float g1[B + B + 2];
	int start;
	void init();
	void normalize3(float v[3]);
	void normalize2(float v[2]);
};

#endif // !defined(AFX_PERLINNOISE_H__F5E11E1A_6C4F_4875_AE74_3F7CB15C61BB__INCLUDED_)
