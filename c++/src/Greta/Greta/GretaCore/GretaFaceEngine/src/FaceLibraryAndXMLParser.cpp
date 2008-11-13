//Copyright 1999-2005 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

// FaceLibraryAndXMLParser.cpp: implementation of the FaceLibraryAndXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "FaceLibraryAndXMLParser.h"
#include "FaceEngine.h"
//#include "../eLiteDyn.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include "IniManager.h"

extern IniManager inimanager;


extern "C" void yyrestart(FILE*);
extern "C" int yyparse(void);
extern "C" fap_id_list *dictionary;
//extern "C" struct FT_Info;
//extern "C" void FestivalSaveStringToPhoFile(FT_Info *info,char *text,char *output_file,int flag);
//extern "C" FT_Info *server_info;

extern FILE *face_log;

#define OUTPUT

FaceLibraryAndXMLParser::FaceLibraryAndXMLParser(void *e)
{
	engine=e;
	firstexpression=NULL;
	lastexpression=NULL;
	tmp_file_index=1;
	textfile=fopen(((FaceEngine*)engine)->filenames.Text_File,"w");
}

FaceLibraryAndXMLParser::~FaceLibraryAndXMLParser()
{
}

int FaceLibraryAndXMLParser::ParseFacialDisplaysLibrary(char *facialdisplayslibrarypath)
{
	FILE *facialdisplayslibraryfile;
	if(dictionary==NULL)//avoid to re-read the face library dictionary
	{
		facialdisplayslibraryfile=fopen(facialdisplayslibrarypath,"r");
		if (!facialdisplayslibraryfile)
		{
			printf("\n\nError: facial displays library no found\n\n!");
			return 0;
		}
	#ifdef OUTPUT
		fprintf(face_log,"\n\nParsing facial displays library...");
	#endif
		yyrestart(facialdisplayslibraryfile);
		yyparse();
		fclose(facialdisplayslibraryfile);
	#ifdef OUTPUT
		fprintf(face_log,"done\n\n");
	#endif
	}
	return 1;
}

