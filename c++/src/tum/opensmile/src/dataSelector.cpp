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

selects data elements by element name

*/


#include <dataSelector.hpp>

#define MODULE "cDataSelector"

#define MAX_LINE_LENGTH 2048

SMILECOMPONENT_STATICS(cDataSelector)

SMILECOMPONENT_REGCOMP(cDataSelector)
{
  SMILECOMPONENT_REGCOMP_INIT

    scname = COMPONENT_NAME_CDATASELECTOR;
  sdescription = COMPONENT_DESCRIPTION_CDATASELECTOR;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("selFile","The name of the data selection file to load. The file is a text file containing one element name per line of the elements which should be selected (case-sensitive!). (Note: the first two lines make up a header; the first line always contains 'str', the second line is number of features in list, next each line contains one feature name of the features to select)",(const char*)NULL);
    ct->setField("selected","This is an alternative to loading 'selFile'. An array of exact (case-sensitive) names of features / data elements to select.",(const char *)NULL, ARRAY_TYPE);
    ct->setField("selectedRange","This is an alternative to loading 'selFile'. It specifies the selected elements(!) by element index ranges. Each array element contains one range in the format of <start>-<end>, where <start> and <end> are the indicies of the startt and end features to *select* in this range. '$' specifies the last element, and '1' indicates the first element. If the '-' is missing, only a single element will be selected, not a range. NOTE: the 'newNames' option does not work in conjunction with this element selection method!",(const char*)NULL,ARRAY_TYPE);
    ct->setField("newNames","An array of new names to assign to the selected features / data elements (optional). The order thereby corresponds to the order of data element names in the input.",(const char *)NULL, ARRAY_TYPE);
    ct->setField("elementMode","1 = select elements exactly as given in 'selected' array or in 'selFile' (in this case, only full element names are allowed (i.e. mfcc[1], mfcc[2] instead of mfcc, mfcc[], or mfcc[1-2]\n   0 = automatically copy arrays or partial arrays, e.g. if field[1-4] or only 'field' is given as name in the selction array/file, then the partial (1-4) or complete field will be copied to the output",1);
  )

    SMILECOMPONENT_MAKEINFO(cDataSelector);
}


SMILECOMPONENT_CREATE(cDataSelector)

//-----

cDataSelector::cDataSelector(const char *_name) :
cDataProcessor(_name),
selFile(NULL),
names(NULL),
mapping(NULL),
elementMode(0),
selectionIsRange(0),
idxSelected(NULL),
vecO(NULL)
{
}

