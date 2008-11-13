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

// TurnPlanner.cpp: implementation of the TurnPlanner class.
//
//////////////////////////////////////////////////////////////////////
#include "TurnPlanner.h"
#include "GestureTree.h"
#include "GestureList.h"

#include "IniManager.h"

extern IniManager inimanager;

#define OUTPUT

//turn of possible loss of data through conversion warning
#pragma warning( disable : 4244 ) 

//#include "Parser.h"
//#include "IKArm.h"
/**@#-*/
using namespace GestureSpace;
using namespace std;
/**@#+*/
	extern FILE* gesture_log;

//extern "C" char TURNS_FILE[512];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TurnPlanner::TurnPlanner()
{
	output=true;
	lastbeat=3;
}
//////////////////////////////////////////////////////////////////////
TurnPlanner::~TurnPlanner()
{

}


//////////////////////////////////////////////////////////////////////
static float shoulderx=-0.21009;
static float shouldery=0.6078+0.82;
static float shoulderz=-0.0641+0.03;


float TurnPlanner::ReadTurnFile(GestureTree* tree,char *turnsfilename)
{
	FILE *file;
	double end_time=0.0;
	//opens turns file, exit if file is not found or not readable
	if((file=fopen(turnsfilename,"r"))==NULL)
	{
#ifdef OUTPUT
		fprintf(gesture_log,"TurnPlanner::ReadTurnFile - Error opening %s\n",turnsfilename);
#endif
		return 0;
	}
	
	//how many tokens per line in the turns file:
	const int numTokens = 5;

	//ActualNode will store the last node of the building tree
	//and it's initialized with the tree's root
	GestureTree* ActualNode=tree;
	//lines_read counts the number of lines read from turns file
	int lines_read = 0;
	char tempString[200];
	//line is the last read line
	char line[200];
	//words will store the (numTokens) tokens of the line after line scan
	char words[numTokens][100];
	//nospaces is used to deletes white spaces and commas in the gesture description
	char nospaces[100];
	//pos holds a position inside line during the tokenize process
	int pos;
	//while there's something to read from turns file
	while(!feof(file)){
		//count a new line
		lines_read++;
		//read a line, max length id 200 chars
		fgets(line,200,file);
		//if it's an empty line skip it
		if(strcmp(line,"")==0)
			continue;
		//else make it upcase
		strupr(line);
		// also skip comment lines
		if((line[0]=='/')&&(line[1]=='/'))
			continue;
		//then tokenize line into (numTokens) tokens, separator is ';'
		//careful: we could be overwriting memory if there are too many tokens in the file
		pos=0;
		while (strchr(line,';')!=NULL)
		{
			strcpy(tempString,"");
			strncat(tempString,line,strcspn(line,";"));
			strcpy(line,&line[strcspn(line,";")+1]);
			strcpy (words[pos],tempString);
			pos++;
		}
		//delete spaces and commas from the gesture description
		strcpy(nospaces,words[1]);
		while(strchr(nospaces,' ')!=NULL)
		{
			char buff[100];
			strcpy(buff,"");
			strncat(buff,nospaces,strcspn(nospaces," "));
			strcpy(nospaces,&nospaces[strcspn(nospaces," ")+1]);
			strcat(buff,nospaces);
			strcpy(nospaces,buff);
		}
		while(strchr(nospaces,',')!=NULL)
		{
			char buff[100];
			strcpy(buff,"");
			strncat(buff,nospaces,strcspn(nospaces,","));
			strcpy(nospaces,&nospaces[strcspn(nospaces,",")+1]);
			strcat(buff,nospaces);
			strcpy(nospaces,buff);
		}
		strcpy(words[1],nospaces);
		//if the number of tokens is less than 5(=4+intensity) skip to the next line
		if (pos!=numTokens) {
#ifdef OUTPUT
			fprintf(gesture_log,"Unexpected Number of Tokens read on line %d!\n",lines_read);
#endif
			continue;
		} else {
			//else prepare to store the gesture
			double start=atof(words[2]);
			double duration=atof(words[3]);
			double end=start+duration;
			float activation=atof(words[4]); //*added intensity july 2004

			//skip BOUNDARY tags, not useful for gestures			
			strcpy(tempString,"");
			strncat(tempString,words[1],8);
			if(strcmp(tempString,"BOUNDARY")==0){
				end_time=end;
				continue;
			}
			//create a new tree's element
			int  decimal, sign;
			double t1,t2,t3,t4;
			int it1,it2,it3,it4;
			GestureTree* ThisNode=new GestureTree(words[1]);
			ThisNode->SetStartTime(start);
			ThisNode->SetEndTime(end);
			ThisNode->SetType(words[0]);
			ThisNode->SetActivation(activation);
			//store this gesture's timings and last gesture's timings
			t1=ThisNode->GetStartTime();
			t2=ThisNode->GetEndTime();
			t3=ActualNode->GetStartTime();
			t4=ActualNode->GetEndTime();
			//then transform them to avoid problems with == between double vars
			t1=t1*10000;
			t2=t2*10000;
			t3=t3*10000;
			t4=t4*10000;
			it1=atoi(_fcvt(t1,0,&decimal,&sign));
			it2=atoi(_fcvt(t2,0,&decimal,&sign));
			it3=atoi(_fcvt(t3,0,&decimal,&sign));
			it4=atoi(_fcvt(t4,0,&decimal,&sign));
			//if this gesture has the same timings of the last then replace the last
			//if((it1==it3)&&(it2==it4))
			//{
			//	ActualNode->SetStartTime(ThisNode->GetStartTime());
			//	ActualNode->SetEndTime(ThisNode->GetEndTime());
			//	ActualNode->SetDescription(words[1]);
			//	ActualNode->SetType(words[0]);
			//}
			//else if the last gesture is the root, insert this gesture under it
			//else
			if(strcmp(ActualNode->GetDescription(),"AGENT1")==0)
			{
				ActualNode->AddSon(ThisNode);
				ActualNode=ThisNode;
			}
			//else
			else
			{
				//if this gesture's timing is not nested inside the last's timing
				while(
					((it1<it3)||(it2>it4))
					&&(strcmp(ActualNode->GetDescription(),"AGENT1")!=0))
				{
					//go up in the tree
					ActualNode=ActualNode->GetParent();
					t1=ThisNode->GetStartTime();
					t2=ThisNode->GetEndTime();
					t3=ActualNode->GetStartTime();
					t4=ActualNode->GetEndTime();
					//then transform them to avoid problems with == between double vars
					t1=t1*10000;
					t2=t2*10000;
					t3=t3*10000;
					t4=t4*10000;
					it1=atoi(_fcvt(t1,0,&decimal,&sign));
					it2=atoi(_fcvt(t2,0,&decimal,&sign));
					it3=atoi(_fcvt(t3,0,&decimal,&sign));
					it4=atoi(_fcvt(t4,0,&decimal,&sign));
				}
				//and when it nests or arrive to the root then do the insert
				ActualNode->AddSon(ThisNode);
				ActualNode=ThisNode;
			}							
			//update total time
			end_time=end;
		}
	}
	//close the file
	fclose(file);
//	unlink(fileName);
	return float(end_time);
}

