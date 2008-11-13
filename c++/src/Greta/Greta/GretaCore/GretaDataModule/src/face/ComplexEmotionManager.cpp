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

// ComplexExpessionManager.cpp: implementation of the ComplexExpessionManager class.
//
//////////////////////////////////////////////////////////////////////

//NOT USED: SEE complexexpressionmanager


#include "ComplexEmotionManager.h"
#include "IniManager.h"
//#include <process.h>
//#include <direct.h>

extern IniManager inimanager;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ComplexEmotionManager::ComplexEmotionManager()
{
	this->lib=0;
	initialized=false;
	registered = false;
	
	//the first use of cfe takes much time
	//in the realtime version the registration should be made earier (before the code is sent)

	psy = new Psydule("ComplexEmotionManager",inimanager.GetValueString("PSYCLONE_HOST").c_str(),inimanager.GetValueInt("PSYCLONE_PORT"));
	GretaName=inimanager.GetValueString("GRETA_NAME").c_str();
	if(psy->Register(GretaName+PSYCLONE_GRETA_WHITEBOARD,GretaName+PSYCLONE_COMPLEX_OUTPUT)!=0) registered = true; 	
}


ComplexEmotionManager::~ComplexEmotionManager()
{
					
	if(this->lib!=0)
		delete this->lib;

	if(this->psy!=0)	delete this->psy;
}

int ComplexEmotionManager::ReadLibrary(std::string complexemotionlibraryfilename)
{
	XMLGenericParser p;
	p.SetValidating(false);
	this->lib=p.ParseFile(complexemotionlibraryfilename);
	if(this->lib==0)
	{
		initialized=false;
		return 0;
	}
	initialized=true;
	return 1;
}

int **ComplexEmotionManager::GetExpression(std::string classname,std::string instancename)
{
	bool found=false;
	int **faps;

	faps=0;

	std::list<XMLGenericTree*>::iterator iter;

	if(lib==0)
		return 0;

	if(!lib->child.empty())
	{
		iter=lib->child.begin();
		while((iter!=lib->child.end())&&(found==false))
		{
			if((*iter)->name=="complex-emotion")
			{
				if((*iter)->GetAttribute("name")==instancename)
				{

					/*
					if(CreateInputForComplexExpressionModule(*iter,inimanager.Program_Path+"fd/mixmodel/input.xml")==1)
						faps=RunComplexExpressionModule(inimanager.Program_Path+"fd/mixmodel/input.xml");
					else
						return 0;
					*/

					std::string outstring; //it is a request to QuickBlending.jar via Psyclone
					CreateInputBufferForComplexExpressionModule(*iter,outstring);
					
					Psydule p("ComplexEmotionManager",inimanager.GetValueString("PSYCLONE_HOST").c_str(),inimanager.GetValueInt("PSYCLONE_PORT"));					
					
					//if(p.Register(GretaName+PSYCLONE_GRETA_WHITEBOARD,GretaName+PSYCLONE_COMPLEX_OUTPUT)!=0)					
					if (registered)
					{
						if(psy->PostString(outstring,GretaName+PSYCLONE_GRETA_WHITEBOARD,GretaName+PSYCLONE_COMPLEX_INPUT)!=0)
						{
							//wait max. for 1 second for an answer, otherwise exits
														
							int step=1;
							std::string answer="";
							while ((answer=="") && (step<100))
							{
								answer=psy->ReceiveString(5); //aswer from QuickBlending.jar
								step++;
								//if (answer!="") printf ("answer after (mms): %d \n",step*5);
							}
						
							//std::string answer=p.ReceiveString(1000); //aswer from QuickBlending.jar							
							
							if(answer!="")
							{
								faps=RunComplexExpressionModule("",answer);
							}
							else
							{
								printf("error: did not get faps for complex-emotion %s!\n",instancename.c_str());
							}
						}
					}
					found=true;
				}
			}
			iter++;
		}
	}

	if(found==false)
		printf("warning: complex-emotion %s not found in the earl library!\n",instancename.c_str());

	if(faps==0)
		printf("there was a problem running the complex expression module (%s=%s)\n",classname.c_str(),instancename.c_str());

	return faps;
}

