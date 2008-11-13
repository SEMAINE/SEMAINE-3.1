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

#include "ApplyFAPs.h"
#include "init.h"
//#include "global.h"
#include "../AgentAspect.h"

ApplyFAPs::ApplyFAPs(void)
{
}

ApplyFAPs::~ApplyFAPs(void)
{
}

void ApplyFAPs::init(void *agent)
{
	this->agent=(AgentAspect*)agent;
	this->FAPs=(((AgentAspect*)this->agent)->FAPs);
	//this->lfaps=(((AgentAspect*)this->agent)->FAPs->lfaps);

	this->facedata=((AgentAspect*)this->agent)->facedata;
	this->FDPs=this->facedata->FDPs;
	this->FAPUs=this->facedata->FAPUs;
	this->skin_point=this->facedata->skin_point;
	this->surface_list1=this->facedata->surface_list1;

	this->misc_point=((AgentAspect*)this->agent)->misc_point;
	this->flag_surface1=((AgentAspect*)this->agent)->flag_surface1;
	this->flag_surface2=((AgentAspect*)this->agent)->flag_surface2;
	this->flag_ifapu = 1;
}
void ApplyFAPs::apply_faps()
{
	int i,j;

	for (i=0; i<this->facedata->FaceVerticesNumber; i++)
		for (j=0; j<3; j++)
			this->facedata->skin_point[i].increment[j] = 0.0;

	for(i=0; i < SKIN_VERTEX_NUMBER2; i++)
		for(j=0; j < 3; j++)
			this->misc_point[i].increment[j] = 0.0;

		//forehead
	this->apply_fap_31_32();
	this->apply_fap_33_34();
    this->apply_fap_35_36();
	this->apply_aux_fap_1();
		//mouth

		//corners
	this->apply_fap_53_54_59_60();
    this->apply_fap_6_7_12_13();

		//upper lip (factor 0.6)
    this->apply_fap_55_56();
    this->apply_fap_8_9();
    this->apply_fap_51_17();
    this->apply_fap_4_17();

		//lower lip (factor 0.5)
    this->apply_fap_57_58();
    this->apply_fap_10_11();
    this->apply_fap_52_16();
    this->apply_fap_5_16();
		

		//ELISABETTA
		//tongue
    this->apply_fap_43_44();
    this->apply_fap_45();
    this->apply_fap_46();
	 this->apply_fap_47();

		//jaw
    this->apply_fap_3_14_15();
    this->apply_fap_18();

		//cheecks		
		
		//lower
    this->apply_fap_39_40();
    this->apply_fap_41_42(); //con "alisha.fap" ci sono dei problemi
    this->cheek_deformation(); //per la pieghetta
    this->apply_aux_fap_2();

		//nose
    this->apply_fap_61_62();
    this->apply_fap_63_64();

		//ears
    this->apply_fap_65_66();
    this->apply_fap_67_68();

		//eyelid
    this->apply_fap_19_20();
    this->apply_fap_21_22();

		//pupils
    this->apply_fap_29_30();
}

float ApplyFAPs::COEF(float fapuModel, float ifapuModel, float fapuGreta, float ifapuGreta)
{
	if(flag_ifapu)
		return fapuModel*ifapuGreta/(fapuGreta*ifapuModel);
	else
		return 1.0;
}



void ApplyFAPs::apply_fap_31_32()
{	//FINISH !!!	
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, weight=1.0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float COEFZ=COEF(this->FAPUs->ENSFAPU, this->FAPUs->IFAPU_FI, 123.37, 147.37);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[4][2][0];
	ref_point1[1] = FDPfp[4][2][1];
	ref_point1[2] = FDPfp[4][2][2];

	ref_point2[0] = FDPfp[4][1][0];
	ref_point2[1] = FDPfp[4][1][1];
	ref_point2[2] = FDPfp[4][1][2];

	//FAP 31: interessa le superfici 11(L_SUP_EYELID_B), 13(L_EYEBROW_A),15(L_EYEBROW_B),19(L_INF_FOREHEAD_A),
	//21(L_INF_FOREHEAD_B),27(L_INF_FOREHEAD_E),29(L_INF_FOREHEAD_F),31(L_SUP_FOREHEAD)
	//forse ci vuole anche 20,30,32 (per l'influenza sulla parte destra)
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[31] || FAPs->lfaps[32] || FAPs->lfaps[37] || FAPs->lfaps[38])
	{
		for(i=11; i < 33; i++)
		{
			if((i>=11 && i<=16) || (i>=19 && i<=22) || (i>=27 && i<=32))
			{
				if (i==11 || i==12)
					weight=0.5;
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 * 1.7 + 
								skin_point_1 * skin_point_1 / 2 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 2.6;

					if ( dist < M_PI )
					{
						weight;
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if(FAPs->lfaps[32] && (i%2==0))
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[32]*temp1;
						if(i%2==0)
							skin_point_i_j->increment[2] += COEFZ*0.1*((fabs(FAPs->lfaps[32])+FAPs->lfaps[38]))*temp1;

						if(FAPs->lfaps[31] && (i%2==1))
							if (skin_point_i_j->coordinate[0] != 0.0)
								skin_point_i_j->increment[1] += weight*FAPs->lfaps[31]*temp1;
			
                        if (skin_point_i_j->coordinate[0] != 0.0 && (i%2==1))
							skin_point_i_j->increment[2] += COEFZ*0.1*((fabs(FAPs->lfaps[31])+FAPs->lfaps[37]))*temp1;

						if ( dist < M_PI || FAPs->lfaps[37] || FAPs->lfaps[38] )
						{
							temp1 *= temp1;

							if (skin_point_i_j->coordinate[0] != 0.0)
							{
								if(FAPs->lfaps[38] && (i%2==0))
									skin_point_i_j->increment[0] += FAPs->lfaps[38]*temp1;

								if(FAPs->lfaps[37] && (i%2==1))
									skin_point_i_j->increment[0] += -FAPs->lfaps[37]*temp1;
							}
						}						
					}
				}
				weight=1.0;
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[31] || FAPs->lfaps[37])
		{
			flag_surface1[11] = 1;
			flag_surface1[13] = 1;
			flag_surface1[15] = 1;
			flag_surface1[17] = 1;
			flag_surface1[19] = 1;
			flag_surface1[21] = 1;
			flag_surface1[23] = 1;
			flag_surface1[25] = 1;
			flag_surface1[27] = 1;
			flag_surface1[29] = 1;
			flag_surface1[31] = 1;
			//piccola parte destra
			flag_surface1[16] = 1;
			flag_surface1[20] = 1;
			flag_surface1[30] = 1;
			flag_surface1[32] = 1;
		}

		if(FAPs->lfaps[32] || FAPs->lfaps[38])
		{
			flag_surface1[12] = 1;
			flag_surface1[14] = 1;
			flag_surface1[16] = 1;
			flag_surface1[18] = 1;
			flag_surface1[20] = 1;
			flag_surface1[22] = 1;
			flag_surface1[24] = 1;
			flag_surface1[26] = 1;
			flag_surface1[28] = 1;
			flag_surface1[30] = 1;
			flag_surface1[32] = 1;
			//piccola parte sinistra
			flag_surface1[15] = 1;
			flag_surface1[19] = 1;
			flag_surface1[29] = 1;
			flag_surface1[31] = 1;
		}
	}
}



void ApplyFAPs::apply_fap_33_34()
{	//FINISH !!!	
	int i,j;
	float  ref_point1[3], ref_point2[3];
	float dist=0, weight=1.0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float COEFX=COEF(this->FAPUs->ENSFAPU, this->FAPUs->ESFAPU, 123.37, 71.05);
	float COEFZ=COEF(this->FAPUs->ENSFAPU, this->FAPUs->IFAPU_FI, 123.37, 147.37);
	
	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[4][4][0];
	ref_point1[1] = FDPfp[4][4][1];
	ref_point1[2] = FDPfp[4][4][2];

	ref_point2[0] = FDPfp[4][3][0];
	ref_point2[1] = FDPfp[4][3][1];
	ref_point2[2] = FDPfp[4][3][2];

	//FAP 33: interessa le superfici 11(L_SUP_EYELID_B), 13(L_EYEBROW_A),15(L_EYEBROW_B), 17(L_EYEBROW_C), 19(L_INF_FOREHEAD_A),
	//21(L_INF_FOREHEAD_B), 23(L_INF_FOREHEAD_C), 25(L_INF_FOREHEAD_D), 27(L_INF_FOREHEAD_E),29(L_INF_FOREHEAD_F),31(L_SUP_FOREHEAD)
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[33] || FAPs->lfaps[34] || FAPs->lfaps[37] || FAPs->lfaps[38])
	{
		for(i=11; i < 33; i++)
		{
			if(i>=11 && i<=32)
			{
				if (i==11 || i==12)
					weight=0.6;
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}

					dist = sqrt(
								skin_point_0 * skin_point_0 * 2 + 
								skin_point_1 * skin_point_1 / 2 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 3.5;

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if( (FAPs->lfaps[33] || FAPs->lfaps[37]) && (i%2==1) )
						{
							skin_point_i_j->increment[0] += -FAPs->lfaps[37]*temp1*COEFX;
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[33]*temp1;
							skin_point_i_j->increment[2] += 0.1*fabs(FAPs->lfaps[33])*temp1*COEFZ;
						}

						if( (FAPs->lfaps[34] || FAPs->lfaps[38]) && (i%2==0) )
						{
							skin_point_i_j->increment[0] += FAPs->lfaps[38]*temp1*COEFX;
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[34]*temp1;
							skin_point_i_j->increment[2] += 0.1*fabs(FAPs->lfaps[34])*temp1*COEFZ;
						}
					}
				}
				weight=1.0;
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[33] || FAPs->lfaps[37])
		{
			flag_surface1[11] = 1;
			flag_surface1[13] = 1;
			flag_surface1[15] = 1;
			flag_surface1[17] = 1;
			flag_surface1[19] = 1;
			flag_surface1[21] = 1;
			flag_surface1[23] = 1;
			flag_surface1[25] = 1;
			flag_surface1[27] = 1;
			flag_surface1[29] = 1;
			flag_surface1[31] = 1;
			//piccola parte destra
			flag_surface1[16] = 1;
			flag_surface1[20] = 1;
			flag_surface1[30] = 1;
			flag_surface1[32] = 1;
		}

		if(FAPs->lfaps[34] || FAPs->lfaps[38])
		{
			flag_surface1[12] = 1;
			flag_surface1[14] = 1;
			flag_surface1[16] = 1;
			flag_surface1[18] = 1;
			flag_surface1[20] = 1;
			flag_surface1[22] = 1;
			flag_surface1[24] = 1;
			flag_surface1[26] = 1;
			flag_surface1[28] = 1;
			flag_surface1[30] = 1;
			flag_surface1[32] = 1;
			//piccola parte sinistra
			flag_surface1[15] = 1;
			flag_surface1[19] = 1;
			flag_surface1[29] = 1;
			flag_surface1[31] = 1;
		}
	}
}







