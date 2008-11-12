/*******************************************************************************
 * feaTUM, fast, efficient audio feature extractor by TUM
 * Copyright (C) 2008  Florian Eyben, Martin Woellmer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

This class saves features from featureMemory in CSV format

input can be joined from multiple levels in the featureMemory
However, the levels must have the same frameStep!

Merging data from asynchronus level might be supported in the future
 (then frames from levels with greater frameStep will be duplicated, etc.)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 

extern "C" {
#include "featum_common.h"
#include "featum_util.h"
}

#include "SVMpredict.hpp"
#include <stdio.h>
#include <malloc.h>
#include "../svm/svm.h"

cSvmPredict::cSvmPredict( cFeatureMemory &mem, const char * modelfilename, const char *scalefile ) : cGenOutput(mem), predict_probability(0) {

    scale = NULL;
    model = NULL;
    
    // load model
   	if((model=svm_load_model(modelfilename))==0)
	{
		FEATUM_ERROR_FATAL(1,"can't open libSVM model file %s\n",modelfilename);
		throw 1;
	}

    // load scale
   	if((scale=svm_load_scale(scalefile))==0)
	{
		FEATUM_ERROR_FATAL(1,"can't open scale file %s\n",scalefile);
		throw 1;
	}

}

cSvmPredict::cSvmPredict( cFeatureMemory &mem, const char * modelfilename ) : cGenOutput(mem), predict_probability(0) {

    scale = NULL;
    
    // load model
   	if((model=svm_load_model(modelfilename))==0)
	{
		FEATUM_ERROR_FATAL(1,"can't open libSVM model file %s\n",modelfilename);
		throw 1;
	}
    
    
}


int cSvmPredict::processNextFrame( ) {

    pOutputVector vec = getFrame();

    if (vec == NULL) _FUNCTION_RETURN_(0);
    if (vec->filled == 0) {
      FEATUM_WARNING(4,"vector not filled, cannot run SVM prediction on empty data!");
      _FUNCTION_RETURN_(0);
    }

/*	int correct = 0;
	int total = 0;
	double error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
    */
	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	double *prob_estimates=NULL;
	int j;
    struct svm_node *x = NULL;
    
    predict_probability = 0;
	if(predict_probability)
	{
		if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
			printf("Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n",svm_get_svr_probability(model));
		else
		{
			int *labels=(int *) malloc(nr_class*sizeof(int));
			svm_get_labels(model,labels);
			prob_estimates = (double *) malloc(nr_class*sizeof(double));
			printf("labels");		
			for(j=0;j<nr_class;j++)
				printf(" %d",labels[j]);
			printf("\n");
			free(labels);
		}
	}

		int i = 0;
		int c;
		double v;

                                          // need one more for index = -1
        x = (struct svm_node *) malloc( (vec->n + 1) * sizeof(struct svm_node));
        for (i=0; i<vec->n; i++) {
          x[i].index = i+1; // FIXME!!! +1 is ok??? (no!?)
          x[i].value = vec->data[i];
        }
		x[i].index = -1;
		x[i].value = 0.0;

        svm_apply_scale(scale,x);

        /*
        for (i=0; i<vec->n; i++) {
          printf("%i:%f ",i,x[i].value);
        }
        printf("\n");
        */
        
		if (predict_probability && (svm_type==C_SVC || svm_type==NU_SVC))
		{
			v = svm_predict_probability(model,x,prob_estimates);
			printf("%g",v);
			for(j=0;j<nr_class;j++)
				printf(" %g",prob_estimates[j]);
			printf("\n");
		}
		else
		{
			v = svm_predict(model,x);
			result = v;
			printf("%g\n",v);
		}
        free(x);
        
/*
	if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
	{
		printf("Mean squared error = %g (regression)\n",error/total);
		printf("Squared correlation coefficient = %g (regression)\n",
		       ((total*sumvy-sumv*sumy)*(total*sumvy-sumv*sumy))/
		       ((total*sumvv-sumv*sumv)*(total*sumyy-sumy*sumy))
		       );
	}
	else
		printf("Accuracy = %g%% (%d/%d) (classification)\n",
		       (double)correct/total*100,correct,total);
  */
	if(predict_probability)
		free(prob_estimates);    
    
    _FUNCTION_RETURN_(1);
}

