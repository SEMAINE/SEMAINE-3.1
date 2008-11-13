//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// C_DataContainer.cpp: implementation of the  C_DataContainer class.
//
//////////////////////////////////////////////////////////////////////

/*
*
* Created on 11 mai 2007, 17:11
*
*/


#include "C_DataContainer.h"


C_DataContainer::~C_DataContainer()
{
	//TO DO: free memory

}


C_DataContainer::C_DataContainer()
{

	fslimits=(int**)malloc(sizeof(int*)*2);
	for(unsigned int i=0;i<2;i++)
	{
		fslimits[i]=(int*)malloc(sizeof(int)*CEM_MAX_FAP);
		for(unsigned int j=0;j<CEM_MAX_FAP;j++)
			fslimits[i][j]=0;
	}

	facialareas=(int**)malloc(sizeof(int*)*RESULT_LENGTH);

	for(unsigned int i=0;i<RESULT_LENGTH;i++)
	{
		facialareas[i]=(int*)malloc(sizeof(int)*CEM_MAX_FAP);
		for(unsigned int j=0;j<CEM_MAX_FAP;j++)
			facialareas[i][j]=0;
	}

	masking_facialareas=(int**)malloc(sizeof(int*)*RESULT_LENGTH);

	for(unsigned int i=0;i<RESULT_LENGTH;i++)
	{
		masking_facialareas[i]=(int*)malloc(sizeof(int)*CEM_MAX_FAP);
		for(unsigned int j=0;j<CEM_MAX_FAP;j++)
			masking_facialareas[i][j]=0;
	}


}




int **C_DataContainer::getfacialareas()
{
	return facialareas;

}


int **C_DataContainer::getmasking_facialareas()
{
	return masking_facialareas;
}

/**
* gets an array of extreme values of faps
* @return array of extreme values of faps
*/

int **C_DataContainer::getfs_limits()
{
	return fslimits;
}

/**
* gets a vector of section schemas
* @return a vector of sectionschemas
*/
std::vector<C_SchemaSection> *C_DataContainer::getsectionschemas()
{
	return sectionschemas;
}

/**
* gets a list of expressions
* @return vector of expressions

std::vector<Expression> *C_DataContainer::getexpressions()
{
return expressions;
}
*/


