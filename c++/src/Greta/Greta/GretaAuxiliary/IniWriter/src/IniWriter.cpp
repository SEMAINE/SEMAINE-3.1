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

// IniWriter.cpp: implementation of the IniWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "IniWriter.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <fl/fl_widget.h>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>

extern IniManager inimanager;
extern std::string ini_filename;

void selectedliptensionlight(Fl_Widget* w, IniWriter* v)
{
	v->liptensionnormal->clear();
	v->liptensionstrong->clear();
	v->liptensionlight->set();

}
void selectedliptensionnormal(Fl_Widget* w, IniWriter* v)
{
	v->liptensionlight->clear();
	v->liptensionstrong->clear();
	v->liptensionnormal->set();

}
void selectedliptensionstrong(Fl_Widget* w, IniWriter* v)
{
	v->liptensionnormal->clear();
	v->liptensionlight->clear();
	v->liptensionstrong->set();

}

void selectedarticulationhypo(Fl_Widget* w, IniWriter* v)
{
	v->articulationmedium->clear();
	v->articulationhyper->clear();
	v->articulationhypo->set();

}
void selectedarticulationmedium(Fl_Widget* w, IniWriter* v)
{
	v->articulationhypo->clear();
	v->articulationhyper->clear();
	v->articulationmedium->set();

}
void selectedarticulationhyper(Fl_Widget* w, IniWriter* v)
{
	v->articulationmedium->clear();
	v->articulationhypo->clear();
	v->articulationhyper->set();

}
void selectedlipaddemotion(Fl_Widget* w, IniWriter* v)
{
	v->lipdataemotion->clear();
	v->lipaddemotion->set();

}
void selectedlipdataemotion(Fl_Widget* w, IniWriter* v)
{
	v->lipaddemotion->clear();
	v->lipdataemotion->set();

}
void selectedenglishf1(Fl_Widget* w, IniWriter* v)
{
	v->italian->clear();
	v->english_male1->clear();
	v->english_male2->clear();
	v->polish->clear();
	v->swedish->clear();
	v->english_female1->set();

}
void selectedenglishm1(Fl_Widget* w, IniWriter* v)
{
	v->italian->clear();
	v->english_female1->clear();
	v->english_male2->clear();
	v->polish->clear();
	v->swedish->clear();
	v->english_male1->set();

}
void selectedenglishm2(Fl_Widget* w, IniWriter* v)
{
	v->italian->clear();
	v->english_female1->clear();
	v->english_male1->clear();
	v->polish->clear();
	v->swedish->clear();
	v->english_male2->set();
}
void selecteditalian(Fl_Widget* w, IniWriter* v)
{
	v->english_female1->clear();
	v->english_male1->clear();
	v->english_male2->clear();
	v->polish->clear();
	v->swedish->clear();
	v->italian->set();
}

void selectedpolish(Fl_Widget* w, IniWriter* v)
{
	v->english_female1->clear();
	v->english_male1->clear();
	v->english_male2->clear();
	v->italian->clear();
	v->swedish->clear();
	v->polish->set();
}

void selectedswedish(Fl_Widget* w, IniWriter* v)
{
	v->english_female1->clear();
	v->english_male1->clear();
	v->english_male2->clear();
	v->italian->clear();
	v->polish->clear();
	v->swedish->set();
}

void selectedgazebn(Fl_Widget* w, IniWriter* v)
{
	if(((Fl_Button*)w)->value()==0)
	{
		v->MAX_T_S1_L1->hide();
		v->MAX_T_S1->hide();
		v->MAX_T_L1->hide();
		v->MAX_T_S0->hide();
		v->MAX_T_L0->hide();
		v->DT->hide();
		v->LIMIT->hide();
		v->SCALE->hide();
	}
	else
	{
		v->MAX_T_S1_L1->show();
		v->MAX_T_S1->show();
		v->MAX_T_L1->show();
		v->MAX_T_S0->show();
		v->MAX_T_L0->show();
		v->DT->show();
		v->LIMIT->show();
		v->SCALE->show();
		v->gazediagram->value(0);
		v->gazediagram->hide();
	}
}

void selectedarminterpeuler(Fl_Widget* w, IniWriter* v)
{
	v->arminterpquat->clear();
	v->arminterppath->clear();
	v->arminterpeuler->set();

}
void selectedarminterpquat(Fl_Widget* w, IniWriter* v)
{
	v->arminterpeuler->clear();
	v->arminterppath->clear();
	v->arminterpquat->set();

}
void selectedarminterppath(Fl_Widget* w, IniWriter* v)
{
	v->arminterpeuler->clear();
	v->arminterpquat->clear();
	v->arminterppath->set();

}
void selectedwristinterpeuler(Fl_Widget* w, IniWriter* v)
{
	v->wristinterpquat->clear();
	v->wristinterpeuler->set();

}
void selectedwristinterpquat(Fl_Widget* w, IniWriter* v)
{
	v->wristinterpeuler->clear();
	v->wristinterpquat->set();
}
void selectedOAC(Fl_Widget* w, IniWriter* v)
{
	char value[255];
	sprintf(value,"%.2f",v->OAC->value());
	v->OACoutput->value(value);
}

