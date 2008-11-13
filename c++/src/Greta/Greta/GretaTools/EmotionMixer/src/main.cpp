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

#include <FL/Fl.H>
#include "EmotionMixer.h"
#include "DataContainer.h"
#include "IniManager.h"

DataContainer *datacontainer;
IniManager inimanager;
std::string ini_filename;


EmotionMixer *window;



int main (int argc, char *argv[])
{
	if(argc==2)
	{
		ini_filename=argv[1];
	} else {
		ini_filename="greta.ini";	
	}
	
	//datacontainer needs some data..
	inimanager.ReadIniFile(ini_filename);
	
	datacontainer = new DataContainer();

	//load faces, gestures etc
//Radek!!!
	int code=datacontainer->initAll();
	//int code=datacontainer->initFMLEngine();
	if (code==0) {
		printf("Problem : out \n");
		exit(1);
	}

	window = new EmotionMixer();
	
	window->show();
	
	window->glutw->show();

	Fl::run();

	//delete window;
	return 1;
}