int cDataSelector::loadSelection()
{
  if (selFile != NULL) {
    if (strlen(selFile)<1) return 0;

    FILE *f = fopen(selFile,"r");
    if (f== NULL) {
      SMILE_IERR(2,"error opening feature selection file '%s' for reading! NOT using a feature selection!",selFile);
      return 0;
    }

    // read first line to determine filetype:
    char line[MAX_LINE_LENGTH+1];
    long nStr=0;
    fgets( line, 5, f);
    line[3] = 0;
    if (!strcmp(line,"str")) { // string list
      fselType = 2;
      SMILE_IDBG(5,"reading string list of features");
      fscanf( f, "%u\n", &nStr);
      if (nStr < 1) { COMP_ERR("Error reading feature selection file, nFeatures < 1!"); }

      nSel = nStr;
      names = (char**)calloc(1,sizeof(char*) * nSel);

      int i=0; line[0] = 0;
      while(fgets(line,MAX_LINE_LENGTH,f) != NULL) {
        if (strlen( line ) > 1) { 
          if (i<nStr) {
            line[strlen( line ) - 1] = 0;
            names[i++] = strdup(line);
          } else { 
            SMILE_IERR(1,"excess line in feature selection file '%s' : '%s' (expected only %i lines with features)",selFile,line,nStr);
          }
        }
      }
      if (nSel > i) {
        SMILE_IWRN(1,"less feature names (only %i) in feature selection file '%s' than specified in the header (%i).",i,selFile,nSel);
        nSel = i;
      }
      /*
      while(fgets(line,MAX_LINE_LENGTH,f) != NULL) {
      if (strlen( line ) > 1) { 
      line[strlen( line ) - 1] = 0;
      names[i++] = strdup(line);
      }
      }*/
      SMILE_IDBG(5,"selected %i features",i);
      fclose(f);
      return 1;
    } else if (!strcmp(line,"idx")) { // index list
      fclose(f);
      SMILE_IERR(1,"feature index list not yet supported in dataSelector.");
      COMP_ERR("Aborting.");
      /*      
      fselType = 1;
      SMILE_IDBG(5,"reading index list of features");
      long idx=0; int i;
      // pass1: parse for max index
      while(fscanf(f,"%u\n",&idx) == 1)
      outputSelIdx.nFull = MAX(outputSelIdx.nFull, idx);
      SMILE_IDBG(5,"max index in selection file was found to be %i",outputSelIdx.nFull);
      outputSelIdx.nFull++;
      outputSelIdx.enabled = (long *)calloc(1,sizeof(long)*outputSelIdx.nFull);
      rewind( f );
      fgets(line, 5, f); // skip header line;
      // pass2: parse for max index
      i=0;
      while(fscanf(f,"%u\n",&idx) == 1) {
      outputSelIdx.enabled[idx] = 1; // enable feature "idx"
      i++;
      }
      outputSelIdx.nSel = i;
      Nsel = i;
      SMILE_IDBG(5,"enabled %i features",i);
      fclose(f);
      return 1;
      */
    } else { // bogus file...
      //TODO: read raw list of names??
      fclose( f );
      COMP_ERR("error parsing fselection file '%s'. bogus header! expected 'str' or 'idx' at beginning. found '%s'.",selFile,line);
    }
  }
  return 0;
}

void cDataSelector::fetchConfig()
{
  cDataProcessor::fetchConfig();

  selFile = getStr("selFile");
  if (selFile == NULL) {

    // load names of selected features:
    nSel = getArraySize("selected");
    if (nSel>0) {

      names = (char**)calloc(1,sizeof(char*) * nSel);

      int i;
      for (i=0; i<nSel; i++) {
        names[i] = (char *)getStr_f(myvprint("selected[%i]",i));
        SMILE_IDBG(2,"selected: '%s'",names[i]);
      }


    } else {
      // get feature selection by index ranges:
      nSel = getArraySize("selectedRange");
      if (nSel > 0) {
        names = (char**)calloc(1,sizeof(char*) * nSel);

        int i;
        for (i=0; i<nSel; i++) {
          names[i] = (char *)getStr_f(myvprint("selectedRange[%i]",i));
          SMILE_IDBG(2,"selectedRange: '%s'",names[i]);
        }
        selectionIsRange = 1;
      } else {
        SMILE_IERR(1,"no features selected, this does not make sense!");
        COMP_ERR("stopping here");
      }
    }
    if (selectionIsRange) { elementMode = 1; }
    else {
      elementMode = getInt("elementMode");
      if (elementMode) { SMILE_IDBG(2,"exact element name matching enabled"); }
    } 
  } else {
    SMILE_IDBG(2,"exact element name matching enabled");
    SMILE_IDBG(2,"loading feature selection from file '%s' (this forces elementMode=1 !).",selFile);
    elementMode = 1;
    loadSelection();
  }

}

/*
int cDataSelector::myConfigureInstance()
{
int ret=1;
// if you would like to change the write level parameters... do so HERE:

//
// .. or override configureWriter() to do so, after the reader config is available!
//
ret *= cDataProcessor::myConfigureInstance();
return ret;
}
*/

/*
int cDataSelector::configureWriter(const sDmLevelConfig *c)
{

// we must return 1, in order to indicate configure success (0 indicates failure)
return 1;
}
*/

// TODO!!!!!!!!!!   -> setupNewNames instead of finaliseInstance

// field selection -> fields (arrays) will be added as arrays in output
// element selection -> every output field will be one element
// how to choose the mode?? 
// --> a) if array index is given -> element selection
// --> b) if no array index for an array is given (field selection, select full field)
// --> c) if array index contains "-", assume range -> field selection (partial)

