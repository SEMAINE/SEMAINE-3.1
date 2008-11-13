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

// BlockPropertiesWindow.cpp: implementation of the BlockPropertiesWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "BlockPropertiesWindow.h"
#include "ScoreWindow.h"
#include "Block.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <vector>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>
#include <FL/Fl.H>

void selectedOk(Fl_Widget*w, void* v)
{
	ScoreWindow *win;
	Block *blk;
	win=(ScoreWindow*)((BlockPropertiesWindow*)v)->w;
	blk=(Block*)((BlockPropertiesWindow*)v)->b;
	float s, e;
	int ret;

	if(strcmp(((BlockPropertiesWindow*)v)->name->value(),"")==0)
	{
		((BlockPropertiesWindow*)v)->message="give a name to the block";
		((BlockPropertiesWindow*)v)->redraw();
	}
	if((strcmp(((BlockPropertiesWindow*)v)->classname->text(),"")==0)||(strcmp(((BlockPropertiesWindow*)v)->instancename->text(),"")==0))
	{
		((BlockPropertiesWindow*)v)->message="class and instance must be specified";
		((BlockPropertiesWindow*)v)->redraw();
	}
	if(strcmp(((BlockPropertiesWindow*)v)->name->value(),"")!=0)
	{
		ret=((BlockPropertiesWindow*)v)->SetStartEnd(((BlockPropertiesWindow*)v)->start->value(), ((BlockPropertiesWindow*)v)->end->value());
		if(ret==1)
		{
			((BlockPropertiesWindow*)v)->hide();
			((BlockPropertiesWindow*)v)->finished=true;
			blk->SetName(((BlockPropertiesWindow*)v)->name->value());

			/*s=atof(((BlockPropertiesWindow*)v)->start->value());
			e=atof(((BlockPropertiesWindow*)v)->end->value());
			if(s<e)
				blk->SetTimes(s, e);*/
			blk->SetClassInstance(((BlockPropertiesWindow*)v)->classname->text(),((BlockPropertiesWindow*)v)->instancename->text());
			blk->SetExpressivity(((BlockPropertiesWindow*)v)->SPC->value(),((BlockPropertiesWindow*)v)->TMP->value(),((BlockPropertiesWindow*)v)->FLD->value(),((BlockPropertiesWindow*)v)->PWR->value(),((BlockPropertiesWindow*)v)->REP->value());
			win->channelsview->EndInserting();
			win->redraw();
		}
		else
		if(ret==0)
		{
			((BlockPropertiesWindow*)v)->message="signal duration must be longer then 0.08 sec.";
			((BlockPropertiesWindow*)v)->redraw();
		}
		else
		if(ret==2)
		{
			((BlockPropertiesWindow*)v)->message="time markers are not defined, maybe the Mary server is off!";
			((BlockPropertiesWindow*)v)->redraw();
		}
	}
}

void selectedCancel(Fl_Widget*w, void* v)
{
	ScoreWindow *win;
	Block *blk;

	blk=(Block*)((BlockPropertiesWindow*)v)->b;
	win=(ScoreWindow*)((BlockPropertiesWindow*)v)->w;
	((BlockPropertiesWindow*)v)->hide();
	((BlockPropertiesWindow*)v)->finished=true;
	win->channelsview->EndInserting();
	win->redraw();
	if(blk->signal->id=="") //if(blk->name=="")
		win->DeleteOneBlock(blk->channelname.c_str(), blk->signal->id.c_str());
}

void selectedDelete(Fl_Widget*w, void* v)
{
	ScoreWindow *win;
	Block *blk;

	blk=(Block*)((BlockPropertiesWindow*)v)->b;
	win=(ScoreWindow*)((BlockPropertiesWindow*)v)->w;
	((BlockPropertiesWindow*)v)->hide();
	((BlockPropertiesWindow*)v)->finished=true;
	win->channelsview->EndInserting();

	win->DeleteOneBlock(blk->channelname.c_str(), blk->signal->id.c_str());
	
}

void selectStart(Fl_Widget*w, void* v)
{
	((BlockPropertiesWindow*)v)->start->value(((BlockPropertiesWindow*)v)->starttm->text());	
}

