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

// Complex_Expression.cpp: implementation of the Complex_Expression class.
//
//////////////////////////////////////////////////////////////////////

#include "ComplexExpression.h"

#include ".\utils\C_DataContainer.h"
#include ".\utils\CemGlobals.h"

#include ".\blending\C_Masking.h"
#include ".\blending\C_Superposition.h"
#include ".\blending\C_Assymetry.h"


C_DataContainer local_container;

/*
How to use it:

ComplexExpression ce;

Emotion *e1 =new Emotion("sadness");
e1->setType("felt");

Emotion *e2 = new Emotion("joy");
e2->setType("fake");

int **result;

result=ce.generate("masking", e1, e2);

for (int i=0;i<69;i++)
{
if (result[1][i]>0)
printf("fap %i, value %i \n", i, result[1][i] );
}

*/



//it should be used instead of complexfacialexpressionmanager
//check all the methods the other uses
ComplexExpression::ComplexExpression()
{
	//run inimanager
	//run datacontainer	
	local_container.init();	
}

ComplexExpression::~ComplexExpression()
{
}


int **ComplexExpression::generate(std::string type, Emotion *emotion1, Emotion *emotion2)
{
	if (type.compare("superposition")==0)
	{
		C_Superposition superp;
		superp.addEmotion(emotion1);
		superp.addEmotion(emotion2);
		superp.evaluate(RESULT_LENGTH);
		return superp.getExpression();
	}

	if (type.compare("masking")==0)
	{
		C_Masking masking;
		masking.addEmotion(emotion1);
		masking.addEmotion(emotion2);
		masking.evaluate(RESULT_LENGTH);
		C_Assymetry assymetry;
		if  (emotion1->getType().compare("fake")==0) 
		{
			int **exp=masking.getExpression(0.05);
			//assymetry.update(exp,emotion1);		
			return exp;
		}
		if  (emotion2->getType().compare("fake")==0) 
		{
			int **exp=masking.getExpression(0.05);
			//assymetry.update(exp,emotion2);		
			return exp;
		}

		return NULL;
	}
	return NULL;
}