void selectedTMP(Fl_Widget* w, IniWriter* v)
{
	char value[255];
	sprintf(value,"%.2f",v->TMP->value());
	v->TMPoutput->value(value);
}
void selectedSPC(Fl_Widget* w, IniWriter* v)
{
	char value[255];
	sprintf(value,"%.2f",v->SPC->value());
	v->SPCoutput->value(value);
}
void selectedFLD(Fl_Widget* w, IniWriter* v)
{
	char value[255];
	sprintf(value,"%.2f",v->FLD->value());
	v->FLDoutput->value(value);
}
void selectedPWR(Fl_Widget* w, IniWriter* v)
{
	char value[255];
	sprintf(value,"%.2f",v->PWR->value());
	v->PWRoutput->value(value);
}

void selectedREP(Fl_Widget* w, IniWriter* v)
{
	char value[255];
	sprintf(value,"%.2f",v->REP->value());
	v->REPoutput->value(value);
}


void selectedquit(Fl_Widget* w, IniWriter* v)
{
	v->SaveParameters();
	v->hide();
}

void selectedcancel(Fl_Widget* w, IniWriter* v)
{
	v->hide();
}


void selectedbrowsebaseline(Fl_Widget* w, IniWriter* v)
{
	Fl_File_Chooser *chooser;
	chooser= new Fl_File_Chooser((inimanager.Program_Path+"mmsystem").c_str(),"baseline files (*.xml)",FL_SINGLE,"select a baseline file");
	chooser->show();
	while (chooser->shown())
		Fl::wait();
	//char path[1024];
	//fl_filename_relative(path,1024,chooser->value());
	//v->filename->value(path);
	v->baselinefilename->value(chooser->value());
	delete chooser;
}

void selectedmary(Fl_Widget* w, IniWriter* v)
{
	v->euler->clear();
	//v->realspeech->clear();
	v->festival->clear();
}

/*
void selectedmaryenglishmale(Fl_Widget* w, IniWriter* v)
{
	v->mary_englishfemale->value(0);
	v->mary_germanfemale->value(0);
	v->mary_germanmale->value(0);
}
*/

void selectedmaryenglishfemale(Fl_Widget* w, IniWriter* v)
{
	//v->mary_englishmale->value(0);
	v->mary_germanfemale->value(0);
	//v->mary_germanmale->value(0);
}

/*
void selectedmarygermanmale(Fl_Widget* w, IniWriter* v)
{
	v->mary_germanfemale->value(0);
	v->mary_englishfemale->value(0);
	v->mary_englishmale->value(0);
}*/

void selectedmarygermanfemale(Fl_Widget* w, IniWriter* v)
{
	//v->mary_germanmale->value(0);
	v->mary_englishfemale->value(0);
	//v->mary_englishmale->value(0);
}

