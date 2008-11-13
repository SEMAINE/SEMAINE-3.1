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

#include ".\phoneme.h"
#include "IniManager.h"
#include "DataContainer.h"
#include "LipData.h"

extern IniManager inimanager;
extern DataContainer *datacontainer;

Phoneme::Phoneme(std::string pho, float startTime, float Duration)
{
	this->Pho=pho;
	this->StartTime=startTime*1000;
	this->Duration=Duration*1000;
	this->Emotion="Neutral";
	this->BlendWithEmotion="Neutral";
	this->BlendingCoeff=0;
	this->PhonemePrec=NULL;
	this->PhonemeSuc=NULL;
	this->next=0;
	this->Emphasis=false;
	this->RhemeTheme=PHONEME_NO_RT;
	this->Intensity=1;

}


Phoneme::Phoneme(LipData* lipdata, std::string pho, float startTime, float duration, CommunicativeAct *comm_act)
{
	int i,j;

	this->lipdata=lipdata;

	this->Pho=pho;
	this->StartTime=startTime;
	this->Duration=duration;
	this->Emotion="";
	this->BlendWithEmotion="";
	this->BlendingCoeff=1.0;
	
	this->Emphasis=false;
	this->Intensity=1.0;
	this->comm_act=comm_act;

	this->PhonemePrec=NULL;
	this->PhonemeSuc=NULL;
	
	SPC = new EngineParameter(0.5, 1.0, 1.5);

	for(i=0; i<NUM_PAR; i++)
		for(j=0;j<3;j++)
			this->Time[i][j]=-1;
	
	SetEmotion();
}

Phoneme::~Phoneme(void)
{
}

void Phoneme::SetNewDuration(float duration)
{
	this->Duration=duration*1000;
}

void Phoneme::SetEmotion()
{
	std::string performative="";
	std::string affect="";
	CommunicativeAct *comm_act2=NULL;

	float cP=1.0;
	float cA=1.0; //coefficient of blending between adjacent emotions (Performative and Affenct)

	float time;
	time=((StartTime+Duration)/NUM_POINTS)/2;

	while(comm_act!=NULL) {

		//here the emotion for each phoneme is defined


		if (strcmp(comm_act->function, "performative") == 0)  
		{
			if (time > comm_act->start_time && time < comm_act->start_time+comm_act->dur_time) {
				performative=(std::string)comm_act->value;
				comm_act2=comm_act;
				
				if (time > comm_act->start_time && time < comm_act->start_time+comm_act->GetPhaseDur(caphase_attack)) {
					cP = lipdata->direction(comm_act->start_time,0, comm_act->start_time+comm_act->GetPhaseDur(caphase_attack),1,time);
				}
				else 
					if (time > comm_act->start_time+comm_act->dur_time-comm_act->GetPhaseDur(caphase_release) && time < comm_act->start_time+comm_act->dur_time) {
						cP = lipdata->direction(comm_act->start_time+comm_act->dur_time-comm_act->GetPhaseDur(caphase_release),1,comm_act->start_time+comm_act->dur_time,0,time);
					}
					else {
						cP=1.0;
					}
			}
		}


		if (strcmp(comm_act->function, "affect") == 0) 
		{
			if (time > comm_act->start_time && time < comm_act->start_time+comm_act->dur_time) 
			{
				affect=(std::string)comm_act->value;
				comm_act2=comm_act;

				if (time > comm_act->start_time && time < comm_act->start_time+comm_act->GetPhaseDur(caphase_attack)) 
				{
					cA = lipdata->direction(comm_act->start_time,0,comm_act->start_time+comm_act->GetPhaseDur(caphase_attack),1,time);
				}
				else 
					if (time > comm_act->start_time+comm_act->dur_time-comm_act->GetPhaseDur(caphase_release) && time < comm_act->start_time+comm_act->dur_time) 
					{
						cA = lipdata->direction(comm_act->start_time+comm_act->dur_time-comm_act->GetPhaseDur(caphase_release),1,comm_act->start_time+comm_act->dur_time,0,time);
					}
					else 
					{
						cA=1.0;
					}
			}
		}
		
		if (strcmp(comm_act->function, "emphasis") == 0)
			if (time > comm_act->start_time && time < comm_act->start_time+comm_act->dur_time)
				Emphasis=true;
		

		comm_act=(CommunicativeAct*)comm_act->next;
	}


	if (performative!="" && affect=="")
	{
		Emotion=performative;
		BlendWithEmotion="neutral";
		BlendingCoeff=cP;
		comm_act=comm_act2;
	}
	else
		if(affect!="" && performative=="")
		{
			Emotion=affect;
			BlendWithEmotion="neutral";
			BlendingCoeff=cA;
			comm_act=comm_act2;
		}
		else
			if(affect!="" && performative!="")
			{
				Emotion=affect;
				BlendWithEmotion="neutral";
				BlendingCoeff=cA;
				comm_act=comm_act2;
			}

	if(Emotion=="")
	{
		Emotion="neutral";
		BlendWithEmotion="neutral";
		BlendingCoeff=1.0;
	}
}

