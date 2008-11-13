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

#include ".\CommunicativeIntention.h"
#include "Signal.h"

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

bool CommunicativeIntention::operator<(CommunicativeIntention& a)
{
	if(isemotion&&!a.isemotion)
		return true;
	if(!isemotion&&a.isemotion)
		return false;

	if(isemphasis&&!a.isemphasis)
		return false;
	if(!isemphasis&&a.isemphasis)
		return true;

	if(ispitchaccent&&!a.ispitchaccent)
		return false;
	if(!ispitchaccent&&a.ispitchaccent)
		return true;

	if(importance>a.importance)
		return true;

	if(a.importance>importance)
		return false;

	if(start!=a.start)
		return start<a.start;
	else
	{
		if(!noend&&!a.noend)
			return duration<a.duration;
		if(!noend&&a.noend)
			return true;
		if(noend&&!a.noend)
			return false;
	}
}


CommunicativeIntention::CommunicativeIntention(void)
{
	importance=0.5f;
	concretized=false;
	instantiated=false;
	noend=false;
	isemotion=false;
	isemphasis=false;
	ispitchaccent=false;
	UsedModalities.clear();
}

CommunicativeIntention::~CommunicativeIntention(void)
{
}

int CommunicativeIntention::Temporize(std::vector<MMSystemSpace::CommunicativeIntention> &commacts,void *speech,int *loopnum)
{

	if(concretized==true)
		return 1;

	if(duration=0)
		return 0;

	(*loopnum)++;

	if((*loopnum)>=11)
	{
		printf("MMSystem::Error too many levels of recursion in communicative acts temporization (%d)\n",loopnum);
		return 0;
	}

	start=-1;
	duration=-1;
	
	if(start_sym.find_first_not_of("0123456789.-")==std::string::npos)
		start=atof(start_sym.c_str());
	if(dur_sym!="")
		if(dur_sym.find_first_not_of("0123456789.-")==std::string::npos)
			duration=atof(dur_sym.c_str());

	if(start==-1)
	{
		TemporizeAttribute(&start,&start_sym,commacts,speech,loopnum);
	}

	if((duration==-1)&&(dur_sym!=""))
	{
		TemporizeAttribute(&duration,&dur_sym,commacts,speech,loopnum);
		if(duration!=-1)
			duration=duration-start;

	}
	
	if(start==-1)
	{
		printf("Error temporizing communicative act %s %s\n",this->name.c_str(),this->id.c_str());
		return 0;
	}

	if(duration<0)
		noend=true;

	if(name=="boundary")
	{
		start=start-duration;
	}


	concretized=true;

	return 1;
}

