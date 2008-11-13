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

// GretaXMLParser.cpp: implementation of the GretaXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "GretaXMLParser.h"
//#include "../eLiteDyn.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include "IniManager.h"

extern IniManager inimanager;

extern FILE *face_log;

//extern fap_id_list *dictionary;

#define OUTPUT

GretaXMLParser::GretaXMLParser()
{
	first_comm_act=NULL;
	last_comm_act=NULL;
	bc_first_comm_act=NULL;
	bc_last_comm_act=NULL;
	tmp_file_index=1;
	rheme_or_theme=PHONEME_RHEME;
	//textfile=fopen(filenames.Text_File,"w");
}

GretaXMLParser::~GretaXMLParser()
{
}

int GretaXMLParser::ParseXMLFile(std::string filename,xmlNodePtr &root,CommunicativeAct **first_comm_act,CommunicativeAct **last_comm_act) 
{

#ifdef OUTPUT
	fprintf(face_log,"\nParsing file '%s'...\n",filename.c_str());
#endif

	/*
	* build an XML tree from a the file;
	*/



	doc = xmlParseFile(filename.c_str());
	if (doc == NULL) 
	{
#ifdef OUTPUT
		fprintf(face_log,"File '%s' not found\n", filename.c_str());
#endif
		xmlFreeDoc(doc);
		return 0;
	}

    
	/*
	* Check the document is of the right kind
	*/
    
	root = xmlDocGetRootElement(doc);
    if (root == NULL) 
	{
#ifdef OUTPUT
		fprintf(face_log,"Empty document\n");
#endif
		xmlFreeDoc(doc);
		return 0;
	}	

    if (strcmp((char*)root->name,"APML")&&
		strcmp((char*)root->name,"apml"))
  	{
#ifdef OUTPUT
		fprintf(face_log,"Not APML document: %s\n",root->name);
#endif
		xmlFreeDoc(doc);
		return 0;
	}
	
	treeroot=root;
	
	DFS(root);

	xmlFreeDoc(doc);
	xmlCleanupParser();
	*first_comm_act=this->first_comm_act;
	*last_comm_act=this->last_comm_act;

	//fclose(textfile);

	return 1;
}

//! Depth First Search of the XML tree
void GretaXMLParser::DFS(xmlNodePtr node)
{
	//printf("node->name:%s\n",node->name);
	CreateCommActsFromXML(node);
	
	node=node->children;
	
	while (node != NULL) 
	{
			
		DFS(node);
		node=node->next;		
	}
}

char *GretaXMLParser::new_string(const char *s1)
{
	char *s2;

	if (!s1)
		return NULL;

	s2=(char*)malloc(strlen(s1)+1);
	if (!s2)
	{
#ifdef OUTPUT
		fprintf(face_log,"\nMalloc error new_string\n");
#endif
		return NULL;
	}
	memset(s2,0,strlen(s1)+1);
	strcpy(s2,s1);
	return s2;
}

/*

//! Get communication function name from XML node
char* GretaXMLParser::getFunction(xmlNodePtr node)
{		
	char *s;
	// temp fix
	s=NULL;
//	if (strcmp(node->name,"text")&&(strcmp(node->name,"APML")&&strcmp(node->name,"apml")))
//		return class_exists(node->name);
//	return node->name;
	if (strcmp((char*)(node->name),"text")&&(strcmp((char*)(node->name),"APML")&&strcmp((char*)(node->name),"apml")))
		s=class_exists((char*)(node->name));
	if (s==NULL)
		return (char*)(node->name);
	else
		return s;
}

//!
//! Check if 'class_type' exists in the FD Dictionary
//!
char* GretaXMLParser::class_exists(char *class_type)
{
	fap_id_list *fap_element;
//	int i;
	fap_element=dictionary;

	while (fap_element&&strcmp(fap_element->class_type,class_type))
		fap_element=(fap_id_list*)fap_element->next;
	
	if (!fap_element)
		return NULL;

	return class_type;
}
*/

void GretaXMLParser::InitExprPar(CommunicativeAct *comm_act)
{
	comm_act->expressivitySPC=inimanager.GetValueFloat("ENGINE_EXPR_SPC");
	comm_act->expressivityTMP=inimanager.GetValueFloat("ENGINE_EXPR_TMP");
	comm_act->expressivityFLD=inimanager.GetValueFloat("ENGINE_EXPR_FLD");
	comm_act->expressivityPWR=inimanager.GetValueFloat("ENGINE_EXPR_PWR");
	comm_act->expressivityREP=inimanager.GetValueFloat("ENGINE_EXPR_REP");
}