void IniWriter::SaveParameters()
{
	inimanager.SetValueString("PLAYER_WIN_WIDTH",std::string(playerwidth->value()));
	inimanager.SetValueString("PLAYER_WIN_HEIGHT",std::string(playerheight->value()));
	inimanager.SetValueString("SPEECH_STRETCH",std::string(speechstretch->value()));
	
	inimanager.SetValueString("FESTIVAL_LANGUAGE",std::string("US_ENGLISH_FEMALE1"));

	inimanager.SetValueString("HEAD_NOISE","3000.00");
	
/*	if(conflicts->value()==1)
		inimanager.SetValueString("CONFLICTS_RESOLVER",std::string("1"));
	else
		inimanager.SetValueString("CONFLICTS_RESOLVER",std::string("0"));
*/
	inimanager.SetValueString("FESTIVAL",std::string("0"));

	inimanager.SetValueString("MARY",std::string("1"));

	inimanager.SetValueString("MARY_ADDRESS",std::string(MARYhost->value()));

//	if(mary_englishmale->value()==1)
//		inimanager.SetValueString("MARY_LANGUAGE",std::string("ENGLISH_MALE1"));
	if(mary_englishfemale->value()==1)
		inimanager.SetValueString("MARY_LANGUAGE",std::string("ENGLISH_FEMALE1"));

//	if(mary_germanmale->value()==1)
//		inimanager.SetValueString("MARY_LANGUAGE",std::string("GERMAN_MALE1"));
	if(mary_germanfemale->value()==1)
		inimanager.SetValueString("MARY_LANGUAGE",std::string("GERMAN_FEMALE1"));

	inimanager.SetValueString("EULER",std::string("0"));

	inimanager.SetValueString("EULER_LANGUAGE",std::string("FRENCH_FEMALE1"));

	inimanager.SetValueString("GAZE_MODEL_BN",std::string("0"));
	
	inimanager.SetValueString("ENGINE_MAX_T_S1_L1","4.00");
	inimanager.SetValueString("ENGINE_MAX_T_S1","6.00");
	inimanager.SetValueString("ENGINE_MAX_T_L1","8.2");
	inimanager.SetValueString("ENGINE_MAX_T_S0","1.0");
	inimanager.SetValueString("ENGINE_MAX_T_L0","1.5");
	inimanager.SetValueString("ENGINE_DT","0.12");
	inimanager.SetValueString("ENGINE_SCALE","0.35000");

	inimanager.SetValueInt("DRAW_GAZE_CURVES",0);

	if(liptensionlight->value()==1)
		inimanager.SetValueString("ENGINE_LIP_WEIGHT",std::string("LIGHT"));
	if(liptensionnormal->value()==1)
		inimanager.SetValueString("ENGINE_LIP_WEIGHT",std::string("NORMAL"));
	if(liptensionstrong->value()==1)
		inimanager.SetValueString("ENGINE_LIP_WEIGHT",std::string("STRONG"));

	if(articulationhypo->value()==1)
		inimanager.SetValueString("ENGINE_LIP_FLOW",std::string("BOUND"));
	if(articulationmedium->value()==1)
		inimanager.SetValueString("ENGINE_LIP_FLOW",std::string("MEDIUM"));
	if(articulationhyper->value()==1)
		inimanager.SetValueString("ENGINE_LIP_FLOW",std::string("FREE"));

	inimanager.SetValueString("LIP_EMOTION_MODEL","ADD");

	inimanager.SetValueString("ENGINE_LIP_PARAMETERS","NONE");

	inimanager.SetValueString("PSYCLONE_HOST",std::string(PSYhost->value()));
	inimanager.SetValueString("PSYCLONE_PORT",std::string(PSYport->value()));

	inimanager.SetValueString("ENGINE_EXPR_OAC",std::string(OACoutput->value()));
	inimanager.SetValueString("ENGINE_EXPR_SPC",std::string(SPCoutput->value()));
	inimanager.SetValueString("ENGINE_EXPR_TMP",std::string(TMPoutput->value()));
	inimanager.SetValueString("ENGINE_EXPR_FLD",std::string(FLDoutput->value()));
	inimanager.SetValueString("ENGINE_EXPR_PWR",std::string(PWRoutput->value()));
	inimanager.SetValueString("ENGINE_EXPR_REP",std::string(REPoutput->value()));
	
	//if(arminterpeuler->value()==1)
		inimanager.SetValueString("ENGINE_ARM_INTERP_TYPE",std::string("0"));
/*
	if(arminterpquat->value()==1)
		inimanager.SetValueString("ENGINE_ARM_INTERP_TYPE",std::string("1"));

	if(arminterppath->value()==1)
		inimanager.SetValueString("ENGINE_ARM_INTERP_TYPE",std::string("2"));
*/	
//	if(wristinterpeuler->value()==1)
		inimanager.SetValueString("ENGINE_WRIST_ORIENTATION_INTERP_TYPE",std::string("0"));
/*
	if(wristinterpquat->value()==1)
		inimanager.SetValueString("ENGINE_WRIST_ORIENTATION_INTERP_TYPE",std::string("1"));
*/
	//if(jointslimits->value()==1)
		inimanager.SetValueString("ENGINE_USE_JOINT_LIMITS",std::string("1"));
	//else
	//	inimanager.SetValueString("ENGINE_USE_JOINT_LIMITS",std::string("0"));

	//if(torsonoise->value()==1)
		inimanager.SetValueString("ENGINE_USE_PERLIN_TORSO_NOISE",std::string("1"));
	//else
	//	inimanager.SetValueString("ENGINE_USE_PERLIN_TORSO_NOISE",std::string("0"));

	if(strcmp(baselinefilename->value(),"")==0)
		inimanager.SetValueString("BASELINE_DEFAULT","NONE");
	else
		inimanager.SetValueString("BASELINE_DEFAULT",std::string(baselinefilename->value()));

	if(onlyface->value()==1)
		inimanager.SetValueString("PLAYER_ONLYFACE",std::string("1"));
	else
		inimanager.SetValueString("PLAYER_ONLYFACE",std::string("0"));

	inimanager.SetValueString("CHARACTER_SPEAKER","greta");

	inimanager.SetValueString("CHARACTER_LISTENER","greta");

	inimanager.SetValueInt("PLAYER_SHOWLISTENER",0);

	inimanager.SetValueInt("PLAYER_LISTENERSUBWINDOW",0);

	inimanager.WriteIniFile(ini_filename);

}

void IniWriterclosed(IniWriter* v)
{
}

IniWriter::IniWriter():Fl_Window(100,100,600,460,"Greta setup")
{
	
	int i;
	char value[255];

	if(!inimanager.initialized)
		inimanager.ReadIniFile(ini_filename);		

	if(!inimanager.initialized)
		printf("couldn't read inifile\n");

	/*
	fl_font(FL_HELVETICA,8);
	fl_color(255,255,255);
	conflicts=new Fl_Check_Button(145,10,20,20,"resolve face conficts");
	conflicts->align(FL_ALIGN_LEFT);
	conflicts->value(inimanager.GetValueInt("CONFLICTS_RESOLVER"));
*/
	
	//speechbox->align(FL_ALIGN_INSIDE|FL_ALIGN_LEFT|FL_ALIGN_TOP);

	PSYbox=new Fl_Box(FL_BORDER_BOX,10,40,270,40,"PSYCLONE");
	PSYbox->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);

	PSYhost=new Fl_Input(50,50,120,20,"host");
	PSYhost->align(FL_ALIGN_LEFT);
	PSYhost->value(inimanager.GetValueString("PSYCLONE_HOST").c_str());
	
	PSYport=new Fl_Input(210,50,55,20,"port");
	PSYport->align(FL_ALIGN_LEFT);
	PSYport->value(inimanager.GetValueString("PSYCLONE_PORT").c_str());

	//speechbox=new Fl_Box(FL_BORDER_BOX,5,60,164,64,"");
