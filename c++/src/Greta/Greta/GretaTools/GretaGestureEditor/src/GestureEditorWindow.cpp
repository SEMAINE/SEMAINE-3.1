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

// GestureEditorWindow.cpp: implementation of the GestureEditorWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "GestureEditorWindow.h"
#include "IniManager.h"
#include "GestureEngine.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <crtdbg.h>

#include "DataContainer.h"
extern DataContainer *datacontainer;

using namespace GestureSpace;

IniManager inimanager;
extern FILE* gesture_log;

void GestureEditorWindowclosed(GestureEditorWindow* v)
{
}

void selectedphasestartslider(Fl_Widget* w, GestureEditorWindow* v)
{
	char t[10];
	sprintf(t,"%.2f",v->phasestartslider->value());
	v->phasestartoutput->value(t);
	(*(v->phasesVector))[v->actualframe]->SetRelTime(v->phasestartslider->value());
}

void selectedframetype(Fl_Choice* w, GestureEditorWindow* v)
{
	(*(v->phasesVector))[v->actualframe]->type=GestureSpace::GesturePhaseType(w->value());
}

void selectedshape0(Fl_Widget* w, GestureEditorWindow* v)
{
/*	if((*(v->phasesVector))[v->actualframe]->whichshape==0)
		(*(v->phasesVector))[v->actualframe]->whichshape=-1;
	else
		(*(v->phasesVector))[v->actualframe]->whichshape=0;
	v->ShowActualFrame();*/
}

void selectedshape1(Fl_Widget* w, GestureEditorWindow* v)
{
/*	if((*(v->phasesVector))[v->actualframe]->whichshape==1)
		(*(v->phasesVector))[v->actualframe]->whichshape=-1;
	else
		(*(v->phasesVector))[v->actualframe]->whichshape=1;
	v->ShowActualFrame();*/
}

void selectedshape2(Fl_Widget* w, GestureEditorWindow* v)
{
/*	if((*(v->phasesVector))[v->actualframe]->whichshape==2)
		(*(v->phasesVector))[v->actualframe]->whichshape=-1;
	else
		(*(v->phasesVector))[v->actualframe]->whichshape=2;
	v->ShowActualFrame();*/
}

void selectedstoredpose(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->storedpose->value()==1)
		(*(v->phasesVector))[v->actualframe]->AddStoredPose("select file");
	else
		(*(v->phasesVector))[v->actualframe]->RemoveStoredPose();
	v->ShowActualFrame();
}

void selectedfixed_x(Fl_Widget* w, GestureEditorWindow* v)
{
    //ADDME: Can't this be done simpler?
	
	GesturePhase * frame = (*(v->phasesVector))[v->actualframe];

	if(v->fixed_x->value()==1)
		frame->Add(frame->GetPArm()->x, true);
	else
		frame->Add(frame->GetPArm()->x, false);

	v->ShowActualFrame();
}

void selectedfixed_y(Fl_Widget* w, GestureEditorWindow* v)
{
    //ADDME: Can't this be done simpler?
	
	GesturePhase * frame = (*(v->phasesVector))[v->actualframe];

	if(v->fixed_y->value()==1)
		frame->Add(frame->GetPArm()->y, true);
	else
		frame->Add(frame->GetPArm()->y, false);

	v->ShowActualFrame();
}

void selectedfixed_z(Fl_Widget* w, GestureEditorWindow* v)
{
    //ADDME: Can't this be done simpler?
	
	GesturePhase * frame = (*(v->phasesVector))[v->actualframe];

	if(v->fixed_z->value()==1)
		frame->Add(frame->GetPArm()->z, true);
	else
		frame->Add(frame->GetPArm()->z, false);

	v->ShowActualFrame();
}

void selectedfixed_form(Fl_Widget* w, GestureEditorWindow* v)
{
    //ADDME: Can't this be done simpler?
	
	GesturePhase * frame = (*(v->phasesVector))[v->actualframe];
	BasicForm* pForm = (BasicForm*) frame->GetPHand()->shape;

	if(v->fixed_form->value()==1)
		frame->Add(pForm->type, true);
	else
		frame->Add(pForm->type, false);

	v->ShowActualFrame();
}

void selectedquit(Fl_Widget* w, GestureEditorWindow* v)
{
	v->agentwindow->hide();
	v->expressivitywindow->hide();
	v->hide();
}

void selectedsave(Fl_Widget* w, GestureEditorWindow* v)
{
	v->Save();
}

void selectedload(Fl_Widget* w, GestureEditorWindow* v)
{
	v->Load();
}

void selectedtestgesture(Fl_Widget* w, GestureEditorWindow* v)
{
	v->Test();
}

void selectedsideright(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->sideright->value() == 1) v->gesture->SetSide(r);
	else v->gesture->SetSide(no_side);
	v->ShowActualSide();
}

void selectedsideleft(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->sideleft->value() == 1) v->gesture->SetSide(l);
	else v->gesture->SetSide(no_side);
	v->ShowActualSide();
}

void selectedsideboth(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->sideboth->value() == 1) v->gesture->SetSide(both_sides);
	else v->gesture->SetSide(no_side);
	v->ShowActualSide();
}

void selectedsideassym(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->sideassym->value() == 1) v->gesture->SetSide(assym);
	else v->gesture->SetSide(no_side);
	v->assymright->value(1);
	v->ShowActualSide();
}

void selectedassymright (Fl_Widget* w, GestureEditorWindow* v){
	if(v->gesture->GetSide()==assym){
		v->assymright->value(1);
		if(!(v->firstArm)){
			v->phasesVector = &(v->gesture->phases);
			v->firstArm = true;
			v->actualframe = 0;
			v->ShowActualFrame();
		}
	}
	else v->assymright->value(0);
	v->assymleft->value(0);
}

void selectedassymleft (Fl_Widget* w, GestureEditorWindow* v){
	if(v->gesture->GetSide()==assym) {
		v->assymleft->value(1);
		if(v->firstArm){
			v->phasesVector = &(v->gesture->phasesAssym);
			v->firstArm = false;
			v->actualframe = 0;
			v->ShowActualFrame();
		}
	}
	else v->assymleft->value(0);
	v->assymright->value(0);
}

void selectedassymswitch (Fl_Widget* w, GestureEditorWindow* v){
	if(v->gesture->GetSide()==assym){
		v->gesture->SwitchPhasesVectors();
		v->assymright->value(!v->assymright->value());
		v->assymleft->value(!v->assymleft->value());
		if(v->firstArm){
			//v->assymright->value(0);
			//v->assymleft->value(1);
			v->phasesVector = &(v->gesture->phasesAssym);
		}
		else{
			//v->assymright->value(1);
			//v->assymleft->value(0);
			v->phasesVector = &(v->gesture->phases);
		}
		v->firstArm = !(v->firstArm);
	}
}

void selectedinsertframe(Fl_Widget* w, GestureEditorWindow* v)
{
	std::vector<GesturePhase *>::iterator iter;
	iter=v->phasesVector->begin();
	int count=0;
	while(count<v->actualframe)
	{
		iter++;
		count++;
	}
	iter++;
	GesturePhase *phase=new GesturePhase();
	phase->Start();
	phase->Finalize();
	//FXME phase->type="PHASETYPE_DEFAULT";
	//if(actualframe==(phasesVector->size()-1))
		v->phasesVector->push_back(phase);
	/*else
		phasesVector->insert(iter,phase);*/
	v->ShowActualFrame();
	v->redraw();
}

void selecteddeleteframe(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->phasesVector->size()>1)
	{
		std::vector<GesturePhase *>::iterator iter;
		iter=v->phasesVector->begin();
		int count=0;
		while(count<v->actualframe)
		{
			iter++;
			count++;
		}
		v->phasesVector->erase(iter);
		if(v->actualframe>(v->phasesVector->size()-1))
			v->actualframe--;
		v->ShowActualFrame();
		v->redraw();
	}
}

void selectednextframe(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->actualframe<(v->phasesVector->size()-1))
	{
		v->actualframe++;
		v->ShowActualFrame();
	}
}

void selectedprevframe(Fl_Widget* w, GestureEditorWindow* v)
{
	if(v->actualframe>0)
	{
		v->actualframe--;
		v->ShowActualFrame();
	}
}

void selectedwristmixminus(Fl_Widget* w, GestureEditorWindow* v)
{
	WristOrientation *wrist = (*(v->phasesVector))[v->actualframe]->GetPWrist();
	
	if(wrist->GetWristMagnitude()>0.05)
	{
		wrist->SetWristMagnitude(wrist->GetWristMagnitude()-0.05);
	}
	v->ShowActualFrame();
}

void selectedwristmixplus(Fl_Widget* w, GestureEditorWindow* v)
{
	WristOrientation *wrist = (*(v->phasesVector))[v->actualframe]->GetPWrist();
	
	if(wrist->GetWristMagnitude()<1)
	{
		wrist->SetWristMagnitude(wrist->GetWristMagnitude()+0.05);
	}
	v->ShowActualFrame();
}