void C_DataContainer::init()
{


	//<emotion name="NEUTRAL" id="0"/>        
	//<emotion name="ANGER"  id="1"/>
	//<emotion name="DISGUST"  id="2"/>
	//<emotion name="JOY"  id="3"/>
	//<emotion name="FEAR"  id="4"/>
	//<emotion name="SADNESS" id="5"/>
	//<emotion name="SURPRISE" id="6"/>
	//<emotion name="EMBARRASSMENT" id="7"/>
	//<emotion name="JEALOUSY" id="8"/>
	//<emotion name="PRIDE" id="9"/>
	//<emotion name="SHAME" id="10"/>
	//<emotion name="DISTRESS" id="11"/>
	//<emotion name="HAPPY-FOR" id="12"/>
	//<emotion name="GLOATING" id="13"/>
	//<emotion name="RESENTMENT" id="14"/>
	//<emotion name="RELIEF" id="15"/>
	//<emotion name="ENVY" id="16"/>
	//<emotion name="SORRY-FOR" id="17"/>
	//<emotion name="SATISFACTION" id="18"/>
	//<emotion name="HOPE" id="19"/>
	//<emotion name="FEAR_CONFIRMED" id="20"/>
	//<emotion name="DISAPPOINTMENT" id="21"/>
	//<emotion name="REPROACH" id="22"/>
	//<emotion name="LIKING" id="23"/>
	//<emotion name="DISLIKING" id="24"/>
	//<emotion name="GRATITUDE" id="25"/>
	//<emotion name="GRATIFICATION" id="26"/>
	//<emotion name="REMORSE" id="27"/>
	//<emotion name="LOVE" id="28"/>
	//<emotion name="HATE" id="29"/>
	//<emotion name="WORRIED" id="30"/>
	//<emotion name="B_JOY" id="31"/>
	//<emotion name="SMALL_JOY" id="32"/>
	//<emotion name="CONTEMPT" id="33"/>
	//<emotion name="RELIEF" id="34"/>

	divisions=new std::vector<C_SchemaSection>;

	/*
	<face sections="8">
	<section name="BROWS" order="0">
	<fap num="31"/>
	<fap num="32"/>
	<fap num="33"/>
	<fap num="34"/>
	<fap num="35"/>
	<fap num="36"/>
	<fap num="37"/>
	<fap num="38"/>
	</section>
	*/

	C_SchemaSection one("BROWS",0);
	one.addSection(31);
	one.addSection(32);
	one.addSection(33);
	one.addSection(34);
	one.addSection(35);
	one.addSection(36);
	one.addSection(37);
	one.addSection(38);

	divisions->push_back(one);

	facialareas[0][31]=1;
	facialareas[0][32]=1;
	facialareas[0][33]=1;
	facialareas[0][34]=1;
	facialareas[0][35]=1;
	facialareas[0][36]=1;
	facialareas[0][37]=1;
	facialareas[0][38]=1;
	/*
	<section name="UPPERLID" order="1">
	<fap num="19"/>
	<fap num="20"/>
	</section>
	*/

	C_SchemaSection two("UPPERLID",1);
	two.addSection(19);
	two.addSection(20);

	divisions->push_back(two);

	facialareas[1][19]=1;
	facialareas[1][20]=1;

	/*
	<section name="EYES" order="2">
	<fap num="23"/>
	<fap num="24"/>
	<fap num="25"/>
	<fap num="26"/>
	<fap num="27"/>
	<fap num="28"/>
	<fap num="29"/>
	<fap num="30"/>
	</section>
	*/

	C_SchemaSection three("EYES",2);
	three.addSection(23);
	three.addSection(24);
	three.addSection(25);
	three.addSection(26);
	three.addSection(27);
	three.addSection(28);
	three.addSection(29);
	three.addSection(30);		
	divisions->push_back(three);

	facialareas[2][23]=1;
	facialareas[2][24]=1;
	facialareas[2][25]=1;
	facialareas[2][26]=1;
	facialareas[2][27]=1;
	facialareas[2][28]=1;
	facialareas[2][29]=1;
	facialareas[2][30]=1;


	/*
	<section name="LOWEREYELID" order="3">
	<fap num="21"/>
	<fap num="22"/>
	<fap num="41"/>
	<fap num="42"/>
	</section>
	*/

	C_SchemaSection four("LOWEREYELID",3);
	four.addSection(21);
	four.addSection(22);
	four.addSection(41);
	four.addSection(42);

	divisions->push_back(four);

	facialareas[3][21]=1;
	facialareas[3][22]=1;
	facialareas[3][41]=1;
	facialareas[3][42]=1;

	/*
	<section name="NOSE" order="4">
	<fap num="61"/>
	<fap num="62"/>
	<fap num="63"/>
	<fap num="64"/>
	</section>
	*/

	C_SchemaSection five("NOSE",4);
	five.addSection(61);
	five.addSection(62);
	five.addSection(63);
	five.addSection(64);

	divisions->push_back(five);

	facialareas[4][61]=1;
	facialareas[4][62]=1;
	facialareas[4][63]=1;
	facialareas[4][64]=1;

	/*
	<section name="CHEEK" order="5">
	<fap num="39"/>
	<fap num="40"/>
	</section>
	*/

	C_SchemaSection six("CHEEK",5);
	six.addSection(39);
	six.addSection(40);

	divisions->push_back(six);

	facialareas[5][39]=1;
	facialareas[5][40]=1;

	/*
	<section name="UPPERLIP" order="6">
	<fap num="59"/>
	<fap num="60"/>
	<fap num="53"/>
	<fap num="54"/>
	<fap num="6"/>
	<fap num="7"/>
	<fap num="12"/>
	<fap num="13"/>
	<fap num="4"/>
	<fap num="8"/>
	<fap num="9"/>
	<fap num="17"/>
	<fap num="51"/>
	<fap num="55"/>
	<fap num="56"/>
	<fap num="3"/>
	</section>
	*/

	C_SchemaSection seven("UPPERLIP",6);
	seven.addSection(59);
	seven.addSection(60);
	seven.addSection(53);
	seven.addSection(54);
	seven.addSection(6);
	seven.addSection(7);
	seven.addSection(12);
	seven.addSection(13);
	seven.addSection(4);
	seven.addSection(8);
	seven.addSection(9);
	seven.addSection(17);
	seven.addSection(51);
	seven.addSection(55);
	seven.addSection(56);
	seven.addSection(3);

	divisions->push_back(seven);

	facialareas[6][59]=1;
	facialareas[6][60]=1;
	facialareas[6][53]=1;
	facialareas[6][54]=1;
	facialareas[6][6]=1;
	facialareas[6][7]=1;
	facialareas[6][12]=1;
	facialareas[6][13]=1;
	facialareas[6][4]=1;
	facialareas[6][8]=1;
	facialareas[6][9]=1;
	facialareas[6][17]=1;
	facialareas[6][51]=1;
	facialareas[6][55]=1;
	facialareas[6][56]=1;
	facialareas[6][3]=1;

	/*
	<section name="LOWERLIP" order="7">
	<fap num="5"/>
	<fap num="10"/>
	<fap num="11"/>
	<fap num="16"/>
	<fap num="52"/>
	<fap num="57"/>
	<fap num="58"/>
	</section>
	</face>
	*/

	C_SchemaSection eight("LOWERLIP",7);
	eight.addSection(5);
	eight.addSection(10);
	eight.addSection(11);
	eight.addSection(16);
	eight.addSection(52);
	eight.addSection(57);
	eight.addSection(58);

	divisions->push_back(eight);


	facialareas[7][5]=1;
	facialareas[7][10]=1;
	facialareas[7][11]=1;
	facialareas[7][16]=1;
	facialareas[7][52]=1;
	facialareas[7][57]=1;
	facialareas[7][58]=1;

	//fill masking_divisions

	/*
	<face sections="8">
	<section name="BROWS" order="0">
	<fap num="31"/>
	<fap num="32"/>
	<fap num="33"/>
	<fap num="34"/>
	<fap num="35"/>
	<fap num="36"/>
	<fap num="37"/>
	<fap num="38"/>
	*/

	masking_divisions=new std::vector<C_SchemaSection>;

	C_SchemaSection one2("BROWS",0);
	one2.addSection(31);
	one2.addSection(32);
	one2.addSection(33);
	one2.addSection(34);
	one2.addSection(35);
	one2.addSection(36);
	one2.addSection(37);
	one2.addSection(38);

	masking_divisions->push_back(one2);

	masking_facialareas[0][31]=1;
	masking_facialareas[0][32]=1;
	masking_facialareas[0][33]=1;
	masking_facialareas[0][34]=1;
	masking_facialareas[0][35]=1;
	masking_facialareas[0][36]=1;
	masking_facialareas[0][37]=1;
	masking_facialareas[0][38]=1;

	/*
	</section>
	<section name="UPPERLID" order="1">
	<fap num="19"/>
	<fap num="20"/>
	</section>
	*/
	C_SchemaSection two2("UPPERLID",1);
	two2.addSection(19);
	two2.addSection(20);

	divisions->push_back(two2);

	masking_facialareas[1][19]=1;
	masking_facialareas[1][20]=1;


	/*
	<section name="EYES" order="2">
	<fap num="23"/>
	<fap num="24"/>
	<fap num="25"/>
	<fap num="26"/>
	<fap num="27"/>
	<fap num="28"/>
	<fap num="29"/>
	<fap num="30"/>
	</section>
	*/
	C_SchemaSection three2("EYES",2);
	three2.addSection(23);
	three2.addSection(24);
	three2.addSection(25);
	three2.addSection(26);
	three2.addSection(27);
	three2.addSection(28);
	three2.addSection(29);
	three2.addSection(30);		
	divisions->push_back(three2);


	masking_facialareas[2][23]=1;
	masking_facialareas[2][24]=1;
	masking_facialareas[2][25]=1;
	masking_facialareas[2][26]=1;
	masking_facialareas[2][27]=1;
	masking_facialareas[2][28]=1;
	masking_facialareas[2][29]=1;
	masking_facialareas[2][30]=1;	


	/*
	<section name="LOWEREYELID" order="3">
	<fap num="21"/>
	<fap num="22"/>
	<fap num="41"/>
	<fap num="42"/>
	</section>
	*/
	C_SchemaSection four2("LOWEREYELID",3);
	four2.addSection(21);
	four2.addSection(22);
	four2.addSection(41);
	four2.addSection(42);

	divisions->push_back(four2);

	masking_facialareas[3][21]=1;
	masking_facialareas[3][22]=1;
	masking_facialareas[3][41]=1;
	masking_facialareas[3][42]=1;

	/*
	<section name="NOSE" order="4">
	<fap num="61"/>
	<fap num="62"/>
	<fap num="63"/>
	<fap num="64"/>
	</section>
	*/


	C_SchemaSection five2("NOSE",4);
	five2.addSection(61);
	five2.addSection(62);
	five2.addSection(63);
	five2.addSection(64);

	divisions->push_back(five2);

	masking_facialareas[4][61]=1;
	masking_facialareas[4][62]=1;
	masking_facialareas[4][63]=1;
	masking_facialareas[4][64]=1;

	/*
	<section name="CHEEK" order="5">
	<fap num="39"/>
	<fap num="40"/>
	</section>
	*/

	C_SchemaSection six2("CHEEK",5);
	six2.addSection(39);
	six2.addSection(40);

	divisions->push_back(six2);

	masking_facialareas[5][39]=1;
	masking_facialareas[5][40]=1;


	/*
	<section name="UPPERLIP" order="6">
	<fap num="59"/>
	<fap num="60"/>
	<fap num="53"/>
	<fap num="54"/>
	<fap num="6"/>
	<fap num="7"/>
	<fap num="12"/>
	<fap num="13"/>
	<fap num="4"/>
	<fap num="8"/>
	<fap num="9"/>
	<fap num="17"/>
	<fap num="55"/>
	<fap num="56"/>
	<fap num="5"/>
	<fap num="10"/>
	<fap num="11"/>
	<fap num="16"/>
	<fap num="57"/>
	<fap num="58"/>
	<fap num="3"/>
	</section>
	*/

	C_SchemaSection seven2("UPPERLIP",6);
	seven2.addSection(59);
	seven2.addSection(60);
	seven2.addSection(53);
	seven2.addSection(54);
	seven2.addSection(6);
	seven2.addSection(7);
	seven2.addSection(12);
	seven2.addSection(13);
	seven2.addSection(4);
	seven2.addSection(8);
	seven2.addSection(9);
	seven2.addSection(17);
	seven2.addSection(55);
	seven2.addSection(56);
	seven2.addSection(5);
	seven2.addSection(10);
	seven2.addSection(11);
	seven2.addSection(16);
	seven2.addSection(57);
	seven2.addSection(58);
	seven2.addSection(3);

	divisions->push_back(seven2);

	masking_facialareas[6][59]=1;
	masking_facialareas[6][60]=1;
	masking_facialareas[6][53]=1;
	masking_facialareas[6][54]=1;
	masking_facialareas[6][6]=1;
	masking_facialareas[6][7]=1;
	masking_facialareas[6][12]=1;
	masking_facialareas[6][13]=1;
	masking_facialareas[6][4]=1;
	masking_facialareas[6][8]=1;
	masking_facialareas[6][9]=1;
	masking_facialareas[6][17]=1;
	//masking_facialareas[6][55]=1;
	//masking_facialareas[6][56]=1;
	masking_facialareas[6][5]=1;
	masking_facialareas[6][10]=1;
	masking_facialareas[6][11]=1;
	masking_facialareas[6][16]=1;
	//masking_facialareas[6][57]=1;
	//masking_facialareas[6][58]=1;
	masking_facialareas[6][3]=1;

	/*
	<section name="LOWERLIP" order="7">
	<fap num="51"/>
	<fap num="52"/>
	<fap num="55"/>
	<fap num="56"/>
	<fap num="57"/>
	<fap num="58"/>
	</section>
	*/

	//CHECK IT
	C_SchemaSection eight2("LOWERLIP",7);

	eight2.addSection(51);
	eight2.addSection(52);
	eight2.addSection(55);
	eight2.addSection(56);
	eight2.addSection(57);
	eight2.addSection(58);

	divisions->push_back(eight2);

	masking_facialareas[7][51]=1;
	masking_facialareas[7][52]=1;	
	masking_facialareas[7][55]=1;
	masking_facialareas[7][56]=1;
	masking_facialareas[7][57]=1;
	masking_facialareas[7][58]=1;


	//fill limits

	//DEBUG
	// lim_tab[temp.getId()][0]=temp.getMin();
	//lim_tab[temp.getId()][1]=temp.getMax();

	//<fap nr="0" min="0" max="0" masking="0"/>	
	fslimits[0][0]=0;
	fslimits[1][0]=0;
	//<fap nr="1" min="0" max="0" masking="0"/>
	fslimits[0][1]=0;
	fslimits[1][1]=0;
	//<fap nr="2" min="0" max="0" masking="0"/>
	fslimits[0][2]=0;
	fslimits[1][2]=0;
	//<fap nr="3" min="255" max="-150" masking="1"/>
	fslimits[0][3]=255;
	fslimits[1][3]=-150;
	//<fap nr="4" min="45" max="-45" masking="1"/>
	fslimits[0][4]=45;
	fslimits[1][4]=-45;
	//<fap nr="5" min="300" max="-540" masking="1"/>
	fslimits[0][5]=300;
	fslimits[1][5]=-540;
	//<fap nr="6" min="50" max="-40" masking="1"/>
	fslimits[0][6]=50;
	fslimits[1][6]=-40;
	//<fap nr="7" min="50" max="-40" masking="1"/>
	fslimits[0][7]=50;
	fslimits[1][7]=-40;
	//<fap nr="8" min="0" max="0" masking="1"/>
	fslimits[0][8]=0;
	fslimits[1][8]=0;
	//<fap nr="9" min="0" max="0" masking="1"/>
	fslimits[0][9]=0;
	fslimits[1][9]=0;
	//<fap nr="10" min="0" max="0" masking="1"/>
	fslimits[0][10]=0;
	fslimits[1][10]=0;
	//<fap nr="11" min="0" max="0" masking="1"/>
	fslimits[0][11]=0;
	fslimits[1][11]=0;
	//<fap nr="12" min="0" max="0" masking="1"/>
	fslimits[0][12]=0;
	fslimits[1][12]=0;
	//<fap nr="13" min="0" max="0" masking="1"/>
	fslimits[0][13]=0;
	fslimits[1][13]=0;
	//<fap nr="14" min="0" max="0" masking="0"/>
	fslimits[0][14]=0;
	fslimits[1][14]=0;
	//<fap nr="15" min="0" max="0" masking="0"/>
	fslimits[0][15]=0;
	fslimits[1][15]=0;
	//<fap nr="16" min="0" max="0" masking="0"/>
	fslimits[0][16]=0;
	fslimits[1][16]=0;
	//<fap nr="17" min="200" max="0" masking="1"/>
	fslimits[0][17]=200;
	fslimits[1][17]=0;
	//<fap nr="18" min="0" max="0" masking="1"/>
	fslimits[0][18]=0;
	fslimits[1][18]=0;
	//<fap nr="19" min="200" max="-256" masking="1"/>
	fslimits[0][19]=200;
	fslimits[1][19]=-256;
	//<fap nr="20" min="200" max="-256" masking="1"/>
	fslimits[0][20]=200;
	fslimits[1][20]=-256;
	//<fap nr="21" min="100" max="-100" masking="1"/>
	fslimits[0][21]=100;
	fslimits[1][21]=-100;
	//<fap nr="22" min="100" max="-100" masking="1"/>
	fslimits[0][22]=100;
	fslimits[1][22]=-100;
	//<fap nr="23" min="0" max="0" masking="0"/>
	fslimits[0][23]=0;
	fslimits[1][23]=0;
	//<fap nr="24" min="0" max="0" masking="0"/>
	fslimits[0][24]=0;
	fslimits[1][24]=0;
	//<fap nr="25" min="0" max="0" masking="0"/>
	fslimits[0][25]=0;
	fslimits[1][25]=0;
	//<fap nr="26" min="0" max="0" masking="0"/>
	fslimits[0][26]=0;
	fslimits[1][26]=0;
	//<fap nr="27" min="0" max="0" masking="0"/>
	fslimits[0][27]=0;
	fslimits[1][27]=0;
	//<fap nr="28" min="0" max="0" masking="0"/>
	fslimits[0][28]=0;
	fslimits[1][28]=0;
	//<fap nr="29" min="0" max="0" masking="0"/>
	fslimits[0][29]=0;
	fslimits[1][29]=0;
	//<fap nr="30" min="0" max="0" masking="0"/>
	fslimits[0][30]=0;
	fslimits[1][30]=0;
	//<fap nr="31" min="220" max="-110" masking="1"/>
	fslimits[0][31]=220;
	fslimits[1][31]=-110;
	//<fap nr="32" min="220" max="-110" masking="1"/>
	fslimits[0][32]=220;
	fslimits[1][32]=-110;
	//<fap nr="33" min="140" max="-55" masking="1"/>
	fslimits[0][33]=140;
	fslimits[1][33]=-55;
	//<fap nr="34" min="140" max="-55" masking="1"/>
	fslimits[0][34]=140;
	fslimits[1][34]=-55;
	//<fap nr="35" min="70" max="0" masking="1"/>
	fslimits[0][35]=70;
	fslimits[1][35]=0;
	//<fap nr="36" min="70" max="0" masking="1"/>
	fslimits[0][36]=70;
	fslimits[1][36]=0;
	//<fap nr="37" min="110" max="0" masking="1"/>
	fslimits[0][37]=110;
	fslimits[1][37]=0;
	//<fap nr="38" min="110" max="0" masking="1"/>
	fslimits[0][38]=110;
	fslimits[1][38]=0;
	//<fap nr="39" min="0" max="0" masking="0"/>
	fslimits[0][39]=0;
	fslimits[1][39]=0;
	//<fap nr="40" min="0" max="0" masking="0"/>
	fslimits[0][40]=0;
	fslimits[1][40]=0;
	//<fap nr="41" min="100" max="0" masking="1"/>
	fslimits[0][41]=100;
	fslimits[1][41]=0;
	//<fap nr="42" min="100" max="0" masking="1"/>
	fslimits[0][42]=100;
	fslimits[1][42]=0;
	//<fap nr="43" min="0" max="0" masking="0"/>
	fslimits[0][43]=0;
	fslimits[1][43]=0;
	//<fap nr="44" min="0" max="0" masking="0"/>
	fslimits[0][44]=0;
	fslimits[1][44]=0;
	//<fap nr="45" min="0" max="0" masking="0"/>
	fslimits[0][45]=0;
	fslimits[1][45]=0;
	//<fap nr="46" min="0" max="0" masking="0"/>
	fslimits[0][46]=0;
	fslimits[1][46]=0;
	//<fap nr="47" min="0" max="0" masking="0"/>
	fslimits[0][47]=0;
	fslimits[1][47]=0;
	//<fap nr="48" min="0" max="0" masking="0"/>
	fslimits[0][48]=0;
	fslimits[1][48]=0;
	//<fap nr="49" min="0" max="0" masking="0"/>
	fslimits[0][49]=0;
	fslimits[1][49]=0;

	//<fap nr="50" min="0" max="0" masking="0"/>
	fslimits[0][50]=0;
	fslimits[1][50]=0;
	//<fap nr="51" min="150" max="0" masking="1"/>
	fslimits[0][51]=150;
	fslimits[1][51]=0;	
	//<fap nr="52" min="180" max="0" masking="1"/>
	fslimits[0][52]=180;
	fslimits[1][52]=0;
	//<fap nr="53" min="50" max="-32" masking="1"/>
	fslimits[0][53]=50;
	fslimits[1][53]=-32;
	//<fap nr="54" min="50" max="-32" masking="1"/>
	fslimits[0][54]=50;
	fslimits[1][54]=-32;
	//<fap nr="55" min="0" max="-120" masking="1"/>
	fslimits[0][55]=0;
	fslimits[1][55]=-120;
	//<fap nr="56" min="0" max="-120" masking="1"/>
	fslimits[0][56]=0;
	fslimits[1][56]=-120;
	//<fap nr="57" min="0" max="0" masking="1"/>
	fslimits[0][57]=0;
	fslimits[1][57]=0;
	//<fap nr="58" min="0" max="0" masking="1"/>
	fslimits[0][58]=0;
	fslimits[1][58]=0;
	//<fap nr="59" min="200" max="-130" masking="1"/>
	fslimits[0][59]=200;
	fslimits[1][59]=-130;
	//<fap nr="60" min="200" max="-130" masking="1"/>
	fslimits[0][60]=200;
	fslimits[1][60]=-130;
	//<fap nr="61" min="140" max="0" masking="1"/>
	fslimits[0][61]=140;
	fslimits[1][61]=0;
	//<fap nr="62" min="140" max="0" masking="1"/>
	fslimits[0][62]=140;
	fslimits[1][62]=0;
	//<fap nr="63" min="0" max="0" masking="0"/>
	fslimits[0][63]=0;
	fslimits[1][63]=0;
	//<fap nr="64" min="0" max="0" masking="0"/>
	fslimits[0][64]=0;
	fslimits[1][64]=0;
	//<fap nr="65" min="0" max="0" masking="0"/>
	fslimits[0][65]=0;
	fslimits[1][65]=0;
	//<fap nr="66" min="0" max="0" masking="0"/>
	fslimits[0][66]=0;
	fslimits[1][66]=0;
	//<fap nr="67" min="0" max="0" masking="0"/>
	fslimits[0][67]=0;
	fslimits[1][67]=0;
	//<fap nr="68" min="0" max="0" masking="0"/>
	fslimits[0][68]=0;
	fslimits[1][68]=0;


	//fill sections

	sectionschemas=new std::vector<C_SchemaSection>;

	/*
	<face sections="10">
	<section name="S1" order="1">
	<fap num="0"/>
	</section>
	*/
	C_SchemaSection one3("S1",1);
	one3.addSection(0);		
	sectionschemas->push_back(one3);

	/*
	<section name="S2" order="2">
	<fap num="1"/>
	<fap num="2"/>
	<fap num="3"/>
	<fap num="4"/>
	<fap num="5"/>
	<fap num="6"/>
	<fap num="7"/>
	</section>
	*/
	C_SchemaSection two3("S2",2);
	two3.addSection(1);				
	two3.addSection(2);		
	two3.addSection(3);		
	two3.addSection(4);		
	two3.addSection(5);
	two3.addSection(6);		
	two3.addSection(7);		
	sectionschemas->push_back(two3);

	/*
	<section name="S3" order="3">
	<fap num="0"/>
	<fap num="1"/>
	<fap num="2"/>
	<fap num="3"/>
	</section>
	*/

	C_SchemaSection three3("S3",3);
	three3.addSection(0);		
	three3.addSection(1);		
	three3.addSection(2);		
	three3.addSection(3);		
	sectionschemas->push_back(three3);

	/*
	<section name="S4" order="4">
	<fap num="4"/>
	<fap num="5"/>
	<fap num="6"/>
	<fap num="7"/>
	</section>
	*/
	C_SchemaSection four3("S4",4);
	four3.addSection(4);		
	four3.addSection(5);		
	four3.addSection(6);		
	four3.addSection(7);		
	sectionschemas->push_back(four3);

	/*
	<section name="S5" order="5">
	<fap num="0"/>
	<fap num="1"/>
	<fap num="2"/>
	</section>
	*/

	C_SchemaSection five3("S5",5);
	five3.addSection(0);		
	five3.addSection(1);		
	five3.addSection(2);				
	sectionschemas->push_back(five3);

	/*
	<section name="S6" order="6">
	<fap num="3"/>
	<fap num="4"/>
	<fap num="5"/>
	<fap num="6"/>
	<fap num="7"/>
	</section>

	*/	
	C_SchemaSection six3("S6",6);
	six3.addSection(3);		
	six3.addSection(4);		
	six3.addSection(5);				
	six3.addSection(6);				
	six3.addSection(7);				
	sectionschemas->push_back(six3);

	/*
	<section name="S7" order="7">
	<fap num="0"/>
	<fap num="1"/>
	<fap num="2"/>
	<fap num="3"/>
	<fap num="4"/>
	<fap num="5"/>
	</section>
	*/

	C_SchemaSection seven3("S7",7);
	seven3.addSection(0);		
	seven3.addSection(1);		
	seven3.addSection(2);				
	seven3.addSection(3);				
	seven3.addSection(4);				
	seven3.addSection(5);				
	sectionschemas->push_back(seven3);

	/*
	<section name="S8" order="8">
	<fap num="6"/>
	<fap num="7"/>
	</section>
	*/

	C_SchemaSection eight3("S8",8);
	eight3.addSection(6);		
	eight3.addSection(7);				
	sectionschemas->push_back(eight3);
	/*
	<section name="S9" order="9">
	<fap num="0"/>
	<fap num="1"/>
	<fap num="2"/>
	<fap num="3"/>
	<fap num="5"/>
	</section>
	*/

	C_SchemaSection nine3("S9",9);
	nine3.addSection(0);		
	nine3.addSection(1);				
	nine3.addSection(2);				
	nine3.addSection(3);				
	nine3.addSection(5);				
	sectionschemas->push_back(nine3);
	/*
	<section name="S10" order="10">
	<fap num="4"/>
	<fap num="6"/>
	<fap num="7"/>
	</section>
	</face>
	*/
	C_SchemaSection ten3("S10",10);
	ten3.addSection(4);		
	ten3.addSection(6);				
	ten3.addSection(7);						
	sectionschemas->push_back(ten3);

}// end of init
