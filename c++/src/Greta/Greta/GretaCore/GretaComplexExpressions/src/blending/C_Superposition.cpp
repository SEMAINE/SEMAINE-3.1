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

// C_Superpostion.cpp: implementation of the  C_Superpostion class.
//
//////////////////////////////////////////////////////////////////////

#include "C_Superposition.h"
#include "..\utils\CemGlobals.h"
#include ".\..\utils\C_DataContainer.h"
#include ".\..\math\C_FuzzySimilarity.h"

#include "DataContainer.h"

#include <math.h>

extern DataContainer *datacontainer;
extern C_DataContainer local_container;

		  
bool C_Superposition::addEmotion(Emotion *e)
{
	if (emotion1==NULL) 
	{
		emotion1=e;

		//find expression in facedictionary
		FaceExpression *temp=facedictionary->GetExpression("affect", e->getName() );	
		expression1=temp->GetRawFaps( 0.0 );

		return true;
	}
	else 
	{
		if (emotion2==NULL) 
		{
			emotion2=e;

			//find expression in facedictionary
			FaceExpression *temp=facedictionary->GetExpression("affect", e->getName() );					
			expression2=temp->GetRawFaps( 0.0 );			
		}
	}

	return false;
}
    

	  std::string C_Superposition::getType()
	  {
          return type;
      }
      

	  void C_Superposition::setType(std::string type)
	  {
          this->type=type;
      }
      
                
    C_Superposition::C_Superposition()
	{

		emotion1=NULL;
		emotion2=NULL;	
    
		expression1=NULL;
		expression2=NULL;
    
		std::string type="";

        facedictionary=datacontainer->getFaceExpressionDictionary();

        facesareas=local_container.getfacialareas();

		//emotions=local_container.getexpressions();                              
        superpositionschemas1 = local_container.getsectionschemas();        

		final_expression=(int**)malloc(sizeof(int*)*2);		
	
		for(unsigned int i=0;i<2;i++)
		{
			final_expression[i]=(int*)malloc(sizeof(int)*69);
			for(int j=0;j<69;j++)
				final_expression[i][j]=0;
		}

    }
    
	C_Superposition::~C_Superposition()
	{
		//To DO: free memory
	}
	
    int *C_Superposition::getDivision()
	{
		return division;
	}
    
    int C_Superposition::find(double **similarities, double deltaintensity)
	{
        
        int first=-1;
        int second=-1;
        double max = 0.0;
        
        for (unsigned int i=0;i < 6; i++)  
		{
            for (unsigned int j=0;j < 6; j++) 
			{
                if ((i!=j)&&(similarities[i][0]*similarities[j][1]>max))
				{
                    max=similarities[i][0]*similarities[j][1];
                    first=i;
                    second=j;
                }
            }
        }

        for (unsigned int i=0;i < 6; i++)   
		{
            for (unsigned int j=0;j < 6; j++)  
			{
                if ((i!=j)&&(similarities[i][1]*similarities[j][0]>max))
				{
                    max=similarities[i][1]*similarities[j][0];
                    first=j;
                    second=i;
                }
            }
        }
        
        
        //anger1, double disgust1, double joy1, double fear1, double sadness1, double surprise1,
        //double anger2, double disgust2, double joy2, double fear2, double sadness2, double surprise2,
        
            /*
                s1 S2 1/7,
                s3 S4 4-4
                s5 S6 3-5
                s7 S8 6-2
                s9 S10 4-1-1-2
             */
        
        
        int result = -1;
                         
        
        if ( (first==5) && (second==3) && ( fabs(deltaintensity)<0.3) ) 
		{
			result=1;
		}
        
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//        temp.addAntecedent(new FuzzyValue(DeltaIntensity,"zero"));
//
//        temp.addConclusion(new FuzzyValue(S1,"yes"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==5)&&(second==3)&&(deltaintensity>0.3)) 
		{
			result=3;
		}
        
        //RULE nr 2
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//        temp.addAntecedent(new FuzzyValue(DeltaIntensity,"positive"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==5)&&(second==3)&&(deltaintensity<-0.3)) 
		{
			result=4;
		}
        
        //RULE nr 3
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//        temp.addAntecedent(new FuzzyValue(DeltaIntensity,"negative"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==5)&&(second==1)) 
		{
			result=5;
		}
        
        //RULE nr 4
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Disgust2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"yes"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==3)&&(second==1)) 
		{
			result=5;
		}
        
        //RULE nr 5
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Disgust2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"yes"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        
        //................................................
        
        if ((first==0)&&(second==1)) 
		{
			result=3;
		}
        
        //RULE nr 6
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Anger1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Disgust2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==0)&&(second==5)) 
		{
			result=3;
		}
        
        //RULE nr 7
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Anger1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==0)&&(second==3)) 
		{
			result=3;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Anger1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==5)&&(second==2)) 
		{
			result=7;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Joy2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"yes"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==0)&&(second==2)) 
		{
			result=3;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Anger1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Joy2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==3)&&(second==2)) 
		{
			result=5;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Joy2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"yes"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        
        //................................................
        
        if ((first==4)&&(second==3)) 
		{
			result=3;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Sadness1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==0)&&(second==4)) 
		{
			result=3; 
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Anger1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Sadness2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==4)&&(second==1)) 
		{
			result=5;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Sadness1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Disgust2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"yes"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==4)&&(second==2)) 
		{
			result=5; 
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Sadness1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Joy2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"yes"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==4)&&(second==5)) 
		{
			result=1; 
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Sadness1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"yes"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        //RULE nr 17
        if ((first==2)&&(second==1)) 
		{
			result=9;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Joy1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Disgust2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"yes"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        
        //................................................
        //RULE nr 21
        if ((first==3)&&(second==5)&&(fabs(deltaintensity)<0.3)) 
		{
			result=2;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//        temp.addAntecedent(new FuzzyValue(DeltaIntensity,"zero"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"yes"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        //RULE nr 22
        if ((first==3)&&(second==5)&&(deltaintensity<-0.3)) 
		{
			result=4;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//        temp.addAntecedent(new FuzzyValue(DeltaIntensity,"negative"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        //RULE nr 23
        if ((first==3)&&(second==5)&&(deltaintensity>0.3)) 
		{
			result=3;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//        temp.addAntecedent(new FuzzyValue(DeltaIntensity,"positive"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"yes"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==1)&&(second==5)) 
		{
			result=6;
		}
        
        //RULE nr 24
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Disgust1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"yes"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==1)&&(second==3)) 
		{
			result=6;
		}
        
        //RULE nr 25
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Disgust1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"yes"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==1)&&(second==0)) 
		{
			result=4;
		}
        
        //RULE nr 26
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Disgust1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Anger2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==5)&&(second==0)) 
		{
			result=4;
		}
        
        //RULE nr 27
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Anger2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==3)&&(second==0)) 
		{
			result=4;
		}
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Anger2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==2)&&(second==5)) 
		{
			result=8;
		}
        
        //RULE nr 29
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Joy1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Surprise2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"yes"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
//
//        //................................................
//
        if ((first==2)&&(second==0)) 
		{
			result=4;
		}
