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

#include "FAPFrame.h"
#include <iostream>
#include <sstream>

extern FILE *face_log;

FAPFrame::FAPFrame(void)
{
	isKeyFrame=false;
//	time=0;
	this->eyesdir.h=0;
	this->eyesdir.v=0;
	this->headdir.h=0;
	this->headdir.v=0;
	this->headdir.t=0;
	this->headmov.type="";
	this->headmov.amplitude=0;
	this->headmov.period=0;
	this->eyesdir.active=false;
	this->headdir.active=false;
	this->headmov.active=false;
	this->probability=1;
	this->TCBParam[Tension]=1;
	this->TCBParam[Continuity]=0;
	this->TCBParam[Bias]=0;
	this->use_at=-1;
	this->use_before=-1;
	this->received_at=-1;

	framenumber=0;

	id="";
}

FAPFrame::~FAPFrame(void)
{
}

float FAPFrame::GetTCBParam(TCBParamType which)
{
	return TCBParam[which];
}

void FAPFrame::ActivateAllFAPs(void)
{
	for(int i=0;i<NUMBER_OF_FAPS;i++)
		this->FAPs[i].active=true;
}

void FAPFrame::DeactivateAll(void)
{
	for(int i=0;i<NUMBER_OF_FAPS;i++)
		this->FAPs[i].active=false;
}

void FAPFrame::ResetAllToZero()
{
	for(int i=0;i<NUMBER_OF_FAPS;i++)
	{
		this->FAPs[i].active=false;
		this->FAPs[i].value=0;
	}
}


//comment 20-5-08 it was activate=false - why?
void FAPFrame::ActivateAndSetAllFAPsTo(int v)
{
	for(int i=0;i<NUMBER_OF_FAPS;i++)
	{
		this->FAPs[i].active=true;
		this->FAPs[i].value=v;
	}
}

int FAPFrame::SetFAP(int num, int value, bool active)
{
	
	/*
	if(value==0)
	{
		FAPs[num].value=0;
		FAPs[num].active=false;
	}
	else
	{
	*/
	if((num>=NUMBER_OF_FAPS) || (num<0)) 
		return 0;

	FAPs[num].value=value;
	FAPs[num].active=active;

	//printf("set fap %d to value %d\n",num,FAPs[num].value);
	//}
	return 1;
}

int FAPFrame::AddToFAP(int num, int value)
{
//	printf("set fap %d to value %d\n",num,value);
	/*
	if(value==0)
	{
		FAPs[num].value=0;
		FAPs[num].active=false;
	}
	else
	{
	*/

	if((num>=NUMBER_OF_FAPS) || (num<0)) 
		return 0;

		FAPs[num].value=FAPs[num].value+value;
		FAPs[num].active=true;
	//}
	return 1;
}

int FAPFrame::GetFAP(int num)
{
	return FAPs[num].value;
}

void FAPFrame::Print()
{
	int i;
	/*
	fprintf(logfile,"probability %f\n",this->probability);
	for(int i=0;i<NUMBER_OF_FAPS;i++)
	{
		if(this->FAPs[i].active)
			fprintf(logfile,"1 ");
		else
			fprintf(logfile,"0 ");
	}
	fprintf(logfile,"\n");
	for(i=0;i<NUMBER_OF_FAPS;i++)
	{
		fprintf(logfile,"%d ",this->FAPs[i].value);
	}
	fprintf(logfile,"\n");
	if(eyesdir.active)
		fprintf(logfile,"eyesdir h:%f v:%f\n",this->eyesdir.h,this->eyesdir.v);
	if(headdir.active)
	fprintf(logfile,"headdir h:%f v:%f t:%f\n",this->headdir.h,this->headdir.v,this->headdir.t);
	if(headmov.active)
		fprintf(logfile,"headmov type:%s ampl:%f period:%f\n",this->headmov.type.c_str(),this->headmov.amplitude,this->headmov.period);
	fprintf(logfile,"\n");
*/

	//printf("probability %f\n",this->probability);
	for(i=0;i<NUMBER_OF_FAPS;i++)
	{
		if(this->FAPs[i].active)
			printf("1 ");
		else
			printf("0 ");
	}
	printf("\n");
	for(i=0;i<NUMBER_OF_FAPS;i++)
	{
		printf("%d ",this->FAPs[i].value);
	}
	printf("\n");
	if(eyesdir.active)
		printf("eyesdir h:%f v:%f\n",this->eyesdir.h,this->eyesdir.v);
	if(headdir.active)
	printf("headdir h:%f v:%f t:%f\n",this->headdir.h,this->headdir.v,this->headdir.t);
	if(headmov.active)
		printf("headmov type:%s ampl:%f period:%f\n",this->headmov.type.c_str(),this->headmov.amplitude,this->headmov.period);
	printf("\n");
}

void FAPFrame::MergeFAP(int num, int value)
{
//	if(value!=0)
//	{
		FAPs[num].value=FAPs[num].value+value;
		FAPs[num].active=true;
//	}
}

void FAPFrame::CopyFrom(FAPFrame *origin)
{
	for(int i=0;i<NUMBER_OF_FAPS;i++)
	{
		this->FAPs[i].value=origin->FAPs[i].value;
		this->FAPs[i].active=origin->FAPs[i].active;
	}
}

void FAPFrame::MergeFrom(FAPFrame *origin)
{
	for(int i=0;i<NUMBER_OF_FAPS;i++)
	{
		if(origin->FAPs[i].active)
		{
			this->FAPs[i].value=origin->FAPs[i].value;
			this->FAPs[i].active=origin->FAPs[i].active;
		}
	}
}