int FaceLibraryAndXMLParser::ParseXMLFile(char *filename,xmlNodePtr &root,FacialExpression **first_expr,FacialExpression **last_expr) 
{
	/////ERASE
	/////  |
	/////  v
#ifdef HACK_EXPRESSIONS_TIMINGS
//	expression* Hexpr;
#endif
#ifdef OUTPUT
	fprintf(face_log,"\nParsing file '%s'...\n",filename);
#endif

	/*
	* build an XML tree from a the file;
	*/



	doc = xmlParseFile(filename);
	if (doc == NULL) 
	{
#ifdef OUTPUT
		fprintf(face_log,"File '%s' not found\n", filename);
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

#ifdef HACK_EXPRESSIONS_TIMINGS
	//Hexpr=first_expr;
	//printf("********HACKING EXPRESSIONS\n");
	//while(Hexpr!=NULL)
	//{
	//	printf("function %s; value %s; start %f; duration %f;\n",Hexpr->function,Hexpr->value,Hexpr->start_time,Hexpr->dur_time);
	//	Hexpr=Hexpr->next;
	//}
#endif
	xmlFreeDoc(doc);
	xmlCleanupParser();
	*first_expr=firstexpression;
	*last_expr=lastexpression;

	fclose(textfile);

	return 1;
}

//! Depth First Search of the XML tree
void FaceLibraryAndXMLParser::DFS(xmlNodePtr node)
{
	//printf("node->name:%s\n",node->name);
	CreateExpressionsFromXML(node);        
	
	node=node->children;
	
	while (node != NULL) 
	{
			
		DFS(node);
		node=node->next;		
	}
}

char *FaceLibraryAndXMLParser::new_string(const char *s1)
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

//! Get communication function name from XML node
char* FaceLibraryAndXMLParser::getFunction(xmlNodePtr node)
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
char* FaceLibraryAndXMLParser::class_exists(char *class_type)
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


//old check_node
void FaceLibraryAndXMLParser::CreateExpressionsFromXML(xmlNodePtr node)
{
	FacialExpression *expr;
	int rheme_or_theme=PHONEME_RHEME;

	expr=new FacialExpression();

	
	
	// deallocando crashava qui con la linea successiva 
	// forse perche' andavo a deallocare due volte
	// la stessa funzione
//	expr->function=getFunction(node);
	expr->function=new_string(getFunction(node));
	if (expr->function==NULL)
		ParseError(1,(char*)node->name,NULL);
	else
	if (!strcmp(expr->function,"APML")||
		!strcmp(expr->function,"apml"))
	{
		if (node!=treeroot)
			ParseError(2,(char*)node->name,NULL);
		else
		{
			expr->value=(char*)node->name;

			InsertExpr(expr);
//			BuildTextDep(expr,node);				

		}
		
	}
	else
	if (strcmp(expr->function,"text")!=0)
	{
		rheme_or_theme=!strcmp(expr->function,"rheme")?PHONEME_RHEME:PHONEME_THEME;

		if (!strcmp(expr->function,"theme")||
			!strcmp(expr->function,"rheme"))
		{
			char *affect;
			char *belief;
			char *certainty;
			char *metacog;

			//expr->value=expr->function;
			//InsertExpr(expr);
			//BuildTextDep(expr,node);				
			affect=(char*)xmlGetProp(node,(const unsigned char*)"affect");
			if (affect)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=affect;
				new_expr->function=new_string("affect");
				new_expr->rheme_or_theme=rheme_or_theme;
/////HACK_EXPRESSIONS_TIMINGS
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
			belief=(char*)xmlGetProp(node,(const unsigned char*)"belief-relation");
			if (belief)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=belief;
				new_expr->function=new_string("belief-relation");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
			certainty=(char*)xmlGetProp(node,(const unsigned char*)"certainty");
			if (certainty)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=certainty;
				new_expr->function=new_string("certainty");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
			metacog=(char*)xmlGetProp(node,(const unsigned char*)"meta-cognitive");
			if (metacog)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=metacog;
				new_expr->function=new_string("meta-cognitive");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
		}	
		else
		if (!strcmp(expr->function,"emphasis"))
		{
			char *type;
			char *level;
			char *xpitch;
			char *deictic;
			char *adjectival;

			type=(char*)xmlGetProp(node,(const unsigned char*)"type");
			if (type)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=type;
				new_expr->function=new_string("emphasis");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}

			level=(char*)xmlGetProp(node,(const unsigned char*)"level");
			if (level)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=level;
				new_expr->function=new_string("emphasis");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
			xpitch=(char*)xmlGetProp(node,(const unsigned char*)"x-pitchaccent");
			if (xpitch)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=xpitch;
				new_expr->function=new_string("emphasis");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
			deictic=(char*)xmlGetProp(node,(const unsigned char*)"deictic");
			if (deictic)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=deictic; // coordinate
//				new_expr->obj=xmlNodeGetContent(node); // object: is it correct?
				new_expr->obj=deictic; // we'll check later if is an object
				new_expr->function=new_string("deictic");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
			adjectival=(char*)xmlGetProp(node,(const unsigned char*)"adjectival");
			if (adjectival)
			{
				FacialExpression *new_expr;

				new_expr=new FacialExpression();
				new_expr->value=adjectival;
				new_expr->function=new_string("adjectival");
				new_expr->rheme_or_theme=rheme_or_theme;
#ifdef HACK_EXPRESSIONS_TIMINGS
				if(xmlGetProp(node,(const unsigned char*)"delay")!=NULL)
					new_expr->delay_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"delay"));
				if(xmlGetProp(node,(const unsigned char*)"duration")!=NULL)
					new_expr->dur_perc=atoi((char*)xmlGetProp(node,(const unsigned char*)"duration"));
				if(xmlGetProp(node,(const unsigned char*)"onset")!=NULL)
					new_expr->onset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"onset"));
				if(xmlGetProp(node,(const unsigned char*)"offset")!=NULL)
					new_expr->offset_num_of_fr=atoi((char*)xmlGetProp(node,(const unsigned char*)"offset"));
#endif
#ifdef HACK_EXPRESSIONS_INTENSITIES
				if(xmlGetProp(node,(const unsigned char*)"intensity")!=NULL)
					new_expr->intensity=(float)atof((char*)xmlGetProp(node,(const unsigned char*)"intensity"));
