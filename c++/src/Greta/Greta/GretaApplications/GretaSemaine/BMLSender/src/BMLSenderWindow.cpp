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

// BMLSenderWindow.cpp: implementation of the BMLSenderWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "BMLSenderWindow.h"
//#include "BML_AgentEngine.h"
//#include "PlayerFLTKWindow.h"
#include "FL/Fl.H"

#include "BMLSenderPsydule.h"

//extern IniManager inimanager;


void selectedsendbmlbutton(Fl_Widget* w, void* v)
{
	std::string bml;
	bml="";
	bml+="<?xml version=\"1.0\" ?>\n";
	bml+="<!DOCTYPE score SYSTEM \"\" []>\n";
	bml+="<bml>\n";
	bml+="<face id='face_expression1' type='affect=anger' start='0.0' end='5.0'/>\n";
	bml+="<gesture id='gesture1' type='ADJECTIVAL=GENOVA2' start='1.0' end='4.0'/>\n";
	bml+="<gesture id='gesture1' stroke='2.5'/>\n";
	bml+="<gesture id='gesture1' expressivityspc='1'/>\n";
	bml+="<gesture id='gesture1' expressivitytmp='-1'/>\n";
	bml+="</bml>";
	if(((BMLSenderWindow*)v)->bp!=0)
		((BMLSenderPsydule*)((BMLSenderWindow*)v)->bp)->PostString(bml, "Greta.Whiteboard", "Greta.Data.InternalBMLCode");
}

void bmlsenderpsyduleidle(void* v)
{
	Message *msg;
	if(((BMLSenderWindow*)v)->bp!=0)
	{
		msg=((BMLSenderPsydule*)((BMLSenderWindow*)v)->bp)->ReceiveMessage(100);
		((BMLSenderPsydule*)((BMLSenderWindow*)v)->bp)->ReadMessage(msg);
	}
	((BMLSenderWindow*)v)->redraw();
}


BMLSenderWindow::BMLSenderWindow():Fl_Window(100,100,200,160,"bml sender")
{
	bp=(BMLSenderPsydule*)new BMLSenderPsydule("Greta.BMLSenderPsydule","127.0.0.1",10000);
	if(((BMLSenderPsydule*)bp)->Register("Greta.Whiteboard", "Greta.Data.EyesWebCues")==0)
	{
		printf("quitting...\n");
		delete bp;
		bp=0;
		this->hide();
	}
	else
	{

		sendBML=new Fl_Button(10,10,70,20,"send BML");
		sendBML->callback((Fl_Callback *)selectedsendbmlbutton,this);

		receivedAcc=new Fl_Output(100,40,40,20,"received Acc");
		receivedVel=new Fl_Output(100,70,40,20,"received Vel");
		receivedDI=new Fl_Output(100,100,40,20,"received DI");
		receivedCI=new Fl_Output(100,130,40,20,"received CI");

		this->redraw();

		Fl::add_idle(bmlsenderpsyduleidle,this);

		this->show();
	}
}

BMLSenderWindow::~BMLSenderWindow()
{
}

void BMLSenderWindow::draw()
{
	char value[10];
	sprintf(value,"%.2f",((BMLSenderPsydule*)bp)->Acc);
	receivedAcc->value(value);
	sprintf(value,"%.2f",((BMLSenderPsydule*)bp)->Vel);
	receivedVel->value(value);
	sprintf(value,"%.2f",((BMLSenderPsydule*)bp)->DI);
	receivedDI->value(value);
	sprintf(value,"%.2f",((BMLSenderPsydule*)bp)->CI);
	receivedCI->value(value);
	Fl_Window::draw();
}

int BMLSenderWindow::handle(int e)
{
	return Fl_Window::handle(e);
}