/*
	festival=new Fl_Check_Button(145,60,20,20,"Festival");
	festival->align(FL_ALIGN_LEFT);
	festival->value(inimanager.GetValueInt("FESTIVAL"));
	festival->callback((Fl_Callback *)selectedfestival,this);


	english_female1=new Fl_Toggle_Button(10,80,50,20,"en_f");
	english_female1->callback((Fl_Callback *)selectedenglishf1,this);
	if(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_FEMALE1")
		english_female1->value(1);

	english_male1=new Fl_Toggle_Button(60,80,50,20,"en_m1");
	english_male1->callback((Fl_Callback *)selectedenglishm1,this);
	if(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE1")
		english_male1->value(1);

	english_male2=new Fl_Toggle_Button(110,80,50,20,"en_m2");
	english_male2->callback((Fl_Callback *)selectedenglishm2,this);
	if(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="US_ENGLISH_MALE2")
		english_male2->value(1);

	italian=new Fl_Toggle_Button(10,100,50,20,"it");
	italian->callback((Fl_Callback *)selecteditalian,this);
	if(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="ITALIAN")
		italian->value(1);

	polish=new Fl_Toggle_Button(60,100,50,20,"polish");
	polish->callback((Fl_Callback *)selectedpolish,this);
	if(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="POLISH")
		polish->value(1);

	swedish=new Fl_Toggle_Button(110,100,50,20,"swedish");
	swedish->callback((Fl_Callback *)selectedswedish,this);
	if(inimanager.GetValueString("FESTIVAL_LANGUAGE")=="SWEDISH")
		swedish->value(1);
*/

	MARYbox=new Fl_Box(FL_BORDER_BOX,10,120,270,110,"MARY TTS");
	MARYbox->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);

	MARYhost=new Fl_Input(50,130,120,20,"host");
	MARYhost->align(FL_ALIGN_LEFT);
	MARYhost->value(inimanager.GetValueString("MARY_ADDRESS").c_str());
/*
	mary=new Fl_Check_Button(145,120,20,20,"Mary");
	mary->align(FL_ALIGN_LEFT);
	mary->value(inimanager.GetValueInt("MARY"));
	mary->callback((Fl_Callback *)selectedmary,this);

	mary_englishmale=new Fl_Toggle_Button(10,140,40,20,"en_m");
	mary_englishmale->callback((Fl_Callback *)selectedmaryenglishmale,this);
	if(inimanager.GetValueString("MARY_LANGUAGE")=="ENGLISH_MALE1")
		mary_englishmale->value(1);
*/
	mary_englishfemale=new Fl_Toggle_Button(40,165,100,20,"english");
	mary_englishfemale->callback((Fl_Callback *)selectedmaryenglishfemale,this);
	if(inimanager.GetValueString("MARY_LANGUAGE")=="ENGLISH_FEMALE1")
		mary_englishfemale->value(1);
/*
	mary_germanmale=new Fl_Toggle_Button(85,140,40,20,"de_m");
	mary_germanmale->callback((Fl_Callback *)selectedmarygermanmale,this);
	if(inimanager.GetValueString("MARY_LANGUAGE")=="GERMAN_MALE1")
		mary_germanmale->value(1);
*/
	mary_germanfemale=new Fl_Toggle_Button(150,165,100,20,"german");
	mary_germanfemale->callback((Fl_Callback *)selectedmarygermanfemale,this);
	if(inimanager.GetValueString("MARY_LANGUAGE")=="GERMAN_FEMALE1")
		mary_germanfemale->value(1);

	speechstretch=new Fl_Input(118,200,50,20,"speech stretch");
	speechstretch->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("SPEECH_STRETCH"));
	speechstretch->value(value);
/*
	euler=new Fl_Check_Button(45,170,20,20,"Euler");
	euler->align(FL_ALIGN_LEFT);
	euler->value(inimanager.GetValueInt("EULER"));
	euler->callback((Fl_Callback *)selectedeuler,this);

	euler_frenchmale=new Fl_Toggle_Button(85,170,40,20,"fr_m");
	euler_frenchmale->callback((Fl_Callback *)selectedeulerfrenchmale,this);
	if(inimanager.GetValueString("EULER_LANGUAGE")=="FRENCH_MALE1")
		euler_frenchmale->value(1);

	euler_frenchfemale=new Fl_Toggle_Button(125,170,35,20,"fr_f");
	euler_frenchfemale->callback((Fl_Callback *)selectedeulerfrenchfemale,this);
	if(inimanager.GetValueString("EULER_LANGUAGE")=="FRENCH_FEMALE1")
		euler_frenchfemale->value(1);

//	realspeech=new Fl_Check_Button(145,170,20,20,"realspeech");
//	realspeech->align(FL_ALIGN_LEFT);
//	realspeech->value(inimanager.GetValueInt("REALSPEECH"));
//	realspeech->callback((Fl_Callback *)selectedrealspeech,this);
*/
	lipbox=new Fl_Box(FL_BORDER_BOX,10,270,270,110,"LIPS");
	lipbox->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);

	liptensionlight=new Fl_Toggle_Button(20,295,70,20,"light");
	liptensionlight->callback((Fl_Callback *)selectedliptensionlight,this);
	liptensionnormal=new Fl_Toggle_Button(110,295,70,20,"normal");
	liptensionnormal->callback((Fl_Callback *)selectedliptensionnormal,this);
	liptensionstrong=new Fl_Toggle_Button(200,295,70,20,"strong");
	liptensionstrong->callback((Fl_Callback *)selectedliptensionstrong,this);
	if(inimanager.GetValueString("ENGINE_LIP_WEIGHT")=="LIGHT")
		liptensionlight->value(1);
	if(inimanager.GetValueString("ENGINE_LIP_WEIGHT")=="NORMAL")
		liptensionnormal->value(1);
	if(inimanager.GetValueString("ENGINE_LIP_WEIGHT")=="STRONG")
		liptensionstrong->value(1);

	articulationhypo=new Fl_Toggle_Button(20,345,70,20,"hypo");
	articulationhypo->callback((Fl_Callback *)selectedarticulationhypo,this);
	articulationmedium=new Fl_Toggle_Button(110,345,70,20,"medium");
	articulationmedium->callback((Fl_Callback *)selectedarticulationmedium,this);
	articulationhyper=new Fl_Toggle_Button(200,345,70,20,"hyper");
	articulationhyper->callback((Fl_Callback *)selectedarticulationhyper,this);
	if(inimanager.GetValueString("ENGINE_LIP_FLOW")=="BOUND")
		articulationhypo->value(1);
	if(inimanager.GetValueString("ENGINE_LIP_FLOW")=="MEDIUM")
		articulationmedium->value(1);
	if(inimanager.GetValueString("ENGINE_LIP_FLOW")=="FREE")
		articulationhyper->value(1);