cSvmPredict::~cSvmPredict() {
  svm_destroy_model(model);
  svm_destroy_scale(scale);
}



/**************************************************************************/
/*********              LibSVM   addon:   scale functions  ****************/
/**************************************************************************/

#include <float.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef __WINDOWS
#define max(x,y) (((x)>(y))?(x):(y))
#define min(x,y) (((x)<(y))?(x):(y))
#endif

/*
struct svm_scale {
  int max_index;
  double lower,upper;
  double y_lower,y_upper;
  double y_min,y_max;
  double *feature_max;
  double *feature_min;
};
*/
char *line = NULL;
int max_line_len = 1024;

char* readline(FILE *input)
{
	int len;
	
	if (line == NULL) line = (char *) calloc(1, max_line_len*sizeof(char));

	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line, max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

struct svm_scale * svm_load_scale(const char* restore_filename) 
{

  if (restore_filename==NULL) { return NULL; }

  int idx, c;
  FILE *fp_restore = NULL;
  struct svm_scale *ret = NULL;
  double fmin, fmax;
  double y_max = -DBL_MAX;
  double y_min = DBL_MAX;
  double y_lower, y_upper;

  fp_restore = fopen(restore_filename,"r");
  if(fp_restore==NULL)
  {
    fprintf(stderr,"can't open scale file %s\n", restore_filename);
    return NULL;
  }

  c = fgetc(fp_restore);
  if(c == 'y')
  {
    readline(fp_restore);
	readline(fp_restore);
	readline(fp_restore);
  }
  readline(fp_restore);
  readline(fp_restore);

  ret = (struct svm_scale *) calloc(1, sizeof(struct svm_scale) );
  if (ret == NULL)
  {
    fprintf(stderr,"Error allocating memory!\n");
    return NULL;
  }
  
  while(fscanf(fp_restore,"%d %*f %*f\n",&idx) == 1)
    ret->max_index = max(idx,ret->max_index);

  rewind(fp_restore);


  ret->feature_max = (double *)malloc((ret->max_index+1)* sizeof(double));
  ret->feature_min = (double *)malloc((ret->max_index+1)* sizeof(double));
  ret->feature_min[0] = 0.0;
  ret->feature_max[0] = 0.0;
  
  if((c = fgetc(fp_restore)) == 'y')
  {
    fscanf(fp_restore, "%lf %lf\n", &(ret->y_lower), &(ret->y_upper));
    fscanf(fp_restore, "%lf %lf\n", &(ret->y_min), &(ret->y_max));
    ret->y_scaling = 1;
  }
  else {
    ungetc(c, fp_restore);
    ret->y_scaling = 0;
  }

  ret->lower = -1.0;
  ret->upper = 1.0;
			
  if (fgetc(fp_restore) == 'x') {
     fscanf(fp_restore, "%lf %lf\n", &(ret->lower), &(ret->upper));
     while(fscanf(fp_restore,"%d %lf %lf\n",&idx,&fmin,&fmax)==3)
     {
       if(idx<=ret->max_index)
       {
         ret->feature_min[idx] = fmin;
         ret->feature_max[idx] = fmax;
       }
     }
  }
  fclose(fp_restore);    
  
  return ret;
}

void svm_destroy_scale(struct svm_scale *scale) 
{
  if (scale) {
    if (scale->feature_min) free(scale->feature_min);           
    if (scale->feature_max) free(scale->feature_max);
    free(scale);
  }     
}

void svm_apply_scale(struct svm_scale *scale, struct svm_node * x) 
{
  int i=0;
  if (scale == NULL) return;
  if (x == NULL) return;
  
  while(1) 
  {
    if (x[i].index == -1) break;
    
    if (x[i].index <= scale->max_index)
    {
      int index = x[i].index;
      double value = x[i].value;
      
      /* skip single-valued attribute */
      if(scale->feature_max[index] == scale->feature_min[index])
        { i++; continue; }

      if(value == scale->feature_min[index])
        value = scale->lower;
      else if(value == scale->feature_max[index])
		value = scale->upper;
      else
		value = scale->lower + (scale->upper - scale->lower) * 
			(value- scale->feature_min[index])/
			(scale->feature_max[index] - scale->feature_min[index]);
			
      x[i].value = value;

    }
    i++;
  }
  
}
     

/**************************************************************************/
