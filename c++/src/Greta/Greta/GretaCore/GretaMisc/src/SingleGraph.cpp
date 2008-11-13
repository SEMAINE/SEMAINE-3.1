#include ".\singlegraph.h"
#include "FL/Fl_draw.H"

#include <stdio.h>
#include <iostream>
#include <fstream>

SingleGraph::SingleGraph(std::string name)
{
	this->name=name;
	continuosline=true;
	markpoints=true;
	color_r=0;
	color_g=0;
	color_b=0;
	minx=0;
	maxx=0;
	miny=0;
	maxy=0;
	width=1;
	points.clear();
}

SingleGraph::~SingleGraph(void)
{
}

void SingleGraph::AddPoint(float x,float y)
{
	pointforsinglegraph *p;
	p=new pointforsinglegraph;
	p->x=x;
	p->y=y;
	points.push_back(*p);
}

void SingleGraph::AddKeyPoint(float x,float y)
{
	pointforsinglegraph *p;
	p=new pointforsinglegraph;
	p->x=x;
	p->y=y;
	keypoints.push_back(*p);
}

void SingleGraph::SetColor(int r,int g,int b)
{
	color_r=r;
	color_g=g;
	color_b=b;
}

void SingleGraph::SetBoundaries(float minx,float maxx,float miny,float maxy)
{
	this->minx=minx;
	this->maxx=maxx;
	this->miny=miny;
	this->maxy=maxy;
}

void SingleGraph::draw(int w,int h)
{
	parentwidth=w;
	parentheight=h;

	if((minx==0)&&(maxx==0)&&(miny==0)&&(maxy==0))
		return;

	std::vector<pointforsinglegraph>::iterator iter;

	
	
	iter=points.begin();
	if(iter!=points.end())
		iter++;
	while(iter!=points.end())
	{
		fl_color(this->color_r,this->color_g,this->color_b);
	
		if(continuosline==false)
			fl_line_style(FL_DASH,width,0);
		else
			fl_line_style(FL_SOLID,width,0);

		//printf("line from %.2f,%.2f to %.2f,%.2f\n",(*(iter-1)).x,(*(iter-1)).y,(*iter).x,(*iter).y);
		float p1y=h-80-(15.0f+((*iter).y-miny)/(maxy-miny)*(h-80-30.0f));
		float p1x=15.0f+((*iter).x-minx)/(maxx-minx)*(w-30.0f);
		float p2y=h-80-(15.0f+((*(iter-1)).y-miny)/(maxy-miny)*(h-80-30.0f));
		float p2x=15.0f+((*(iter-1)).x-minx)/(maxx-minx)*(w-30.0f);
	
		fl_line(p1x,p1y,p2x,p2y);

		if(markpoints==true)
		{
			char value[255];
			fl_circle(15+((*iter).x-minx)*((float)w-30.0f)/(maxx-minx),h-80-(15+((*iter).y-miny)/(maxy-miny)*(h-80-30)),3);
			fl_circle(15+((*(iter-1)).x-minx)*((float)w-30.0f)/(maxx-minx),h-80-(15+((*(iter-1)).y-miny)/(maxy-miny)*(h-80-30)),3);
			
			fl_line_style(FL_DASH,1,0);

			fl_color(180,180,180);

			//fl_line(p1x,15,p1x,h-95);
			//fl_line(p2x,15,p2x,h-95);

			fl_font(FL_HELVETICA,9);

			sprintf(value,"%.2f",(*(iter)).x);
			//fl_draw(value,p1x+1,h/2-15);

			sprintf(value,"%.2f",(*(iter-1)).x);
			//fl_draw(value,p2x+1,h/2-15);
		}

		iter++;
	}

	iter=keypoints.begin();
	if(iter!=keypoints.end())
		iter++;
	fl_color(255,0,0);
	fl_line_style(FL_SOLID,width+2,0);
	while(iter!=keypoints.end())
	{
		//printf("line from %.2f,%.2f to %.2f,%.2f\n",(*(iter-1)).x,(*(iter-1)).y,(*iter).x,(*iter).y);
		float p1y=h-80-(15.0f+((*iter).y-miny)/(maxy-miny)*(h-80-30.0f));
		float p1x=15.0f+((*iter).x-minx)/(maxx-minx)*(w-30.0f);
		float p2y=h-80-(15.0f+((*(iter-1)).y-miny)/(maxy-miny)*(h-80-30.0f));
		float p2x=15.0f+((*(iter-1)).x-minx)/(maxx-minx)*(w-30.0f);
	
		fl_line(p1x,p1y,p2x,p2y);

		if(markpoints==true)
		{
			fl_circle(15+((*iter).x-minx)*((float)w-30.0f)/(maxx-minx),h-80-(15+((*iter).y-miny)/(maxy-miny)*(h-80-30)),3);
		}

		iter++;
	}
}

void SingleGraph::draw_key(int x,int y)
{
	fl_font(FL_HELVETICA,9);
	fl_color(this->color_r,this->color_g,this->color_b);
	if(points.empty())
		return;
	fl_line(x,y+parentheight-80,x+10,y+parentheight-80);
	fl_draw(this->name.c_str(),x+12,y+parentheight-80);
}

void SingleGraph::SetWidth(int w)
{
	width=w;
}

void SingleGraph::LoadCSV(std::string path,int x_column,int y_column,char limiter)
{
	std::string line;

	std::ifstream inputfile(path.c_str());

	std::vector<std::string> tokens;
	std::string token;

	points.clear();

	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			if(line!="")
			{
				tokens.clear();
				while(line!="")
				{
					if(line.find_first_of(limiter)==std::string::npos)
					{
						token=line;
						line="";
					}
					else
					{
						token=line.substr(0,line.find_first_of(limiter));
						line=line.substr(line.find_first_of(limiter)+1);
					}

					tokens.push_back(token);
				}
				float x,y;
				x=atof(tokens[x_column].c_str());
				y=atof(tokens[y_column].c_str());
				this->AddPoint(x,y);
			}
		}
	}
	else
		return ;
	inputfile.close();
}