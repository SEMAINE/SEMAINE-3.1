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


/* THIS FILE IS PART OF OPENSMILE (see license and header above!) */


/*

This program normalises (mean/variance) HTK feature files and computes normalisation data, which can be saved to file for use in openSMILE (component cVectorMVN)

USAGE: htk-standardize  <feature_file_list>  <output_file_list or - for analysis only (no output)>  [normdata_file to load or '-' or empty to save to default file 'normdata.dat']  [normdata file to save to if previous parameter was '-']

------

read in htk feature files in one directory or as file list given in a text file (each file on one line)
compute mean and variance 
go through filelist again and save normalised and standardised version of the files (optionally in a new directory)

*/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <strings.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>


typedef struct {
  uint32_t nSamples;
  uint32_t samplePeriod;
  uint16_t sampleSize;
  uint16_t parmKind;
} sHTKheader;
typedef sHTKheader * pHTKheader;

int IsVAXOrder ()
{
//	short x, *px;
	short x;
	unsigned char *pc;
//	px = &x;
//	pc = (unsigned char *) px;
	pc = (unsigned char *) &x;
	*pc = 1; *( pc + 1 ) = 0;			// store bytes 1 0
	return x==1;					// does it read back as 1?
}

void Swap32 ( uint32_t *p )
{
	uint8_t temp,*q;
	q = (uint8_t*) p;
	temp = *q; *q = *( q + 3 ); *( q + 3 ) = temp;
	temp = *( q + 1 ); *( q + 1 ) = *( q + 2 ); *( q + 2 ) = temp;
}

void Swap16 ( uint16_t *p )
{
	uint8_t temp,*q;
	q = (uint8_t*) p;
	temp = *q; *q = *( q + 1 ); *( q + 1 ) = temp;
}

void SwapFloat( float *p )
{
	uint8_t temp,*q;
	q = (uint8_t*) p;
	temp = *q; *q = *( q + 3 ); *( q + 3 ) = temp;
	temp = *( q + 1 ); *( q + 1 ) = *( q + 2 ); *( q + 2 ) = temp;
}

void prepareHeader( pHTKheader h )
{
  if ( IsVAXOrder () )
  {
    Swap32 ( &(h->nSamples) );
    Swap32 ( &(h->samplePeriod) );
    Swap16 ( &(h->sampleSize) );
    Swap16 ( &(h->parmKind) );
  }     
}

typedef struct {
  long dim;
  float *x;
} sVectorFloat;
typedef sVectorFloat * vectorFloat;

typedef struct {
  long dim;
  double *x;
} sVectorDouble;
typedef sVectorDouble * vectorDouble;

typedef struct {
  long dim;
  void *x;
} sVector;
typedef sVector * vector;

typedef struct {
  int nDim;
  long *dim;
  long long els;
  float *x;
} sMatrixFloat;
typedef sMatrixFloat * matrixFloat;

typedef struct {
  long rows, cols;
  long long els;
  float *x;
} sMatrix2DFloat;
typedef sMatrix2DFloat * matrix2DFloat;


inline vector vectorCreate(long dim, int elSize) // elSize : size of vector element in bytes
{
  vector ret = (vector)malloc(sizeof(sVector));
  if (ret == NULL) return NULL;
  ret->x = calloc(1,elSize*dim);
  ret->dim = dim;
  return ret;
}

inline vectorFloat vectorFloatCreate(long dim) 
{
  vectorFloat ret = (vectorFloat)malloc(sizeof(sVectorFloat));
  if (ret == NULL) return NULL;
  ret->x = (float *)calloc(1,sizeof(float)*dim);
  ret->dim = dim;
  return ret;
}

inline vectorDouble vectorDoubleCreate(long dim) 
{
  vectorDouble ret = (vectorDouble)malloc(sizeof(sVectorDouble));
  if (ret == NULL) return NULL;
  ret->x = (double *)calloc(1,sizeof(double)*dim);
  ret->dim = dim;
  return ret;
}

inline vectorFloat vectorFloatDestroy(vectorFloat vec)
{
  if (vec != NULL) {
    if (vec->x != NULL) free(vec->x);
    free(vec);
  }
  return NULL;       
}

inline vectorDouble vectorDoubleDestroy(vectorDouble vec)
{
  if (vec != NULL) {
    if (vec->x != NULL) free(vec->x);
    free(vec);
  }
  return NULL;       
}