void ApplyFAPs::apply_fap_35_36()
{
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, weight=1.0;
	double  temp1 =0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float COEFZ=COEF(this->FAPUs->ENSFAPU, this->FAPUs->IFAPU_FI, 123.37, 147.37);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[4][6][0];
	ref_point1[1] = FDPfp[4][6][1];
	ref_point1[2] = FDPfp[4][6][2];

	ref_point2[0] = FDPfp[4][5][0];
	ref_point2[1] = FDPfp[4][5][1];
	ref_point2[2] = FDPfp[4][5][2];

	//FAP 35: interessa le superfici 11(L_SUP_EYELID_B), 15(L_EYEBROW_B), 17(L_EYEBROW_C),
	//21(L_INF_FOREHEAD_B), 23(L_INF_FOREHEAD_C), 25(L_INF_FOREHEAD_D), 27(L_INF_FOREHEAD_E), 31(L_SUP_FOREHEAD)
	//forse ci vuole la superficie 65
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[35] || FAPs->lfaps[36] || FAPs->lfaps[37] || FAPs->lfaps[38])
	{
		for(i=11; i < 33; i++)
		{
			if((i>=11 && i<=12) || (i>=15 && i<=18) || (i>=21 && i<=28) || (i>=31 && i<=32))
			{
				if (i==11 || i==12)
					weight=0.3;
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 3.0;

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if( (FAPs->lfaps[35] || FAPs->lfaps[37]) && (i%2==1) )
						{
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[35]*temp1;
							skin_point_i_j->increment[2] += 0.3*fabs(FAPs->lfaps[35])*temp1*COEFZ;
						}

						if( (FAPs->lfaps[36] || FAPs->lfaps[38]) && (i%2==0) )
						{
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[36]*temp1;
							skin_point_i_j->increment[2] += 0.3*fabs(FAPs->lfaps[36])*temp1*COEFZ;
						}
					}
				}
				weight=1.0;
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[35] || FAPs->lfaps[37])
		{
			flag_surface1[11] = 1;
			flag_surface1[13] = 1;
			flag_surface1[15] = 1;
			flag_surface1[17] = 1;
			flag_surface1[19] = 1;
			flag_surface1[21] = 1;
			flag_surface1[23] = 1;
			flag_surface1[25] = 1;
			flag_surface1[27] = 1;
			flag_surface1[29] = 1;
			flag_surface1[31] = 1;
		}

		if(FAPs->lfaps[36] || FAPs->lfaps[38])
		{
			flag_surface1[12] = 1;
			flag_surface1[14] = 1;
			flag_surface1[16] = 1;
			flag_surface1[18] = 1;
			flag_surface1[20] = 1;
			flag_surface1[22] = 1;
			flag_surface1[24] = 1;
			flag_surface1[26] = 1;
			flag_surface1[28] = 1;
			flag_surface1[30] = 1;
			flag_surface1[32] = 1;
		}
	}
}




void ApplyFAPs::apply_aux_fap_1()
{//CODE A VERIFIER
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0.0, temp1=0.0;
	double temp2=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	//punto per la correzione 
	ref_point1[0] = FDPfp[4][2][0];
	ref_point1[1] = FDPfp[4][2][1];
	ref_point1[2] = FDPfp[4][2][2];

	//punto per il calcolo del rigonfiamento
	ref_point2[0] = FDPfp[4][1][0];
	ref_point2[1] = FDPfp[4][1][1];
	ref_point2[2] = FDPfp[4][1][2];

	if(FAPs->lfaps[37]>0 && FAPs->lfaps[38]>0)
	{
		for(i=19; i < 31; i++)
		{
			if(i==19 || i==20 || i==29 || i==30)
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else						
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}

					dist = sqrt(
								skin_point_0 * skin_point_0 * 10 +
								skin_point_1 * skin_point_1 / 1.5 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 2.3;

					if ( dist < (M_PI/2.0))
					{
						temp1=(0.5*2*(cos(dist)));

						if(i%2==0)
						{
							skin_point_i_j->increment[1] += 0.3*FAPs->lfaps[38]*FAPs->lfaps[32]*temp1;
							skin_point_i_j->increment[2] += 0.2*(FAPs->lfaps[38]+FAPs->lfaps[37])*temp1;
						}

						//per la parte sinistra sfruttando la simmetria
						if(skin_point_i_j->coordinate[0]!=0.0 && (i%2==1))
						{
							skin_point_i_j->increment[1] += 0.3 * FAPs->lfaps[37] * FAPs->lfaps[31] * temp1;
							skin_point_i_j->increment[2] += 0.2 * (FAPs->lfaps[37] + FAPs->lfaps[38]) * temp1;
						}

						//correzione degli spostamenti delle FAP 31,32
						if(skin_point_i_j->coordinate[0]!=0.0)
						{
							if(i%2==0)
							{
								skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
								skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
								skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
							}
							else						
							{
								skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
								skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
								skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
							}

							dist = sqrt(
										skin_point_0 * skin_point_0 + 
										skin_point_1 * skin_point_1 / 2 + 
										skin_point_2 * skin_point_2
										);
							dist /= 2.5;

							if ( dist < M_PI )
							{
								temp2 = 0.5*(1+cos(dist));
								temp2 *= temp2*temp2*temp2;

								if(i%2==0) skin_point_i_j->increment[0] += -0.8*FAPs->lfaps[38]*(temp2);
								if(i%2==1) skin_point_i_j->increment[0] +=  0.8*FAPs->lfaps[37]*(temp2);
							}
						}
					}
				}
			}
		}
	}
}







void ApplyFAPs::forehead_bump()
{	//A FINIR
	int k=0, i=0, j=0;
	float distance=0.0, aux=0.0, temp1=0.0, temp2=0.0, temp3=0.0; 
	float node1[3];	//for the left eye
	float node2[3];	//for the right eye
	

	node1[0]=10.0;
	node1[1]=5.0;
	node1[2]=19.4;
	node2[0]=-10.0;
	node2[1]=5.0;
	node2[2]=19.4;
	
	pointtype* skin_point_k_i;

	//giochiamo con le normali per fare il bump sulla fronte
	//con questo ciclo facciamo le rughe più esterne
	for (k=31; k<33; k++)
	{
		for(i=0;surface_list1[k][i]!=-1;i++)
		{
			j=1;
			skin_point_k_i = &skin_point[surface_list1[k][i]];

			if (skin_point_k_i->coordinate[j] >= 8.5 && skin_point_k_i->coordinate[j] <= 17.2 && skin_point_k_i->coordinate[j-1] >= 0.0 && skin_point_k_i->coordinate[j-1] <= 13.5 && ( (FAPs->lfaps[33]) > 0 || (FAPs->lfaps[34]) > 0) )
			{				
				distance = sqrt(
								3 * (skin_point_k_i->coordinate[0]-node1[0]) * 
								(skin_point_k_i->coordinate[0]-node1[0]) + 
								(skin_point_k_i->coordinate[1]-node1[1]) * 
								(skin_point_k_i->coordinate[1]-node1[1]) + 
								(skin_point_k_i->coordinate[2]-node1[2]) * 
								(skin_point_k_i->coordinate[2]-node1[2])
								);
				aux = 4.0 * (	cos(skin_point_k_i->coordinate[j]/0.7) * 
								sin(skin_point_k_i->coordinate[j]/0.7)
							) * (exp(-(distance)/3.2))
							;
		
				temp1 = 0.0;
				if (FAPs->lfaps[33]>0 && (i%2==1))
				{
					temp1 = FAPs->lfaps[33] * FAPs->lfaps[33];
					skin_point_k_i->normal[j] += MIN(temp1,5)*aux;
				}
				temp2 = 0.0;
				//Occhio destro 
 				if (FAPs->lfaps[34]>0 && (i%2==0))
				{
					temp2 = FAPs->lfaps[34] * FAPs->lfaps[34];
					skin_point_k_i->normal[j] += MIN(temp2,5)*aux;
				}
			}
		}
	}

	//giochiamo con le normali per fare il bump sulla fronte
	//con questo ciclo facciamo le rughe più esterne
	for (k=31; k<33; k++)
	{
		for(i=0;surface_list1[k][i]!=-1;i++)
		{
			j=1; //era j=2 cioè il vecchio asse z
			
			skin_point_k_i = &skin_point[surface_list1[k][i]];

			if (skin_point_k_i->coordinate[j] >= 8.5 && skin_point_k_i->coordinate[j] <= 17.2 && skin_point_k_i->coordinate[j-1] >= -13.5 && skin_point_k_i->coordinate[j-1] <= 13.5 && (FAPs->lfaps[31] > 0 || FAPs->lfaps[32] > 0 ) )
			{
				if ((FAPs->lfaps[31]-0.2)>0 || (FAPs->lfaps[32]-0.2)>0)
				{
					distance = sqrt(
									(skin_point_k_i->coordinate[0]) * 
									(skin_point_k_i->coordinate[0]) + 
									(skin_point_k_i->coordinate[1] - node1[1]) * 
									(skin_point_k_i->coordinate[1] - node1[1]) / 6 + 
									(skin_point_k_i->coordinate[2] - node1[2]) * 
									(skin_point_k_i->coordinate[2] - node1[2])
									);
					aux = 2.5 * (	cos(skin_point_k_i->coordinate[j]/0.7) * 
									sin(skin_point_k_i->coordinate[j]/0.7)
								) *(exp(-(distance)/3.2))
								;
					temp3=0.0;
					if ((FAPs->lfaps[31]-0.2)>0 && (i%2==1))
					{
						temp3 += (FAPs->lfaps[31]-0.2)*(FAPs->lfaps[31]-0.2);
					}
					if ((FAPs->lfaps[32]-0.2)>0 && (i%2==0))
					{
						temp3 += (FAPs->lfaps[32]-0.2)*(FAPs->lfaps[32]-0.2);
					}
					aux = MIN(temp3,2)*aux;
					skin_point_k_i->normal[j] += aux;	
				}
			}
		}
	}
}


//corner lip

void ApplyFAPs::apply_fap_53_54_59_60()
{	//FINISH !!!	
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, weight=1.0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float COEFZ_L=COEF(this->FAPUs->MWFAPU, this->FAPUs->IFAPU_LCB, 86.64, 641.84);
	float COEFZ_R=COEF(this->FAPUs->MWFAPU, this->FAPUs->IFAPU_RCB, 86.64, 641.84);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][4][0];
	ref_point1[1] = FDPfp[8][4][1];
	ref_point1[2] = FDPfp[8][4][2];

	ref_point2[0] = FDPfp[8][3][0];
	ref_point2[1] = FDPfp[8][3][1];
	ref_point2[2] = FDPfp[8][3][2];

	//FAP 8.4: interessa le superfici (destre) : 2, 36, 38, 40, 42, 48, 50, 52, 54, 56, 60 
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[53] || FAPs->lfaps[54] || FAPs->lfaps[59] || FAPs->lfaps[60])
	{
		for(i=1; i < 61; i++)
		{
			if( (i>=1 && i<=2) || (i>=33 && i<=60) )
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}

					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 3.0;

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if(FAPs->lfaps[54] && (i%2==0))
						{
							skin_point_i_j->increment[0] += -FAPs->lfaps[54]*temp1; 
							skin_point_i_j->increment[2] -= 0.4*FAPs->lfaps[54]*temp1*COEFZ_R;
						}
						
						if(FAPs->lfaps[53] && (i%2==1))
						{
							skin_point_i_j->increment[0] += FAPs->lfaps[53]*temp1;
							skin_point_i_j->increment[2] -= 0.4*FAPs->lfaps[53]*temp1*COEFZ_L;
						}
						
						if(FAPs->lfaps[60] && (i%2==0))
							skin_point_i_j->increment[1] += FAPs->lfaps[60]*temp1;

						if(FAPs->lfaps[59] && (i%2==1))
							skin_point_i_j->increment[1] += FAPs->lfaps[59]*temp1;
					}
				}
			}
		}

		if (FAPs->lfaps[53] != 0)
		{
			press81(FAPs->lfaps[53]*0.1);
			press85(FAPs->lfaps[53]*0.1);
			press86(FAPs->lfaps[53]*0.1);
			prot2(FAPs->lfaps[53]*0.2, 0);
		}
		if (FAPs->lfaps[54] != 0)
		{
			press81(FAPs->lfaps[54]*0.1);
			press85(FAPs->lfaps[54]*0.1);
			press86(FAPs->lfaps[54]*0.1);
			prot2(FAPs->lfaps[54]*0.2, 0);
		}

		if(FAPs->lfaps[54] || FAPs->lfaps[60])
		{
			flag_surface1[2] = 1;
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[46] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
			//piccola parte sinistra
			flag_surface1[59] = 1;
		}

		if(FAPs->lfaps[53] || FAPs->lfaps[59])
		{
			flag_surface1[1] = 1;
			flag_surface1[33] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[45] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
			//piccola parte destra
			flag_surface1[60] = 1;
		}
	}
}