int ComplexEmotionManager::CreateInputForComplexExpressionModule(XMLGenericTree *tree,std::string complexexpressionfilename)
{
	std::ofstream outfile(complexexpressionfilename.c_str());
	if (!outfile) { 
		printf("error creating complex emotion module input file.\n"); 
		return 0;
	}
	
	std::string outstring;

	CreateInputBufferForComplexExpressionModule(tree,outstring);

	outfile << outstring;
	
	outfile.close();
	return 1;
}




//send a part of tree with complex facial expression
void ComplexEmotionManager::CreateInputBufferForComplexExpressionModule(XMLGenericTree *tree,std::string &outstring)
{

/*
how it works?

1.  you write e.g. NEUTRAL_maskedby_FEAR in APML/BML.
 
2. in earl there is a line:
   <complex-emotion name="NEUTRAL_maskedby_FEAR">
		<emotion category="NEUTRAL" regulation="suppress"/>
		<emotion category="FEAR" regulation="simulate"/>
	</complex-emotion>

3. you switch to :

		<emotion category="NEUTRAL" regulation="suppress"/>
		<emotion category="FEAR" regulation="simulate"/>

4. then you arrive here, and yo switch again 3. to valid QuickBlending.jar string

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

	std::string emotion1,emotion2;
	std::string regulation1,regulation2;
	std::string intensity1,intensity2;
	std::string emotiontype;

	std::list<XMLGenericTree*>::iterator t;

	t=tree->child.begin();

	emotion1=(*t)->GetAttribute("category");
	regulation1=(*t)->GetAttribute("regulation");
	intensity1=(*t)->GetAttribute("intensity");
	if(intensity1=="") intensity1="1.0";

		t++;

	emotion2=(*t)->GetAttribute("category");
	regulation2=(*t)->GetAttribute("regulation");
	intensity2=(*t)->GetAttribute("intensity");
	if(intensity2=="") intensity2="1.0";


	/* //zacmienie umyslu
		
		//previous version:

	if((regulation1=="amplify")||(regulation1=="attenuate")){
		t++;

		if ((*t)->GetAttribute("category")!="") emotion2=(*t)->GetAttribute("category"); else emotion2="";
		if ((*t)->GetAttribute("regulation")!="") regulation2=(*t)->GetAttribute("regulation"); else regulation2="";
		if ((*t)->GetAttribute("intensity")!="") {
			intensity2=(*t)->GetAttribute("intensity");
			if(intensity2=="") intensity2="1.0";
		} else intensity2="0.0";
	} else {	
		emotion2="";
		regulation2="";
		intensity2="";
	}
	*/

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

	outstring="";

	outstring += "<blending>\n";

	if(emotiontype=="masking")
	{
		outstring = outstring+ "<" + emotiontype + " length=\"10\" static=\"true\">\n";

		if(regulation1=="simulate")
			outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"fake\"/>\n";
		if(regulation1=="suppress")
			outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"felt\"/>\n";
		if(regulation2=="simulate")
			outstring = outstring+ "<emotion name=\"" + emotion2 + "\" intensity=\"" + intensity2 + "\" type=\"fake\"/>\n";
		if(regulation2=="suppress")
			outstring = outstring+ "<emotion name=\"" + emotion2 + "\" intensity=\"" + intensity2 + "\" type=\"felt\"/>\n";
	}

	if(emotiontype=="superposition")
	{
		outstring = outstring+ "<" + emotiontype + " length=\"10\" static=\"true\">\n";

		outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\"/>\n";
		outstring = outstring+ "<emotion name=\"" + emotion2 + "\" intensity=\"" + intensity2 + "\"/>\n";
	}


	//TODO: double conversion sting to float to string
	if(emotiontype=="attenuated")
	{
		outstring = outstring+ "<" + "masking" + " length=\"10\" static=\"true\">\n";

		outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"felt\"/>\n";
		outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"fake\"/>\n";
	}

	//TODO: double conversion sting to float to string
	if(emotiontype=="exaggerated")
	{
		outstring = outstring+ "<" + "masking" + " length=\"10\" static=\"true\">\n";

		outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"felt\"/>\n";
		outstring = outstring+ "<emotion name=\"" + emotion1 + "\" intensity=\"" + intensity1 + "\" type=\"fake\"/>\n";
	}
	
		
	if(emotiontype=="masking")
		outstring = outstring + "<mtype name=\"static\"/>\n";

	if(emotiontype=="exaggerated")
		outstring = outstring +"<mtype name=\"facilitation\"/>\n";

	if(emotiontype=="attenuated")
		outstring = outstring + "<mtype name=\"suppression\"/>\n";

	
if	((emotiontype=="exaggerated")||(emotiontype=="attenuated")) outstring = outstring+ "</" + "masking" + ">\n";
	else outstring = outstring+ "</" + emotiontype + ">\n";
	
	outstring = outstring+ "</blending>\n";
	
}