void Phoneme::LoadVowelTargets(float rate)
{
	int i,j;

	if(comm_act!=NULL)
	{
		if(comm_act->expressivitySPC>=-1 && comm_act->expressivitySPC<=1)
			SPC->SetValue(comm_act->expressivitySPC);
		else
			SPC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
	}
	else
		SPC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
    //	SPC->SetValue(0);

	Intensity = SPC->GetScaledValue();
	
	//find vowel targets influenced by left consonant
	std::vector<Vowel>::iterator itervowelLeft;
	itervowelLeft=lipdata->FindVowel(Pho, PhonemePrec->Pho, "neutral");
	
	//find vowel targets influenced by right consonant
	std::vector<Vowel>::iterator itervowelRight;
	itervowelRight=lipdata->FindVowel(Pho, PhonemeSuc->Pho, "neutral");
	
	for(i=0; i<LIP_PAR; i++)
		for(j=0;j<3;j++)
		{
			//targets
			this->TargetLeft[i][j]=((*itervowelLeft).target[i][j]* get_weight(i)) * rate;
			this->TargetRight[i][j]=((*itervowelRight).target[i][j]* get_weight(i)) * rate;
			
			//time
			Time[i][j]=StartTime + (*itervowelLeft).time[i][j]*(Duration - StartTime)/(*itervowelLeft).duration;
		}

	//load tongue targets
	std::vector<Tongue>::iterator itertongue;
	itertongue=lipdata->FindTongue(Pho);

	for(i=LIP_PAR;i<NUM_PAR;i++)
	{
		for(j=0;j<3;j++)
			Time[i][j]=Time[0][j];
		TargetLeft[i][0]=(*itertongue).target[i-LIP_PAR][0];
		TargetLeft[i][1]=(*itertongue).target[i-LIP_PAR][1];
		TargetLeft[i][2]=(*itertongue).target[i-LIP_PAR][1];
	}
}


void Phoneme::LoadConsonantTargets(float rate)
{
	int i,j;
	std::string left_vowel;
	std::string right_vowel;

	if(comm_act!=NULL)
	{
		if(comm_act->expressivitySPC>=-1 && comm_act->expressivitySPC<=1)
			SPC->SetValue(comm_act->expressivitySPC);
		else
			SPC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
	}
	else
		SPC->SetValue(inimanager.GetValueFloat("ENGINE_EXPR_SPC"));
	//	SPC->SetValue(0);

	Intensity = SPC->GetScaledValue();

	if(PhonemePrec==NULL)
		left_vowel="a";
	else
		left_vowel=set_vowel(PhonemePrec->Pho);
	
	if(PhonemeSuc==NULL)
		right_vowel="a";
	else
		right_vowel=set_vowel(PhonemeSuc->Pho);

	
	//find consonant targets influenced by left vowel
	std::vector<Consonant>::iterator iterconsonantleft;
	iterconsonantleft=lipdata->FindConsonant(Pho, left_vowel, "neutral");

	
	//find consonant targets influenced by right vowel
	std::vector<Consonant>::iterator iterconsonantright;
	iterconsonantright=lipdata->FindConsonant(Pho, right_vowel, "neutral");
	
	for(i=0; i<LIP_PAR; i++)
	{
		for(j=0;j<3;j++)
		{
			//targets
			this->TargetLeft[i][j]=((*iterconsonantleft).target[i]* get_weight(i)) * rate;
			this->TargetRight[i][j]=((*iterconsonantright).target[i]* get_weight(i)) * rate;
			
			//time
			Time[i][j]=StartTime + (*iterconsonantleft).time[i]*(Duration - StartTime)/(*iterconsonantleft).duration;
		}
	}

	//load tongue targets
	std::vector<Tongue>::iterator itertongue;
	itertongue=lipdata->FindTongue(Pho);

	if(Pho=="#")
	{
		for(i=LIP_PAR;i<NUM_PAR;i++)
		{
			for(j=0;j<2;j++)
			{
				TargetLeft[i][j]=0;
				Time[i][j]=Time[0][0];
			}
		}
	}
	else
	{
		for(i=LIP_PAR;i<NUM_PAR;i++)
		{
			for(j=0;j<2;j++)
			{
				TargetLeft[i][j]=(*itertongue).target[i-LIP_PAR][0]*rate;
				Time[i][0]=Time[0][0];
			}
		}
	}
}

