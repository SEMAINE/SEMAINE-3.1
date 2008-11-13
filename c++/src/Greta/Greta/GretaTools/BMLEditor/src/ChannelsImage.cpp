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

// ChannelsImage.cpp: implementation of the ChannelsImage class.
//
//////////////////////////////////////////////////////////////////////

#include "ChannelsImage.h"
#include "ScoreWindow.h"
#include "Channel.h"
#include "BlockPropertiesWindow.h"
#include <stdio.h>
#include <FL/fl_draw.H>
#include <math.h>
#include <vector>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void ChannelsImage::draw()
{
	int i;
	fl_color(255,255,255);
	fl_rectf(position_x,position_y,this->w(),this->h());

	if(tempblock!=0)
		tempblock->draw(this);

	fl_frame("AAAA",position_x,position_y,this->w(),this->h());
	fl_color(0,0,0);
	fl_line_style(FL_SOLID);
	Line(5,5,this->w()-5,5);
	char s[8];

	incr=(((float)this->w()-5)/zoom);

	starttime=startingfrom*(maxtime/100.f);
	endtime=starttime+maxtime/zoom;
	if(endtime>maxtime)
		endtime=maxtime;
	incr=0.01f;
	
	int posx,prevx;
	prevx=-100;
	for(i=0;i<((endtime-starttime)/incr);i++)
	{
		posx=(i*incr)*(w())/(endtime-starttime);
		if((posx-prevx)<25)
			continue;
		prevx=posx;
		Line(posx,5,posx,10);
		//sprintf(s,"%d",i+startingfrom);
		sprintf(s,"%.2f",starttime+i*incr);
		Text(s,posx,25);
	}
	fl_line_style(FL_DOT);
	fl_color(200,200,200);
	prevx=-100;
	for(i=0;i<((endtime-starttime)/incr);i++)
	{
		posx=(i*incr)*(w())/(endtime-starttime);
		if((posx-prevx)<25)
			continue;
		prevx=posx;
		Line(posx,45,posx,this->h()-5);
	}

	std::vector<Channel>::iterator iter;
	for(iter=((ScoreWindow*)window)->channels.begin();iter!=((ScoreWindow*)window)->channels.end();iter++)
	{
		Line(5,(*iter).position_y-position_y,this->w()-5,(*iter).position_y-position_y);
		Line(5,(*iter).position_y+(*iter).height-position_y,this->w()-5,(*iter).position_y+(*iter).height-position_y);
	}
	
}

void ChannelsImage::Line(int x,int y,int x1,int y1)
{
	fl_line(position_x+x,position_y+y,position_x+x1,position_y+y1);
}

void ChannelsImage::Text(char *s,int x,int y)
{
	fl_font(FL_HELVETICA,9);
	fl_draw(s,position_x+x,position_y+y);
}

