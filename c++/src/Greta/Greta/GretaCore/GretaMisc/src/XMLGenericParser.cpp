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

// XMLGenericParser.cpp: implementation of the XMLGenericParser class.
//
//////////////////////////////////////////////////////////////////////

#include "XMLGenericParser.h"
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <iostream>
#include <fstream>
#include <libxml/xmlreader.h>



//for general comments refer to the header file

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XMLGenericParser::XMLGenericParser()
{
	result_tree=0;
	validating=true;
}

XMLGenericParser::~XMLGenericParser()
{

}

XMLGenericTree *XMLGenericParser::ParseFile(std::string name)
{
	xmlTextReaderPtr reader;
	int ret;

	result_tree=0;

	//xmlSwitchEncoding();

    reader = xmlNewTextReaderFilename(name.c_str());

	xmlTextReaderNormalization(reader);

	xmlTextReaderSetParserProp(reader,XML_PARSER_DEFAULTATTRS,1);

	//reader = xmlReaderForFile	(name.c_str(), 
	//					 "iso-8859-1", 0);

	if (reader != NULL)
	{
		if(validating)
		{
			ret = xmlTextReaderSetParserProp(reader,XML_PARSER_VALIDATE,1);
			ret = xmlTextReaderIsValid(reader);
		}
		else
		{
			ret = xmlTextReaderSetParserProp(reader,XML_PARSER_VALIDATE,0);
			ret = 1;
		}
        
		if(ret==1)
		{
			result_tree=CreateTree(reader);
			xmlFreeTextReader(reader);
			return result_tree;
		}
		else
		{
			xmlFreeTextReader(reader);
			return 0;
		}
	}
	else
	{
        printf("Unable to open %s\n", name.c_str());
		return 0;
    }
}

void GretaXmlTextReaderErrorFunc (void * arg, const char * msg, xmlParserSeverities severity, xmlTextReaderLocatorPtr locator)
{
	printf("Error parsing file:\n line %d column %d\n",
		xmlTextReaderGetParserLineNumber((xmlTextReaderPtr)arg),
		xmlTextReaderGetParserColumnNumber((xmlTextReaderPtr)arg));
}

XMLGenericTree *XMLGenericParser::ParseFileWithXSD(std::string name,std::string xsdpath)
{
	xmlTextReaderPtr reader;
	int ret;

	result_tree=0;

	//xmlSwitchEncoding();

    reader = xmlNewTextReaderFilename(name.c_str());

	xmlTextReaderNormalization(reader);

	xmlTextReaderSetParserProp(reader,XML_PARSER_DEFAULTATTRS,1);

	//xmlTextReaderSetErrorHandler(reader,GretaXmlTextReaderErrorFunc,reader);

	//reader = xmlReaderForFile	(name.c_str(), 
	//					 "iso-8859-1", 0);

	if (reader != NULL)
	{

		ret = xmlTextReaderSchemaValidate(reader,xsdpath.c_str());
        
		if(ret==0)
		{
			validating=1;
			result_tree=CreateTree(reader);
			xmlFreeTextReader(reader);
			return result_tree;
		}
		else
		{
			xmlFreeTextReader(reader);
			return 0;
		}
	}
	else
	{
        printf("[With XSD] Unable to open %s\n", name.c_str());
		return 0;
    }
}

XMLGenericTree *XMLGenericParser::ParseBufferWithXSD(char *buffer,std::string xsdpath)
{
	xmlTextReaderPtr reader;
	int ret;

	result_tree=0;

	//xmlSwitchEncoding();

	reader = xmlReaderForMemory(buffer,strlen(buffer),"","",0);

	xmlTextReaderNormalization(reader);

	xmlTextReaderSetParserProp(reader,XML_PARSER_DEFAULTATTRS,1);

	//xmlTextReaderSetErrorHandler(reader,GretaXmlTextReaderErrorFunc,reader);

	//reader = xmlReaderForFile	(name.c_str(), 
	//					 "iso-8859-1", 0);

	if (reader != NULL)
	{

		ret = xmlTextReaderSchemaValidate(reader,xsdpath.c_str());
        
		if(ret==0)
		{
			validating=1;
			result_tree=CreateTree(reader);
			xmlFreeTextReader(reader);
			return result_tree;
		}
		else
		{
			xmlFreeTextReader(reader);
			return 0;
		}
	}
	else
	{
        printf("Unable to parse buffer\n");
		return 0;
    }
}