//corner lip 
void ApplyFAPs::apply_fap_6_7_12_13()
{	//A TESTER	
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, weight=0.3, temp1=0.0, weightX=1.0, weightY=1.0, weightZ=1.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float COEFZ_L=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_LCB, 222.61, 641.84);
	float COEFZ_R=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_RCB, 222.61, 641.84);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][5][0];
	ref_point1[1] = FDPfp[2][5][1];
	ref_point1[2] = FDPfp[2][5][2];

	ref_point2[0] = FDPfp[2][4][0];
	ref_point2[1] = FDPfp[2][4][1];
	ref_point2[2] = FDPfp[2][4][2];

	//FAP 2.5: interessa le superfici (destre) : 2, 36, 38, 40, 42, 48, 50, 52, 54, 56, 60 
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if((FAPs->lfaps[6] || FAPs->lfaps[7] || FAPs->lfaps[21] || FAPs->lfaps[13]) && (!FAPs->lfaps[54] || !FAPs->lfaps[53] || !FAPs->lfaps[60] || !FAPs->lfaps[59]))
	{  
		for(i=1; i < 61; i++)
		{
			if((i==1 || i==2) || (i>=33 && i<=44) || (i>=47 && i<=56) || (i==59 || i==60))
			{
				if ((i==1 || i==2) || (i==59 || i==60))
					weight=0.3;
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);   

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if(FAPs->lfaps[7] && !FAPs->lfaps[54] && (i%2==0))
							skin_point_i_j->increment[0] += weight*FAPs->lfaps[7]*temp1; 
						
						if(FAPs->lfaps[6] && !FAPs->lfaps[53] && (i%2==1))
							skin_point_i_j->increment[0] += -weight*FAPs->lfaps[6]*temp1; 

						if(FAPs->lfaps[13] && !FAPs->lfaps[60] && (i%2==0))
						{
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[13]*temp1;
							if(FAPs->lfaps[13]<=0.0)
								skin_point_i_j->increment[2] += -0.3*weight*FAPs->lfaps[13]*temp1*COEFZ_R;
						}

						if(FAPs->lfaps[12] && !FAPs->lfaps[59] && (i%2==1))
						{
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[12]*temp1;
							if(FAPs->lfaps[12]<=0.0)
								skin_point_i_j->increment[2] += -0.3*weight*FAPs->lfaps[12]*temp1*COEFZ_L;
						}						
					}
				}
				weight=0.3;
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[7] || FAPs->lfaps[13])
		{
			flag_surface1[2] = 1;
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[60] = 1;
			//piccola parte sinistra
			flag_surface1[59] = 1;
		}

		if(FAPs->lfaps[6] || FAPs->lfaps[12])
		{
			flag_surface1[1] = 1;
			flag_surface1[33] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[59] = 1;
			//piccola parte destra
			flag_surface1[60] = 1;
		}
	}
}



void ApplyFAPs::apply_fap_55_56()
{	//FINISH !!!
	
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float coefFaceGreta;

	coefFaceGreta=479.683105/FAPUs->AFAPU_UL;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][6][0];
	ref_point1[1] = FDPfp[8][6][1];
	ref_point1[2] = FDPfp[8][6][2];

	ref_point2[0] = FDPfp[8][5][0];
	ref_point2[1] = FDPfp[8][5][1];
	ref_point2[2] = FDPfp[8][5][2];


	//FAP 8.6: interessa le superfici (destre) : 34, 36, 38, 46, 48, 50, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[55] || FAPs->lfaps[56])
	{  
		for(i=33; i < 53; i++)
		{
			if( (i>=33 && i<=38) || (i>=45 && i<=52) )
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 1.5 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 1.5;

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));

						if(FAPs->lfaps[56] && (i%2==0))
							skin_point_i_j->increment[1] += -0.9*(FAPs->lfaps[56]*coefFaceGreta)*temp1;

						if(FAPs->lfaps[55] && (i%2==1))
							if (skin_point_i_j->coordinate[0] != 0.0)
								skin_point_i_j->increment[1] += -0.9*(FAPs->lfaps[55]*coefFaceGreta)*temp1;
					}
				}
			}
		}

		nouse();

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[56])
		{
			flag_surface1[2] = 1;
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[46] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			//piccola parte sinistra
			flag_surface1[33] = 1;
			flag_surface1[45] = 1;
		}

		if(FAPs->lfaps[55])
		{
			flag_surface1[1] = 1;
			flag_surface1[33] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[45] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			flag_surface1[57] = 1;
			//piccola parte destra
			flag_surface1[34] = 1;
			flag_surface1[46] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_8_9()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][6][0];
	ref_point1[1] = FDPfp[2][6][1];
	ref_point1[2] = FDPfp[2][6][2];

	ref_point2[0] = FDPfp[2][7][0];
	ref_point2[1] = FDPfp[2][7][1];
	ref_point2[2] = FDPfp[2][7][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 38, 46, 48, 50, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[8] || FAPs->lfaps[9])
	{
		for(i=33; i < 53; i++)
		{
			if((i>=33 && i<=38) || (i>=45 && i<=52))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(FAPs->lfaps[8])
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
						
						dist = sqrt(
								skin_point_0 * skin_point_0 / 10 + 
								skin_point_1 * skin_point_1 / 2 + 
								skin_point_2 * skin_point_2 / 5
								);
						if ( dist < M_PI )
						{
							temp1  =0.5*(1+cos(dist));
							temp1 *= temp1;
							temp1 *= temp1;
							skin_point_i_j->increment[1] += -0.3*FAPs->lfaps[8]*temp1;
						}
					}

					if(FAPs->lfaps[9])
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
						
						dist = sqrt(
								skin_point_0 * skin_point_0 / 10 + 
								skin_point_1 * skin_point_1 / 2 + 
								skin_point_2 * skin_point_2 / 5
								);
						if ( dist < M_PI )
						{
							temp1  =0.5*(1+cos(dist));
							temp1 *= temp1;
							temp1 *= temp1;
							skin_point_i_j->increment[1] += -0.3*FAPs->lfaps[9]*temp1;
						}
					}
					
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[8])
		{
			flag_surface1[2] = 1;
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[46] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			//piccola parte sinistra
			flag_surface1[33] = 1;
			flag_surface1[45] = 1;
		}

		if(FAPs->lfaps[9])
		{
			flag_surface1[1] = 1;
			flag_surface1[33] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[45] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			//piccola parte destra
			flag_surface1[34] = 1;
			flag_surface1[46] = 1;
		}
	}
}



void ApplyFAPs::apply_fap_51_17()
{	//FINISH !!!
	
	int i,j;
	float ref_point1[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float coef=0;
	float coefFaceGreta;

	coefFaceGreta=479.683105/FAPUs->AFAPU_UL;

	//printf("FAPUs->AFAPU_UL %f\n", FAPUs->AFAPU_UL);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][1][0];	// dont need ref_point2
	ref_point1[1] = FDPfp[8][1][1];
	ref_point1[2] = FDPfp[8][1][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 46, 48, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[51] || FAPs->lfaps[17])
	{
		for(i=3; i < 53; i++)
		{
			if((i>=3 && i<=4) || (i>=33 && i<=38) || (i>=45 && i<=52) )
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];

					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 7.0 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 2.5;					

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));

						if(FAPs->lfaps[51] && (i%2==0))
							skin_point_i_j->increment[1] += -0.6*(FAPs->lfaps[51]*coefFaceGreta)*temp1;

						if ( skin_point[surface_list1[i][j]].coordinate[0] != 0.0)
							if(FAPs->lfaps[51] && (i%2==1))
								skin_point_i_j->increment[1] += -0.6*(FAPs->lfaps[51]*coefFaceGreta)*temp1;

						if (FAPs->lfaps[17]<0) 
							coef=0.2;
						else
							coef=0.7;

						if(FAPs->lfaps[17] && (i%2==0))
							skin_point_i_j->increment[2] += coef*FAPs->lfaps[17]*temp1;

						if ( skin_point_i_j->coordinate[0] != 0.0)
							if(FAPs->lfaps[17] && (i%2==1))
								skin_point_i_j->increment[2] += coef*FAPs->lfaps[17]*temp1;
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[51] || FAPs->lfaps[17])
		{
			flag_surface1[4] = 1;
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[46] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			//piccola parte sinistra
			flag_surface1[33] = 1;
			flag_surface1[45] = 1;
		}

		if(FAPs->lfaps[51] || FAPs->lfaps[17])
		{
			flag_surface1[3] = 1;
			flag_surface1[33] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[45] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			//piccola parte destra
			flag_surface1[34] = 1;
			flag_surface1[46] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_4_17()
{	//FINISH !!!
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float coef=1;

	float COEFZ=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_ULP, 222.61, 512.0);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][2][0];
	ref_point1[1] = FDPfp[2][2][1];
	ref_point1[2] = FDPfp[2][2][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 46, 48, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[4] || FAPs->lfaps[17])
	{
		for(i=33; i < 53; i++)
		{
			if((i>=33 && i<=38) || (i>=45 && i<=52))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					dist = sqrt(
								skin_point_0 * skin_point_0 / 3.0f + 
								skin_point_1 * skin_point_1 / 10.0f + 
								skin_point_2 * skin_point_2
								);   
				
					if ( dist < M_PI )
					{
						temp1 =0.3*(1+cos(dist));
						if(FAPs->lfaps[4])
							skin_point_i_j->increment[1] += -1.2*FAPs->lfaps[4]*temp1;
				
						//if(FAPs->lfaps[4])
						//	skin_point_i_j->increment[1] += -1.2*FAPs->lfaps[4]*temp1;

						/*
						if (FAPs->lfaps[17]<0) 
							coef=0.8;
						else
							coef=0.3;
						*/
						if(FAPs->lfaps[17])
							skin_point_i_j->increment[2] -= COEFZ*coef*FAPs->lfaps[17]*temp1;
							//skin_point_i_j->increment[2] -= coef*FAPs->lfaps[17]*temp1;

					//		if(FAPs->lfaps[17] && (i%2==1))
					//			skin_point_i_j->increment[2] -= COEFZ*coef*FAPs->lfaps[17]*temp1;
					}
				}
			}
		}

		if(FAPs->lfaps[4])
		{
			flag_surface1[2] = 1;
			flag_surface1[34] = 1;
			flag_surface1[46] = 1;
			flag_surface1[33] = 1;
			flag_surface1[45] = 1;
		}

		if(FAPs->lfaps[17])
		{
			flag_surface1[1] = 1;
			flag_surface1[33] = 1;
			flag_surface1[34] = 1;
			flag_surface1[35] = 1;
			flag_surface1[36] = 1;
			flag_surface1[37] = 1;
			flag_surface1[38] = 1;
			flag_surface1[45] = 1;
			flag_surface1[46] = 1;
			flag_surface1[47] = 1;
			flag_surface1[48] = 1;
			flag_surface1[49] = 1;
			flag_surface1[50] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_57_58()
{	//NOT TESTED	
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float coefFaceGreta;

	coefFaceGreta=465.752380/FAPUs->AFAPU_LL;

	float ***FDPfp = FDPs->featurePoints;

	//ref_point1[0] = FDPfp[8][8][0]+0.5;
	ref_point1[0] = FDPfp[8][8][0];
	ref_point1[1] = FDPfp[8][8][1];
	ref_point1[2] = FDPfp[8][8][2];

	ref_point2[0] = FDPfp[8][7][0];
	ref_point2[1] = FDPfp[8][7][1];
	ref_point2[2] = FDPfp[8][7][2];

	//FAP 8.8: interessa le superfici (destre) : 40, 42, 44, 54, 56, 58, 60
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[57] || FAPs->lfaps[58])
	{
		for(i=39; i < 61; i++)
		{
			if((i>=39 && i<=44) || (i>53 && i<=60))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 1.5 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 1.5;

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));

						if(FAPs->lfaps[58] && (i%2==0))
							skin_point_i_j->increment[1] += 0.1*(FAPs->lfaps[58]*coefFaceGreta)*temp1;

						if ( skin_point_i_j->coordinate[0] != 0.0)
							if(FAPs->lfaps[57] && (i%2==1))
								skin_point_i_j->increment[1] += 0.1*(FAPs->lfaps[57]*coefFaceGreta)*temp1;
						else
							if(FAPs->lfaps[57] && (i%2==1))
								skin_point_i_j->increment[1] += 0.15*(FAPs->lfaps[57]*coefFaceGreta)*temp1;
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[58])
		{
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
			//piccola parte sinistra
			flag_surface1[43] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
		}

		if(FAPs->lfaps[57])
		{
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
			//piccola parte destra
			flag_surface1[44] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_10_11()
{	//FINISH !!!
	
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][9][0]; //potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point1[1] = FDPfp[2][9][1];
	ref_point1[2] = FDPfp[2][9][2];

	ref_point2[0] = FDPfp[2][8][0]; //potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point2[1] = FDPfp[2][8][1];
	ref_point2[2] = FDPfp[2][8][2];

	//FAP 2.9: interessa le superfici (destre) : 40, 42, 44, 54, 56, 58, 60
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[10] || FAPs->lfaps[11])
	{
		for(i=39; i < 59; i++)
		{
			if((i>=39 && i<=44) || (i>=51 && i<=58))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(FAPs->lfaps[10])
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];

						dist = sqrt(
									skin_point_0 * skin_point_0 + 
									skin_point_1 * skin_point_1 * 2 + 
									skin_point_2 * skin_point_2 / 01.1
									);   
						dist /= 1.5;
						
						if ( dist < M_PI )
						{
							temp1 = 0.5*(1+cos(dist)); 
                            skin_point_i_j->increment[1] += 0.3*FAPs->lfaps[10]*temp1;
						}
					}

					if(FAPs->lfaps[11])
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];

						dist = sqrt(
									skin_point_0 * skin_point_0 + 
									skin_point_1 * skin_point_1 * 2 + 
									skin_point_2 * skin_point_2 / 01.1
									);   
						dist /= 1.5;
						
						if ( dist < M_PI )
						{
							temp1 = 0.5*(1+cos(dist)); 
                            skin_point_i_j->increment[1] += 0.3*FAPs->lfaps[11]*temp1;
						}
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[11])
		{
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
			//piccola parte sinistra
			flag_surface1[43] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
		}

		if(FAPs->lfaps[10])
		{
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
			//piccola parte destra
			flag_surface1[44] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
		}
	}
}



