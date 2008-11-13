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

//  C_Masking.cpp: implementation of the C_Masking class.
//
//////////////////////////////////////////////////////////////////////

/*
* Three intervals of values:
* -1..-resulline      ---> FELT
* -resultline .. resultline   ---> nothing
* resultline..1       ---> Fake
*
*simialrities, results and division - dimension=8
*/

#include "C_Masking.h"
#include ".\..\math\C_FuzzySimilarity.h"
#include "..\utils\C_DataContainer.h"
#include "DataContainer.h"
#include "RandomGen.h"
#include "FaceExpression.h"

#include "..\utils\CemGlobals.h"

extern DataContainer *datacontainer;
extern C_DataContainer local_container;
extern RandomGen *randomgen;


bool C_Masking::addEmotion(Emotion *e)
{
	if (emotion1==NULL) 
	{
		emotion1=e;

		//find expression in facedictionary
		FaceExpression *temp=facedictionary->GetExpression("affect", e->getName() );

		if (e->getType().compare("fake")==0)
			fake_expression=temp->GetRawFaps( 0.0 );
		else 
			felt_expression=temp->GetRawFaps( 0.0 );

		return true;
	}
	else 
	{
		if (emotion2==NULL) 
		{
			emotion2=e;

			//find expression in facedictionary
			FaceExpression *temp=facedictionary->GetExpression("affect", e->getName() );

			if (e->getType().compare("fake")==0)
				fake_expression=temp->GetRawFaps( 0.0 );
			else 
				felt_expression=temp->GetRawFaps( 0.0 );
			return true;
		}
	}

	return false;
}



std::string C_Masking::getType()
{
	return type;
}

void C_Masking::setType(std::string type)
{
	this->type=type;
}



C_Masking::C_Masking() 
{                        

	emotion1=NULL;
	emotion2=NULL;	

	fake_expression=NULL;
	felt_expression=NULL;

	std::string type="";

	final_expression=(int**)malloc(sizeof(int*)*2);		

	for(unsigned int i=0;i<2;i++)
	{
		final_expression[i]=(int*)malloc(sizeof(int)*69);
		for(int j=0;j<69;j++)
			final_expression[i][j]=0;
	}

	facedictionary=datacontainer->getFaceExpressionDictionary();

	facesareas=local_container.getmasking_facialareas();

	//expressions=local_container.getexpressions();

	//result=NULL;
	//division=NULL;        
}



C_Masking::~C_Masking() 
{
	
	//TO DO: free memory
	
	/*
	for(unsigned int i=0;i<2;i++)	
		free(final_expression[i]);

	free(final_expression);
	*/
}


/**
* It returns a result of facial masking in a form of an array where:
* 
* -1 FELT 
* 0  NEUTRAL
* 1  FAKE
*2 RANDOM
*
* @return array of face division for masking expression
* 
*/
int *C_Masking::getDivision()
{

	// FELT -1
	// FAKE 1
	// NEUTRAL 0
	// RANDOM 2

	//division = new int [RESULT_LENGTH];

	for (unsigned int i=0;i<RESULT_LENGTH;i++)
	{

		//RANDOM 2
		if (result[i]>1.0)
		{
			float result=randomgen->GetRand01();
			if (result>0.5) 	division[i]=1;
			else division[i]=-1;
	
		}

		// FAKE 1
		if (result[i]>0.0) division[i]=1;

		// FELT -1
		if (result[i]<0.0) division[i]=-1;

		// NEUTRAL 0
		if (result[i]==0.0) division[i]=0;
	}

	return division;
}


/**
* main method of the algorithm, it finds deception clues
* @param maxareas number of facial areas
* @return  true if OK
*/

