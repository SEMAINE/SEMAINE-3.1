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
//Foundation, Inc., 59 Temple Place, Suite 330index, Boston, MA  02111-1307  USA

// FaceLibraryViewerWindow.cpp: implementation of the FaceLibraryViewerWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceLibraryViewerWindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include "IniManager.h"

extern IniManager inimanager;

/*
extern "C" int** get_fap_id(char*,float);
extern "C" int get_fap_id_count(char *id);
extern "C" int** get_fap_id_n(char *id,int n);
extern "C" float get_fap_id_prob(char *id,int n);
extern fap_id_list *dictionary;
*/

extern FILE *face_log;

void selectedmenu(Fl_Widget* w, void* v)
{
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"about")==0)
	{
		((FaceLibraryViewerWindow*)v)->about=new AboutWindow();

		((FaceLibraryViewerWindow*)v)->about->set_modal();
		
		((FaceLibraryViewerWindow*)v)->about->show();
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Reload face library")==0)
	{
		((FaceLibraryViewerWindow*)v)->LoadLibrary();
		((FaceLibraryViewerWindow*)v)->LoadFaceFrame(0);
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Save for face library (XML)")==0)
	{
		((FaceLibraryViewerWindow*)v)->SaveFAPFrame();
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Save FAP File")==0)
	{
		((FaceLibraryViewerWindow*)v)->SaveFAPFile();
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Save geometry")==0)
	{
		((FaceLibraryViewerWindow*)v)->SaveGeometry();
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Export VRML")==0)
	{
		((FaceLibraryViewerWindow*)v)->SaveGeometryVRML();
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Show FAPs")==0)
	{
		((FaceLibraryViewerWindow*)v)->fi = new FAPsImage();
		//((FaceLibraryViewerWindow*)v)->fi->set_modal();
		((FaceLibraryViewerWindow*)v)->fi->show();
		while (((FaceLibraryViewerWindow*)v)->fi->shown())
			Fl::wait();
	}
	if(strcmp(((FaceLibraryViewerWindow*)v)->menu->text(),"Quit")==0)
	{
		((FaceLibraryViewerWindow*)v)->hide();
		delete ((FaceLibraryViewerWindow*)v);
	}
}

void resetCB(Fl_Widget*w, void* v)
{
	int i;
	
	for(i=3;i<65;i++)
	{
		if(i!=27 && i!=28 && i!=29 && i!=30)
		{
			((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(0);
			((FaceLibraryViewerWindow*)v)->lframe[i]->value("0.00");
		}
	}
	for(i=0;i<6;i++)
	{
		((FaceLibraryViewerWindow*)v)->facscounter[i]->value(0);
		((FaceLibraryViewerWindow*)v)->lfacs[i]->value("0.00");
	}
	
	for(i=0;i<8;i++)
	{
		((FaceLibraryViewerWindow*)v)->lipscounter[i]->value(0);
		((FaceLibraryViewerWindow*)v)->llips[i]->value("0.00");
	}

	for(i=0;i<6;i++)
	{
		((FaceLibraryViewerWindow*)v)->AU1[i]=0;
		((FaceLibraryViewerWindow*)v)->AU2[i]=0;
		((FaceLibraryViewerWindow*)v)->AU4[i]=0;

		((FaceLibraryViewerWindow*)v)->ULO[i]=0;
		((FaceLibraryViewerWindow*)v)->LLO[i]=0;
		((FaceLibraryViewerWindow*)v)->LSW[i]=0;
	}
	for(i=0;i<2;i++)
	{
		((FaceLibraryViewerWindow*)v)->AU5[i]=0;
		((FaceLibraryViewerWindow*)v)->AU6[i]=0;
		((FaceLibraryViewerWindow*)v)->ULP[i]=0;
		((FaceLibraryViewerWindow*)v)->CRR[i]=0;
		((FaceLibraryViewerWindow*)v)->CRL[i]=0;
	}
	for(i=0;i<4;i++)
	{
		((FaceLibraryViewerWindow*)v)->AU7[i]=0;
		((FaceLibraryViewerWindow*)v)->JAW[i]=0;
	}
	
	((FaceLibraryViewerWindow*)v)->LLP=0;

	((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
	((FaceLibraryViewerWindow*)v)->instances->value(0);
	((FaceLibraryViewerWindow*)v)->probability->value(0);
	((FaceLibraryViewerWindow*)v)->framenumber->value(0);
	
	//((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
}



void selectedclass(Fl_Widget*w, void* v)
{
	// printf("scelto %d\n",((FaceLibraryViewerWindow*)v)->classes->value());
	resetCB(w,v);
	
	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(0);
	((FaceLibraryViewerWindow*)v)->LoadInstances();
}

void selectedinstance(Fl_Widget*w, void* v)
{
	//((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
//	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->framenumber->value());
	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(0);
	((FaceLibraryViewerWindow*)v)->UpdateFAPValues(((FaceLibraryViewerWindow*)v)->GetSelectedFAPFrame(0));
}

void changedMOUTHgroup(Fl_Widget*w, void* v)
{	
	int i;
	if(((FaceLibraryViewerWindow*)v)->mouth->value()==1)
	{
		((FaceLibraryViewerWindow*)v)->eyes_eyebrows->value(0);
		((FaceLibraryViewerWindow*)v)->head->value(0);
		((FaceLibraryViewerWindow*)v)->cheek->value(0);
		for(i=3; i<65; i++)
		{
			if((i>=4 && i<18) || (i>=51 && i<61))
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->show();
				((FaceLibraryViewerWindow*)v)->lframe[i]->show();
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->activate();
				((FaceLibraryViewerWindow*)v)->lframe[i]->activate();
			}
			else
			{
				if(i!=27 && i!=28 && i!=29 && i!=30)
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->hide();
					((FaceLibraryViewerWindow*)v)->lframe[i]->hide();
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->deactivate();
					((FaceLibraryViewerWindow*)v)->lframe[i]->deactivate();
				}
			}
		}
	}
	else
		((FaceLibraryViewerWindow*)v)->mouth->value(1);
	((FaceLibraryViewerWindow*)v)->redraw();

	//if (((FaceLibraryViewerWindow*)v)->IndexB!=0)
	//	((FaceLibraryViewerWindow*)v)->fapcounter[((FaceLibraryViewerWindow*)v)->IndexB]->value(atof(((FaceLibraryViewerWindow*)v)->lframe[((FaceLibraryViewerWindow*)v)->IndexB]->value()));
}

void changedEYESgroup(Fl_Widget*w, void* v)
{	
	int i;
	if(((FaceLibraryViewerWindow*)v)->eyes_eyebrows->value()==1)
	{
		((FaceLibraryViewerWindow*)v)->mouth->value(0);
		((FaceLibraryViewerWindow*)v)->head->value(0);
		((FaceLibraryViewerWindow*)v)->cheek->value(0);
		for(i=3; i<65; i++)
		{
			if((i>=19 && i<27) || (i>=31 && i<39))
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->show();
				((FaceLibraryViewerWindow*)v)->lframe[i]->show();
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->activate();
				((FaceLibraryViewerWindow*)v)->lframe[i]->activate();
			}
			else
			{
				if(i!=27 && i!=28 && i!=29 && i!=30)
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->hide();
					((FaceLibraryViewerWindow*)v)->lframe[i]->hide();
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->deactivate();
					((FaceLibraryViewerWindow*)v)->lframe[i]->deactivate();
				}
			}
		}
	}
	else
		((FaceLibraryViewerWindow*)v)->eyes_eyebrows->value(1);
	((FaceLibraryViewerWindow*)v)->redraw();

	//if (((FaceLibraryViewerWindow*)v)->IndexB!=0)
	//	((FaceLibraryViewerWindow*)v)->fapcounter[((FaceLibraryViewerWindow*)v)->IndexB]->value(atof(((FaceLibraryViewerWindow*)v)->lframe[((FaceLibraryViewerWindow*)v)->IndexB]->value()));
}

void changedHEADgroup(Fl_Widget*w, void* v)
{	
	int i;
	if(((FaceLibraryViewerWindow*)v)->head->value()==1)
	{
		((FaceLibraryViewerWindow*)v)->eyes_eyebrows->value(0);
		((FaceLibraryViewerWindow*)v)->mouth->value(0);
		((FaceLibraryViewerWindow*)v)->cheek->value(0);
		for(i=3; i<65; i++)
		{
			if((i>=48 && i<51))
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->show();
				((FaceLibraryViewerWindow*)v)->lframe[i]->show();
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->activate();
				((FaceLibraryViewerWindow*)v)->lframe[i]->activate();
			}
			else
			{
				if(i!=27 && i!=28 && i!=29 && i!=30)
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->hide();
					((FaceLibraryViewerWindow*)v)->lframe[i]->hide();
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->deactivate();
					((FaceLibraryViewerWindow*)v)->lframe[i]->deactivate();
				}
			}
		}
	}
	else
		((FaceLibraryViewerWindow*)v)->head->value(1);
	((FaceLibraryViewerWindow*)v)->redraw();
	//if (((FaceLibraryViewerWindow*)v)->IndexB!=0)
	//	((FaceLibraryViewerWindow*)v)->fapcounter[((FaceLibraryViewerWindow*)v)->IndexB]->value(atof(((FaceLibraryViewerWindow*)v)->lframe[((FaceLibraryViewerWindow*)v)->IndexB]->value()));
}

void changedCHEEKgroup(Fl_Widget*w, void* v)
{	
	int i;
	if(((FaceLibraryViewerWindow*)v)->cheek->value()==1)
	{
		((FaceLibraryViewerWindow*)v)->eyes_eyebrows->value(0);
		((FaceLibraryViewerWindow*)v)->mouth->value(0);
		((FaceLibraryViewerWindow*)v)->head->value(0);
		for(i=3; i<65; i++)
		{
			if((i==3) || (i==18) || (i>=39 && i<48) || (i>=61 && i<65))
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->show();
				((FaceLibraryViewerWindow*)v)->lframe[i]->show();
				((FaceLibraryViewerWindow*)v)->fapcounter[i]->activate();
				((FaceLibraryViewerWindow*)v)->lframe[i]->activate();
			}
			else
			{
				if(i!=27 && i!=28 && i!=29 && i!=30)
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->hide();
					((FaceLibraryViewerWindow*)v)->lframe[i]->hide();
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->deactivate();
					((FaceLibraryViewerWindow*)v)->lframe[i]->deactivate();
				}
			}
		}
	}
	else
		((FaceLibraryViewerWindow*)v)->cheek->value(1);
	((FaceLibraryViewerWindow*)v)->redraw();
	//if (((FaceLibraryViewerWindow*)v)->IndexB!=0)
	//	((FaceLibraryViewerWindow*)v)->fapcounter[((FaceLibraryViewerWindow*)v)->IndexB]->value(atof(((FaceLibraryViewerWindow*)v)->lframe[((FaceLibraryViewerWindow*)v)->IndexB]->value()));
}

void changedFAPvalue(Fl_Widget*w, void* v)
{
	char value[255];
	
	if (((FaceLibraryViewerWindow*)v)->Index!=0)
	{
		sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[((FaceLibraryViewerWindow*)v)->Index]->value());
		((FaceLibraryViewerWindow*)v)->lframe[((FaceLibraryViewerWindow*)v)->Index]->value(value);
	}

	((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
	//((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->framenumber->value());
}

void changeValueFAP(Fl_Widget*w, void* v)
{	
	if (((FaceLibraryViewerWindow*)v)->IndexB!=0)
		((FaceLibraryViewerWindow*)v)->fapcounter[((FaceLibraryViewerWindow*)v)->IndexB]->value(atof(((FaceLibraryViewerWindow*)v)->lframe[((FaceLibraryViewerWindow*)v)->IndexB]->value()));

	((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
	//((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->framenumber->value());
}

void changedFACSvalue(Fl_Widget*w, void* v)
{
	int i;
	char value[255];
	float coef;
	
	if (((FaceLibraryViewerWindow*)v)->IndexFACS!=-1)
	{
		sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value());
		((FaceLibraryViewerWindow*)v)->lfacs[((FaceLibraryViewerWindow*)v)->IndexFACS]->value(value);
	}
	
	switch(((FaceLibraryViewerWindow*)v)->IndexFACS)
	{
	case 0:   //FACS AU1
		for (i=31; i<37; i++)
		{
			if (i==31 || i==32) coef=1;
			if (i==33 || i==34) coef=0.5;
			if (i==35 || i==36) coef=0.1;

			if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[i][0] && ((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[i][1])
			{
				if (i==31 || i==32 || i==33 || i==34)
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+((FaceLibraryViewerWindow*)v)->AU4[i-31]+(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef));
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					((FaceLibraryViewerWindow*)v)->AU1[i-31]=atof(value);
				}
				else
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->value((((FaceLibraryViewerWindow*)v)->AU2[i-31])+(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef));
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					((FaceLibraryViewerWindow*)v)->AU1[i-31]=atof(value);
				}
			}
			else
				if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>((FaceLibraryViewerWindow*)v)->bounds[i][0])
				{
					if (i==31 || i==32 || i==33 || i==34)
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+((FaceLibraryViewerWindow*)v)->AU4[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef));
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef);
						((FaceLibraryViewerWindow*)v)->AU1[i-31]=atof(value);
					}
					else
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef));
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef);
						((FaceLibraryViewerWindow*)v)->AU1[i-31]=atof(value);
					}
				}
				else
					if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<((FaceLibraryViewerWindow*)v)->bounds[i][1])
					{
						if (i==31 || i==32 || i==33 || i==34)
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+((FaceLibraryViewerWindow*)v)->AU4[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef));
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							((FaceLibraryViewerWindow*)v)->AU1[i-31]=atof(value);
						}
						else
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef));
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							((FaceLibraryViewerWindow*)v)->AU1[i-31]=atof(value);
						}
					}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
			((FaceLibraryViewerWindow*)v)->lframe[i]->value(value);
		}
		break;
	case 1:   //FACS AU2
		for (i=31; i<37; i++)
		{
			if (i==31 || i==32) coef=0.2;
			if (i==33 || i==34) coef=1;
			if (i==35 || i==36) coef=0.7;

			if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[i][0] && ((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[i][1])
			{
				if (i==31 || i==32 || i==33 || i==34)
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU1[i-31]+((FaceLibraryViewerWindow*)v)->AU4[i-31]+(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef));
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					((FaceLibraryViewerWindow*)v)->AU2[i-31]=atof(value);
				}
				else
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->value((((FaceLibraryViewerWindow*)v)->AU1[i-31])+(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef));
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					((FaceLibraryViewerWindow*)v)->AU2[i-31]=atof(value);
				}
			}
			else
				if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>((FaceLibraryViewerWindow*)v)->bounds[i][0])
				{
					if (i==31 || i==32 || i==33 || i==34)
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU1[i-31]+((FaceLibraryViewerWindow*)v)->AU4[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef));
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef);
						((FaceLibraryViewerWindow*)v)->AU2[i-31]=atof(value);
					}
					else
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU1[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef));
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef);
						((FaceLibraryViewerWindow*)v)->AU2[i-31]=atof(value);
					}
				}
				else
					if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<((FaceLibraryViewerWindow*)v)->bounds[i][1])
					{
						if (i==31 || i==32 || i==33 || i==34)
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU1[i-31]+((FaceLibraryViewerWindow*)v)->AU4[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef));
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							((FaceLibraryViewerWindow*)v)->AU2[i-31]=atof(value);
						}
						else
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU1[i-31]+(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef));
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							((FaceLibraryViewerWindow*)v)->AU2[i-31]=atof(value);
						}
					}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
			((FaceLibraryViewerWindow*)v)->lframe[i]->value(value);
		}
		break;
		case 2:   //FACS AU4
			for (i=31; i<39; i++)
			{
				if (i==31 || i==32 || i==33 || i==34 || i==37 || i==38)
				{
					if (i==31 || i==32) coef=1.3;
					if (i==33 || i==34) coef=1.3;
					if (i==37 || i==38) coef=1;

					if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[i][0] && ((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[i][1])
					{
						if (i==31 || i==32 || i==33 || i==34)
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+((FaceLibraryViewerWindow*)v)->AU1[i-31]-((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
							((FaceLibraryViewerWindow*)v)->AU4[i-31]=-atof(value);
						}
						else
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
							((FaceLibraryViewerWindow*)v)->AU4[i-33]=atof(value);
						}
					}
					else
						if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>((FaceLibraryViewerWindow*)v)->bounds[i][1])
						{
							if (i==31 || i==32 || i==33 || i==34)
							{
								((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU2[i-31]+((FaceLibraryViewerWindow*)v)->AU1[i-31]-((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef);
								sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][1]*coef);
								((FaceLibraryViewerWindow*)v)->AU4[i-31]=-atof(value);
							}
							else
							{
								((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
								sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
								((FaceLibraryViewerWindow*)v)->AU4[i-33]=atof(value);
							}
						}
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
					((FaceLibraryViewerWindow*)v)->lframe[i]->value(value);
				}
			}
		break;
		case 3: //FACS AU5
			for (i=19; i<21; i++)
			{
				coef=1;

				if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[i][0] && ((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[i][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU7[i-19]-((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					((FaceLibraryViewerWindow*)v)->AU5[i-19]=-atof(value);
				}
				else
					if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>((FaceLibraryViewerWindow*)v)->bounds[i][1])
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU7[i-19]-((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
						((FaceLibraryViewerWindow*)v)->AU5[i-19]=-atof(value);
					}
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
				((FaceLibraryViewerWindow*)v)->lframe[i]->value(value);
			}
		break;
		case 4: //FACS AU6
			for (i=41; i<43; i++)
			{
				coef=1;

				if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[i][0] && ((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[i][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
					((FaceLibraryViewerWindow*)v)->AU6[i-41]=atof(value);
				}
				else
					if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>((FaceLibraryViewerWindow*)v)->bounds[i][1])
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
						((FaceLibraryViewerWindow*)v)->AU6[i-42]=atof(value);
					}
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
				((FaceLibraryViewerWindow*)v)->lframe[i]->value(value);
			}
		break;
		case 5: //FACS AU7
			for (i=19; i<23; i++)
			{
				coef=1;

				if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[i][0] && ((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[i][1])
				{
					if (i==19 || i==20)
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU5[i-19]+((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
						((FaceLibraryViewerWindow*)v)->AU7[i-19]=atof(value);
					}
					else
					{
						((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
						sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()*coef);
						((FaceLibraryViewerWindow*)v)->AU7[i-21]=atof(value);
					}
				}
				else
					if (((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACS]->value()>((FaceLibraryViewerWindow*)v)->bounds[i][1])
					{
						if (i==19 || i==20)
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->AU5[i-19]+((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							((FaceLibraryViewerWindow*)v)->AU7[i-19]=atof(value);
						}
						else
						{
							((FaceLibraryViewerWindow*)v)->fapcounter[i]->value(((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[i][0]*coef);
							((FaceLibraryViewerWindow*)v)->AU7[i-21]=atof(value);
						}
					}
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[i]->value());
				((FaceLibraryViewerWindow*)v)->lframe[i]->value(value);
			}
		break;
	default:
		break;
	}

	((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->framenumber->value());
}


//LIP PARAMETERS

void changedLipParametersvalue(Fl_Widget*w, void* v)
{
	int i, ind;
	char value[255];
	float coef=1.0f;
	
	if (((FaceLibraryViewerWindow*)v)->IndexLIPS!=-1)
	{
		sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
		((FaceLibraryViewerWindow*)v)->llips[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value(value);
	}
	
	switch(((FaceLibraryViewerWindow*)v)->IndexLIPS)
	{
	case 0:   //LIP PARAMETER ÜPPER LIPS OPENING: ULO 4 8 9 51 55 56
		for (i=0; i<6; i++)
		{
			switch(i)
			{
				case 0: ind=4; coef=-2.5f; break;
				case 1: ind=8; coef=-1.4f; break;
				case 2: ind=9; coef=-1.4f; break;
				case 3: ind=51; coef=-0.6f; break;
				case 4: ind=55; coef=-0.3f; break;
				case 5: ind=56; coef=-0.3f; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()*coef);
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->ULO[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]*coef);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->ULO[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	case 1:   //LIP PARAMETER LOWER LIPS OPENING: LLO 5 10 11 52 57 58
		for (i=0; i<6; i++)
		{
			switch(i)
			{
				case 0: ind=5; coef=-0.9f; break;
				case 1: ind=10; coef=-1.0f; break;
				case 2: ind=11; coef=-1.0f; break;
				case 3: ind=52; coef=-0.8f; break;
				case 4: ind=57; coef=-0.7f; break;
				case 5: ind=58; coef=-0.7f; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()*coef);
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->LLO[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]*coef);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->LLO[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	case 2:   //LIP PARAMETER JAW: JAW 3 18 41 42
		for (i=0; i<4; i++)
		{
			switch(i)
			{
				case 0: ind=3; break;
				case 1: ind=18; break;
				case 2: ind=41; break;
				case 3: ind=42; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->JAW[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->JAW[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	case 3: //LIP PARAMETER LIP WIDTH: LSW 6 7 53 54 61 62
		for (i=0; i<6; i++)
		{
			switch(i)
			{
				case 0: ind=6; coef=1.0f; break;
				case 1: ind=7; coef=1.0f; break;
				case 2: ind=53; coef=1.0f;  break;
				case 3: ind=54; coef=1.0f; break;
				case 4: ind=61; coef=0.2f; break;
				case 5: ind=62; coef=0.2f; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()*coef);
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->LSW[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]*coef);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->LSW[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	case 4: //LIP PARAMETER UPPER LIP PROTRUSION: ULP 17 63
		for (i=0; i<2; i++)
		{
			switch(i)
			{
				case 0: ind=17; coef=1.0f; break;
				case 1: ind=63; coef=-0.1f; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()*coef);
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->ULP[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]*coef);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->ULP[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	case 5: //LIP PARAMETER LOWER LIP PROTRUSION: LLP 16
		ind=16;

		if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
		{
			((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
			((FaceLibraryViewerWindow*)v)->LLP=atof(value);
		}
		else
			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
				((FaceLibraryViewerWindow*)v)->LLP=atof(value);
			}
		sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
		((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		break;
	case 6: //LIP PARAMETER CORNER RIGHT: CRR 13 60
		for (i=0; i<2; i++)
		{
			switch(i)
			{
				case 0: ind=13; break;
				case 1: ind=60; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->CRR[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->CRR[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	case 7:   //LIP PARAMETER CORNER LEFT: CRL 12 59
		for (i=0; i<2; i++)
		{
			switch(i)
			{
				case 0: ind=12; break;
				case 1: ind=59; break;
				default: break;
			}

			if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>=((FaceLibraryViewerWindow*)v)->bounds[ind][0] && ((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()<=((FaceLibraryViewerWindow*)v)->bounds[ind][1])
			{
				((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value());
				((FaceLibraryViewerWindow*)v)->CRL[i]=atof(value);
			}
			else
				if (((FaceLibraryViewerWindow*)v)->lipscounter[((FaceLibraryViewerWindow*)v)->IndexLIPS]->value()>((FaceLibraryViewerWindow*)v)->bounds[ind][1])
				{
					((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value(((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->bounds[ind][1]);
					((FaceLibraryViewerWindow*)v)->CRL[i]=atof(value);
				}
			sprintf(value,"%.2f",((FaceLibraryViewerWindow*)v)->fapcounter[ind]->value());
			((FaceLibraryViewerWindow*)v)->lframe[ind]->value(value);
		}
		break;
	default:
		break;
	}

	((FaceLibraryViewerWindow*)v)->CopyFAPValuesToFAPFrame();
	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->framenumber->value());
}
/*
void changeValueFACS(Fl_Widget*w, void* v)
{	
	if (((FaceLibraryViewerWindow*)v)->IndexFACSB!=-1)
		((FaceLibraryViewerWindow*)v)->facscounter[((FaceLibraryViewerWindow*)v)->IndexFACSB]->value(atof(((FaceLibraryViewerWindow*)v)->lfacs[((FaceLibraryViewerWindow*)v)->IndexFACSB]->value()));	
}
*/

void changedframenumber(Fl_Widget*w, void* v)
{
	((FaceLibraryViewerWindow*)v)->LoadFaceFrame(((FaceLibraryViewerWindow*)v)->framenumber->value());
	((FaceLibraryViewerWindow*)v)->UpdateFAPValues(((FaceLibraryViewerWindow*)v)->GetSelectedFAPFrame(((FaceLibraryViewerWindow*)v)->framenumber->value()));
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FaceLibraryViewerWindow::FaceLibraryViewerWindow():Fl_Window(10,20,905,690,"agent score 0.1")
{
	int i;

	this->size_range(375,250,905,690,1,1,0);

	//inimanager.ReadIniFile("greta.ini");
	face_log=fopen("logs/face_dictionary_log.txt","w");

	menu=new FaceLibraryViewerMenu(0,0,100,20);

	menu->add("File/Reload face library",0,(Fl_Callback *)selectedmenu,this,0);
	menu->add("File/Save for face library (XML)",0,(Fl_Callback *)selectedmenu, this, 0);
	menu->add("File/Save FAP File",0,(Fl_Callback *)selectedmenu, this, 0);
	menu->add("File/Save geometry",0,(Fl_Callback *)selectedmenu, this, 0);
	menu->add("File/Export VRML",0,(Fl_Callback *)selectedmenu, this, FL_MENU_DIVIDER);
	menu->add("File/Quit",0,(Fl_Callback *)selectedmenu,this,0);
	menu->add("Help/Show FAPs",0,(Fl_Callback *)selectedmenu,this,0);
	menu->add("Help/about",0,(Fl_Callback *)selectedmenu,this,0);
	menu->box(FL_NO_BOX);

	boxFAPS = new Fl_Box(FL_BORDER_BOX, 10, 100, 390, 340,"FAPS");
	boxFAPS->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	bounds[3][0]=0; bounds[3][1]=500;
	bounds[4][0]=-500; bounds[4][1]=70;
	bounds[5][0]=-2000; bounds[5][1]=1800;
	bounds[6][0]=-1000; bounds[6][1]=1000;
	bounds[7][0]=-1000; bounds[7][1]=1000;
	bounds[8][0]=-1800; bounds[8][1]=1800;
	bounds[9][0]=-1800; bounds[9][1]=1800;
	bounds[10][0]=-1800; bounds[10][1]=1000;
	bounds[11][0]=-1800; bounds[11][1]=1000;
	bounds[12][0]=-10000; bounds[12][1]=10000;
	bounds[13][0]=-10000; bounds[13][1]=10000;
	bounds[14][0]=-100; bounds[14][1]=250;
	bounds[15][0]=-500; bounds[15][1]=500;
	bounds[16][0]=-400; bounds[16][1]=500;
	bounds[17][0]=-420; bounds[17][1]=420;
	bounds[18][0]=-250; bounds[18][1]=250;
	bounds[19][0]=-700; bounds[19][1]=1000;
	bounds[20][0]=-700; bounds[20][1]=1000;
	bounds[21][0]=-700; bounds[21][1]=1000;
	bounds[22][0]=-700; bounds[22][1]=1000;
	bounds[23][0]=-60000; bounds[23][1]=60000;
	bounds[24][0]=-60000; bounds[24][1]=60000;
	bounds[25][0]=-60000; bounds[25][1]=60000;
	bounds[26][0]=-60000; bounds[26][1]=60000;
	bounds[27][0]=-1420; bounds[27][1]=1420;
	bounds[28][0]=-1420; bounds[28][1]=1420;
	bounds[29][0]=-1420; bounds[29][1]=1420;
	bounds[30][0]=-1900; bounds[30][1]=1900;    
	bounds[31][0]=-450; bounds[31][1]=450;
	bounds[32][0]=-450; bounds[32][1]=450;
	bounds[33][0]=-350; bounds[33][1]=600;
	bounds[34][0]=-350; bounds[34][1]=600;
	bounds[35][0]=-600; bounds[35][1]=500;
	bounds[36][0]=-600; bounds[36][1]=500;
	bounds[37][0]=-100; bounds[37][1]=200;
	bounds[38][0]=-100; bounds[38][1]=200;
	bounds[39][0]=-200; bounds[39][1]=200;
	bounds[40][0]=-200; bounds[40][1]=200;
	bounds[41][0]=-400; bounds[41][1]=400;
	bounds[42][0]=-400; bounds[42][1]=400;
	bounds[43][0]=-800; bounds[43][1]=800;
	bounds[44][0]=-500; bounds[44][1]=500;
	bounds[45][0]=-800; bounds[45][1]=800;
	bounds[46][0]=-60; bounds[46][1]=60;
	bounds[47][0]=-60; bounds[47][1]=60;
	bounds[48][0]=-60000; bounds[48][1]=60000;
	bounds[49][0]=-60000; bounds[49][1]=60000;
	bounds[50][0]=-60000; bounds[50][1]=60000;
	bounds[51][0]=-600; bounds[51][1]=600;
	bounds[52][0]=-700; bounds[52][1]=500;
	bounds[53][0]=-100; bounds[53][1]=400;
	bounds[54][0]=-100; bounds[54][1]=400;
	bounds[55][0]=-300; bounds[55][1]=300;
	bounds[56][0]=-300; bounds[56][1]=300;
	bounds[57][0]=-900; bounds[57][1]=900;
	bounds[58][0]=-900; bounds[58][1]=900;
	bounds[59][0]=-600; bounds[59][1]=600;
	bounds[60][0]=-600; bounds[60][1]=600;
	bounds[61][0]=-100; bounds[61][1]=150;
	bounds[62][0]=-100; bounds[62][1]=150;
	bounds[63][0]=-100; bounds[63][1]=100;
	bounds[64][0]=-100; bounds[64][1]=100;
	bounds[65][0]=-900; bounds[65][1]=900;
	bounds[66][0]=-900; bounds[66][1]=900;
	bounds[67][0]=-900; bounds[67][1]=900;
	bounds[68][0]=-900; bounds[68][1]=900;

	int x;

	for(i=4;i<61;i++)
	{
		if((i>3&&i<18)||(i>50&&i<61))
		{
			if(i>3&&i<12)
			{
				fapcounter[i]=new Fl_Slider(20,120+(i-4)*42,70,16,GetLabel(i));
				lframe[i]=new Fl_Input(91, 120+(i-4)*42,40,16, "");
			}
			else
				if(i>11&&i<53)
				{
					if(i>11&&i<18)
						x=12;
					else
						x=45;
					fapcounter[i]=new Fl_Slider(143,120+(i-x)*42,70,16,GetLabel(i));
					lframe[i]=new Fl_Input(214, 120+(i-x)*42,40,16, "");
				}
				else
					if(i>52&&i<61)
					{
						fapcounter[i]=new Fl_Slider(267,120+(i-53)*42,70,16,GetLabel(i));
						lframe[i]=new Fl_Input(338, 120+(i-53)*42,40,16, "");
					}
			
			fapcounter[i]->type(FL_HOR_NICE_SLIDER);
			fapcounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
			fapcounter[i]->bounds(bounds[i][0],bounds[i][1]);
			fapcounter[i]->step(10);
			
			lframe[i]->textsize(10);
			lframe[i]->value("0.00");
			fapcounter[i]->callback((Fl_Callback *)changedFAPvalue,this);
			lframe[i]->callback((Fl_Callback *)changeValueFAP,this);
		}
	}

	for(i=3;i<65;i++)
	{
		if((i==3) || (i==18) || (i>38&&i<48) || (i>60&&i<65))
		{
			if((i==3) || (i==18) || (i>38&&i<42))
			{
				if(i==3)
					x=3;
				else
				if(i==18)
					x=17;
				else
				if(i>38&&i<42)
					x=37;
				fapcounter[i]=new Fl_Slider(20,120+(i-x)*42,70,16,GetLabel(i));
				lframe[i]=new Fl_Input(91, 120+(i-x)*42,40,16, "");
			}
			else
				if(i>41&&i<47)
				{
					fapcounter[i]=new Fl_Slider(143,120+(i-42)*42,70,16,GetLabel(i));
					lframe[i]=new Fl_Input(214, 120+(i-42)*42,40,16, "");
				}
				else
					if((i==47) || (i>60&&i<65))
					{
						if(i==47)
							x=47;
						else
							x=60;
						fapcounter[i]=new Fl_Slider(267,120+(i-x)*42,70,16,GetLabel(i));
						lframe[i]=new Fl_Input(338, 120+(i-x)*42,40,16, "");
					}
			
			fapcounter[i]->type(FL_HOR_NICE_SLIDER);
			fapcounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
			fapcounter[i]->bounds(bounds[i][0],bounds[i][1]);
			fapcounter[i]->step(10);
			
			lframe[i]->textsize(10);
			lframe[i]->value("0.00");
			fapcounter[i]->callback((Fl_Callback *)changedFAPvalue,this);
			lframe[i]->callback((Fl_Callback *)changeValueFAP,this);
		}
	}

	for(i=19;i<39;i++)
	{
		if((i>18&&i<27) || (i>30&&i<39))
		{
			if(i>18&&i<25)
			{
				fapcounter[i]=new Fl_Slider(20,120+(i-19)*42,70,16,GetLabel(i));
				lframe[i]=new Fl_Input(91, 120+(i-19)*42,40,16, "");
			}
			else
				if((i>24&&i<27) || (i>30&&i<35))
				{
					if(i>24&&i<27)
						x=25;
					else
						x=29;
					fapcounter[i]=new Fl_Slider(143,120+(i-x)*42,70,16,GetLabel(i));
					lframe[i]=new Fl_Input(214, 120+(i-x)*42,40,16, "");
				}
				else
					if(i>34&&i<39)
					{
						fapcounter[i]=new Fl_Slider(267,120+(i-35)*42,70,16,GetLabel(i));
						lframe[i]=new Fl_Input(338, 120+(i-35)*42,40,16, "");
					}
			
			fapcounter[i]->type(FL_HOR_NICE_SLIDER);
			fapcounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
			fapcounter[i]->bounds(bounds[i][0],bounds[i][1]);
			fapcounter[i]->step(10);
			
			lframe[i]->textsize(10);
			lframe[i]->value("0.00");
			fapcounter[i]->callback((Fl_Callback *)changedFAPvalue,this);
			lframe[i]->callback((Fl_Callback *)changeValueFAP,this);
		}
	}

	for(i=48;i<51;i++)
	{
		if(i==48)
		{
			fapcounter[i]=new Fl_Slider(20,120+(i-48)*42,70,16,GetLabel(i));
			lframe[i]=new Fl_Input(91, 120+(i-48)*42,40,16, "");
		}
		else
			if(i==49)
			{
				fapcounter[i]=new Fl_Slider(143,120+(i-49)*42,70,16,GetLabel(i));
				lframe[i]=new Fl_Input(214, 120+(i-49)*42,40,16, "");
			}
			else
				if(i==50)
				{
					fapcounter[i]=new Fl_Slider(267,120+(i-50)*42,70,16,GetLabel(i));
					lframe[i]=new Fl_Input(338, 120+(i-50)*42,40,16, "");
				}
		
		fapcounter[i]->type(FL_HOR_NICE_SLIDER);
		fapcounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
		fapcounter[i]->bounds(bounds[i][0],bounds[i][1]);
		fapcounter[i]->step(10);
		
		lframe[i]->textsize(10);
		lframe[i]->value("0.00");
		fapcounter[i]->callback((Fl_Callback *)changedFAPvalue,this);
		lframe[i]->callback((Fl_Callback *)changeValueFAP,this);
	}

	for(i=3;i<65;i++)
	{
		if((i>=4 && i<18) || (i>=51 && i<61))
		{
			lframe[i]->show();
			fapcounter[i]->show();
		}
		else
		{
			if(i!=27 && i!=28 && i!=29 && i!=30)
			{
				lframe[i]->hide();
				fapcounter[i]->hide();
			}
		}
	}

	mouth=new Fl_Round_Button(10,40,16,16,"Mouth");
	mouth->align(FL_ALIGN_RIGHT);
	mouth->callback((Fl_Callback *)changedMOUTHgroup,this);
	mouth->value(1);
	
	eyes_eyebrows=new Fl_Round_Button(10,60,16,16,"Eyes and Eyebrows");
	eyes_eyebrows->align(FL_ALIGN_RIGHT);
	eyes_eyebrows->callback((Fl_Callback *)changedEYESgroup,this);

	head=new Fl_Round_Button(170,60,16,16,"Head");
	head->align(FL_ALIGN_RIGHT);
	head->callback((Fl_Callback *)changedHEADgroup,this);

	cheek=new Fl_Round_Button(170,40,16,16,"Cheeks, Jaw, Tongue and Nose");
	cheek->align(FL_ALIGN_RIGHT);
	cheek->callback((Fl_Callback *)changedCHEEKgroup,this);

	
	
	//FACS

	boxFACS = new Fl_Box(FL_BORDER_BOX, 460, 525, 390, 155,"FACS");
	boxFACS->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	facsbounds[0][0]=0; facsbounds[0][1]=500;
	facsbounds[1][0]=0; facsbounds[1][1]=500;
	facsbounds[2][0]=0; facsbounds[2][1]=150;
	facsbounds[3][0]=0; facsbounds[3][1]=800;
	facsbounds[4][0]=0; facsbounds[4][1]=400;
	facsbounds[5][0]=-200; facsbounds[5][1]=650;

	for(i=0;i<3;i++)
	{
		if (i==0)
			facscounter[i]=new Fl_Slider(480,555+(i)*42,100,16,"AU1");
		else if (i==1)
			facscounter[i]=new Fl_Slider(480,555+(i)*42,100,16,"AU2");
		else
			facscounter[i]=new Fl_Slider(480,555+(i)*42,100,16,"AU4");
		facscounter[i]->type(FL_HOR_NICE_SLIDER);
		facscounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
		facscounter[i]->bounds(facsbounds[i][0],facsbounds[i][1]);
		facscounter[i]->step(10);
		lfacs[i]=new Fl_Input(581, 555+(i)*42,40,16, "");
		lfacs[i]->textsize(10);
		lfacs[i]->value("0.00");
		lfacs[i]->deactivate();
		facscounter[i]->callback((Fl_Callback *)changedFACSvalue,this);
	}
	
	for(i=3;i<6;i++)
	{
		if (i==3)
			facscounter[i]=new Fl_Slider(670,555+(i-3)*42,100,16,"AU5");
		else if (i==4)
			facscounter[i]=new Fl_Slider(670,555+(i-3)*42,100,16,"AU6");
		else
			facscounter[i]=new Fl_Slider(670,555+(i-3)*42,100,16,"AU7");
		facscounter[i]->type(FL_HOR_NICE_SLIDER);
		facscounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
		facscounter[i]->bounds(facsbounds[i][0],facsbounds[i][1]);
		lfacs[i]=new Fl_Input(771, 555+(i-3)*42, 40,16, "");
		lfacs[i]->textsize(10);
		lfacs[i]->value("0.00");
		lfacs[i]->deactivate();
		facscounter[i]->step(10);
		facscounter[i]->callback((Fl_Callback *)changedFACSvalue,this);
	}

	for(i=0; i<6; i++)
	{
		AU1[i]=0;
		AU2[i]=0;
		AU4[i]=0;
	}
	for(i=0;i<2;i++)
	{
		AU5[i]=0;
		AU6[i]=0;
	}
	for(i=0;i<4;i++)
		AU7[i]=0;
	/// fine facs	


	//LIP PARAMETERS

	boxLIPS = new Fl_Box(FL_BORDER_BOX, 10, 475, 390, 200,"LIP PARAMETERS");
	boxLIPS->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	lipsbounds[0][0]=-30; lipsbounds[0][1]=500;
	lipsbounds[1][0]=-200; lipsbounds[1][1]=700;
	lipsbounds[2][0]=0; lipsbounds[2][1]=300;
	lipsbounds[3][0]=-400; lipsbounds[3][1]=300;
	lipsbounds[4][0]=-420; lipsbounds[4][1]=420;
	lipsbounds[5][0]=-400; lipsbounds[5][1]=500;
	lipsbounds[6][0]=-600; lipsbounds[6][1]=600;
	lipsbounds[7][0]=-600; lipsbounds[7][1]=600;

	for(i=0;i<4;i++)
	{
		if (i==0)
			lipscounter[i]=new Fl_Slider(30,505+(i)*42,100,16,"ULO");
		else if (i==1)
			lipscounter[i]=new Fl_Slider(30,505+(i)*42,100,16,"LLO");
		else if (i==2)
			lipscounter[i]=new Fl_Slider(30,505+(i)*42,100,16,"JAW");
		else
			lipscounter[i]=new Fl_Slider(30,505+(i)*42,100,16,"LSW");
		lipscounter[i]->type(FL_HOR_NICE_SLIDER);
		lipscounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
		lipscounter[i]->bounds(lipsbounds[i][0],lipsbounds[i][1]);
		lipscounter[i]->step(10);
		llips[i]=new Fl_Input(131, 505+(i)*42,40,16, "");
		llips[i]->textsize(10);
		llips[i]->value("0.00");
		llips[i]->deactivate();
		lipscounter[i]->callback((Fl_Callback *)changedLipParametersvalue,this);
	}
	
	for(i=4;i<8;i++)
	{
		if (i==4)
			lipscounter[i]=new Fl_Slider(220,505+(i-4)*42,100,16,"ULP");
		else if (i==5)
			lipscounter[i]=new Fl_Slider(220,505+(i-4)*42,100,16,"LLP");
		else if (i==6)
			lipscounter[i]=new Fl_Slider(220,505+(i-4)*42,100,16,"CRR");
		else
			lipscounter[i]=new Fl_Slider(220,505+(i-4)*42,100,16,"CRL");
		lipscounter[i]->type(FL_HOR_NICE_SLIDER);
		lipscounter[i]->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
		lipscounter[i]->bounds(lipsbounds[i][0],lipsbounds[i][1]);
		llips[i]=new Fl_Input(321, 505+(i-4)*42, 40,16, "");
		llips[i]->textsize(10);
		llips[i]->value("0.00");
		llips[i]->deactivate();
		lipscounter[i]->step(10);
		lipscounter[i]->callback((Fl_Callback *)changedLipParametersvalue,this);
	}

	for(i=0; i<6; i++)
	{
		ULO[i]=0;
		LLO[i]=0;
		LSW[i]=0;
	}
	for(i=0;i<2;i++)
	{
		ULP[i]=0;
		CRR[i]=0;
		CRL[i]=0;
	}
	for(i=0;i<4;i++)
		JAW[i]=0;
	LLP=0;
	/// fine lips parameters	

	glutw=new FaceLibraryViewerGlutWindow(420,20,470,400,"test");
	
	reset = new Fl_Button(620, 430, 80, 25, "Reset");
	reset->callback((Fl_Callback *)resetCB,this);

	classes=new Fl_Choice(440,480,100,20,"class:");
	classes->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	
	instances=new Fl_Choice(570,480,100,20,"instances:");
	instances->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);

	framenumber=new Fl_Counter(700,480,90,20,"variety of exp:");

	framenumber->type(FL_SIMPLE_COUNTER);
	framenumber->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
	framenumber->step(1);

	framenumber->bounds(0,0);

	framenumber->callback((Fl_Callback *)changedframenumber,this);

	probability=new Fl_Value_Output(820,480,50,20,"prob:");
	probability->align(FL_ALIGN_LEFT | FL_ALIGN_TOP);
	
	LoadLibrary();

	position_x=0;
	position_y=0;
	width=500;
	height=200;
	
}

FaceLibraryViewerWindow::~FaceLibraryViewerWindow()
{
	fclose(face_log);
}

//DEL void FaceLibraryViewerWindow::draw()
//DEL {
//DEL 	channels->resize(5,5,width-10,height-10);
//DEL }

//DEL void FaceLibraryViewerWindow::resize(int x, int y, int w, int h)
//DEL {
//DEL 	position_x=x;
//DEL 	position_y=y;
//DEL 	width=w;
//DEL 	height=h;
//DEL 	channels->resize(5,5,width-10,height-10);
//DEL }

int FaceLibraryViewerWindow::handle(int event)
{
	int i;
	if(Fl::event_button()==FL_LEFT_MOUSE)
	{
		if(event==FL_PUSH)
		{
			for (i=3; i<65; i++)
			{
				if(i!=27 && i!=28 && i!=29 && i!=30)
				{
					if(Fl::event_inside(fapcounter[i]))
					{
						if(fapcounter[i]->visible())
						{
							Index=i;
							break;
						}
					}
					else
						Index=0;
					
					if(Fl::event_inside(lframe[i]))
					{
						if(lframe[i]->visible())
						{
							IndexB=i;
							break;
						}
					}
					else
						IndexB=0;	
				}
			}
			for (i=0; i<6; i++)
			{
				if(Fl::event_inside(facscounter[i]))
				{
					IndexFACS=i;
					break;
				}
				else
					IndexFACS=-1;
			}
			for (i=0; i<8; i++)
			{
				if(Fl::event_inside(lipscounter[i]))
				{
					IndexLIPS=i;
					break;
				}
				else
					IndexLIPS=-1;
			}
		}
	}

	return Fl_Window::handle(event);
}

void FaceLibraryViewerWindow::LoadInstances()
{	
	this->instances->clear();
	std::string s;
	s=classes->text();
	std::list<FaceExpression>::iterator iter;
	instances->add("",0,(Fl_Callback *)selectedinstance,this,0);
	for(iter=facelibrary.FExpressions.begin();iter!=facelibrary.FExpressions.end();iter++)
	{
		if(s==(*iter).classname)
			instances->add((*iter).instance.c_str(),0,(Fl_Callback *)selectedinstance,this,0);
	}
	instances->value(0);

}

void FaceLibraryViewerWindow::LoadFaceFrame(int n)
{
	FaceExpression *selectedexpression;
	FAPFrame *frame;
	
	selectedexpression=GetSelectedExpression();


	frame=GetSelectedFAPFrame(n);

	if(frame!=0)
	{
		glutw->agent->LoadFAPFrame(frame);
		framenumber->value(n);
		framenumber->range(0,selectedexpression->GetNumberOfFrames()-1);
		probability->value(frame->probability);
	}
}

void FaceLibraryViewerWindow::UpdateFAPValues(FAPFrame *f)
{
	char value[256];

	int i;
	if(f!=NULL)
	{
		
		for(i=0; i<6; i++)
		{
			facscounter[i]->value(0);
			lfacs[i]->value("0");
			AU1[i]=0;
			AU2[i]=0;
			AU4[i]=0;
		}
		for(i=0;i<2;i++)
		{
			AU5[i]=0;
			AU6[i]=0;
		}
		for(i=0;i<4;i++)
			AU7[i]=0;

		for(i=3; i<65; i++)
		{
			if(i!=27 && i!=28 && i!=29 && i!=30)
			{
				//fapcounter[i]->value(f->FAPs[i].value);
				//fapcounter[i]->set_changed();
				fapcounter[i]->value(((f->FAPs[i]).value));
				sprintf(value,"%.2f",fapcounter[i]->value());
				lframe[i]->value(value);
			}
		}
	}
	//this->redraw();
}

void FaceLibraryViewerWindow::CopyFAPValuesToFAPFrame()
{
	int i;
	//frame=new FAPFrame();
	frame=GetSelectedFAPFrame(framenumber->value());

	if(frame==0)
		frame = new FAPFrame();

	for(i=3; i<65; i++)
	{
		if(i!=27 && i!=28 && i!=29 && i!=30)
		{
			if(fapcounter[i]->value()!=0)
				frame->SetFAP(i,fapcounter[i]->value());
			//if(fapcounter[i]->value()!=0)
			//	frame->SetFAP(i,0);

		}
	}
	glutw->agent->LoadFAPFrame(frame);
}


int FaceLibraryViewerWindow::SaveFAPFrame()
{
	int i;
	FILE *fid;
	std::string nameExpression;
	std::string classexpression;
	std::string nameFile;
	std::string face_part_mouth, face_part_eyelids, face_part_eyes, face_part_eyebrows, face_part_head;
	int flag_mouth, flag_eyelids, flag_eyes, flag_eyebrows, flag_head;

	ow = new OpenWin("Save Faps for face library (XML)");
	ow->set_modal();
	ow->show();
	while (ow->shown())
		Fl::wait();
	nameExpression = ow->nameEmotion->value();
	//strcpy(classexpression, ow->classexpression);
	classexpression=this->classes->text();
	
	if (nameExpression=="")
		return 0;
	
	nameFile=nameExpression;
	nameFile.append(".txt");

	fid=fopen(nameFile.c_str(), "w");
	if (!fid)
		return 0;

	fprintf(fid,"\n### DECLARATION ###\n\n");

	//expression_mouth_&_cheeks_&_nouse
	face_part_mouth=nameExpression;
	face_part_mouth=face_part_mouth.append("_mouth");
	flag_mouth=0;
	for(i=3;i<19;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_mouth==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_mouth.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_mouth=1;
			}					
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}

	for(i=39;i<48;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_mouth==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_mouth.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_mouth=1;
			}
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}

	for(i=51;i<65;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_mouth==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_mouth.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_mouth=1;
			}
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}
	if(flag_mouth==1)
		fprintf(fid,"</actiondeclaration>\n\n");

	//expression_eyes
	face_part_eyes=nameExpression;
	face_part_eyes=face_part_eyes.append("_eyes");
	flag_eyes=0;
	for(i=23;i<27;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_eyes==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_eyes.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_eyes=1;
			}					
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}
	if(flag_eyes==1)
		fprintf(fid,"</actiondeclaration>\n\n");

	//expression_eyelids
	face_part_eyelids=nameExpression;
	face_part_eyelids=face_part_eyelids.append("_eyelids");
	flag_eyelids=0;
	for(i=19;i<23;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_eyelids==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_eyelids.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_eyelids=1;
			}					
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}
	if(flag_eyelids==1)
		fprintf(fid,"</actiondeclaration>\n\n");
	
	//expression_eyebrows
	face_part_eyebrows=nameExpression;
	face_part_eyebrows=face_part_eyebrows.append("_eyebrows");
	flag_eyebrows=0;
	for(i=31;i<39;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_eyebrows==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_eyebrows.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_eyebrows=1;
			}					
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}
	if(flag_eyebrows==1)
		fprintf(fid,"</actiondeclaration>\n\n");

	//expression_head
	face_part_head=nameExpression;
	face_part_head=face_part_head.append("_head");
	flag_head=0;
	for(i=48;i<51;i++)
	{
		if(fapcounter[i]->value()!=0)
		{
			if(flag_head==0)
			{
				fprintf(fid,"<actiondeclaration name=\"%s\">\n", face_part_head.c_str());
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
				flag_head=1;
			}					
			else
				fprintf(fid,"<fap num=\"%d\" value=\"%d\"/>\n", i, (int)fapcounter[i]->value());
		}
	}
	if(flag_head==1)
		fprintf(fid,"</actiondeclaration>\n\n");

	fprintf(fid,"\n### IMPLEMENTATION ###\n\n");

	if(flag_mouth!=0 || flag_eyelids!=0 || flag_eyebrows!=0 || flag_head!=0  || flag_eyes!=0)
	{
		fprintf(fid,"<expression class=\"%s\" instance=\"%s\">\n", classexpression.c_str(), nameExpression.c_str());
		
		nameExpression.append("=");
		if(flag_mouth!=0)
			fprintf(fid,"<action name=\"%s\"/>\n", face_part_mouth.c_str());

		if(flag_eyelids!=0)
			fprintf(fid,"<action name=\"%s\"/>\n", face_part_eyelids.c_str());
		if(flag_eyebrows!=0)
			fprintf(fid,"<action name=\"%s\"/>\n", face_part_eyebrows.c_str());
		if(flag_head!=0)
			fprintf(fid,"<action name=\"%s\"/>\n", face_part_head.c_str());
		if(flag_eyes!=0)
			fprintf(fid,"<action name=\"%s\"/>\n", face_part_eyes.c_str());
		
		fprintf(fid,"</expression>\n\n");
	}

	fclose(fid);
	return 1;
}

int FaceLibraryViewerWindow::SaveFAPFile()
{
	int i;
	FILE *fid;
	std::string nameExpression;
	std::string path;

	ow = new OpenWin("Save Fap File");
	ow->set_modal();
	ow->show();
	while (ow->shown())
		Fl::wait();
	nameExpression = ow->nameEmotion->value();

	if (nameExpression=="")
		return 0;
	path="2.1 ";
	path.append(nameExpression.c_str());
	path.append(" 25 1\n");
	
	nameExpression.append(".fap");

	fid=fopen(nameExpression.c_str(), "w");
	if (!fid)
		return 0;
	
	fprintf(fid,path.c_str(),0);
	//fprintf(fid,"\n0 0 ",0);
	for(i=1;i<NUMBER_OF_FAPS;i++)
	{
		if(i==1 || i==2 || i==27 || i==28 || i==29 || i==30 || i==65 || i==66 || i==67 || i==68)
			fprintf(fid,"%d ",0);
		else
		{
			if(fapcounter[i]->value()!=0)
				fprintf(fid,"%d ",1);
			else
				fprintf(fid,"%d ",0);
		}
	}

	fprintf(fid,"\n0 ",0);

	for(i=3;i<65;i++)
	{
		if(i!=27 && i!=28 && i!=29 && i!=30)
			if(fapcounter[i]->value()!=0)
				fprintf(fid,"%d ",(int)fapcounter[i]->value());
	}
	fprintf(fid,"\n",0);
	fclose(fid);
	return 1;
}


FAPFrame * FaceLibraryViewerWindow::GetSelectedFAPFrame(int n)
{
	FaceExpression *faceexpression;
	FAPFrame *frame;


	faceexpression=GetSelectedExpression();	

	if(faceexpression!=0)
	{
		frame=(faceexpression->GetFAPFrame(n));
		return frame;
	}
	else
		return 0;
}

void FaceLibraryViewerWindow::LoadLibrary()
{
	//fap_id_list *fap_element;
	//fap_id_list *fap_element_prev;
	
	//int **faps_id;
	///std::string facialdisplayslibrarypath;
	//std::string Script_File_Modified;

	/*
	if(dictionary!=NULL)
	{
		resetCB(reset, this);
		FaceLibParser->ResetDictionary();
	}
	*/

	//facialdisplayslibrarypath=inimanager.Program_Path+"fd/apml_facelibrary_wip.txt";
	
	/*
	FaceLibParser=new FaceLibraryParser();
	if(FaceLibParser->ParseFacialDisplaysLibrary(facialdisplayslibrarypath)==0)
	{
		printf("Error in loading the library!!\n");
		exit(0);
	}
	
	fap_element=dictionary;
	*/
	/*
	if(fap_element->id!=NULL && strcmp(fap_element->id, "nothing")!=0)
		classes->add(fap_element->class_type,0,(Fl_Callback *)selectedclass,this,0);
	fap_element_prev=fap_element;
	fap_element=(fap_id_list*)(fap_element->next);
	while (fap_element!=NULL)
	{	
		if(strcmp(fap_element_prev->class_type, fap_element->class_type)!=0 && 
			strcmp(fap_element->class_type, "deictic")!=0 && 
			(fap_element->id!=NULL && strcmp(fap_element->id, "nothing")!=0))
		{
			classes->add(fap_element->class_type,0,(Fl_Callback *)selectedclass,this,0);
			//printf("%s  %s\n", fap_element->class_type, fap_element->id);
		}
		fap_element_prev=fap_element;
		fap_element=(fap_id_list*)(fap_element->next);
	}
	*/

//	resetCB(reset, this);
	
	facelibrary.Init(inimanager.Program_Path+inimanager.GetValueString("FACELIBRARY_PATH"));
	//facelibrary.Init("fd/facelibrary.xml");

	std::list<std::string>::iterator iter;
	if(!facelibrary.classes.empty())
		for(iter=facelibrary.classes.begin();iter!=facelibrary.classes.end();iter++)
		{
			classes->add((*iter).c_str(),0,(Fl_Callback *)selectedclass,this,0);
		}

	classes->value(0);
	LoadInstances();
	instances->value(0);

}

FaceExpression * FaceLibraryViewerWindow::GetSelectedExpression()
{
	std::string cl,in;


	FaceExpression *faceexpression;

	faceexpression=0;

	cl=classes->text();
	in=instances->text();

	if((cl!="")&&(in!=""))
		faceexpression=facelibrary.GetExpression(cl,in);

	return faceexpression;
}

int FaceLibraryViewerWindow::SaveGeometry()
{
	std::string path;
	path=inimanager.Program_Path;

	chooser = new Fl_File_Chooser(path.c_str(),"File vtx (*.vtx)",2,"Save");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	if(chooser->value()!=0)
	{
		if(glutw->agent->ExportFace((char*)chooser->value(),0)==0, 0)
		{
			delete chooser;
			return 0;	
		}
	}
	else
	{
		delete chooser;
		return 0;
	}
	delete chooser;
	return 1;
}

int FaceLibraryViewerWindow::SaveGeometryVRML()
{
	std::string path;
	path=inimanager.Program_Path;

	chooser = new Fl_File_Chooser(path.c_str(),"File vrml (*.vrml)",2,"Save");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	if(chooser->value()!=0)
	{
		if(glutw->agent->ExportFaceVRML((char*)chooser->value(),0)==0, 0)
		{
			delete chooser;
			return 0;	
		}
	}
	else
	{
		delete chooser;
		return 0;
	}
	delete chooser;
	return 1;
}


char* FaceLibraryViewerWindow::GetLabel(int fap)
{
	switch(fap)
	{
	case 1: return("1"); break;
	case 2: return("2"); break;
	case 3: return("3   (2.1 y)"); break;
	case 4: return("4   (2.2 y)"); break;
	case 5: return("5   (2.3 y)"); break;
	case 6: return("6   (2.4 x)"); break;
	case 7: return("7   (2.5 x)"); break;
	case 8: return("8   (2.6 y)"); break;
	case 9: return("9   (2.7 y)"); break;
	case 10: return("10   (2.9 y)"); break;
	case 11: return("11   (2.8 y)"); break;
	case 12: return("12   (2.4 y)"); break;
	case 13: return("13   (2.5 y)"); break;
	case 14: return("14   (2.1 z)"); break;
	case 15: return("15   (2.1 x)"); break;
	case 16: return("16   (8.2 z)"); break;
	case 17: return("17   (8.1 z)"); break;
	case 18: return("18   (2.10 y)"); break;
	case 19: return("19   (3.1 y)"); break;
	case 20: return("20   (3.2 y)"); break;
	case 21: return("21   (3.3 y)"); break;
	case 22: return("22   (3.4 y)"); break;
	case 23: return("23   (3.5 x)"); break;
	case 24: return("24   (3.6 x)"); break;
	case 25: return("25   (3.5 y)"); break;
	case 26: return("26   (3.6 y)"); break;
	case 27: return("27   (--)"); break;
	case 28: return("28   (--)"); break;
	case 29: return("29   (pupil sx)"); break;
	case 30: return("30   (pupil dx)"); break;
	case 31: return("31   (4.1 y)"); break;
	case 32: return("32   (4.2 y)"); break;
	case 33: return("33   (4.3 y)"); break;
	case 34: return("34   (4.4 y)"); break;
	case 35: return("35   (4.5 y)"); break;
	case 36: return("36   (4.6 y)"); break;
	case 37: return("37   (4.1 x)"); break;
	case 38: return("38   (4.2 x)"); break;
	case 39: return("39   (5.1 x)"); break;
	case 40: return("40   (5.2 x)"); break;
	case 41: return("41   (5.3 y)"); break;
	case 42: return("42   (5.4 y)"); break;
	case 43: return("43   (6.1 y)"); break;
	case 44: return("44   (6.1 z)"); break;
	case 45: return("45   (6.2 y)"); break;
	case 46: return("46   (6.3 xy)"); break;
	case 47: return("47   (6.4 xy)"); break;
	case 48: return("48   (rotation x)"); break;
	case 49: return("49   (rotation y)"); break;
	case 50: return("50   (rotation z)"); break;
	case 51: return("51   (8.1 y)"); break;
	case 52: return("52   (8.2 y)"); break;
	case 53: return("53   (8.3 x)"); break;
	case 54: return("54   (8.4 x)"); break;
	case 55: return("55   (8.5 y)"); break;
	case 56: return("56   (8.6 y)"); break;
	case 57: return("57   (8.7 y)"); break;
	case 58: return("58   (8.8 y)"); break;
	case 59: return("59   (8.3 y)"); break;
	case 60: return("60   (8.4 y)"); break;
	case 61: return("61   (9.1 x)"); break;
	case 62: return("62   (9.2 x)"); break;
	case 63: return("63   (9.3 y)"); break;
	case 64: return("64   (9.3 x)"); break;
	case 65: return("65   (10.1 y)"); break;
	case 66: return("66   (10.2 x)"); break;
	case 67: return("67   (10.3 y)"); break;
	case 68: return("68   (10.4 x)"); break;
	default: return("");
	}
}