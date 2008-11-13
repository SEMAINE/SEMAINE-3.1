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

// LipModel.cpp: implementation of the LipModel class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <conio.h>
#include <math.h>
#include <crtdbg.h>
#include "LipModel.h"
#include "FaceEngine.h"
#include "IniManager.h"
#include "DataContainer.h"

//#include "FaceData.h"

extern IniManager inimanager;
extern DataContainer *datacontainer;
//extern FramesTotalNumber;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



LipModel::LipModel(std::vector<FAPFrame> *fapvector, CommunicativeAct *first_comm_act)
{
	int i;

	lipdata=datacontainer->getLipData();

	animationtime = 0.0;

	for(i=0;i<64;i++)
		isLipFap[i]=1;
	isLipFap[0]=0;
	isLipFap[1]=0;
	isLipFap[2]=0;
	isLipFap[14]=0;
	isLipFap[15]=0;
	isLipFap[19]=0;
	isLipFap[20]=0;
	isLipFap[21]=0;
	isLipFap[22]=0;
	isLipFap[23]=0;
	isLipFap[24]=0;
	isLipFap[25]=0;
	isLipFap[26]=0;
	isLipFap[27]=0;
	isLipFap[28]=0;
	isLipFap[29]=0;
	isLipFap[30]=0;
	isLipFap[31]=0;
	isLipFap[32]=0;
	isLipFap[33]=0;
	isLipFap[34]=0;
	isLipFap[35]=0;
	isLipFap[36]=0;
	isLipFap[37]=0;
	isLipFap[38]=0;
	isLipFap[39]=0;
	isLipFap[40]=0;
	isLipFap[48]=0;
	isLipFap[49]=0;
	isLipFap[50]=0;
	isLipFap[64]=0;
	isLipFap[65]=0;
	isLipFap[66]=0;
	isLipFap[67]=0;
	isLipFap[68]=0;


	//16-01-06
	//Expessivity Parameters
	ENGINE_ARTICULATION_OAC=inimanager.GetValueFloat("ENGINE_EXPR_OAC");
	ENGINE_INTENSIVITY_SPC=inimanager.GetValueFloat("ENGINE_EXPR_SPC");
	ENGINE_TENSION_PWR=inimanager.GetValueFloat("ENGINE_EXPR_PWR");

	ENGINE_LIP_FLOW=inimanager.GetValueString("ENGINE_LIP_FLOW");
	ENGINE_LIP_PARAMETERS=inimanager.GetValueString("ENGINE_LIP_PARAMETERS");
	FESTIVAL_LANGUAGE=inimanager.GetValueString("FESTIVAL_LANGUAGE");
	ENGINE_LIP_WEIGHT=inimanager.GetValueString("ENGINE_LIP_WEIGHT");
	LIP_EMOTION_MODEL=inimanager.GetValueString("LIP_EMOTION_MODEL");
	FESTIVAL=inimanager.GetValueString("FESTIVAL");
	EULER=inimanager.GetValueString("EULER");
	EULER_LANGUAGE=inimanager.GetValueString("EULER_LANGUAGE");
	MARY=inimanager.GetValueString("MARY");
	MARY_LANGUAGE=inimanager.GetValueString("MARY_LANGUAGE");

	this->first_comm_act=first_comm_act;
	this->fapvector=fapvector;


	ita2ita[0][0]="a1"; ita2ita[0][1]="a1";
	ita2ita[1][0]="a"; ita2ita[1][1]="a";
	ita2ita[2][0]="e1"; ita2ita[2][1]="e1";
	ita2ita[3][0]="e"; ita2ita[3][1]="e";
	ita2ita[4][0]="E1"; ita2ita[4][1]="E1";
	ita2ita[5][0]="i1"; ita2ita[5][1]="i1";
	ita2ita[6][0]="i"; ita2ita[6][1]="i";
	ita2ita[7][0]="o1"; ita2ita[7][1]="o1";
	ita2ita[8][0]="o"; ita2ita[8][1]="o";
	ita2ita[9][0]="O1"; ita2ita[9][1]="O1";
	ita2ita[10][0]="u1"; ita2ita[10][1]="u1"; 
	ita2ita[11][0]="u"; ita2ita[11][1]="u";
	ita2ita[12][0]="y"; ita2ita[12][1]="y";
	ita2ita[13][0]="b"; ita2ita[13][1]="b";
	ita2ita[14][0]="c"; ita2ita[14][1]="c";
	ita2ita[15][0]="d"; ita2ita[15][1]="d";
	ita2ita[16][0]="f"; ita2ita[16][1]="e";
	ita2ita[17][0]="g"; ita2ita[17][1]="g";
	ita2ita[18][0]="h"; ita2ita[18][1]="h";
	ita2ita[19][0]="l"; ita2ita[19][1]="l";
	ita2ita[20][0]="m"; ita2ita[20][1]="m";
	ita2ita[21][0]="n"; ita2ita[21][1]="n";
	ita2ita[22][0]="p"; ita2ita[22][1]="p";
	ita2ita[23][0]="q"; ita2ita[23][1]="q";
	ita2ita[24][0]="r"; ita2ita[24][1]="r";
	ita2ita[25][0]="s"; ita2ita[25][1]="s";
	ita2ita[26][0]="t"; ita2ita[26][1]="t";
	ita2ita[27][0]="v"; ita2ita[27][1]="v";
	ita2ita[28][0]="z"; ita2ita[28][1]="z";
	ita2ita[29][0]="E"; ita2ita[29][1]="e";
	ita2ita[30][0]="O"; ita2ita[30][1]="o";
	ita2ita[31][0]="S"; ita2ita[31][1]="SS";
	ita2ita[32][0]="dz"; ita2ita[32][1]="z";
	ita2ita[33][0]="Z"; ita2ita[33][1]="tS";
	ita2ita[34][0]="dZ"; ita2ita[34][1]="tS";
	ita2ita[35][0]="nf"; ita2ita[35][1]="n";
	ita2ita[36][0]="J"; ita2ita[36][1]="n";
	ita2ita[37][0]="JJ"; ita2ita[37][1]="n";
	ita2ita[38][0]="ng"; ita2ita[38][1]="n";
	ita2ita[39][0]="L="; ita2ita[39][1]="i1";
	ita2ita[40][0]="#"; ita2ita[40][1]="#";
	ita2ita[41][0]="LL"; ita2ita[41][1]="i1";
	ita2ita[42][0]=NULL; ita2ita[42][1]=NULL;
	ita2ita[43][0]="k"; ita2ita[43][1]="k";
	ita2ita[44][0]="w"; ita2ita[44][1]="w";
	ita2ita[45][0]="x"; ita2ita[45][1]="c";
	ita2ita[46][0]="j"; ita2ita[46][1]="i";


	eng2ita[0][0]="pau"; eng2ita[0][1]="#";
	eng2ita[1][0]="ey"; eng2ita[1][1]="e+i";
	eng2ita[2][0]="er"; eng2ita[2][1]="e";
	eng2ita[3][0]="ih"; eng2ita[3][1]="i";
	eng2ita[4][0]="iy"; eng2ita[4][1]="i1";
	eng2ita[5][0]="ah"; eng2ita[5][1]="a1";
	eng2ita[6][0]="ax"; eng2ita[6][1]="a";
	eng2ita[7][0]="ae"; eng2ita[7][1]="e1";
	eng2ita[8][0]="ow"; eng2ita[8][1]="o1";
	eng2ita[9][0]="uw"; eng2ita[9][1]="u";
	eng2ita[10][0]="aw"; eng2ita[10][1]="a1"; 
	eng2ita[11][0]="uh"; eng2ita[11][1]="u1";
	eng2ita[12][0]="eh"; eng2ita[12][1]="E1";
	eng2ita[13][0]="aa"; eng2ita[13][1]="O1";
	eng2ita[14][0]="ao"; eng2ita[14][1]="a+o";
	eng2ita[15][0]="oy"; eng2ita[15][1]="o+i";
	eng2ita[16][0]="ay"; eng2ita[16][1]="a+i";
	eng2ita[17][0]="hh"; eng2ita[17][1]="g";
	eng2ita[18][0]="dh"; eng2ita[18][1]="d";
	eng2ita[19][0]="pp"; eng2ita[19][1]="p";
	eng2ita[20][0]="tt"; eng2ita[20][1]="t";
	eng2ita[21][0]="kk"; eng2ita[21][1]="k";
	eng2ita[22][0]="bb"; eng2ita[22][1]="b";
	eng2ita[23][0]="dd"; eng2ita[23][1]="d";
	eng2ita[24][0]="gg"; eng2ita[24][1]="g";
	eng2ita[25][0]="dZ"; eng2ita[25][1]="tS";
	eng2ita[26][0]="Z"; eng2ita[26][1]="tS";
	eng2ita[27][0]="ch"; eng2ita[27][1]="tS";
	eng2ita[28][0]="jh"; eng2ita[28][1]="tS";
	eng2ita[29][0]="ff"; eng2ita[29][1]="f";
	eng2ita[30][0]="ss"; eng2ita[30][1]="s";
	eng2ita[31][0]="sh"; eng2ita[31][1]="SS";
	eng2ita[32][0]="vv"; eng2ita[32][1]="v";
	eng2ita[33][0]="zh"; eng2ita[33][1]="z";
	eng2ita[34][0]="mm"; eng2ita[34][1]="m";
	eng2ita[35][0]="nn"; eng2ita[35][1]="n";
	eng2ita[36][0]="ll"; eng2ita[36][1]="l";
	eng2ita[37][0]="th"; eng2ita[37][1]="th";
	eng2ita[38][0]="ng"; eng2ita[38][1]="g";
	eng2ita[39][0]="r="; eng2ita[39][1]="r";
	eng2ita[40][0]="h"; eng2ita[40][1]="#";
	eng2ita[41][0]="E"; eng2ita[41][1]="e";
	eng2ita[42][0]="w"; eng2ita[42][1]="w";
	
	eng2ita[43][0]="V"; eng2ita[43][1]="a1";
	eng2ita[44][0]="i"; eng2ita[44][1]="i1";
	eng2ita[45][0]="_"; eng2ita[45][1]="#";
	eng2ita[46][0]="I"; eng2ita[46][1]="i";
	eng2ita[47][0]="U"; eng2ita[47][1]="u1";
	eng2ita[48][0]="{"; eng2ita[48][1]="e1";
	eng2ita[49][0]="@"; eng2ita[49][1]="a";
	eng2ita[50][0]="r="; eng2ita[50][1]="e";
	eng2ita[51][0]="A"; eng2ita[51][1]="O1";
	eng2ita[52][0]="O"; eng2ita[52][1]="a+o";
	eng2ita[53][0]="u"; eng2ita[53][1]="u";
	eng2ita[54][0]="E"; eng2ita[54][1]="E1";
	eng2ita[55][0]="EI"; eng2ita[55][1]="e+i";
	eng2ita[56][0]="AI"; eng2ita[56][1]="a+i";
	eng2ita[57][0]="OI"; eng2ita[57][1]="o+i";
	eng2ita[58][0]="aU"; eng2ita[58][1]="a1";
	eng2ita[59][0]="@U"; eng2ita[59][1]="o1";
	eng2ita[60][0]="j"; eng2ita[60][1]="i";
	eng2ita[61][0]="h"; eng2ita[61][1]="g";
	eng2ita[62][0]="N"; eng2ita[62][1]="g";
	eng2ita[63][0]="S"; eng2ita[63][1]="SS";
	eng2ita[64][0]="T"; eng2ita[64][1]="th";
	eng2ita[65][0]="Z"; eng2ita[65][1]="z";
	eng2ita[66][0]="D"; eng2ita[66][1]="d";
	eng2ita[67][0]="tS"; eng2ita[67][1]="tS";
	eng2ita[68][0]="dZ"; eng2ita[68][1]="tS";
	eng2ita[69][0]=NULL; eng2ita[69][1]=NULL;

	fra2ita[0][0]="#"; fra2ita[0][1]="#";
	fra2ita[1][0]="i"; fra2ita[1][1]="i";
	fra2ita[2][0]="e"; fra2ita[2][1]="e";
	fra2ita[3][0]="E"; fra2ita[3][1]="e";
	fra2ita[4][0]="a"; fra2ita[4][1]="a1";
	fra2ita[5][0]="o"; fra2ita[5][1]="o";
	fra2ita[6][0]="y"; fra2ita[6][1]="y";
	fra2ita[7][0]="O"; fra2ita[7][1]="o";
	fra2ita[8][0]="u"; fra2ita[8][1]="u";
	fra2ita[9][0]="o~"; fra2ita[9][1]="o";
	fra2ita[10][0]="a~"; fra2ita[10][1]="a"; 
	fra2ita[11][0]="H"; fra2ita[11][1]="u";
	fra2ita[12][0]="E"; fra2ita[12][1]="E1";
	fra2ita[13][0]="@"; fra2ita[13][1]="o";
	fra2ita[14][0]="9~"; fra2ita[14][1]="o";
	fra2ita[15][0]="2"; fra2ita[15][1]="a";
	fra2ita[16][0]="9"; fra2ita[16][1]="a";
	fra2ita[17][0]="@"; fra2ita[17][1]="e";
	fra2ita[18][0]="e~"; fra2ita[18][1]="o";
	fra2ita[19][0]="p"; fra2ita[19][1]="p";
	fra2ita[20][0]="t"; fra2ita[20][1]="t";
	fra2ita[21][0]="k"; fra2ita[21][1]="k";
	fra2ita[22][0]="b"; fra2ita[22][1]="b";
	fra2ita[23][0]="o~"; fra2ita[23][1]="o";
	fra2ita[24][0]="g"; fra2ita[24][1]="g";
	fra2ita[25][0]="Z"; fra2ita[25][1]="SS";
	fra2ita[26][0]="S"; fra2ita[26][1]="tS";
	fra2ita[27][0]="R"; fra2ita[27][1]="r";
	fra2ita[28][0]="j"; fra2ita[28][1]="i";
	fra2ita[29][0]="N"; fra2ita[29][1]="n";
	fra2ita[30][0]="s"; fra2ita[30][1]="s";
	fra2ita[31][0]="w"; fra2ita[31][1]="u+a";
	fra2ita[32][0]="v"; fra2ita[32][1]="v";
	fra2ita[33][0]="z"; fra2ita[33][1]="s";
	fra2ita[34][0]="m"; fra2ita[34][1]="m";
	fra2ita[35][0]="n"; fra2ita[35][1]="n";
	fra2ita[36][0]="l"; fra2ita[36][1]="l";
	fra2ita[37][0]="d"; fra2ita[37][1]="d";
	fra2ita[38][0]="f"; fra2ita[38][1]="f";
	fra2ita[39][0]=NULL; fra2ita[39][1]=NULL;
	fra2ita[40][0]="pau"; fra2ita[40][1]="#";

	de2it[0][0]="_"; de2it[0][1]="#";
	de2it[1][0]="I"; de2it[1][1]="i";
	de2it[2][0]="E"; de2it[2][1]="e";
	de2it[3][0]="a"; de2it[3][1]="a1";
	de2it[4][0]="O"; de2it[4][1]="o";
	de2it[5][0]="U"; de2it[5][1]="u1";
	de2it[6][0]="Y"; de2it[6][1]="u1";
	de2it[7][0]="9"; de2it[7][1]="o";
	de2it[8][0]="i:"; de2it[8][1]="i1";
	de2it[9][0]="e:"; de2it[9][1]="e";
	de2it[10][0]="E:"; de2it[10][1]="e";
	de2it[11][0]="a:"; de2it[11][1]="a1";
	de2it[12][0]="o:"; de2it[12][1]="o";
	de2it[13][0]="u:"; de2it[13][1]="u";
	de2it[14][0]="y:"; de2it[14][1]="u";
	de2it[15][0]="2:"; de2it[15][1]="o";
	de2it[16][0]="aI"; de2it[16][1]="a+i";
	de2it[17][0]="aU"; de2it[17][1]="a+u1";
	de2it[18][0]="OY"; de2it[18][1]="o";
	de2it[19][0]="@"; de2it[19][1]="E1";
	de2it[20][0]="6"; de2it[20][1]="E1";
	de2it[21][0]="?"; de2it[21][1]="#";
	de2it[22][0]="p"; de2it[22][1]="p";
	de2it[23][0]="b"; de2it[23][1]="b";
	de2it[24][0]="t"; de2it[24][1]="t";
	de2it[25][0]="d"; de2it[25][1]="d";
	de2it[26][0]="k"; de2it[26][1]="k";
	de2it[27][0]="g"; de2it[27][1]="g";
	de2it[28][0]="pf"; de2it[28][1]="p+f";
	de2it[29][0]="ts"; de2it[29][1]="z";
	de2it[30][0]="tS"; de2it[30][1]="z";
	de2it[31][0]="f"; de2it[31][1]="f";
	de2it[32][0]="v"; de2it[32][1]="v";
	de2it[33][0]="s"; de2it[33][1]="s";
	de2it[34][0]="z"; de2it[34][1]="z";
	de2it[35][0]="S"; de2it[35][1]="SS";
	de2it[36][0]="Z"; de2it[36][1]="SS";
	de2it[37][0]="C"; de2it[37][1]="i";
	de2it[38][0]="j"; de2it[38][1]="i";
	de2it[39][0]="x"; de2it[39][1]="g";
	de2it[40][0]="h"; de2it[40][1]="#";
	de2it[41][0]="m"; de2it[41][1]="m";
	de2it[42][0]="n"; de2it[42][1]="n";
	de2it[43][0]="N"; de2it[43][1]="g";
	de2it[44][0]="l"; de2it[44][1]="l";
	de2it[45][0]="R"; de2it[45][1]="g";
	de2it[46][0]="EI"; de2it[46][1]="e+i";
	de2it[47][0]="T"; de2it[47][1]="th";
	de2it[48][0]="D"; de2it[48][1]="th";
	de2it[49][0]="r"; de2it[49][1]="r";
	de2it[50][0]="w"; de2it[50][1]="w";
	de2it[51][0]="e~"; de2it[51][1]="e";
	de2it[52][0]="a~"; de2it[52][1]="o";
	de2it[53][0]="=6"; de2it[53][1]="#";
	de2it[54][0]="pau"; de2it[54][1]="#"; 
	de2it[55][0]=NULL; de2it[55][1]=NULL; 

	//TO ADD POLISH

	pol2ita[0][0]="#"; pol2ita[0][1]="#";
	pol2ita[1][0]="a"; pol2ita[1][1]="a";
	pol2ita[2][0]="e"; pol2ita[2][1]="e";
	pol2ita[3][0]="i"; pol2ita[3][1]="i";
	pol2ita[4][0]="I"; pol2ita[4][1]="n";
	pol2ita[5][0]="o"; pol2ita[5][1]="o";
	pol2ita[6][0]="u"; pol2ita[6][1]="u";
	pol2ita[7][0]="e~"; pol2ita[7][1]="n";
	pol2ita[8][0]="o~"; pol2ita[8][1]="o1";
	pol2ita[9][0]="p"; pol2ita[9][1]="p";
	pol2ita[10][0]="t"; pol2ita[10][1]="t"; 
	pol2ita[11][0]="d"; pol2ita[11][1]="d";
	pol2ita[12][0]="b"; pol2ita[12][1]="b";
	pol2ita[13][0]="k"; pol2ita[13][1]="k";
	pol2ita[14][0]="g"; pol2ita[14][1]="g";
	pol2ita[15][0]="f"; pol2ita[15][1]="f";
	pol2ita[16][0]="v"; pol2ita[16][1]="v";
	pol2ita[17][0]="N"; pol2ita[17][1]="n";
	pol2ita[18][0]="s"; pol2ita[18][1]="s";
	pol2ita[19][0]="z"; pol2ita[19][1]="z";
	pol2ita[20][0]="S"; pol2ita[20][1]="SS";
	pol2ita[21][0]="Z"; pol2ita[21][1]="SS";
	pol2ita[22][0]="s+"; pol2ita[22][1]="SS";
	pol2ita[23][0]="z+"; pol2ita[23][1]="SS";
	pol2ita[24][0]="x"; pol2ita[24][1]="k";
	pol2ita[25][0]="ts"; pol2ita[25][1]="tS";
	pol2ita[26][0]="dz"; pol2ita[26][1]="tS";
	pol2ita[27][0]="tS"; pol2ita[27][1]="SS";
	pol2ita[28][0]="dZ"; pol2ita[28][1]="tS";
	pol2ita[29][0]="ts+"; pol2ita[29][1]="tS";
	pol2ita[30][0]="dz+"; pol2ita[30][1]="tS";
	pol2ita[31][0]="m"; pol2ita[31][1]="m";
	pol2ita[32][0]="n"; pol2ita[32][1]="n";
	pol2ita[33][0]="n+"; pol2ita[33][1]="n";
	pol2ita[34][0]="ng"; pol2ita[34][1]="g";
	pol2ita[35][0]="l"; pol2ita[35][1]="l";
	pol2ita[36][0]="r"; pol2ita[36][1]="r";
	pol2ita[37][0]="w"; pol2ita[37][1]="w";
	pol2ita[38][0]="y"; pol2ita[38][1]="y";
	pol2ita[39][0]="j"; pol2ita[39][1]="y";
	pol2ita[40][0]=NULL; pol2ita[40][1]=NULL;

	//TO ADD SWEDISH

	swe2ita[0][0]="_"; swe2ita[0][1]="#";
	swe2ita[1][0]="i:"; swe2ita[1][1]="i1";
	swe2ita[2][0]="e:"; swe2ita[2][1]="e1";
	swe2ita[3][0]="E:"; swe2ita[3][1]="e1";
	swe2ita[4][0]="y:"; swe2ita[4][1]="o1";
	swe2ita[5][0]="}:"; swe2ita[5][1]="u1";
	swe2ita[6][0]="2:"; swe2ita[6][1]="o1";
	swe2ita[7][0]="u:"; swe2ita[7][1]="u1";
	swe2ita[8][0]="o:"; swe2ita[8][1]="o1";
	swe2ita[9][0]="A:"; swe2ita[9][1]="a1";
	swe2ita[10][0]="I"; swe2ita[10][1]="i"; 
	swe2ita[11][0]="e"; swe2ita[11][1]="e";
	swe2ita[12][0]="E"; swe2ita[12][1]="e";
	swe2ita[13][0]="Y"; swe2ita[13][1]="o";
	swe2ita[14][0]="u0"; swe2ita[14][1]="u";
	swe2ita[15][0]="2"; swe2ita[15][1]="o";
	swe2ita[16][0]="U"; swe2ita[16][1]="o";
	swe2ita[17][0]="O"; swe2ita[17][1]="o";
	swe2ita[18][0]="a"; swe2ita[18][1]="a";
	swe2ita[19][0]="{:"; swe2ita[19][1]="a1";
	swe2ita[20][0]="9:"; swe2ita[20][1]="o1";
	swe2ita[21][0]="{"; swe2ita[21][1]="e";
	swe2ita[22][0]="9"; swe2ita[22][1]="o";
	swe2ita[23][0]="@"; swe2ita[23][1]="e";
	swe2ita[24][0]="p"; swe2ita[24][1]="p";
	swe2ita[25][0]="b"; swe2ita[25][1]="b";
	swe2ita[26][0]="d"; swe2ita[26][1]="d";
	swe2ita[27][0]="t"; swe2ita[27][1]="t";
	swe2ita[28][0]="k"; swe2ita[28][1]="k";
	swe2ita[29][0]="g"; swe2ita[29][1]="g";
	swe2ita[30][0]="f"; swe2ita[30][1]="f";
	swe2ita[31][0]="v"; swe2ita[31][1]="v";
	swe2ita[32][0]="s"; swe2ita[32][1]="s";
	swe2ita[33][0]="S"; swe2ita[33][1]="S";
	swe2ita[34][0]="h"; swe2ita[34][1]="#";
	swe2ita[35][0]="C"; swe2ita[35][1]="S";
	swe2ita[36][0]="m"; swe2ita[36][1]="m";
	swe2ita[37][0]="n"; swe2ita[37][1]="n";
	swe2ita[38][0]="N"; swe2ita[38][1]="n";
	swe2ita[39][0]="r"; swe2ita[39][1]="r";
	swe2ita[40][0]="l"; swe2ita[40][1]="l";
	swe2ita[41][0]="j"; swe2ita[41][1]="y";
	swe2ita[42][0]="rt"; swe2ita[42][1]="S";
	swe2ita[43][0]="rd"; swe2ita[43][1]="S";
	swe2ita[44][0]="rn"; swe2ita[44][1]="n";
	swe2ita[45][0]="rs"; swe2ita[45][1]="S";
	swe2ita[46][0]="rl"; swe2ita[46][1]="r";
	swe2ita[47][0]=NULL; swe2ita[47][1]=NULL;

	
	for(i=0; i<50; i++)
	{
		sprintf(unknown_emotion[i],"");
	}

	strcpy(current_performative,"");
	strcpy(current_affect,"");


	index_UE=0;

	for (i=0; i<64; i++)
		if(isLipFap[i])
			KeyFrameInt[i] = 0;
}