void ApplyFAPs::apply_fap_52_16()
{	//FINISH !!!
	
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float coef;
	float coefFaceGreta;

	coefFaceGreta=465.752380/FAPUs->AFAPU_LL;

	float COEFZ=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_LLP, 222.61, 784.14);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][2][0];
	ref_point1[1] = FDPfp[8][2][1];
	ref_point1[2] = FDPfp[8][2][2];

	//FAP 8.6: interessa le superfici (destre) : 42, 44, 56, 58, 60
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[52] || FAPs->lfaps[16])
	{
		for(i=39; i < 61; i++)
		{
			if( (i>=39 && i<=44) || (i>=51 && i<=60) )
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					dist = sqrt(
								skin_point_0 * skin_point_0 / 5 + 
								skin_point_1 * skin_point_1 * 3 + 
								skin_point_2 * skin_point_2
								);
					dist /= 1.5;

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));
						if(FAPs->lfaps[52] && (i%2==0))
							skin_point_i_j->increment[1] += 0.7*(FAPs->lfaps[52]*coefFaceGreta)*temp1;
						
						if ( skin_point_i_j->coordinate[0] != 0.0)
							if(FAPs->lfaps[52] && (i%2==1))
								skin_point_i_j->increment[1] += 0.7*(FAPs->lfaps[52]*coefFaceGreta)*temp1;

						if (FAPs->lfaps[16]<0) 
							coef=0.7;
						else
							coef=0.4;

						if(FAPs->lfaps[16] && (i%2==0))
							skin_point_i_j->increment[2] += 0.5*FAPs->lfaps[16]*temp1*COEFZ;

						if ( skin_point_i_j->coordinate[0] != 0.0)
							if(FAPs->lfaps[16] && (i%2==1))
								skin_point_i_j->increment[2] += 0.5*FAPs->lfaps[16]*temp1*COEFZ;
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[52] || FAPs->lfaps[16])
		{
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
		}

		if(FAPs->lfaps[52] || FAPs->lfaps[16])
		{
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_5_16()
{	//FINISH !!!
	
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0, weight, weightZ=1;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	float coef;

	float COEFZ=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_LLP, 222.61, 784.14);
	float COEFY=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_MH, 222.61, 249.75);

	//MAURIZIO
	//TODO: assign weightZ

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][3][0]; //potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point1[1] = FDPfp[2][3][1];
	ref_point1[2] = FDPfp[2][3][2];

	//FAP 8.6: interessa le superfici (destre) : 42, 44, 56, 58, 60
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[5] || FAPs->lfaps[16])
	{  
		for(i=41; i < 61; i++)
		{
			if((i>=41 && i<=44) || (i>=55 && i<=60))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];

					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 4.0f + 
								skin_point_2 * skin_point_2
								);   
					dist /= 1.5;

					if ( dist < M_PI )
					{
						weight=.5;
						temp1 =0.5*(1+cos(dist));

						if(FAPs->lfaps[5] && (i%2==0))
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[5]*temp1*COEFY;

						if (skin_point_i_j->coordinate[0] != 0.0)
							if(FAPs->lfaps[5] && (i%2==1))
								skin_point_i_j->increment[1] += weight*FAPs->lfaps[5]*temp1*COEFY;

						if (FAPs->lfaps[16]<0) 
							coef=0.3;
						else
							coef=0.6;

						if(FAPs->lfaps[16] && (i%2==0))
							skin_point_i_j->increment[2] += weightZ*FAPs->lfaps[16]*temp1*COEFZ;

						if (skin_point_i_j->coordinate[0] != 0.0)
							if(FAPs->lfaps[16] && (i%2==1))
								skin_point_i_j->increment[2] += weightZ*FAPs->lfaps[16]*temp1*COEFZ;
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[5] || FAPs->lfaps[16])
		{
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
		}

		if(FAPs->lfaps[5] || FAPs->lfaps[16])
		{
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
		}
	}
}



void ApplyFAPs::apply_fap_3_14_15()
{	//FINISH !!!
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0, weight=1.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float COEFX_3_14=COEF(this->FAPUs->MNSFAPU, this->FAPUs->MWFAPU, 222.61, 86.64);
	float COEFY_15  =COEF(this->FAPUs->MWFAPU, this->FAPUs->MNSFAPU, 86.64, 222.61);
	float COEFZ=1.0;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][1][0];
	ref_point1[1] = FDPfp[2][1][1];
	ref_point1[2] = FDPfp[2][1][2];

	//FAP 8.6: interessa le superfici (destre) : 2, 52, 54, 56, 58, 60, 68
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[3] || FAPs->lfaps[14] || FAPs->lfaps[15])
	{  
		for(i=1; i < 69; i++)
		{
			if((i==1 || i==2) || (i>=39 && i<=44) || (i>=51 && i<=60) || (i==67 || i==68))
			{
				/*if ((i>=39 && i<=44) || (i>=55 && i<=58))
				{
					if(FAPs->lfaps[3]>0)
						weight = 0.3;
					else
						weight = 0.6;
				}*/
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					dist = sqrt(
								skin_point_0 * skin_point_0 / 16 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2 / 4
								);   
					dist /= 3.5; //3.5					

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));

						if(FAPs->lfaps[3] || FAPs->lfaps[14] || FAPs->lfaps[15])
						{
							if(i%2==0)
							{
								skin_point_i_j->increment[0] += -weight*FAPs->lfaps[15]*temp1;
								skin_point_i_j->increment[1] += -weight*FAPs->lfaps[3]*temp1;
								skin_point_i_j->increment[2] += weight*FAPs->lfaps[14]*temp1;
							}
						
							if (skin_point_i_j->coordinate[0] != 0.0 && (i%2==1))
							{
								skin_point_i_j->increment[0] += -weight*FAPs->lfaps[15]*temp1;
								skin_point_i_j->increment[1] += -weight*FAPs->lfaps[3]*temp1;
								skin_point_i_j->increment[2] += weight*FAPs->lfaps[14]*temp1;
							}
						}
					}
				}
				weight = 1.0;
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[3] || FAPs->lfaps[14] || FAPs->lfaps[15])
		{
			flag_surface1[2] = 1;
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
			flag_surface1[68] = 1;
		}

		if(FAPs->lfaps[3] || FAPs->lfaps[14] || FAPs->lfaps[15])
		{
			flag_surface1[1] = 1;
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
			flag_surface1[67] = 1;
		}
	}
}





void ApplyFAPs::apply_fap_18()
{	// FINISH ??
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	pointtype* skin_point_i_j;

	float COEFZ=COEF(this->FAPUs->MNSFAPU, this->FAPUs->IFAPU_LLP, 222.61, 784.14);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[2][10][0]; 
	ref_point1[1] = FDPfp[2][10][1];
	ref_point1[2] = FDPfp[2][10][2];

	//FAP 2.10: interessa le superfici (destre) : 52, 54, 56, 58, 60, 68
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[18])
	{
		for(i=39; i < 69; i++)
		{
			if((i>=39 && i<=44) || (i>=51 && i<=60) || i==67 || i==68)
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					dist = sqrt(
								skin_point_0 * skin_point_0 / 12 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);   

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));
						
						if(FAPs->lfaps[18])
						{
							if(i%2==0)
								skin_point_i_j->increment[1] += FAPs->lfaps[18]*temp1;
							if(FAPs->lfaps[18]>0 && (i%2==0))
								skin_point_i_j->increment[2] += 0.3*FAPs->lfaps[18]*temp1*COEFZ;
					
							if (skin_point_i_j->coordinate[0] != 0.0 && (i%2==1))
							{
								skin_point_i_j->increment[1] += FAPs->lfaps[18]*temp1;
								if(FAPs->lfaps[18]>0)
									skin_point_i_j->increment[2] += 0.3*FAPs->lfaps[18]*temp1*COEFZ;
							}
						}						
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[18])
		{
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[44] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[58] = 1;
			flag_surface1[60] = 1;
			flag_surface1[68] = 1;
		}

		if(FAPs->lfaps[18])
		{
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[43] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[57] = 1;
			flag_surface1[59] = 1;
			flag_surface1[67] = 1;
		}
	}
}