matrixFloat matrixFloatCreate(int nDim, long *dim) 
{
  int i;
  matrixFloat ret = (matrixFloat)malloc(sizeof(sMatrixFloat));
  if (ret == NULL) return NULL;
  ret->els = 1;
  for (i=0; i<nDim; i++) {
    ret->els *= dim[i];    
  }
  ret->x = (float *)calloc(1,sizeof(float)*ret->els);
  ret->dim = dim;
  return ret;
}

matrix2DFloat matrix2DFloatCreate(long rows, long cols) 
{
  matrix2DFloat ret = (matrix2DFloat)malloc(sizeof(sMatrix2DFloat));
  if (ret == NULL) return NULL;
  ret->els = rows*cols;
  ret->rows = rows;
  ret->cols = cols;
  ret->x = (float *)calloc(1,sizeof(float)*ret->els);
  return ret;
}

matrix2DFloat matrix2DFloatDestroy(matrix2DFloat mat) 
{
  if (mat == NULL) return NULL;
  if (mat->x != NULL) free(mat->x);
  free(mat);
  return NULL;
}

/*
read a htk file and return contents as matrix
*/
matrix2DFloat readHTK(char *filename, uint32_t *samplePeriod, uint16_t *parmKind) 
{
  if (strlen(filename) <= 0) return 0;
  char *tmp = filename;
  while (tmp[0] == ' ') {
    tmp++;
  }
  if (strlen(tmp) <= 0) return 0;

  FILE *htk = fopen(filename, "rb");
  if (htk == NULL) {
    fprintf(stderr,"readHTK: error opening %s for reading\n",filename);
    return 0;
  }
  sHTKheader header;
  fread(&header,sizeof(header),1,htk);
  prepareHeader(&header);
  matrix2DFloat ret = matrix2DFloatCreate(header.nSamples, header.sampleSize/sizeof(float));
  if (ret == NULL) { fclose(htk); return NULL; }
  long bread = fread(ret->x,1,ret->els*sizeof(float),htk);
  if (bread < ret->els) {
    fprintf(stderr,"less bytes (%i) than expected (%i) read from %s\n",filename, bread, ret->els);
    fclose(htk); 
    return matrix2DFloatDestroy(ret);            
  }

  if (IsVAXOrder()) {
    float *tmp = ret->x; int i;
    for (i=0; i<ret->els; i++) {
      SwapFloat(tmp); tmp++;
    }
  }
  fclose(htk);
  if (samplePeriod != NULL) { *samplePeriod = header.samplePeriod; }
  if (parmKind != NULL) { *parmKind = header.parmKind; }    
  return ret;
}

/*
  write a matrix to a htk file and destroy matrix
*/
matrix2DFloat writeHTK(char *filename, matrix2DFloat ret, uint32_t samplePeriod, uint16_t parmKind) 
{
  if (ret == NULL) return 0;
  if (ret->x == NULL) return 0;
  
  FILE *htk = fopen(filename, "wb");
  if (htk == NULL) {
    fprintf(stderr,"error opening %s for writing\n",filename);
    return 0;
  }
  
  sHTKheader header;
  header.samplePeriod = samplePeriod;
  header.parmKind = parmKind;
  header.nSamples = ret->rows;
  header.sampleSize = ret->cols*sizeof(float);
  prepareHeader(&header);
  fwrite(&header,sizeof(header),1,htk);

  if (IsVAXOrder()) {
    float *tmp = ret->x; int i;
    for (i=0; i<ret->els; i++) {
      SwapFloat(tmp); tmp++;
    }
  }

  long wrote = fwrite(ret->x,1,ret->els*sizeof(float),htk);
  if (wrote < ret->els) {
    fprintf(stderr,"less bytes (%i) than expected (%i) written to %s! Disk full?\n",filename, wrote, ret->els);
    return ret;            
  }
  fclose(htk);
  return matrix2DFloatDestroy(ret);
}

inline float minFloat(float a, float b)
{
  if (a < b) return a;
  else return b;         
}

inline long minLong(long a, long b)
{
  if (a < b) return a;
  else return b;         
}

vectorFloat getMatrix2DFloatRow(matrix2DFloat mat, long row)
{
  if ((mat != NULL)&&(mat->x != NULL)) {
    vectorFloat r = vectorFloatCreate(mat->cols);
    if (r == NULL) return NULL;
    memcpy(r->x, mat->x + (row * (mat->cols)), (mat->cols) * sizeof(float));
    return r;
  }
  return NULL;
}

