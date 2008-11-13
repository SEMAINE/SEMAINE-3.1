//Copyright 1999-2005 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// FacePartsToolWindow.h: interface for the FacePartsToolWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FacePartsToolWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_FacePartsToolWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Output.H>
#include <fl/fl_counter.h>
#include <fl/Fl_Box.h>
#include <fl/fl_value_output.h>
#include "PlayerFLTKGLWindow.h"

class FacePartsToolWindow : public Fl_Window  
{
public:
	FacePartsToolWindow();
	virtual ~FacePartsToolWindow();
	PlayerFLTKGLWindow *glutw;
	int handle(int event);
	void draw();

private:
	Fl_Button *leyexminus;
	Fl_Button *leyeyminus;
	Fl_Button *leyezminus;
	Fl_Button *leyexplus;
	Fl_Button *leyeyplus;
	Fl_Button *leyezplus;
	Fl_Button *reyexminus;
	Fl_Button *reyeyminus;
	Fl_Button *reyezminus;
	Fl_Button *reyexplus;
	Fl_Button *reyeyplus;
	Fl_Button *reyezplus;

	Fl_Button *eyescaleminus;
	Fl_Button *eyescaleplus;

	Fl_Button *teethsxminus;
	Fl_Button *teethsxplus;
	Fl_Button *teethsyminus;
	Fl_Button *teethsyplus;
	Fl_Button *teethszminus;
	Fl_Button *teethszplus;


	Fl_Output *leyex;
	Fl_Output *leyey;
	Fl_Output *leyez;
	Fl_Output *reyex;
	Fl_Output *reyey;
	Fl_Output *reyez;
	Fl_Output *eyescale;
	Fl_Output *teethsx;
	Fl_Output *teethsy;
	Fl_Output *teethsz;

	Fl_Button *switchhair;
	Fl_Button *switchwireframe;

	Fl_Button *headxminus;
	Fl_Button *headxplus;
	Fl_Button *headyminus;
	Fl_Button *headyplus;
	Fl_Button *headzminus;
	Fl_Button *headzplus;
	Fl_Button *headscaleminus;
	Fl_Button *headscaleplus;

	Fl_Button *save;

	Fl_Box *box_eyes;
	Fl_Box *box_teeth;
	Fl_Box *box_head;
	Fl_Box *box_head_scale;
	Fl_Box *box_hair;

	Fl_Button *hairpxminus;
	Fl_Button *hairpxplus;
	Fl_Button *hairpyminus;
	Fl_Button *hairpyplus;
	Fl_Button *hairpzminus;
	Fl_Button *hairpzplus;
	Fl_Button *hairsxminus;
	Fl_Button *hairsxplus;
	Fl_Button *hairsyminus;
	Fl_Button *hairsyplus;
	Fl_Button *hairszminus;
	Fl_Button *hairszplus;


public:
	void WriteFile(void);
};

#endif // !defined(AFX_FacePartsToolWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