#endif
				InsertExpr(new_expr);
				BuildTextDep(new_expr,node);				
			}
		}
		else
		if (!strcmp(expr->function,"boundary"))
		{
			expr->value=(char*)xmlGetProp(node,(const unsigned char*)"type");
			if (!expr->value)
			{
				ParseError(4,expr->function,expr->value);
				// TODO: temp fix for unsupported tags
				expr->value=expr->function;
			}
			expr->rheme_or_theme=rheme_or_theme;
			InsertExpr(expr);
			BuildTextDep(expr,node);				
		}
		else
		if (!strcmp(expr->function,"deictic"))
		{
			expr->value=(char*)xmlGetProp(node,(const unsigned char*)"coord");
			if (!expr->value)
				ParseError(3,NULL,NULL);
			
			expr->obj=(char*)xmlGetProp(node,(const unsigned char*)"obj");
			if (!expr->obj)
				ParseError(3,NULL,NULL);
			expr->rheme_or_theme=rheme_or_theme;
			InsertExpr(expr);		
			BuildTextDep(expr,node);				
		}
		else 
		if (!strcmp(expr->function,"pause"))
		{
			expr->value=(char*)xmlGetProp(node,(const unsigned char*)"sec");
			if (!expr->value)
				ParseError(3,NULL,NULL);
			else
				expr->dur_time=(float)atof(expr->value);
			InsertExpr(expr);
		}	
	// new parser: only performative - turnallocation here
			 // old parser: all the old syntax supported
		else
		{
			rheme_or_theme=0;
			expr->value=(char*)xmlGetProp(node,(const unsigned char*)"type");
			if (!expr->value)
			{
				ParseError(4,expr->function,expr->value);
				// TODO: temp fix for unsupported tags
				expr->value=new_string(expr->function);
			}
			else
			if (!(isValueFunction(expr->function,expr->value)))
			{
				ParseError(5,expr->function,expr->value);
			}
			InsertExpr(expr);	
			BuildTextDep(expr,node);				
		}
			
			
	}
	else // se e' del testo ma non vuoto (linea a capo o tab etc etc)
	if (!strcmp(expr->function,"text")&&strcmp((const char*)xmlNodeGetContent(node),"\n")&&!blank_text(node))
	{		
		int i;
		char *temp;
		
		expr->value=(char*)xmlNodeGetContent(node);
		i=0;
		while (expr->value[i]!='\0')
		{
			if (expr->value[i]=='\n')
				expr->value[i]=' ';
			i++;
		}

		temp=(char*)malloc(strlen(expr->value)+3);
		if (!temp)
		{
#ifdef OUTPUT
			fprintf(face_log,"out of memory\n");
#endif
			xmlFreeDoc(doc);
			exit(2);
		}
		strcpy(temp," ");
		strcat(temp,expr->value);
		strcat(temp," ");
		expr->value=temp;
		InsertExpr(expr);

		//sprintf(expr->phonemes_file,"%stmp%d.pho",((FaceEngine*)engine)->configuration.Tmp_Path,tmp_file_index);
		sprintf(expr->phonemes_file,"%stmp\\tmp%d.pho",inimanager.Program_Path.c_str(),tmp_file_index);
//		if (((FaceEngine*)engine)->configuration.Remote_Festival_Server==0)
//		{

		//IF ITALIAN OR ENGLISH
		if(inimanager.GetValueString("ENGINE_LANGUAGE")!="FRENCH")
		{
			((FaceEngine*)engine)->InvertSlashes(expr->phonemes_file);
			fprintf(((FaceEngine*)engine)->script_for_festival,"(Phdur_save_text \"%s\" \"%s\")\n",expr->value,expr->phonemes_file);
			//fprintf(((FaceEngine*)engine)->script_for_festival,"(APML_Phdur_save \"%s\" '%s)\n",expr->value,expr->phonemes_file);
		}

		//IF FRENCH
		if(inimanager.GetValueString("ENGINE_LANGUAGE")=="FRENCH")
		{
#ifndef SKIPFRENCH
#endif
		}
//		}
//		else
//			FestivalSaveStringToPhoFile(server_info,expr->value,expr->phonemes_file,0);
		tmp_file_index++;

	}
	else // e' del testo BLANK
	if (!strcmp(expr->function,"text"))
		free(expr);
	else
	{
		free(expr);
		ParseError(6,(char*)node->name,NULL);
	}
	

}

//! Insert 'expr' in the facial expression list
void FaceLibraryAndXMLParser::InsertExpr(FacialExpression *expr)
{
	if ((firstexpression==NULL)&&(lastexpression==NULL))
	{
		firstexpression=expr;
       	lastexpression=expr;
    }
    else
    {
       	lastexpression->next=expr;
		expr->prev=lastexpression;
		lastexpression=expr;		
    }
}

//!
//! Given an expression 'expr' and his corresponding XML 'node'
//! scan the subtree starting from 'node' and fills in the list 
//! 'expr->text_list' with all the text phrases nested in 'expr'.
//!	
void FaceLibraryAndXMLParser::BuildTextDep(FacialExpression *expr,xmlNodePtr node)
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
            if (expr->text_list==NULL)
				expr->text_list=text_ptr;
			else
			{
                tmp_txt_ptr=expr->text_list;
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
		BuildTextDep(expr,node);
		node=node->next;
	}
}

int FaceLibraryAndXMLParser::isValueFunction(char *function,char *value)
{
	char *temp;

	temp=get_class_id(value);

	if (!temp||strcmp(temp,function))
		return 0;
	return 1;
}

int FaceLibraryAndXMLParser::blank_text(xmlNodePtr node)
{
	int i,len;
	char *value;
	value=(char*)xmlNodeGetContent(node);
	len=strlen(value);
	for (i=0;i<len;i++)
	  if (!isspace(value[i]))
	    return 0;

	return 1;
}

//!
//! Returns the class of FD 'id'
//!
char* FaceLibraryAndXMLParser::get_class_id(char *id)
{
	fap_id_list *fap_element;
//	int i;
	fap_element=dictionary;

	while (fap_element&&strcmp(fap_element->id,id))
		fap_element=(fap_id_list*)(fap_element->next);
	
	if (!fap_element)
		return NULL;

	return fap_element->class_type;
}

void FaceLibraryAndXMLParser::ParseError(int error_code,char *s1,char *s2)
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