FAPFrame *FAPFrame::GetCorrespondingZeroFrame()
{
	FAPFrame *f;
	f=new FAPFrame();
	for(int i=0;i<NUMBER_OF_FAPS;i++)
	{
		if(this->FAPs[i].active)
			f->SetFAP(i,0);
	}
	return f;
}

void FAPFrame::SaveToFile(FILE *f,int num)
{

	fprintf(f,"%s",WriteFAP(num));
}

std::string FAPFrame::WriteFAP(int num)
{
	char s[30];
	std::ostringstream buffer,buffer2;
	std::string mask("");
	std::string data("");

	//CHRIS fix
	int i = 0;
	for(i=1;i<NUMBER_OF_FAPS;i++)
	{
		if(this->FAPs[i].active)
			mask.append("1 ");
		else
			mask.append("0 ");
	}
	mask.append("\n");
	_itoa(num,s,10);
	data.append(s);
	data.append(" ");
	for(i=1;i<NUMBER_OF_FAPS;i++)
	{
		if(this->FAPs[i].active)
		{
			_itoa(FAPs[i].value,s,10);
			data.append(s);
			data.append(" ");
		}
	}
	data.append("\n");
	buffer2 << mask <<"\n"<<data<<"\n";
	return buffer2.str();

}

void FAPFrame::ReadFromBuffer(char *buffer)
{
	int fapnum;
	for(fapnum=1;fapnum<NUMBER_OF_FAPS;fapnum++)
	{
		sscanf(buffer,"%d",&FAPs[fapnum].active);
		while((buffer[0]!=' ')&&(buffer[0]!=0))
			buffer++;
		if(buffer!=0)
			buffer++;
		else
			break;
	}

	while((buffer[0]=='\n')||(buffer[0]=='\t')||(buffer[0]==' '))
		buffer++;
	
	for(fapnum=0;fapnum<NUMBER_OF_FAPS;fapnum++) 
	{
		if(fapnum==0)
		{
			sscanf(buffer,"%d",&FAPs[0].value);
			this->framenumber=FAPs[0].value;
			while((buffer[0]!=' ')&&(buffer[0]!=0))
				buffer++;
			if(buffer!=0)
				buffer++;
			else
				break;
		}
		else
		{
			if(FAPs[fapnum].active)
			{
				sscanf(buffer,"%d",&FAPs[fapnum].value);
				while((buffer[0]!=' ')&&(buffer[0]!=0))
					buffer++;
				if(buffer!=0)
					buffer++;
				else
					break;
			}
		}
	}
}

void FAPFrame::SetTCBParam(float T, float C, float B)
{
	TCBParam[Tension]=T;
	TCBParam[Continuity]=C;
	TCBParam[Bias]=B;
}

void FAPFrame::SetTension(float T)
{
	TCBParam[Tension]=T;
}

void FAPFrame::SetContinuity(float C)
{
	TCBParam[Continuity]=C;
}

void FAPFrame::SetBias(float B)
{
	TCBParam[Bias]=B;
}

bool FAPFrame::operator<(FAPFrame& a)
{
	if(this->use_at<a.use_at)
		return true;
}

std::string FAPFrame::toString()
{
	int fapnum;
	std::string ff="";
	char st[256];

	for(fapnum=1;fapnum<69;fapnum++)
	{
		sprintf(st,"%d", FAPs[fapnum].active);
		ff+=(std::string)st + " ";
	}
	ff+="\n";
	
	//fprintf(fapfile,"%d ",(*frames)[frameind].GetFAP(0));
	sprintf(st,"%d", this->framenumber);
	ff+=(std::string)st + " ";

	/* Scrivi i vari faps */
	for(fapnum=1;fapnum<69;fapnum++) 
		if(FAPs[fapnum].active==true) 
		{
			sprintf(st,"%d", GetFAP(fapnum));
			ff+=(std::string)st + " ";
		}
	ff+="\n";
	return ff;
}

FAPFrame FAPFrame::clone()
{
	FAPFrame newone;
//	time=0;
	
	newone.eyesdir.h=this->eyesdir.h;
	newone.eyesdir.v=this->eyesdir.v;

	newone.headdir.h=this->headdir.h;
	newone.headdir.v=this->headdir.v;
	newone.headdir.t=this->headdir.t;
	
	newone.headmov.type=this->headmov.type;
	newone.headmov.amplitude=this->headmov.amplitude;
	newone.headmov.period=this->headmov.period;
	
	newone.eyesdir.active=this->eyesdir.active;
	newone.headdir.active=this->headdir.active;
	newone.headmov.active=this->headmov.active;
	
	newone.probability=this->probability;
	
	newone.TCBParam[Tension]=this->TCBParam[Tension];
	newone.TCBParam[Continuity]=this->TCBParam[Continuity];
	newone.TCBParam[Bias]=this->TCBParam[Bias];
	
	newone.use_at=this->use_at;
	newone.use_before=this->use_before;
	newone.received_at=this->received_at;
	
	newone.framenumber=this->framenumber;
	
	newone.id=this->id;	
	
	newone.isKeyFrame=this->isKeyFrame;	
	
	newone.FAPs[NUMBER_OF_FAPS];

	//CHRIS fix
	int i = 0;
	for (i=0;i<NUMBER_OF_FAPS;i++){
		newone.FAPs[i]=this->FAPs[i];
	}
return newone;
}