LipModel::~LipModel()
{
	FreeAll();
	FAPPhonemes.clear();
	Phonemes.clear();
}

int LipModel::CalculateLips(std::string phonemefilename, float speech_delay)
{
	float dim, min=1.1f, max=1.8f;


	if(ENGINE_LIP_FLOW=="FREE"){
		min=1.7f;
		max=4.0f;
	}
	else 
		if(ENGINE_LIP_FLOW=="BOUND"){
			min=0.9f;
			max=1.6f;
		}
	
	
	dim = lipdata->direction(1, min, 4, max, inimanager.GetValueFloat("SPEECH_STRETCH"));


	if(Conversion(phonemefilename)==0)
		return 0;

	if(ComputeLipMovemet(speech_delay)==0)
		return 0;


	return 1;
}


///////////////////////////////////////////////////////////////////////////////
int LipModel::Conversion(std::string filePho)  //OLD
{
	FILE *fid; 
	char pho[5], pho1[5], pho2[5];
	char* pho3;
	float time, time1, time2;

	PhoData phoneme; 

	// convert phonemes from Italian to English

	// geminates are special cases

	/* -------------------------------------------------------------------
				word		SAMPA				English
	vowels

	i |	i1		finito		f i n i1 t o	I	vim			v I m	ih
	e |	e1		veloce		v e l e1 no		EI	h			EI tS	ey (ih)
	a |	a1		vanità  	v a n i t a1	V	cut			k V t	ah
	o |	o1		rotto		r o1 t t o		@U	over		@U v r=	ow	
	u |	u1		puntura		p u n t u1 r a	U	put			p U t	uh

	E |	E1		caffè		k a f f E1		E	bEt			b E t	eh	
	O |	O1		però		p e r O1		A	pot			p A t	aa

	In standard "tuscan" Italian E and O should never
	occurr in non-tonic position (unstressed).
	In "compund words" where the first component has a
	stressed open E/O vowel, given that only one stressed
	vowel should exist in all Italian words, the first open
	E/O vowel reduces to its close counterpart e/o as in:
	termo + sifone
	{t E1 r m o} + {s i f o1 n e}
	   |
	 t e r m o s i f o1 n e

	{p O1 z a} + {t u1 b i}
	   |
	 p o z a t u1 b i

	semiconsonants
	j			piume		p j u1 m e		j	yacht		jQt		y
	w			quando		k w a1 n d o	w	wasp		wQsp	w

	plosives
	p | pp		pera		p e1 r a		p	pin			pIn		p
	t | tt		torre		t o1 r r e		t	tin			tIn 	t
	k | kk		caldo		k a1 l d o		k	kin			kIn 	k
	b | bb		botte		b o1 t t e		b	bin			bIn		b
	d | dd		dente		d E1 n t e		d	din			dIn 	d
	g | gg		gatto		g a1 t t o		g	give		gIv 	g

	affricates
	ts | ts ts 	pizza		p i1 ts ts a	t+s pet-sin		pet sin	t+s
	tS | tS tS	pece		p e1 tS e		d+z pod-zing	pQd zIN	d+z

	dz | dz dz	zero		dz E1 r o		tS	chin		tSIn	ch
	dZ | dZ dZ	magia		m a dZ i1 a		dZ	gin			dZIn	jh

	fricatives
	f | ff		faro		f a1 r o		f	fin			fIn		f
	s | ss		sole		s o1 l e		s	sin			sIn		s
	S | SS		sci			S i1			S	shin		SIn 	sh
	v | vv		via			v i1 a			v	vim			vIm		v
	z			peso		p e1 s o		z	zing		zIN 	z
	Z			garage		g a r a1 Z	

	nasals
	m | mm		mano		m a1 n o		m	mock		mQk		m
	n | n		nave		n a1 v e		n	knock		nQk		n
	J | JJ		legna		l e1 J J a		dZ	gin			dZIn	jh
	nf		 	anfora		a1 nf f o r a	
	ng			ingordo		i ng g o1 r d o	

	liquids
	l | ll		palo		p a1 l o		l	long		lQN		l
	L | LL		soglia		s O1 L L a		l	long		lQN		l
	r | rr		remo		r E1 m o		dZ	gin			dZIn	jh

	#	PAUSE   _
	------------------------------------------------------------------- */

	fid = fopen(filePho.c_str(), "r");

	if(fid==0)
	{
		printf("LipModel: cannot open %s\n",filePho.c_str());
		return 0;
	}


	if (FESTIVAL=="1" && (FESTIVAL_LANGUAGE=="ITALIAN"))
	{
		fscanf(fid, "%s %f", pho1, &time1);
		strcpy(pho, "");
	
		while (fscanf(fid, "%s %f", pho2, &time2) != EOF)
		{		
			if ((strcmp("i", pho1)==0 || strcmp("j", pho1) == 0 || strcmp("i1", pho1)==0) && (strcmp("u", pho2)==0 || strcmp("u1", pho2)==0)) strcpy(pho1, "y");
			if (strcmp("k", pho)==0 && strcmp("w", pho1) == 0) strcpy(pho1, "u");
			if (strcmp("E", pho1) == 0) strcpy(pho1, "e");
			if (strcmp("O", pho1) == 0) strcpy(pho1, "o");

			if (strcmp("j", pho1) == 0) strcpy(pho1, "i1");
			if (strcmp("ts", pho1) == 0) strcpy(pho1, "z");
			if (strcmp("S", pho1) == 0) strcpy(pho1, "SS");

			if (strcmp(pho1, "dz") == 0) strcpy(pho1, "z");
			if (strcmp(pho1, "dZ") == 0 || strcmp(pho1, "Z") == 0) strcpy(pho1, "tS");
			if (strcmp(pho1, "nf") == 0 || strcmp(pho1, "J") == 0 || strcmp(pho1, "JJ") == 0 || strcmp(pho1, "ng") == 0) strcpy(pho1, "n");
			if (strcmp(pho1, "L") == 0 || strcmp(pho1, "LL") == 0) strcpy(pho1, "i1");
			
			strcpy(phoneme.phoneme,pho1);
			phoneme.time=time1;
			PhonemeVec.push_back(phoneme);
			
			strcpy(pho, pho1);
			strcpy(pho1, pho2);
			time1=time2;
		}
		strcpy(phoneme.phoneme,pho1);
		phoneme.time=time1;
		PhonemeVec.push_back(phoneme);
	}

	if ((FESTIVAL=="1"
			&&((FESTIVAL_LANGUAGE=="US_ENGLISH_FEMALE1")
			||(FESTIVAL_LANGUAGE=="US_ENGLISH_MALE1")
			||(FESTIVAL_LANGUAGE=="US_ENGLISH_MALE2")))
		||
		(MARY=="1" 
			&&((MARY_LANGUAGE=="ENGLISH_FEMALE1")
			||(MARY_LANGUAGE=="ENGLISH_MALE1")
			||(MARY_LANGUAGE=="ENGLISH_MALE2")))
		)		
	{
		while (fscanf(fid, "%s %f", pho, &time) != EOF)
		{
			ConvertEng2Ita(pho);
			pho3=strstr(pho,"+");

			if (time!=0&&!pho3) 
			{
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time;
				PhonemeVec.push_back(phoneme);
			}
			else
			if (time!=0)
			{
				pho3++;
				pho[pho3-pho-1]='\0';
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time/2;
				PhonemeVec.push_back(phoneme);
				strcpy(phoneme.phoneme,pho3);
				phoneme.time=time/2;
				PhonemeVec.push_back(phoneme);
			}
		}
	}

	if (EULER=="1"
			&& (EULER_LANGUAGE=="FRENCH_FEMALE1"
			|| EULER_LANGUAGE=="FRENCH_MALE1"
			|| EULER_LANGUAGE=="FRENCH_MALE2"))
	{
		while (fscanf(fid, "%s %f", pho, &time) != EOF)
		{
			if(ConvertFra2Ita(pho)==0)
				return 0;
			pho3=strstr(pho,"+");

			if (time!=0&&!pho3) 
			{
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time;
				PhonemeVec.push_back(phoneme);
			}
			else
			if (time!=0)
			{
					pho3++;
					pho[pho3-pho-1]='\0';
					strcpy(phoneme.phoneme,pho);
					phoneme.time=time/2;
					PhonemeVec.push_back(phoneme);
					strcpy(phoneme.phoneme,pho3);
					phoneme.time=time/2;
					PhonemeVec.push_back(phoneme);
			}
		}
	}

	if (MARY=="1" && (MARY_LANGUAGE=="GERMAN_FEMALE1"))
	{
		while (fscanf(fid, "%s %f", pho, &time) != EOF)
		{
			if(ConvertGer2Ita(pho)==0)
				return 0;
			pho3=strstr(pho,"+");

			if (time!=0&&!pho3) 
			{
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time;
				PhonemeVec.push_back(phoneme);
			}
			else
			if (time!=0)
			{
					pho3++;
					pho[pho3-pho-1]='\0';
					strcpy(phoneme.phoneme,pho);
					phoneme.time=time/2;
					PhonemeVec.push_back(phoneme);
					strcpy(phoneme.phoneme,pho3);
					phoneme.time=time/2;
					PhonemeVec.push_back(phoneme);
			}
		}
	}

	if (FESTIVAL=="1" && FESTIVAL_LANGUAGE=="POLISH")		
	{
		while (fscanf(fid, "%s %f", pho, &time) != EOF)
		{
			ConvertPol2Ita(pho);
			pho3=strstr(pho,"+");

			if (time!=0&&!pho3) 
			{
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time;
				PhonemeVec.push_back(phoneme);
			}
			else
			if (time!=0)
			{
				pho3++;
				pho[pho3-pho-1]='\0';
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time/2;
				PhonemeVec.push_back(phoneme);
				strcpy(phoneme.phoneme,pho3);
				phoneme.time=time/2;
				PhonemeVec.push_back(phoneme);
			}
		}
	}

	if (FESTIVAL=="1" && FESTIVAL_LANGUAGE=="SWEDISH")		
	{
		while (fscanf(fid, "%s %f", pho, &time) != EOF)
		{
			ConvertSwe2Ita(pho);
			pho3=strstr(pho,"+");

			if (time!=0&&!pho3) 
			{
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time;
				PhonemeVec.push_back(phoneme);
			}
			else
			if (time!=0)
			{
				pho3++;
				pho[pho3-pho-1]='\0';
				strcpy(phoneme.phoneme,pho);
				phoneme.time=time/2;
				PhonemeVec.push_back(phoneme);
				strcpy(phoneme.phoneme,pho3);
				phoneme.time=time/2;
				PhonemeVec.push_back(phoneme);

			}
		}
	}

	fclose(fid);

	return 1;
}

