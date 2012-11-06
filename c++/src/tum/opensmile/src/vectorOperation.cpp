/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
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
 *
 ******************************************************************************E*/


/*  openSMILE component:

do elementary operations on vectors 
(i.e. basically everything that does not require history or context,
 everything that can be performed on single vectors w/o external data (except for constant parameters, etc.))

*/



#include <vectorOperation.hpp>
#include <cmath>

#define MODULE "cVectorOperation"


SMILECOMPONENT_STATICS(cVectorOperation)

SMILECOMPONENT_REGCOMP(cVectorOperation)
{
  SMILECOMPONENT_REGCOMP_INIT
  scname = COMPONENT_NAME_CVECTOROPERATION;
  sdescription = COMPONENT_DESCRIPTION_CVECTOROPERATION;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  // if the inherited config type was found, we register our configuration variables
  SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
    // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
 	  // if you set description to NULL, the existing description will be used, thus the following call can
	  // be used to update the default value:
    //ct->setField("nameAppend",NULL,"processed");
  ct->setField("operation","A string which specifies the type of operation to perform:\n   norm = normalise vector length (euclidean norm, L2) to 1\n   mul = multiply vector by param1\n   add = add param1 to each element\n   log = compute natural logarithm\n   lgA = compute logarithm to base param1\n   nl1 = normalise vector sum (L1 norm) to 1\n   sqrt = compute square root\n   pow = take values to the power of param1\n   exp = raise param1 to the power of the vector elements\n    ee = raise the base e to the power of the vector elements\n    abs = take absolute value of each element\n    agn = add Gaussian noise with mean param1 and std.dev. param2","norm");
  ct->setField("param1","parameter 1",1.0);
  ct->setField("param2","parameter 2",1.0);
  ct->setField("logfloor","floor for log operation",0.0000001);
  ct->setField("powOnlyPos","if 'operation' = 'pow', do not take negative values to the power of 'param1'; instead, output 0. This is necessary to avoid 'nan' values if the exponent is rational. ",0);
  )

  // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro
  
  // we now create out sComponentInfo, including name, description, success status, etc. and return that
  SMILECOMPONENT_MAKEINFO(cVectorOperation);
}

SMILECOMPONENT_CREATE(cVectorOperation)

//-----

cVectorOperation::cVectorOperation(const char *_name) :
  cVectorProcessor(_name),
  param1(0.0),
  param2(0.0)
{

}

void cVectorOperation::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  param1 = (FLOAT_DMEM)getDouble("param1");
  param2 = (FLOAT_DMEM)getDouble("param2");
  powOnlyPos = (int)getInt("powOnlyPos");

  const char * op = getStr("operation");
  operation=VOP_NORMALISE;
  if (!strncmp(op,"nor",3)) {
    operation=VOP_NORMALISE;
  } else if (!strncmp(op,"mul",3)) {
    operation=VOP_MUL;
  } else if (!strncmp(op,"add",3)) {
    operation=VOP_ADD;
  } else if (!strncmp(op,"log",3)) {
    operation=VOP_LOG;
  } else if (!strncmp(op,"lgA",3)) {
    operation=VOP_LOGA;
    if (param1 <= 0) {
      SMILE_IWRN(1,"log-base cannot be negative! setting base to exp(1) (-> natural logarithm)");
      param1 = (FLOAT_DMEM)std::exp(1.0);
    }
    if (param1 == 1.0) {
      SMILE_IWRN(1,"log-base cannot be 1.0! setting base to exp(1) (-> natural logarithm)");
      param1 = (FLOAT_DMEM)std::exp(1.0);
    }
  } else if (!strncmp(op,"nl1",3)) {
    operation=VOP_NORMALISE_L1;
  } else if (!strncmp(op,"sqr",3)) {
    operation=VOP_SQRT;
  } else if (!strncmp(op,"pow",3)) {
    operation=VOP_POW;
  } else if (!strncmp(op,"exp",3)) {
    operation=VOP_EXP;
  } else if (!strncmp(op,"ee",2)) {
    operation=VOP_E;
  } else if (!strncmp(op,"abs",3)) {
    operation=VOP_ABS;
  } else if (!strncmp(op,"agn",3)) {
    operation=VOP_AGN;
  } else {
    SMILE_IERR(1,"unknown operation '%s' specified in config file.",op);
  }


  logfloor = (FLOAT_DMEM)getDouble("logfloor");
  if (logfloor <= 0) { 
    SMILE_IWRN(1,"log-floor cannot be <= 0 ! setting to 0.00000000001 !");
    logfloor = (FLOAT_DMEM)0.00000000001; 
  }
}

// a derived class should override this method, in order to implement the actual processing
int cVectorOperation::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  
  return 1;
}

// a derived class should override this method, in order to implement the actual processing
int cVectorOperation::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...
  FLOAT_DMEM sum =0.0;
  long i;

  switch(operation) {

    case VOP_NORMALISE:

      // normalise L2 norm of all incoming vectors
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        sum+=src[i]*src[i];
      }
      if (sum <= 0.0) {
        sum = 1.0;
      }
      sum = sqrt(sum/(FLOAT_DMEM)(MIN(Nsrc,Ndst)));
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = src[i] / sum;
      }

      break;

    case VOP_NORMALISE_L1:

      // normalise L1 norm of all incoming vectors
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        sum+=src[i];
      }
      if (sum <= 0.0) {
        sum = 1.0;
      }
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = src[i] / sum;
      }

      break;

    case VOP_ADD:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = src[i] + param1;
      }
      break;

    case VOP_MUL:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = src[i] * param1;
      }
      break;

    case VOP_LOG:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        if (src[i]>logfloor) {
          dst[i] = std::log(src[i]);
        } else {
          dst[i] = std::log(logfloor);
        }
      }
      break;

    case VOP_LOGA:
      sum = std::log(param1);
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        if (src[i]>logfloor) {
          dst[i] = std::log(src[i]) / sum;
        } else {
          dst[i] = std::log(logfloor) / sum;
        }
      }
      break;

    case VOP_SQRT:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        if (src[i]>0.0) {
          dst[i] = std::sqrt(src[i]);
        } else {
          dst[i] = 0.0;
        }
      }
      break;

    case VOP_POW:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        if (powOnlyPos) {
          if (src[i]>0.0) {
            dst[i] = std::pow(src[i],param1);
          } else {
            dst[i] = 0.0;
          }
        } else {
          dst[i] = std::pow(src[i],param1);
        }
      }
      break;

    case VOP_EXP:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = std::pow(param1,src[i]);
      }
      break;
    
    case VOP_E:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = std::exp(src[i]);
      }
      break;

    case VOP_ABS:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = fabs(src[i]);
      }
      break;

    case VOP_AGN:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = src[i] + param2 * gnGenerator() + param1;
      }
      break;


    default:
      for (i=0; i<MIN(Nsrc,Ndst); i++) {
        dst[i] = src[i];
      }

  }
  return 1;
}


// Code contributed by Felix Weninger
double 
cVectorOperation::gnGenerator()
{
  static bool haveNumber = false;
  static double number = 0.0;
  if (haveNumber) {
    haveNumber = false;
    return number;
  }
  else {
    double q = 2.0;
    double x, y;
    // Use Polar Method to obtain normally distributed random numbers.
    while (q > 1.0) {
        x = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        y = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        q = x * x + y * y;
    }
    double z = -2.0 * log(q) / q;
    number = y * sqrt(z);
    haveNumber = true;
    return x * sqrt(z);
  }
}


cVectorOperation::~cVectorOperation()
{
}

