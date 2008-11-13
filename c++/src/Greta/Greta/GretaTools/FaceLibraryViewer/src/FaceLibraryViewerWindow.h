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

// FaceLibraryViewerWindow.h: interface for the FaceLibraryViewerWindow class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <fl/fl_counter.h>
#include <fl/Fl_Slider.h>
#include <fl/Fl_Check_Button.h>
#include <fl/Fl_Round_Button.h>
#include <fl/Fl_Box.h>
#include <fl/fl_value_output.h>
#include "FaceLibraryViewerMenu.h"
#include "FaceLibraryViewerGlutWindow.h"
#include "AboutWindow.h"
#include "FAPsImage.h"
#include "FaceExpressionDictionary.h"	// Added by ClassView
#include "FaceEngineGlobals.h"
//#include "FaceLibraryParser.h"
#include "FaceEngine.h"
#include ".\openwin.h"

class FaceLibraryViewerWindow : public Fl_Window  
{
public:

	FaceExpression * GetSelectedExpression();
	void LoadLibrary();
	int SaveGeometry();
	int SaveGeometryVRML();
	FAPFrame * GetSelectedFAPFrame(int n);
	void CopyFAPValuesToFAPFrame();
	void UpdateFAPValues(FAPFrame *f);
	char* FaceLibraryViewerWindow::GetLabel(int fap);
	char labels[69][3];
	//char min[69][20];
	//char max[69][20];
	//Fl_Counter *fapcounter[69];
	Fl_Slider *fapcounter[69];
	Fl_Slider *facscounter[6];
	Fl_Slider *lipscounter[8];
	Fl_Counter *framenumber;
	Fl_Round_Button *mouth;
	Fl_Round_Button *eyes_eyebrows;
	Fl_Round_Button *head;
	Fl_Round_Button *cheek;
	Fl_Value_Output *probability;
	void LoadFaceFrame(int n);
	void LoadInstances();
	Fl_File_Chooser *chooser;
	Fl_Choice *classes;
	Fl_Choice *instances;
	Fl_Box *boxFACS;
	Fl_Box *boxFAPS;
	Fl_Box *boxLIPS;
	FaceExpressionDictionary facelibrary;
	FAPFrame *frame;
	int Index;
	int IndexB;
	int IndexFACS;
	int IndexLIPS;
	float AU1[6];
	float AU2[6];
	float AU4[6];
	float AU5[2];
	float AU6[2];
	float AU7[4];

	// Lip Parameters
	float ULO[6];
	float LLO[6];
	float JAW[4];
	float LSW[6];
	float ULP[2];
	float LLP;
	float CRR[2];
	float CRL[2];

	Fl_JPEG_Image *button1image;
	Fl_Button *reset;
	Fl_Button *button1;
	Fl_Button *button2;
	int handle(int event);
	FaceLibraryViewerWindow();
	virtual ~FaceLibraryViewerWindow();
	FaceLibraryViewerMenu *menu;
	FaceLibraryViewerGlutWindow *glutw;
	FaceLibraryViewerGlutWindow *glutw2;
	int SaveFAPFrame();
	int SaveFAPFile();
	AboutWindow *about;
	int bounds[69][2];
	int facsbounds[6][2];
	int lipsbounds[8][2];
	Fl_Input *lframe[69];
	Fl_Input *lfacs[6];
	Fl_Input *llips[8];
	FAPsImage *fi;
	OpenWin *ow;

private:
//	FaceLibraryParser *FaceLibParser;
	int position_y;
	int position_x;
	int height;
	int width;
	
};