void setMatrix2DFloatRow(matrix2DFloat mat, long row, vectorFloat r)
{
  if ((mat != NULL)&&(mat->x != NULL)) {
    if (r == NULL) return;
    memcpy(mat->x + (row * (mat->cols)), r->x , (mat->cols) * sizeof(float));
  }
}

// add vector b onto vector a
void vectorFloatAdd(vectorFloat a, vectorFloat b)
{
  if ((a!= NULL)&&(a->x != NULL)&&(b!= NULL)&&(b->x != NULL)) {
    long i;
    long lng = minLong(a->dim, b->dim);
    for (i=0; i<lng; i++) {
      a->x[i] += b->x[i];    
    }
  }
}

// subtract vector b from vector a
void vectorFloatSub(vectorFloat a, vectorFloat b)
{
  if ((a!= NULL)&&(a->x != NULL)&&(b!= NULL)&&(b->x != NULL)) {
    long i;
    long lng = minLong(a->dim, b->dim);
    for (i=0; i<lng; i++) {
      a->x[i] -= b->x[i];    
    }
  }
}

// add vector b onto vector a
void vectorDoubleAdd(vectorDouble a, vectorDouble b)
{
  if ((a!= NULL)&&(a->x != NULL)&&(b!= NULL)&&(b->x != NULL)) {
    long i;
    long lng = minLong(a->dim, b->dim);
    for (i=0; i<lng; i++) {
      a->x[i] += b->x[i];    
    }
  }
}

// subtract vector b from vector a
void vectorDoubleSub(vectorDouble a, vectorDouble b)
{
  if ((a!= NULL)&&(a->x != NULL)&&(b!= NULL)&&(b->x != NULL)) {
    long i;
    long lng = minLong(a->dim, b->dim);
    for (i=0; i<lng; i++) {
      a->x[i] -= b->x[i];    
    }
  }
}

// divide elements in vector a by corresponding elements in vector b, save in a
void vectorFloatElemDiv(vectorFloat a, vectorFloat b)
{
  if ((a!= NULL)&&(a->x != NULL)&&(b!= NULL)&&(b->x != NULL)) {
    long i;
    long lng = minLong(a->dim, b->dim);
    for (i=0; i<lng; i++) {
      a->x[i] /= b->x[i];    
    }
  }
}

vectorDouble vectorFloatToVectorDouble(vectorFloat a)
{
  if ((a!= NULL)&&(a->x != NULL)) {
    long i;
    vectorDouble d = vectorDoubleCreate(a->dim);
    if (d==NULL) return NULL;
    for (i=0; i<a->dim; i++) {
      d->x[i] = (double)(a->x[i]);
    }
    return d;
  }
  return NULL;
}

vectorFloat vectorDoubleToVectorFloat(vectorDouble a)
{
  if ((a!= NULL)&&(a->x != NULL)) {
    long i;
    vectorFloat d = vectorFloatCreate(a->dim);
    if (d==NULL) return NULL;
    for (i=0; i<a->dim; i++) {
      d->x[i] = (float)(a->x[i]);
    }
    return d;
  }
  return NULL;
}

void vectorDoubleScalarDiv(vectorDouble vec, long long n)
{
  if ((vec != NULL)&&(vec->x != NULL)) {
    long i;
    double nD = (double)n;
    for (i=0; i<vec->dim; i++) {
      vec->x[i] /= nD;
    }         
  }
}

void vectorDoubleElemSqrt(vectorDouble vec)
{
  if ((vec != NULL)&&(vec->x != NULL)) {
    long i;
    for (i=0; i<vec->dim; i++) {
      vec->x[i] = sqrt(vec->x[i]);
    }         
  }
}

void vectorDoubleElemSqr(vectorDouble vec)
{
  if ((vec != NULL)&&(vec->x != NULL)) {
    long i;
    for (i=0; i<vec->dim; i++) {
      vec->x[i] = vec->x[i] * vec->x[i];
    }         
  }
}


vectorFloat columnSum (matrix2DFloat mat)
{
  long i,j;
  if ((mat != NULL)&&(mat->x != NULL)) {
    vectorFloat v = getMatrix2DFloatRow(mat,0);
    for(i=1; i<mat->rows; i++) {
       vectorFloat r = getMatrix2DFloatRow(mat,i);
       vectorFloatAdd(v,r);
       vectorFloatDestroy(r);
    }
    return v;
  }       
  return NULL;
}