int **ComplexEmotionManager::RunComplexExpressionModule(std::string complexexpressionfilename,std::string buffer)
{
	int **faps;
	int i;

	
	// never used !!!
	if(complexexpressionfilename!="")
	{
		chdir((inimanager.Program_Path+"/fd/mixmodel").c_str());

		std::string javapath,parameter1;

		parameter1=" "+inimanager.Program_Path+"fd/mixmodel/;"+inimanager.Program_Path+"fd/mixmodel/blending.jar;"+inimanager.Program_Path+"fd/mixmodel/fuzzyJ18.jar";

		javapath=inimanager.GetValueString("ENGINE_JAVA_PATH");

		if(javapath=="")
		{
			printf("java path not found in the ini file\n");
			return 0;
		}

		if(_spawnl( _P_WAIT,javapath.c_str()," ",
			"-cp",
			parameter1.c_str(),
			"it.unipg.dipmat.radek.blends.SimpleApplication",complexexpressionfilename.c_str(),
			(inimanager.Program_Path+"fd/mixmodel/output.xml").c_str(),
			(inimanager.Program_Path+"fd/mixmodel/").c_str(),
			NULL)!=EXIT_SUCCESS)
		{
			printf("error executing %s\n",javapath.c_str());
			chdir(inimanager.Program_Path.c_str());
			return 0;
		}
		
	}


	chdir(inimanager.Program_Path.c_str());

	XMLGenericTree *tree;

	XMLGenericParser parser;

	parser.SetValidating(false);


	// never used !!!
	if(complexexpressionfilename!="")
	{
		tree=parser.ParseFile("C:/code/Greta/runtime/fd/mixmodel/output.xml");
	}
	else
	{
		tree=parser.ParseBuffer((char *)buffer.c_str());
	}


	if(tree==0)
		return 0;

	faps=(int**)malloc(sizeof(int*)*2);
	for(i=0;i<2;i++)
	{
		faps[i]=(int*)malloc(sizeof(int)*69);
		for(int j=0;j<69;j++)
			faps[i][j]=0;
	}


	std::list<XMLGenericTree*>::iterator iter,keyframe,fap;

	for(iter=(tree->child).begin();iter!=(tree->child).end();iter++)
	{
		for(keyframe=((*iter)->child).begin();keyframe!=((*iter)->child).end();keyframe++)
		{
			if((*keyframe)->GetAttributef("keyframe")==0)
			{
				for(fap=((*keyframe)->child).begin();fap!=((*keyframe)->child).end();fap++)
				{
					faps[0][(int)((*fap)->GetAttributef("fap"))]=1;
					faps[1][(int)((*fap)->GetAttributef("fap"))]=(*fap)->GetAttributef("value");
				}
			}
		}
	}


	return faps;
}