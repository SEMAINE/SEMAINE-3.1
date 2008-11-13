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

// C_FuzzySimilarity.cpp: implementation of the  C_FuzzySimilarity class.
//
//////////////////////////////////////////////////////////////////////

#include "C_FuzzySimilarity.h"
#include "..\utils\C_DataContainer.h"
#include "..\utils\CEmGlobals.h"
#include "C_FuzzySet.h"
#include "DataContainer.h"

#include <math.h>
extern C_DataContainer local_container;
extern DataContainer *datacontainer;
        
    
C_FuzzySimilarity::~C_FuzzySimilarity()
{
}


C_FuzzySimilarity::C_FuzzySimilarity()
{
        //expressions = local_container.getexpressions();
		facedictionary=datacontainer->getFaceExpressionDictionary();
        limits = local_container.getfs_limits();
    }
    
    
double C_FuzzySimilarity::count_similarity(int *first, int *second) 
{        
        //if (first.length!=second.length) return 0.0;
        
        double wynik [CEM_MAX_FAP];
        
        for (unsigned int i=0;i<CEM_MAX_FAP;i++)
		{                        
            //C_FuzzySet aSet (first[i],limits[i][0]-limits[i][1]);
			
            C_FuzzySet aSet (first[i], limits[0][i]-limits[1][i]);
			wynik[i] = aSet.include(second[i]);
		
        }//end of for
        
        double result=0;
        int div=0;
        int neutral_ind=0;
        
		int neutral=1;
		for (int i=3; i<CEM_MAX_FAP;i++)
		    if ((first[i]!=0) || (second[i]!=0)) neutral=0;
		if (neutral==1) return 1.0;

        for (int i=3; i<CEM_MAX_FAP;i++)
		{
			//skip never used faps
			if ((i==23)||(i==24)||(i==25)||(i==26)||(i==48)||(i==49)||(i==50)) continue;
			if ((i==14)||(i==15)||(i==16)||(i==27)||(i==28)||(i==29)||(i==30)) continue;
            if ((i==39)||(i==40)||(i==43)||(i==44)||(i==45)||(i==46)||(i==47)) continue;
            if ((i==63)||(i==64)||(i==65)||(i==66)||(i==67)||(i==68)) continue;
            if ((first[i]!=0) || (second[i]!=0)) 
			{                
                result += wynik[i];
				//printf (" f %i v %f ",i, wynik[i]);
                div++;
            } 
			else 
				neutral_ind++;
            
        }//end of for        
        
		if (neutral_ind>(CEM_MAX_FAP-32)) return 1;
        return (double) (result / (double) div );
    }
                
    
    
    

double C_FuzzySimilarity::findSimilarity(std::string complex_label, std::string basic_label)
  {
        
		//Expression complex;
        //Expression basic;
        
        //int k=0;
        //double w1 [] = new double [6];
        //double w2 [] = new double [6];
       
        double ww1=0;
                       
            
		/*
			std::vector<Expression>::iterator iter;

			for(iter=(*expressions).begin();iter!=(*expressions).end();iter++)            
            //for (int i=0;i<expressions.size();i++)
			{		        
                Expression temp = (Expression) (*iter);
                
                if (temp.getName().compare(complex_label)==0) 
				{
                    complex=temp;
                }
                
                if (temp.getName().compare(basic_label)==0) 
				{
                    basic=temp;                    
                }
            }//end for
            
            //mamy wiec tabele z warotsciami dla szesciu emocji di base
		*/      
			
			int **complex = (facedictionary->GetExpression("affect", complex_label))->GetRawFaps( 0.0 );
			int **basic = (facedictionary->GetExpression("affect", basic_label))->GetRawFaps( 0.0 );
			
            int first  [CEM_MAX_FAP];
            int second  [CEM_MAX_FAP];
            
            for (int j=0;j<CEM_MAX_FAP;j++)
			{
                //first[j]=basic.getFapValue(j);                
				first[j]=basic[1][j];                
                
				//second[j]=complex.getFapValue(j);
                //second[j]=complex.getFapValue(j);
				second[j]=complex[1][j];
            }                
                                                
            ww1= roundtotwo(count_similarity(first, second));
                                          
        return ww1;
    }
    
    
  double C_FuzzySimilarity::roundtotwo(double value)
  {
        value=value*10000;		
        value=floor(value);
        value=(double) (value / (double)10000);
        return value;
    }
    