//////////////////////////////////////////////////////////////////////
// ONLY CALCULATE TURN 
// INTERPOLATED FRAMES ARE AVAILABLE VIA mMPlanner.GetFrame()
void TurnPlanner::CalculateTurn(std::string turnsfilename)
{
//	char buffer[200];
//	Parser parser;
//	int retParser;
//	sprintf(buffer,"%s%s",GLOBAL_BASE_DIR,"xml\\example.xml");
//	parser.Parse(buffer);
//	if(retParser==1) cout << "Error opening XML file.";
//	if(retParser==2) cout << "Error executing parser.";
	TurnTheTurn(turnsfilename);
	// CREATE ANIMATION IN MEMORY
	// no longer needed; we are writing to file.
	//mMPlanner.GenerateAnimation();
}

//////////////////////////////////////////////////////////////////////

MotorPlanner* TurnPlanner::GetMPlannerPtr()
{
	return &mMPlanner;
}

//////////////////////////////////////////////////////////////////////

void TurnPlanner::PreVisit(GestureTree* t,char* tab)
{
	char p[10];
	strcpy(p,tab);
#ifdef OUTPUT
	fprintf(gesture_log,"%s%s%s start %f end %f\n",
		(t->GetRequested()?"*":" "),
		p,t->GetDescription(),t->GetStartTime(),t->GetEndTime());
#endif
	strcat(p,"-");
	int count=1;
	GestureTree* son=t->GetSonN(count);
	while(son!=NULL)
	{
		PreVisit(son,p);
		count++;
		son=t->GetSonN(count);
	}
}

