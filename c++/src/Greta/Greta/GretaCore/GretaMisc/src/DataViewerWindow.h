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

// DataViewerWindow.h: interface for the DataViewerWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DataViewerWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
#define AFX_DataViewerWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <FL/Fl_Window.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Slider.h>
//#include "FaceCosineInterpolator.h"
//#include "FaceLinearInterpolator.h"
#include "TCBInterpolator.h"
#include "WildMagic/Wm4TCBSpline2.h"
//#include "ADSRGenerator.h"
#include ".\singlegraph.h"

struct pointfordataviewer
{
	float x;
	float y;
};

/**
*
* class :DataViewerWindow
*
*/

class DataViewerWindow : public Fl_Window  
{
public:

	/**
	*
	* contructor 
	*
	*/

	DataViewerWindow();

	/**
	*
	* destructor 
	*/

	virtual ~DataViewerWindow();

	/**
	* this method 
	* 
	*
	* @return 
	* @param  e
	*/

	int handle(int e);

	/**
	* this method 
	* 
	*
	*/

	void draw();

	/**
	* this method 
	* 
	*
	*/

	void drawtcb();

	/**
	* this method 
	* 
	*
	* @param  name
	* @param  r
	* @param  g
	* @param  b
	* @param  minx
	* @param  maxx
	* @param  miny
	* @param  maxy
	*/

	void AddGraph(std::string name,float r,float g,float b,int minx,int maxx,int miny,int maxy);

	/**
	* this method 
	* 
	*
	* @param  name
	* @param  x
	* @param  y
	*/

	void AddPointToGraph(std::string name,float x,float y);

	/**
	* this method 
	* 
	*
	* @param  name
	* @param  x
	* @param  y
	*/

	void AddKeyPointToGraph(std::string name,float x,float y);

	/**
	* this method 
	* 
	*
	* @param  x
	* @param  y
	*/

	void AddSpecialPoint(float x,float y);

	/**
	* this method 
	* 
	*
	* @param  name
	* @param  path
	* @param  x_column
	* @param  y_column
	* @param  limiter
	*/

	void LoadCSV(std::string name,std::string path,int x_column,int y_column,char limiter);

	/**
	* this method 
	* 
	*
	* @param  name
	*/

	void ShowPointsOfGraph(std::string name);

	/**
	* this method 
	* 
	*
	* @param  name
	*/

	void HidePointsOfGraph(std::string name);

	/**
	* this method 
	* 
	*
	* @param  name
	* @param  w
	*/

	void SetGraphWidth(std::string name,int w);
	//ADSRGenerator *adsr;
	TCBInterpolator *i;
	Wm4::TCBSpline2f *mpTCB2_Swivel;
	Fl_Slider *Tslider;
	Fl_Output *Toutput;
	Fl_Slider *Cslider;
	Fl_Output *Coutput;
	Fl_Slider *Bslider;
	Fl_Output *Boutput;

	float minx,miny,maxx,maxy;
	std::vector<pointfordataviewer> points1;
	std::vector<pointfordataviewer> points2;
	std::vector<pointfordataviewer> points3;
	std::vector<pointfordataviewer> points4;
	std::vector<pointfordataviewer> points5;
	std::vector<pointfordataviewer> points6;
	std::vector<pointfordataviewer> specialpoints;

	std::vector<SingleGraph*> graphs;

	bool contline;

	float line1,line2;

};

#endif // !defined(AFX_DataViewerWindow_H__FE981889_170B_11D9_9C92_FEB87D89E47F__INCLUDED_)