void selectedbrowseposefile(Fl_Widget* w, GestureEditorWindow* v)
{
	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser("poses","pose files (*.txt)",0|2,"select a pose file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	if(chooser->value()!=0)
		(*(v->phasesVector))[v->actualframe]->AddStoredPose(chooser->value());
	delete chooser;
	v->ShowActualFrame();
}

void selectedgestureinstance(Fl_Input* w, GestureEditorWindow* v)
{
	v->gesture->SetInstance(w->value());
}

void selectedgestureclass(Fl_Input* w, GestureEditorWindow* v)
{
	v->gesture->SetClass(w->value());
}

void selectedradio(Fl_Round_Button* w, GestureEditorWindow* v)
{
	//printf("adding %s\n",w->label());
	(*(v->phasesVector))[v->actualframe]->Add(w->label());
	v->ShowActualFrame();
}

void selected_use_transition(Fl_Check_Button* w, GestureEditorWindow* v){
	if(w->value()==0)
		(*(v->phasesVector))[v->actualframe]->SetCurvedTransition(NULL);
	else
		(*(v->phasesVector))[v->actualframe]->SetCurvedTransition(new CurvedTransition());
	v->ShowActualTransition();
}
void selected_wave(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	Direction d = ct->getStartDirection();
	if(d==INTERN || d==EXTERN)
		d = UP;
	ct->set(WAVE,ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),d,0);
	v->ShowActualTransition();
}
void selected_circle(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(CIRCLE,ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),ct->getStartDirection(),ct->getInverseWay());
	v->ShowActualTransition();
}

void selected_spiral_in(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(SPIRAL_IN,ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),ct->getStartDirection(),ct->getInverseWay());
	v->ShowActualTransition();
}

void selected_spiral_out(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(SPIRAL_OUT,ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),ct->getStartDirection(),ct->getInverseWay());
	v->ShowActualTransition();
}

void selected_custom(Fl_Round_Button* w, GestureEditorWindow* v){
	v->GetActualTransition()->setMovement(CUSTOM);
	v->ShowActualTransition();
}

void selected_way(Fl_Check_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),ct->getStartDirection(),!(ct->getInverseWay()));
	v->ShowActualTransition();
}

void selected_up(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),UP,ct->getInverseWay());
	v->ShowActualTransition();
}

void selected_down(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),DOWN,ct->getInverseWay());
	v->ShowActualTransition();
}

void selected_intern(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),INTERN,ct->getInverseWay());
	v->ShowActualTransition();
}

void selected_extern(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),ct->getOrientation(),EXTERN,ct->getInverseWay());
	v->ShowActualTransition(); 
}

void selected_XY(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),SAGITTAL,ct->getStartDirection(),ct->getInverseWay());
	v->ShowActualTransition(); 
}

void selected_YZ(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),FRONTAL,ct->getStartDirection(),ct->getInverseWay());
	v->ShowActualTransition(); 
}

void selected_XZ(Fl_Round_Button* w, GestureEditorWindow* v){
	CurvedTransition* ct = v->GetActualTransition();
	ct->set(ct->getMovement(),ct->getFrequencyOnPlane(),ct->getAmplitudeOnPlane(),TRANSVERSAL,ct->getStartDirection(),ct->getInverseWay());
	v->ShowActualTransition(); 
}

void selected_amplitude_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	float a = w->value();
	CurvedTransition * ct = v->GetActualTransition();
	Orientation o = ct->getOrientation();
	
	if(ct->getMovement()==WAVE){
		if(o == SAGITTAL) ct->setAmplitude(a,1);
		if(o == TRANSVERSAL) ct->setAmplitude(a,0);
		if(o == FRONTAL) ct->setAmplitude(a,2);
	}
	else if(ct->getMovement()!=CUSTOM) ct->setAmplitudeOnPlane(a);

	v->x_amplitude->value(ct->getAmplitude(0));
	v->y_amplitude->value(ct->getAmplitude(1));
	v->z_amplitude->value(ct->getAmplitude(2));
}
void selected_x_amplitude_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	
	CurvedTransition * ct = v->GetActualTransition();
	ct->setAmplitude(w->value(),0);
	v->amplitude->value(ct->getAmplitudeOnPlane());
}
void selected_y_amplitude_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	
	CurvedTransition * ct = v->GetActualTransition();
	ct->setAmplitude(w->value(),1);
	v->amplitude->value(ct->getAmplitudeOnPlane());
}

void selected_z_amplitude_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	
	CurvedTransition * ct = v->GetActualTransition();
	ct->setAmplitude(w->value(),2);
	v->amplitude->value(ct->getAmplitudeOnPlane());
}

void selected_frequency_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	float a = w->value();
	CurvedTransition * ct = v->GetActualTransition();
	Orientation o = ct->getOrientation();
	
	if(ct->getMovement()==WAVE){
		if(o == SAGITTAL) ct->setFrequency(a,1);
		if(o == TRANSVERSAL) ct->setFrequency(a,0);
		if(o == FRONTAL) ct->setFrequency(a,2);
	}
	else if(ct->getMovement()!=CUSTOM) ct->setFrequencyOnPlane(a);

	v->x_frequency->value(ct->getFrequency(0));
	v->y_frequency->value(ct->getFrequency(1));
	v->z_frequency->value(ct->getFrequency(2));
}
void selected_x_frequency_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	
	CurvedTransition * ct = v->GetActualTransition();
	ct->setFrequency(w->value(),0);
	v->frequency->value(ct->getFrequencyOnPlane());
}
void selected_y_frequency_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	
	CurvedTransition * ct = v->GetActualTransition();
	ct->setFrequency(w->value(),1);
	v->frequency->value(ct->getFrequencyOnPlane());
}
void selected_z_frequency_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	
	CurvedTransition * ct = v->GetActualTransition();
	ct->setFrequency(w->value(),2);
	v->frequency->value(ct->getFrequencyOnPlane());
}

void selected_x_shift_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setShift(w->value(),0);
}

void selected_y_shift_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setShift(w->value(),1);
}

void selected_z_shift_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setShift(w->value(),2);
}
void selected_x_space_variation_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setSpatialVariation(w->value(),0);
}

void selected_y_space_variation_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setSpatialVariation(w->value(),1);
}

void selected_z_space_variation_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setSpatialVariation(w->value(),2);
}

void selected_x_temp_variation_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setTemporalVariation(w->value(),0);
}

void selected_y_temp_variation_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setTemporalVariation(w->value(),1);
}