int ChannelsImage::handle(int e)
{
	std::string movedonchannel;

	float movedontime=starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w();

	
	possibleresize=false;
	fl_cursor(FL_CURSOR_ARROW,FL_BLACK,FL_WHITE);
	
	std::vector<Channel>::iterator iter;			
	for(iter=(((ScoreWindow*)window)->channels).begin();iter!=(((ScoreWindow*)window)->channels).end();iter++)
		if((Fl::event_y()>(*iter).position_y)&&(Fl::event_y()<(*iter).position_y+(*iter).height))
		{
			movedonchannel=(*iter).name;
			break;
		}
	
	if(movedonchannel=="speech1") return(1);

	if(movedonchannel!="")
		if(
			(((((ScoreWindow*)window)->GetChannel(movedonchannel))->IsThereBlockAt(movedontime))))
		{
			Block* movedonblock;
			movedonblock=(((((ScoreWindow*)window)->GetChannel(movedonchannel))->GetTheBlockAt(movedontime)));
			if(movedonblock!=0)
			{
				int blockendx;

				blockendx=x()+((movedonblock->signal->start+movedonblock->signal->duration)-starttime)*(w()/(endtime-starttime));

				if(abs(blockendx-Fl::event_x())<5)
				{
					fl_cursor(FL_CURSOR_WE,FL_BLACK,FL_WHITE);
					possibleresize=true;
				}
				else
				{
					possibleresize=false;
					fl_cursor(FL_CURSOR_ARROW,FL_BLACK,FL_WHITE);
				}
			}
		}

	
	if(Fl::event_inside(this))
	{
		if((e==FL_DRAG)&&(inserting==true))
		{
			dragendtime=starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w();
			if((dragendtime-dragstarttime)<0.01)
				dragendtime=dragstarttime+0.01;
			if(tempblock!=0)
			{
				if(tempblock->signal->duration!=dragendtime-dragstarttime)
				{
					tempblock->signal->duration=dragendtime-dragstarttime;
					tempblock->signal->duration_sym="";
				}
			}
			this->redraw();
		}
		if((e==FL_DRAG)&&(movingblock!=0))
		{
			if(!resizing)
			{
				float dur=movingblock->signal->duration; 
				movingblock->signal->start=(starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w())-movingoffset;
				if(movingblock->signal->start<0)
					movingblock->signal->start=0.0;
				movingblock->signal->start_sym="";
				if(dur!=movingblock->signal->duration)
				{
					movingblock->signal->duration=dur;
					movingblock->signal->duration_sym="";
				}
				if((movingblock->signal->duration+movingblock->signal->start)>maxtime)
				{
					movingblock->signal->start=maxtime-dur;
					if(dur!=movingblock->signal->duration)
					{
						movingblock->signal->duration=dur;
						movingblock->signal->duration_sym="";
					}
				}

			}
			else
			{
				if(movingblock->signal->duration!=movedontime-movingblock->signal->start)
				{
					movingblock->signal->duration=movedontime-movingblock->signal->start;
					movingblock->signal->duration_sym="";
				}
				if((movingblock->signal->duration)<0.08)
				{
					movingblock->signal->duration=0.08;
					movingblock->signal->duration_sym="";
				}
			}
			this->redraw();
			return 1;
		}
		if((Fl::event_button()==FL_RIGHT_MOUSE)&&(inserting==true))
		{
			dragstarttime=-1;
			inserting=false;
		}
		if((Fl::event_button()==FL_RIGHT_MOUSE)&&(inserting==false)&&(editingblock==false))
		{
			if(e==FL_PUSH)
			{
				Block *selectedblock=0;
				pushattime=starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w();
				std::vector<Channel>::iterator iter;
				pushatchannel="";
				for(iter=(((ScoreWindow*)window)->channels).begin();iter!=(((ScoreWindow*)window)->channels).end();iter++)
					if((Fl::event_y()>(*iter).position_y)&&(Fl::event_y()<(*iter).position_y+(*iter).height))
					{
						pushatchannel=(*iter).name;
						break;
					}
				if(pushatchannel!="")
					if(
						(((((ScoreWindow*)window)->GetChannel(pushatchannel))->IsThereBlockAt(pushattime))))
					{
						selectedblock=(((((ScoreWindow*)window)->GetChannel(pushatchannel))->GetTheBlockAt(pushattime)));
						BlockPropertiesWindow *bw;
						bw=new BlockPropertiesWindow(window,selectedblock);
						bw->set_modal();
						bw->show();
						editingblock=true;
					}
				return 1;
			}			
		}
		if((Fl::event_button()==FL_LEFT_MOUSE)&&(inserting==true))
		{
			if(e==FL_PUSH)
			{
				std::vector<Channel>::iterator iter;
				dragstartchannel="";
				for(iter=(((ScoreWindow*)window)->channels).begin();iter!=(((ScoreWindow*)window)->channels).end();iter++)
					if((Fl::event_y()>(*iter).position_y)&&(Fl::event_y()<(*iter).position_y+(*iter).height))
					{
						dragstartchannel=(*iter).name;
						break;
					}
				if(dragstartchannel!="")
				{
					dragstarttime=starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w();
					Channel *ch;
					ch=((ScoreWindow*)window)->GetChannel(dragstartchannel);
					if(!(ch->IsThereBlockAt(dragstarttime))	)
					{
						MMSystemSpace::Signal *newsignal;
						newsignal=new MMSystemSpace::Signal;
						newsignal->id="newblock";
						newsignal->modality=ch->type;
						newsignal->start=dragstarttime;
						newsignal->duration=0.04;
						tempblock=new Block("inserting",newsignal,this->position_x,this->position_y);
						tempblock->channelname=dragstartchannel;
						((ScoreWindow*)window)->SignalsVec.push_back(newsignal);
					}
					else
					{
						dragstartchannel="";
						dragstarttime=-1;
					}
					this->redraw();
					return 1;
				}
			}
			if(e==FL_RELEASE)
			{
				if(dragstarttime!=-1)
				{
					if(dragendtime!=0)
					{
						if(!
						((((ScoreWindow*)window)->GetChannel(dragstartchannel))->IsThereBlockAt(dragendtime))
						)
						{
							Channel *ch;
							ch=((ScoreWindow*)window)->GetChannel(dragstartchannel);
							if(ch!=0)
							{
								this->AddBlock("");
								Channel *ch;
								ch=((ScoreWindow*)window)->GetChannel(dragstartchannel);
								BlockPropertiesWindow *bw;
								bw=new BlockPropertiesWindow(window,ch->GetLastBlock());
								bw->set_modal();
								bw->show();
							}
						}
						else
							EndInserting();
					}
				}
			}
		}
		
		if((Fl::event_button()==FL_LEFT_MOUSE)&&(inserting==false))
		{
			if(e==FL_PUSH)
			{
				movingblock=0;
				pushattime=starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w();
				std::vector<Channel>::iterator iter;
				pushatchannel="";
				for(iter=(((ScoreWindow*)window)->channels).begin();iter!=(((ScoreWindow*)window)->channels).end();iter++)
					if((Fl::event_y()>(*iter).position_y)&&(Fl::event_y()<(*iter).position_y+(*iter).height))
					{
						pushatchannel=(*iter).name;
						break;
					}
				if(pushatchannel!="")
					if(
						(((((ScoreWindow*)window)->GetChannel(pushatchannel))->IsThereBlockAt(pushattime))))
						movingblock=(((((ScoreWindow*)window)->GetChannel(pushatchannel))->GetTheBlockAt(pushattime)));
				if(movingblock!=0)
				{
					movingoffset=(starttime+(Fl::event_x()-this->position_x)*(endtime-starttime)/w())-movingblock->signal->start;
					fl_cursor(FL_CURSOR_WE,FL_BLACK,FL_WHITE);
				}
				if(possibleresize)
					resizing=true;
				else
					resizing=false;
				return 1;
			}
			if(e==FL_RELEASE)
			{
				movingblock=0;
				fl_cursor(FL_CURSOR_ARROW,FL_BLACK,FL_WHITE);
				return 1;
			}
			//printf("moving block %d\n",movingblock);	
		}
	}
		
	return Fl_Box::handle(e);
}