std::string Phoneme::set_vowel(std::string vow)
{
	if (vow=="a" || vow=="a1") return("a");
	else if (vow=="i" || vow=="i1") return("i");
	else if (vow=="u" || vow=="u1") return("u");
	else if (vow=="e" || vow=="e1" || vow=="E1") return("e");
	else if (vow=="o" || vow=="o1" || vow=="O1") return("o");
	else if (vow=="y") return("y");
	return("a");
}

int Phoneme::get_weight(int inI)
{

	if (inI==0) return(WEIGHT_ULH);
	else if (inI==1) return(WEIGHT_LLH);
	else if (inI==2) return(WEIGHT_JAW);

	else if (inI==3) return(WEIGHT_LW);
	else if (inI==4) return(WEIGHT_ULP);
	else if (inI==5) return(WEIGHT_LLP);
	else if (inI==6) return(WEIGHT_CR);
	else return(1);
}

void Phoneme::Print(std::string filename)
{
	FILE *fid;
	int i, j;

	fid=fopen(filename.c_str(), "a");

	//fprintf(fid, "Pho: %s   ST: %.2f   D: %.2f\n", Pho.c_str(), StartTime, Duration);
/*	for(i=0;i<NUM_PAR;i++)
		for(j=0;j<3;j++)
			fprintf(fid, "%d   time: %.2f   tgleft: %.2f   tgright:%.2f\n", i, Time[i][j], TargetLeft[i][j], TargetRight[i][j]);
*/

	fprintf(fid, "%s  %s   %s   %f\n", Pho.c_str(), Emotion.c_str(), BlendWithEmotion.c_str(), BlendingCoeff);
/*	if(PhonemePrec!=NULL)
		fprintf(fid, "PhoPrec: %s\t", PhonemePrec->Pho.c_str());
	else
		fprintf(fid, "PhoPrec: not found\t");

	fprintf(fid, "Pho: %s\t", Pho.c_str());
	if(PhonemeSuc!=NULL)
		fprintf(fid, "PhoSuc: %s\n", PhonemeSuc->Pho.c_str());
	else
		fprintf(fid, "PhoSuc: not found\n");*/
	fclose(fid);
}

/*
std::string Phoneme::GetEmotion(int index)
{
	if(index==0) return("neutral");
	if(index==1) return("joy");
	if(index==2) return("surprise");
	if(index==3) return("fear");
	if(index==4) return("anger");
	if(index==5) return("disgust");
	if(index==6) return("sadness");
}*/