void selectEnd(Fl_Widget*w, void* v)
{

	((BlockPropertiesWindow*)v)->end->value(((BlockPropertiesWindow*)v)->endtm->text());	
}

void setInstances(Fl_Widget*w, void* v)
{	
	int i;
	ScoreWindow *win;
	Block *blk;
	win=(ScoreWindow*)((BlockPropertiesWindow*)v)->w;
	blk=(Block*)((BlockPropertiesWindow*)v)->b;
	
	((BlockPropertiesWindow*)v)->instancename->clear();
	
	if(blk->channelname=="face1" || blk->channelname=="face2")
	{
		//printf("ho selezionato: %s\n",((BlockPropertiesWindow*)v)->classname->text(((BlockPropertiesWindow*)v)->classname->value()));
		for(i=0;i<win->FacialExpressionVec[((BlockPropertiesWindow*)v)->classname->value()]->InstanceVec.size();i++)
		{
			((BlockPropertiesWindow*)v)->instancename->add(win->FacialExpressionVec[((BlockPropertiesWindow*)v)->classname->value()]->InstanceVec[i].c_str());
			//printf("ho aggiuntoto: %s\n", win->FacialExpressionVec[((BlockPropertiesWindow*)v)->classname->value()].InstanceVec[i].c_str());
		}
		((BlockPropertiesWindow*)v)->instancename->value(0);
	}

	if(blk->channelname=="gesture1" || blk->channelname=="gesture2")
	{
		for(i=0;i<win->GestureVec[((BlockPropertiesWindow*)v)->classname->value()]->InstanceVec.size();i++)
			((BlockPropertiesWindow*)v)->instancename->add(win->GestureVec[((BlockPropertiesWindow*)v)->classname->value()]->InstanceVec[i].c_str());
		((BlockPropertiesWindow*)v)->instancename->value(0);
	}
}