void LipModel::ConvertEng2Ita(char* pho) 
{
	int i;
	int found=0;


	for (i=0;!found&&eng2ita[i][0]!=NULL;i++)
	{
		if (!strcmp(eng2ita[i][0],pho))
		{

			found=1;		
			strcpy(pho,eng2ita[i][1]);
		}
	}
}

int LipModel::ConvertFra2Ita(char* pho) 
{
	int i;
	int found=0;


	for (i=0;!found&&fra2ita[i][0]!=NULL;i++)
	{
		if (!strcmp(fra2ita[i][0],pho))
		{

			found=1;		
			strcpy(pho,fra2ita[i][1]);
		}
	}
	if(!found)
	{
		printf("LipModel: unknown french phoneme \"%s\"\n",pho);
		return 0;
	}
	return 1;
}

int LipModel::ConvertGer2Ita(char* pho)
{
	int i;
	int found=0;


	for (i=0;!found&&de2it[i][0]!=NULL;i++)
	{
		if (!strcmp(de2it[i][0],pho))
		{

			found=1;		
			strcpy(pho,de2it[i][1]);
		}
	}
	if(!found)
	{
		printf("LipModel: unknown German phoneme \"%s\"\n",pho);
		return 0;
	}
	return 1;
}

//TO ADD POLISH
int LipModel::ConvertPol2Ita(char* pho) 
{
	int i;
	int found=0;


	for (i=0;!found&&pol2ita[i][0]!=NULL;i++)
	{
		if (!strcmp(pol2ita[i][0],pho))
		{

			found=1;		
			strcpy(pho,pol2ita[i][1]);
		}
	}
	if(!found)
	{
		printf("LipModel: unknown polish phoneme \"%s\"\n",pho);
		return 0;
	}
	return 1;
}