bool C_Masking::evaluate(int maxareas) 
{

	C_FuzzySimilarity *fs1 = new C_FuzzySimilarity();

	double anger1=fs1->findSimilarity(emotion1->getName(), "anger");
	double disgust1=fs1->findSimilarity(emotion1->getName(), "disgust");
	double sadness1=fs1->findSimilarity(emotion1->getName(), "sadness");
	double surprise1=fs1->findSimilarity(emotion1->getName(), "surprise");
	double fear1=fs1->findSimilarity(emotion1->getName(), "fear");
	double joy1=fs1->findSimilarity(emotion1->getName(), "joy");
	double neutral1=fs1->findSimilarity(emotion1->getName(), "neutral");

	double anger2=fs1->findSimilarity(emotion2->getName(), "anger");
	double disgust2=fs1->findSimilarity(emotion2->getName(), "disgust");
	double sadness2=fs1->findSimilarity(emotion2->getName(), "sadness");
	double surprise2=fs1->findSimilarity(emotion2->getName(), "surprise");
	double fear2=fs1->findSimilarity(emotion2->getName(), "fear");
	double joy2=fs1->findSimilarity(emotion2->getName(), "joy");
	double neutral2=fs1->findSimilarity(emotion2->getName(), "neutral");

	double max1=MAX(anger1,disgust1);
	max1=MAX(max1,sadness1);
	max1=MAX(max1,fear1);
	max1=MAX(max1,joy1);
	max1=MAX(max1,neutral1);
	//no surprise? utnij ta linie
	max1=MAX(max1,surprise1);

	double max2=MAX(anger2,disgust2);
	max2=MAX(max2,sadness2);
	max2=MAX(max2,fear2);
	max2=MAX(max2,joy2);
	max2=MAX(max2,neutral2);
	//no surprise? utnij ta linie
	max2=MAX(max2,surprise2);

	//instead of using fuzzy stuff...
	if (anger1==max1) 
	{
		surprise1=0;
		disgust1=0;
		sadness1=0;
		fear1=0;
		joy1=0;
		neutral1=0.0;
	}
	if (disgust1==max1) 
	{
		surprise1=0;
		anger1=0;
		sadness1=0;
		fear1=0;
		joy1=0;
		neutral1=0.0;
	}
	if (sadness1==max1) 
	{
		surprise1=0;
		disgust1=0;
		anger1=0;
		fear1=0;
		joy1=0;
		neutral1=0.0;
	}
	if (fear1==max1) 
	{
		surprise1=0;
		disgust1=0;
		sadness1=0;
		anger1=0;
		joy1=0;
		neutral1=0.0;
	}
	if (joy1==max1) 
	{
		surprise1=0;
		disgust1=0;
		sadness1=0;
		anger1=0;
		fear1=0;
		neutral1=0.0;
	}
	if (neutral1==max1) 
	{
		surprise1=0;
		disgust1=0;
		sadness1=0;
		anger1=0;
		joy1=0;
		fear1=0.0;
	}
	//no surprise? utnij ta linie
	if (surprise1==max1) 
	{
		neutral1=0;
		disgust1=0;
		sadness1=0;
		anger1=0;
		joy1=0;
		fear1=0.0;
	}

	if (anger2==max2) 
	{
		surprise2=0;
		disgust2=0;
		sadness2=0;
		fear2=0;
		joy2=0;
		neutral2=0.0;
	}

	if (disgust2==max2) 
	{
		surprise2=0;
		anger2=0;
		sadness2=0;
		fear2=0;
		joy2=0;
		neutral2=0.0;
	}

	if (sadness2==max2) 
	{
		surprise2=0;
		disgust2=0;
		anger2=0;
		fear2=0;
		joy2=0;
		neutral2=0.0;
	}
	if (fear2==max2) 
	{
		surprise2=0;
		disgust2=0;
		sadness2=0;
		anger2=0;
		joy2=0;
		neutral2=0.0;
	}
	if (joy2==max2) 
	{
		surprise2=0;
		disgust2=0;
		sadness2=0;
		anger2=0;
		fear2=0;
		neutral2=0.0;
	}
	if (neutral2==max2) 
	{
		surprise2=0;
		disgust2=0;
		sadness2=0;
		anger2=0;
		joy2=0;
		fear2=0.0;
	}
	//no surprise? utnij ta linie
	if (surprise2==max2) 
	{
		neutral2=0;
		disgust2=0;
		sadness2=0;
		anger2=0;
		joy2=0;
		fear2=0.0;
	}
	
	double fake_similarities [7];
	double felt_similarities  [7];

	//first felt, second fake
	if ((emotion2->getType().compare("fake")==0)&&(emotion1->getType().compare("felt")==0)) 
	{

		fake_similarities[0]=anger2;
		fake_similarities[1]=disgust2;
		fake_similarities[2]=joy2;
		fake_similarities[3]=fear2;
		fake_similarities[4]=sadness2;
		fake_similarities[5]=neutral2;
		fake_similarities[6]=surprise2;

		felt_similarities[0]=anger1;
		felt_similarities[1]=disgust1;
		felt_similarities[2]=joy1;
		felt_similarities[3]=fear1;
		felt_similarities[4]=sadness1;
		felt_similarities[5]=neutral1;
		felt_similarities[6]=surprise1;

	}

	//first fake, second felt
	if ( (emotion1->getType().compare("fake")==0)&&(emotion2->getType().compare("felt")==0) )
	{

		fake_similarities[0]=anger1;
		fake_similarities[1]=disgust1;
		fake_similarities[2]=joy1;
		fake_similarities[3]=fear1;
		fake_similarities[4]=sadness1;
		fake_similarities[5]=neutral1;
		fake_similarities[6]=surprise1;

		felt_similarities[0]=anger2;
		felt_similarities[1]=disgust2;
		felt_similarities[2]=joy2;
		felt_similarities[3]=fear2;
		felt_similarities[4]=sadness2;
		felt_similarities[5]=neutral2;
		felt_similarities[6]=surprise2;

	}


	//two vector of dimension 8 *the number of facial areas) are returned with the values in the interval -1 1 
	double *fakeresult = findfake(fake_similarities); 
	double *feltresult = findfelt(felt_similarities); 

	//result = new double [feltresult.length];
	//result = new double [RESULT_LENGTH];

	for (int i=0;i<RESULT_LENGTH;i++)
	{            
		result[i] = join(feltresult[i], fakeresult[i]);
	}

	return true;
}