//old check_node
void GretaXMLParser::CreateCommActsFromXML(xmlNodePtr node)
{
	CommunicativeAct *comm_act;

	comm_act=new CommunicativeAct();
	
	// deallocando crashava qui con la linea successiva 
	// forse perche' andavo a deallocare due volte
	// la stessa funzione
//	comm_act->function=getFunction(node);
//	comm_act->function=new_string(getFunction(node));
	comm_act->function=new_string((char*)node->name);

	InitExprPar(comm_act);
	
	if (comm_act->function==NULL)
		ParseError(1,(char*)node->name,NULL);
	else
	if (!strcmp(comm_act->function,"APML")||
		!strcmp(comm_act->function,"apml"))
	{
		if (node!=treeroot)
			ParseError(2,(char*)node->name,NULL);
		else
		{
			comm_act->value=new_string((char*)node->name);

			InsertCommAct(comm_act);
//			BuildTextDep(comm_act,node);				

		}
		
	}
	else
	if (strcmp(comm_act->function,"text")!=0)
	{
		if(strcmp(comm_act->function,"theme")==0)
			rheme_or_theme=PHONEME_THEME;
		if(strcmp(comm_act->function,"rheme")==0)
			rheme_or_theme=PHONEME_RHEME;

		comm_act->rheme_or_theme=rheme_or_theme;

		if (!strcmp(comm_act->function,"theme")||
			!strcmp(comm_act->function,"rheme"))
		{
			char *affect;
			char *belief;
			char *certainty;
			char *metacog;

			//comm_act->value=comm_act->function;
			//InsertExpr(comm_act);
			//BuildTextDep(comm_act,node);				
			affect=(char*)xmlGetProp(node,(const unsigned char*)"affect");
			if (affect)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(affect);
				new_comm_act->function=new_string("affect");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));

				//Intensity
				if((char*)xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_comm_act->setIntensity(atof((char*)xmlGetProp(node,(const unsigned char*)"intensity")));

				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
			belief=(char*)xmlGetProp(node,(const unsigned char*)"belief-relation");
			if (belief)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(belief);
				new_comm_act->function=new_string("belief-relation");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			

				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
			certainty=(char*)xmlGetProp(node,(const unsigned char*)"certainty");
			if (certainty)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(certainty);
				new_comm_act->function=new_string("certainty");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			
				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
			metacog=(char*)xmlGetProp(node,(const unsigned char*)"meta-cognitive");
			if (metacog)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(metacog);
				new_comm_act->function=new_string("meta-cognitive");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			
				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
		}	
		else
		if (!strcmp(comm_act->function,"emphasis"))
		{
			char *type;
			char *level;
			char *xpitch;
			char *deictic;
			char *adjectival;

			type=(char*)xmlGetProp(node,(const unsigned char*)"type");
			if (type)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(type);
				new_comm_act->function=new_string("emphasis");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			
				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}

			level=(char*)xmlGetProp(node,(const unsigned char*)"level");
			if (level)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(level);
				new_comm_act->function=new_string("emphasis");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			
				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
			xpitch=(char*)xmlGetProp(node,(const unsigned char*)"x-pitchaccent");
			if (xpitch)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(xpitch);
				new_comm_act->function=new_string("emphasis");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			
				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
			deictic=(char*)xmlGetProp(node,(const unsigned char*)"deictic");
			if (deictic)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(deictic); // coordinate
//				new_comm_act->obj=xmlNodeGetContent(node); // object: is it correct?
				new_comm_act->obj=deictic; // we'll check later if is an object
				new_comm_act->function=new_string("deictic");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
			adjectival=(char*)xmlGetProp(node,(const unsigned char*)"adjectival");
			if (adjectival)
			{
				CommunicativeAct *new_comm_act;

				new_comm_act=new CommunicativeAct();
				new_comm_act->value=new_string(adjectival);
				new_comm_act->function=new_string("adjectival");
				new_comm_act->rheme_or_theme=rheme_or_theme;

				InitExprPar(new_comm_act);

				//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					new_comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));
			
				InsertCommAct(new_comm_act);
				BuildTextDep(new_comm_act,node);				
			}
		}
		else
		if (!strcmp(comm_act->function,"boundary"))
		{
			comm_act->value=(char*)xmlGetProp(node,(const unsigned char*)"type");
			if (!comm_act->value)
			{
				ParseError(4,comm_act->function,comm_act->value);
				// TODO: temp fix for unsupported tags
				comm_act->value=new_string(comm_act->function);
			}
			comm_act->rheme_or_theme=rheme_or_theme;
			InsertCommAct(comm_act);
			BuildTextDep(comm_act,node);				
		}
		else
		if (!strcmp(comm_act->function,"deictic"))
		{
			comm_act->value=(char*)xmlGetProp(node,(const unsigned char*)"coord");
			if (!comm_act->value)
				ParseError(3,NULL,NULL);
			
			comm_act->obj=(char*)xmlGetProp(node,(const unsigned char*)"obj");
			if (!comm_act->obj)
				ParseError(3,NULL,NULL);
			comm_act->rheme_or_theme=rheme_or_theme;
			InsertCommAct(comm_act);		
			BuildTextDep(comm_act,node);				
		}
		else
		//// BACKCHANNEL
		if (!strcmp(comm_act->function,"backchannel"))
		{
			char *contact;
			char *perception;
			char *attention;
			char *interest;
			char *understanding;
			char *attitude;

			contact=(char*)xmlGetProp(node,(const unsigned char*)"contact");
			if (contact)
			{
				BC_CommunicativeAct *new_bc_comm_act;

				new_bc_comm_act=new BC_CommunicativeAct();
				new_bc_comm_act->value=new_string(contact);
				new_bc_comm_act->function=new_string("contact");
			
				new_bc_comm_act->Print();
				InsertBackChannelCommAct(new_bc_comm_act);
				//BuildTextDep(new_comm_act,node);
				
			}

			perception=(char*)xmlGetProp(node,(const unsigned char*)"perception");
			if (perception)
			{
				BC_CommunicativeAct *new_bc_comm_act;

				new_bc_comm_act=new BC_CommunicativeAct();
				new_bc_comm_act->value=new_string(perception);
				new_bc_comm_act->function=new_string("perception");
			
				InsertBackChannelCommAct(new_bc_comm_act);
				//BuildTextDep(new_comm_act,node);
			}
			attention=(char*)xmlGetProp(node,(const unsigned char*)"attention");
			if (attention)
			{	
				BC_CommunicativeAct *new_bc_comm_act;

				new_bc_comm_act=new BC_CommunicativeAct();
				new_bc_comm_act->value=new_string(attention);
				new_bc_comm_act->function=new_string("attention");
			
				InsertBackChannelCommAct(new_bc_comm_act);
				//BuildTextDep(new_comm_act,node);
			}
			interest=(char*)xmlGetProp(node,(const unsigned char*)"interest");
			if (interest)
			{
				BC_CommunicativeAct *new_bc_comm_act;

				new_bc_comm_act=new BC_CommunicativeAct();
				new_bc_comm_act->value=new_string(interest);
				new_bc_comm_act->function=new_string("interest");
			
				InsertBackChannelCommAct(new_bc_comm_act);
				//BuildTextDep(new_comm_act,node);			
			}
			understanding=(char*)xmlGetProp(node,(const unsigned char*)"understanding");
			if (understanding)
			{
				BC_CommunicativeAct *new_bc_comm_act;

				new_bc_comm_act=new BC_CommunicativeAct();
				new_bc_comm_act->value=new_string(understanding);
				new_bc_comm_act->function=new_string("understanding");
			
				InsertBackChannelCommAct(new_bc_comm_act);
				//BuildTextDep(new_comm_act,node);			
			}
			attitude=(char*)xmlGetProp(node,(const unsigned char*)"attitude");
			if (attitude)
			{
				BC_CommunicativeAct *new_bc_comm_act;

				new_bc_comm_act=new BC_CommunicativeAct();
				new_bc_comm_act->value=new_string(attitude);
				new_bc_comm_act->function=new_string("attitude");
			
				InsertBackChannelCommAct(new_bc_comm_act);
				//BuildTextDep(new_comm_act,node);			
			}
		}
		else
		if (!strcmp(comm_act->function,"pause"))
		{
			comm_act->value=(char*)xmlGetProp(node,(const unsigned char*)"sec");
			if (!comm_act->value)
				ParseError(3,NULL,NULL);
			else
				comm_act->dur_time=(float)atof(comm_act->value);
			InsertCommAct(comm_act);
		}	
	// new parser: only performative - turnallocation here
			 // old parser: all the old syntax supported
		else
		{
			rheme_or_theme=0;

			comm_act->rheme_or_theme=0;

			comm_act->value=new_string((char*)xmlGetProp(node,(const unsigned char*)"type"));
			
			//Elisabetta 01/2006 Activation
				if((char*)xmlGetProp(node,(const unsigned char*)"activation")!=NULL)
					comm_act->activation=atof((char*)xmlGetProp(node,(const unsigned char*)"activation"));

			if (!comm_act->value)
			{
				ParseError(4,comm_act->function,comm_act->value);
				// TODO: temp fix for unsupported tags
				comm_act->value=new_string(comm_act->function);
			}
			InsertCommAct(comm_act);	
			BuildTextDep(comm_act,node);				
		}
			
			
	}
	else // se e' del testo ma non vuoto (linea a capo o tab etc etc)
	if (!strcmp(comm_act->function,"text")&&strcmp((const char*)xmlNodeGetContent(node),"\n")&&!blank_text(node))
	{		
		int i;
		char *temp;
		
		comm_act->value=new_string((char*)xmlNodeGetContent(node));
		i=0;
		while (comm_act->value[i]!='\0')
		{
			if (comm_act->value[i]=='\n')
				comm_act->value[i]=' ';
			i++;
		}

		temp=(char*)malloc(strlen(comm_act->value)+3);
		if (!temp)
		{
#ifdef OUTPUT
			fprintf(face_log,"out of memory\n");
#endif
			xmlFreeDoc(doc);
			exit(2);
		}
		strcpy(temp," ");
		strcat(temp,comm_act->value);
		strcat(temp," ");
		comm_act->value=temp;
		InsertCommAct(comm_act);

		char index[10];
		sprintf(index,"%d",tmp_file_index);
		//sprintf(comm_act->phonemes_file,"%stmp%d.pho",((FaceEngine*)engine)->configuration.Tmp_Path,tmp_file_index);
		comm_act->phonemes_file=inimanager.Program_Path+"tmp/tmp";
		comm_act->phonemes_file+=index;
		comm_act->phonemes_file+=".pho";
	
//		if (((FaceEngine*)engine)->configuration.Remote_Festival_Server==0)
//		{

		//IF ITALIAN OR ENGLISH
		if(inimanager.GetValueString("FESTIVAL_LANGUAGE")!="FRENCH")
		{
			InvertSlashes(comm_act->phonemes_file);
			//TODO:
			//reinsert fprintf(((FaceEngine*)engine)->script_for_festival,"(Phdur_save_text \"%s\" \"%s\")\n",comm_act->value,comm_act->phonemes_file);
			//VERY OLD:
			//fprintf(((FaceEngine*)engine)->script_for_festival,"(APML_Phdur_save \"%s\" '%s)\n",comm_act->value,comm_act->phonemes_file);
		}

		tmp_file_index++;

	}
	else // e' del testo BLANK
	if (!strcmp(comm_act->function,"text"))
		free(comm_act);
	else
	{
		free(comm_act);
		ParseError(6,(char*)node->name,NULL);
	}
	

}