//
//        //RULE nr 30
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Joy1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Anger2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
//
//        //................................................
//
        if ((first==2)&&(second==3)) 
		{
			result=6;
		}
//
//        //RULE nr 31
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Joy1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Fear2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"yes"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==3)&&(second==4)) 
		{
			result=4;
		}
        
        //RULE nr 32
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Fear1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Sadness2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==4)&&(second==0)) 
		{
			result=4;
		}
        
        //RULE nr 33
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Sadness1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Anger2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"yes"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==1)&&(second==4)) 
		{
			result=6;
		}
        
        //RULE nr 34
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Disgust1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Sadness2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"yes"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==2)&&(second==4)) 
		{
			result=6;
		}
        //RULE nr 35
        
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Joy1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Sadness2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"no"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"yes"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==5)&&(second==4)) 
		{
			result=2;
		}
        //RULE nr 36
//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
//        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
//
//        temp.addAntecedent(new FuzzyValue(Surprise1,"similar"));
//        temp.addAntecedent(new FuzzyValue(Sadness2,"similar"));
//
//        temp.addConclusion(new FuzzyValue(S1,"no"));
//        temp.addConclusion(new FuzzyValue(S2,"yes"));
//        temp.addConclusion(new FuzzyValue(S3,"no"));
//        temp.addConclusion(new FuzzyValue(S4,"no"));
//        temp.addConclusion(new FuzzyValue(S5,"no"));
//        temp.addConclusion(new FuzzyValue(S6,"no"));
//        temp.addConclusion(new FuzzyValue(S7,"no"));
//        temp.addConclusion(new FuzzyValue(S8,"no"));
//        temp.addConclusion(new FuzzyValue(S9,"no"));
//        temp.addConclusion(new FuzzyValue(S10,"no"));
        
        //................................................
        
        if ((first==1)&&(second==2)) 
		{
			result=10;
		}
        
        //RULE nr 37
        /*
        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
        temp.setAntecedentCombineOperator(new ProductAntecedentCombineOperator());
         
        temp.addAntecedent(new FuzzyValue(Disgust1,"similar"));
        temp.addAntecedent(new FuzzyValue(Joy2,"similar"));
         
        temp.addConclusion(new FuzzyValue(S1,"no"));
        temp.addConclusion(new FuzzyValue(S2,"no"));
        temp.addConclusion(new FuzzyValue(S3,"no"));
        temp.addConclusion(new FuzzyValue(S4,"no"));
        temp.addConclusion(new FuzzyValue(S5,"no"));
        temp.addConclusion(new FuzzyValue(S6,"no"));
        temp.addConclusion(new FuzzyValue(S7,"no"));
        temp.addConclusion(new FuzzyValue(S8,"no"));
        temp.addConclusion(new FuzzyValue(S9,"no"));
        temp.addConclusion(new FuzzyValue(S10,"yes"));
         */        
        return result;
    }
    
    
	bool C_Superposition::evaluate(int maxareas) 
	{
        
        
        C_FuzzySimilarity *fs1 = new C_FuzzySimilarity();
        
		
        //The order of emotions: anger, disgust, joy, fear, sadness, surprise
        
		const int emo_length=6;

        double emo1_sim[emo_length];
        double emo2_sim[emo_length];
        
        emo1_sim[0]=fs1->findSimilarity(emotion1->getName(), "anger");
        emo1_sim[1]=fs1->findSimilarity(emotion1->getName(), "disgust");
        emo1_sim[2]=fs1->findSimilarity(emotion1->getName(), "joy");
        emo1_sim[3]=fs1->findSimilarity(emotion1->getName(), "fear");
        emo1_sim[4]=fs1->findSimilarity(emotion1->getName(), "sadness");
        emo1_sim[5]=fs1->findSimilarity(emotion1->getName(), "surprise");
        
        emo2_sim[0]=fs1->findSimilarity(emotion2->getName(), "anger");
        emo2_sim[1]=fs1->findSimilarity(emotion2->getName(), "disgust");
        emo2_sim[2]=fs1->findSimilarity(emotion2->getName(), "joy");
        emo2_sim[3]=fs1->findSimilarity(emotion2->getName(), "fear");
        emo2_sim[4]=fs1->findSimilarity(emotion2->getName(), "sadness");
        emo2_sim[5]=fs1->findSimilarity(emotion2->getName(), "surprise");
                
        //if (emo1_sim.length!=emo2_sim.length) return false;
                
		double **similarities;

		similarities=(double**)malloc(sizeof(double*)*emo_length);
			for(unsigned int i=0;i<emo_length;i++)
			{
				similarities[i]=(double*)malloc(sizeof(double)*2);
				for(unsigned int j=0;j<2;j++)
				similarities[i][j]=0.0;
			}

		for (unsigned int i=0;i<emo_length;i++)
		{
            similarities[i][0]=emo1_sim[i];
            similarities[i][1]=emo2_sim[i];
        }

	    int result = find (similarities, emotion1->getIntensity() - emotion2->getIntensity() );
                
        //find a section
		C_SchemaSection schema_result;
        C_SchemaSection temp;
        
		std::vector<C_SchemaSection>::iterator iter;

		for(iter=superpositionschemas1->begin();iter!=superpositionschemas1->end();iter++)     
		{
            temp = (C_SchemaSection) (*iter);
            if (temp.getId()==result) schema_result = temp;
        }//end of for
                
        division = schema_result.getDivision(maxareas);                                
        
        return true;
    }
     

int **C_Superposition::getExpression()
	{

		for (unsigned int i=1;i<CEM_MAX_FAP;i++)
		{

			
			for (unsigned int k=0; k<RESULT_LENGTH ; k++)
			{

				//printf("facearea k %i i %i is %i \n", k, i, facesareas[k][i] );
				
				if (facesareas[k][i]>0)
				{

					if (division[k]<=0) 
					{
						//LOWER FACE

						if ( /*(expression2[0][i]==1) && */ ( expression2[1][i]!=0) )
						{
							final_expression[0][i]=1;
							
							final_expression[1][i] = expression2[1][i] /** (int) facesareas[k][i] */;                                              
							
							k=RESULT_LENGTH;                     
						}

					} 
					else 
					{                    
						//UPPER FACE

						if ( /*(expression1[0][i]==1) &&*/ ( expression1[1][i]!=0) )
						{
							final_expression[0][i]=1;
							
							final_expression[1][i]=expression1[1][i] /**facesareas[k][i] */;                                                
							
							k=RESULT_LENGTH;                     
						}
					}
				}
			}//end of for k			

		}

		return final_expression;
	}
	   