double C_Masking::join(double felt, double fake) 
{

	if ((felt>0.2) && (fake>0.2)) return -1.0;
	if ((felt<-0.2) && (fake<-0.2)) return 0.0;
	if ((felt>0.2) && (fake<-0.2)) return -1.0;
	if ((felt<-0.2) && (fake>0.2)) return 1.0;

	if ((felt<0.2) && (felt>-0.2) && (fake>0.2)) return 1.0;
	if ((felt<0.2) && (felt>-0.2) && (fake<-0.2)) return -1.0;
	if ((felt>0.2) && (fake>-0.2) && (fake<0.2)) return -1.0;
	if ((felt<-0.2) && (fake>-0.2) && (fake<0.2)) return 1.0;

	if ((felt<0.2) && (felt>-0.2) && (fake>-0.2) && (fake<0.2)) return 2.0;
	return -2.0 ;
}

double C_Masking::count(double newone, double oldone)
{

	if ((newone==oldone) && (oldone==0.0)) return 0.0;

	if ( (oldone!=0.0) && (newone==0.0)) return oldone;
	if ((oldone==0.0) && (newone!=0.0)) return newone;

	if ((oldone>0.0) && (newone>0.0)) return MAX(newone,oldone);
	if ((oldone<0.0) && (newone<0.0)) return MIN(newone,oldone);

	if ((oldone<0.0) && (newone>0.0)) 
	{

		double temp = MAX(fabs(oldone),newone);
		if (temp == fabs(oldone)) return oldone;
		else return newone;

	}
	if ((oldone>0.0) && (newone<0.0)) 
	{

		double temp = MAX(fabs(newone),oldone);
		if (temp == fabs(newone)) return newone;
		else return oldone;

	}


	return 0.0;
}