void selected_z_temp_variation_slider(Fl_Slider* w, GestureEditorWindow* v){
	//char t[10];
	//sprintf(t,"%.2f",v->phasestartslider->value());
	//v->phasestartoutput->value(t);
	v->GetActualTransition()->setTemporalVariation(w->value(),2);
}
GestureEditorWindow::GestureEditorWindow():Fl_Window(100,50,550,770,"gesture editor 0.9b")
{
	
	firstArm = true;

	Fl_Group* flgroup; // For grouping radio buttons
	{
	gestureclass=new Fl_Input(80,10,200,17,"gesture class:");
	gestureclass->align(FL_ALIGN_LEFT);
	gestureclass->labelfont(FL_HELVETICA);
	gestureclass->labelsize(11);
	gestureclass->textfont(FL_HELVETICA);
	gestureclass->textsize(11);
	gestureclass->callback((Fl_Callback *)selectedgestureclass,this);

	gestureinstance=new Fl_Input(80,30,200,17,"instance:");
	gestureinstance->align(FL_ALIGN_LEFT);
	gestureinstance->labelfont(FL_HELVETICA);
	gestureinstance->labelsize(11);
	gestureinstance->textfont(FL_HELVETICA);
	gestureinstance->textsize(11);
	gestureinstance->callback((Fl_Callback *)selectedgestureinstance,this);
	
	//duration=new Fl_Output(80,50,40,17,"duration:");
	//duration->align(FL_ALIGN_LEFT);
	//duration->labelfont(FL_HELVETICA);
	//duration->labelsize(11);
	//duration->textfont(FL_HELVETICA);
	//duration->textsize(11);
	//duration->value("1.0s");

	//durationminus=new Fl_Button(130,50,17,17,"-");
	//durationplus=new Fl_Button(147,50,17,17,"+");


	sideright=new Fl_Toggle_Button(80,70,40,17,"right");
	sideright->labelfont(FL_HELVETICA);
	sideright->labelsize(11);
	sideright->callback((Fl_Callback *)selectedsideright,this);

	sideleft=new Fl_Toggle_Button(120,70,40,17,"left");
	sideleft->labelfont(FL_HELVETICA);
	sideleft->labelsize(11);
	sideleft->callback((Fl_Callback *)selectedsideleft,this);

	sideboth=new Fl_Toggle_Button(160,70,40,17,"both");
	sideboth->labelfont(FL_HELVETICA);
	sideboth->labelsize(11);
	sideboth->callback((Fl_Callback *)selectedsideboth,this);

	sideassym=new Fl_Toggle_Button(200,70,40,17,"assym");
	sideassym->labelfont(FL_HELVETICA);
	sideassym->labelsize(11);
	sideassym->callback((Fl_Callback *)selectedsideassym,this);

	assymright=new Fl_Toggle_Button(80,90,60,17,"right arm");
	assymright->labelfont(FL_HELVETICA);
	assymright->labelsize(11);
	assymright->callback((Fl_Callback *)selectedassymright,this);

	assymleft=new Fl_Toggle_Button(140,90,60,17,"left arm");
	assymleft->labelfont(FL_HELVETICA);
	assymleft->labelsize(11);
	assymleft->callback((Fl_Callback *)selectedassymleft,this);

	assymswitch=new Fl_Button(200,90,40,17,"switch");
	assymswitch->labelfont(FL_HELVETICA);
	assymswitch->labelsize(11);
	assymswitch->callback((Fl_Callback *)selectedassymswitch,this);

	load=new Fl_Button(345,10,100,30,"load gesture");
	load->labelfont(FL_HELVETICA);
	load->labelsize(11);
	load->callback((Fl_Callback *)selectedload,this);

	save=new Fl_Button(445,10,100,30,"save gesture");
	save->labelfont(FL_HELVETICA);
	save->labelsize(11);
	save->callback((Fl_Callback *)selectedsave,this);

	testgesture=new Fl_Button(345,40,100,30,"test gesture");
	testgesture->labelfont(FL_HELVETICA);
	testgesture->labelsize(11);
	testgesture->callback((Fl_Callback *)selectedtestgesture,this);

	quit=new Fl_Button(445,40,100,30,"quit");
	quit->labelfont(FL_HELVETICA);
	quit->labelsize(11);
	quit->callback((Fl_Callback *)selectedquit,this);

	framenumber=new Fl_Output(120,120,30,17,"actual phase number:");
	framenumber->align(FL_ALIGN_LEFT);
	framenumber->labelfont(FL_HELVETICA);
	framenumber->labelsize(11);
	framenumber->textfont(FL_HELVETICA);
	framenumber->textsize(11);
	framenumber->value("1");

	prevframe=new Fl_Button(190,120,110,17,"@<< go previous phase");
	prevframe->labelfont(FL_HELVETICA);
	prevframe->labelsize(11);
	prevframe->callback((Fl_Callback *)selectedprevframe,this);

	nextframe=new Fl_Button(300,120,100,17,"go next phase @>>");
	nextframe->labelfont(FL_HELVETICA);
	nextframe->labelsize(11);
	nextframe->callback((Fl_Callback *)selectednextframe,this);

	insertframe=new Fl_Button(400,120,70,17,"insert phase");
	insertframe->labelfont(FL_HELVETICA);
	insertframe->labelsize(11);
	insertframe->callback((Fl_Callback *)selectedinsertframe,this);

	deleteframe=new Fl_Button(470,120,70,17,"delete phase");
	deleteframe->labelfont(FL_HELVETICA);
	deleteframe->labelsize(11);
	deleteframe->callback((Fl_Callback *)selecteddeleteframe,this);

	storedpose=new Fl_Check_Button(520,140,15,15,"pose from file");
	storedpose->align(FL_ALIGN_LEFT);
	storedpose->labelfont(FL_HELVETICA);
	storedpose->labelsize(11);
	storedpose->callback((Fl_Callback *)selectedstoredpose,this);

	posefilename=new Fl_Output(400,160,140,17,"");
	posefilename->textfont(FL_HELVETICA);
	posefilename->textsize(11);

	browseposefile=new Fl_Button(320,160,80,17,"browse");
	browseposefile->labelfont(FL_HELVETICA);
	browseposefile->labelsize(11);
	browseposefile->callback((Fl_Callback *)selectedbrowseposefile,this);

	frametype=new Fl_Choice(80,140,150,17,"phase type:");
	frametype->align(FL_ALIGN_LEFT);
	frametype->labelfont(FL_HELVETICA);
	frametype->labelsize(11);
	frametype->textfont(FL_HELVETICA);
	frametype->textsize(11);
	frametype->add("PHASETYPE_DEFAULT");
	frametype->add("PREPARATION");
	frametype->add("PRE_STROKE_HOLD");
	frametype->add("STROKE");
	frametype->add("STROKE_START");
	frametype->add("STROKE_END");
	frametype->add("POST_STROKE_HOLD");
	frametype->add("RETRACTION");
	frametype->value(3);
	frametype->callback((Fl_Callback *)selectedframetype,this);

	phasestartslider=new Fl_Slider(80,160,120,17,"starts at:");
	phasestartslider->align(FL_ALIGN_LEFT);
	phasestartslider->labelfont(FL_HELVETICA);
	phasestartslider->labelsize(11);
	phasestartslider->type(FL_HOR_NICE_SLIDER);
	phasestartslider->bounds(0,1);
	phasestartslider->step(0.01);
	phasestartslider->value(0.5);
	phasestartslider->callback((Fl_Callback *)selectedphasestartslider,this);

	phasestartoutput=new Fl_Output(200,160,40,17,"");
	phasestartoutput->textfont(FL_HELVETICA);
	phasestartoutput->textsize(11);
	phasestartoutput->value("0.5s");

	bmp_arm_x=new Fl_BMP_Image("gestedbmps/arm x.bmp");
	bmp_arm_y=new Fl_BMP_Image("gestedbmps/arm y.bmp");
	bmp_arm_z=new Fl_BMP_Image("gestedbmps/arm z.bmp");
	bmp_hand_form=new Fl_BMP_Image("gestedbmps/hand form.bmp");
	bmp_hand_symbol_open=new Fl_BMP_Image("gestedbmps/hand symbol open.bmp");
	bmp_hand_symbol_closed=new Fl_BMP_Image("gestedbmps/hand symbol closed.bmp");
	bmp_thumb_form=new Fl_BMP_Image("gestedbmps/thumb form.bmp");
	bmp_thumb_symbol_open=new Fl_BMP_Image("gestedbmps/thumb symbol open.bmp");
	bmp_thumb_symbol_closed=new Fl_BMP_Image("gestedbmps/thumb symbol closed.bmp");
	bmp_orientation=new Fl_BMP_Image("gestedbmps/orientation.bmp");
	
	int i;

	const char * arm_x_lookup[]={"XEP", "XP", "XC", "XCC", "XOPPC"};
	
	flgroup = new Fl_Group(82, 240, 30 * 4 + 15, 15);

	for(i=0;i<5;i++)
	{
		arm_x[i]=new Fl_Round_Button(82+30*i,240,15,15,"");
		arm_x[i]->type(FL_RADIO_BUTTON);
		arm_x[i]->labeltype(FL_NO_LABEL);
		arm_x[i]->label(arm_x_lookup[i]);
		arm_x[i]->callback((Fl_Callback *)selectedradio, this);
	}

	flgroup->end();

	fixed_x=new Fl_Check_Button(142,253,17,17,"fixed");
	fixed_x->align(FL_ALIGN_LEFT);
	fixed_x->labelfont(FL_HELVETICA);
	fixed_x->labelsize(11);
	fixed_x->callback((Fl_Callback *)selectedfixed_x, this);

	const char * arm_y_lookup[]={"YUPPEREP", "YUPPERP", "YUPPERC", "YCC", "YLOWERC", "YLOWERP", "YLOWEREP"};
	
	flgroup = new Fl_Group(232, 240, 20 * 6 + 15, 15);

	for(i=0;i<7;i++)
	{
		arm_y[i]=new Fl_Round_Button(232+20*i,240,15,15,"");
		arm_y[i]->type(FL_RADIO_BUTTON);
		arm_y[i]->labeltype(FL_NO_LABEL);
		arm_y[i]->label(arm_y_lookup[i]);
		arm_y[i]->callback((Fl_Callback *)selectedradio, this);
	}

	flgroup->end();

	fixed_y=new Fl_Check_Button(292,253,17,17,"fixed");
	fixed_y->align(FL_ALIGN_LEFT);
	fixed_y->labelfont(FL_HELVETICA);
	fixed_y->labelsize(11);
	fixed_y->callback((Fl_Callback *)selectedfixed_y,this);

	const char * arm_z_lookup[]={"ZNEAR", "ZMIDDLE", "ZFAR"};

	flgroup = new Fl_Group(392, 240, 48 * 2 + 15, 15);

	for(i=0;i<3;i++)
	{
		arm_z[i]=new Fl_Round_Button(392+48*i,240,15,15,"");
		arm_z[i]->type(FL_RADIO_BUTTON);
		arm_z[i]->labeltype(FL_NO_LABEL);
		arm_z[i]->label(arm_z_lookup[i]);
		arm_z[i]->callback((Fl_Callback *)selectedradio, this);
	}

	flgroup->end();

	fixed_z=new Fl_Check_Button(440,253,17,17,"fixed");
	fixed_z->align(FL_ALIGN_LEFT);
	fixed_z->labelfont(FL_HELVETICA);
	fixed_z->labelsize(11);
	fixed_z->callback((Fl_Callback *)selectedfixed_z,this);

	const char * hand_form_lookup[]={"FORM_FIST", "FORM_OPEN", "FORM_POINT1", "FORM_POINT2", "FORM_2APART", "FORM_OPENAPART"};
	
	flgroup = new Fl_Group(82, 315, 30 * 5 + 15, 15);

	for(i=0;i<6;i++)
	{
		hand_form[i]=new Fl_Round_Button(82+30*i,315,15,15,"");
		hand_form[i]->type(FL_RADIO_BUTTON);
		hand_form[i]->labeltype(FL_NO_LABEL);
		hand_form[i]->label(hand_form_lookup[i]);
		hand_form[i]->callback((Fl_Callback *)selectedradio, this);
	}

	flgroup->end();

	fixed_form=new Fl_Check_Button(290,300,17,17,"fixed");
	fixed_form->align(FL_ALIGN_LEFT);
	fixed_form->labelfont(FL_HELVETICA);
	fixed_form->labelsize(11);
	fixed_form->callback((Fl_Callback *)selectedfixed_form,this);

	const char * thumb_form_lookup[]={"THUMB_DEFAULT", "THUMB_AWAY", "THUMB_OVER"};

	flgroup = new Fl_Group(362, 309, 30 * 2 + 15, 15);

	for(i=0;i<3;i++)
	{
		thumb_form[i]=new Fl_Round_Button(362+30*i,309,15,15,"");
		thumb_form[i]->type(FL_RADIO_BUTTON);
		thumb_form[i]->labeltype(FL_NO_LABEL);
		thumb_form[i]->label(thumb_form_lookup[i]);
		thumb_form[i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	const char * thumb_symbol_open_lookup[]={"OPEN_DEFAULT", "OPEN_THUMBOUT", "OPEN_THUMBIN"};

	flgroup = new Fl_Group(362, 371, 30 * 2 + 15, 15);

	for(i=0;i<3;i++)
	{
		thumb_symbol_open[i]=new Fl_Round_Button(362+30*i,371,15,15,"");
		thumb_symbol_open[i]->type(FL_RADIO_BUTTON);
		thumb_symbol_open[i]->labeltype(FL_NO_LABEL);
		thumb_symbol_open[i]->label(thumb_symbol_open_lookup[i]);
		thumb_symbol_open[i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	const char * thumb_symbol_closed_lookup[]={"CLOSED_DEFAULT", "CLOSED_STRAIGHT", "CLOSED_INSIDE", "CLOSED_TIGHT"};

	flgroup = new Fl_Group(362, 433, 42 * 3 + 15, 15);

	for(i=0;i<4;i++)
	{
		thumb_symbol_closed[i]=new Fl_Round_Button(362+42*i,433,15,15,"");
		thumb_symbol_closed[i]->type(FL_RADIO_BUTTON);
		thumb_symbol_closed[i]->labeltype(FL_NO_LABEL);
		thumb_symbol_closed[i]->label(thumb_symbol_closed_lookup[i]);
		thumb_symbol_closed[i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	const char * hand_symbol_open_lookup[]={"SYMBOL_1_OPEN", "SYMBOL_2_OPEN", "SYMBOL_3_OPEN"};
	
	flgroup = new Fl_Group(85, 373, 42 * 2 + 15, 15);

	for(i=0;i<3;i++)
	{
		hand_symbol_open[i]=new Fl_Round_Button(85+42*i,373,15,15,"");
		hand_symbol_open[i]->type(FL_RADIO_BUTTON);
		hand_symbol_open[i]->labeltype(FL_NO_LABEL);
		hand_symbol_open[i]->label(hand_symbol_open_lookup[i]);
		hand_symbol_open[i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	const char * hand_symbol_closed_lookup[]={"SYMBOL_1_CLOSED", "SYMBOL_2_CLOSED", "SYMBOL_3_CLOSED"};

	flgroup = new Fl_Group(85, 433, 42 * 2 + 15, 15);

	for(i=0;i<3;i++)
	{
		hand_symbol_closed[i]=new Fl_Round_Button(85+42*i,433,15,15,"");
		hand_symbol_closed[i]->type(FL_RADIO_BUTTON);
		hand_symbol_closed[i]->labeltype(FL_NO_LABEL);
		hand_symbol_closed[i]->label(hand_symbol_closed_lookup[i]);
		hand_symbol_closed[i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();
	flgroup = new Fl_Group(82, 503, 36 * 5 + 15, 15);

	const char * orientation_palm_lookup[]={/*"PALMDEFAULT", */"PALMUP", "PALMDOWN", "PALMINWARDS", "PALMOUTWARDS", "PALMAWAY", "PALMTOWARDS"};

	for(i=0;i<6;i++)
	{
		orientation_palm[0][i]=new Fl_Round_Button(82+36*i,503,15,15,"");
		orientation_palm[0][i]->type(FL_RADIO_BUTTON);
		orientation_palm[0][i]->labeltype(FL_NO_LABEL);
		orientation_palm[0][i]->label(orientation_palm_lookup[i]);
		orientation_palm[0][i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();
	flgroup = new Fl_Group(82, 523, 36 * 5 + 15, 15);

	const char * orientation_fingers_lookup[]={"FBUP", "FBDOWN", "FBINWARDS", "FBOUTWARDS", "FBAWAY", "FBTOWARDS"};

	for(i=0;i<6;i++)
	{
		orientation_fingers[0][i]=new Fl_Round_Button(82+36*i,523,15,15,"");
		orientation_fingers[0][i]->type(FL_RADIO_BUTTON);
		orientation_fingers[0][i]->labeltype(FL_NO_LABEL);
		orientation_fingers[0][i]->label(orientation_fingers_lookup[i]);
		orientation_fingers[0][i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	const char * orientation_palm_to_lookup[]={/*"PALMDEFAULT:TO", */"PALMUP:TO", "PALMDOWN:TO", "PALMINWARDS:TO", "PALMOUTWARDS:TO", "PALMAWAY:TO", "PALMTOWARDS:TO"};
	
	flgroup = new Fl_Group(342, 503, 36 * 5 + 15, 15);

	for(i=0;i<6;i++)
	{
		orientation_palm[1][i]=new Fl_Round_Button(342+36*i,503,15,15,"");
		orientation_palm[1][i]->type(FL_RADIO_BUTTON);
		orientation_palm[1][i]->labeltype(FL_NO_LABEL);
		orientation_palm[1][i]->label(orientation_palm_to_lookup[i]);
		orientation_palm[1][i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	const char * orientation_fingers_to_lookup[]={"FBUP:TO", "FBDOWN:TO", "FBINWARDS:TO", "FBOUTWARDS:TO", "FBAWAY:TO", "FBTOWARDS:TO"};

	flgroup = new Fl_Group(342, 523, 36 * 5 + 15, 15);

	for(i=0;i<6;i++)
	{
		orientation_fingers[1][i]=new Fl_Round_Button(342+36*i,523,15,15,"");
		orientation_fingers[1][i]->type(FL_RADIO_BUTTON);
		orientation_fingers[1][i]->labeltype(FL_NO_LABEL);
		orientation_fingers[1][i]->label(orientation_fingers_to_lookup[i]);
		orientation_fingers[1][i]->callback((Fl_Callback *)selectedradio,this);
	}

	flgroup->end();

	//flgroup = new Fl_Group(295, 485, 310 - 295 + 15, 505 - 485 + 15);

	wristmix=new Fl_Output(295,485,30,17,"");
	wristmix->textfont(FL_HELVETICA);
	wristmix->textsize(11);

	wristmixminus=new Fl_Button(295,505,15,15,"-");
	wristmixminus->labelfont(FL_HELVETICA);
	wristmixminus->labelsize(13);
	wristmixminus->callback((Fl_Callback *)selectedwristmixminus,this);

	wristmixplus=new Fl_Button(310,505,15,15,"+");
	wristmixplus->labelfont(FL_HELVETICA);
	wristmixplus->labelsize(13);
	wristmixplus->callback((Fl_Callback *)selectedwristmixplus,this);

	//flgroup->end();

	flgroup = new Fl_Group(30, 300, 15, 120 + 15);

	const char * shape_lookup[]={"SHAPE_FORM", "SHAPE_SYMBOL"};

	for(i=0;i<2;i++)
	{
		shape[i]=new Fl_Round_Button(30,300 + 60 * i,15,15,"");
		shape[i]->type(FL_RADIO_BUTTON);
		shape[i]->labeltype(FL_NO_LABEL);
		shape[i]->label(shape_lookup[i]);
		shape[i]->callback((Fl_Callback *)selectedradio, this);
	}

	flgroup->end();

	this->callback((Fl_Callback *)GestureEditorWindowclosed,this);

	}
	
	{ //transition:
	use_transition = new Fl_Check_Button(15,553,17,17,"movement to the next phase:");
	use_transition->align(FL_ALIGN_RIGHT);
	use_transition->labelfont(FL_HELVETICA);
	use_transition->labelsize(11);
	use_transition->callback((Fl_Callback *)selected_use_transition, this);

	const char * mov_Types[]={"Wave", "Circle", "Increasing Spiral", "Decreasing Spiral", "Custom"};
	flgroup = new Fl_Group(15, 555, 17, 110);
	movement_type[4]=new Fl_Round_Button(15,650,17,17);
	for(int i=0;i<5;i++){
		if(i<4)
			movement_type[i]=new Fl_Round_Button(15,570+i*15,17,17);
		movement_type[i]->type(FL_RADIO_BUTTON);
		movement_type[i]->align(FL_ALIGN_RIGHT);
		movement_type[i]->labelfont(FL_HELVETICA);
		movement_type[i]->labelsize(11);
		movement_type[i]->label(mov_Types[i]);
	}
	movement_type[0]->callback((Fl_Callback *)selected_wave, this);
	movement_type[1]->callback((Fl_Callback *)selected_circle, this);
	movement_type[2]->callback((Fl_Callback *)selected_spiral_out, this);
	movement_type[3]->callback((Fl_Callback *)selected_spiral_in, this);
	movement_type[4]->callback((Fl_Callback *)selected_custom, this);
	flgroup->end();
	
	amplitude=new Fl_Slider(160,585,135,10,"Amplitude(0-30)");
	amplitude->type(FL_HOR_NICE_SLIDER);
	amplitude->align(FL_ALIGN_TOP);
	amplitude->labelfont(FL_HELVETICA);
	amplitude->labelsize(11);
	amplitude->bounds(0,30);
	amplitude->step(1);
	amplitude->value(0.0);
	amplitude->callback((Fl_Callback *)selected_amplitude_slider,this);

	frequency=new Fl_Slider(160,610,135,10,"Frequency(0-4)");
	frequency->type(FL_HOR_NICE_SLIDER);
	frequency->align(FL_ALIGN_TOP);
	frequency->labelfont(FL_HELVETICA);
	frequency->labelsize(11);
	frequency->bounds(0,4);
	frequency->step(0.1);
	frequency->value(0.0);
	frequency->callback((Fl_Callback *)selected_frequency_slider,this);

	inverse_way = new Fl_Check_Button(160,625,17,17,"Inverse way");
	inverse_way->align(FL_ALIGN_RIGHT);
	inverse_way->labelfont(FL_HELVETICA);
	inverse_way->labelsize(11);
	inverse_way->callback((Fl_Callback *)selected_way, this);

	const char * direction[]={"up", "down", "intern", "extern"};
	flgroup = new Fl_Group(330, 560, 17, 80);
	for(int i=0;i<4;i++){
		direction_start[i]=new Fl_Round_Button(330,560+i*20,17,17);
		direction_start[i]->type(FL_RADIO_BUTTON);
		direction_start[i]->align(FL_ALIGN_RIGHT);
		direction_start[i]->labelfont(FL_HELVETICA);
		direction_start[i]->labelsize(11);
		direction_start[i]->label(direction[i]);
	}
	direction_start[0]->callback((Fl_Callback *)selected_up, this);
	direction_start[1]->callback((Fl_Callback *)selected_down, this);
	direction_start[2]->callback((Fl_Callback *)selected_intern, this);
	direction_start[3]->callback((Fl_Callback *)selected_extern, this);
	flgroup->end();

	const char * planes[]={"xy", "xz", "yz"};
	flgroup = new Fl_Group(460, 550, 17, 115);
	for(int i=0;i<3;i++){
		plane[i]=new Fl_Round_Button(460,550+i*35,17,17);
		plane[i]->type(FL_RADIO_BUTTON);
		plane[i]->align(FL_ALIGN_LEFT);
		plane[i]->labelfont(FL_HELVETICA);
		plane[i]->labelsize(11);
		plane[i]->label(planes[i]);
	}
	plane[0]->callback((Fl_Callback *)selected_XY, this);
	plane[1]->callback((Fl_Callback *)selected_XZ, this);
	plane[2]->callback((Fl_Callback *)selected_YZ, this);
	flgroup->end();

	int h = 675;
	int l = 135;
	x_amplitude=new Fl_Slider(525-3*l,h,l,10,"x");
	y_amplitude=new Fl_Slider(530-2*l,h,l,10,"y");
	z_amplitude=new Fl_Slider(535-l,h,l,10,"z");

	x_frequency=new Fl_Slider(525-3*l,h+15,l,10);
	y_frequency=new Fl_Slider(530-2*l,h+15,l,10);
	z_frequency=new Fl_Slider(535-l,h+15,l,10);

	x_shift=new Fl_Slider(525-3*l,h+30,l,10);
	y_shift=new Fl_Slider(530-2*l,h+30,l,10);
	z_shift=new Fl_Slider(535-l,h+30,l,10);

	x_space_variation=new Fl_Slider(525-3*l,h+50,l,10);
	y_space_variation=new Fl_Slider(530-2*l,h+50,l,10);
	z_space_variation=new Fl_Slider(535-l,h+50,l,10);

	x_temp_variation=new Fl_Slider(525-3*l,h+65,l,10);
	y_temp_variation=new Fl_Slider(530-2*l,h+65,l,10);
	z_temp_variation=new Fl_Slider(535-l,h+65,l,10);

	{
	x_amplitude->type(FL_HOR_NICE_SLIDER);
	x_amplitude->align(FL_ALIGN_TOP);
	x_amplitude->labelfont(FL_HELVETICA);
	x_amplitude->labelsize(11);
	x_amplitude->bounds(0,30);
	x_amplitude->step(1);
	x_amplitude->value(0.0);
	x_amplitude->callback((Fl_Callback *)selected_x_amplitude_slider,this);
	
	y_amplitude->type(FL_HOR_NICE_SLIDER);
	y_amplitude->align(FL_ALIGN_TOP);
	y_amplitude->labelfont(FL_HELVETICA);
	y_amplitude->labelsize(11);
	y_amplitude->bounds(0,30);
	y_amplitude->step(1);
	y_amplitude->value(0.0);
	y_amplitude->callback((Fl_Callback *)selected_y_amplitude_slider,this);
	
	z_amplitude->type(FL_HOR_NICE_SLIDER);
	z_amplitude->align(FL_ALIGN_TOP);
	z_amplitude->labelfont(FL_HELVETICA);
	z_amplitude->labelsize(11);
	z_amplitude->bounds(0,30);
	z_amplitude->step(1);
	z_amplitude->value(0.0);
	z_amplitude->callback((Fl_Callback *)selected_z_amplitude_slider,this);
	}
	{
	x_frequency->type(FL_HOR_NICE_SLIDER);
	x_frequency->bounds(0,4);
	x_frequency->step(0.1);
	x_frequency->value(0.0);
	x_frequency->callback((Fl_Callback *)selected_x_frequency_slider,this);

	y_frequency->type(FL_HOR_NICE_SLIDER);
	y_frequency->bounds(0,4);
	y_frequency->step(0.1);
	y_frequency->value(0.0);
	y_frequency->callback((Fl_Callback *)selected_y_frequency_slider,this);
	
	z_frequency->type(FL_HOR_NICE_SLIDER);
	z_frequency->bounds(0,4);
	z_frequency->step(0.1);
	z_frequency->value(0.0);
	z_frequency->callback((Fl_Callback *)selected_z_frequency_slider,this);
	}
	{
	x_shift->type(FL_HOR_NICE_SLIDER);
	x_shift->bounds(0,1);
	x_shift->step(0.05);
	x_shift->value(0.0);
	x_shift->callback((Fl_Callback *)selected_x_shift_slider,this);
	
	y_shift->type(FL_HOR_NICE_SLIDER);
	y_shift->bounds(0,1);
	y_shift->step(0.05);
	y_shift->value(0.0);
	y_shift->callback((Fl_Callback *)selected_y_shift_slider,this);
	
	z_shift->type(FL_HOR_NICE_SLIDER);
	z_shift->bounds(0,1);
	z_shift->step(0.05);
	z_shift->value(0.0);
	z_shift->callback((Fl_Callback *)selected_z_shift_slider,this);
	}
	{
	x_space_variation->type(FL_HOR_NICE_SLIDER);
	x_space_variation->bounds(-1,1);
	x_space_variation->step(1);
	x_space_variation->value(0.0);
	x_space_variation->callback((Fl_Callback *)selected_x_space_variation_slider,this);
	
	y_space_variation->type(FL_HOR_NICE_SLIDER);
	y_space_variation->bounds(-1,1);
	y_space_variation->step(1);
	y_space_variation->value(0.0);
	y_space_variation->callback((Fl_Callback *)selected_y_space_variation_slider,this);
	
	z_space_variation->type(FL_HOR_NICE_SLIDER);
	z_space_variation->bounds(-1,1);
	z_space_variation->step(1);
	z_space_variation->value(0.0);
	z_space_variation->callback((Fl_Callback *)selected_z_space_variation_slider,this);
	}

	{
	x_temp_variation->type(FL_HOR_NICE_SLIDER);
	x_temp_variation->bounds(-1,1);
	x_temp_variation->step(1);
	x_temp_variation->value(0.0);
	x_temp_variation->callback((Fl_Callback *)selected_x_temp_variation_slider,this);
	
	y_temp_variation->type(FL_HOR_NICE_SLIDER);
	y_temp_variation->bounds(-1,1);
	y_temp_variation->step(1);
	y_temp_variation->value(0.0);
	y_temp_variation->callback((Fl_Callback *)selected_y_temp_variation_slider,this);
	
	z_temp_variation->type(FL_HOR_NICE_SLIDER);
	z_temp_variation->bounds(-1,1);
	z_temp_variation->step(1);
	z_temp_variation->value(0.0);
	z_temp_variation->callback((Fl_Callback *)selected_z_temp_variation_slider,this);
	}
	bmp_planes = new Fl_BMP_Image("gestedbmps/planes.bmp");

	}

	{
	gesture=new Gesture();
	phasesVector = &(gesture->phases);

	//ADDME: should we really start with 1 phase?
	GesturePhase *phase=new GesturePhase();
	phase->Start();
	phase->Finalize();
	phasesVector->push_back(phase);

	phase=new GesturePhase();
	phase->Start();
	phase->Finalize();
	gesture->phasesAssym.push_back(phase);

	actualframe=0;

	ShowActualFrame();
	ShowActualSide();

	inimanager.ReadIniFile("greta.ini");

	agentwindow=0;
	}
}

GestureEditorWindow::~GestureEditorWindow()
{
	delete agentwindow;
	delete expressivitywindow;
}

int GestureEditorWindow::handle(int e)
{

/*FIXME
	if(Fl::event_inside(durationminus))
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(e==FL_PUSH)
			{
				if(gesture->GetDuration()>0.05)
				{
					char d[10];
					gesture->SetDuration(gesture->GetDuration()-0.05);
					sprintf(d,"%.2fs",gesture->GetDuration());
					duration->value(d);
				}
			}

	if(Fl::event_inside(durationplus))
		if(Fl::event_button()==FL_LEFT_MOUSE)
			if(e==FL_PUSH)
			{
				char d[10];
				gesture->SetDuration(gesture->GetDuration()-0.05);
				sprintf(d,"%.2fs",gesture->GetDuration());
				duration->value(d);
			}

*/	

#if 0

	//FIXME: add support for orientation mixing again.. AddWristMagnitude, AddWrist(string sFb, string sP, bool to)
	//for(k=0;k<2;k++)
	{
		for(i=0;i<6;i++)
		{
			if(Fl::event_inside(orientation_palm[k][i]))
				if(Fl::event_button()==FL_LEFT_MOUSE)
					if(e==FL_PUSH)
					{
						for(int j=0;j<6;j++)
						{
							orientation_fingers[k][j]->activate();
						}
						/* FIXME if((*phasesVector)[actualframe]->palm_int[k]==i)
						{
							//(*phasesVector)[actualframe]->palm_int[k]=-1;
							(*phasesVector)[actualframe]->Add("PALMDEFAULT");
						}
						else*/
						{
							//(*phasesVector)[actualframe]->palm_int[k]=i;
							if(i==0)
							{
								(*phasesVector)[actualframe]->Add("PALMUP");
								orientation_fingers[k][0]->deactivate();
								orientation_fingers[k][1]->deactivate();
								/*(if(((*phasesVector)[actualframe]->fingersbase_int[k]==0)||((*phasesVector)[actualframe]->fingersbase_int[k]==1))
								{
									(*phasesVector)[actualframe]->fingersbase_int[k]=-1;
									(*phasesVector)[actualframe]->Add("FBDEFAULT");
								}*/
							}
							if(i==1)
							{
								(*phasesVector)[actualframe]->Add("PALMDOWN");
								orientation_fingers[k][0]->deactivate();
								orientation_fingers[k][1]->deactivate();
								/*if(((*phasesVector)[actualframe]->fingersbase_int[k]==0)||((*phasesVector)[actualframe]->fingersbase_int[k]==1))
								{
									(*phasesVector)[actualframe]->fingersbase_int[k]=-1;
									(*phasesVector)[actualframe]->Add("FBDEFAULT")
								}*/
							}
							if(i==2)
							{
								(*phasesVector)[actualframe]->Add("PALMINWARDS");
								orientation_fingers[k][2]->deactivate();
								orientation_fingers[k][3]->deactivate();
								/*if(((*phasesVector)[actualframe]->fingersbase_int[k]==2)||((*phasesVector)[actualframe]->fingersbase_int[k]==3))
								{
									(*phasesVector)[actualframe]->fingersbase_int[k]=-1;
									(*phasesVector)[actualframe]->Add("FBDEFAULT");
								}*/
							}
							if(i==3)
							{
								(*phasesVector)[actualframe]->Add("PALMOUTWARDS");
								orientation_fingers[k][2]->deactivate();
								orientation_fingers[k][3]->deactivate();
								/*if(((*phasesVector)[actualframe]->fingersbase_int[k]==2)||((*phasesVector)[actualframe]->fingersbase_int[k]==3))
								{
									(*phasesVector)[actualframe]->fingersbase_int[k]=-1;
									(*phasesVector)[actualframe]->Add("FBDEFAULT");
								}*/
							}
							if(i==4)
							{
								(*phasesVector)[actualframe]->Add("PALMAWAY");
								orientation_fingers[k][4]->deactivate();
								orientation_fingers[k][5]->deactivate();
								/*if(((*phasesVector)[actualframe]->fingersbase_int[k]==4)||((*phasesVector)[actualframe]->fingersbase_int[k]==5))
								{
									(*phasesVector)[actualframe]->fingersbase_int[k]=-1;
									(*phasesVector)[actualframe]->Add("FBDEFAULT");
								}*/
							}
							if(i==5)
							{
								(*phasesVector)[actualframe]->Add("PALMTOWARDS");
								orientation_fingers[k][4]->deactivate();
								orientation_fingers[k][5]->deactivate();
								/*if(((*phasesVector)[actualframe]->fingersbase_int[k]==4)||((*phasesVector)[actualframe]->fingersbase_int[k]==5))
								{
									(*phasesVector)[actualframe]->fingersbase_int[k]=-1;
									(*phasesVector)[actualframe]->Add("FBDEFAULT");
								}*/
							}
						}
						ShowActualFrame();
						return 1;
					}
		}
	}


#endif

	return Fl_Window::handle(e);
}

void GestureEditorWindow::draw()
{
	{
	Fl_Window::draw();

	if(agentwindow==0)
	{
		agentwindow=new GLContainer();
		agentwindow->show();
		//agentwindow->clear_border();

		expressivitywindow=new ExpressivityWindow();
		expressivitywindow->show();

 
		expressivitywindow->SPC->value(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
		expressivitywindow->TMP->value(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));
		expressivitywindow->FLD->value(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));
		expressivitywindow->PWR->value(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));
	}

	
	fl_font(FL_HELVETICA,11);
	fl_color(0,0,0);
	fl_draw("gesture side:",14,82);
	fl_draw("assymetric:",14,102);

	fl_rect(440,90,105,21);
	fl_rect(5,110,540,655);
	fl_draw("gesture phases",455,104);

	char pn[10];
	sprintf(pn,"of %d",phasesVector->size());
	fl_draw(pn,152,132);

	fl_draw("arm position:",14,212);
	bmp_arm_x->draw(80,190);
	bmp_arm_y->draw(230,190);
	bmp_arm_z->draw(380,190);

	fl_rect(10,275,530,55);
	fl_draw("hand shape:",14,292);
	bmp_hand_form->draw(80,280);
	fl_draw("thumb shape:",294,292);
	bmp_thumb_form->draw(360,280);

	fl_rect(10,335,530,115);
	fl_draw("hand shape:",14,352);
	bmp_hand_symbol_open->draw(80,340);
	fl_draw("thumb shape:",294,352);
	bmp_thumb_symbol_open->draw(360,340);

	//fl_rect(10,395,530,55);
	//fl_draw("hand shape:",14,412);
	bmp_hand_symbol_closed->draw(80,400);
	//fl_draw("thumb shape:",294,412);
	bmp_thumb_symbol_closed->draw(360,400);

	fl_draw("hand",50,472);
	fl_draw("orientation:",22,486);
	bmp_orientation->draw(80,460);
	fl_draw("palm:",50,516);
	fl_draw("fingers:",38,535);

	fl_font(FL_COURIER,16);
	fl_draw("->",300,480);
	bmp_orientation->draw(340,460);
	fl_rect(10,455,530,90);
	}
	fl_font(FL_HELVETICA,11);
	//fl_draw("movement to the next phase:",35,565);
	bmp_planes->draw(474,552);
	//fl_draw("plane:",450,580);
	int h = 683;
	fl_draw("Amplitude (0-30):",30,h);
	fl_draw("Frequency (0-4):",30,h+15);
	fl_draw("Shifting (0-1):",30,h+30);
	fl_draw("Space Variation :",30,h+50); //(smaller to greater)
	fl_draw("Time Variation :",30,h+65); //(slacken to quicken)

	fl_line(35,645,539,645);
	fl_rect(10,550,530,210);
}

void GestureEditorWindow::ShowActualSide(){
	if(gesture->GetSide()==r) sideright->value(1);
	else sideright->value(0);
	if(gesture->GetSide()==l) sideleft->value(1);
	else sideleft->value(0);
	if(gesture->GetSide()==both_sides)sideboth->value(1);
	else sideboth->value(0);
	if(gesture->GetSide()==assym)sideassym->value(1);
	else {
		sideassym->value(0);
		assymright->value(0);
		assymleft->value(0);
		if(!firstArm){
			phasesVector = &(gesture->phases);
			firstArm = true;
			actualframe = 0;
			ShowActualFrame();
		}
	}
}
void GestureEditorWindow::ShowActualTransition(){
	CurvedTransition* ct = GetActualTransition();
	movement_type[0]->value(0);
	movement_type[1]->value(0);
	movement_type[2]->value(0);
	movement_type[3]->value(0);
	movement_type[4]->value(0);
	plane[0]->value(0);
	plane[1]->value(0);
	plane[2]->value(0);
	direction_start[0]->value(0);
	direction_start[1]->value(0);
	direction_start[2]->value(0);
	direction_start[3]->value(0);
	if(ct == NULL){
		use_transition->value(0);
		movement_type[0]->deactivate();
		movement_type[1]->deactivate();
		movement_type[2]->deactivate();
		movement_type[3]->deactivate();
		movement_type[4]->deactivate();
		plane[0]->deactivate();
		plane[1]->deactivate();
		plane[2]->deactivate();
		direction_start[0]->deactivate();
		direction_start[1]->deactivate();
		direction_start[2]->deactivate();
		direction_start[3]->deactivate();
		amplitude->deactivate();
		frequency->deactivate();
		inverse_way->deactivate();
		x_amplitude->deactivate();
		y_amplitude->deactivate();
		z_amplitude->deactivate();
		x_frequency->deactivate();
		y_frequency->deactivate();
		z_frequency->deactivate();
		x_shift->deactivate();
		y_shift->deactivate();
		z_shift->deactivate();
		x_space_variation->deactivate();
		y_space_variation->deactivate();
		z_space_variation->deactivate();
		x_temp_variation->deactivate();
		y_temp_variation->deactivate();
		z_temp_variation->deactivate();
		amplitude->value(0);
		frequency->value(0);
		inverse_way->value(0);
		x_amplitude->value(0);
		y_amplitude->value(0);
		z_amplitude->value(0);
		x_frequency->value(0);
		y_frequency->value(0);
		z_frequency->value(0);
		x_shift->value(0);
		y_shift->value(0);
		z_shift->value(0);
		x_space_variation->value(0);
		y_space_variation->value(0);
		z_space_variation->value(0);
		x_temp_variation->value(0);
		y_temp_variation->value(0);
		z_temp_variation->value(0);
	}
	else{
		use_transition->value(1);
		movement_type[0]->activate();
		movement_type[1]->activate();
		movement_type[2]->activate();
		movement_type[3]->activate();
		movement_type[4]->activate();
		movement_type[ct->getMovement()]->value(1);
		plane[ct->getOrientation()]->value(1);
		direction_start[ct->getStartDirection()]->value(1);
		amplitude->value(ct->getAmplitudeOnPlane());
		frequency->value(ct->getFrequencyOnPlane());
		inverse_way->value(ct->getInverseWay());
		x_amplitude->value(ct->getAmplitude(0));
		y_amplitude->value(ct->getAmplitude(1));
		z_amplitude->value(ct->getAmplitude(2));
		x_frequency->value(ct->getFrequency(0));
		y_frequency->value(ct->getFrequency(1));
		z_frequency->value(ct->getFrequency(2));
		x_shift->value(ct->getShift(0));
		y_shift->value(ct->getShift(1));
		z_shift->value(ct->getShift(2));
		x_space_variation->value(ct->getSpatialVariation(0));
		y_space_variation->value(ct->getSpatialVariation(1));
		z_space_variation->value(ct->getSpatialVariation(2));
		x_temp_variation->value(ct->getTemporalVariation(0));
		y_temp_variation->value(ct->getTemporalVariation(1));
		z_temp_variation->value(ct->getTemporalVariation(2));
		if(ct->getMovement()==CUSTOM){
			plane[0]->deactivate();
			plane[1]->deactivate();
			plane[2]->deactivate();
			direction_start[0]->deactivate();
			direction_start[1]->deactivate();
			direction_start[2]->deactivate();
			direction_start[3]->deactivate();
			amplitude->deactivate();
			frequency->deactivate();
			inverse_way->deactivate();
			x_amplitude->activate();
			y_amplitude->activate();
			z_amplitude->activate();
			x_frequency->activate();
			y_frequency->activate();
			z_frequency->activate();
			x_shift->activate();
			y_shift->activate();
			z_shift->activate();
			x_space_variation->activate();
			y_space_variation->activate();
			z_space_variation->activate();
			x_temp_variation->activate();
			y_temp_variation->activate();
			z_temp_variation->activate();
		}
		else{
			plane[0]->activate();
			plane[1]->activate();
			plane[2]->activate();
			direction_start[0]->activate();
			direction_start[1]->activate();
			amplitude->activate();
			frequency->activate();
			x_amplitude->deactivate();
			y_amplitude->deactivate();
			z_amplitude->deactivate();
			x_frequency->deactivate();
			y_frequency->deactivate();
			z_frequency->deactivate();
			x_shift->deactivate();
			y_shift->deactivate();
			z_shift->deactivate();
			x_space_variation->deactivate();
			y_space_variation->deactivate();
			z_space_variation->deactivate();
			x_temp_variation->deactivate();
			y_temp_variation->deactivate();
			z_temp_variation->deactivate();
			if(ct->getMovement()!=WAVE){
				direction_start[2]->activate();
				direction_start[3]->activate();
				inverse_way->activate();
				plane[0]->label("xy");
				plane[1]->label("xz");
				plane[2]->label("yz");
			}
			else{
				direction_start[2]->deactivate();
				direction_start[3]->deactivate();
				inverse_way->deactivate();
				plane[0]->label("y  ");
				plane[1]->label("x  ");
				plane[2]->label("z  ");
			}
		}
	}
}
void GestureEditorWindow::ShowActualFrame()
{
	int i,j;
	char t[10];

	phasestartslider->value((*phasesVector)[actualframe]->time);
	sprintf(t,"%.2f",phasestartslider->value());
	phasestartoutput->value(t);

	sprintf(t,"%d",actualframe+1);
	framenumber->value(t);

	gestureinstance->value(gesture->GetInstance().c_str());
	gestureclass->value(gesture->GetClass().c_str());

	frametype->value((*phasesVector)[actualframe]->type);


	//posefilename->value((*phasesVector)[actualframe]->GetStoredPose().c_str());
	
	//	int bla = (*phasesVector)[actualframe]->GetPArm()->AbstractX;

	//arm_x[(*phasesVector)[actualframe]->GetPArm()->AbstractX]->setonly();

	for(i=0;i<5;i++)
		arm_x[i]->value(0);

	for(i=0;i<7;i++)
		arm_y[i]->value(0);

	for(i=0;i<3;i++)
		arm_z[i]->value(0);

	for(i=0;i<6;i++)
		hand_form[i]->value(0);

	for(i=0;i<3;i++)
		thumb_form[i]->value(0);

	for(i=0;i<3;i++)
		hand_symbol_open[i]->value(0);

	for(i=0;i<3;i++)
		thumb_symbol_open[i]->value(0);

	for(i=0;i<3;i++)
		hand_symbol_closed[i]->value(0);

	for(i=0;i<4;i++)
		thumb_symbol_closed[i]->value(0);

	for(j=0;j<2;j++)
		for(i=0;i<6;i++)
		{
			orientation_palm[j][i]->value(0);
			orientation_fingers[j][i]->value(0);
		}

	//if((*phasesVector)[actualframe]->GetPHand())
	{
		if((*phasesVector)[actualframe]->GetPHand()->type==shape_form)
		{
			for(i=0;i<6;i++)
				hand_form[i]->activate();

			for(i=0;i<3;i++)
				thumb_form[i]->activate();
		}
		else
		{
			for(i=0;i<6;i++)
				hand_form[i]->deactivate();

			for(i=0;i<3;i++)
				thumb_form[i]->deactivate();
		}

		if((*phasesVector)[actualframe]->GetPHand()->type==shape_symbol)
		{
			for(i=0;i<3;i++)
				hand_symbol_open[i]->activate();

			for(i=0;i<3;i++)
				thumb_symbol_open[i]->activate();
		}
		else
		{
			for(i=0;i<3;i++)
				hand_symbol_open[i]->deactivate();

			for(i=0;i<3;i++)
				thumb_symbol_open[i]->deactivate();
		}

		if((*phasesVector)[actualframe]->GetPHand()->type==shape_symbol)
		{
			for(i=0;i<3;i++)
				hand_symbol_closed[i]->activate();

			for(i=0;i<4;i++)
				thumb_symbol_closed[i]->activate();
		}
		else
		{
			for(i=0;i<3;i++)
				hand_symbol_closed[i]->deactivate();

			for(i=0;i<4;i++)
				thumb_symbol_closed[i]->deactivate();
		}


		//i = (*phasesVector)[actualframe]->GetPHand()->type;
		//printf("hand type is %d\n",(*phasesVector)[actualframe]->GetPHand()->type);
		for(i=0;i<2;i++)
		{
			if((*phasesVector)[actualframe]->GetPHand()->type==(i+1))
				shape[i]->value(1);
			else
				shape[i]->value(0);
		}

		if((*phasesVector)[actualframe]->GetPHand()->type == shape_form)
		{
			i = ((BasicForm*)((*phasesVector)[actualframe]->GetPHand()->shape))->type - 1;
			if(0 <= i && i < 6)
				hand_form[i]->value(1);

			i = ((BasicForm*)((*phasesVector)[actualframe]->GetPHand()->shape))->thumb;
			if(0 <= i && i < 3)
				thumb_form[i]->value(1);
		}
		
		if((*phasesVector)[actualframe]->GetPHand()->type == shape_symbol)
		{
			

			i = ((BasicSymbol*)((*phasesVector)[actualframe]->GetPHand()->shape))->type - 1;
			if(0 <= i && i < 3)
				hand_symbol_open[i]->value(1);

			i -= 3;
			if(0 <= i && i < 3)
				hand_symbol_closed[i]->value(1);


			i = ((BasicSymbol*)((*phasesVector)[actualframe]->GetPHand()->shape))->opentype;

			if(0 <= i && i < 3)
				thumb_symbol_open[i]->value(1);

			i = ((BasicSymbol*)((*phasesVector)[actualframe]->GetPHand()->shape))->closedtype;

			if(0 <= i && i < 4)
				thumb_symbol_closed[i]->value(1);
		}

	}


	//if ((*phasesVector)[actualframe]->GetPArm())
	{
		i = (*phasesVector)[actualframe]->GetPArm()->AbstractX;
		if (i < 5)
			arm_x[i]->setonly();

		i = (*phasesVector)[actualframe]->GetPArm()->AbstractY;
		if (i < 7)
			arm_y[i]->setonly();

		i = (*phasesVector)[actualframe]->GetPArm()->AbstractZ;
		if (i < 3)
			arm_z[i]->setonly();

		if((*phasesVector)[actualframe]->GetPArm()->fixedX)
			fixed_x->value(1);
		else
			fixed_x->value(0);

		if((*phasesVector)[actualframe]->GetPArm()->fixedY)
			fixed_y->value(1);
		else
			fixed_y->value(0);

		if((*phasesVector)[actualframe]->GetPArm()->fixedZ)
			fixed_z->value(1);
		else
			fixed_z->value(0);

		/* FIXME
		if((*phasesVector)[actualframe]->fixed_form==true)
			fixed_form->value(1);
		else
			fixed_form->value(0);*/
	}


	if((*phasesVector)[actualframe]->GetStoredPose()!="")
	{
		posefilename->activate();
		posefilename->value((*phasesVector)[actualframe]->GetStoredPose().c_str());
		browseposefile->activate();
		storedpose->value(1);
		DeactivateAll();
	}
	else
	{
		posefilename->deactivate();
		browseposefile->deactivate();
		storedpose->value(0);
		ActivateAll();
	}


	//if ((*phasesVector)[actualframe]->GetPWrist())
	{

		sprintf(t,"%.2f",(*phasesVector)[actualframe]->GetPWrist()->GetWristMagnitude());

		wristmix->value(t);

		if(strcmp(t,"1.00")==0)
		{
			for(i=0;i<6;i++)
			{
				orientation_palm[1][i]->deactivate();
				orientation_fingers[1][i]->deactivate();
			}
		}
		else
		{
			for(i=0;i<6;i++)
			{
				orientation_palm[1][i]->activate();
				orientation_fingers[1][i]->activate();
			}
		}


		if ((*phasesVector)[actualframe]->GetPWrist())
		{
			i = (*phasesVector)[actualframe]->GetPWrist()->GetFromPalm() - 1;
			if(0 <= i && i < 6)
				orientation_palm[0][i]->value(1);

			i = (*phasesVector)[actualframe]->GetPWrist()->GetFromFingerBase() - 1;
			if(0 <= i && i < 6)
				orientation_fingers[0][i]->value(1);

			i = (*phasesVector)[actualframe]->GetPWrist()->GetPalm() - 1;
			if(0 <= i && i < 6)
				orientation_palm[1][i]->value(1);

			i = (*phasesVector)[actualframe]->GetPWrist()->GetFingerBase() - 1;
			if(0 <= i && i < 6)
				orientation_fingers[1][i]->value(1);
		}

	}
#if 0
//FIXME: Make the posefiles work again
	else
		if((*phasesVector)[actualframe]->storedpose==false)
		{
			for(i=0;i<6;i++)
			{
				orientation_palm[1][i]->activate();
			}
			if(((*phasesVector)[actualframe]->palm_int[1]>1)||((*phasesVector)[actualframe]->palm_int[1]==-1))
			{
				orientation_fingers[1][0]->activate();
				orientation_fingers[1][1]->activate();
			}
			if((*phasesVector)[actualframe]->palm_int[1]<4)
			{
				orientation_fingers[1][4]->activate();
				orientation_fingers[1][5]->activate();
			}
			if(((*phasesVector)[actualframe]->palm_int[1]<2)||((*phasesVector)[actualframe]->palm_int[1]>3))
			{
				orientation_fingers[1][2]->activate();
				orientation_fingers[1][3]->activate();
			}
		}
#endif	

		ShowActualTransition();
}

void GestureEditorWindow::Save()
{
	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser("gestures","gesture files (*.txt)",0|2,"select a gesture file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();

	if(chooser->value()!=0)
		gesture->Save(chooser->value());
}

void GestureEditorWindow::Load()
{
	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser("gestures","gesture files (*.txt)",0,"select a gesture file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();

	if(chooser->value()==0)
	{
		return;
	}

    //gesturereader = new GestureReader();

	delete gesture;

	gesture=new Gesture();

	gesture->Load(chooser->value());
	firstArm = true;
	phasesVector = &(gesture->phases);
	if(phasesVector->empty()){
		GesturePhase *phase=new GesturePhase();
		phase->Start();
		phase->Finalize();
		phasesVector->push_back(phase);
	}
	if(gesture->phasesAssym.empty()){
		GesturePhase *phase=new GesturePhase();
		phase->Start();
		phase->Finalize();
		gesture->phasesAssym.push_back(phase);
	}
	actualframe=0;
	this->ShowActualFrame();
	this->ShowActualSide();
	redraw();
}

void GestureEditorWindow::Test()
{
	std::string name = "GestureEditor=test";
	Gesture * mgest = new Gesture(*(this->gesture));
	mgest->SetClass("GestureEditor");
	mgest->SetInstance("test");
	datacontainer->getGestuary()->mMap.erase(name);
	datacontainer->getGestuary()->mMap.insert(GestureMapType::value_type(name,mgest));

	MotorPlanner	*mMPlanner;
	GesturePlanner	*mGPlanner;

	gesture_log=fopen((inimanager.Program_Path+"logs/gesture_engine_log.txt").c_str(),"w");

	float start_time,end_time;

	mgest->SetDuration(1.0f);

	start_time=0.0f;
	if(expressivitywindow->REP->value()==-1)
	{
		end_time=4.0f+mgest->GetDuration();
	}
	else
	{
		end_time=4.0f+mgest->GetDuration()*2.0f;
	}
	inimanager.SetValueFloat("ENGINE_EXPR_SPC",expressivitywindow->SPC->value());
	inimanager.SetValueFloat("ENGINE_EXPR_TMP",expressivitywindow->TMP->value());
	inimanager.SetValueFloat("ENGINE_EXPR_FLD",expressivitywindow->FLD->value());
	inimanager.SetValueFloat("ENGINE_EXPR_PWR",expressivitywindow->PWR->value());

	printf("gesture engine started\n");

	mGPlanner=new GesturePlanner();

	mMPlanner=new MotorPlanner();

	mGPlanner->Init();

	mGPlanner->Request("REST=LEGS_BOTH", -1.0, -0.5, 0.0, -2, -2, -2, -2, -2);
	
	if(expressivitywindow->REP->value()==-1)
	{
		mGPlanner->Request(name, 2.0, 2.0+(mgest->GetDuration()/2.0f),2.0+mgest->GetDuration(),-2,-2, -2, -2, -2);
	}
	else
	{
		vector<float>f;
		f.clear();
			f.push_back(2.0+mgest->GetDuration()+(mgest->GetDuration()/2.0f));
			if(expressivitywindow->REP->value()==1)
			{	
				f.push_back(2.0+mgest->GetDuration()*2.0f);
			}
			mGPlanner->RequestWithRepetition(name, 2.0, 2.0+(mgest->GetDuration()/2.0f),2.0+mgest->GetDuration(),-2,-2,-2,-2,-2,f);
	}

	mGPlanner->Request("REST=LEGS_BOTH", end_time+1.50,end_time+1.60,end_time+1.75,-2,-2, -2, -2, -2);
	
	mGPlanner->InsertRestKeys(true);

	mMPlanner->Init(mGPlanner->GetGestureVector(),int(start_time*25),int((end_time+1.75)*25));

	mMPlanner->GenerateAnimation();

	mMPlanner->WriteAnimation(inimanager.Program_Path+"output/fromgestureeditor.bap");

	delete mMPlanner;
	delete mGPlanner;

	printf("gesture engine ended\n");

	fclose(gesture_log);

	//ADDME: would be nice to play it directly without using files: agentwindow->Play(mMPlanner->GetAnimation());
	agentwindow->Play(inimanager.Program_Path+"output/fromgestureeditor");
}

void GestureEditorWindow::DeactivateAll()
{
	int i,j;

	for(i=0;i<5;i++)
	{
		arm_x[i]->deactivate();
	}

	for(i=0;i<7;i++)
	{
		arm_y[i]->deactivate();
	}

	for(i=0;i<3;i++)
	{
		arm_z[i]->deactivate();
	}

	for(i=0;i<2;i++)
	{
		shape[i]->deactivate();
	}

	for(i=0;i<6;i++)
	{
		hand_form[i]->deactivate();
	}

	for(i=0;i<3;i++)
	{
		thumb_form[i]->deactivate();
	}

	for(i=0;i<3;i++)
	{
		hand_symbol_open[i]->deactivate();
	}
	for(i=0;i<3;i++)
	{
		thumb_symbol_open[i]->deactivate();
	}

	for(i=0;i<3;i++)
	{
		hand_symbol_closed[i]->deactivate();
	}
	for(i=0;i<4;i++)
	{
		thumb_symbol_closed[i]->deactivate();
	}

	for(j=0;j<2;j++)
		for(i=0;i<6;i++)
		{
			orientation_palm[j][i]->deactivate();
			orientation_fingers[j][i]->deactivate();
		}
}

void GestureEditorWindow::ActivateAll()
{
    int i,j;

	for(i=0;i<5;i++)
	{
		arm_x[i]->activate();
	}

	for(i=0;i<7;i++)
	{
		arm_y[i]->activate();
	}

	for(i=0;i<3;i++)
	{
		arm_z[i]->activate();
	}

	for(i=0;i<2;i++)
	{
		shape[i]->activate();
	}

/*	FIXME 
	if((*phasesVector)[actualframe]->whichshape==0)
	{
		for(i=0;i<6;i++)
		{
			hand_form[i]->activate();
		}

		for(i=0;i<3;i++)
		{
			thumb_form[i]->activate();
		}
	}

	if((*phasesVector)[actualframe]->whichshape==1)
	{
		for(i=0;i<3;i++)
		{
			hand_symbol_open[i]->activate();
		}
		for(i=0;i<3;i++)
		{
			thumb_symbol_open[i]->activate();
		}
	}

	if((*phasesVector)[actualframe]->whichshape==2)
	{
		for(i=0;i<3;i++)
		{
			hand_symbol_closed[i]->activate();
		}
		for(i=0;i<4;i++)
		{
			thumb_symbol_closed[i]->activate();
		}
	}

	for(j=0;j<2;j++)
	{
		for(i=0;i<6;i++)
		{
			orientation_palm[j][i]->activate();
		}
		if(((*phasesVector)[actualframe]->palm_int[j]>1)||((*phasesVector)[actualframe]->palm_int[j]==-1))
		{
			orientation_fingers[j][0]->activate();
			orientation_fingers[j][1]->activate();
		}
		if((*phasesVector)[actualframe]->palm_int[j]<4)
		{
			orientation_fingers[j][4]->activate();
			orientation_fingers[j][5]->activate();
		}
		if(((*phasesVector)[actualframe]->palm_int[j]<2)||((*phasesVector)[actualframe]->palm_int[j]>3))
		{
			orientation_fingers[j][2]->activate();
			orientation_fingers[j][3]->activate();
		}
	}*/
}