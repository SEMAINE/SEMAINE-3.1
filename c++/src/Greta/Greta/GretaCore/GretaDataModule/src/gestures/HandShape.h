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

/// @file HandShape.h
/// interface for the HandShape class.
///
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HANDSHAPE_H__A31EDF79_F9ED_435D_A2A3_D022C1810897__INCLUDED_)
#define AFX_HANDSHAPE_H__A31EDF79_F9ED_435D_A2A3_D022C1810897__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include <iostream>
//#include <vector>
//#include "GestureEngineGlobals.h"
#include "GestureComponent.h"

/**@#-*/
namespace GestureSpace {
/**@#+*/
	enum HandShapeType 		{shape_default=0, shape_form, shape_symbol};
	enum FormType  			{form_default, form_fist, form_open, form_point1, form_point2, form_2apart, form_openapart};
	enum FormThumbType 		{thumb_default, thumb_away, thumb_over};
	enum BendType 			{bend_default, bend_straight, bend_angled, bend_curved, bend_closed};
	enum SymbolType 		{symbol_default, symbol_1_open, symbol_2_open, symbol_3_open, symbol_1_closed, symbol_2_closed, symbol_3_closed};
	enum SymbolOpenType 	{open_default, open_thumbout, open_thumbin};
	enum SymbolClosedType   {closed_default, closed_straight, closed_inside, closed_tight};
	enum FingerType			{finger_default, thumb, index, middle, ring, pinky};

	/** Defines state of a single finger; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu  
	* @version 1.0
	*/
	class FingerSpec : public GestureComponent{
	public:
		void Print();
		FingerSpec();
		virtual ~FingerSpec();
		FingerType				type;
		BendType				bend;
		void GetAngles(BAPFrame& f);
	};

	/** Base class for basic hand shape; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class BasicShape : public GestureComponent
	{
	public:
		BasicShape();
		virtual ~BasicShape();
		virtual void GetAngles(BAPFrame& f);
		virtual void Print();
	};

	/** Defines a basic hand shape as one of the 6 basic forms
	*
	* Derived from BasicShape, which is derived from GestureComponent<br>
	* Other possibility for basic shape of hand: class BasicSymbol
	* @see GestureComponent
	* @see BasicShape
	* @see BasicSymbol
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class BasicForm : public BasicShape 
	{
	public:
		BasicForm();
		virtual ~BasicForm();
		FormType	 			type;
		FormThumbType			thumb;
		BendType				bend;
		void GetAngles(BAPFrame &f);
		void Print();
	};

	/** Defines a basic hand shape as one of the 6 basic symbols
	*
	* Derived from BasicShape, which is derived from GestureComponent<br>
	* Other possibility for basic shape of hand: class BasicForm
	* @see GestureComponent
	* @see BasicShape
	* @see BasicForm
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class BasicSymbol : public BasicShape 
	{
	public:
		BasicSymbol();
		virtual ~BasicSymbol();
		SymbolType				type;
		SymbolOpenType			opentype;
		SymbolClosedType		closedtype;
		void GetAngles(BAPFrame& f);
		void Print();
	};

	/** Defines complete shape of hand and fingers; derived from GestureComponent
	*
	* @see GestureComponent
	* @author Bjoern Hartmann, bjoern@graphics.cis.upenn.edu 
	* @version 1.0
	*/
	class HandShape : public GestureComponent 
	{
	public:
		void SetSide(SideType s);
		void Concretize();
		void Print();
		HandShape();
		virtual ~HandShape();
		HandShapeType			type;
		BasicShape*				shape;
		std::vector<FingerSpec *>fingers;
		void GetAngles(BAPFrame &f);
		bool fixedForm;
		
	};

/**@#-*/
} // end namespace GestureSpace
/**@#+*/

#endif // !defined(AFX_HANDSHAPE_H__A31EDF79_F9ED_435D_A2A3_D022C1810897__INCLUDED_)
