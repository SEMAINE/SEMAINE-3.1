#include "Mimicry.h"


#define ROTATION_ANGLE 0.15

extern CentralClock *pc;

Mimicry::Mimicry()
{
	nodtime=0;
	shaketime=0;
}

Mimicry::~Mimicry()
{

}

DataBackchannel *Mimicry::GenerateMimicryBehaviour(InputData data)
{
	DataBackchannel *db=NULL;
	std::string bml="";
	int time;

	time=pc->GetTime();

/*	if(data.name=="NOD")
	{
		if(time>=nodtime+3000)
		{
			db=CalulateBML(data, time);
			nodtime=time;
		}
	}
	else
	if(data.name=="SHAKE")
	{
		if(time>=shaketime+3000)
		{
			db=CalulateBML(data, time);
			shaketime=time;
		}
	}
	else*/
	db=CalulateBML(data, time);
	
	return(db);
}

DataBackchannel *Mimicry::CalulateBML(InputData data, int time)
{
	//Create BML for mimicry behaviour
	DataBackchannel *db;
	std::string bml="";
	std::string headdirection="head=head";
	int starttime;
	char s[255];
	std::string end="1.2";

	std::string reference="";
	
	db=new DataBackchannel("MIMICRY");
/*
	if(data.name=="head_nod")
	{
		end="1.2";
		headdirection+="_nod";
	}
	if(data.name=="head_shake")
	{
		end="1.2";
		headdirection+="_shake";
	}

	if(data.name=="POSES")
	{
		end="infinity";
		if(data.zone=="BA" || data.zone=="CB")
			headdirection="head=head";//headdirection+="_infront";
		else
		{
			if(data.radiant_head_rotation[0]>ROTATION_ANGLE)
				headdirection+="_down";
			if(data.radiant_head_rotation[0]<-ROTATION_ANGLE)
				headdirection+="_up";

			if(data.radiant_head_rotation[1]<-ROTATION_ANGLE)
				headdirection+="_left";
			if(data.radiant_head_rotation[1]>ROTATION_ANGLE)
				headdirection+="_right";

			if(data.radiant_head_rotation[2]<-ROTATION_ANGLE)
				headdirection+="_aside_l";
			if(data.radiant_head_rotation[2]>ROTATION_ANGLE)
				headdirection+="_aside_r";
		}
			
	}*/

	reference=data.name;
/*
	if(data.modality=="head")
	{
		if(headdirection=="head=head")
			return db;
		else
			db->reference=headdirection;
	}
	else*/
		db->reference=reference;


	db->modality=data.modality;
	db->time=time;
	db->priority=data.priority;
	db->end=end;

	return(db);
}