//TO ADD SWEDISH
int LipModel::ConvertSwe2Ita(char* pho)
{
	int i;
	int found=0;


	for (i=0;!found&&swe2ita[i][0]!=NULL;i++)
	{
		if (!strcmp(swe2ita[i][0],pho))
		{

			found=1;		
			strcpy(pho,swe2ita[i][1]);
		}
	}
	if(!found)
	{
		printf("LipModel: unknown swedish phoneme \"%s\"\n",pho);
		return 0;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////

std::string LipModel::ConvertIta2Ita(std::string pho)  
{
	int i;
	int found=0;

	for (i=0;!found&&ita2ita[i][0]!=NULL;i++)
	{
		if (ita2ita[i][0]==pho)
		{
			found=1;		
			return(ita2ita[i][1]);
		}
	}
	if(!found)
	{
		printf("LipModel: unknown italian phoneme \"%s\"\n",pho);
		return "";
	}
	return "";
}

float LipModel::CalculateRate()
{
	float rate, min=1.1f, max=1.8f;

	if(ENGINE_LIP_FLOW=="FREE")
	{
		min=1.7f;
		max=4.0f;
	}
	else 
		if(ENGINE_LIP_FLOW=="BOUND")
		{
			min=0.9f;
			max=1.6f;
		}
	
	
	rate = lipdata->direction(1, min, 4, max, inimanager.GetValueFloat("SPEECH_STRETCH"));
	return(rate);
}


// Load list of phonemes generates by the TTS
float LipModel::ReadPhonemes()
{
	int lastphoneme=0;
	char pho[5];
	float rate;
	float time, par_time = 0.0;
	std::vector<PhoData>::iterator iterpho;
	Phoneme* newPhoneme;

	Phoneme *vow=NULL;
	Phoneme *cons=NULL;
	
	for(iterpho=PhonemeVec.begin();iterpho!=PhonemeVec.end();iterpho++)
	{
		strcpy(pho,(*iterpho).phoneme);
		time=(*iterpho).time;
		
		newPhoneme=new Phoneme(lipdata, pho,par_time*NUM_POINTS, (par_time+time)*NUM_POINTS, this->first_comm_act);

		if (is_vowel(pho)) //vowel
		{
			vow=newPhoneme;
			newPhoneme->PhonemePrec=cons;
			Phonemes.push_back(*newPhoneme);
			lastphoneme=1;
		}
		else {
			if(strcmp(pho,"#")==0) //pause
			{
				if(lastphoneme==1) 
					newPhoneme->PhonemePrec=vow;
				else
					newPhoneme->PhonemePrec=cons;
				newPhoneme->Duration=((par_time+time+par_time)/2)*NUM_POINTS;
				Phonemes.push_back(*newPhoneme);
				cons=newPhoneme;

				newPhoneme=new Phoneme(lipdata, pho,((par_time+time+par_time)/2)*NUM_POINTS, (par_time+time)*NUM_POINTS, this->first_comm_act);
				newPhoneme->PhonemePrec=cons;
				Phonemes.push_back(*newPhoneme);
				cons=newPhoneme;
				vow=newPhoneme;
			}
			else //consonant
				{
					cons=newPhoneme;
					newPhoneme->PhonemePrec=vow;
					Phonemes.push_back(*newPhoneme);
					lastphoneme=2;
				}
		}

		par_time += time;
	}

	//put last data in phoneme vector and load vowels, consonants and tongue targets
	vow=NULL;
	cons=NULL;

	rate=CalculateRate();
	
	std::vector<Phoneme>::iterator iterphonemes;
	iterphonemes=Phonemes.begin();
	std::string lastemotion=(*iterphonemes).Emotion;

	for(iterphonemes=Phonemes.end()-1;iterphonemes>=Phonemes.begin();iterphonemes--)
	{
		if(is_vowel((*iterphonemes).Pho))  //vowel
		{
			(*iterphonemes).PhonemeSuc=cons;
			vow=&(*iterphonemes);
			lastphoneme=1;
			(*iterphonemes).LoadVowelTargets(rate);
		}
		else {
			if((*iterphonemes).Pho=="#") //pause
			{
				if(lastphoneme==1) 
					(*iterphonemes).PhonemeSuc=vow;
				else
					(*iterphonemes).PhonemeSuc=cons;
				vow=&(*iterphonemes);
				cons=&(*iterphonemes);
				lastphoneme=2;
				(*iterphonemes).LoadConsonantTargets(rate);
			}
			else //consonant
				{
					(*iterphonemes).PhonemeSuc=vow;
					cons=&(*iterphonemes);
					lastphoneme=2;
					(*iterphonemes).LoadConsonantTargets(rate);
				}
		}
	}

	// smoothing passage between two adiacent emotions (non c'entra niente col blending emotion di Radek!)
	std::vector<Phoneme>::iterator iterphonemes1;
	iterphonemes1=Phonemes.begin();
	std::vector<Phoneme>::iterator run;

	for(iterphonemes=Phonemes.begin()+1;iterphonemes!=Phonemes.end();iterphonemes++)
	{
		if((*iterphonemes1).Emotion!=(*iterphonemes).Emotion)
		{
			if((*iterphonemes1).BlendingCoeff!=1.0 && (*iterphonemes).BlendingCoeff!=1.0)
			{
				run=iterphonemes1;
				while((*run).BlendingCoeff<1 && run>=Phonemes.begin())
				{
					(*run).BlendWithEmotion=(*iterphonemes).Emotion;
					(*run).BlendingCoeff=(*run).BlendingCoeff*0.5 + 0.5;

					run-=1;
				}
				run=iterphonemes;
				while((*run).BlendingCoeff<1 && run<Phonemes.end())
				{
					(*run).BlendWithEmotion=(*iterphonemes1).Emotion;
					(*run).BlendingCoeff=(*run).BlendingCoeff*0.5 + 0.5;
					run+=1;
				}
			}
		}
		else
		{
			if(((*iterphonemes1).comm_act!=NULL && (*iterphonemes).comm_act!=NULL) && ((*iterphonemes1).comm_act!=(*iterphonemes).comm_act) && ((*iterphonemes1).BlendingCoeff<1.0 && (*iterphonemes).BlendingCoeff<1.0))
			{
				run=iterphonemes1;
				while((*run).BlendingCoeff<1 && run>=Phonemes.begin())
				{
					(*run).BlendingCoeff=1.0;
					run-=1;
				}
				run=iterphonemes;
				while((*run).BlendingCoeff<1 && run<Phonemes.end())
				{
					(*run).BlendingCoeff=1.0;
					run+=1;
				}
			}
		}

		iterphonemes1+=1;
	}

	return(par_time);
}
	

int LipModel::ComputeLipMovemet(float istant)
{
	std::vector<Phoneme>::iterator iterphonemes;

	animationtime=ReadPhonemes();

	//CHRIS fix	
	std::vector<Phoneme>::iterator iterphonemesBegin;
	std::vector<Phoneme>::iterator iterphonemesEnd;
	iterphonemesBegin = Phonemes.begin();
	iterphonemesEnd = Phonemes.end();

	// Coarticulation effects on phonemes
	for(iterphonemes=Phonemes.begin();iterphonemes!=Phonemes.end();iterphonemes++)
	{
		if (is_vowel((*iterphonemes).Pho))
			Vowel_Coarticulation(iterphonemes, iterphonemesBegin, iterphonemesEnd);
		else
		{

			if ((*iterphonemes).Pho!="#") 
			{
				if ((*iterphonemes).PhonemePrec != NULL && (*iterphonemes).PhonemeSuc != NULL)
				{
					Consonant_Coarticulation(iterphonemes);
					if((*iterphonemes).Pho=="tS" || (*iterphonemes).Pho=="SS" || (*iterphonemes).Pho=="q" || (*iterphonemes).Pho=="w" || 
						(*iterphonemes).Pho=="b" || (*iterphonemes).Pho=="m"  || (*iterphonemes).Pho=="p" || (*iterphonemes).Pho=="f" || 
						(*iterphonemes).Pho=="v")
						Cons_Cons_Coarticulation(iterphonemes, iterphonemesEnd);
				}
			}
		}
	}


	for(iterphonemes=Phonemes.begin();iterphonemes!=Phonemes.end();iterphonemes++)
	{
		//if(LIP_EMOTION_MODEL=="DATA")
		//	lp2fapDataEmo(&head_fap_phoneme, &tail_fap_phoneme, &temp_head_phonemes);	
		//else
			Lip2Fap(iterphonemes, iterphonemesEnd);	
	}


	InterpolateLipFap();

	StoreLipFaps(istant);
	
	return 1;
}

int LipModel::InterpolateLipFap()
{
	int i;
	
	// Interpolators for lip
	for (i=0; i<64; i++)
		if(isLipFap[i])
			KeyFrameInt[i] = new TCBInterpolator(1,0,0);


	std::vector<FAPPhoneme>::iterator iter;

	for(iter=FAPPhonemes.begin(); iter!=FAPPhonemes.end(); iter++)
	{
		if ((iter==FAPPhonemes.begin()) && ((*iter).phoneme!="#"))
		{
			printf("The first phoneme is not a pause. It MUST be a pause!\n");
			return 0;
		}
		if ((*iter).phoneme=="#") 
		{
			//add the points of the pause 
			for (i=0; i<64; i++)
			{
				if(isLipFap[i]) 
				{
					KeyFrameInt[i]->AddPointNoSort((*iter).time[0][i],(*iter).target[0][i]);
				}
			}
		}
		else
		{
			if (is_vowel((*iter).phoneme))
			{
				if ((*(iter-1)).phoneme=="#") 
				{
					for (i=0; i<64; i++)
						if(isLipFap[i]) 
						{
							KeyFrameInt[i]->AddPointNoSort((*iter).time[0][i],(*iter).target[0][i]);
							KeyFrameInt[i]->AddPointNoSort((*iter).time[1][i],(*iter).target[1][i]);
						}
				}
				else
				{
					if (is_vowel((*(iter-1)).phoneme)) {
						for (i=0; i<64; i++) 
							if(isLipFap[i])
							{
								KeyFrameInt[i]->AddPointNoSort((*iter).time[1][i],(*iter).target[1][i]);
							}
					}
					else {
						//preceduta da consonante
						for (i=0; i<64; i++) 
							if(isLipFap[i]) {
								if (((*(iter-1)).target[0][i]<(*iter).target[0][i] && (*iter).target[0][i]<(*iter).target[1][i]) || ((*(iter-1)).target[0][i]>(*iter).target[0][i] && (*iter).target[0][i]>(*iter).target[1][i])) 
								{
									KeyFrameInt[i]->AddPointNoSort((*iter).time[0][i],(*iter).target[0][i]);
								}
								else 
								{
									KeyFrameInt[i]->AddPointNoSort((*iter).time[0][i],((*(iter-1)).target[0][i]+(*iter).target[1][i])/2.0);
								}
								KeyFrameInt[i]->AddPointNoSort((*iter).time[1][i],(*iter).target[1][i]);

							}
					}

					if ((*(iter+1)).phoneme=="#")
						for (i=0; i<64; i++) {
							if(isLipFap[i])
							{
								KeyFrameInt[i]->AddPointNoSort((*iter).time[2][i],(*iter).target[2][i]);
							}
						}
					else
						if (!(is_vowel((*(iter+1)).phoneme)))
						{
							//seguita da consonante
							for (i=0; i<64; i++)
								if(isLipFap[i])
									if (((*(iter+1)).target[0][i]<(*iter).target[2][i] && (*iter).target[2][i]<(*iter).target[1][i]) || ((*(iter+1)).target[0][i]>(*iter).target[2][i] && (*iter).target[2][i]>(*iter).target[1][i])) 
									{
										KeyFrameInt[i]->AddPointNoSort((*iter).time[2][i],(*iter).target[2][i]);
									}
									else
									{
										KeyFrameInt[i]->AddPointNoSort((*iter).time[2][i],((*(iter+1)).target[0][i] + (*iter).target[1][i])/2.0);
									}
						}
				}
			}
			else 
			{
				//inserisco punti consonante
				for (i=0; i<64; i++)
					if(isLipFap[i])
					{
						KeyFrameInt[i]->AddPointNoSort((*iter).time[0][i],(*iter).target[0][i]);
					}
			}
		}
	}


	for(i=0; i<64; i++) 
		if(isLipFap[i])
			if(KeyFrameInt[i]->points.size()>3)
				KeyFrameInt[i]->Prepare();
}


// **********************	IS_VOWEL	**************
//	Indica se la stringa in input e' una vocale o no

// ***************************************************
int LipModel::is_vowel(std::string inSeq)
{
	if (
		(inSeq=="y") || 
		(inSeq=="a") || 
		(inSeq=="e") || 
		(inSeq=="i") || 
		(inSeq=="o") || 
		(inSeq=="u") || 
		(inSeq=="a1") || 
		(inSeq=="e1") || 
		(inSeq=="i1") || 
		(inSeq=="o1") || 
		(inSeq=="u1") || 
		(inSeq=="E1") || 
		(inSeq=="O1")
		)
		return(1);

	else 
		return(0);
}




// **********************************	MODIFY_VOW_TARGET   ********************************
//	It modifies vowels targets looking at the adjacent consonants and vowels
// *****************************************************************************************
void LipModel::Vowel_Coarticulation(std::vector<Phoneme>::iterator phoneme, std::vector<Phoneme>::iterator vectorBegin, std::vector<Phoneme>::iterator vectorEnd)
{

	//FILE *fid;
	int i, j;
	float x;
	std::string FC=(*phoneme).PhonemePrec->Pho;
	std::string LC=(*phoneme).PhonemeSuc->Pho;


	//ULO LLO
	if ((LC=="b" || LC=="m" || LC=="p" || LC=="f" || LC=="v" || LC=="tS" || LC=="SS" || LC=="q" || LC=="w") && !(FC=="b" || FC=="m" || FC=="p" || FC=="f" || FC=="v" || FC=="tS" || FC=="SS" || FC=="q" || FC=="w")) {
		for (j=0; j<=2; j++) {
			for (i=0; i<3; i++) {
				x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
				(*phoneme).TargetLeft[j][i] = A_coart(MILD,x)*(*phoneme).TargetRight[j][i] + (1 - A_coart(MILD,x))*(*phoneme).TargetLeft[j][i];	
			}
		}
	} 
	else
		if (FC=="b" || FC=="m" || FC=="p" || FC=="f" || FC=="v" || FC=="tS" || FC=="SS" || FC=="q" || FC=="w") {
			for (j=0; j<=2; j++) {
				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = CO_coart(MILD,x)*(*phoneme).TargetLeft[j][i] + (1 - CO_coart(MILD,x))*(*phoneme).TargetRight[j][i];	
				}
			}
		}
		else {
			// all the other consonants
			for (j=0; j<=2; j++) {
				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = (1-x)*(*phoneme).TargetLeft[j][i] + x*(*phoneme).TargetRight[j][i];	
				}

			}
		}
		
		//LW
	if ((LC=="tS" || LC=="SS" || LC=="q" || LC=="w") && !(FC=="tS" || FC=="SS" || FC=="q" || FC=="w")) {
		for (j=3; j<=3; j++) {
			for (i=0; i<3; i++) {
				x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
				(*phoneme).TargetLeft[j][i] = A_coart(STRONG,x)*(*phoneme).TargetRight[j][i] + (1 - A_coart(STRONG,x))*(*phoneme).TargetLeft[j][i];	
			}
		}

	} 
	else
		if (FC=="tS" || FC=="SS" || FC=="q" || FC=="w") {
			for (j=3; j<=3; j++) {
				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = CO_coart(STRONG,x)*(*phoneme).TargetLeft[j][i] + (1 - CO_coart(STRONG,x))*(*phoneme).TargetRight[j][i];	
				}
			}
		}
		else {
			// all the other consonants
			for (j=3; j<=3; j++) {
				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = (1-x)*(*phoneme).TargetLeft[j][i] + x*(*phoneme).TargetRight[j][i];	
				}
			}
		}



	//ULP LLP
	if ((LC=="tS" || LC=="SS" || LC=="q" || LC=="w" || LC=="f" || LC=="v") && !(FC=="tS" || FC=="SS" || FC=="q" || FC=="w" || FC=="f" || FC=="v")) {
		for (j=4; j<=5; j++) {
			for (i=0; i<3; i++) {
				x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
				(*phoneme).TargetLeft[j][i] = A_coart(STRONG,x)*(*phoneme).TargetRight[j][i] + (1 - A_coart(STRONG,x))*(*phoneme).TargetLeft[j][i];	
			}
		}
	} 
	else
		if (FC=="tS" || FC=="SS" || FC=="q" || FC=="w" || FC=="f" || FC=="v") {
			for (j=4; j<=5; j++) {
				for (i=0; i<3; i++) {

					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = CO_coart(STRONG,x)*(*phoneme).TargetLeft[j][i] + (1 - CO_coart(STRONG,x))*(*phoneme).TargetRight[j][i];	
				}
			}
		}

		else {
			//all the other consonants
			for (j=4; j<=5; j++) {

				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = (1-x)*(*phoneme).TargetLeft[j][i] + x*(*phoneme).TargetRight[j][i];	
				}
			}
		}

	//LC
	if ((LC=="tS" || LC=="SS" || LC=="q" || LC=="w" || LC=="f" || LC=="v") && !(FC=="tS" || FC=="SS" || FC=="q" || FC=="w" || FC=="f" || FC=="v")) {
		for (j=6; j<=6; j++) {
			for (i=0; i<3; i++) {
				x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
				(*phoneme).TargetLeft[j][i] = A_coart(STRONG,x)*(*phoneme).TargetRight[j][i] + (1 - A_coart(STRONG,x))*(*phoneme).TargetLeft[j][i];	

			}
		}
	} 
	else
		if (FC=="tS" || FC=="SS" || FC=="q" || FC=="w" || FC=="f" || FC=="v") {
			for (j=6; j<=6; j++) {
				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = CO_coart(STRONG,x)*(*phoneme).TargetLeft[j][i] + (1 - CO_coart(STRONG,x))*(*phoneme).TargetRight[j][i];	
				}
			}
		}
		else {
			//all the other consonants
			for (j=6; j<=6; j++) {
				for (i=0; i<3; i++) {
					x = ((*phoneme).Time[j][i] - (*phoneme).PhonemePrec->Time[j][0])/((*phoneme).PhonemeSuc->Time[j][0] - (*phoneme).PhonemePrec->Time[j][0]);
					(*phoneme).TargetLeft[j][i] = (1-x)*(*phoneme).TargetLeft[j][i] + x*(*phoneme).TargetRight[j][i];	

				}
			}
		}


	//vocali adiacenti ad altre vocali che si influenzano sulla protrusione e sulla larghezza labiale
	
	//CHRIS fix I assume that original code checks to see if 'phoneme' is the first element of the list
	//if(phoneme-1 != NULL)
	if (phoneme != vectorBegin)
	{

		if (((*(phoneme-1)).Pho=="u" || (*(phoneme-1)).Pho=="u1" || (*(phoneme-1)).Pho=="o" || 
			(*(phoneme-1)).Pho=="o1" || (*(phoneme-1)).Pho=="O1" || (*(phoneme-1)).Pho=="y") &&
			((*phoneme).Pho!="u" && (*phoneme).Pho!="u1" && (*phoneme).Pho!="o" && (*phoneme).Pho!="o1" && 
			(*phoneme).Pho!="O1" && (*phoneme).Pho!="y")) 
		{
			for (j=0; j<=6; j++) {
				for (i=0; i<3; i++) {
					(*phoneme).TargetLeft[j][i] = (float)((*(phoneme-1)).TargetRight[j][i]+(*phoneme).TargetLeft[j][i])/2;
				}
			}
		}
	}

	//CHRIS MSVC2005 fix: look for past the end
	//if(phoneme+1 != NULL)
	if(phoneme+1 != vectorEnd)
	{
		if (((*(phoneme+1)).Pho=="u" || (*(phoneme+1)).Pho=="u1" || (*(phoneme+1)).Pho=="o" || 
			(*(phoneme+1)).Pho=="o1" || (*(phoneme+1)).Pho=="O1" || (*(phoneme+1)).Pho=="y") &&
			((*phoneme).Pho!="u" && (*phoneme).Pho!="u1" && (*phoneme).Pho!="o" && (*phoneme).Pho!="o1" && 
			(*phoneme).Pho!="O1" && (*phoneme).Pho!="y"))
		{
			
			for (j=0; j<=6; j++) {
				for (i=0; i<3; i++) {
					(*phoneme).TargetRight[j][i] = (float)((*(phoneme+1)).TargetLeft[j][i]+(*phoneme).TargetRight[j][i])/2;
				}
			}
		}
	}

}