void ApplyFAPs::apply_fap_39_40()
{	//FINISH ??
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, weight=1.0, temp1=0.0;
	
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float COEFZ_L=COEF(this->FAPUs->ESFAPU, this->FAPUs->IFAPU_LCB, 71.05, 641.84);
	float COEFZ_R=COEF(this->FAPUs->ESFAPU, this->FAPUs->IFAPU_RCB, 71.05, 641.84);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[5][2][0]; 
	ref_point1[1] = FDPfp[5][2][1];
	ref_point1[2] = FDPfp[5][2][2];

	ref_point2[0] = FDPfp[5][1][0]; 
	ref_point2[1] = FDPfp[5][1][1];
	ref_point2[2] = FDPfp[5][1][2];

	//FAP 8.4: interessa le superfici (destre) : 2, 36, 38, 40, 42, 48, 50, 52, 54, 56, 60,68 
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[39] || FAPs->lfaps[40])
	{
		for(i=1; i < 69; i++)
		{
			if((i==1 || i==2) || (i>=35 && i<=42) || (i>47 && i<=56) || (i==59 || i==60) || (i==67 || i==68))
			{
				if ( (i>=35 && i<=42) || (i>=47 && i<=50) || (i>=53 && i<=56))
					weight=1.0;
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 / 2 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 3.0;

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if(FAPs->lfaps[40] && (i%2==0))
						{
							skin_point_i_j->increment[0] += -weight*FAPs->lfaps[40]*temp1; 
							if(FAPs->lfaps[40]>0)
								skin_point_i_j->increment[2] += 0.5*FAPs->lfaps[40]*temp1*COEFZ_R;
						}

						if(FAPs->lfaps[39] && (i%2==1))
						{
							skin_point_i_j->increment[0] += weight*FAPs->lfaps[39]*temp1; 
							if(FAPs->lfaps[39]>0)
								skin_point_i_j->increment[2] += 0.5*FAPs->lfaps[39]*temp1*COEFZ_L;
						}
					}
				}
				weight=1.0;
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[40])
		{
			flag_surface1[2] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[40] = 1;
			flag_surface1[42] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			flag_surface1[54] = 1;
			flag_surface1[56] = 1;
			flag_surface1[60] = 1;
			flag_surface1[68] = 1;
			//piccola parte sinistra
			flag_surface1[59] = 1;
		}

		if(FAPs->lfaps[39])
		{
			flag_surface1[1] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[39] = 1;
			flag_surface1[41] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			flag_surface1[53] = 1;
			flag_surface1[55] = 1;
			flag_surface1[59] = 1;
			flag_surface1[67] = 1;
			//piccola parte destra
			flag_surface1[60] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_41_42()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float COEFZ_L=COEF(this->FAPUs->ENSFAPU, this->FAPUs->IFAPU_LCB, 123.37, 641.84);
	float COEFZ_R=COEF(this->FAPUs->ENSFAPU, this->FAPUs->IFAPU_RCB, 123.37, 641.84);

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[5][4][0];
	ref_point1[1] = FDPfp[5][4][1];
	ref_point1[2] = FDPfp[5][4][2];

	ref_point2[0] = FDPfp[5][3][0];
	ref_point2[1] = FDPfp[5][3][1];
	ref_point2[2] = FDPfp[5][3][2];

	//FAP 8.4: interessa le superfici (destre) : 2, 4, 8
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[41] || FAPs->lfaps[42])
	{
		for(i=1; i < 53; i++)
		{
			if((i>=1 && i<=4) || (i==7 || i==8) || (i>=49 && i<52))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);    
					dist /= 3.0;

					if ( dist < M_PI )
					{
						temp1 = 0.5*(1+cos(dist));
						temp1 *= temp1;

						if(FAPs->lfaps[42] && (i%2==0))
						{
							skin_point_i_j->increment[1] += FAPs->lfaps[42]*temp1; 
							if(FAPs->lfaps[42]>0)
								skin_point_i_j->increment[2] += 0.5*FAPs->lfaps[42]*temp1*COEFZ_R;
							else
								skin_point_i_j->increment[2] += -0.1*FAPs->lfaps[42]*temp1*COEFZ_R;
						}

						if(FAPs->lfaps[41] && (i%2==1))
						{
							skin_point_i_j->increment[1] += FAPs->lfaps[41]*temp1; 
							if(FAPs->lfaps[41]>0)
								skin_point_i_j->increment[2] += 0.5*FAPs->lfaps[41]*temp1*COEFZ_L;
							else
								skin_point_i_j->increment[2] += -0.1*FAPs->lfaps[41]*temp1*COEFZ_L;
						}				
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[42])
		{
			flag_surface1[2] = 1;
			flag_surface1[4] = 1;
			flag_surface1[8] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
		}

		if(FAPs->lfaps[41])
		{
			flag_surface1[1] = 1;
			flag_surface1[3] = 1;
			flag_surface1[7] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
		}
	}
}





void ApplyFAPs::cheek_deformation()
{
	int i,j;
	float ref_point1[3], ref_point2[3], ref_point3[3], ref_point4[3]; 
	float dist1=0.0, dist2=0.0, temp1=0.0, temp2=0.0, aux1=0.0, aux2=0.0, distance=0.0, attenuation1=0.0, attenuation2=0.0, attenuation3=0.0, point[3];
	float lfap_53, lfap_55, lfap_59, lfap_54, lfap_56, lfap_60, increment=0.0;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][3][0]; 
	ref_point1[1] = FDPfp[8][3][1]; 
	ref_point1[2] = FDPfp[8][3][2];


	ref_point2[0] = FDPfp[8][5][0];//-1.0; 
	ref_point2[1] = FDPfp[8][5][1]; 
	ref_point2[2] = FDPfp[8][5][2];


	ref_point3[0] = FDPfp[8][4][0]; 
	ref_point3[1] = FDPfp[8][4][1]; 
	ref_point3[2] = FDPfp[8][4][2];


	ref_point4[0] = FDPfp[8][6][0];//-1.0; 
	ref_point4[1] = FDPfp[8][6][1]; 
	ref_point4[2] = FDPfp[8][6][2];


	//FAP 8.4: interessa le superfici (destre) : 2, 4, 8
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[53] || FAPs->lfaps[54] || FAPs->lfaps[55] || FAPs->lfaps[56] || FAPs->lfaps[59] || FAPs->lfaps[60])
	{  
		//considero prima le superfici sinistre
		//i = 1;
		//devo scandire tutta la lista
		//j = 0;

		dist1=M_PI;

		for(i=1;i<3;i++)
		{
		
			for(j=0;surface_list1[i][j]!=-1;j++)
			{

				if(i==1)
				{
					//occhio se serve stringere il centroide
					dist1 = sqrt((skin_point[surface_list1[i][j]].coordinate[0]-ref_point1[0])*(skin_point[surface_list1[i][j]].coordinate[0]-ref_point1[0])+(skin_point[surface_list1[i][j]].coordinate[1]-ref_point1[1])*(skin_point[surface_list1[i][j]].coordinate[1]-ref_point1[1])+(skin_point[surface_list1[i][j]].coordinate[2]-ref_point1[2])*(skin_point[surface_list1[i][j]].coordinate[2]-ref_point1[2]));   
					dist1 = dist1/3.0;//3.0; //la distanza a cui arriva il displacement è 3.5*M_PI/2
					
					dist2 = sqrt((skin_point[surface_list1[i][j]].coordinate[0]-ref_point2[0])*(skin_point[surface_list1[i][j]].coordinate[0]-ref_point2[0])+(skin_point[surface_list1[i][j]].coordinate[1]-ref_point2[1])*(skin_point[surface_list1[i][j]].coordinate[1]-ref_point2[1])+(skin_point[surface_list1[i][j]].coordinate[2]-ref_point2[2])*(skin_point[surface_list1[i][j]].coordinate[2]-ref_point2[2]));   
					dist2 = dist2/5.5;//4.5; //la distanza a cui arriva il displacement è 3.5*M_PI/2
				}
				if(i==2)
				{
					dist1 = sqrt((skin_point[surface_list1[i][j]].coordinate[0]-ref_point3[0])
						*(skin_point[surface_list1[i][j]].coordinate[0]-ref_point3[0])
						+(skin_point[surface_list1[i][j]].coordinate[1]-ref_point3[1])
						*(skin_point[surface_list1[i][j]].coordinate[1]-ref_point3[1])
						+(skin_point[surface_list1[i][j]].coordinate[2]-ref_point3[2])
						*(skin_point[surface_list1[i][j]].coordinate[2]-ref_point3[2]));   
					dist1 = dist1/3.0;//3.0; //la distanza a cui arriva il displacement è 3.5*M_PI/2
					
					dist2 = sqrt((skin_point[surface_list1[i][j]].coordinate[0]-ref_point4[0])
						*(skin_point[surface_list1[i][j]].coordinate[0]-ref_point4[0])
						+(skin_point[surface_list1[i][j]].coordinate[1]-ref_point4[1])
						*(skin_point[surface_list1[i][j]].coordinate[1]-ref_point4[1])
						+(skin_point[surface_list1[i][j]].coordinate[2]-ref_point4[2])
						*(skin_point[surface_list1[i][j]].coordinate[2]-ref_point4[2]));   
					dist2 = dist2/5.5;//4.5; //la distanza a cui arriva il displacement è 3.5*M_PI/2

				}
				if ( dist1 < M_PI )
				{

					temp1 =0.5*(1+cos(dist1));
					temp2 =0.5*(1+cos(dist2));
					
					distance = skin_point[surface_list1[i][j]].distance;

					point[0] = skin_point[surface_list1[i][j]].coordinate[0];
					point[1] = skin_point[surface_list1[i][j]].coordinate[1];
					point[2] = skin_point[surface_list1[i][j]].coordinate[2];

					//per la sinistra
					lfap_53 = FAPs->lfaps[53];
					lfap_55 = FAPs->lfaps[55];
					lfap_59 = FAPs->lfaps[59];

					if(lfap_53 <= 0.0) lfap_53 = 0.0;
					if(lfap_55 >= 0.0) lfap_55 = 0.0;
					if(lfap_59 <= 0.0) lfap_59 = 0.3*lfap_59;


					//per la destra
					lfap_54 = FAPs->lfaps[54];
					lfap_56 = FAPs->lfaps[56];
					lfap_60 = FAPs->lfaps[60];

					if(lfap_54 <= 0.0) lfap_54 = 0.0;
					if(lfap_56 >= 0.0) lfap_56 = 0.0;
					if(lfap_60 <= 0.0) lfap_60 = 0.3*lfap_60;

					//calcolo il displacement 
					//aux1=10.2*(1-exp((-distance)/7))*(1-exp(-(distance/0.7)*(distance/0.7)))*sqrt(exp(-((point[1]-(ref_point1[1]+fabs(lfap_55)))/5)*((point[1]-(ref_point1[1]+fabs(lfap_55)))/5)));
					//aux1=2.0*(1-exp((-distance)/7))*(1-exp(-(distance/0.7)*(distance/0.7)))*sqrt(exp(-((point[1]-(ref_point1[1]))/5)*((point[1]-(ref_point1[1]))/5)));				
					//aux1=2.0*(1-exp((-distance)/7))*(1-exp(-(distance/0.5)*(distance/0.5)))*sqrt(exp(-(dist1/1.5)*(dist2/1.5)));
					//aux1 = 3.0*(1-exp((-distance)/14))*(1-exp(-(distance/0.5)*(distance/0.5)))*sqrt(exp(-((point[1]-(ref_point1[1]-lfap_55))/5)*((point[1]-(ref_point1[1]-lfap_56))/5)));					
					if(i==1)
					{
						aux1 = 3.0*(1-exp((-distance)/14))*(1-exp(-(distance/0.5)*(distance/0.5)))
						*sqrt(exp(-((point[1]-(ref_point1[1]-lfap_55))/5)
						*((point[1]-(ref_point1[1]-lfap_56))/5)));					
					}
					if(i==2)
					{
						aux1 = 3.0*(1-exp((-distance)/14))*(1-exp(-(distance/0.5)*(distance/0.5)))
						*sqrt(exp(-((point[1]-(ref_point3[1]-lfap_55))/5)
						*((point[1]-(ref_point3[1]-lfap_56))/5)));			
					}

					

					//per la parte sinistra
					if(i==1)
					{
						if(FAPs->lfaps[53] || FAPs->lfaps[55] || FAPs->lfaps[59]){
							//temp1=0;
							//temp2=0;

							temp1=temp1*temp1;
							temp2=temp2*temp2;

							attenuation1 = point[1]-ref_point1[1];
							if(attenuation1 >= 0) attenuation1 = 0.0;
							attenuation1 = 1/(1-1.5*attenuation1); //per attenuare il valore del displacement quando scendo sotto il livello della bocca

							attenuation3 = point[1]-FDPfp[9][1][1];;
							if(attenuation3 <= 0) attenuation3 = 0.0;
							attenuation3 = 1/(1+1*attenuation3); //per attenuare il valore del displacement quando salgo sopra il livello del naso

							attenuation2 = 0.0;
							if(FAPs->lfaps[39]>=0.0) attenuation2 = FAPs->lfaps[39];
							attenuation2 = 1/(1+4*attenuation2); //per attenuare il valore del displacement se allargo le guancie

					
							//increment = (aux1*(0.25*lfap_53*temp1-1.0*(lfap_55)*temp2+0.25*fabs(lfap_59)*temp1)*attenuation1*attenuation2*attenuation3);
							increment = (aux1*(0.75*lfap_53*temp1-1.0*(lfap_55)*temp2+0.75*fabs(lfap_59)*temp1)*attenuation1*attenuation2*attenuation3);
							if(increment>0.4) 
								increment=0.4;			 //metto un limite al livello dello pieghetta (0.4 è stato trovato sul campo)


							skin_point[surface_list1[1][j]].increment[2] += (increment);
							skin_point[surface_list1[1][j]].increment[0] += (increment);


						}
					}

					//fine parte sinistra
					if(i==2)
					{

					//per la parte destra, sfruttando la simmetria
						if(FAPs->lfaps[54] || FAPs->lfaps[56] || FAPs->lfaps[60])
						{

							temp1=temp1*temp1;
							temp2=temp2*temp2;

							attenuation1 = point[1]-ref_point3[1];
							if(attenuation1 >= 0) 
								attenuation1 = 0.0;
							attenuation1 = 1/(1-1.5*attenuation1); //per attenuare il valore del displacement quando scendo sotto il livello della bocca

							attenuation3 = point[1]-FDPfp[9][2][1];;
							if(attenuation3 <= 0)
								attenuation3 = 0.0;
							attenuation3 = 1/(1+1*attenuation3); //per attenuare il valore del displacement quando salgo sopra il livello del naso

							attenuation2 = 0.0;
							if(FAPs->lfaps[40]>=0.0) 
								attenuation2 = FAPs->lfaps[40];
							attenuation2 = 1/(1+4*attenuation2); //per attenuare il valore del displacement se allargo le guancie

					
							//increment = (aux1*(0.25*lfap_53*temp1-1.0*(lfap_55)*temp2+0.25*fabs(lfap_59)*temp1)*attenuation1*attenuation2*attenuation3);
							increment = (aux1*(0.75*lfap_54*
								temp1-1.0*(lfap_56)*temp2+0.75*
								fabs(lfap_60)*temp1)*attenuation1*attenuation2*attenuation3);
							if(increment>0.4)
								increment=0.4;			 //metto un limite al livello dello pieghetta (0.4 è stato trovato sul campo)
							
							skin_point[surface_list1[i][j]].increment[2] += increment;
							skin_point[surface_list1[i][j]].increment[0] += -increment;
						}
					//fine parte destra
					}
				}
			}
		}
	}


		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[53] || FAPs->lfaps[55] || FAPs->lfaps[59]){
			flag_surface1[1] = 1;
		}

		if(FAPs->lfaps[54] || FAPs->lfaps[56] || FAPs->lfaps[60]){
			flag_surface1[2] = 1;
		}
}



