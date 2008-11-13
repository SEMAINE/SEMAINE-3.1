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

// MainWindow.cpp: implementation of the MainWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "MainWindow.h"
#include "IniManager.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl.H>
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>

IniManager inimanager;

MainWindow::MainWindow():Fl_Window(100,100,435,140,"Anvil Analyzer")
{
	loadbutton=new Fl_Button(5,100,180,30,"load anvil");

	savebmlbutton=new Fl_Button(250,100,180,30,"export to bml");

	converterEli=new Fl_Round_Button(10,20,16,16,"Converter Eli");
	converterEli->align(FL_ALIGN_RIGHT);
	converterEli->value(1);

	converterMau=new Fl_Round_Button(250,20,16,16,"Converter Mau");
	converterMau->align(FL_ALIGN_RIGHT);
	converterMau->value(0);

	inimanager.ReadIniFile("greta.ini");

	anvilmanager=0;
}

MainWindow::~MainWindow()
{

}

int MainWindow::handle(int e)
{
	if((Fl::event_inside(loadbutton))&&(e==FL_PUSH))
	{
		Fl_File_Chooser *chooser;
		chooser= new Fl_File_Chooser(inimanager.Program_Path.c_str(),"anvil files (*.anvil)",0,"select an anvil file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		if(chooser->value()!=0)
		{
			anvilfilename=std::string(chooser->value());
			if(anvilmanager!=0)
				delete anvilmanager;
			if(converterEli->value()==1)
			{
				anvilmanager2=new AnvilManager2();
				anvilmanager2->ParseAnvil(anvilfilename);
			}
			else
				if(converterMau->value()==1)
				{
					anvilmanager=new AnvilManager();
					anvilmanager->ParseAnvil(anvilfilename);
				}
		}
		delete chooser;

	}
	if((Fl::event_inside(savebmlbutton))&&(e==FL_PUSH))
	{
		Fl_File_Chooser *chooser;
		chooser= new Fl_File_Chooser(inimanager.Program_Path.c_str(),"bml files (*.xml)",2,"select an anvil file");
		chooser->show();
		while (chooser->shown())
			Fl::wait();
		if(chooser->value()!=0)
		{
			if(converterEli->value()==1)
				anvilmanager2->SaveBML(chooser->value());
			else
				if(converterMau->value()==1)
					anvilmanager->SaveBML(chooser->value());
		}
		delete chooser;
	}
	if((Fl::event_inside(converterEli))&&(e==FL_PUSH))
	{
		if(converterEli->value()==1)
			converterMau->value(1);
		else
			converterMau->value(0);

		redraw();
	}
	if((Fl::event_inside(converterMau))&&(e==FL_PUSH))
	{
		if(converterMau->value()==1)
			converterEli->value(1);
		else
			converterEli->value(0);
		redraw();
	}
	return Fl_Window::handle(e);
}

void MainWindow::draw()
{
	Fl_Window::draw();

}