ChannelsImage::ChannelsImage(int x, int y, int w, int h, const char* label,void *wind):Fl_Box(x,y,w,h,label)
{
	position_x=x;
	position_y=y;
	startingfrom=0;
	zoom=20;
	inserting=false;
	editingblock=false;
	dragstarttime=-1;
	dragstartchannel=-1;
	dragendtime=0;
	tempblock=0;
	window=wind;
	movingblock=0;
}



//DEL void ChannelsImage::resize(int x, int y, int w, int h)
//DEL {
//DEL 	position_x=x;
//DEL 	position_y=y;
//DEL 	width=w;
//DEL 	height=h;
//DEL }

void ChannelsImage::AddBlock(std::string n)
{
	Channel *ch;
	MMSystemSpace::Signal *newsignal;
	ch=((ScoreWindow*)window)->GetChannel(dragstartchannel);
	newsignal=new MMSystemSpace::Signal;
	newsignal->id=n;
	newsignal->start=dragstarttime;
	newsignal->duration=dragendtime-dragstarttime;
	newsignal->modality=ch->type;
	ch->AddBlock(dragstartchannel,newsignal,this->position_x,this->position_y);
	((ScoreWindow*)window)->SignalsVec.push_back(newsignal);
	//CancelInserting();
	
}

void ChannelsImage::EndInserting()
{
	dragstartchannel="";
	inserting=false;
	editingblock=false;
	delete tempblock;
	tempblock=0;
	dragstarttime=-1;
	dragendtime=0;
	this->redraw();
}
