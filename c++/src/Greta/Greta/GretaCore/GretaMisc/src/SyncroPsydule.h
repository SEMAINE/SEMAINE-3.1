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

// SyncroPsydule.h: interface for the SyncroPsydule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCROPSYDULE_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_)
#define AFXSYNCROPSYDULE_H__8D6DB6DB_FC9A_432B_8450_4B941018BD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <fstream>
#include "Psydule.h"


/**
*
* class :SyncroPsydule
*
*/

class SyncroPsydule : public Psydule
{
public:

	/**
	*
	* contructor 
	*
	* @param  name
	* @param  JString host
	* @param  int port
	*/

	SyncroPsydule(JString name, JString host, int port);

	/**
	*
	* destructor 
	*/

	virtual ~SyncroPsydule();

	/**
	* this method 
	* 
	*
	* @return 
	*/

	int SyncroPsydule::InitSP();

	int time;

	void *mutex;
};

#endif