// **********************************	MODIFY_TARGET   ********************************
//	It modifies consonantal targets looking at the adjacent vowels
// *************************************************************************************
void LipModel::Consonant_Coarticulation(std::vector<Phoneme>::iterator inPho)
{

	int i;
	float x;
	std::string FV;
	std::string LV;

	if((*inPho).PhonemePrec->Pho=="#") FV="e";
		else FV=set_vowel((*inPho).PhonemePrec->Pho);
	if((*inPho).PhonemeSuc->Pho=="#") LV="e";
		else LV=set_vowel((*inPho).PhonemeSuc->Pho);


	//ULH - LLH - JAW

	if (FV=="a") {
		for (i=0; i<=2; i++) {
			x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
			(*inPho).TargetLeft[i][0] = CO_coart(STRONG,x)*(*inPho).TargetLeft[i][0] + (1 - CO_coart(STRONG,x))*(*inPho).TargetRight[i][0];	
		}
	}
	if (LV=="a") {
		for (i=0; i<=2; i++) {
			x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
			(*inPho).TargetLeft[i][0] = A_coart(STRONG,x)*(*inPho).TargetRight[i][0] + (1 - A_coart(STRONG,x))*(*inPho).TargetLeft[i][0];	
		}
	}



	if ((FV=="e" || FV=="o") && (LV=="u" || LV=="i" || LV=="y")) {
		for (i=0; i<=2; i++) {
			x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
			(*inPho).TargetLeft[i][0] = CO_coart(MILD,x)*(*inPho).TargetLeft[i][0] + (1 - CO_coart(MILD,x))*(*inPho).TargetRight[i][0];	

		}
	}
	if ((FV=="u" || FV=="i" || FV=="y") && (LV=="e" || LV=="o")) {
		for (i=0; i<=2; i++) {
			x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
			(*inPho).TargetLeft[i][0] = A_coart(STRONG,x)*(*inPho).TargetRight[i][0] + (1 - A_coart(STRONG,x))*(*inPho).TargetLeft[i][0];	
		}
	}


	if ((FV=="e" || FV=="o") && (LV=="o" || LV=="e")) {
		for (i=0; i<=2; i++) {
			x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
			(*inPho).TargetLeft[i][0] = (1-x)*(*inPho).TargetLeft[i][0] + x*(*inPho).TargetRight[i][0];	
		}
	}

	if ((FV=="u" || FV=="i" || FV=="y") && (LV=="u" || LV=="i" || LV=="y")) {
		for (i=0; i<=2; i++) {
			x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
			(*inPho).TargetLeft[i][0] = (1-x)*(*inPho).TargetLeft[i][0] + x*(*inPho).TargetRight[i][0];	
		}
	}


	
	if ((*inPho).Pho!="tS" && (*inPho).Pho!="SS" && (*inPho).Pho!="q" && (*inPho).Pho!="w")
	{		
		//LW
		if (LV=="a" && (FV=="e" || FV=="i" || FV=="u" || FV=="o" || FV=="y")) {
			for (i=3; i<=3; i++) {
				x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
				(*inPho).TargetLeft[i][0] = CO_coart(STRONG,x)*(*inPho).TargetLeft[i][0] + (1 - CO_coart(STRONG,x))*(*inPho).TargetRight[i][0];	
			}
		}
		if (FV=="a" && (LV=="e" || LV=="i" || LV=="u" || LV=="o" || LV=="y")) {
			for (i=3; i<=3; i++) {
				x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
				(*inPho).TargetLeft[i][0] = A_coart(STRONG,x)*(*inPho).TargetRight[i][0] + (1 - A_coart(STRONG,x))*(*inPho).TargetLeft[i][0];	
			}
		}


		if((FV=="e" || FV=="i" || FV=="u" || FV=="o" || FV=="y") && (LV=="e" || LV=="i" || LV=="u" || LV=="o" || LV=="y")) {
			for (i=3; i<=3; i++) {
				x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
				(*inPho).TargetLeft[i][0] = (1-x)*(*inPho).TargetLeft[i][0] + x*(*inPho).TargetRight[i][0];	
			}
		}



		//ULP - LLP

	

		if ((FV=="u" || FV=="o" || FV=="y") && (LV=="u" || LV=="o" || LV=="y" || LV=="a" || LV=="e" || LV=="i")) {
			for (i=4; i<=5; i++) {
				x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
				(*inPho).TargetLeft[i][0]= CO_coart(STRONG,x)*(*inPho).TargetLeft[i][0] + (1 - CO_coart(STRONG,x))*(*inPho).TargetRight[i][0];	
			}
		}


		if ((LV=="u" || LV=="o" || LV=="y") && (FV=="u" || FV=="o" || FV=="y" || FV=="a" || FV=="e" || FV=="i")) {

			for (i=4; i<=5; i++) {
				x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
				(*inPho).TargetLeft[i][0] = A_coart(STRONG,x)*(*inPho).TargetRight[i][0] + (1 - A_coart(STRONG,x))*(*inPho).TargetLeft[i][0];	
			}
		}


		if ((FV=="a" || FV=="e" || FV=="i") && (LV=="a" || LV=="e" || LV=="i")) {
			for (i=4; i<=5; i++) {

				x = ((*inPho).Time[i][0] - (*inPho).PhonemePrec->Time[i][1])/((*inPho).PhonemeSuc->Time[i][1] - (*inPho).PhonemePrec->Time[i][1]);
				(*inPho).TargetLeft[i][0] = (1-x)*(*inPho).TargetLeft[i][0] + x*(*inPho).TargetRight[i][0];	
			}
		}

		//LC

		if ((FV=="u" || FV=="o" || FV=="y") && (LV=="u" || LV=="o" || LV=="y" || LV=="a" || LV=="e" || LV=="i"))
		{
			x = ((*inPho).Time[6][0] - (*inPho).PhonemePrec->Time[6][1])/((*inPho).PhonemeSuc->Time[6][1] - (*inPho).PhonemePrec->Time[6][1]);
			(*inPho).TargetLeft[6][0]= CO_coart(STRONG,x)*(*inPho).TargetLeft[6][0] + (1 - CO_coart(STRONG,x))*(*inPho).TargetRight[6][0];	
		}


		if ((LV=="u" || LV=="o" || LV=="y") && (FV=="u" || FV=="o" || FV=="y" || FV=="a" || FV=="e" || FV=="i"))
		{
			x = ((*inPho).Time[6][0] - (*inPho).PhonemePrec->Time[6][1])/((*inPho).PhonemeSuc->Time[6][1] - (*inPho).PhonemePrec->Time[6][1]);
			(*inPho).TargetLeft[6][0] = A_coart(STRONG,x)*(*inPho).TargetRight[6][0] + (1 - A_coart(STRONG,x))*(*inPho).TargetLeft[6][0];	
		}


		if ((FV=="a" || FV=="e" || FV=="i") && (LV=="a" || LV=="e" || LV=="i"))
		{
			x = ((*inPho).Time[6][0] - (*inPho).PhonemePrec->Time[6][1])/((*inPho).PhonemeSuc->Time[6][1] - (*inPho).PhonemePrec->Time[6][1]);
			(*inPho).TargetLeft[6][0] = (1-x)*(*inPho).TargetLeft[6][0] + x*(*inPho).TargetRight[6][0];
		}
	}
}

