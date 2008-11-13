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

#pragma once

#include <string>
#include <vector>

struct pointforsinglegraph
{
	float x;
	float y;
};

/**
*
* class :SingleGraph
*
*/

class SingleGraph
{
public:

/**
 * this method 
 * 
 *
* @return 
* @param  name
*/

	SingleGraph(std::string name);

/**
*
* destructor 
*/

	~SingleGraph(void);

/**
 * this method 
 * 
 *
* @param  x
* @param  y
*/

	void AddPoint(float x,float y);


/**
 * this method 
 * 
 *
* @param  x
* @param  y
*/

	void AddKeyPoint(float x,float y);


/**
 * this method 
 * 
 *
* @param  w
* @param  h
*/

	void draw(int w,int h);

/**
 * this method 
 * 
 *
* @param  x
* @param  y
*/

	void draw_key(int x,int y);


/**
 * this method 
 * 
 *
* @param  r
* @param  g
* @param  b
*/

	void SetColor(int r,int g,int b);


/**
 * this method 
 * 
 *
* @param  minx
* @param  maxx
* @param  miny
* @param  maxy
*/

	void SetBoundaries(float minx,float maxx,float miny,float maxy);


/**
 * this method 
 * 
 *
* @param  w
*/

	void SetWidth(int w);

	std::string name;
	int color_r,color_g,color_b;
	std::vector<pointforsinglegraph> points;
	
	std::vector<pointforsinglegraph> keypoints;

	float minx,maxx,miny,maxy;

	bool continuosline;
	bool markpoints;

	int width;
	int parentwidth,parentheight;


/**
 * this method 
 * 
 *
* @param  path
* @param  x_column
* @param  y_column
* @param  limiter
*/

	void LoadCSV(std::string path,int x_column,int y_column,char limiter);
};