BlockPropertiesWindow::BlockPropertiesWindow(void *window,void *block):Fl_Window(((ScoreWindow*)window)->x()+100,((ScoreWindow*)window)->y()+100,500,300,"block properties")
{
	ScoreWindow *win;
	int tm=1;
	std::string timemarker;
	char app[5];

	name=new Fl_Input(70,20,150,20,"name:");
	classname=new Fl_Choice(70,50,150,20,"class:");
	classname->callback((Fl_Callback *)setInstances,this);
	instancename=new Fl_Choice(70,80,150,20,"instance:");
	start=new Fl_Input(70,110,60,20,"start time:");
	end=new Fl_Input(70,140,60,20,"dur time:");
	starttm=new Fl_Choice(160,110,60,20,"TM:");
	starttm->callback((Fl_Callback *)selectStart,this);
	endtm=new Fl_Choice(160,140,60,20,"TM:");
	endtm->callback((Fl_Callback *)selectEnd,this);
	b=block;

	w=window;
	win=(ScoreWindow*)w;

	std::map<std::string,MMSystemSpace::TimeMarker>::iterator itertimem;
	for(itertimem=win->speech->timemarkers.begin();itertimem!=win->speech->timemarkers.end();itertimem++)
	{
		timemarker=win->speech->id + ":" + itertimem->second.id;
		starttm->add(timemarker.c_str());
		endtm->add(timemarker.c_str());
	}

	std::vector<MMSystemSpace::Signal*>::iterator itersig;
	for(itersig=win->SignalsVec.begin();itersig!=win->SignalsVec.end();itersig++)
	{
		for(itertimem=(*itersig)->timemarkers.begin();itertimem!=(*itersig)->timemarkers.end();itertimem++)
		{
			timemarker=(*itersig)->id + ":" + itertimem->second.id;
			starttm->add(timemarker.c_str());
			endtm->add(timemarker.c_str());
		}
	}

	okbutton=new Fl_Button(150,210,50,25,"ok");
	okbutton->callback((Fl_Callback *)selectedOk,this);
	cancelbutton=new Fl_Button(210,210,50,25,"cancel");
	cancelbutton->callback((Fl_Callback *)selectedCancel,this);
	deletebutton=new Fl_Button(270,210,100,25,"delete block");
	deletebutton->callback((Fl_Callback *)selectedDelete,this);

	char st[20];
	if(((Block*)b)->signal->start_sym=="")
	{
		sprintf(st,"%.2f",((Block*)b)->signal->start);
		start->value(st);
	}
	else
		start->value(((Block*)b)->signal->start_sym.c_str());

	
	if(((Block*)b)->signal->duration_sym=="")
	{
		sprintf(st,"%.2f",((Block*)b)->signal->duration);
		end->value(st);
	}
	else
		end->value(((Block*)b)->signal->duration_sym.c_str());


	finished=false;
	message="";
	name->value(((Block*)b)->signal->id.c_str());

	std::string parname="";
	//parname=((Block*)b)->signal->modality + ".SPC.value";
	parname="SPC.value";
	SPC=new Fl_Slider(300,20,150,15,"SPC");
	SPC->align(FL_ALIGN_LEFT);
	SPC->labelfont(FL_HELVETICA);
	SPC->labelsize(11);
	SPC->type(1);
	SPC->bounds(-1,1);
	SPC->step(0.01);
	SPC->value(0.0);
	SPC->value(((Block*)b)->signal->GetParamValuef(parname));

	//parname=((Block*)b)->signal->modality + ".TMP.value";
	parname="TMP.value";
	TMP=new Fl_Slider(300,50,150,15,"TMP");
	TMP->align(FL_ALIGN_LEFT);
	TMP->labelfont(FL_HELVETICA);
	TMP->labelsize(11);
	TMP->type(1);
	TMP->bounds(-1,1);
	TMP->step(0.01);
	TMP->value(0.0);
	TMP->value(((Block*)b)->signal->GetParamValuef(parname));

	//parname=((Block*)b)->signal->modality + ".FLD.value";
	parname="FLD.value";
	FLD=new Fl_Slider(300,80,150,15,"FLD");
	FLD->align(FL_ALIGN_LEFT);
	FLD->labelfont(FL_HELVETICA);
	FLD->labelsize(11);
	FLD->type(1);
	FLD->bounds(-1,1);
	FLD->step(0.01);
	FLD->value(0.0);
	FLD->value(((Block*)b)->signal->GetParamValuef(parname));

	//parname=((Block*)b)->signal->modality + ".PWR.value";
	parname="PWR.value";
	PWR=new Fl_Slider(300,110,150,15,"PWR");
	PWR->align(FL_ALIGN_LEFT);
	PWR->labelfont(FL_HELVETICA);
	PWR->labelsize(11);
	PWR->type(1);
	PWR->bounds(-1,1);
	PWR->step(0.01);
	PWR->value(0.0);
	PWR->value(((Block*)b)->signal->GetParamValuef(parname));

	//parname=((Block*)b)->signal->modality + ".REP.value";
	parname="REP.value";
	REP=new Fl_Slider(300,140,150,15,"REP");
	REP->align(FL_ALIGN_LEFT);
	REP->labelfont(FL_HELVETICA);
	REP->labelsize(11);
	REP->type(1);
	REP->bounds(-1,1);
	REP->step(0.01);
	REP->value(0.0);
	REP->value(((Block*)b)->signal->GetParamValuef(parname));


	LoadClassInstance();
}