/*
	//insert new buttons for emtion model
	lipaddemotion=new Fl_Toggle_Button(10,320,75,20,"Add Emo");
	lipaddemotion->callback((Fl_Callback *)selectedlipaddemotion,this);
	lipdataemotion=new Fl_Toggle_Button(85,320,75,20,"Data Emo");
	lipdataemotion->callback((Fl_Callback *)selectedlipdataemotion,this);
	if(inimanager.GetValueString("LIP_EMOTION_MODEL")=="ADD")
		lipaddemotion->value(1);
	if(inimanager.GetValueString("LIP_EMOTION_MODEL")=="DATA")
		lipdataemotion->value(1);
	lipdataemotion->deactivate();


	lipparLSO=new Fl_Toggle_Button(10,370,50,20,"LSO");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("LSO")!=std::string::npos)
		lipparLSO->value(1);
	lipparJAW=new Fl_Toggle_Button(60,370,50,20,"JAW");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("JAW")!=std::string::npos)
		lipparJAW->value(1);
	lipparLSW=new Fl_Toggle_Button(110,370,50,20,"LSW");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("LSW")!=std::string::npos)
		lipparLSW->value(1);
	lipparULP=new Fl_Toggle_Button(10,390,50,20,"ULP");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("ULP")!=std::string::npos)
		lipparULP->value(1);
	lipparLLP=new Fl_Toggle_Button(60,390,50,20,"LLP");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("LLP")!=std::string::npos)
		lipparLLP->value(1);
	lipparCRL=new Fl_Toggle_Button(110,390,50,20,"CRL");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("CRL")!=std::string::npos)
		lipparCRL->value(1);
	lipparCRR=new Fl_Toggle_Button(10,390,50,20,"CRR");
	if(inimanager.GetValueString("ENGINE_LIP_PARAMETERS").find("CRR")!=std::string::npos)
		lipparCRR->value(1);
*/
/*	gazebox=new Fl_Box(FL_BORDER_BOX,180,25,170,220,"");

	gazebn=new Fl_Check_Button(330,28,20,20,"gazeBN");
	gazebn->align(FL_ALIGN_LEFT);
	gazebn->callback((Fl_Callback *)selectedgazebn,this);
	gazebn->value(inimanager.GetValueInt("GAZE_MODEL_BN"));

	MAX_T_S1_L1=new Fl_Input(290,80,50,20,"Max t mutual");
	MAX_T_S1_L1->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_MAX_T_S1_L1"));
	MAX_T_S1_L1->value(value);

	MAX_T_S1=new Fl_Input(290,100,50,20,"Max t sp.lookat");
	MAX_T_S1->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_MAX_T_S1"));
	MAX_T_S1->value(value);

	MAX_T_L1=new Fl_Input(290,120,50,20,"Max t lis.lookat");
	MAX_T_L1->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_MAX_T_L1"));
	MAX_T_L1->value(value);

	MAX_T_S0=new Fl_Input(290,140,50,20,"Max t sp.lookaw.");
	MAX_T_S0->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_MAX_T_S0"));
	MAX_T_S0->value(value);

	MAX_T_L0=new Fl_Input(290,160,50,20,"Max t lis.lookaw.");
	MAX_T_L0->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_MAX_T_L0"));
	MAX_T_L0->value(value);

	DT=new Fl_Input(290,180,50,20,"Min t state tr.");
	DT->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_DT"));
	DT->value(value);

	LIMIT=new Fl_Input(290,200,50,20,"Min prob.");
	LIMIT->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_LIMIT"));
	LIMIT->value(value);

	SCALE=new Fl_Input(290,220,50,20,"Scale factor");
	SCALE->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("ENGINE_SCALE"));
	SCALE->value(value);

	gazediagram=new Fl_Check_Button(320,220,12,12,"draw gaze curves");
	gazediagram->align(FL_ALIGN_LEFT);
	gazediagram->value(inimanager.GetValueInt("DRAW_GAZE_CURVES"));
	

	if(gazebn->value()==1)
	{
		gazediagram->hide();
	}

	if(gazebn->value()==0)
	{
		MAX_T_S1_L1->hide();
		//MAX_T_S0->hide();
		//MAX_T_L0->hide();
		DT->hide();
		LIMIT->hide();
		SCALE->hide();

	}
*/
	
