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

// RandomGen.cpp: implementation of the RandomGen class.
//
//////////////////////////////////////////////////////////////////////

#include "RandomGen.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "randomc.h"


//extern "C" double genrand_real1();
//extern "C" void init_genrand(unsigned long);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RandomGen::RandomGen()
{
	int32 seed = (int32)time(0);
	rg=new CRandomMersenne(seed); 
}

RandomGen::~RandomGen()
{
	delete rg;
}

//! Returns a random number between 0 and 1
float RandomGen::GetRand01()
{
	return (((CRandomMersenne*)rg)->Random());
}
