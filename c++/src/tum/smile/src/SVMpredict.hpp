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
 
#ifndef __SVM_PREDICT_HPP
#define __SVM_PREDICT_HPP

#include "feature-memory.hpp"
#include "GENoutput.hpp"

/**************************************************************************/
/*********              LibSVM   addon:   scale functions  ****************/
/**************************************************************************/

struct svm_scale {
  int max_index;
  int y_scaling;
  double lower,upper;
  double y_lower,y_upper;
  double y_min,y_max;
  double *feature_max;
  double *feature_min;
};
struct svm_scale * svm_load_scale(const char* restore_filename);
void svm_destroy_scale(struct svm_scale *scale);
void svm_apply_scale(struct svm_scale *scale, struct svm_node * x);

/**************************************************************************/


class cSvmPredict : public cGenOutput {
  struct svm_model* model;
  struct svm_scale* scale;
  float result;
  int predict_probability;
  
  public:
    // _data is declared in base class cGenOutput !!
    
    cSvmPredict( cFeatureMemory &mem, const char * modelname );
    cSvmPredict( cFeatureMemory &mem, const char * modelname, const char *scalefile );

    float getLastResult() { return result; }
    
    //static int writeFrame( pGenOutput obj, pOutputVector vec );
    int processNextFrame();
        
    ~cSvmPredict ();
};


#endif // #ifndef __SVM_PREDICT_HPP