/*
void Phoneme::LoadVowelTargetsMixEmotion()
{
	int i,j;
	
	std::vector<MixEmotion>::iterator itermixemotion;
	itermixemotion=lipdata->FindEmotion(Emotion);

	std::vector<Vowel>::iterator itervowelLeft;

	for(i=0; i<NUM_EMOT; i++)
	{
		for(j=0; j<NUM_EMOT; j++)
		{
			if((*itermixemotion).EmotPar[i][j]!=0
			{

			}
		}
	}
	

	///////////////////////////////////////////////////////////////////////
	
	itervowelLeftEmo1=lipdata->FindVowel(Pho, PhonemePrec, Emotion);
	std::vector<Vowel>::iterator itervowelLeftEmo2;
	itervowelLeftEmo2=lipdata->FindVowel(Pho, PhonemePrec, this->BlendWithEmotion);

	for(i=0; i<LIP_PAR; i++)
	{
		this->TargetLeft[i][0]=BlendingCoeff*(*itervowelLeftEmo1).target_left[i] + (1-BlendingCoeff)*(*itervowelLeftEmo2).target_left[i];
		this->TargetLeft[i][1]=BlendingCoeff*(*itervowelLeftEmo1).target[i] + (1-BlendingCoeff)*(*itervowelLeftEmo2).target[i];
		this->TargetLeft[i][2]=BlendingCoeff*(*itervowelLeftEmo1).target_right[i] + (1-BlendingCoeff)*(*itervowelLeftEmo2).target_right[i];
		for(j=0;j<3;j++)
			this->TimeLeft[i][j]=(*itervowelLeftEmo1).time[i][j];
	}



	std::vector<Vowel>::iterator itervowelRightEmo1;
	itervowelRightEmo1=lipdata->FindVowel(Pho, PhonemeSuc, Emotion);
	std::vector<Vowel>::iterator itervowelRightEmo2;
	itervowelRightEmo2=lipdata->FindVowel(Pho, PhonemePrec, this->BlendWithEmotion);

	for(i=0; i<LIP_PAR; i++)
	{
		this->TargetRight[i][0]=BlendingCoeff*(*itervowelRightEmo1).target_left[i] + (1-BlendingCoeff)*(*itervowelRightEmo2).target_left[i];
		this->TargetRight[i][1]=BlendingCoeff*(*itervowelRightEmo1).target[i] + (1-BlendingCoeff)*(*itervowelRightEmo2).target[i];
		this->TargetRight[i][2]=BlendingCoeff*(*itervowelRightEmo1).target_right[i] + (1-BlendingCoeff)*(*itervowelRightEmo2).target_right[i];
		for(j=0;j<3;j++)
			this->TimeRight[i][j]=(*itervowelRightEmo1).time[i][j];
	}
}
*/

/*
void Phoneme::LoadConsonantTargets()
{
	int i;
	
	std::vector<Consonant>::iterator iterconsonantleftEmo1;
	iterconsonantleftEmo1=lipdata->FindConsonant(Pho, PhonemePrec, Emotion);
	std::vector<Consonant>::iterator iterconsonantleftEmo2;
	iterconsonantleftEmo2=lipdata->FindConsonant(Pho, PhonemePrec, this->BlendWithEmotion);

	for(i=0; i<LIP_PAR; i++)
	{
		this->TargetLeft[i][1]=BlendingCoeff*(*iterconsonantleftEmo1).target[i] + (1-BlendingCoeff)*(*iterconsonantleftEmo2).target[i];
		this->TimeLeft[i][0]=(*iterconsonantleftEmo1).time[i];
		this->TimeLeft[i][1]=-1;
		this->TimeLeft[i][2]=-1;
	}


	std::vector<Consonant>::iterator iterconsonantrightEmo1;
	iterconsonantrightEmo1=lipdata->FindConsonant(Pho, PhonemePrec, Emotion);
	std::vector<Consonant>::iterator iterconsonantrightEmo2;
	iterconsonantrightEmo2=lipdata->FindConsonant(Pho, PhonemePrec, this->BlendWithEmotion);

	for(i=0; i<LIP_PAR; i++)
	{
		this->TargetLeft[i][1]=BlendingCoeff*(*iterconsonantrightEmo1).target[i] + (1-BlendingCoeff)*(*iterconsonantrightEmo2).target[i];
		this->TimeLeft[i][0]=(*iterconsonantrightEmo1).time[i];
		this->TimeLeft[i][1]=-1;
		this->TimeLeft[i][2]=-1;
	}
}

*/