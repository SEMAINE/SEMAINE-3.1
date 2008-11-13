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

// C_SchemaSection.cpp: implementation of the  C_SchemaSection class.
//
//////////////////////////////////////////////////////////////////////

#include "C_SchemaSection.h"


/*
 *
 * Created on 18 styczeñ 2006, 20:42
 *
 *
 * uwaga kazda sekcja sklasa sie z n obszarow
 * obszary sa identyfikowalne przez numery nadane obszarom od 0 do MAX_AREAS
 *
 * sekcji jest m, gdzie m>=n
 *
 * natomiast identyfikatore sekcji sa od 1 do m
 * i musza odpowiadac obiektom w modelu fuzzy
 *
 */

       
C_SchemaSection::~C_SchemaSection()
{
}   
        
    
C_SchemaSection::C_SchemaSection(std::string text)
{
	//sections->SetSize(0);
    length=0;    
    id=-1;
    name=text;
    }
    
    
C_SchemaSection::C_SchemaSection(std::string text, int id1)
{
		//sections->SetSize(0);
		length=0;    
        id=id1;
        name=text;
    }
    

C_SchemaSection::C_SchemaSection()
{
		//sections->SetSize(0);
		length=0;    
        id=-1;
		name="";
    }


int C_SchemaSection::getId()
{
	return id;
}

	
void C_SchemaSection::addSection(int section)
{        

	if (length>0)
		{
			
			int *tymczas;

			//tymczas->SetSize(length+1);
         	tymczas=(int*)malloc(sizeof(int)*length+1);

            for (unsigned int i=0;i<length;i++) 
                 tymczas[i]=sections[i];
            
            tymczas[length]=section;
            sections=tymczas;
        }
        
        else 
		{
            //sections->SetSize(length+1);
			sections=(int*)malloc(sizeof(int)*length+1);
            sections[length]=section;
        }
        length++;        
    }
    

int *C_SchemaSection::getSections() 
{
	return sections;
}
    
    
int *C_SchemaSection::getDivision(int max1) 
{
        
        //if (sections->GetSize()==0) return sections;
        
		int *standard=(int*)malloc(sizeof(int)*max1);
		
        for (unsigned int i=0;i<max1;i++)
		{
            standard[i]=-1;
        }

  //for (unsigned int i=0;i<length;i++) printf("section %i, %i", i, sections[i]);
        

        for (unsigned int i=0;i<length;i++)
		{
            standard[sections[i]]=1;
        }
        
        return standard;
    }



    std::string C_SchemaSection::getLabel()
	{
		return name;
	}


	int C_SchemaSection::getLength()
	{
		return length;
	}
    
    


