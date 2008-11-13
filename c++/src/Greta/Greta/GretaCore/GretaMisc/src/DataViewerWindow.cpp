//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// DataViewerWindow.cpp: implementation of the DataViewerWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "DataViewerWindow.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include "FL/Fl_draw.H"
#include "FL/Fl.H"
#include "KeyPoint.h"
#include "IniManager.h"
#include "KeyPoint.h"

extern IniManager inimanager;
extern std::string ini_filename;

void selectedT(Fl_Widget* w, void* v)
{
	char value[255];
	sprintf(value,"%.2f",((DataViewerWindow*)v)->Tslider->value());
	((DataViewerWindow*)v)->Toutput->value(value);
	((DataViewerWindow*)v)->redraw();
}
void selectedC(Fl_Widget* w, void* v)
{
	char value[255];
	sprintf(value,"%.2f",((DataViewerWindow*)v)->Cslider->value());
	((DataViewerWindow*)v)->Coutput->value(value);
	((DataViewerWindow*)v)->redraw();
}
void selectedB(Fl_Widget* w, void* v)
{
	char value[255];
	sprintf(value,"%.2f",((DataViewerWindow*)v)->Bslider->value());
	((DataViewerWindow*)v)->Boutput->value(value);
	((DataViewerWindow*)v)->redraw();
}


DataViewerWindow::DataViewerWindow():Fl_Window(10,10,700,550,"interpolation viewer 0.1")
{
	inimanager.ReadIniFile(ini_filename);

	/*
	char value[10];
	Tslider=new Fl_Slider(10,480,350,10,"T");
	Tslider->align(FL_ALIGN_RIGHT);
	Tslider->type(FL_HOR_NICE_SLIDER);
	Tslider->bounds(-10,10);
	Tslider->step(0.1);
	Tslider->callback((Fl_Callback *)selectedT,this);
	Tslider->value(0);
	Toutput=new Fl_Output(380,475,40,20);
	sprintf(value,"%.2f",Tslider->value());
	Toutput->value(value);

	Cslider=new Fl_Slider(10,500,350,10,"C");
	Cslider->align(FL_ALIGN_RIGHT);
	Cslider->type(FL_HOR_NICE_SLIDER);
	Cslider->bounds(-10,10);
	Cslider->step(0.1);
	Cslider->callback((Fl_Callback *)selectedC,this);
	Cslider->value(0);
	Coutput=new Fl_Output(380,495,40,20);
	sprintf(value,"%.2f",Cslider->value());
	Coutput->value(value);

	Bslider=new Fl_Slider(10,520,350,10,"B");
	Bslider->align(FL_ALIGN_RIGHT);
	Bslider->type(FL_HOR_NICE_SLIDER);
	Bslider->bounds(-10,10);
	Bslider->step(0.1);
	Bslider->callback((Fl_Callback *)selectedB,this);
	Bslider->value(0);
	Boutput=new Fl_Output(380,515,40,20);
	sprintf(value,"%.2f",Bslider->value());
	Boutput->value(value);
*/
	//adsr=new ADSRGenerator(0,4,800,FaceEngineSpace::veryslow);
	//adsr->AddA(7.5f,7.8f,350);
	points1.clear();
	points2.clear();
	points3.clear();
	points4.clear();
	specialpoints.clear();
	minx=0;
	maxx=500;
	miny=-100000;
	maxy=100000;

	line1=0;
	line2=0;

	this->size_range(60,60,2048,1500);

	contline=false;
}

DataViewerWindow::~DataViewerWindow()
{
	//delete i;
}

int DataViewerWindow::handle(int e)
{

	return Fl_Window::handle(e);
}

void DataViewerWindow::draw()
{
	Fl_Window::draw();

	fl_color(255,255,255);
	fl_rectf(15,15,w()-30,h()-80-30);
	fl_rectf(15,h()-88,80,85);
	fl_color(0,0,0);
	fl_rect(15,15,w()-30,h()-80-30);
	fl_rect(15,h()-89,80,87);

	if((maxy>=0)&&(miny<=0))
	{
		float axx=h()-80-(15.0f+(0-miny)/(maxy-miny)*(h()-80-30.0f));
		fl_line(15,axx,w()-15,axx);
	}
	if((maxx>=0)&&(minx<=0))
	{
		float axy=15.0f+(0.0f-minx)/(maxx-minx)*(w()-30.0f);
		fl_line(axy,15,axy,h()-80-15);
	}

	int i;
	i=0;
	std::vector<SingleGraph*>::iterator itergraph;
	for(itergraph=graphs.begin();itergraph!=graphs.end();itergraph++)
	{
		(*itergraph)->draw(w(),h());
		(*itergraph)->draw_key(20,i*15);
		i++;
	}

	fl_color(0,0,0);


	//drawtcb();
}