void BlockPropertiesWindow::LoadClassInstance()
{
	int i;
	int instanceselected, classselected=0;
	if(((Block*)b)->channelname=="face1" || ((Block*)b)->channelname=="face2")
	{
		for(i=0;i<((ScoreWindow*)w)->FacialExpressionVec.size();i++)
		{
			classname->add(((ScoreWindow*)w)->FacialExpressionVec[i]->classname.c_str());
			if(((ScoreWindow*)w)->FacialExpressionVec[i]->classname==((Block*)b)->classname)
				classselected=i;
		}
		classname->value(classselected);

		
		instanceselected=0;
		for(i=0;i<((ScoreWindow*)w)->FacialExpressionVec[classselected]->InstanceVec.size();i++)
		{
			instancename->add(((ScoreWindow*)w)->FacialExpressionVec[classselected]->InstanceVec[i].c_str());
			if(((ScoreWindow*)w)->FacialExpressionVec[classselected]->InstanceVec[i]==((Block*)b)->instancename)
				instanceselected=i;
		}
		instancename->value(instanceselected);
	}

	if(((Block*)b)->channelname=="head1" || ((Block*)b)->channelname=="head2")
	{
		classname->add(((ScoreWindow*)w)->HeadVec[0]->classname.c_str());
		classname->value(0);
		classname->deactivate();

		instanceselected=0;
		for(i=0;i<((ScoreWindow*)w)->HeadVec[0]->InstanceVec.size();i++)
		{
			instancename->add(((ScoreWindow*)w)->HeadVec[0]->InstanceVec[i].c_str());
			if(((ScoreWindow*)w)->HeadVec[0]->InstanceVec[i]==((Block*)b)->instancename)
				instanceselected=i;
		}
		instancename->value(instanceselected);
	}

	if(((Block*)b)->channelname=="gaze1" || ((Block*)b)->channelname=="gaze2")
	{
		classname->add(((ScoreWindow*)w)->GazeVec[0]->classname.c_str());
		classname->value(0);
		classname->deactivate();

		instanceselected=0;
		for(i=0;i<((ScoreWindow*)w)->GazeVec[0]->InstanceVec.size();i++)
		{
			instancename->add(((ScoreWindow*)w)->GazeVec[0]->InstanceVec[i].c_str());
			if(((ScoreWindow*)w)->GazeVec[0]->InstanceVec[i]==((Block*)b)->instancename)
				instanceselected=i;
		}
		instancename->value(instanceselected);
	}

	classselected=0;
	if(((Block*)b)->channelname=="gesture1" || ((Block*)b)->channelname=="gesture2")
	{
		for(i=0;i<((ScoreWindow*)w)->GestureVec.size();i++)
		{
			classname->add(((ScoreWindow*)w)->GestureVec[i]->classname.c_str());
			if(((ScoreWindow*)w)->GestureVec[i]->classname==((Block*)b)->classname)
				classselected=i;
		}
		classname->value(classselected);

		instanceselected=0;
		for(i=0;i<((ScoreWindow*)w)->GestureVec[classselected]->InstanceVec.size();i++)
		{
			instancename->add(((ScoreWindow*)w)->GestureVec[classselected]->InstanceVec[i].c_str());
			if(((ScoreWindow*)w)->GestureVec[classselected]->InstanceVec[i]==((Block*)b)->instancename)
				instanceselected=i;
		}
		instancename->value(instanceselected);
	}

	if(((Block*)b)->channelname=="torso1" || ((Block*)b)->channelname=="torso2")
	{
		classname->add("torso");
		classname->value(0);
		classname->deactivate();

		instanceselected=0;
		for(i=0;i<((ScoreWindow*)w)->TorsoVec.size();i++)
		{
			instancename->add(((ScoreWindow*)w)->TorsoVec[i].c_str());
			if(((ScoreWindow*)w)->TorsoVec[i]==((Block*)b)->instancename)
				instanceselected=i;
		}
		instancename->value(instanceselected);
	}
}

BlockPropertiesWindow::~BlockPropertiesWindow()
{

}

int BlockPropertiesWindow::handle(int e)
{
	if(e==FL_PUSH)
	{
		if(Fl::event_inside(okbutton))
		{
		}
		if(Fl::event_inside(cancelbutton))
		{
		}
	}
	return Fl_Window::handle(e);
}

void BlockPropertiesWindow::draw()
{
	Fl_Window::draw();
	fl_color(0,0,0);
	fl_line(1,275,500,275);

	fl_font(FL_HELVETICA,11);
	
	fl_draw("-1",295,170);
	fl_draw("1",445,170);
	fl_draw("0",373,170);

	Text((char*)message.c_str(),10,293);
}

void BlockPropertiesWindow::Text(char *s,int x,int y)
{
	fl_font(FL_HELVETICA,13);
	fl_color(0,0,0);
	fl_draw(s,x,y);
}

int BlockPropertiesWindow::SetStartEnd(std::string start,std::string end)
{
	Block *blk;
	blk=(Block*)b;
	ScoreWindow *win;
	win=(ScoreWindow*)w;

	blk->signal->start_sym=start;
	blk->signal->duration_sym=end;

	blk->signal->concretized=false;

	int lp=0;
	blk->signal->Temporize(win->SignalsVec, win->speech, &lp);

	if(blk->signal->concretized==true)
		return(1);
	else
		return(0);
}