XMLGenericTree *XMLGenericParser::ParseBuffer(char *buffer)
{
	xmlTextReaderPtr reader;
	int ret;

	result_tree=0;

	//xmlSwitchEncoding();

    reader = xmlReaderForMemory(buffer,strlen(buffer),"","",0);

	xmlTextReaderNormalization(reader);

	xmlTextReaderSetParserProp(reader,XML_PARSER_DEFAULTATTRS,1);

	//reader = xmlReaderForFile	(name.c_str(), 
	//					 "iso-8859-1", 0);

	if (reader != NULL)
	{
		if(validating)
		{
			ret = xmlTextReaderSetParserProp(reader,XML_PARSER_VALIDATE,1);
			ret = xmlTextReaderIsValid(reader);
		}
		else
		{
			ret = xmlTextReaderSetParserProp(reader,XML_PARSER_VALIDATE,0);
			ret = 1;
		}
        
		if(ret==1)
		{
			result_tree=CreateTree(reader);
			xmlFreeTextReader(reader);
			return result_tree;
		}
		else
		{
			xmlFreeTextReader(reader);
			return 0;
		}
	}
	else
	{
        printf("Unable parse xml from memory\n");
		return 0;
    }
}

void XMLGenericParser::SetValidating(bool v)
{
	validating=v;
}

XMLGenericTree *XMLGenericParser::CreateTree(void *readerpointer)
{	
	xmlTextReaderPtr reader;
	XMLGenericTree *element;
	XMLGenericTree *current_element;
	XMLGenericTree *fake;

	int endfile,ris;
	int type;

	fake=new XMLGenericTree();

	reader=(xmlTextReaderPtr)readerpointer;
	
	endfile=1;
	current_element=fake;
	element=fake;

	while(endfile==1)
	{
		endfile = xmlTextReaderRead(reader);
		if(validating)
		{
			ris = xmlTextReaderIsValid(reader);
			if(ris==0)
				return 0;
		}
		if(endfile!=1)
			if(fake->child.empty())
				return 0;
			else
				return (*fake->child.begin());
		else
		{
			std::string s;
			xmlChar *name=xmlTextReaderName(reader);
			s=(char*)name;
			xmlFree(name);

			int short_element=xmlTextReaderIsEmptyElement(reader);

			type=xmlTextReaderNodeType(reader);

			if(type==1)
			{
				element=new XMLGenericTree();

				element->name=s;

				current_element->addChild(element);

				current_element=element;
				if(xmlTextReaderHasAttributes(reader))
				{
					//int count=xmlTextReaderAttributeCount(reader);
					//int i=0;
					while(xmlTextReaderMoveToNextAttribute(reader)==1)
					{
						//xmlTextReaderMoveToNextAttribute(reader);
						current_element->addAttribute((char*)xmlTextReaderName(reader),(char*)xmlTextReaderValue(reader));
						//intf("%s",xmlTextReaderName(reader));
						//i++;
					}
				}
			}

			if(type==3)
			{
				element=new XMLGenericTree();
				element->name="text";
				current_element->addChild(element);
				xmlChar *value=xmlTextReaderReadString(reader);
				std::string s,t;
				s=(char*)value;
				xmlFree(value);
				for(int i=0;i<s.length();i++)
				{
					if((s[i]=='\n')||(s[i]=='\t'))
						;
					else
					if((s[i]=='Ã')&&(s[i+1]=='¨'))
					{
						t=t+"è";
						i++;
					}
					else
					if((s[i]=='Ã')&&(s[i+1]=='©'))
					{
						t=t+"é";
						i++;
					}
					else
					if((s[i]=='Ã')&&(s[i+1]=='ª'))
					{
						t=t+"ê";
						i++;
					}
					else
					if((s[i]=='Ã')&&(s[i+1]=='¹'))
					{
						t=t+"ù";
						i++;
					}
					else
					if((s[i]=='Ã')&&(s[i+1]=='§'))
					{
						t=t+"ç";
						i++;
					}
					else
					if((s[i]=='Ã')&&(s[i+1]==-96))
					{
						t=t+"à";
						i++;
					}
					else
						t=t+s[i];
				}
				element->value=t;
			}
			
			if(type==15)
			{
				current_element=current_element->parent;
			}

			if(short_element==1)
				current_element=current_element->parent;
		}
	}
	return 0;
}


