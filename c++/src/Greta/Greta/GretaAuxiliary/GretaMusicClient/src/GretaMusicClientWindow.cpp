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

// GretaMusicClientWindow.cpp: implementation of the GretaMusicClientWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "GretaMusicClientWindow.h"
#include "FL/Fl.H"
#include <FL/fl_draw.H>
#include "SDL_net.h"
#include <stdio.h>
#include <string.h>

TCPsocket GretaMusicClientSocket;

FILE* face_log;


GretaMusicClientWindow::GretaMusicClientWindow():Fl_Window(10,20,330,400,"greta animation score 0.1")
{
	address=new Fl_Input(10,10,100,20);
	address->value("localhost");
	port=new Fl_Input(120,10,40,20);
	port->value("9999");
	connect=new Fl_Button(170,10,80,20,"connect");
	close=new Fl_Button(260,10,60,20,"close");
	message=new Fl_Input(10,40,240,20);
	send=new Fl_Button(260,40,60,20,"send");
	output=new Output(10,70,310,310);

	if(SDL_Init(0)==-1)
	{
	  printf("SDL_Init: %s\n", SDL_GetError());
	}
	else
		if(SDLNet_Init()==-1)
		{
			printf("SDLNet_Init: %s\n", SDLNet_GetError());
		}
}

GretaMusicClientWindow::~GretaMusicClientWindow()
{
	
}

int GretaMusicClientWindow::handle(int e)
{
	IPaddress ip;

	if(e==FL_PUSH)
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(Fl::event_inside(connect))
				if((strcmp(address->value(),"")!=0)&&(strcmp(port->value(),"")!=0))
				{
					output->printf("trying to connect\n\n");
					if(SDLNet_ResolveHost(&ip,address->value(),atoi(port->value()))==-1)
					{
						printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
						output->printf("SDLNet_ResolveHost error:\n");
						output->printf(SDLNet_GetError());
						output->printf("\n\n");
						return Fl_Window::handle(e);
					}

					GretaMusicClientSocket=SDLNet_TCP_Open(&ip);
					if(!GretaMusicClientSocket)
					{
						printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
						output->printf("SDLNet_TCP_Open error:\n");
						output->printf(SDLNet_GetError());
						output->printf("\n\n");
						return Fl_Window::handle(e);
					}
					output->printf("connection accepted\n\n");
				}
		
		if(e==FL_PUSH)
			if(Fl::event_button()==FL_LEFT_MOUSE)
				if(Fl::event_inside(close))
					this->hide();
		if(e==FL_PUSH)
			if(Fl::event_button()==FL_LEFT_MOUSE)
				if(Fl::event_inside(send))
					if(GretaMusicClientSocket!=0)
					{
						std::string s=message->value();
						int len,result;
						len=s.length(); // add one for the terminating NULL
						std::string t;
						t="";
						for(int i=0; i<=len; i++)
							if(((i<len)&&((s[i]=='\\')&&(s[i+1]=='n')))||(i==len))
							{
								t=t+'\0';
								if(i<len)
									i++;
								//t="";
								//t+="POST /postmessage?radiobutton=AIRCentral&checkbox_WB_Greta.Whiteboard=Greta.Whiteboard&messagetype=dfg&messagecontent=dfgdfg&post=post HTTP/1.1\n\n";
								result=SDLNet_TCP_Send(GretaMusicClientSocket,(void*)t.c_str(),t.length());
								if(result<t.length())
								{
									printf("SDLNet_TCP_Send: %s\n", SDLNet_GetError());
									output->printf("SDLNet_TCP_Send error:\n");
									output->printf(SDLNet_GetError());
									output->printf("\n\n");
									// It may be good to disconnect sock because it is likely invalid now.
								}
								t="";
							}
							else
								t=t+s[i];
					}

		return Fl_Window::handle(e);
}