// ******************	MODIFY_BIS   ********************************
//	Some consonants influence adjacent consonants
// ******************************************************************
void LipModel::Cons_Cons_Coarticulation(std::vector<Phoneme>::iterator inPho, std::vector<Phoneme>::iterator vectorEnd)
{

	int i;
	float x;
	std::vector<Phoneme>::iterator itertemp;

	if((*inPho).Pho=="tS" || (*inPho).Pho=="SS" || (*inPho).Pho=="q" || (*inPho).Pho=="w") {
		if ((*inPho).PhonemePrec!=NULL) {
			itertemp=inPho-1;
			
		//CHRIS MSVC2005 fix: look for past the end
			while((*itertemp).Pho!=(*inPho).PhonemePrec->Pho && itertemp!=vectorEnd) //&& itertemp!=NULL) 
			{	
				if ((*itertemp).Pho!="tS" && (*itertemp).Pho!="SS" && (*itertemp).Pho!="w" && (*itertemp).Pho!="q") {
					for (i=0; i<=6; i++) {
						x=((*itertemp).Time[i][0] - (*inPho).PhonemePrec->StartTime)/((*inPho).Time[i][0] - (*inPho).PhonemePrec->StartTime);
						(*itertemp).TargetLeft[i][0] = A_coart(WEAK,(1-x))*(*inPho).TargetLeft[i][0];
					}
				}

				itertemp-=1;
			}
		}

		if ((*inPho).PhonemeSuc!=NULL) {

			itertemp=inPho+1;

			//CHRIS MSVC2005 fix: look for past the end
			while((*itertemp).Pho!=(*inPho).PhonemeSuc->Pho && itertemp!=vectorEnd) // && itertemp!=NULL)
			{
				if ((*itertemp).Pho!="tS" && (*itertemp).Pho!="SS" && (*itertemp).Pho!="w" && (*itertemp).Pho!="q") {
					for (i=0; i<=6; i++) {
						x=((*itertemp).Time[i][0] - (*inPho).Time[i][0])/((*inPho).PhonemeSuc->Duration - (*inPho).TargetLeft[i][0]);
						(*itertemp).TargetLeft[i][0] = CO_coart(WEAK,(x))*(*inPho).TargetLeft[i][0];	
					}
				}
				itertemp+=1;
			}
		}
	}



	if((*inPho).Pho=="b" || (*inPho).Pho=="m" || (*inPho).Pho=="p" || (*inPho).Pho=="f" || (*inPho).Pho=="v") {
		if ((*inPho).PhonemePrec!=NULL) {
			itertemp=inPho-1;

			//CHRIS MSVC2005 fix: look for past the end
			while((*itertemp).Pho!=(*inPho).PhonemePrec->Pho && itertemp!=vectorEnd) // && itertemp!=NULL)
			{
				if ((*itertemp).Pho!="b" && (*itertemp).Pho!="m" && (*itertemp).Pho!="p") {
					for (i=0; i<=2; i++) {
						x=((*itertemp).Time[i][0] - (*inPho).PhonemePrec->StartTime)/((*inPho).Time[i][0] - (*inPho).PhonemePrec->StartTime);
						(*itertemp).TargetLeft[i][0] = A_coart(10, 1-x)*(*itertemp).TargetLeft[i][0];
					}
					x=((*itertemp).Time[6][0] - (*inPho).PhonemePrec->StartTime)/((*inPho).Time[6][0] - (*inPho).PhonemePrec->StartTime);
					(*itertemp).TargetLeft[6][0] = A_coart(10, 1-x)*(*itertemp).TargetLeft[6][0];
				}
				itertemp-=1;
			}
		}


		if ((*inPho).PhonemeSuc!=NULL) {
			itertemp=inPho+1;

			//CHRIS MSVC2005 fix: look for past the end
			while((*itertemp).Pho!=(*inPho).PhonemeSuc->Pho && itertemp!=vectorEnd) // && itertemp!=NULL)
			{
				if ((*itertemp).Pho!="b" && (*itertemp).Pho!="m" && (*itertemp).Pho!="p") {
					for (i=0; i<=2; i++) {
						x=((*itertemp).Time[i][0] - (*inPho).Time[i][0])/((*inPho).PhonemeSuc->Duration - (*inPho).Time[i][0]);
						(*itertemp).TargetLeft[i][0] = CO_coart(10, x)*(*itertemp).TargetLeft[i][0];	
					}
					x=((*itertemp).Time[6][0] - (*inPho).Time[6][0])/((*inPho).PhonemeSuc->Duration - (*inPho).Time[6][0]);
					(*itertemp).TargetLeft[6][0] = CO_coart(10, x)*(*itertemp).TargetLeft[6][0];
				}
				itertemp+=1;
			}
		}

	}
}