int **ComplexExpression::generate (std::string value)
{

	/*
	how it works?

	1.  you write e.g. NEUTRAL_maskedby_FEAR in APML/BML.

	2. in earl library you may find:
	<complex-emotion name="NEUTRAL_maskedby_FEAR">
	<emotion category="NEUTRAL" regulation="suppress"/>
	<emotion category="FEAR" regulation="simulate"/>
	</complex-emotion>

	3. your command is switched to :

	<emotion category="NEUTRAL" regulation="suppress"/>
	<emotion category="FEAR" regulation="simulate"/>

	4. then you arrive here, and your command (3) is switched again

	now we add:
	attenuate -> <emotion category="FEAR" regulation="attenuate"/> 
	amplify  -> <emotion category="FEAR" regulation="amplify"/>

	e.g.

	<complex-emotion name="FEAR_exaggerated">		
	<emotion category="FEAR" regulation="amplify"/>
	<emotion category="FEAR" regulation="amplify"/>
	</complex-emotion>

	<complex-emotion name="FEAR_attenuated">
	<emotion category="FEAR" regulation="attenuate"/>
	<emotion category="FEAR" regulation="attenuate"/>
	</complex-emotion>

	*/

	bool found=false;

	//int **faps;
	//faps=0;

	std::list<XMLGenericTree*>::iterator iter;

	if(lib==0)
		return 0;

	std::string complexexpressiontype="";

	Emotion *e1;
	Emotion *e2;


	if(!lib->child.empty())
	{
		iter=lib->child.begin();
		while((iter!=lib->child.end())&&(found==false))
		{
			if((*iter)->name=="complex-emotion")
			{
				if((*iter)->GetAttribute("name")==value)
				{

					std::string emotion1,emotion2;
					std::string regulation1,regulation2;
					std::string intensity1,intensity2;
					std::string emotiontype;

					std::list<XMLGenericTree*>::iterator t;

					t=(*iter)->child.begin();

					emotion1=(*t)->GetAttribute("category");
					regulation1=(*t)->GetAttribute("regulation");
					intensity1=(*t)->GetAttribute("intensity");
					if(intensity1=="") intensity1="1.0";

					t++;

					emotion2=(*t)->GetAttribute("category");
					regulation2=(*t)->GetAttribute("regulation");
					intensity2=(*t)->GetAttribute("intensity");
					if (intensity2=="") intensity2="1.0";

					//by default 
					emotiontype="superposition";

					//it means that type is fake or felt
					if(((regulation1=="simulate")&&(regulation2=="suppress"))
						||((regulation2=="simulate")&&(regulation1=="suppress")))
						emotiontype="masking";

					if((regulation1=="amplify")||(regulation2=="amplify"))		
					{
						emotiontype="exaggerated";
						intensity1="1.4";
						intensity2="1.4";
					}

					if((regulation1=="attenuate")||(regulation2=="attenuate"))	 
					{
						emotiontype="attenuated";
						intensity1="0.7";
						intensity2="0.7";
					}

					/*
					if(regulation1=="amplify")
					intensity1="1.5";
					if(regulation2=="amplify")
					intensity2="1.5";
					if(regulation1=="attenuate")
					intensity1="0.5";
					if(regulation2=="attenuate")
					intensity2="0.5";
					*/

					//std::string outstring="";
					//outstring += "<blending>\n";


					if(emotiontype=="masking")
					{
						//outstring = outstring+ "<" + emotiontype + " length=\"10\" static=\"true\">\n";

						complexexpressiontype="masking";

						if(regulation1=="simulate")
						{
							e1 =new Emotion(emotion1);
							e1->setType("fake");	
						}
						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"fake\"/>\n";
						if(regulation1=="suppress")
						{
							e1 =new Emotion(emotion1);
							e1->setType("felt");		
						}
						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"felt\"/>\n";
						if(regulation2=="simulate")
						{
							e2 = new Emotion(emotion2);
							e2->setType("fake");
						}
						//outstring = outstring+ "<emotion name=\"" + emotion2 + "\" intensity=\"" + intensity2 + "\" type=\"fake\"/>\n";
						if(regulation2=="suppress")
						{
							e2 = new Emotion(emotion2);
							e2->setType("felt");		
						}
						//outstring = outstring+ "<emotion name=\"" + emotion2 + "\" intensity=\"" + intensity2 + "\" type=\"felt\"/>\n";
					}

					if(emotiontype=="superposition")
					{
						complexexpressiontype="superposition";

						//outstring = outstring+ "<" + emotiontype + " length=\"10\" static=\"true\">\n";

						e1 =new Emotion(emotion1);
						e1->setType("felt");
						e1->setIntensity(atof(intensity1.c_str()));

						e2 = new Emotion(emotion2);
						e2->setType("felt");
						e2->setIntensity(atof(intensity2.c_str()));

						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\"/>\n";
						//outstring = outstring+ "<emotion name=\"" + emotion2 + "\" intensity=\"" + intensity2 + "\"/>\n";
					}


					if(emotiontype=="attenuated")
					{

						//TODO : add new id
						complexexpressiontype="masking";

						e1 =new Emotion(emotion1);
						e1->setType("felt");
						e1->setIntensity(atof(intensity1.c_str()));

						e2 = new Emotion(emotion1);
						e2->setType("fake");
						e2->setIntensity(atof(intensity1.c_str()));

						//outstring = outstring+ "<" + "masking" + " length=\"10\" static=\"true\">\n";

						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"felt\"/>\n";
						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"fake\"/>\n";
					}


					if(emotiontype=="exaggerated")
					{
						//outstring = outstring+ "<" + "masking" + " length=\"10\" static=\"true\">\n";

						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"felt\"/>\n";
						//outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"fake\"/>\n";

						//TODO : add new id
						complexexpressiontype="masking";

						e1 =new Emotion(emotion1);
						e1->setType("felt");
						e1->setIntensity(atof(intensity1.c_str()));

						e2 = new Emotion(emotion1);
						e2->setType("fake");
						e2->setIntensity(atof(intensity1.c_str()));
					}

					/*
					if(emotiontype=="masking")
					outstring = outstring + "<mtype name=\"static\"/>\n";

					if(emotiontype=="exaggerated")
					outstring = outstring +"<mtype name=\"facilitation\"/>\n";

					if(emotiontype=="attenuated")
					outstring = outstring + "<mtype name=\"suppression\"/>\n";
					*/

					/*
					if	((emotiontype=="exaggerated")||(emotiontype=="attenuated")) outstring = outstring+ "</" + "masking" + ">\n";
					else outstring = outstring+ "</" + emotiontype + ">\n";
					*/

					//outstring = outstring+ "</blending>\n";

					found=true;
				}

			}

			iter++;
		}//end of while
	}// if not empty

	if (found==true) return generate(complexexpressiontype, e1, e2);
	else 
		printf("Complex in %s not found in the earl library!\n",value.c_str());

	return 0;
}



int ComplexExpression::ReadLibrary(std::string complexemotionlibraryfilename)
{
	XMLGenericParser p;
	p.SetValidating(false);
	this->lib=p.ParseFile(complexemotionlibraryfilename);
	if(this->lib==0)
		return 0;

	return 1;
}

void ComplexExpression::setLibrary(XMLGenericTree *lib)
{	
	this->lib=lib;
}