double *C_Masking::findfake(double *similarities) 
{

	//the order: 0 - anger, 1- disgust, 2- joy, 3- fear, 4-sadness, 5-neutral, 6-suprise

	double *results;
	results=(double*)malloc(sizeof(double)*RESULT_LENGTH);
	//double results [RESULT_LENGTH];

	for (unsigned int i=0;i<RESULT_LENGTH;i++)   
	{
		results[i]=0;
	}

	//RULE nr 1

	//fake_similarities[0]=anger2;

	results[0] = count(similarities [0]*1.0,results[0]);
	results[1] = count(similarities [0]*-1.0,results[1]);
	results[2] = count(similarities [0]*-1.0,results[2]);
	results[3] = count(similarities [0]*-1.0,results[3]);
	results[4] = count(similarities [0]*-1.0,results[4]);
	results[5] = count(similarities [0]*1.0,results[5]);
	results[6] = count(similarities [0]*1.0,results[6]);
	results[7] = count(similarities [0]*-1.0,results[7]);

	//        FuzzyRule temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Anger,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));//111
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));//111

	//................................................

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	//RULE nr 2

	//fake_similarities[4]=sadness2;

	results[0] = count(similarities [4]*-1.0,results[0]);
	results[1] = count(similarities [4]*-1.0,results[1]);
	results[2] = count(similarities [4]*1.0,results[2]);
	results[3] = count(similarities [4]*1.0,results[3]);
	results[4] = count(similarities [4]*1.0,results[4]);
	results[5] = count(similarities [4]*1.0,results[5]);
	results[6] = count(similarities [4]*1.0,results[6]);
	results[7] = count(similarities [4]*1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Sadness,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"shown"));

	//................................................

	//RULE nr 3

	//fake_similarities[1]=disgust2;

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [1]*-1.0,results[0]);
	results[1] = count(similarities [1]*1.0,results[1]);
	results[2] = count(similarities [1]*1.0,results[2]);
	results[3] = count(similarities [1]*1.0,results[3]);
	results[4] = count(similarities [1]*1.0,results[4]);
	results[5] = count(similarities [1]*1.0,results[5]);
	results[6] = count(similarities [1]*1.0,results[6]);
	results[7] = count(similarities [1]*1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Disgust,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"shown"));

	//................................................

	//RULE nr 4

	//fake_similarities[3]=fear2;

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [3]*-1.0,results[0]);
	results[1] = count(similarities [3]*1.0,results[1]);
	results[2] = count(similarities [3]*1.0,results[2]);
	results[3] = count(similarities [3]*1.0,results[3]);
	results[4] = count(similarities [3]*1.0,results[4]);
	results[5] = count(similarities [3]*1.0,results[5]);
	results[6] = count(similarities [3]*1.0,results[6]);
	results[7] = count(similarities [3]*1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Fear,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"shown"));

	//................................................

	//RULE nr 5

	//fake_similarities[2]=joy2;

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [2]*-1.0,results[0]);
	results[1] = count(similarities [2]*-1.0,results[1]);
	results[2] = count(similarities [2]*1.0,results[2]);
	results[3] = count(similarities [2]*-1.0,results[3]);
	results[4] = count(similarities [2]*1.0,results[4]);
	results[5] = count(similarities [2]*1.0,results[5]);
	results[6] = count(similarities [2]*1.0,results[6]);
	results[7] = count(similarities [2]*1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Joy,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"shown"));

	//................................................

	//RULE nr 6

	//fake_similarities[5]=neutral2;

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [5]*-1.0,results[0]);
	results[1] = count(similarities [5]*-1.0,results[1]);
	results[2] = count(similarities [5]*-1.0,results[2]);
	results[3] = count(similarities [5]*-1.0,results[3]);
	results[4] = count(similarities [5]*-1.0,results[4]);
	results[5] = count(similarities [5]*-1.0,results[5]);
	results[6] = count(similarities [5]*-1.0,results[6]);
	results[7] = count(similarities [5]*-1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Neutral,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	//................................................

	//RULE nr 6

	//fake_similarities[6]=surprise2;

	results[0] = count(similarities [6]*1.0,results[0]);
	results[1] = count(similarities [6]*-1.0,results[1]);
	results[2] = count(similarities [6]*-1.0,results[2]);
	results[3] = count(similarities [6]*-1.0,results[3]);
	results[4] = count(similarities [6]*1.0,results[4]);
	results[5] = count(similarities [6]*1.0,results[5]);
	results[6] = count(similarities [6]*1.0,results[6]);
	results[7] = count(similarities [6]*1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Surprise,"similar"));
	//        temp.addConclusion(new FuzzyValue(Brows,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"shown"));

	return results;

}

/**
* implement Ekman's hyphothesis 
* @return returns the array of values in the interval  -1, 1 , where
* -1 - not seen
* 0 - I dont know
* 1 - seen
*/