int cDataSelector::setupNewNames(long nEl)
{
  // match our list of selected names to the list of names obtained from the reader
  long i,j,n;
  if (elementMode) {
    long _N = reader->getLevelN();  
    nElSel = 0;
    if (selectionIsRange) {
      idxSelected = (long *)calloc(1,sizeof(long)*_N);
      for (i=0; i<nSel; i++) {
        long start, end;
        char * sep = strchr(names[i],'-');
        char *eptr=NULL;
        if (sep != NULL) { //range
          *sep = 0; 
          start = strtol(names[i],&eptr,10); if (eptr == names[i]) { SMILE_IERR(1,"parse error in config option 'selectedRange[%i]' : '%s' ! Expected a range <start>-<end>! Negative numbers are not allowed!",i,names[i]); }
          if (*(sep+1) == '$') {
            end = _N-1;
          } else {
            end = strtol(sep+1,&eptr,10); if (eptr == sep+1) { SMILE_IERR(1,"parse error in config option 'selectedRange[%i]' : '%s' ! Expected a range <start>-<end>! Negative numbers are not allowed!",i,names[i]); }
          }
        } else { // single number
          start = strtol(names[i],&eptr,10); if (eptr == names[i]) { SMILE_IERR(1,"parse error in config option 'selectedRange[%i]' : '%s' ! Expected a single integer number (or a range with '-')! Negative numbers are not allowed!",i,names[i]); }
          end = start;
        }
        if (start > end) { 
          SMILE_IERR(1,"start element index (%i) cannot be greater than end elemet index (%i) ! Setting start = %i",start,end,end);
          start = end;
        }
        if (end>=_N) {
          SMILE_IERR(1,"end element index (%i) out of range! Must be < %i. Setting end = %i",end,_N,_N-1);
          end = _N-1;
        }
        nElSel += end-start+1;
        for (j=start; j<=end; j++) {
          idxSelected[j] = 1;
        }
      }
      for (i=0; i<_N; i++) {
        if (idxSelected[i]) {
          char * nnn = reader->getElementName(i);
          if (nnn!=NULL) {
            SMILE_IMSG(3,"selected element %i, name '%s'\n",i,nnn);
            writer->addField(nnn,1);
            free(nnn);
          }
          // TODO: check for full array fields in range and add them as original array fields...??
        }
      }
    } else {
      mapping = (sDataSelectorSelData *)calloc(1,sizeof(sDataSelectorSelData) * nSel);
      int *isFound = (int*)calloc(1,sizeof(int)*nSel);
      for (n=0; n<_N; n++) {
        //  int found=0;
        //  SMILE_IDBG(4,"selstr %i of %i",i,outputSelStr.n);
        char * tmpname = reader->getElementName(n);
        for (i=0; i<nSel; i++) {
          if (!isFound[i]) {
            if (!strcmp(tmpname,names[i])) {
              mapping[i].eIdx = n;
              nElSel++; isFound[i]=1;
              break;
            }
          }
        }
        free(tmpname);
        if (nElSel >= nSel) break;
      }


    if (nElSel < nSel) {
      SMILE_IERR(1,"%i elements which were requested in element selection file were not found in current input data! please check openSMILE config! These elements are:",nSel-nElSel);
      for (i=0; i<nSel; i++) {
        if (isFound[i]==0) {
          SMILE_IERR(1,"  element '%s' ",names[i]);
        }
      }
      COMP_ERR("Your system will not run stable, due to data incompatibilities. Thus, we are aborting here!");
    } else {
 
      for (i=0; i<nSel; i++) {
        if (selFile == NULL) {
          const char *newname = getStr_f(myvprint("newNames[%i]",i));
          if (newname != NULL) {
            writer->addField(newname);
          } else {
            writer->addField(names[i]);  // if no newName is given, add old name
          }
        } else {
          writer->addField(names[i]);
        }
      }
      SMILE_IDBG(3,"mapping computed successfully");
    }
    free(isFound);
        }
    /*
    for (j=0; j<nSel; j++) {
    int found = 0;
    for (i=0; i<_N; i++) {
    char * tmp = reader->getElementName(i);
    if (!strcmp(tmp,names[j])) {
    // we found a match...
    found = 1;
    mapping[nElSel++].eIdx = i;
    if (selFile == NULL) {
    const char *newname = getStr_f(myvprint("newNames[%i]",j));
    if (newname != NULL) {
    writer->addField(newname);
    } else {
    writer->addField(tmp);  // if no newName is given, add old name
    }
    } else {
    writer->addField(tmp);
    }
    }
    free(tmp);
    }
    if (found==0) {
    SMILE_IERR(1,"element '%s' requested in selection, but not found in input elements! please check input config!",names[j]);
    }
    }
    SMILE_IDBG(2,"selected %i elements of %i requested elements",nElSel,nSel);
    */
  } else {

    // in non-element mode, create a list of expanded element names, then use the same code as in element mode
    int _N = reader->getLevelNf();  
    nElSel = 0; nFSel = 0;
    mapping = (sDataSelectorSelData *)calloc(1,sizeof(sDataSelectorSelData) * nSel);
    /// !!!!!!!!!!!!!!!!!!! TODO !!!!!!!!!!!!!!!!!!!!!!!
    //for (i=0; i<_N; i++) {
    for (j=0; j<nSel; j++) {

      // analyse type of field specified in selection, and extract fieldname (if array)
      const char *fieldname=NULL;
      int startIdx = 0, endIdx = 0, rangeGiven = 0;

      char *tmp2 = NULL;
      if (names[j] != NULL) {

        tmp2 = strdup(names[j]);
        char *ar = strchr(tmp2,'[');
        if (ar == NULL) { // is non-array field (no [] in name) or full field
          fieldname=names[j];
        } else {    // is array field ( with [] in name )
          *ar = 0;
          fieldname=tmp2;
          ar++;
          int len = (int)strlen(ar);

          if (len > 0) {
            ar[len-1] = 0;  // remove closing ]

            // look for element [x] OR range [x-y]
            char *rng = strchr(ar,'-');
            if (rng != NULL) { // is range
              *rng = 0; rng++;
              char *end=NULL;
              startIdx = strtol(ar, &end, 10); // convert to int
              if ((end != NULL)&&(end[0] != 0)) { // error, invalid chars
                SMILE_IERR(1,"invalid character in array start index in 'selected' field : '%s'\n",names[j]);
                COMP_ERR("stopping");
              }
              end=NULL;
              endIdx = strtol(rng, &end, 10); // convert to int
              if ((end != NULL)&&(end[0] != 0)) { // error, invalid chars
                SMILE_IERR(1,"invalid character in array end index in 'selected' field : '%s'\n",names[j]);
                COMP_ERR("stopping");
              }
              rangeGiven=1;
            } else { // is element in array field
              char *end=0;
              startIdx = strtol(ar, &end, 10); // convert to int
              if ((end != NULL)&&(end[0] != 0)) { // error, invalid chars
                SMILE_IERR(1,"invalid character in array index in 'selected' field : '%s'\n",names[j]);
                COMP_ERR("stopping");
              }
              endIdx = startIdx; rangeGiven=1;
            }

          }

        }
      }

      // check if field exists
      int __N=0;
      int arrNameOffset=0;
      int found = 0;
      int elIdx = 0;
      for (i=0; i<_N; i++) { // for each field
        const char *tmp = reader->getFieldName(i,&__N,&arrNameOffset);
        if (!strcmp(fieldname,tmp)) {
          // match found, ok
          found = 1;
          if (!rangeGiven)  { startIdx = 0; endIdx = __N-1; }
          else {
            startIdx -= arrNameOffset;
            endIdx -= arrNameOffset;
          }
          mapping[nFSel].fIdx = i;
          mapping[nFSel].eIdx = elIdx;
          break;
        }
        elIdx += __N;
      }



      // add to writer, if found
      if (!found) {
        // warn
        SMILE_IWRN(1,"field '%s' requested for selection, but not found in input!",names[j]);
      } else {
        // check range of start/end array index
        if (startIdx < 0) {
          SMILE_IWRN(1,"start index (%i) for selected field '%s' is out of range (must be > 0)! Setting start index to 0.",startIdx,names[j]);
          startIdx = 0;
        }
        else if (startIdx >= __N) {
          SMILE_IWRN(1,"start index (%i) for selected field '%s' is out of range (must be < %i)! Setting start index to %i.",startIdx,names[j],__N,__N-1);
          startIdx = __N-1;
        }

        if (endIdx < 0) {
          SMILE_IWRN(1,"end index (%i) for selected field '%s' is out of range (must be > 0)! Setting end index to 0.",endIdx,names[j]);
          endIdx = 0;
        }
        else if (endIdx >= __N) {
          SMILE_IWRN(1,"end index (%i) for selected field '%s' is out of range (must be < %i)! Setting end index to %i.",endIdx,names[j],__N,__N-1);
          endIdx = __N-1;
        }
        //add
        writer->addField(fieldname,endIdx-startIdx+1,arrNameOffset+startIdx);
        // set names in mapping, and set nElSel correct
        // TODO:::.
        //mapping[nElSel++].fIdx = i; // note: i is still valid from the last for() loop
        mapping[nFSel].aIdx = startIdx+mapping[nElSel].eIdx; // start index as element index
        mapping[nFSel++].N = endIdx-startIdx+1; // start index as element index
        nElSel += endIdx-startIdx+1;
      }

      if (tmp2 != NULL) free(tmp2);

    }


  }
  namesAreSet = 1;
  return nElSel;
}