//! Insert 'comm_act' in the communicative acts list
void GretaXMLParser::InsertCommAct(CommunicativeAct *comm_act)
{
	if ((first_comm_act==NULL)&&(last_comm_act==NULL))
	{
		first_comm_act=comm_act;
       	last_comm_act=comm_act;
    }
    else
    {
       	last_comm_act->next=comm_act;
		comm_act->prev=last_comm_act;
		last_comm_act=comm_act;		
    }
}

//! Insert 'backchannel_comm_act' in the backchannel communicative acts list
void GretaXMLParser::InsertBackChannelCommAct(BC_CommunicativeAct *bc_comm_act)
{
	if ((bc_first_comm_act==NULL)&&(bc_last_comm_act==NULL))
	{
		bc_first_comm_act=bc_comm_act;
       	bc_last_comm_act=bc_comm_act;
    }
    else
    {
       	bc_last_comm_act->next=bc_comm_act;
		bc_comm_act->prev=bc_last_comm_act;
		bc_last_comm_act=bc_comm_act;		
    }
	bc_comm_act->prev_comm_act=last_comm_act;
}

//!
//! Given a communicative act 'comm_act' and his corresponding XML 'node'
//! scan the subtree starting from 'node' and fills in the list 
//! 'comm_act->text_list' with all the text phrases nested in 'comm_act'.
//!	
void GretaXMLParser::BuildTextDep(CommunicativeAct *comm_act,xmlNodePtr node)
{
	char *temp;
	text_l *text_ptr;
	text_l *tmp_txt_ptr;
	int i;

	if (strcmp((const char*)node->name,"text")==0)
	{	
		temp=(char*)xmlNodeGetContent(node);
		
		if (temp&&!blank_text(node))
		{
			i=0;
			while (temp[i]!='\0')
			{
				if (temp[i]=='\n')
					temp[i]=' ';
				i++;
			}
			
			text_ptr=(text_l*)malloc(sizeof(text_l));

			if (text_ptr == NULL) 
			{
#ifdef OUTPUT
				fprintf(face_log,"out of memory\n");
#endif
				xmlFreeDoc(doc);
				exit(2);
			}

            memset(text_ptr, 0, sizeof(text_l));
			text_ptr->next=NULL;
			text_ptr->s=(char*)malloc(strlen(temp)+3);
			strcpy(text_ptr->s," ");
			strcat(text_ptr->s,temp);
			strcat(text_ptr->s," ");
            if (comm_act->text_list==NULL)
				comm_act->text_list=text_ptr;
			else
			{
                tmp_txt_ptr=comm_act->text_list;
				while (tmp_txt_ptr->next)
					tmp_txt_ptr=(text_l*)(tmp_txt_ptr->next);
				tmp_txt_ptr->next=text_ptr;
			}

		}
		//printf("<%s>\n",s);
	}

	node=node->children;
	
	while (node)
	{
		BuildTextDep(comm_act,node);
		node=node->next;
	}
}

