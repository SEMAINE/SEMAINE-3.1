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

// PlayerFLTKMenu.h: interface for the PlayerFLTKMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PlayerFLTKMenu_H__CEB73F20_17DC_11D9_9C92_D317E477FD7F__INCLUDED_)
#define AFX_PlayerFLTKMenu_H__CEB73F20_17DC_11D9_9C92_D317E477FD7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Menu.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Menu_Item.H>

/**
*
* class :PlayerFLTKMenu
*
*/

class PlayerFLTKMenu : public Fl_Menu_Bar  
{
public:

	/**
	*
	* contructor 
	*
	* @param  x
	* @param  y
	* @param  w
	* @param  h
	*/

	PlayerFLTKMenu(int x,int y,int w,int h);

	/**
	*
	* destructor 
	*/

	virtual ~PlayerFLTKMenu();
};

#endif // !defined(AFX_PlayerFLTKMenu_H__CEB73F20_17DC_11D9_9C92_D317E477FD7F__INCLUDED_)