/*	headnoise=new Fl_Input(230,247,45,20,"headnoise");
	headnoise->align(FL_ALIGN_LEFT);
	sprintf(value,"%.2f",inimanager.GetValueFloat("HEAD_NOISE"));
	headnoise->value(value);
	headnoise->labelfont(FL_HELVETICA);
	headnoise->labelsize(10);
	headnoise->textfont(FL_HELVETICA);
	headnoise->textsize(10);

	torsonoise=new Fl_Check_Button(335,250,12,12,"torsonoise");
	torsonoise->align(FL_ALIGN_LEFT);
	torsonoise->value(inimanager.GetValueInt("ENGINE_USE_PERLIN_TORSO_NOISE"));
	torsonoise->labelfont(FL_HELVETICA);
	torsonoise->labelsize(10);


	armsbox=new Fl_Box(FL_BORDER_BOX,180,270,170,125,"");

	jointslimits=new Fl_Check_Button(325,280,10,10,"joits limits");
	jointslimits->align(FL_ALIGN_LEFT);
	jointslimits->value(inimanager.GetValueInt("ENGINE_USE_JOINT_LIMITS"));

	arminterpeuler=new Fl_Toggle_Button(190,320,50,20,"euler");
	arminterpeuler->callback((Fl_Callback *)selectedarminterpeuler,this);
	if(inimanager.GetValueInt("ENGINE_ARM_INTERP_TYPE")==0)
		arminterpeuler->value(1);
	arminterpquat=new Fl_Toggle_Button(240,320,50,20,"quat");
	arminterpquat->callback((Fl_Callback *)selectedarminterpquat,this);
	if(inimanager.GetValueInt("ENGINE_ARM_INTERP_TYPE")==1)
		arminterpquat->value(1);
	arminterppath=new Fl_Toggle_Button(290,320,50,20,"ik path");
	arminterppath->callback((Fl_Callback *)selectedarminterppath,this);
	if(inimanager.GetValueInt("ENGINE_ARM_INTERP_TYPE")==2)
		arminterppath->value(1);

	wristinterpeuler=new Fl_Toggle_Button(190,367,50,20,"euler");
	wristinterpeuler->callback((Fl_Callback *)selectedwristinterpeuler,this);
	if(inimanager.GetValueInt("ENGINE_WRIST_ORIENTATION_INTERP_TYPE")==0)
		wristinterpeuler->value(1);
	wristinterpquat=new Fl_Toggle_Button(240,367,50,20,"quat");
	wristinterpquat->callback((Fl_Callback *)selectedwristinterpquat,this);
	if(inimanager.GetValueInt("ENGINE_WRIST_ORIENTATION_INTERP_TYPE")==1)
		wristinterpquat->value(1);
*/
	/*
	faceanimation_normal=new Fl_Toggle_Button(180,400,80,20,"good faceanim");
	faceanimation_normal->callback((Fl_Callback *)selectedfaceanimation_normal,this);
	faceanimation_normal->value(0);
	faceanimation_normal->labelfont(FL_HELVETICA);
	faceanimation_normal->labelsize(10);

	faceanimation_TestMau=new Fl_Toggle_Button(260,400,90,20,"TestMau faceanim");
	faceanimation_TestMau->callback((Fl_Callback *)selectedfaceanimation_TestMau,this);
	faceanimation_TestMau->labelfont(FL_HELVETICA);
	faceanimation_TestMau->labelsize(10);
	faceanimation_TestMau->value(1);
	*/