void ApplyFAPs::apply_aux_fap_2()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[5][4][0]; 
	ref_point1[1] = FDPfp[5][4][1];
	ref_point1[2] = FDPfp[5][4][2];

	ref_point2[0] = FDPfp[5][3][0]; 
	ref_point2[1] = FDPfp[5][3][1];
	ref_point2[2] = FDPfp[5][3][2];

	//FAP 8.4: interessa le superfici (destre) : 2, 4, 8
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if((FAPs->lfaps[55] < 0.0 || FAPs->lfaps[56] < 0.0 || FAPs->lfaps[59] > 0.0 || FAPs->lfaps[60] > 0.0))
	{  
		for(i=1; i < 9; i++)
		{
			if((i>=1 && i<=4) || (i==7 ||i==8))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}

					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 2.2;

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));
						
						if(FAPs->lfaps[55] < 0.0 && (i%2==1))
						{
							skin_point_i_j->increment[0] += 0.2*FAPs->lfaps[55]*temp1; 
							skin_point_i_j->increment[2] += -0.2*FAPs->lfaps[55]*temp1;
						}
						if(FAPs->lfaps[59] > 0.0 && (i%2==1))
						{
							skin_point_i_j->increment[0] += -0.2*FAPs->lfaps[59]*temp1; 
							skin_point_i_j->increment[2] += 0.2*FAPs->lfaps[59]*temp1;
						}

						if(FAPs->lfaps[56] < 0.0 && (i%2==0))
						{
							skin_point_i_j->increment[0] += -0.2*FAPs->lfaps[56]*temp1; 
							skin_point_i_j->increment[2] += -0.2*FAPs->lfaps[56]*temp1;
						}
						if(FAPs->lfaps[60] > 0.0 && (i%2==0))
						{
							skin_point_i_j->increment[0] += 0.2*FAPs->lfaps[60]*temp1; 
							skin_point_i_j->increment[2] += 0.2*FAPs->lfaps[60]*temp1;
						}
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[56] < 0.0 || FAPs->lfaps[60] > 0.0)
		{
			flag_surface1[2] = 1;
			flag_surface1[4] = 1;
			flag_surface1[8] = 1;
		}

		if(FAPs->lfaps[55] < 0.0 ||FAPs->lfaps[59] > 0.0)
		{
			flag_surface1[1] = 1;
			flag_surface1[3] = 1;
			flag_surface1[7] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_61_62()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[9][2][0];
	ref_point1[1] = FDPfp[9][2][1];
	ref_point1[2] = FDPfp[9][2][2];

	ref_point2[0] = FDPfp[9][1][0];
	ref_point2[1] = FDPfp[9][1][1];
	ref_point2[2] = FDPfp[9][1][2];

	//FAP 9.2: interessa le superfici (destre) : 3, 4
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[61] || FAPs->lfaps[62])
	{
		for(i=3; i < 5; i++)
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 + 
							skin_point_2 * skin_point_2
							);

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(FAPs->lfaps[62] && (i%2==0))
						skin_point_i_j->increment[0] += -FAPs->lfaps[62]*temp1;
						
					if(FAPs->lfaps[61] && (i%2==1))
						skin_point_i_j->increment[0] += FAPs->lfaps[61]*temp1; 
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[62])
		{
			flag_surface1[4] = 1;
		}

		if(FAPs->lfaps[61])
		{
			flag_surface1[3] = 1;
		}
	}
}






void ApplyFAPs::apply_fap_63_64()
{	//FINISH !!!
	int i,j;
	float ref_point1[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[9][3][0];
	ref_point1[1] = FDPfp[9][3][1];
	ref_point1[2] = FDPfp[9][3][2];

	//FAP 9.3: interessa le superfici (destre) : 4
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[63] || FAPs->lfaps[64])
	{
		for(i=3; i < 5; i++)
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
				skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
				skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 / 4 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 1.5;

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(i%2==0)
					{
						skin_point_i_j->increment[0] += -FAPs->lfaps[64]*temp1;
						skin_point_i_j->increment[1] += FAPs->lfaps[63]*temp1;
					}
			
					if(skin_point_i_j->coordinate[0] != 0.0 && (i%2==1))
					{
						skin_point_i_j->increment[0] += -FAPs->lfaps[64]*temp1; 
						skin_point_i_j->increment[1] += FAPs->lfaps[63]*temp1;						
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[63] || FAPs->lfaps[64])
		{
			flag_surface1[4] = 1;
			flag_surface1[3] = 1;
		}
	}
}





void ApplyFAPs::apply_fap_65_66()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[10][2][0]; 
	ref_point1[1] = FDPfp[10][2][1]; 
	ref_point1[2] = FDPfp[10][2][2];

	ref_point2[0] = FDPfp[10][1][0]; 
	ref_point2[1] = FDPfp[10][1][1]; 
	ref_point2[2] = FDPfp[10][1][2];

	//FAP 10.2: interessa le superfici (destre) : 62
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[65] || FAPs->lfaps[66])
	{  
		for(i=61; i < 63; i++)
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 * 8 + 
							skin_point_1 * skin_point_1 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 4.0;

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(FAPs->lfaps[66] && (i%2==0))
						skin_point_i_j->increment[1] += FAPs->lfaps[66]*temp1;

					if(FAPs->lfaps[65] && (i%2==1))
						skin_point_i_j->increment[1] += FAPs->lfaps[65]*temp1; 
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[66])
		{
			flag_surface1[61] = 1;
			flag_surface1[62] = 1;
		}

		if(FAPs->lfaps[65])
		{
			flag_surface1[61] = 1;
			flag_surface1[62] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_67_68()
{	//FINISH !!!
	int i,j;
	float ref_point1[3],  ref_point2[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[10][4][0]; 
	ref_point1[1] = FDPfp[10][4][1]; 
	ref_point1[2] = FDPfp[10][4][2];

	ref_point2[0] = FDPfp[10][3][0]; 
	ref_point2[1] = FDPfp[10][3][1]; 
	ref_point2[2] = FDPfp[10][3][2];

	//FAP 10.2: interessa le superfici (destre) : 62
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[67] || FAPs->lfaps[68])
	{  
		for(i=61; i < 63; i++)
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 * 8 + 
							skin_point_1 * skin_point_1 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 4.0;				

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(FAPs->lfaps[68] && (i%2==0))
						skin_point_i_j->increment[0] += -FAPs->lfaps[68]*temp1;

					if(FAPs->lfaps[67] && (i%2==1))
						skin_point_i_j->increment[0] += FAPs->lfaps[67]*temp1; 
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[68])
		{
			flag_surface1[61] = 1;
			flag_surface1[62] = 1;
		}

		if(FAPs->lfaps[67])
		{
			flag_surface1[61] = 1;
			flag_surface1[62] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_19_20()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3], aux_point[3]; 
	float dist=0, temp1=0.0, weight=1.0;
	float center1[3],center2[3];
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	center1[0]=7.2;
	center1[1]=0.05;
	center1[2]=11.05;
	center2[0]=-7.2;
	center2[1]=0.05;
	center2[2]=11.05;
	
	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[3][2][0];
	ref_point1[1] = FDPfp[3][2][1];
	ref_point1[2] = FDPfp[3][2][2];

	ref_point2[0] = FDPfp[3][1][0];
	ref_point2[1] = FDPfp[3][1][1];
	ref_point2[2] = FDPfp[3][1][2];

	//FAP 3.1: interessa le superfici (sinistre) : 9, 11
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[19] || FAPs->lfaps[20])
	{
		for(i=9; i < 13; i++)
		{
			if (i==11 || i==12)
				weight=0.4;
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 5.3;
				
				if ( dist < M_PI/4 )
				{
					temp1 =3.41*(cos(dist)-RAC_2/2);

					if(FAPs->lfaps[19] && (i%2==1))
						skin_point_i_j->increment[1] += -weight*FAPs->lfaps[19]*temp1; 

					if(FAPs->lfaps[20] && (i%2==0))
						skin_point_i_j->increment[1] += -weight*FAPs->lfaps[20]*temp1;			
				}
			}
			weight=1.0;
		}

		for(i=9; i < 13; i++)
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				aux_point[0] = skin_point_i_j->coordinate[0] + skin_point_i_j->increment[0];
				aux_point[1] = skin_point_i_j->coordinate[1] + skin_point_i_j->increment[1];
				aux_point[2] = skin_point_i_j->coordinate[2] + skin_point_i_j->increment[2];

				ref_point1[1] = center1[1];
				ref_point2[1] = center2[1];

				if(i%2==0)
				{
					skin_point_0 = aux_point[0] - ref_point1[0];
					skin_point_1 = aux_point[1] - ref_point1[1];
					skin_point_2 = aux_point[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = aux_point[0] - ref_point2[0];
					skin_point_1 = aux_point[1] - ref_point2[1];
					skin_point_2 = aux_point[2] - ref_point2[2];
				}

				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 * 4 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 5.0;

				if ( dist < M_PI/4 )
				{
					temp1 =3.41*(cos(dist)-RAC_2/2);

					if(FAPs->lfaps[19] && (i%2==1))
						skin_point_i_j->increment[2] += 0.4*temp1*FAPs->lfaps[19]/(1+fabs(FAPs->lfaps[19])); 

					if(FAPs->lfaps[20] && (i%2==0))
						skin_point_i_j->increment[2] += 0.4*temp1*FAPs->lfaps[20]/(1+fabs(FAPs->lfaps[20]));
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[19])
		{
			flag_surface1[9] = 1;
			flag_surface1[11] = 1;
		}

		if(FAPs->lfaps[20])
		{
			flag_surface1[10] = 1;
			flag_surface1[12] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_21_22()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3], aux_point[3]; 
	float dist=0, temp1=0.0, weight=1.0;
	float center1[3],center2[3];
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;
	
	center1[0]=7.2;
	center1[1]=0.05;
	center1[2]=11.05;
	center2[0]=-7.2;
	center2[1]=0.05;
	center2[2]=11.05;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[3][4][0];
	ref_point1[1] = FDPfp[3][4][1];
	ref_point1[2] = FDPfp[3][4][2];

	ref_point2[0] = FDPfp[3][3][0];
	ref_point2[1] = FDPfp[3][3][1];
	ref_point2[2] = FDPfp[3][3][2];

	//FAP 3.1: interessa le superfici (sinistre) :5, 7
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[21] || FAPs->lfaps[22])
	{
		for(i=5; i < 9; i++)
		{
				if (i==7 || i==8)
					weight=0.4;
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					if(i%2==0)
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
						skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
						skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
					}
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 5.3;

					if ( dist < M_PI/4 )
					{
						temp1 =3.41*(cos(dist)-RAC_2/2);

						if(FAPs->lfaps[21] && (i%2==1))
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[21]*temp1; 

						if(FAPs->lfaps[22] && (i%2==0))
							skin_point_i_j->increment[1] += weight*FAPs->lfaps[22]*temp1; 
					}
				}
				weight=1.0;
		}

		for(i=5; i < 9; i++)
		{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					aux_point[0] = skin_point_i_j->coordinate[0] + skin_point_i_j->increment[0];
					aux_point[1] = skin_point_i_j->coordinate[1] + skin_point_i_j->increment[1];
					aux_point[2] = skin_point_i_j->coordinate[2] + skin_point_i_j->increment[2];

					ref_point1[1] = center1[1];
					ref_point2[1] = center2[1];

					if(i%2==0)
					{
						skin_point_0 = aux_point[0] - ref_point1[0];
						skin_point_1 = aux_point[1] - ref_point1[1];
						skin_point_2 = aux_point[2] - ref_point1[2];
					}
					else
					{
						skin_point_0 = aux_point[0] - ref_point2[0];
						skin_point_1 = aux_point[1] - ref_point2[1];
						skin_point_2 = aux_point[2] - ref_point2[2];
					}

					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 4 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 5.0;					

					if ( dist < M_PI/4 )
					{
						temp1 =3.41*(cos(dist)-RAC_2/2);

						if(FAPs->lfaps[21] && (i%2==1))
							skin_point_i_j->increment[2] += 0.4*temp1*FAPs->lfaps[21]/(1+fabs(FAPs->lfaps[21])); 

						if(FAPs->lfaps[22] && (i%2==0))
							skin_point_i_j->increment[2] += 0.4*temp1*FAPs->lfaps[22]/(1+fabs(FAPs->lfaps[22])); 
					}
				}			
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[21])
		{
			flag_surface1[5] = 1;
			flag_surface1[7] = 1;
		}

		if(FAPs->lfaps[22])
		{
			flag_surface1[6] = 1;
			flag_surface1[8] = 1;
		}
	}
}