void DataViewerWindow::drawtcb()
{

	i=new TCBInterpolator(Tslider->value(),Cslider->value(),Bslider->value());
	
//	std::vector<KeyPoint>::iterator kp;
//	for(kp=adsr->points.begin();kp!=adsr->points.end();kp++)
//	{
		//i->AddPoint((*kp).frame,(*kp).value);
		//printf("%d %f\n",(*kp).frame,(*kp).value);
//	}

	
	i->AddPoint(0,0,1,0,0);
	i->AddPoint(20,90);
	i->AddPoint(40,80);
	i->AddPoint(60,0,1,0,0);

	/*
	i->AddPoint(30,-11);
	i->AddPoint(-100,10);
	i->AddPoint(-40,12);
	i->AddPoint(-80,-5);
	i->AddPoint(-65,-15);
	
	i->AddPoint(-20,12);
	i->AddPoint(0,9);

	i->AddPoint(50,-7);
	*/

	i->Interpolate();



	int minx,maxx;
	float miny,maxy;
	minx=(*i->points.begin()).time;
	maxx=minx+i->result.size()-1;
	if(!i->result.empty())
	{
		miny=0;
		maxy=0;
		std::vector<float>::iterator iter;
		for(iter=i->result.begin();iter!=i->result.end();iter++)
		{
			if((*iter)>maxy)maxy=(*iter);
			if((*iter)<miny)miny=(*iter);
		}
	}
	fl_color(255,255,255);
	fl_rectf(15,15,w()-30,h()-80-30);
	fl_color(0,0,0);
	float axx=h()-80-(15.0f+(0-miny)/(maxy-miny)*(h()-80-30.0f));
	fl_line(15,axx,w()-15,axx);
	float axy=15.0f+(0.0f-minx)/(maxx-minx)*(w()-30.0f);
	fl_line(axy,15,axy,h()-80-15);

	

	fl_color(0,0,0);
	float stepx;
	stepx=((float)w()-30.0f)/(maxx-minx);
	float realx;
	realx=0;
	std::vector<float>::iterator iter;
	std::vector<float>::iterator next;
	for(iter=i->result.begin();iter!=i->result.end();iter++)
	{
		next=iter+1;
		if(next!=i->result.end())
		{
			fl_color(0,0,0);
			fl_line(realx+15,h()-80-(15+((*iter)-miny)/(maxy-miny)*(h()-80-30)),(realx+stepx)+15,h()-80-(15+((*next)-miny)/(maxy-miny)*(h()-80-30)));
			fl_color(255,0,0);
			fl_circle(realx+15,h()-80-(15+((*iter)-miny)/(maxy-miny)*(h()-80-30)),2);
		}
		realx=realx+stepx;
	}
	std::vector<KeyPoint>::iterator p;
	for(p=i->points.begin();p!=i->points.end();p++)
	{
		fl_color(0,0,255);
		fl_circle(15+((*p).time-minx)*((float)w()-30.0f)/(maxx-minx),h()-80-(15+((*p).value-miny)/(maxy-miny)*(h()-80-30)),2);
		fl_circle(15+((*p).time-minx)*((float)w()-30.0f)/(maxx-minx),h()-80-(15+((*p).value-miny)/(maxy-miny)*(h()-80-30)),3);
		fl_circle(15+((*p).time-minx)*((float)w()-30.0f)/(maxx-minx),h()-80-(15+((*p).value-miny)/(maxy-miny)*(h()-80-30)),1);
	}

	fl_color(0,100,0);
	char buffer[10];
	fl_font(FL_HELVETICA,9);
	sprintf(buffer,"%d",minx);
	fl_draw(buffer,15,axx+10);
	sprintf(buffer,"%d",maxx);
	fl_draw(buffer,w()-25,axx+10);
	if(axy<2)
		axy=2;
	sprintf(buffer,"%.2f",miny);
	fl_draw(buffer,axy-12,h()-80-16);
	sprintf(buffer,"%.2f",maxy);
	fl_draw(buffer,axy-12,15);

	delete i;

}

void DataViewerWindow::AddGraph(std::string name,float r,float g,float b,int minx,int maxx,int miny,int maxy)
{
	SingleGraph *sg;
	sg=new SingleGraph(name);
	sg->SetColor(r,g,b);
	sg->SetBoundaries(minx,maxx,miny,maxy);
	graphs.push_back(sg);
}

void DataViewerWindow::AddPointToGraph(std::string name,float x,float y)
{
	std::vector<SingleGraph*>::iterator iter;
	iter=graphs.begin();
	bool found;
	found=false;
	while((iter!=graphs.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==false)
		return;
	(*(*iter)).AddPoint(x,y);
}

void DataViewerWindow::AddKeyPointToGraph(std::string name,float x,float y)
{
	std::vector<SingleGraph*>::iterator iter;
	iter=graphs.begin();
	bool found;
	found=false;
	while((iter!=graphs.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==false)
		return;
	(*(*iter)).AddKeyPoint(x,y);
}

void DataViewerWindow::ShowPointsOfGraph(std::string name)
{
		std::vector<SingleGraph*>::iterator iter;
	iter=graphs.begin();
	bool found;
	found=false;
	while((iter!=graphs.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==false)
		return;
	(*iter)->markpoints=true;
}

void DataViewerWindow::HidePointsOfGraph(std::string name)
{
	std::vector<SingleGraph*>::iterator iter;
	iter=graphs.begin();
	bool found;
	found=false;
	while((iter!=graphs.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==false)
		return;

	(*iter)->markpoints=false;
}

void DataViewerWindow::SetGraphWidth(std::string name,int w)
{
	std::vector<SingleGraph*>::iterator iter;
	iter=graphs.begin();
	bool found;
	found=false;
	while((iter!=graphs.end())&&(found==false))
	{
		if((*iter)->name==name)
		{
			(*iter)->SetWidth(w);
			found=true;
		}
		iter++;
	}
}

void DataViewerWindow::LoadCSV(std::string name,std::string path,int x_column,int y_column,char limiter)
{
	std::vector<SingleGraph*>::iterator iter;
	iter=graphs.begin();
	bool found;
	found=false;
	while((iter!=graphs.end())&&(found==false))
	{
		if((*iter)->name==name)
			found=true;
		else
			iter++;
	}
	if(found==false)
		return;
	(*(*iter)).LoadCSV(path,x_column,y_column,limiter);
}