double *C_Masking::findfelt(double *similarities) 
{

	//double results [RESULT_LENGTH];
	double *results;
	results=(double*)malloc(sizeof(double)*RESULT_LENGTH);

	for (int i=0;i<RESULT_LENGTH;i++)   
	{
		results[i]=0;
	}

	//RULE nr 1

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [0]*-1.0,results[0]);
	results[1] = count(similarities [0]*1.0,results[1]);
	results[2] = count(similarities [0]*1.0,results[2]);
	results[3] = count(similarities [0]*1.0,results[3]);
	results[4] = count(similarities [0]*-1.0,results[4]);
	results[5] = count(similarities [0]*-1.0,results[5]);
	results[6] = count(similarities [0]*-1.0,results[6]);
	results[7] = count(similarities [0]*1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Anger,"similar"));
	//
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"shown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"shown"));//111
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"shown"));//111
	//
	//................................................

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [4]*1.0,results[0]);
	results[1] = count(similarities [4]*1.0,results[1]);
	results[2] = count(similarities [4]*-1.0,results[2]);
	results[3] = count(similarities [4]*-1.0,results[3]);
	results[4] = count(similarities [4]*-1.0,results[4]);
	results[5] = count(similarities [4]*-1.0,results[5]);
	results[6] = count(similarities [4]*-1.0,results[6]);
	results[7] = count(similarities [4]*-1.0,results[7]);

	//RULE nr 2

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Sadness,"similar"));
	//
	//        temp.addConclusion(new FuzzyValue(Brows,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	//................................................

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [1]*1.0,results[0]);
	results[1] = count(similarities [1]*-1.0,results[1]);
	results[2] = count(similarities [1]*-1.0,results[2]);
	results[3] = count(similarities [1]*-1.0,results[3]);
	results[4] = count(similarities [1]*1.0,results[4]);
	results[5] = count(similarities [1]*-1.0,results[5]);
	results[6] = count(similarities [1]*-1.0,results[6]);
	results[7] = count(similarities [1]*-1.0,results[7]);

	//RULE nr 3

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Disgust,"similar"));
	//
	//        temp.addConclusion(new FuzzyValue(Brows,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"shown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	//................................................

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [3]*1.0,results[0]);
	results[1] = count(similarities [3]*-1.0,results[1]);
	results[2] = count(similarities [3]*-1.0,results[2]);
	results[3] = count(similarities [3]*-1.0,results[3]);
	results[4] = count(similarities [3]*-1.0,results[4]);
	results[5] = count(similarities [3]*-1.0,results[5]);
	results[6] = count(similarities [3]*-1.0,results[6]);
	results[7] = count(similarities [3]*-1.0,results[7]);

	//RULE nr 4
	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Fear,"similar"));
	//
	//        temp.addConclusion(new FuzzyValue(Brows,"shown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	//................................................

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [2]*-1.0,results[0]);
	results[1] = count(similarities [2]*-1.0,results[1]);
	results[2] = count(similarities [2]*-1.0,results[2]);
	results[3] = count(similarities [2]*1.0,results[3]);
	results[4] = count(similarities [2]*-1.0,results[4]);
	results[5] = count(similarities [2]*-1.0,results[5]);
	results[6] = count(similarities [2]*-1.0,results[6]);
	results[7] = count(similarities [2]*-1.0,results[7]);

	//RULE nr 5
	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Joy,"similar"));
	//
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"shown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	//................................................

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [5]*-1.0,results[0]);
	results[1] = count(similarities [5]*-1.0,results[1]);
	results[2] = count(similarities [5]*-1.0,results[2]);
	results[3] = count(similarities [5]*-1.0,results[3]);
	results[4] = count(similarities [5]*-1.0,results[4]);
	results[5] = count(similarities [5]*-1.0,results[5]);
	results[6] = count(similarities [5]*-1.0,results[6]);
	results[7] = count(similarities [5]*-1.0,results[7]);

	//RULE nr 6
	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Neutral,"similar"));//
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	//................................................
	//RULE nr 6

	//0anger 1disgust 2joy 3fear 4sadness 5neutral 6suprise

	results[0] = count(similarities [6]*-1.0,results[0]);
	results[1] = count(similarities [6]*-1.0,results[1]);
	results[2] = count(similarities [6]*-1.0,results[2]);
	results[3] = count(similarities [6]*-1.0,results[3]);
	results[4] = count(similarities [6]*-1.0,results[4]);
	results[5] = count(similarities [6]*-1.0,results[5]);
	results[6] = count(similarities [6]*-1.0,results[6]);
	results[7] = count(similarities [6]*-1.0,results[7]);

	//        temp= new FuzzyRule(new MamdaniMinMaxMinRuleExecutor());
	//        temp.addAntecedent(new FuzzyValue(Surprise,"similar"));//
	//        temp.addConclusion(new FuzzyValue(Brows,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Eyes,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLids,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Nose,"notshown"));
	//        temp.addConclusion(new FuzzyValue(Cheeks,"notshown"));
	//        temp.addConclusion(new FuzzyValue(UpperLip,"notshown"));
	//        temp.addConclusion(new FuzzyValue(LowerLip,"notshown"));

	return results;

}