void ApplyFAPs::apply_fap_29_30()
{	//CORPS DE LA FONCTION NON CHANGE : CONCERNE LES PUPILLES
	int i,j;
	float ref_point1[3], alpha; 
	float center[3]={7.2, 0.05, 11.05}; //centro dell'occhio sinistro
	
	pointtype* misc_point_i_j;
	float misc_point_0;
	float misc_point_1;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = center[0]; 
	ref_point1[1] = center[1];
	ref_point1[2] = center[2];

	//FAP 3.1: interessa le superfici (sinistre) :3 (dei misc_point)
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[29] || FAPs->lfaps[30])
	{
		for(i=3; i < 5; i++)
		{
			if(i==3)
			{
				for(j=0;surface_list2[i][j]!=-1;j++)
				{
					misc_point_i_j = &misc_point[surface_list2[i][j]];
					misc_point_0 = misc_point_i_j->coordinate[0] - ref_point1[0];
					misc_point_1 = misc_point_i_j->coordinate[1] - ref_point1[1];
					if(FAPs->lfaps[29])
					{
						if(misc_point_0 >= 0)
							alpha = atan( misc_point_1 / misc_point_0 );
						else
							alpha = M_PI-atan( misc_point_1 / -misc_point_0 );
						misc_point_i_j->increment[0] +=(FAPs->lfaps[29]/2+0.3*0.4)*cos(alpha) - misc_point_0;
						misc_point_i_j->increment[1] +=(FAPs->lfaps[29]/2+0.3*0.4)*sin(alpha) - misc_point_1; 
					}
				}
			}

			if(i==4)
			{
				for(j=0;surface_list2[i][j]!=-1;j++)
				{
					if(FAPs->lfaps[30])
					{
						misc_point_i_j = &misc_point[surface_list2[i][j]];
						misc_point_0 = misc_point_i_j->coordinate[0] + ref_point1[0];
						misc_point_1 = misc_point_i_j->coordinate[1] - ref_point1[1];

						if(misc_point_0 >= 0)
							alpha = atan( misc_point_1 / misc_point_0 );
						else
							alpha = M_PI-atan( misc_point_1 / -misc_point_0 );
						misc_point_i_j->increment[0] +=(FAPs->lfaps[30]/2+0.3*0.4)*cos(alpha) - misc_point_0;
						misc_point_i_j->increment[1] +=(FAPs->lfaps[30]/2+0.3*0.4)*sin(alpha) - misc_point_1;
					}
				}
			}
		}
	}
}


void ApplyFAPs::apply_fap_43_44()
{	//CORPS DE LA FONCTION NON CHANGE : CONCERNE LA LANGUE
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	
	pointtype* misc_point_i_j;
	float misc_point_0;
	float misc_point_1;
	float misc_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[6][1][0]; 
	ref_point1[1] = FDPfp[6][1][1];	//potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point1[2] = FDPfp[6][1][2];

	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[43] || FAPs->lfaps[44]/* || FAPs->lfaps[45]*/)
	{
		for(i=11; i < 12; i++)
		{
			for(j=0;surface_list2[i][j]!=-1;j++)
			{
				misc_point_i_j = &misc_point[surface_list2[i][j]];
				misc_point_0 = misc_point_i_j->coordinate[0] - ref_point1[0];
				misc_point_1 = misc_point_i_j->coordinate[1] - ref_point1[1];
				misc_point_2 = misc_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							misc_point_0 * misc_point_0 + 
							misc_point_1 * misc_point_1 + 
							misc_point_2 * misc_point_2
							);   
				dist /= 2.7;

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					//misc_point_i_j->increment[0] += -FAPs->lfaps[43]*temp1;
					misc_point_i_j->increment[1] += FAPs->lfaps[43]*temp1;
					misc_point_i_j->increment[2] += FAPs->lfaps[44]*temp1;					
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[43] || FAPs->lfaps[44]/* || FAPs->lfaps[45]*/)
		{
			flag_surface2[11] = 1;
		}
	}
}


void ApplyFAPs::apply_fap_45()
{//CORPS DE LA FONCTION NON CHANGE : CONCERNE LA LANGUE
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	
	pointtype* misc_point_i_j;
	float misc_point_0;
	float misc_point_1;
	float misc_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[6][2][0]; 
	ref_point1[1] = FDPfp[6][2][1];	//potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point1[2] = FDPfp[6][2][2];

	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[45])
	{
		for(i=11; i < 12; i++)
		{
			for(j=0;surface_list2[i][j]!=-1;j++)
			{
				misc_point_i_j = &misc_point[surface_list2[i][j]];
				misc_point_0 = misc_point_i_j->coordinate[0] - ref_point1[0];
				misc_point_1 = misc_point_i_j->coordinate[1] - ref_point1[1];
				misc_point_2 = misc_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							misc_point_0 * misc_point_0 + 
							misc_point_1 * misc_point_1 + 
							misc_point_2 * misc_point_2
							);   
				dist /= 3.0;

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));
					misc_point_i_j->increment[1] += FAPs->lfaps[45]*temp1;					
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[45])
		{
			flag_surface2[11] = 1;
		}
	}
}



void ApplyFAPs::apply_fap_46()
{//CORPS DE LA FONCTION NON CHANGE : CONCERNE LA LANGUE
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	
	pointtype* misc_point_i_j;
	float misc_point_0;
	float misc_point_1;
	float misc_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[6][3][0]; 
	ref_point1[1] = FDPfp[6][3][1];	//potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point1[2] = FDPfp[6][3][2];

	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[46])
	{
		for(i=11; i < 12; i++)
		{
			for(j=0;surface_list2[i][j]!=-1;j++)
			{
				misc_point_i_j = &misc_point[surface_list2[i][j]];
				misc_point_0 = misc_point_i_j->coordinate[0] - ref_point1[0];
				misc_point_1 = misc_point_i_j->coordinate[1] - ref_point1[1];
				misc_point_2 = misc_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							misc_point_0 * misc_point_0 + 
							misc_point_1 * misc_point_1 + 
							misc_point_2 * misc_point_2 / 10000000
							);   
			
				if ( dist < M_PI )
				{
					temp1 = 0.5*(1+cos(dist));
					temp1 *= 15;

					if (FAPs->lfaps[46] > 0)
						misc_point_i_j->increment[1] += FAPs->lfaps[46]*temp1;
					else
						misc_point_i_j->increment[0] -= FAPs->lfaps[46]*temp1;					
				}

				/*if ( dist < M_PI/3 && FAPs->lfaps[46] > 0)
				{
					temp1 =0.5*(1+cos(dist));
					temp1 = temp1*1000;

					misc_point_i_j->increment[0] -= FAPs->lfaps[46]*temp1;					
				}*/
			}
		}

		//settiamo la superficie da aggiornare
		if(FAPs->lfaps[46])
		{
			flag_surface2[11] = 1;
		}
	}
}

void ApplyFAPs::apply_fap_47()
{//CORPS DE LA FONCTION NON CHANGE : CONCERNE LA LANGUE
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	
	pointtype* misc_point_i_j;
	float misc_point_0;
	float misc_point_1;
	float misc_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[6][4][0]; 
	ref_point1[1] = FDPfp[6][4][1];	//potrei spostare un po' il centro del centroide anche se poi non diventa mpg4 compliant
	ref_point1[2] = FDPfp[6][4][2];

	//questo primo test si potrebbe anche portare fuori della funzione!!!
	if(FAPs->lfaps[47])
	{
		for(i=11; i < 12; i++)
		{
			for(j=0;surface_list2[i][j]!=-1;j++)
			{
				misc_point_i_j = &misc_point[surface_list2[i][j]];
				misc_point_0 = misc_point_i_j->coordinate[0] - ref_point1[0];
				misc_point_1 = misc_point_i_j->coordinate[1] - ref_point1[1];
				misc_point_2 = misc_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							misc_point_0 * misc_point_0 + 
							misc_point_1 * misc_point_1 + 
							misc_point_2 * misc_point_2 / 10000000
							);   
			
				if ( dist < M_PI )
				{
					temp1 = 0.5*(1+cos(dist));
					temp1 *= 10000;

					if (FAPs->lfaps[47] > 0)
						misc_point_i_j->increment[1] += FAPs->lfaps[47]*temp1;
					else
						misc_point_i_j->increment[0] += FAPs->lfaps[47]*temp1;					
				}

				/*if ( dist < M_PI/3 && FAPs->lfaps[47] > 0)
				{
					temp1 =0.5*(1+cos(dist));
					temp1 = temp1*1000;

					misc_point_i_j->increment[0] -= FAPs->lfaps[47]*temp1;					
				}*/
			}
		}

		//settiamo la superficie da aggiornare
		if(FAPs->lfaps[47])
		{
			flag_surface2[11] = 1;
		}
	}
}