vectorDouble columnVarianceSum (matrix2DFloat mat, vectorDouble means)
{
  long i,j;
  if ((mat != NULL)&&(mat->x != NULL)) {
    vectorFloat vf = getMatrix2DFloatRow(mat,0);
    vectorDouble v = vectorFloatToVectorDouble(vf);

    vf = vectorFloatDestroy(vf);
    for(i=1; i<mat->rows; i++) {
       vectorFloat r = getMatrix2DFloatRow(mat,i);
       vectorDouble rD = vectorFloatToVectorDouble(r);
       vectorFloatDestroy(r);
       vectorDoubleSub(rD,means);
       vectorDoubleElemSqr(rD);
       vectorDoubleAdd(v,rD);
       vectorDoubleDestroy(rD);
    }
    return v;
  }       
  return NULL;
}

void matRowsNormaliseMean (matrix2DFloat mat, vectorFloat mean)
{
  long i,j;
  if ((mat != NULL)&&(mat->x != NULL)) {
    for(i=0; i<mat->rows; i++) {
       vectorFloat r = getMatrix2DFloatRow(mat,i);
       vectorFloatSub(r,mean);
       setMatrix2DFloatRow(mat,i,r);
       vectorFloatDestroy(r);
    }
  }       
}

void matRowsStandardiseVariance (matrix2DFloat mat, vectorFloat stddev)
{
  long i,j;
  if ((mat != NULL)&&(mat->x != NULL)) {
    for(i=0; i<mat->rows; i++) {
       vectorFloat r = getMatrix2DFloatRow(mat,i);
       vectorFloatElemDiv(r,stddev);
       setMatrix2DFloatRow(mat,i,r);
       vectorFloatDestroy(r);
    }
  }       
}

    //  matRowsNormaliseMean(data,mean);
    //  matRowsStandardiseVariance(data,stddev);