// *************************	LP2FAPWITHEXP   *************************
//  From LIP PARAMETERS to FAPs applying correlation rules
// *****************************************************************
void LipModel::Lip2Fap(std::vector<Phoneme>::iterator inPho, std::vector<Phoneme>::iterator vectorEnd)
{
	int i,j;
	int fap[69];

	int **faps_id;
	float generalTensionSup=0;
	float generalTensionInf=0;
	float coef=1.0;
	float rule1[5] = {1, 1, 1, 1, 1};  
	// rule1: it is applied on the internal and the external lip bounder and it allows us to
	// simulate lip pression or lip thinning
	float rule4 = -0.05f;
	// rule4: tip nouse movement because of the lip protrusion
	float rule5 = 0.2f;
	// rule 5: nostril movement because of lip width
	float rule6[2] = {1, 1}; 
	// rule 6: asymmetric expression in disgust

	//FAPFrame frameFinal;

	FAPPhoneme fapPho;
	fapPho.phoneme=(*inPho).Pho;
	
	faps_id=0;

	for(i=1; i<68; i++)	fap[i]=0;

	if ((*inPho).comm_act!=NULL)
		faps_id=(*inPho).comm_act->face_expr_faps;

	if (faps_id!=0)
	{
		//CHRIS MSVC2005 fix: look for past the end
		//if(inPho+1 == vectorEnd)
			//for(i=1; i<68; i++) fap[i]=0;
		//else
			for(i=1; i<68; i++) 
				fap[i]=faps_id[1][i]*(*inPho).BlendingCoeff*(*inPho).Intensity;
	}
	
	//fap initialization
	for (i=0;i<3;i++)
		for (j=0;j<64;j++) 
			fapPho.target[i][j]=0;


	//coef is needed to diminish the tension on the lip near the beginning and the end of the speech
	if((*inPho).StartTime<=0.3)
		coef=(*inPho).StartTime/0.3;

	if((*inPho).Duration>=(animationtime-(animationtime/5)))
		coef=1-((*inPho).Duration-(animationtime-(animationtime/5)))/(animationtime/5);
	
	//compute tension
	if ((*inPho).Pho!="#")
	{
		if(ENGINE_LIP_WEIGHT=="STRONG")
		{
			if(((*inPho).comm_act!=0)&&((*inPho).comm_act->active_channels.mouth!=TENSE))
				generalTensionSup=10*coef;
			else
				generalTensionSup=100*coef;
			generalTensionInf=130*coef;
		}
		else
			if(ENGINE_LIP_WEIGHT=="LIGHT")
			{
				generalTensionSup=0;
				generalTensionInf=0;
			}
			else
			{
				generalTensionSup=50*coef;
				generalTensionInf=70*coef;
			}
	}
	else
	{
		//if (non sono all'ultima pausa)
		if(ENGINE_LIP_WEIGHT=="STRONG")
		{
			generalTensionSup=60*coef;
			generalTensionInf=90*coef;
		}
		else
			if(ENGINE_LIP_WEIGHT=="LIGHT")
			{
				generalTensionSup=0;
				generalTensionInf=0;
			}
			else
			{
				generalTensionSup=40*coef;
				generalTensionInf=60*coef;
			}
	}

	generalTensionSup*=(*inPho).BlendingCoeff;
	generalTensionInf*=(*inPho).BlendingCoeff;
/*	if(inPho+1==NULL)
	{
		generalTensionSup=0;
		generalTensionInf=0;
	}*/

	if (is_labiodental((*inPho).Pho))
	{
			fap[4]/=2;
			fap[8]/=2;
			fap[9]/=2;
			fap[51]/=2;
			fap[55]/=2;
			fap[56]/=2;

			fap[5]=0;
			fap[10]=0;
			fap[11]=0;
			fap[52]=0;
			fap[57]=0;
			fap[58]=0;

			fap[59]/=2;
			fap[60]/=2;
			fap[53]/=2;
			fap[54]/=2;
	}
		
	if (is_bilabial((*inPho).Pho)) {
		// Lip pression when a bilabial consonant is uttered
		rule1[1] *= 3.0f;
		rule1[3] *= 1.0f;

		fap[4]=0;
		fap[8]=0;
		fap[9]=0;
		fap[51]=0;
		fap[55]=0;
		fap[56]=0;

		fap[5]=0;
		fap[10]=0;
		fap[11]=0;
		fap[52]=0;
		fap[57]=0;
		fap[58]=0;

		fap[59]/=2;
		fap[60]/=2;
		fap[53]/=2;
		fap[54]/=2;
	}
	else
	{
		if ((*inPho).TargetLeft[0][3]>0) 
		{
			//lips get thinner because lip width increases
			rule1[0] *= 0.9f;
			rule1[1] *= 0.7f;
			rule1[2] *= 0.9f;
			rule1[3] *= 0.6f;
		}
		else 
			if((*inPho).TargetLeft[0][3]<0) 
			{
				rule1[1] *= 1.1f;
				rule1[3] *= 1.1f;
			}

		if ((inPho-1)>=Phonemes.begin() && (inPho+1)<Phonemes.end()) {
			if (is_bilabial((*(inPho-1)).Pho) || is_bilabial((*(inPho+1)).Pho)) {
				//lip pression because the phoneme is adjacent to a bilabial one
				rule1[1] *= 0.8f;
				rule1[3] *= 0.8f;
			}
		}
	}

	if ((*inPho).TargetLeft[0][4]>0) {
		// tip nouse movement because of the lip protrusion
		rule4 *= -0.05f;
	}

	if ((*inPho).TargetLeft[0][3]>0)
		// nostril movement because of lip width
		rule5 *= 0.25f;
	else
		rule5 *= 0.1f;

/*	for(i=0;i<2;i++)
		rule6[i]*=coef;
	rule4*=coef;
	rule5*=coef;

*/
	if (is_vowel((*inPho).Pho)) j=3;
	else j=1;
	
	for (i=0; i<j; i++) {	
		fapPho.time[i][4]  = (*inPho).Time[0][i];
		fapPho.time[i][8]  = (*inPho).Time[0][i];
		fapPho.time[i][9]  = (*inPho).Time[0][i];
		fapPho.time[i][51] = (*inPho).Time[0][i];
		fapPho.time[i][55] = (*inPho).Time[0][i];
		fapPho.time[i][56] = (*inPho).Time[0][i];

		fapPho.time[i][5]  = (*inPho).Time[1][i];
		fapPho.time[i][10] = (*inPho).Time[1][i];
		fapPho.time[i][11] = (*inPho).Time[1][i];
		fapPho.time[i][52] = (*inPho).Time[1][i];
		fapPho.time[i][57] = (*inPho).Time[1][i];
		fapPho.time[i][58] = (*inPho).Time[1][i];

		fapPho.time[i][3]  = (*inPho).Time[2][i];
		fapPho.time[i][18]  = (*inPho).Time[2][i];    ////altro punto per il mento
		fapPho.time[i][41] = (*inPho).Time[2][i];
		fapPho.time[i][42] = (*inPho).Time[2][i];

		fapPho.time[i][6]  = (*inPho).Time[3][i];
		fapPho.time[i][7]  = (*inPho).Time[3][i];
		fapPho.time[i][53] = (*inPho).Time[3][i];
		fapPho.time[i][54] = (*inPho).Time[3][i];
		fapPho.time[i][61] = (*inPho).Time[3][i];
		fapPho.time[i][62] = (*inPho).Time[3][i];

		fapPho.time[i][17] = (*inPho).Time[4][i];
		fapPho.time[i][63] = (*inPho).Time[4][i];

		fapPho.time[i][16] = (*inPho).Time[5][i];

		fapPho.time[i][12] = (*inPho).Time[6][i];
		fapPho.time[i][13] = (*inPho).Time[6][i];
		fapPho.time[i][59] = (*inPho).Time[6][i];
		fapPho.time[i][60] = (*inPho).Time[6][i];

		fapPho.time[i][43] = (*inPho).Time[7][i];
		fapPho.time[i][44] = (*inPho).Time[8][i];
		fapPho.time[i][45] = (*inPho).Time[9][i];
		fapPho.time[i][46] = (*inPho).Time[10][i];
		fapPho.time[i][47] = (*inPho).Time[11][i];

		

		// UPPER LIP OPENING - Faps 4, 8, 9, 51, 55, 56
		fapPho.target[i][4]  = (-(*inPho).TargetLeft[0][i] + fap[4]) * rule1[0];
		fapPho.target[i][8]  = (-(*inPho).TargetLeft[0][i] + fap[8]) * rule1[0];
		fapPho.target[i][9]  = (-(*inPho).TargetLeft[0][i] + fap[9]) * rule1[0] + generalTensionSup;
		fapPho.target[i][51] = (-(*inPho).TargetLeft[0][i] + fap[51]) * rule1[1] + generalTensionSup; 
		fapPho.target[i][55] = (-(*inPho).TargetLeft[0][i] + fap[55]) * rule1[1] + generalTensionSup;
		fapPho.target[i][56] = (-(*inPho).TargetLeft[0][i] + fap[56]) * rule1[1] + generalTensionSup;

		// LOWER LIP OPENING - Faps 5, 10, 11, 52, 57, 58
		fapPho.target[i][5]  = (-(*inPho).TargetLeft[1][i] + fap[5])* rule1[2] + generalTensionInf*0.2;
		fapPho.target[i][10] = (-(*inPho).TargetLeft[1][i] + fap[10])* rule1[2] + generalTensionInf*0.2;
		fapPho.target[i][11] = (-(*inPho).TargetLeft[1][i] + fap[11])* rule1[2] + generalTensionInf*0.2;
		fapPho.target[i][52] = (-(*inPho).TargetLeft[1][i]* 0.5 + fap[52]) * rule1[3] + generalTensionInf;
		fapPho.target[i][57] = (-(*inPho).TargetLeft[1][i]* 0.5 + fap[57]) * rule1[3] + generalTensionInf;
		fapPho.target[i][58] = (-(*inPho).TargetLeft[1][i]* 0.5 + fap[58]) * rule1[3] + generalTensionInf;

		// JAW - Faps 3, 41, 42	
		fapPho.target[i][3]  = ((*inPho).TargetLeft[2][i] + fap[3]) * rule1[4] - generalTensionInf*0.01;	
		fapPho.target[i][18] = ((*inPho).TargetLeft[2][i] + fap[18]) * rule1[4];	///altro punto del mento
		fapPho.target[i][41] = ((*inPho).TargetLeft[2][i] * 0.5 + fap[41]);
		fapPho.target[i][42] = ((*inPho).TargetLeft[2][i] * 0.5 + fap[42]);

		// LIP WIDTH - Faps 6, 7, 53, 54, 61, 62
		fapPho.target[i][6]  = ((*inPho).TargetLeft[3][i] + fap[6]);
		fapPho.target[i][7]  = ((*inPho).TargetLeft[3][i] + fap[7]);
		fapPho.target[i][53] = ((*inPho).TargetLeft[3][i] + fap[53]);
		fapPho.target[i][54] = ((*inPho).TargetLeft[3][i] + fap[54]);
		fapPho.target[i][61] = ((*inPho).TargetLeft[3][i] + fap[61]) * rule5;
		fapPho.target[i][62] = ((*inPho).TargetLeft[3][i] + fap[62])* rule5;
	
		// UPPER LIP PROTRUSION - Faps Fap 17, 63
		fapPho.target[i][17]  = (*inPho).TargetLeft[4][i] + fap[17];
		fapPho.target[i][63]  = ((*inPho).TargetLeft[4][i] + fap[63]) * rule4;

		// LOWER LIP PROTRUSION - Faps 16
		fapPho.target[i][16] = (*inPho).TargetLeft[5][i] + fap[16];

		// CORNER LIP - Faps 12, 13, 59, 60
		fapPho.target[i][12] = (*inPho).TargetLeft[6][i] + fap[12];
		fapPho.target[i][13] = (*inPho).TargetLeft[6][i] + fap[13];
		fapPho.target[i][59] = (*inPho).TargetLeft[6][i] + fap[59];
		fapPho.target[i][60] = (*inPho).TargetLeft[6][i] + fap[60];

		// TONGUE - Faps 43, 44, 45, 46, 47
		fapPho.target[i][43] = (*inPho).TargetLeft[7][i] + fap[43];
		fapPho.target[i][44] = (*inPho).TargetLeft[8][i] + fap[44];
		fapPho.target[i][45] = (*inPho).TargetLeft[9][i] + fap[45];
		fapPho.target[i][46] = (*inPho).TargetLeft[10][i] + fap[46];
		fapPho.target[i][47] = (*inPho).TargetLeft[11][i] + fap[47];

	}
	FAPPhonemes.push_back(fapPho);
}