void ApplyFAPs::prot(float val, int opt)
{	// FINISH !!!
	// Upper Lip Protrusion
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0, temp1=0.0;
	
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][6][0];
	ref_point1[1] = FDPfp[8][6][1];
	ref_point1[2] = FDPfp[8][6][2];

	ref_point2[0] = FDPfp[8][5][0];
	ref_point2[1] = FDPfp[8][5][1];
	ref_point2[2] = FDPfp[8][5][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 38, 46, 48, 50, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	for(i=33; i < 53; i++)
	{
		if((i>=33 && i<=38) || (i>=45 && i<=52))
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 1.5;				

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if (opt == 0 && (i%2==0))
						skin_point_i_j->increment[0] += -0.2*val*temp1;
					if(i%2==0)
						skin_point_i_j->increment[1] += -0.1*val*temp1;

					if (opt == 0 && (i%2==1))
						skin_point_i_j->increment[0] += 0.2*val*temp1;
					if (skin_point_i_j->coordinate[0] != 0.0 && (i%2==1))
						skin_point_i_j->increment[1] += -0.1*val*temp1;
					
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[56])
		{
			flag_surface1[2] = 1;
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[46] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
			//piccola parte sinistra
			flag_surface1[33] = 1;
			flag_surface1[45] = 1;

		}

		if(FAPs->lfaps[55])
		{
			flag_surface1[1] = 1;
			flag_surface1[33] = 1;
			flag_surface1[35] = 1;
			flag_surface1[37] = 1;
			flag_surface1[45] = 1;
			flag_surface1[47] = 1;
			flag_surface1[49] = 1;
			flag_surface1[51] = 1;
			flag_surface1[57] = 1; //e' in piu'
			//piccola parte destra
			flag_surface1[34] = 1;
			flag_surface1[46] = 1;

		}
	}
}



void ApplyFAPs::prot2(float val, int opt)
{
	int i,j;
	float ref_point1[3], ref_point2[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][8][0];
	ref_point1[1] = FDPfp[8][8][1];
	ref_point1[2] = FDPfp[8][8][2];

	ref_point2[0] = FDPfp[8][7][0];
	ref_point2[1] = FDPfp[8][7][1];
	ref_point2[2] = FDPfp[8][7][2];

	//FAP 8.8: interessa le superfici (destre) : 40, 42, 44, 54, 56, 58, 60
	//questo primo test si potrebbe anche portare fuori della funzione!!!
	for(i=39; i < 61; i++)
	{
		if((i>=39 && i<=44) || (i>=53 && i<=60))
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 * 1.5 + 
							skin_point_2 * skin_point_2
							);   
				dist /= 1.5;
				

				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(i%2==0)
						skin_point_i_j->increment[1] += 0.15*val*temp1;

					if (skin_point_i_j->coordinate[0] != 0.0)
						if(i%2==1)
							skin_point_i_j->increment[1] += 0.15*val*temp1;
					else if(i%2==1)
						skin_point_i_j->increment[1] += 0.25*val*temp1;
					
				}
			}
		}
	}

	ref_point1[0] = FDPfp[8][2][0];
	ref_point1[1] = FDPfp[8][2][1];
	ref_point1[2] = FDPfp[8][2][2];

	for(i=39; i < 61; i++)
	{
		if((i>=39 && i<=44) || (i>=51 && i<=60))
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
				skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
				skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							skin_point_0 * skin_point_0 / 3 +
							skin_point_1 * skin_point_1 * 2 + 
							skin_point_2 * skin_point_2
							);
				dist /= 1.5;
				
				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(i%2==0)
						skin_point_i_j->increment[1] += 0.4*val*temp1;

					if (skin_point_i_j->coordinate[0] != 0.0 && (i%2==1))
						skin_point_i_j->increment[1] += 0.4*val*temp1; 
				}
			}
		}
	}

	//settiamo le superfici da aggiornare a destra e sinistra
		flag_surface1[40] = 1;
		flag_surface1[42] = 1;
		flag_surface1[44] = 1;
		flag_surface1[54] = 1;
		flag_surface1[56] = 1;
		flag_surface1[58] = 1;
		flag_surface1[60] = 1;
		//piccola parte sinistra
		flag_surface1[43] = 1;
		flag_surface1[57] = 1;
		flag_surface1[59] = 1;

		flag_surface1[39] = 1;
		flag_surface1[41] = 1;
		flag_surface1[43] = 1;
		flag_surface1[53] = 1;
		flag_surface1[55] = 1;
		flag_surface1[57] = 1;
		flag_surface1[59] = 1;
		//piccola parte destra
		flag_surface1[44] = 1;
		flag_surface1[58] = 1;
		flag_surface1[60] = 1;
}


void ApplyFAPs::nouse()
{	//FINISH !!!
	int i,j;
	float ref_point1[3], ref_point2[3];
	float dist=0, temp1=0.0;
	
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[9][2][0]; 
	ref_point1[1] = FDPfp[9][2][1]; 
	ref_point1[2] = FDPfp[9][2][2];

	ref_point2[0] = FDPfp[9][1][0]; 
	ref_point2[1] = FDPfp[9][1][1]; 
	ref_point2[2] = FDPfp[9][1][2];

	if(FAPs->lfaps[55] || FAPs->lfaps[56])
	{
		for(i=3; i<5; i++)
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				if(i%2==0)
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				}
				else
				{
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point2[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point2[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point2[2];
				}
				dist = sqrt(
							skin_point_0 * skin_point_0 + 
							skin_point_1 * skin_point_1 + 
							skin_point_2 * skin_point_2
							);   
			
				if ( dist < M_PI )
				{
					temp1 =0.5*(1+cos(dist));

					if(FAPs->lfaps[56]<0 && (i%2==0))
						skin_point_i_j->increment[1] -= FAPs->lfaps[56]*0.5*temp1;

					if(FAPs->lfaps[55]<0 && (i%2==1))
						skin_point_i_j->increment[1] -= FAPs->lfaps[55]*0.5*temp1; 
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		if(FAPs->lfaps[62])
		{
			flag_surface1[4] = 1;
		}

		if(FAPs->lfaps[61])
		{
			flag_surface1[3] = 1;
		}
	}
}


void ApplyFAPs::press81(float val)
{	//FINISH !!!
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][1][0];
	ref_point1[1] = FDPfp[8][1][1];
	ref_point1[2] = FDPfp[8][1][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 46, 48, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!! 
	for(i=3; i < 53; i++)
	{
		if((i>=3 || i<4) || (i>=33 && i<=38) || (i>=45 && i<=52))
		{
			for(j=0;surface_list1[i][j]!=-1;j++)
			{
				skin_point_i_j = &skin_point[surface_list1[i][j]];
				skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
				skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
				skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
				dist = sqrt(
							skin_point_0 * skin_point_0 +
							skin_point_1 * skin_point_1 * 7 + 
							skin_point_2 * skin_point_2
							);
				dist /= 2.5;					

				if ( dist < M_PI )
				{
					temp1 = 0.5*(1+cos(dist));

					if(i%2==0)
						skin_point_i_j->increment[1] += -0.6*val*temp1;
					if ( skin_point[surface_list1[i][j]].coordinate[0] != 0.0 && (i%2==1))
						skin_point_i_j->increment[1] += -0.6*val*temp1;
				}
			}
		}
	}

	//settiamo le superfici da aggiornare a destra e sinistra
	flag_surface1[4] = 1;
	flag_surface1[34] = 1;
	flag_surface1[36] = 1;
	flag_surface1[38] = 1;
	flag_surface1[46] = 1;
	flag_surface1[48] = 1;
	flag_surface1[50] = 1;
	flag_surface1[52] = 1;
	//piccola parte sinistra
	flag_surface1[33] = 1;
	flag_surface1[45] = 1;

	flag_surface1[3] = 1;
	flag_surface1[33] = 1;
	flag_surface1[35] = 1;
	flag_surface1[37] = 1;
	flag_surface1[45] = 1;
	flag_surface1[47] = 1;
	flag_surface1[49] = 1;
	flag_surface1[51] = 1;
	//piccola parte destra
	flag_surface1[34] = 1;
	flag_surface1[46] = 1;
}


void ApplyFAPs::press85(float val)
{	//FINISH !!!
	int i,j;
	float ref_point1[3];
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][5][0];
	ref_point1[1] = FDPfp[8][5][1];
	ref_point1[2] = FDPfp[8][5][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 38, 46, 48, 50, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
		for(i=33; i < 53; i++)
		{
			if((i>=33 && i<=38) || (i>=45 && i<=52) && (i%2==1))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 1.5 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 1.5;

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));

						skin_point_i_j->increment[1] -= 0.3*val*temp1;
						if (val>0)
							skin_point_i_j->increment[2] -= 0.3*val*temp1;

						if (skin_point_i_j->coordinate[0] != 0.0)
							skin_point_i_j->increment[0] += 0.8*val*temp1;
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
		flag_surface1[33] = 1;
		flag_surface1[35] = 1;
		flag_surface1[37] = 1;
		flag_surface1[45] = 1;
		flag_surface1[47] = 1;
		flag_surface1[49] = 1;
		flag_surface1[51] = 1;
}


void ApplyFAPs::press86(float val)
{	//FINISH !!!
	int i,j;
	float ref_point1[3]; 
	float dist=0, temp1=0.0;
	pointtype* skin_point_i_j;
	float skin_point_0;
	float skin_point_1;
	float skin_point_2;

	float ***FDPfp = FDPs->featurePoints;

	ref_point1[0] = FDPfp[8][6][0];
	ref_point1[1] = FDPfp[8][6][1];
	ref_point1[2] = FDPfp[8][6][2];

	//FAP 8.6: interessa le superfici (destre) : 34, 36, 38, 46, 48, 50, 52
	//questo primo test si potrebbe anche portare fuori della funzione!!!
		for(i=33; i < 53; i++)
		{
			if((i>=33 && i<=38) || (i>=45 && i<=52) && (i%2==0))
			{
				for(j=0;surface_list1[i][j]!=-1;j++)
				{
					skin_point_i_j = &skin_point[surface_list1[i][j]];
					skin_point_0 = skin_point_i_j->coordinate[0] - ref_point1[0];
					skin_point_1 = skin_point_i_j->coordinate[1] - ref_point1[1];
					skin_point_2 = skin_point_i_j->coordinate[2] - ref_point1[2];
					dist = sqrt(
								skin_point_0 * skin_point_0 + 
								skin_point_1 * skin_point_1 * 1.5 + 
								skin_point_2 * skin_point_2
								);   
					dist /= 1.5;

					if ( dist < M_PI )
					{
						temp1 =0.5*(1+cos(dist));

						skin_point_i_j->increment[1] -= 0.3*val*temp1;
						if (val>0)
							skin_point_i_j->increment[2] -= 0.3*val*temp1;
						if (skin_point_i_j->coordinate[0] != 0.0)
							skin_point_i_j->increment[0] -= 0.8*val*temp1;
					}
				}
			}
		}

		//settiamo le superfici da aggiornare a destra e sinistra
			flag_surface1[34] = 1;
			flag_surface1[36] = 1;
			flag_surface1[38] = 1;
			flag_surface1[46] = 1;
			flag_surface1[48] = 1;
			flag_surface1[50] = 1;
			flag_surface1[52] = 1;
}

