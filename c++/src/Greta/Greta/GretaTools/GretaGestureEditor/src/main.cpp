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

#include "GestureEditorWindow.h"
#include <FL/Fl.H>
#include "RandomGen.h"
#include "BAPConverter.h"
#include "DataContainer.h"
#include "IniManager.h"

extern std::string ini_filename;

extern IniManager inimanager;

extern RandomGen *randomgen;
BAPConverter TorsoGlobalBAPconverter;
DataContainer * datacontainer;
std::string ini_filename = "greta.ini";

void main()
{
	randomgen=new RandomGen();
	ini_filename="greta.ini";		
	//datacontainer needs some data..
	inimanager.ReadIniFile(ini_filename);
	
	datacontainer = new DataContainer();
	//load faces, gestures etc
	int code=datacontainer->initBMLEngine();	
	if (code==0) {
		printf("Problem : out \n");
		exit(1);
	}

	GestureEditorWindow *window;
	window = new GestureEditorWindow();
	
	window->show();
  
	Fl::run();

	delete window;
}