int **C_Masking::getExpression(float resultline)
{        
	
	for(unsigned int i=0;i<2;i++)
	{
		for(int j=0;j<69;j++)
			final_expression[i][j]=0;
	}

	for (unsigned int i=0;i<RESULT_LENGTH;i++)
	{

		//RANDOM case
		if (result[i]>1.0)
		{

			float result=randomgen->GetRand01();
			if (result>0.5) 	division[i]=1;
			else division[i]=-1;

		}

		// FAKE 1
		if (result[i]>0.0) division[i]=1;

		// FELT -1
		if (result[i]<0.0) division[i]=-1;

		// NEUTRAL 0
		if (result[i]==0.0) division[i]=0;
	}


	/*
	*uwaga! mamy trzy prwedialy wartosci:
	* -1..-resulline      ---> FELT
	* -resultline .. resultline   ---> nothing
	* resultline..1       ---> Fake
	*
	*/
	
	//double resultline=0.05;

	FAPFrame complex_exp;
	complex_exp.ActivateAndSetAllFAPsTo(0);

	double factor = 1.0;

	//first felt, second fake
	if 
		(
		(emotion1->getType().compare("felt")==0 )
		&&

		(emotion2->getType().compare("fake")==0 )
		)
	{				

		for (unsigned int k=0;k<RESULT_LENGTH;k++)   
		{

			for (unsigned int i=0;i<CEM_MAX_FAP;i++)   
			{


				if (facesareas[k][i]>0)
				{

					if (division[k]>resultline)
					{
						//second emotion                            
						//fake

						//raw_faps[0][i]=(*iter).FAPs[i].active;
						//raw_faps[1][i]=(*iter).GetFAP(i);

						//if ( /*(fake_expression[0][i]==1) &&*/ ( fake_expression[1][i]!=0) )
						{
							final_expression[0][i]=1;

							final_expression[1][i]=fake_expression[1][i] *facesareas[k][i];                                                

							//complex_exp.SetFAP(j, exp1, true)
							//faces[e2][i]*

						}//end if

					}// end if

					if (division[k]<( -1.0) * resultline)
					{
						//first emotion                        						
						//felt

						//if ( /*(felt_expression[0][i]==1) && */( felt_expression[1][i]!=0) )
						{
							final_expression[0][i]=1;

							final_expression[1][i]=felt_expression[1][i] /**facesareas[k][i] */;                                                

							//complex_exp.SetFAP(j, exp1, true)
							//faces[e2][i]*
						}// end if

					}// end if

					//WHAT IF DIVISION[0] * FACTOR == 0

				}//end for j

			}
		}//end for i

	} //end of if


	//opposite case
	if 
		(
		(emotion1->getType().compare("fake")==0)
		&&				
		(emotion2->getType().compare("felt")==0)
		)
	{				
		for (unsigned int k=0;k<RESULT_LENGTH;k++)   
		{

			for (unsigned int i=0;i<CEM_MAX_FAP;i++)   
			{

				if (facesareas[k][i]>0)
				{

					if (division[k]>resultline)
					{
						//second emotion                            
						//felt

						//raw_faps[0][i]=(*iter).FAPs[i].active;
						//raw_faps[1][i]=(*iter).GetFAP(i);

						//if ( /*(felt_expression[0][i]==1) && */ ( felt_expression[1][i]!=0) )
						{
							final_expression[0][i]=1;
							final_expression[1][i] = felt_expression[1][i] *facesareas[k][i];                                                

							//complex_exp.SetFAP(j, exp1, true)
							//faces[e2][i]*

						}//end if

					}// end if

					if (division[k]<-(1.0)*resultline)
					{
						//first emotion                        
						//fake
						
						//if ( /*(fake_expression[0][i]==1)  && */( fake_expression[1][i]!=0) )
						{
							final_expression[0][i]=1;
							final_expression[1][i] = fake_expression[1][i]*facesareas[k][i]  ;                                                

							//complex_exp.SetFAP(j, exp1, true)
							//faces[e2][i]*
						}// end if

					}// end if

					//WHAT IF DIVISION[0] * FACTOR == 0
				}

			}//end for j

		}//end for i

	} //end of if

	return final_expression;

}//end of getExpression