/*	APMLfilename=new Fl_Input(75,425,450,15,"APML file");
	APMLfilename->align(FL_ALIGN_LEFT);
	APMLfilename->labelfont(FL_HELVETICA);
	APMLfilename->labelsize(9);
	APMLfilename->textfont(FL_HELVETICA);
	APMLfilename->textsize(9);

	browseAPML=new Fl_Button(530,425,55,15,"browse");
	browseAPML->callback((Fl_Callback *)selectedbrowseAPML,this);
	browseAPML->labelfont(FL_HELVETICA);
	browseAPML->labelsize(9);
	
	FMLAPMLfilename=new Fl_Input(75,445,205,15,"FML-APML file");
	FMLAPMLfilename->align(FL_ALIGN_LEFT);
	FMLAPMLfilename->labelfont(FL_HELVETICA);
	FMLAPMLfilename->labelsize(9);
	FMLAPMLfilename->textfont(FL_HELVETICA);
	FMLAPMLfilename->textsize(9);

	browseFMLAPML=new Fl_Button(290,445,55,15,"browse");
	browseFMLAPML->callback((Fl_Callback *)selectedbrowseFMLAPML,this);
	browseFMLAPML->labelfont(FL_HELVETICA);
	browseFMLAPML->labelsize(9);
	*/

	baselinefilename=new Fl_Input(10,420,210,20,"baseline");
	baselinefilename->align(FL_ALIGN_TOP|FL_ALIGN_LEFT);
	if(inimanager.GetValueString("BASELINE_DEFAULT")=="NONE")
		baselinefilename->value("");
	else
		baselinefilename->value(inimanager.GetValueString("BASELINE_DEFAULT").c_str());

	browsebaseline=new Fl_Button(220,420,60,20,"browse");
	browsebaseline->callback((Fl_Callback *)selectedbrowsebaseline,this);
	/*
	BMLfilename=new Fl_Input(75,465,450,15,"BML file");
	BMLfilename->align(FL_ALIGN_LEFT);
	BMLfilename->labelfont(FL_HELVETICA);
	BMLfilename->labelsize(9);
	BMLfilename->textfont(FL_HELVETICA);
	BMLfilename->textsize(9);

	browseBML=new Fl_Button(530,465,55,15,"browse");
	browseBML->callback((Fl_Callback *)selectedbrowseBML,this);
	browseBML->labelfont(FL_HELVETICA);
	browseBML->labelsize(9);
	*/
	expressivitybox=new Fl_Box(FL_BORDER_BOX,320,40,270,245,"EXPRESSIVITY PARAMETERS");
	expressivitybox->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);

	OAC=new Fl_Slider(330,50,200,15,"OAC");
	OAC->type(FL_HOR_NICE_SLIDER);
	//OAC->box(FL_NO_BOX);
	OAC->bounds(0,1);
	OAC->step(0.1);
	OAC->value(0.5);
	OAC->callback((Fl_Callback *)selectedOAC,this);
	OAC->value(inimanager.GetValueFloat("ENGINE_EXPR_OAC"));
	OACoutput=new Fl_Output(540,50,40,15);
	OACoutput->textfont(FL_HELVETICA);
	OACoutput->textsize(10);
	sprintf(value,"%.2f",OAC->value());
	OACoutput->value(value);
	
	SPC=new Fl_Slider(330,90,200,15,"SPC");
	SPC->type(FL_HOR_NICE_SLIDER);
	//SPC->box(FL_NO_BOX);
	SPC->bounds(-1,1);
	SPC->step(0.1);
	SPC->value(0);
	SPC->callback((Fl_Callback *)selectedSPC,this);
	SPC->value(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
	SPCoutput=new Fl_Output(540,90,40,15);
	SPCoutput->textfont(FL_HELVETICA);
	SPCoutput->textsize(10);
	sprintf(value,"%.2f",SPC->value());
	SPCoutput->value(value);

	TMP=new Fl_Slider(330,130,200,15,"TMP");
	TMP->type(FL_HOR_NICE_SLIDER);
	//TMP->box(FL_NO_BOX);
	TMP->bounds(-1,1);
	TMP->step(0.1);
	TMP->value(0);
	TMP->callback((Fl_Callback *)selectedTMP,this);
	TMP->value(inimanager.GetValueFloat("ENGINE_EXPR_TMP"));
	TMPoutput=new Fl_Output(540,130,40,15);
	TMPoutput->textfont(FL_HELVETICA);
	TMPoutput->textsize(10);
	sprintf(value,"%.2f",TMP->value());
	TMPoutput->value(value);

	FLD=new Fl_Slider(330,170,200,15,"FLD");
	FLD->type(FL_HOR_NICE_SLIDER);
	//FLD->box(FL_NO_BOX);
	FLD->bounds(-1,1);
	FLD->step(0.1);
	FLD->value(0);
	FLD->callback((Fl_Callback *)selectedFLD,this);
	FLD->value(inimanager.GetValueFloat("ENGINE_EXPR_FLD"));
	FLDoutput=new Fl_Output(540,170,40,15);
	FLDoutput->textfont(FL_HELVETICA);
	FLDoutput->textsize(10);
	sprintf(value,"%.2f",FLD->value());
	FLDoutput->value(value);

	PWR=new Fl_Slider(330,210,200,15,"PWR");
	PWR->type(FL_HOR_NICE_SLIDER);
	//PWR->box(FL_NO_BOX);
	PWR->bounds(-1,1);
	PWR->step(0.1);
	PWR->value(0);
	PWR->callback((Fl_Callback *)selectedPWR,this);
	PWR->value(inimanager.GetValueFloat("ENGINE_EXPR_PWR"));
	PWRoutput=new Fl_Output(540,210,40,15);
	PWRoutput->textfont(FL_HELVETICA);
	PWRoutput->textsize(10);
	sprintf(value,"%.2f",PWR->value());
	PWRoutput->value(value);

	REP=new Fl_Slider(330,250,200,15,"REP");
	REP->type(FL_HOR_NICE_SLIDER);
	//REP->box(FL_NO_BOX);
	REP->bounds(-1,1);
	REP->step(0.1);
	REP->value(0);
	REP->callback((Fl_Callback *)selectedREP,this);
	REP->value(inimanager.GetValueFloat("ENGINE_EXPR_REP"));
	REPoutput=new Fl_Output(540,250,40,15);
	//REPoutput->textfont(FL_HELVETICA);
	REPoutput->textsize(10);
	sprintf(value,"%.2f",REP->value());
	REPoutput->value(value);


	playerbox=new Fl_Box(FL_BORDER_BOX,320,320,270,60,"PLAYER");
	playerbox->align(FL_ALIGN_LEFT|FL_ALIGN_TOP);

	playerwidth=new Fl_Input(365,328,50,20,"width");
	//playerwidth->label("player width");
	playerwidth->align(FL_ALIGN_LEFT);
	sprintf(value,"%i",inimanager.GetValueInt("PLAYER_WIN_WIDTH"));
	playerwidth->value(value);


	playerheight=new Fl_Input(490,328,50,20,"height");
	//playerheight->label("height");
	playerheight->align(FL_ALIGN_LEFT);
	sprintf(value,"%i",inimanager.GetValueInt("PLAYER_WIN_HEIGHT"));
	playerheight->value(value);

/*	
	preloadagent=new Fl_Check_Button(464,320,20,20,"preload agent");
	preloadagent->align(FL_ALIGN_LEFT);
	preloadagent->value(inimanager.GetValueInt("PLAYER_PRELOADAGENT"));
	
	showlistener=new Fl_Check_Button(473,305,20,20,"show listener");
	showlistener->align(FL_ALIGN_LEFT);
	showlistener->value(inimanager.GetValueInt("PLAYER_SHOWLISTENER"));
	showlistener->labelfont(FL_HELVETICA);
	showlistener->labelsize(10);
*/
	onlyface=new Fl_Check_Button(330,355,20,20,"only face");
	onlyface->align(FL_ALIGN_RIGHT);
	onlyface->value(inimanager.GetValueInt("PLAYER_ONLYFACE"));

/*	showlistenersubwindow=new Fl_Check_Button(563,320,20,20,"listener subwindow");
	showlistenersubwindow->align(FL_ALIGN_LEFT);
	showlistenersubwindow->value(inimanager.GetValueInt("PLAYER_LISTENERSUBWINDOW"));
	showlistenersubwindow->labelfont(FL_HELVETICA);
	showlistenersubwindow->labelsize(10);
*/
	//capturevideo=new Fl_Check_Button(450,320,20,20,"capture video");
	//capturevideo->align(FL_ALIGN_LEFT);
/*
	numcharacters_speaker=6;
	characters_speaker[0]="greta";
	characters_speaker[1]="greta_hairhigh";
	characters_speaker[2]="girl";
	characters_speaker[3]="sinkman";
	characters_speaker[4]="test";
	characters_speaker[5]="simona";
	character_speaker=new Fl_Choice(455,342,120,18,"agent");
	character_speaker->align(FL_ALIGN_LEFT);
	character_speaker->add(characters_speaker[0].c_str());
	character_speaker->add(characters_speaker[1].c_str());
	character_speaker->add(characters_speaker[2].c_str());
	character_speaker->add(characters_speaker[3].c_str());
	character_speaker->add(characters_speaker[4].c_str());
	character_speaker->add(characters_speaker[5].c_str());
	character_speaker->labelfont(FL_HELVETICA);
	character_speaker->labelsize(10);
	character_speaker->textfont(FL_HELVETICA);
	character_speaker->textsize(10);
	for(i=0;i<numcharacters_speaker;i++)
		if(characters_speaker[i]==inimanager.GetValueString("CHARACTER_SPEAKER"))
			character_speaker->value(i);

	numcharacters_listener=4;
	characters_listener[0]="greta";
	characters_listener[1]="greta_hairhigh";
	characters_listener[2]="girl";
	characters_listener[3]="sinkman";
	character_listener=new Fl_Choice(455,362,120,18,"listener");
	character_listener->align(FL_ALIGN_LEFT);
	character_listener->add(characters_speaker[0].c_str());
	character_listener->add(characters_speaker[1].c_str());
	character_listener->add(characters_speaker[2].c_str());
	character_listener->add(characters_speaker[3].c_str());
	character_listener->labelfont(FL_HELVETICA);
	character_listener->labelsize(10);
	character_listener->textfont(FL_HELVETICA);
	character_listener->textsize(10);
	for(i=0;i<numcharacters_listener;i++)
		if(characters_listener[i]==inimanager.GetValueString("CHARACTER_LISTENER"))
			character_listener->value(i);

	startplayer=new Fl_Button(380,385,200,30,"start player");
	startplayer->callback((Fl_Callback *)selectedstartplayer,this);
	startplayer->labelfont(FL_HELVETICA);
	startplayer->labelsize(13);
*/
//	facemodel->value(inimanager.GetValueString("CHARACTER_SPEAKER").c_str());

	

	//MARYConnect=new Fl_Button(375,440,120,20,"MARY Connect");
	//MARYConnect->callback((Fl_Callback *)selectedmaryconnect,this);
/*
	startengine=new Fl_Button(10,485,340,30,"start engine");
	startengine->callback((Fl_Callback *)selectedstartengine,this);
*/	
	
	quit=new Fl_Button(340,415,100,25,"save and exit");
	quit->callback((Fl_Callback *)selectedquit,this);

	cancel=new Fl_Button(470,415,100,25,"cancel");
	cancel->callback((Fl_Callback *)selectedcancel,this);
	

	this->callback((Fl_Callback *)IniWriterclosed,this);
}

IniWriter::~IniWriter()
{
}

int IniWriter::handle(int e)
{
	return Fl_Window::handle(e);
}

void IniWriter::draw()
{
	Fl_Window::draw();
	//fl_draw("language",12,78);
	//fl_draw("PSYCLONE",10,40);
	//fl_draw("MARY TTS",10,110);
	fl_draw("tension",20,290);
	fl_draw("articulation",20,340);
	//fl_draw("arm interpolation",190,313);
	//fl_draw("wrist interpolation",190,360);
}

