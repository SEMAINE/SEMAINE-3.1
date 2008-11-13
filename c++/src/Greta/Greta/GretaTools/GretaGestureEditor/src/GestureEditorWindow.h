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

// GestureEditorWindow.h: interface for the GestureEditorWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GestureEditorWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_GestureEditorWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.h>
#include <FL/Fl_Counter.H>
#include <fl/fl_value_output.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Choice.h>
#include <FL/Fl_Round_Button.h>
#include <vector>
#include "GestureEngine.h"
#include "GLContainer.h"
#include "ExpressivityWindow.h"

class GestureEditorWindow : public Fl_Window  
{
public:

	Fl_Input *gestureclass;
	Fl_Input *gestureinstance;
	Fl_Input *gesturefilename;
	Fl_Output *duration;
	Fl_Button *durationplus;
	Fl_Button *durationminus;

	Fl_Button *save;
	Fl_Button *load;
	Fl_Button *testgesture;
	Fl_Button *quit;

	Fl_Toggle_Button *sideright;
	Fl_Toggle_Button *sideleft;
	Fl_Toggle_Button *sideboth;
	Fl_Toggle_Button *sideassym;

	Fl_Toggle_Button *assymright;
	Fl_Toggle_Button *assymleft;
	Fl_Button *assymswitch;

	Fl_Output *framenumber;
	Fl_Button *nextframe;
	Fl_Button *prevframe;
	Fl_Button *insertframe;
	Fl_Button *deleteframe;

	Fl_Check_Button *storedpose;

	Fl_Output *posefilename;

	Fl_Button *browseposefile;

	Fl_Choice *frametype;

	Fl_Slider *phasestartslider;

	Fl_Output *phasestartoutput;

	Fl_BMP_Image *bmp_arm_x;
	Fl_BMP_Image *bmp_arm_y;
	Fl_BMP_Image *bmp_arm_z;
	Fl_BMP_Image *bmp_hand_form;
	Fl_BMP_Image *bmp_hand_symbol_open;
	Fl_BMP_Image *bmp_hand_symbol_closed;
	Fl_BMP_Image *bmp_thumb_form;
	Fl_BMP_Image *bmp_thumb_symbol_open;
	Fl_BMP_Image *bmp_thumb_symbol_closed;
	Fl_BMP_Image *bmp_orientation;

	Fl_Round_Button *arm_x[5];
	Fl_Round_Button *arm_y[7];
	Fl_Round_Button *arm_z[3];

	Fl_Check_Button *fixed_x;
	Fl_Check_Button *fixed_y;
	Fl_Check_Button *fixed_z;

	Fl_Round_Button *hand_form[6];
	Fl_Round_Button *hand_symbol_open[3];
	Fl_Round_Button *hand_symbol_closed[3];
	Fl_Check_Button *fixed_form;
	Fl_Round_Button *thumb_form[3];
	Fl_Round_Button *thumb_symbol_open[3];
	Fl_Round_Button *thumb_symbol_closed[4];
	Fl_Round_Button *orientation_palm[2][6];
	Fl_Round_Button *orientation_fingers[2][6];

	Fl_Output *wristmix;
	Fl_Button *wristmixplus;
	Fl_Button *wristmixminus;

	Fl_Round_Button *shape[3];
	Fl_Group *shape_group;
	
	Fl_Check_Button *use_transition;
	Fl_Round_Button *movement_type[5];
	Fl_Round_Button *plane[3];
	Fl_Round_Button *direction_start[4];
	Fl_Slider *amplitude;
	Fl_Slider *frequency;
	Fl_Check_Button *inverse_way;
	Fl_Slider *x_amplitude;
	Fl_Slider *y_amplitude;
	Fl_Slider *z_amplitude;
	Fl_Slider *x_frequency;
	Fl_Slider *y_frequency;
	Fl_Slider *z_frequency;
	Fl_Slider *x_shift;
	Fl_Slider *y_shift;
	Fl_Slider *z_shift;
	Fl_Slider *x_space_variation;
	Fl_Slider *y_space_variation;
	Fl_Slider *z_space_variation;
	Fl_Slider *x_temp_variation;
	Fl_Slider *y_temp_variation;
	Fl_Slider *z_temp_variation;
	Fl_BMP_Image *bmp_planes;

	GLContainer *agentwindow;

	ExpressivityWindow *expressivitywindow;

	GestureEditorWindow();
	virtual ~GestureEditorWindow();
	int handle(int e);

	void draw();
	
	void ShowActualSide();
	
	void ShowActualTransition();

	void ShowActualFrame();

	void Save();

	void Load();

	void Test();
	void DeactivateAll();

	void ActivateAll();

	GestureSpace::Gesture *gesture;

	int actualframe;

	GestureSpace::CurvedTransition* GetActualTransition(){return (*phasesVector)[actualframe]->GetCurvedTransition();}

	bool firstArm; //the first arm is curently modify. It can be false only with assymetric gestures
	GestureSpace::GesturePhaseVector *phasesVector;

};

#endif // !defined(AFX_GestureEditorWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
