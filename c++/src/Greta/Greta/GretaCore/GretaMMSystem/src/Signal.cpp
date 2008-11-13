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

#include ".\signal.h"
#include "MaryInterface.h"
#include "RandomGen.h"
#include <time.h>

extern RandomGen *randomgen;
extern IniManager inimanager;

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

TimeMarker::TimeMarker(void){concretized=false;}

TimeMarker::~TimeMarker(void){}

bool Signal::operator<(Signal& a)
{
	if(this->start<=a.start)
		return true;
	return false;
}




Signal::Signal(void)
{
	start=0;
	concretized=false;
	noend=false;
	//idle=false;
	intensity=1;
}

Signal::Signal(const Signal &rhs)
{
	this->modality=rhs.modality;
	this->type=rhs.type;
	this->id=rhs.id;
	this->start_sym=rhs.start_sym;
	this->duration_sym=rhs.duration_sym;
	this->start=rhs.start;
	this->duration=rhs.duration;
	this->reference=rhs.reference;
	this->direction=rhs.direction;
	this->posture=rhs.posture;
	this->shape=rhs.shape;
	this->language=rhs.language;
	this->voice=rhs.voice;
	this->speed_num=rhs.speed_num;
	this->speed_sym=rhs.speed_sym;
	this->concretized=rhs.concretized;
	this->noend=rhs.noend;
	this->BMLcode=rhs.BMLcode;
	this->start=rhs.start;
	this->duration=rhs.duration;
	
	//using the flag "noend" instead
	//this->idle=rhs.idle;
	
	//is it ok??
	this->intensity=rhs.intensity;
		
	if(rhs.strokes.empty()==false)
	{
		std::vector<float>::const_iterator iter;
		for(iter=rhs.strokes.begin();iter!=rhs.strokes.end();iter++)
		{
			float f=(*iter);
			this->strokes.push_back(f);
		}
	}

	if(rhs.parameters.empty()==false)
	{
		EngineParameters::const_iterator iter;
		for(iter=rhs.parameters.begin();iter!=rhs.parameters.end();iter++)
		{
			this->parameters[(*iter).second.name]=(*iter).second;
		}
	}

	if(rhs.timemarkers.empty()==false)
	{
		std::map<std::string,TimeMarker>::const_iterator iter;
		for(iter=rhs.timemarkers.begin();iter!=rhs.timemarkers.end();iter++)
		{
			this->timemarkers[(*iter).first]=(*iter).second;
		}
	}
	if(rhs.alternativeshapes.empty()==false)
	{
		std::vector<alternativeshape>::const_iterator iter;
		for(iter=rhs.alternativeshapes.begin();iter!=rhs.alternativeshapes.end();iter++)
		{
			alternativeshape *as;
			as=new alternativeshape();
			as->name=(*iter).name;
			as->probability=(*iter).probability;
			this->alternativeshapes.push_back(*as);
		}
	}

	if(rhs.stressingpoints.empty()==false)
	{
		std::vector<float>::const_iterator iter;
		for(iter=rhs.stressingpoints.begin();iter!=rhs.stressingpoints.end();iter++)
		{
			this->stressingpoints.push_back(*iter);
		}
	}

}

Signal::~Signal(void)
{
	if(this->parameters.empty()==false)
		this->parameters.clear();
}

float Signal::GetParamValuef(std::string name)
{
	if(this->parameters.find(name)==this->parameters.end())
	{
		printf("Signal::Warning: unknown parameter %s\n",name.c_str());
		return 0;
	}
	return this->parameters[name].GetValue();
}

void Signal::SetParameterf(std::string name, float value)
{
	if(this->parameters.find(name)==this->parameters.end())
	{
		EngineParameter p;
		p.name=name;
		p.SetValue(value);
		this->parameters[name]=p;
	}
	else
		this->parameters[name].SetValue(value);
}

void Signal::SetParameter(std::string name, std::string value)
{
	if(this->parameters.find(name)==this->parameters.end())
	{
		EngineParameter p;
		p.name=name;
		p.SetValue(atof(value.c_str()));
		this->parameters[name]=p;
	}
	else
		this->parameters[name].SetValue(atof(value.c_str()));
}