void GretaXMLParser::InvertSlashes(std::string target)
{
	int i,l;
	l=target.length();
	for(i=0;i<l;i++)
		if(target[i]=='\\')target[i]='/';
}


int GretaXMLParser::blank_text(xmlNodePtr node)
{
	int i,len;
	char *value;
	value=(char*)xmlNodeGetContent(node);
	len=strlen(value);
	for (i=0;i<len;i++)
	  if ((value[i]!=' ')&&(value[i]!=10)&&(value[i]!=13))
	    return 0;

	return 1;
}


void GretaXMLParser::ParseError(int error_code,char *s1,char *s2)
{
	switch (error_code)
	{
	case 1:
#ifdef OUTPUT
		fprintf(face_log,"<??%s??? ........>\n",s1);
#endif
#ifdef STOP_PARSER_ON_ERROR
		xmlFreeDoc(doc);
		exit(2);
#endif
		break;
	case 2:
#ifdef OUTPUT
		fprintf(face_log,"<APML> must be first tag!\n",s1);
#endif
#ifdef STOP_PARSER_ON_ERROR
		xmlFreeDoc(doc);
		exit(2);
#endif
		break;
	case 3:
#ifdef OUTPUT
		fprintf(face_log,"<deictic ???=......\n");
#endif
#ifdef STOP_PARSER_ON_ERROR
		xmlFreeDoc(doc);
		exit(2);
#endif
		break;
	case 4:
#ifdef OUTPUT
		fprintf(face_log,"<%s ???%s???=......\n",s1,s2);
#endif
#ifdef STOP_PARSER_ON_ERROR
		xmlFreeDoc(doc);
		exit(2);
#endif
		break;
	case 5:
#ifdef OUTPUT
		fprintf(face_log,"<%s type=\"???%s???\"\n",s1,s2);
#endif
#ifdef STOP_PARSER_ON_ERROR
		xmlFreeDoc(doc);
		exit(2);
#endif
		break;
	case 6:
#ifdef OUTPUT
		fprintf(face_log,"<?%s?.......>\n",s1);
#endif
#ifdef STOP_PARSER_ON_ERROR
		xmlFreeDoc(doc);
		exit(2);
#endif
		break;

	}
}