// *****************************	WRITEFAP  ***************************
//	It computes the FAPs values for each frame
// **********************************************************************

int LipModel::StoreLipFaps(float start_time)
{
	int i;
	int j = start_time * FRAMERATE;
	float speed;

	speed = (1.0/FRAMERATE);

	float istant=0.0;
	//if the fap vector is not big enough to contain the lip faps
	//then resize it, but also initialize it correctly


	if( ( (start_time + animationtime)*FRAMERATE) >= (*fapvector).size() )
	{
		(*fapvector).resize((start_time + animationtime)*FRAMERATE+1);
		for(i=(*fapvector).size()-1;i>0;i--)
			if((*fapvector)[i].GetFAP(0)!=0)
				break;
			else
				(*fapvector)[i].SetFAP(0,i,0);
	}

	while (istant < animationtime)
	{
		for (i=0; i<64; i++)
		{
			if(isLipFap[i])
			{
				(*fapvector)[j].FAPs[i].active=true;
				(*fapvector)[j].SetFAP(i,(int)(KeyFrameInt[i]->GetAtTime(istant)));
			}
		}
		j+=1;
		istant += speed;
	}

	return 1;
}



std::string LipModel::set_vowel(std::string vow)
{
	if (vow=="a" || vow=="a1") return("a");
	else if (vow=="i" || vow=="i1") return("i");
	else if (vow=="u" || vow=="u1") return("u");
	else if (vow=="e" || vow=="e1" || vow=="E1") return("e");
	else if (vow=="o" || vow=="o1" || vow=="O1") return("o");
	else if (vow=="y") return("y");
}



// **********************************	A_COART	 *******************************************
//	Per il calcolo dell'influenza. Anticipatory coarticulation. Funzione dal basso all'alto
// *****************************************************************************************
float LipModel::A_coart(float b, float x)
{
	float a, c;

	c = (float)((1+exp(-b))/(exp(-b)-1));
	a = -2*c;
	return((float)(a/(1 + exp(-b*x)) + c));
}


// **********************************	CO_COART *******************************************
//	Per il calcolo dell'influenza. Carry-over coarticulation. Funzione dall'alto al basso
// *****************************************************************************************
float LipModel::CO_coart(float b, float x)
{
	float a, c;

	x -=1;
	c = (float)((exp(b)+1)/(exp(b)-1));
	a = -2*c;
	return((float)(a/(1 + exp(-b*x)) + c));
}

int LipModel::is_bilabial(std::string inSeq)
{
	if (inSeq=="b" || inSeq=="m" || inSeq=="p")
		return(1);
	else 
		return(0);
}

int LipModel::is_labiodental(std::string inSeq)
{
	if (inSeq=="f" || inSeq=="v")
		return(1);
	else 
		return(0);
}


// ************	FREE_ALL  *************************
//	It freew all the queues and the useless files 
// ************************************************
void LipModel::FreeAll()
{
	int i;

	for (i=0; i<64; i++)
		if(isLipFap[i])
			if(KeyFrameInt[i]!=0)
				delete KeyFrameInt[i];
}