int CommunicativeIntention::TemporizeAttribute(float *attribute,std::string *attribute_sym,std::vector<MMSystemSpace::CommunicativeIntention> &commacts,void *speech,int *loopnum)
{
	std::string referto;
	std::string referattr;

	referto=(*attribute_sym).substr(0,(*attribute_sym).find_first_of(":"));
	referattr=(*attribute_sym).substr((*attribute_sym).find_first_of(":")+1);

	if(referto!="")
	{
		if(((Signal*)speech)->id!=referto)
		{
			CommunicativeIntention *ref;
			ref=GetAct(commacts,referto);
			if(ref!=0)
			{
				if(ref->Temporize(commacts,speech,loopnum)==true)
				{
					if(referattr=="start")
						*attribute=ref->start;
					if(referattr=="end")
						*attribute=ref->start+ref->duration;
				}
				else
				{
					printf("MMSystem::Cannot concretize act: %s-%s\n",name.c_str(),type.c_str());
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

void CommunicativeIntention::DefineContext(std::vector<MMSystemSpace::CommunicativeIntention> &commacts)
{
	float otherstart;
	float otherend;
	float thisstart;
	float thisend;
	std::vector<MMSystemSpace::CommunicativeIntention>::iterator iter;
	
	if(concretized==false)
		return;

	if(commacts.empty()==true)
		return;

	thisstart=this->start;
	thisend=this->start+this->duration;
	for(iter=commacts.begin();iter!=commacts.end();iter++)
	{
		if(((*iter).id!=id)&&((*iter).concretized==true))
		{
			otherstart=(*iter).start;
			otherend=otherstart+(*iter).duration;
			//std::cout<<" Communicative intention :"<<this->name <<" This.start : "<< thisstart<<" this.end : "<<thisend << " other.start : "<< otherstart<< "other.end : "<<otherend <<std::endl;
			if(!noend&&((*iter).noend==false))
			{
				if((otherstart<thisend)&&(otherend>thisstart))
					this->CommunicativeContext.push_back(&(*iter));
			}
			if(noend&&((*iter).noend==false))
			{
				if(thisstart<otherend)
					this->CommunicativeContext.push_back(&(*iter));
			}
			if(!noend&&((*iter).noend==true))
			{
				if(otherstart<thisend)
					this->CommunicativeContext.push_back(&(*iter));
			}
			if(noend&&((*iter).noend==true))
				this->CommunicativeContext.push_back(&(*iter));
		}
	}
}

CommunicativeIntention *CommunicativeIntention::GetAct(std::vector<CommunicativeIntention> &commacts,std::string id)
{
	std::vector<CommunicativeIntention>::iterator iter;
	for(iter=commacts.begin();iter!=commacts.end();iter++)
	{
		if((*iter).id==id)
			return &(*iter);
	}
	return 0;
}

CommunicativeIntention *CommunicativeIntention::GetPitchAccentFromContext()
{
	std::list<CommunicativeIntention*>::iterator iter;
	for(iter=CommunicativeContext.begin();iter!=CommunicativeContext.end();iter++)
	{
		if((*iter)->name=="pitchaccent")
			return (*iter);
	}
	return 0;
}

std::list<std::string> CommunicativeIntention::GetContextUsedModalities()
{
	std::list<std::string> modlist;
	std::list<CommunicativeIntention*>::iterator iter;
	std::list<std::string>::iterator moditer;
	for(iter=CommunicativeContext.begin();iter!=CommunicativeContext.end();iter++)
	{
		if((*iter)->instantiated==true)
		{
			for(moditer=(*iter)->UsedModalities.begin();
				moditer!=(*iter)->UsedModalities.end();
				moditer++)
					modlist.push_back(*moditer);
		}
	}
	return modlist;
}

CommunicativeIntention *CommunicativeIntention::GetActFromContext(std::string searchingname)
{
	std::list<CommunicativeIntention*>::iterator iter;
	for(iter=CommunicativeContext.begin();iter!=CommunicativeContext.end();iter++)
	{
		if((*iter)->name==searchingname)
		{
			return (*iter);
		}
	}
	return 0;
}

void CommunicativeIntention::Print(FILE *f)
{
	std::list<CommunicativeIntention*>::iterator iter;
	if(f==0)
		return;
	if(concretized==false)
		return;
	if(duration<0)
		fprintf(f,"<strong>id=%s;</strong> name=%s; type=%s;<br/>start=%.2f NOEND<br/>\n",
			id.c_str(),name.c_str(),type.c_str(),start);
	else
		fprintf(f,"<strong>id=%s;</strong> name=%s; type=%s;<br/>start=%.2f end=%.2f<br/>\n",
			id.c_str(),name.c_str(),type.c_str(),start,start+duration);
	if(CommunicativeContext.empty()==true)
	{
		fprintf(f,"this act has no communicative context;<br/>\n");
		return;
	}
	fprintf(f,"communicative context:<br/>\n");
	for(iter=CommunicativeContext.begin();iter!=CommunicativeContext.end();iter++)
	{
		if((*iter)->duration<0)
			fprintf(f,"- %s (start:%.2f NOEND);<br/>\n",
				(*iter)->id.c_str(),
				(*iter)->start);
		else
			fprintf(f,"- %s (start:%.2f end:%.2f);<br/>\n",
				(*iter)->id.c_str(),
				(*iter)->start,
				(*iter)->start+(*iter)->duration);
	}
}

//added by radek
void CommunicativeIntention::SetIntensity(float intensity1){this->intensity=intensity1;}
float CommunicativeIntention::GetIntensity(){return intensity;}