/*
int cDataSelector::myFinaliseInstance()
{
return cDataProcessor::myFinaliseInstance();
}
*/

int cDataSelector::myTick(long long t)
{
  SMILE_DBG(4,"tick # %i, processing value vector",t);

  // get next frame from dataMemory
  cVector *vec = reader->getNextFrame();
  if (vec != NULL) {

    if (vecO == NULL) vecO = new cVector(nElSel, vec->type);
    long i;

    if (vec->type == DMEM_FLOAT) {

      if (elementMode) {
        if (selectionIsRange) {
          long j=0;
          for (i=0; i<vec->N; i++) {
            if (idxSelected[i]) { vecO->dataF[j++] = vec->dataF[i]; }
          }
        } else {
          for (i=0; i<nElSel; i++) {
            vecO->dataF[i] = vec->dataF[mapping[i].eIdx];
          }
        }
      } else {
        int j; long n=0;
        for (i=0; i<nFSel; i++) {
          for (j=0; j<mapping[i].N; j++) {
            vecO->dataF[n++] = vec->dataF[mapping[i].aIdx+j];
          }
        }
      }

    } else if (vec->type == DMEM_INT) {

      if (elementMode) {
        if (selectionIsRange) {
          long j=0;
          for (i=0; i<vec->N; i++) {
            if (idxSelected[i]) { vecO->dataI[j++] = vec->dataI[i]; }
          }
        } else {
          for (i=0; i<nElSel; i++) {
            vecO->dataI[i] = vec->dataI[mapping[i].eIdx];
          }
        }
      } else {
        int j; long n=0;
        for (i=0; i<nFSel; i++) {
          for (j=0; j<mapping[i].N; j++) {
            vecO->dataI[n++] = vec->dataI[mapping[i].aIdx+j];
          }
        }
      }

    }


    vecO->tmetaReplace(vec->tmeta);

    // save to dataMemory
    writer->setNextFrame(vecO);

    //   writer->setNextFrame(myVec);
    return 1;

  } 

  return 0;

}


cDataSelector::~cDataSelector()
{
  int i;
  if (vecO != NULL) delete(vecO);
  if (mapping!=NULL) free(mapping);
  if (idxSelected!=NULL) free(idxSelected);
  if (names!=NULL) {
    if (selFile != NULL) {
      for (i=0; i<nSel; i++) {
        if (names[i] != NULL) free(names[i]);  // memory allocated via strdup()
      }
    } // else we don't free the names, since they are allocated in the config manager!
    free(names); 
  }
}