//////////////////////////////////////////////////////////////////////

bool TurnPlanner::inside(char* s1,char* s2)
{
	char *pdest;
	int  result;
	pdest = strstr( s1, s2 );
	result = pdest - s1 + 1;
	if( pdest != NULL )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////

GestureTree* TurnPlanner::FindEmphasis(GestureTree* g)
{
	if(inside(g->GetDescription(),"EMPHASIS"))
		return g;
	if(g->GetSonN(1)!=NULL)
	{
		GestureTree* g1=g->GetSonN(1);
		while(g1!=NULL)
		{
			if(FindEmphasis(g1)!=NULL)
				return FindEmphasis(g1);
			g1=g1->GetNextBrother();
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////

bool TurnPlanner::SearchForAdjectival(GestureTree* g)
{
	bool res=false;

	//-------------------------------------------------------------------
	//gesture expressivity: overall activation threshold filtering
	bool try_scheduling=false;
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float oac=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
		//GlobalEngineCfg->Engine_Expr_Oac; //oac==overall activation
	try_scheduling =( inside(g->GetDescription(),"ADJECTIVAL")
		&& (!g->GetRequested()) 
		&& (g->GetActivation()>=(1.f-oac)));
#else
	try_scheduling = ( inside(g->GetDescription(),"ADJECTIVAL")
		&& (!g->GetRequested()) );
#endif
	//-------------------------------------------------------------------

	if(try_scheduling) {
		GestureTree* g1;
#ifdef OUTPUT
		fprintf(gesture_log,"I will ask for %s starting at time %f until time %f\n",
		g->GetDescription(),g->GetStartTime(),
		g->GetEndTime());
#endif
		char mainname[250];
		strcpy(mainname,g->GetDescription());
		float mainstart=float(g->GetStartTime());
		float mainstroke=float(g->GetStartTime()+(g->GetEndTime()-g->GetStartTime())*0.75);
		float mainend=float(g->GetEndTime());
		vector<float>f;
		f.clear();
		//now there could be a repetition
		float end,start;

		g1=g;
		g1=g1->GetParent();
		end=float(g1->GetEndTime());
		while(g1->GetNextBrother()!=NULL)
		{
			g1=g1->GetNextBrother();
			start=float(g1->GetStartTime());
			// WITH_EXPRESSIVITY: REPETITION HERE
#ifdef WITH_EXPRESSIVITY
 			assert(inimanager.initialized);
 			if ( (inside(g1->GetDescription(),"EMPHASIS")) &&
 				(inimanager.GetValueFloat("ENGINE_EXPR_REP") > 0.f))
#else
			if(inside(g1->GetDescription(),"EMPHASIS"))
#endif
			{
				end=float(g1->GetEndTime());
#ifdef OUTPUT
				fprintf(gesture_log,"with a repetition starting at time %f until time %f\n",
				g1->GetStartTime(),g1->GetEndTime());
#endif
				f.push_back(float(g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75));
			}
		}
		if(f.size()>0)
		{
			if(mGPlanner.RequestWithRepetition(mainname,mainstart,mainstroke,mainend,-2,-2, -2, -2, -2,f))
			{
				g->SetRequested(true);
				return true;
			}
		}
		else
		{
			if((mainend-mainstart)>1.5)
			{
				if(mGPlanner.Request(mainname,mainstart,mainstroke,mainend,-2,-2, -2, -2, -2))
				{
					g->SetRequested(true);
					return true;
				}
			}
			else
			{
				if(mGPlanner.Request(mainname,mainstart,mainstroke,mainstart+1.5,-2,-2, -2, -2, -2))
				{
					g->SetRequested(true);
					return true;
				}
			}
		}
	}
	//search in the sons
	if(g->GetSonN(1)!=NULL)
	{
		GestureTree* g1=g->GetSonN(1);
		while(g1!=NULL)
		{
			res=res||SearchForAdjectival(g1);
			g1=g1->GetNextBrother();
		}
	}
	return res;
}

//////////////////////////////////////////////////////////////////////

bool TurnPlanner::SearchForDeictic(GestureTree* g)
{
	bool res=false;

	//-------------------------------------------------------------------
	//gesture expressivity: overall activation threshold filtering
	bool try_scheduling=false;
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float oac=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
		//GlobalEngineCfg->Engine_Expr_Oac; //oac==overall activation
	try_scheduling =( inside(g->GetDescription(),"DEICTIC")
		&& (!g->GetRequested())
		&& (g->GetActivation()>=(1.f-oac)));
#else
	try_scheduling = ( inside(g->GetDescription(),"DEICTIC")
		&& (!g->GetRequested()) );
#endif
	//-------------------------------------------------------------------

	if(try_scheduling)
	{
		GestureTree* g1;
#ifdef OUTPUT
		fprintf(gesture_log,"DEICTIC: I will ask for %s starting at time %f until time %f\n",
			g->GetDescription(),g->GetStartTime(),g->GetEndTime());
#endif
		char mainname[250];
		strcpy(mainname,g->GetDescription());
		float mainstart = float(g->GetStartTime());
		float mainstroke= float(g->GetStartTime()+(g->GetEndTime()-g->GetStartTime())*0.75);
		float mainend   = float(g->GetEndTime());
		vector<float>f;
		f.clear();

		//now there could be a repetition
		double end,start;

		g1=g;
		g1=g1->GetParent();
		
		end=g1->GetEndTime();
		while(g1->GetNextBrother()!=NULL)
		{
			g1=g1->GetNextBrother();
			start=g1->GetStartTime();
#ifdef WITH_EXPRESSIVITY
			assert(inimanager.initialized);
			if ( (inside(g1->GetDescription(),"EMPHASIS")) &&
				(inimanager.GetValueFloat("ENGINE_EXPR_REP") > 0.f))
#else
			if(inside(g1->GetDescription(),"EMPHASIS"))
#endif
			{
				end=g1->GetEndTime();
#ifdef OUTPUT
				fprintf(gesture_log,"with a repetition starting at time %f until time %f\n",
				g1->GetStartTime(),g1->GetEndTime());
#endif
				f.push_back(float(g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75));
			}
		}
		if(f.size()>0)
		{
			if(mGPlanner.RequestWithRepetition(mainname,mainstart,mainstroke,mainend,-2,-2, -2, -2, -2,f))
			{
				g->SetRequested(true);
				return true;
			}
		}
		else
		{
			if(mGPlanner.Request(mainname,mainstart,mainstroke,mainend,-2,-2, -2, -2, -2))
			{
				g->SetRequested(true);
				return true;
			}
		}
	}
	//searches in the sons
	if(g->GetSonN(1)!=NULL)
	{
		GestureTree* g1=g->GetSonN(1);
		while(g1!=NULL)
		{
			res=res||SearchForDeictic(g1);
			g1=g1->GetNextBrother();
		}
	}
	return res;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool TurnPlanner::SearchForBeliefRelation(GestureTree* g)
{
	bool res=false;

	//-------------------------------------------------------------------
	//gesture expressivity: overall activation threshold filtering
	bool try_scheduling=false;
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float oac=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
		//GlobalEngineCfg->Engine_Expr_Oac; //oac==overall activation
	try_scheduling =( inside(g->GetDescription(),"BELIEF-RELATION")
		&& (!g->GetRequested())
		&& (g->GetActivation()>=(1.f-oac)));
#else
	try_scheduling = ( inside(g->GetDescription(),"BELIEF-RELATION")
		&& (!g->GetRequested()) );
#endif
	//-------------------------------------------------------------------
	
	if(try_scheduling)
	{
#ifdef OUTPUT
		fprintf(gesture_log,"I will ask for %s with the following timings:\n",
		g->GetDescription());
#endif
		GestureTree* g1=g;
		GestureTree* g2=g;
		g1=g1->GetSonN(1);
		while(g1!=NULL)
		{
			if(inside(g1->GetDescription(),"EMPHASIS"))
			{
#ifdef OUTPUT
				fprintf(gesture_log,"on time %f ",g1->GetStartTime());
#endif
				while(!inside(g2->GetDescription(),"PERFORMATIVE")
					&&!inside(g2->GetDescription(),"AGENT"))
					g2=g2->GetParent();
				if(!inside(g2->GetDescription(),"AGENT"))
				{
#ifdef OUTPUT
					fprintf(gesture_log,"until time %f\n",g2->GetEndTime());
#endif
					if(mGPlanner.Request(
					g->GetDescription(),
					g1->GetStartTime(),
					g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75,
					g2->GetEndTime(),-2,-2, -2, -2, -2
					)) 
					{
						g->SetRequested(true);
						g1=NULL;
						res=true;
					}
					else
					{
#ifdef OUTPUT
						fprintf(gesture_log,"problems scheduling this gesture, skipping belief-relation!\n");
#endif
						return false;
					}
				}
				else
					g1=g1->GetNextBrother();
			}
			else
				g1=g1->GetNextBrother();
		}
	}
	else//searches in the sons
		if(g->GetSonN(1)!=NULL)
		{
			GestureTree* g1=g->GetSonN(1);
			while(g1!=NULL)
			{
				res=res||SearchForBeliefRelation(g1);
				g1=g1->GetNextBrother();
			}
		}
	return res;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
bool TurnPlanner::SearchForAffect(GestureTree* g)
{
	bool res=false;

	//-------------------------------------------------------------------
	//gesture expressivity: overall activation threshold filtering
	bool try_scheduling=false;
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float oac=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
		//GlobalEngineCfg->Engine_Expr_Oac; //oac==overall activation
	try_scheduling =( inside(g->GetDescription(),"AFFECT")
		&& (!g->GetRequested())
		&& (g->GetActivation()>=(1.f-oac)));
#else
	try_scheduling = ( inside(g->GetDescription(),"AFFECT")
		&& (!g->GetRequested()) );
#endif
	//-------------------------------------------------------------------

	if(try_scheduling)
	{
#ifdef OUTPUT
		fprintf(gesture_log,"I will ask for %s with the following timings %f %f:\n",
			g->GetDescription(),g->GetStartTime(),g->GetEndTime());
#endif		
		GestureTree* g1=g;
		GestureTree* g2=g;
		g1=FindEmphasis(g);
		if(g1!=NULL)
		{
#ifdef OUTPUT
			fprintf(gesture_log,"on time %f until time %f\n",g1->GetStartTime(),g1->GetEndTime());
#endif

			if(mGPlanner.Request(
			g->GetDescription(),
			g1->GetStartTime(),
			g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75,
			g1->GetEndTime(),-2,-2, -2, -2, -2
			)) 
			{
				g1=NULL;
				g->SetRequested(true);
				res=true;
			}
			else
				{
#ifdef OUTPUT
					fprintf(gesture_log,"problems scheduling this gesture, skipping affectives!\n");
#endif
					return false;
				}
		}
	}
	else//searches in the sons
		if(g->GetSonN(1)!=NULL)
		{
			GestureTree* g1=g->GetSonN(1);
			while(g1!=NULL)
			{
				res=res||SearchForAffect(g1);
				g1=g1->GetNextBrother();
			}
		}
	return res;
}

//////////////////////////////////////////////////////////////////////
// SIMPLE EMPHASIS:
// MORE SPECIFIC ALGO NEEDED HERE FOR OVERALL ACTIVATION?
//////////////////////////////////////////////////////////////////////

bool TurnPlanner::SearchForSimpleEmphasis(GestureTree* g)
{
	GestureTree* g1=g;
	GestureTree* g2=g;

	g1=FindEmphasis(g);

	while(!inside(g2->GetDescription(),"PERFORMATIVE")
		&&!inside(g2->GetDescription(),"AGENT"))
		g2=g2->GetParent();

	if(g1==NULL)return false;

	//-------------------------------------------------------------------
	//gesture expressivity: overall activation threshold filtering
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float oac=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
		//GlobalEngineCfg->Engine_Expr_Oac; //oac==overall activation
	if (g->GetActivation()<(1.f-oac)) return false;
#endif
	//-------------------------------------------------------------------

#ifdef OUTPUT
	fprintf(gesture_log,"trying to schedule %s on the first emphasis\n",g2->GetDescription());
	fprintf(gesture_log,"timed to start on %f until %f\n",g1->GetStartTime(),g1->GetEndTime());
#endif

	if((g1->GetEndTime()-g1->GetStartTime())<0.7)
	{
		if(mGPlanner.Request(
				g2->GetDescription(),
				g1->GetStartTime(),
				g1->GetStartTime()+0.7*0.75,
				g1->GetStartTime()+0.7,-2,-2, -2, -2, -2
				))
		{
			g->SetRequested(true);
			return true;
		}
	}
	else
	{
		if(mGPlanner.Request(
				g2->GetDescription(),
				g1->GetStartTime(),
				g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75,
				g1->GetEndTime(),-2,-2, -2, -2, -2
				))
		{
			g->SetRequested(true);
			return true;
		}

	}
	
	char buffer[256];
	sprintf(buffer,"BEAT=BEAT%d",lastbeat);
	//here we are circling through beat gestures. good idea?
	if(lastbeat==3)lastbeat=1;
	else lastbeat++;
#ifdef OUTPUT
	fprintf(gesture_log,"failed! so scheduling %s on the first emphasis\n",buffer);
	fprintf(gesture_log,"timed to start on %f until %f\n",g1->GetStartTime(),g1->GetEndTime());
#endif
	mGPlanner.Request(
		buffer,
		g1->GetStartTime(),
		g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75,
		g1->GetEndTime(),-2,-2, -2, -2, -2);
	g->SetRequested(true);
	return true;
}

//////////////////////////////////////////////////////////////////////


bool TurnPlanner::SearchForTopicComment(GestureTree* g)
{
	bool res=false;

	//-------------------------------------------------------------------
	//gesture expressivity: overall activation threshold filtering
	bool try_scheduling=false;
#ifdef WITH_EXPRESSIVITY
	assert(inimanager.initialized);
	float oac=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
		//GlobalEngineCfg->Engine_Expr_Oac; //oac==overall activation
	try_scheduling =( inside(g->GetDescription(),"TOPIC-COMMENT")
		&& (!g->GetRequested())
		&& (g->GetActivation()>=(1.f-oac)) );
#else
	try_scheduling = ( inside(g->GetDescription(),"TOPIC-COMMENT")
		&& (!g->GetRequested()) );
#endif
	//-------------------------------------------------------------------

	if(try_scheduling)
	{
#ifdef OUTPUT
		fprintf(gesture_log,"I will ask for %s with the following timings:\n",
			g->GetDescription());
#endif
		res=true;
		g->SetRequested(true);
		GestureTree* g1=g;
		g1=g1->GetSonN(1);
		while(g1!=NULL)
		{
#ifdef OUTPUT
			fprintf(gesture_log,"on time %f until time %f\n",g1->GetStartTime(),g1->GetEndTime());
#endif
			mGPlanner.Request(
				"BEAT=BEAT1",
				g1->GetStartTime(),
				g1->GetStartTime()+(g1->GetEndTime()-g1->GetStartTime())*0.75,
				g1->GetEndTime(),-2,-2, -2, -2, -2
				);
			g1=g1->GetNextBrother();
		}
	}
	else//searches in the sons
		if(g->GetSonN(1)!=NULL)
		{
			GestureTree* g1=g->GetSonN(1);
			while(g1!=NULL)
			{
				res=SearchForTopicComment(g1);
				g1=g1->GetNextBrother();
			}
		}
	return res;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

void TurnPlanner::TurnTheTurn(std::string turnsfilename)
{
	float start_time =0.0;			// START TIME IN SECONDS
	float end_time;					// END TIME IN SECONDS

	/* deictics preparation removed - should now be done via IK */
	
	// INITIALIZE GESTURE PLANNER
	mGPlanner.Init();


	mGPlanner.Request("REST=LEGS_BOTH", -1.0, -0.5, 0.0,-2,-2, -2, -2, -2);
	//mGPlanner.Request("REST=BOTH", -1.5, -1.25, -1.0);

	GestureTree* gesturetree=new GestureTree("AGENT1");
	end_time=ReadTurnFile(gesturetree,(char*)turnsfilename.c_str());
	if(end_time!=0)
	{

#ifdef OUTPUT
		fprintf(gesture_log,"%s successfully read, total time is %f\n",turnsfilename.c_str(),end_time);
		fprintf(gesture_log,"\npre-visit of the tree:\n\n");
#endif

		PreVisit(gesturetree,"-");

#ifdef OUTPUT
		fprintf(gesture_log,"\n");
		fprintf(gesture_log,"Scanning main blocks\n");
#endif
		int count=1;
		GestureTree* t=gesturetree->GetSonN(count);
		GestureList* themelist=NULL;
		GestureList* rhemelist=NULL;
		GestureList* tl=NULL;
		GestureList* rl=NULL;
		while(t!=NULL)
		{
#ifdef OUTPUT
			fprintf(gesture_log,"Block %d\nGestures are\n",count);
#endif
			t=t->GetSonN(1);
			//build two separate lists scanning the current block of conversation
			while(t!=NULL)
			{
#ifdef OUTPUT
				fprintf(gesture_log,"Type %s Description %s\n",t->GetType(),t->GetDescription());
#endif
				if((strcmp(t->GetType(),"RHEME")==0)
					||(strcmp(t->GetType(),"EMPTY")==0))
					if(rhemelist==NULL)
					{
						rhemelist=new GestureList(t);
						rl=rhemelist;
					}
					else
					{
						rhemelist->next=new GestureList(t,rhemelist);
						rhemelist=rhemelist->next;
					}
				if(strcmp(t->GetType(),"THEME")==0)
					if(themelist==NULL)
					{
						themelist=new GestureList(t);
						tl=themelist;
					}
					else
					{
						themelist->next=new GestureList(t,themelist);
						themelist=themelist->next;
					}
				t=t->GetNextBrother();
			}
			// *************** RHEME LIST *****************
			//scans the rheme list
			int kindofsearch;
			bool finished;
			if(rl!=NULL)
			{
				rhemelist=rl;
#ifdef OUTPUT
				fprintf(gesture_log,"\nrheme items are\n");
#endif
				while(rhemelist!=NULL)
				{
#ifdef OUTPUT
					fprintf(gesture_log,"%s\n",rhemelist->item->GetDescription());
#endif
					rhemelist=rhemelist->next;
				}
				rhemelist=rl;

				//KIND OF SEARCH:
				// 1 - Adjectival or Deictic tags
				// 2 - Belief-Relation tags
				// 3 - Affect tags
				// 4 - Performative or Beat tags
				kindofsearch=1;

				finished=false;
#ifdef OUTPUT
				fprintf(gesture_log,"now scanning rheme list\n");
#endif
				while((kindofsearch<=4)&&(finished==false))
				{
					if(rhemelist==NULL)
					{
#ifdef OUTPUT
						fprintf(gesture_log,"finished searching for gestures of kind %d\n",kindofsearch);
#endif
						kindofsearch++;
						rhemelist=rl;
					}

					if(kindofsearch==1)
						if(SearchForAdjectival(rhemelist->item)||SearchForDeictic(rhemelist->item))
						{
#ifdef OUTPUT
							fprintf(gesture_log,"SCHEDULED ADJECTIVAL/DEICTIC, EXITING\n");
#endif
							finished=true;
						}

					if(kindofsearch==2)
						if(SearchForBeliefRelation(rhemelist->item))
						{
#ifdef OUTPUT
							fprintf(gesture_log,"SCHEDULED BELIEF-RELATION, EXITING\n");
#endif
							finished=true;
						}

					if(kindofsearch==3)
						if(SearchForAffect(rhemelist->item))
						{
#ifdef OUTPUT
							fprintf(gesture_log,"SCHEDULED AFFECT, EXITING\n");
#endif
							finished=true;
						}

					if(kindofsearch==4)
						if(SearchForSimpleEmphasis(rhemelist->item))
						{
#ifdef OUTPUT
							fprintf(gesture_log,"SCHEDULED PERFORMATIVE/BEAT, EXITING\n");
#endif
							finished=true;
						}

					rhemelist=rhemelist->next;
				}

			}
			// *************** THEME LIST *****************
			//scans the theme list
			
			if(tl!=NULL)
			{
				themelist=tl;
#ifdef OUTPUT
				fprintf(gesture_log,"\ntheme items are\n");
#endif
				while(themelist!=NULL)
				{
#ifdef OUTPUT
					fprintf(gesture_log,"%s\n",themelist->item->GetDescription());
#endif
					themelist=themelist->next;
				}
				themelist=tl;
#ifdef OUTPUT
				fprintf(gesture_log,"now scanning theme list\n");
#endif
				while(themelist!=NULL)
				{
					if(themelist->item->GetRequested()==false)
					{
						SearchForAdjectival(themelist->item);
						SearchForDeictic(themelist->item);
						SearchForTopicComment(themelist->item);
					}
					themelist=themelist->next;
				}
			}
			
			//go to the next block of conversation
#ifdef OUTPUT
			fprintf(gesture_log,"\n");
#endif
			count++;
			t=gesturetree->GetSonN(count);
			rhemelist=NULL;
			themelist=NULL;
			rl=NULL;
			tl=NULL;
		}
#ifdef OUTPUT
		fprintf(gesture_log,"\npre-visit of the tree:\n\n");
#endif
		PreVisit(gesturetree,"-");
#ifdef OUTPUT
		fprintf(gesture_log,"\n");
#endif
	}
	
	//LAST GESTURE IS REST POSITION - DO NOT TAKE OUT
	mGPlanner.Request("REST=LEGS_BOTH", end_time+1.50,end_time+1.60,end_time+1.75,-2,-2, -2, -2, -2);
	
	// INSERT REST GESTURES
	mGPlanner.InsertRestKeys();
		
	if (output) mGPlanner.Print();
	
	// MAKE A MOTOR PLAN
	mMPlanner.Init(mGPlanner.GetGestureVector(),int(start_time*GLOBAL_FPS),int((end_time+1.75)*GLOBAL_FPS));
}