int main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(stderr, "USAGE: %s  <feature_file_list>  <output_file_list or - for analysis only (no output)>  [normdata_file to load or '-' or empty to save to default file 'normdata.dat']  [normdata file to save to if previous parameter was '-']\n",argv[0]);
    return -1;
  }
  
  vectorDouble sum = NULL; 
  vectorDouble mean = NULL;
  vectorDouble stddev = NULL;
  vectorFloat meanF = NULL;
  vectorFloat stddevF = NULL;
  long long n = 0;
  char line[1000];
  FILE *flist;
  
  if ((argc <= 3)||((argv[3][0]=='-')&&(argv[3][1]==0))) { // no normdata file given, we must analyse the data
 
  printf("Analysing input data:\n  Mean... "); fflush(stdout);
  /*** mean ****/
  flist = fopen(argv[1],"r");
  while (!feof(flist)) {
    fgets(line,1000,flist); // get a line
    if (line[strlen(line)-1] == '\n') 
      line[strlen(line)-1] = 0; // strip \n character
    if (line[strlen(line)-1] == '\r') 
      line[strlen(line)-1] = 0; // strip \r character
    matrix2DFloat data = readHTK(line,NULL,NULL);
/*int ii;
for (ii=0; ii<data->rows*data->cols; ii++) {
if (isnan(data->x[ii])) printf("NAN NO\n");
}*/
    if (data != NULL) {
      if (sum == NULL) {
        sum = vectorDoubleCreate(data->cols);
      }
      n += data->rows;
      vectorFloat v = columnSum(data);
      vectorDouble d = vectorFloatToVectorDouble(v);
      vectorDoubleAdd(sum,d);
      vectorFloatDestroy(v);
      vectorDoubleDestroy(d);
      matrix2DFloatDestroy(data);
    }
  }
  fclose(flist);
  // compute means:
//printf("NO n %i\n",n);
  vectorDoubleScalarDiv(sum,n);
  mean = sum;
  
  printf("OK\n  Standard Deviation... "); fflush(stdout);
  /**** variance ****/
  vectorDouble var = NULL;
  flist = fopen(argv[1],"r");
  while (!feof(flist)) {
    fgets(line,1000,flist); // get a line
    if (line[strlen(line)-1] == '\n') 
      line[strlen(line)-1] = 0; // strip \n character
    if (line[strlen(line)-1] == '\r') 
      line[strlen(line)-1] = 0; // strip \r character
    matrix2DFloat data = readHTK(line,NULL,NULL);
    if (data != NULL) {
      if (var == NULL) {
        var = vectorDoubleCreate(data->cols);
      }
//      n += data->rows;
      vectorDouble d = columnVarianceSum(data,mean); // printf("var0d: %f (dim=%i)\n",d->x[0],d->dim);
      vectorDoubleAdd(var,d);
      vectorDoubleDestroy(d);
      matrix2DFloatDestroy(data);
    }
  }
  fclose(flist);
  // compute variance and stddev:
  vectorDoubleScalarDiv(var,n);
  stddev = var;
  vectorDoubleElemSqrt(stddev);
  
  printf("OK\n"); fflush(stdout);

  //printf("var0: %f (dim=%i)\n",var->x[0],var->dim);
  // save normdata file in current path 
  FILE *nd;
  if ((argc>4)&&(argv[3][0]=='-')&&(argv[3][1]==0)) {
    nd = fopen(argv[4],"wb");
    printf("Saving mean/std.dev. data to '%s'... ",argv[4]); fflush(stdout);
  } else {
    nd = fopen("normdata.dat","wb");  // save to default file
    printf("Saving mean/std.dev. data to normdata.dat... "); fflush(stdout);
  }
  if (nd != NULL) {
    /* check data we save... */
    int ii;
    for (ii=0; ii<stddev->dim; ii++) {
      if (!finite(stddev->x[ii])) { stddev->x[ii] = 0.0; printf(" NON-finite stddev value at index %i, zeroing\n",ii); }
      if (!finite(mean->x[ii])) { mean->x[ii] = 0.0; printf(" NON-finite mean value at index %i, zeroing\n",ii); }
    }
    fwrite(mean->x, sizeof(double)*mean->dim, 1, nd);
    fwrite(stddev->x, sizeof(double)*stddev->dim, 1, nd);
    fclose(nd);  
    printf("OK\n"); fflush(stdout);
  } else {
    fprintf(stderr,"ERROR saving normdata\n");       
  }

  meanF = vectorDoubleToVectorFloat(mean);
  stddevF = vectorDoubleToVectorFloat(stddev);

  } else { // load normdata file

  
    meanF = NULL;
  }
 
  if ((argv[2][0] == '-')&&(argv[2][1]==0)) {
    printf("Analyse only mode was selected (out-file list == '-'). Stopping at this point.\n");
    return 0;
  }

  printf("Transforming input data... "); fflush(stdout); 
  /*** normalise: first mean 0, then divide each sample by sqrt(variance) ***/
  flist = fopen(argv[1],"r");
  FILE *flistOut;
  if (argc > 2) { // output list specified
    flistOut = fopen(argv[2],"r");
  }
  uint32_t samplePeriod;
  uint16_t parmKind;
  while (!feof(flist)) {
    fgets(line,1000,flist); // get a line
    if (line[strlen(line)-1] == '\n') 
      line[strlen(line)-1] = 0; // strip \n character
    if (line[strlen(line)-1] == '\r') 
      line[strlen(line)-1] = 0; // strip \r character
    matrix2DFloat data = readHTK(line,&samplePeriod,&parmKind);
    if (data != NULL) {
      if (meanF == NULL) {
      if (argc > 3) {
        mean = vectorDoubleCreate(data->cols);
        stddev = vectorDoubleCreate(data->cols);
//  FILE *nd = fopen("normdata.dat","rb");
        FILE *nd = fopen(argv[3],"rb");
        if (nd != NULL) {
          fread(mean->x, sizeof(double)*mean->dim, 1, nd);
          fread(stddev->x, sizeof(double)*stddev->dim, 1, nd);
          fclose(nd);  
        } else {
          fprintf(stderr,"ERROR loading normdata\n");       
        }
        meanF = vectorDoubleToVectorFloat(mean);
        stddevF = vectorDoubleToVectorFloat(stddev);
      }
      }
      // do normalisation
      matRowsNormaliseMean(data,meanF);
      matRowsStandardiseVariance(data,stddevF);
      // save
      if (argc > 2) { // output list specified
        fgets(line,1000,flistOut); // get a line (output file)
        if (line[strlen(line)-1] == '\n') 
          line[strlen(line)-1] = 0; // strip \n character
        if (line[strlen(line)-1] == '\r') 
          line[strlen(line)-1] = 0; // strip \r character
      } // else: use input file
      writeHTK(line,data,samplePeriod,parmKind);
    }
  }

  fclose(flist);  
  
  printf("OK\n"); fflush(stdout);

  vectorDoubleDestroy(mean);  // at the end...
  vectorDoubleDestroy(stddev);  
  vectorFloatDestroy(meanF);  // at the end...
  vectorFloatDestroy(stddevF);  
}


