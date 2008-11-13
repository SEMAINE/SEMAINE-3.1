// PlayerPerlinNoise.h: interface for the PlayerPerlinNoise class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PlayerPerlinNoise_H__F5E11E1A_6C4F_4875_AE74_3F7CB15C61BB__INCLUDED_)
#define AFX_PlayerPerlinNoise_H__F5E11E1A_6C4F_4875_AE74_3F7CB15C61BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define pn_B 0x100
#define pn_BM 0xff

#define pn_N 0x1000
#define pn_NP 12   /* 2^N */
#define pn_NM 0xfff

#include "RandomGen.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/** Perlin noise function in 1,2,3 dimensions.
* Ported to C++ from Perlin's C source code on his website.
*/
class PlayerPerlinNoise  
{
public:
	/** 3D Perlin noise */
	float noise3(float vec[3]);

	/** 2D Perlin noise */
	float noise2(float vec[2]);

	/** 1D Perlin noise - 0.1 is a good timestep between frames */
	double noise1(double arg);

	PlayerPerlinNoise();
	virtual ~PlayerPerlinNoise();

private:
	int p[pn_B + pn_B + 2];
	float g3[pn_B + pn_B + 2][3];
	float g2[pn_B + pn_B + 2][2];
	float g1[pn_B + pn_B + 2];
	int start;
	void init();
	void normalize3(float v[3]);
	void normalize2(float v[2]);
	RandomGen *randomgen;
};

#endif // !defined(AFX_PlayerPerlinNoise_H__F5E11E1A_6C4F_4875_AE74_3F7CB15C61BB__INCLUDED_)
