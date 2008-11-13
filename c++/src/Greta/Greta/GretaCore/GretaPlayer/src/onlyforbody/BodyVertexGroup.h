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

/** holds the body vertex indices associated to a single body joint
*
* @see Joint
* @author Maurizio Mancini, manmau@yahoo.com
* @version 1.0
*/

#if !defined(AFX_BODYVERTEXGROUP_H__6260CFFC_9EAA_4FC3_BD25_A77DDA5C394D__INCLUDED_)
#define AFX_BODYVERTEXGROUP_H__6260CFFC_9EAA_4FC3_BD25_A77DDA5C394D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//limits the number of body groups
//it's enough for mpeg4 skeletons
//static const MAX_BODY_GROUPS=256;

//this is a list
//indices are read from file and new list elements are allocated
//then all the list is dumped into an array for faster access
typedef struct {
	int vind; //vertex index
	void* next; //pointer to the next vertex in list
}MyBodyVertexList;

class BodyVertexGroup  
{
public:
	char name[256];//name of the group
	int size;//number of indices stored in the group
	int* vertexarray;//final array of indices
	bool changed;//have the linked-to vertices moved? (ADDED)
	MyBodyVertexList* vertexlist;//pointer to the first list element
	BodyVertexGroup();//constructor
	void AddVertex(int v);//inserts a new index in the list
	void StoreArray();//dumps the list into the array
	virtual ~BodyVertexGroup();//destructor
};

#endif // !defined(AFX_BODYVERTEXGROUP_H__6260CFFC_9EAA_4FC3_BD25_A77DDA5C394D__INCLUDED_)