int Signal::StoreBML(XMLGenericTree *t,IniManager inimanager)
{
	XMLGenericTree *d;

	if(t==0)
		return 0;

	type=t->GetAttribute("type");
	modality=std::string(t->name);

	if(modality=="speech")
		BMLcode=t->ToString();

	//parse file
	if(modality=="file") {
		reference=t->GetAttribute("name");
	}

	id=t->GetAttribute("id");

	direction=t->GetAttribute("direction");
	posture=t->GetAttribute("posture");
	shape=t->GetAttribute("shape");
	voice=t->GetAttribute("voice");
	language=t->GetAttribute("language");
	speed_num=t->GetAttributef("speed_num");
	speed_sym=t->GetAttribute("speed_sym");
	text=t->GetAttribute("text");
	start_sym=t->GetAttribute("start");
	start=t->GetAttributef("start");
	duration_sym=t->GetAttribute("end");
	duration=t->GetAttributef("end");

	if(t->GetAttributef("stroke")!=0)
	{
		strokes.push_back(t->GetAttributef("stroke"));
	}

	SetParameterf("SPC.value",inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
	SetParameterf("TMP.value",inimanager.GetValueFloat("ENGINE_EXPR_TMP"));
	SetParameterf("PWR.value",inimanager.GetValueFloat("ENGINE_EXPR_PWR"));
	SetParameterf("FLD.value",inimanager.GetValueFloat("ENGINE_EXPR_FLD"));
	SetParameterf("REP.value",inimanager.GetValueFloat("ENGINE_EXPR_REP"));
	
	d=t->FindNodeCalled("description");
	
	if(d!=0)
	{
		if((d->GetAttribute("level")=="1")&&(d->GetAttribute("type")=="gretabml"))
		{
			std::list<XMLGenericTree*>::iterator iter;
			for(iter=d->child.begin();iter!=d->child.end();iter++)
			{
				if((*iter)->name=="stroke")
				{
					strokes.push_back((*iter)->GetAttributef("time"));
					continue;
				}
				if((*iter)->name=="reference")
				{
					reference=(*iter)->FindNodeCalled("text")->value;
					continue;
				}
				if((*iter)->name=="intensity")
				{
					intensity= atof (((*iter)->FindNodeCalled("text")->value).c_str());
					continue;
				}

				SetParameter((*iter)->name,(*iter)->FindNodeCalled("text")->value);
			}
		}
	}

	if(modality=="speech")
	{
		StoreTimeMarkers(t);
	}


	return 1;
}


std::string Signal::GetBML(int *uid)
{
	if(modality=="speech")
		return BMLcode;

	std::string s;
	char v[30];

	s="\t<"+modality;

	if(uid!=0)
	{
		sprintf(v,"%d",*uid);

		(*uid)++;

		s=s+" id=\""+this->id+"-"+v+"\" ";
	}
	else
		s=s+" id=\""+this->id+"\" ";

	if(type!="")
		s=s+" type=\""+type+"\"";

	sprintf(v,"%.2f",this->start);
	s=s+" start=\""+v+"\"";

	sprintf(v,"%.2f",this->duration);
	s=s+" end=\""+v+"\"";
	
	if(strokes.empty()==false)
	{
		sprintf(v,"%.2f",strokes[0]);
		s=s+" stroke=\""+v+"\"";
	}

	if(modality=="torso")
		s=s+" posture=\""+this->posture+"\"";

	s=s+">\n";

	s=s+"\t\t<description level=\"1\" type=\"gretabml\">\n";
	s=s+"\t\t<reference>"+reference+"</reference>\n";

	sprintf(v,"%.2f",this->intensity);
	s=s+"\t\t<intensity>"+v+"</intensity>\n";

	if(strokes.size()>1)
	{
		for(int i=1;i<strokes.size();i++)
		{
			sprintf(v,"%.3f",strokes[i]);
			s=s+"\t\t<stroke time=\""+v+"\"/>\n";
		}
	}

	EngineParameters::iterator param;

	for(param=parameters.begin();param!=parameters.end();param++)
	{
		sprintf(v,"%.2f",GetParamValuef((*param).second.name));
		s=s+"\t\t\t<"+(*param).second.name+">"+v+"</"+(*param).second.name+">\n";
	}

	s=s+"\t\t</description>\n";
	s=s+"\t</"+modality+">\n";

	return s;
}

float Signal::GetTimeMarker(std::string name)
{
	if((timemarkers.find(name)!=timemarkers.end())&&(timemarkers[name].concretized==true))
		return timemarkers[name].time;
	else
		return -1;
}

void Signal::StoreTimeMarkers(XMLGenericTree *t)
{
	std::list<XMLGenericTree*>::iterator iterch;

	if(t==0)
		return;

	timemarkers.clear();

	for(iterch=t->child.begin();iterch!=t->child.end();iterch++)
	{
		if((*iterch)->name=="tm")
		{
			TimeMarker tm;
			tm.id=(*iterch)->GetAttribute("id");
			if((*iterch)->HasAttribute("time"))
			{
				tm.time=(*iterch)->GetAttributef("time");
				tm.concretized=true;
			}
			timemarkers[tm.id]=tm;
		}
	}
}

void Signal::StoreTimeMarkersTimings(std::list<TimeMarker> *tml)
{
	std::list<TimeMarker>::iterator iter;
	for(iter=(*tml).begin();iter!=(*tml).end();iter++)
	{
		if(timemarkers.find((*iter).id)!=timemarkers.end())
		{
			if(timemarkers[(*iter).id].concretized==false)
			{
				//printf("%s=%.2f  ",(*iter).id.c_str(),(*iter).time);
				timemarkers[(*iter).id].time=(*iter).time;
				timemarkers[(*iter).id].concretized=true;
			}
		}
	}
}

std::string Signal::ToMaryXML()
{

	std::string s;
	s="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	s=s+"<maryxml version=\"0.4\"\n";
	s=s+"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
	s=s+"xmlns=\"http://mary.dfki.de/2002/MaryXML\"\n";
	s=s+"xml:lang=\"en\">\n\n";
	
	if(this->modality!="speech")
		return "";

	XMLGenericTree *speechtag;
	XMLGenericParser parser;

	parser.SetValidating(false);
	speechtag=parser.ParseBuffer((char*)this->BMLcode.c_str());

	if(speechtag==0)
		return "";

	/*
	char temp [255];
	float temp0=this->start*1000;
	sprintf(temp,"%i",(int) temp0);
	std::string temp2=temp;
	s=s+"<boundary tone='H-^H%' duration='";
	s=s+temp2;
	s=s+"'/>";
*/
	std::list<XMLGenericTree*>::iterator iter;
	for(iter=speechtag->child.begin();iter!=speechtag->child.end();iter++)
	{
		//XMLGenericTree* test =(*iter);
		if((*iter)->name=="boundary")
		{
			prosodystructure ps;
			std::string bs=(*iter)->GetAttribute("start");
			ps.start=bs.substr(bs.find_first_of(":")+1);
			std::string be=(*iter)->GetAttribute("end");
			ps.end=be.substr(be.find_first_of(":")+1);

			if((*iter)->GetAttribute("type")=="LH")
				ps.type="L-H%";
			else
			if((*iter)->GetAttribute("type")=="L")
				ps.type="L-";
			else
			if((*iter)->GetAttribute("type")=="H")
				ps.type="H-";
			else
			if((*iter)->GetAttribute("type")=="LL")
				ps.type="L-%";
			else
			if((*iter)->GetAttribute("type")=="HH")
				ps.type="H-^H%";
			else
			if((*iter)->GetAttribute("type")=="HL")
				ps.type="H-%";
			else
				ps.type="H-^H%";

			boundaries.push_back(ps);
		}
		if((*iter)->name=="pitchaccent")
		{
			prosodystructure ps;
			std::string bs=(*iter)->GetAttribute("start");
			ps.start=bs.substr(bs.find_first_of(":")+1);
			std::string be=(*iter)->GetAttribute("end");
			ps.end=be.substr(be.find_first_of(":")+1);

			if((*iter)->GetAttribute("type")=="Hstar")
				ps.type="H*";
			else
			if((*iter)->GetAttribute("type")=="Lstar")
				ps.type="L*";
			else
			if((*iter)->GetAttribute("type")=="LplusHstar")
				ps.type="L+H*";
			else
			if((*iter)->GetAttribute("type")=="LstarplusH")
				ps.type="L*+H";
			else
			if((*iter)->GetAttribute("type")=="HstarplusL")
				ps.type="H*+L";
			else
			if((*iter)->GetAttribute("type")=="HplusLstar")
				ps.type="H+L*";
			else
				ps.type="H*";

			pitchaccents.push_back(ps);
		}
	}

		
	for(iter=speechtag->child.begin();iter!=speechtag->child.end();iter++)
	{
		if((*iter)->name=="tm")
		{
			std::list<prosodystructure>::iterator bi;

			for(bi=pitchaccents.begin();bi!=pitchaccents.end();bi++)
 				if((*bi).end==(*iter)->GetAttribute("id"))
				{
					s=s+"</t>\n";
					break;
				}
			
			s=s+"\t<mark name='"+(*iter)->GetAttribute("id")+"'/>\n";

			for(bi=boundaries.begin();bi!=boundaries.end();bi++)
				if((*bi).start==(*iter)->GetAttribute("id"))
				{
					float durationmillisecs=atof((*bi).end.c_str())*1000;
					char dmc[10];
					sprintf(dmc,"%.0f",durationmillisecs);
					s=s+"<boundary tone='"+(*bi).type+"' duration='"+dmc+"'/>\n";
					break;
				}

			

			for(bi=pitchaccents.begin();bi!=pitchaccents.end();bi++)
				if((*bi).start==(*iter)->GetAttribute("id"))
				{
					s=s+"<t accent='"+(*bi).type+"'>\n";
					break;
				}
		}
		if((*iter)->name=="text")
			s=s+"\t"+(*iter)->value+"\n";
	}
	s=s+"</maryxml>";
	//std::cout << s << std::endl;
	return s;
}

int Signal::TemporizeTimeMarkers(float time)
{
	if(this->modality!="speech")
		return 0;

	std::string maryxml;
	// Generation of the xml Command for Mary
	maryxml=this->ToMaryXML();

	//printf("%s\n",maryxml.c_str());

	MaryInterface mi;

	//if you use preregistered file
	if(inimanager.GetValueInt("MARY_NATURAL_SPEECH")!=1)
	{
		if(mi.MaryXMLToPhonemes(maryxml,"tmp/mary-out.xml")==0)
		{
			printf("Signal:: Error running the Mary client module\n");
			return 0;
		}
	}//end MARY_NATURAL_SPEECH
	else {printf("Natural speech\n");}
	

	XMLGenericParser parsemary;
	XMLGenericTree *treemary;
	parsemary.SetValidating(false);
	treemary=parsemary.ParseFile("tmp/mary-out.xml");
//	treemary=parsemary.ParseFile("tmp/nestor_extended_original-marystyle2.pho");
	if(treemary==0)
	{
		printf("Signal:: Error parsing Mary output\n");
		return 0;
	}

	std::string phonemesfilename;

	if(this->reference!="")
		phonemesfilename=this->reference;
	else
		phonemesfilename="tmp/mary-out.pho";

	std::list<TimeMarker> tml;
	//float time=0;

	if(language=="french")
		mi.SetFrench();
	if(language=="english")
		mi.SetEnglish();
	if(language=="german")
		mi.SetGerman();

	mi.ExtractAllPhonemes(phonemesfilename,treemary,&tml,&stressingpoints,&time);

	this->StoreTimeMarkersTimings(&tml);

	return 1;
}

int Signal::GenerateSpeech(std::string wavefilename)
{
	if(this->modality!="speech")
		return 0;
	std::string maryxml;

	maryxml=this->ToMaryXML();

	MaryInterface mi;

	//if you use preregistered file
	if(inimanager.GetValueInt("MARY_NATURAL_SPEECH")!=1)
	{

		if(mi.MaryXMLToWave(maryxml,wavefilename)==0)
		{
			printf("Signal:: Error running the Mary client module\n");
			return 0;
		}
	}// end MARY_NATURAL_SPEECH
	else {printf("Natural speech\n");}

	return 1;
}


char * Signal::GenerateSpeechToChar(int *size)
{
	if(this->modality!="speech")
		return 0;
	std::string maryxml;

	maryxml=this->ToMaryXML();

	MaryInterface mi;

	char * output=mi.MaryXMLToWaveInChar(maryxml,size);
	
	if (output == NULL) 
	{
		printf("Signal:: Error running the Mary client module\n");
		return NULL;
	}

	return output;

}


int Signal::Temporize(std::vector<Signal> &signals,void *speech,int *loopnum)
{
	if(concretized==true)
		return 1;

	//if(duration==0)
	//	return 0;

	(*loopnum)++;

	if((*loopnum)>=11)
	{
		printf("Signal::Error too many levels of recursion in signal temporization (%d) \n", *loopnum);
		return 0;
	}

	start=-1;
	duration=-1;
	
	if(start_sym.find_first_not_of("0123456789.-")==std::string::npos)
	{
		start=atof(start_sym.c_str());
	}
	if(duration_sym!="")
	{
		if(duration_sym.find_first_not_of("0123456789.-")==std::string::npos)
		{
			duration=atof(duration_sym.c_str());
		}
	}

	if(start==-1)
	{
		TemporizeAttribute(&start,&start_sym,signals,speech,loopnum);
	}

	if((duration==-1)&&(duration_sym!=""))
	{
		TemporizeAttribute(&duration,&duration_sym,signals,speech,loopnum);
		if(duration!=-1)
			duration=duration-start;

	}
	
	if(start==-1)
		return 0;

	if(duration<0)
	{
		//TRICKY: fake duration for the realtime version of the engine
		duration_sym="6.0";
		duration=6.0;
		//idle=true;
		noend=true;
	}
	else
	{
		noend=false;
	}

	concretized=true;

	return 1;
}


int Signal::Temporize(std::vector<Signal*> &signals,void *speech,int *loopnum)
{
	if(concretized==true)
		return 1;

	if(duration==0)
		return 0;

	(*loopnum)++;

	if((*loopnum)>=11)
	{
		printf("Signal::Error too many levels of recursion in signal temporization (%d)\n", *loopnum);
		return 0;
	}

	start=-1;
	duration=-1;
	
	if(start_sym.find_first_not_of("0123456789.-")==std::string::npos)
	{
		start=atof(start_sym.c_str());
	}
	if(duration_sym!="")
	{
		if(duration_sym.find_first_not_of("0123456789.-")==std::string::npos)
		{
			duration=atof(duration_sym.c_str());
		}
	}

	if(start==-1)
	{
		TemporizeAttribute(&start,&start_sym,signals,speech,loopnum);
	}

	if((duration==-1)&&(duration_sym!=""))
	{
		TemporizeAttribute(&duration,&duration_sym,signals,speech,loopnum);
		if(duration!=-1)
			duration=duration-start;

	}
	
	if(start==-1)
		return 0;

	if(duration<0)
	{
		//TRICKY: fake duration for the realtime version of the engine
		duration_sym="6.0";
		duration=6.0;
		//idle=true;
		noend=true;
	}
	else
	{
		noend=false;
	}

	concretized=true;

	return 1;
}


int Signal::TemporizeAttribute(float *attribute,std::string *attribute_sym,std::vector<Signal> &signals,void *speech,int *loopnum)
{
	std::string referto;
	std::string referattr;

	referto=(*attribute_sym).substr(0,(*attribute_sym).find_first_of(":"));
	referattr=(*attribute_sym).substr((*attribute_sym).find_first_of(":")+1);

	if(speech==0)
		return 0;

	if(referto!="")
	{
		if(((Signal*)speech)->id!=referto)
		{
			Signal *ref;
			ref=GetSignal(signals,referto);
			if(ref!=0)
			{
				if(ref->Temporize(signals,speech,loopnum)==true)
				{
					if(referattr=="start")
						*attribute=ref->start;
					if(referattr=="end")
						*attribute=ref->start+ref->duration;
				}
				else
				{
					printf("MMSystem::Cannot concretize signal: %s-%s\n",id.c_str());
					return 0;
				}
			}
		}
		else
		{
			*attribute=((Signal*)speech)->GetTimeMarker(referattr);
		}
	}
	return 1;
}


int Signal::TemporizeAttribute(float *attribute,std::string *attribute_sym,std::vector<Signal*> &signals,void *speech,int *loopnum)
{
	std::string referto;
	std::string referattr;

	referto=(*attribute_sym).substr(0,(*attribute_sym).find_first_of(":"));
	referattr=(*attribute_sym).substr((*attribute_sym).find_first_of(":")+1);

	if(referto!="")
	{
		if(((Signal*)speech)->id!=referto)
		{
			Signal *ref;
			ref=GetSignal(signals,referto);
			if(ref!=0)
			{
				if(ref->Temporize(signals,speech,loopnum)==true)
				{
					if(referattr=="start")
						*attribute=ref->start;
					if(referattr=="end")
						*attribute=ref->start+ref->duration;
				}
				else
				{
					printf("MMSystem::Cannot concretize signal: %s-%s\n",id.c_str());
					return 0;
				}
			}
		}
		else
		{
			*attribute=((Signal*)speech)->GetTimeMarker(referattr);
		}
	}
	return 1;
}

Signal *Signal::GetSignal(std::vector<Signal> &signals,std::string id)
{
	std::vector<Signal>::iterator iter;
	for(iter=signals.begin();iter!=signals.end();iter++)
	{
		if((*iter).id==id)
			return &(*iter);
	}
	return 0;
}

Signal *Signal::GetSignal(std::vector<Signal*> &signals,std::string id)
{
	std::vector<Signal*>::iterator iter;
	for(iter=signals.begin();iter!=signals.end();iter++)
	{
		if((*iter)->id==id)
			return (*iter);
	}
	return 0;
}

void Signal::ShuffleShape()
{
	if(this->alternativeshapes.empty()==true)
		return;
	
	double p;
	p = randomgen->GetRand01();

	float accum;
	accum=0;
	std::vector<alternativeshape>::iterator iter;
	for(iter=this->alternativeshapes.begin();iter!=this->alternativeshapes.end();iter++)
	{
		accum=accum+(*iter).probability;
		if(p<accum)
		{
			std::string store;
			store=this->reference;
			this->reference=(*iter).name;
			(*iter).name=store;
			break;
		}
	}
}

void Signal::Clear()
{
	this->modality="";
	this->type="";
	this->direction="";
	this->voice="";
	this->language="";
	this->text="";
	this->start=0;
	this->duration=0;
	this->start_sym="";
	this->duration_sym="";
	this->BMLcode="";
	this->id="";
	this->posture="";
	this->pitchaccents.clear();
	this->boundaries.clear();
	this->shape="";
	this->speed_num=0;
	this->speed_sym="";
	this->strokes.clear();
	this->timemarkers.clear();
	this->parameters.clear();

	this->intensity=1;
	//this->idle=false;
	this->noend=false;
}

float Signal::GetIntensity(){return intensity;}
   /** 
    * It allows todefine the value of the intensity.
    * 
    */
void Signal::SetIntensity(float intensity1){this->intensity=intensity1;}


Signal * Signal::clone()
{
	Signal *newone = new Signal();

	newone->modality=this->modality;
	newone->type=this->type;
	newone->id=this->id;
	newone->start_sym=this->start_sym;
	newone->duration_sym=this->duration_sym;
	newone->start=this->start;
	newone->duration=this->duration;
	newone->reference=this->reference;
	newone->direction=this->direction;
	newone->posture=this->posture;
	newone->shape=this->shape;
	newone->language=this->language;
	newone->voice=this->voice;
	newone->speed_num=this->speed_num;
	newone->speed_sym=this->speed_sym;
	newone->concretized=this->concretized;
	newone->noend=this->noend;
	newone->BMLcode=this->BMLcode;
	newone->start=this->start;
	newone->duration=this->duration;
	
	//newone->idle=this->idle;
	newone->noend=this->noend;
	
	newone->intensity=this->intensity;
		
	if(this->strokes.empty()==false)
	{
		std::vector<float>::const_iterator iter;
		for(iter=this->strokes.begin();iter!=this->strokes.end();iter++)
		{
			float f=(*iter);
			newone->strokes.push_back(f);
		}
	}

	if(this->parameters.empty()==false)
	{
		EngineParameters::const_iterator iter;
		for(iter=this->parameters.begin();iter!=this->parameters.end();iter++)
		{
			newone->parameters[(*iter).second.name]=(*iter).second;
		}
	}

	if(this->timemarkers.empty()==false)
	{
		std::map<std::string,TimeMarker>::const_iterator iter;
		for(iter=this->timemarkers.begin();iter!=this->timemarkers.end();iter++)
		{
			newone->timemarkers[(*iter).first]=(*iter).second;
		}
	}
	if(this->alternativeshapes.empty()==false)
	{
		std::vector<alternativeshape>::const_iterator iter;
		for(iter=this->alternativeshapes.begin();iter!=this->alternativeshapes.end();iter++)
		{
			alternativeshape *as;
			as=new alternativeshape();
			as->name=(*iter).name;
			as->probability=(*iter).probability;
			newone->alternativeshapes.push_back(*as);
		}
	}

	if(this->stressingpoints.empty()==false)
	{
		std::vector<float>::const_iterator iter;
		for(iter=this->stressingpoints.begin();iter!=this->stressingpoints.end();iter++)
		{
			newone->stressingpoints.push_back(*iter);
		}
	}

	return newone;
}

bool Signal::intersect(float begin, float end)
{

	if ( (this->start<=begin) && (this->start+ this->duration >= begin) ) return true;
	if ( (this->start<=end) && (this->start+ this->duration >= end) ) return true;
	return false;
}

bool Signal::intersect(float begin)
{
	if ( (this->start<=begin) && (this->start+ this->duration >= begin) ) return true;
	
	return false;
}
