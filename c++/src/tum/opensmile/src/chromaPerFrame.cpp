/*F******************************************************************************
*
* openSMILE - open Speech and Music Interpretation by Large-space Extraction
*       the open-source Munich Audio Feature Extraction Toolkit
* Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
* This module has been created by Benedikt Gollan for ADMIRE
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

example of a vector processor

*/



#include <chromaPerFrame.hpp>

#define MODULE "cChromaPerFrame"

SMILECOMPONENT_STATICS(cChromaPerFrame)

SMILECOMPONENT_REGCOMP(cChromaPerFrame)
{
  SMILECOMPONENT_REGCOMP_INIT
    scname = COMPONENT_NAME_CCHROMAPERFRAME;
  sdescription = COMPONENT_DESCRIPTION_CCHROMAPERFRAME;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

    // if the inherited config type was found, we register our configuration variables
    SMILECOMPONENT_IFNOTREGAGAIN( {} // <- this is only to avoid compiler warnings...
  // name append has a special role: it is defined in cDataProcessor, and can be overwritten here:
  // if you set description to NULL, the existing description will be used, thus the following call can
  // be used to update the default value:
  ct->setField("nameAppend",NULL,"chromabasedfeature");
  ct->setField("frameSize","Size of frame from Framer",0);
  ct->setField("frameStep","Size of frame Step",0);
  ct->setField("Tracknumber","# of Track",0);
  ct->setField("HTM_SVM","Decide between Hard Template Matching and SVM for classification",1);
  ct->setField("Chord_Key","sending data to Chord or key recognition by libsvm 0 = chord 1 = key",1);

  // this is an example for adding an integer option:
  //ct->setField("inverse","1 = perform inverse FFT",0);
  )

    // The configType gets automatically registered with the config manger by the SMILECOMPONENT_IFNOTREGAGAIN macro

    // we now create out sComponentInfo, including name, description, success status, etc. and return that
    SMILECOMPONENT_MAKEINFO(cChromaPerFrame);
}

SMILECOMPONENT_CREATE(cChromaPerFrame)

//-----

cChromaPerFrame::cChromaPerFrame(const char *_name) :
cVectorProcessor(_name)
{

}

void cChromaPerFrame::fetchConfig()
{
  cVectorProcessor::fetchConfig();

  for (int i = 0; i < 12; i++)
  {
    key[i] = 0;
    chordmaj[i] = 0;
    chordmin[i] = 0;
    tone[i]=0;
  }
  for (int i=0;i<100;i++)
  {
    chord_svm_ch[i]=0;
  }
  chord_svm_p = chord_svm_ch;
  framedone = 1;
  filehandle = fopen("keyclassification.txt","w");
  n = 0;
  svm_idx=0;
  m = 0;
  HTM_SVM=0;
  HTM_SVM =getInt("HTM_SVM");
  curbar=0;
  p=0;
  l=0;
  loopnum = 0;
  for (int i = 0; i<4;i++)
  {
    count[i]=0;
  }
  for (int i=0;i<2000;i++)
  {
    for (int j=0; j<5; j++)
    {
      BeatPos[i][j]=0;
    }
  }

  Tracknumber = getInt("Tracknumber");
  frameSize = getDouble("frameSize");
  frameStep = getDouble("frameStep");
  Chord_Key = getInt("Chord_Key");

  //TODO get Chromavector

  //chroma = ...

  /*
  inverse = getInt("inverse");
  if (inverse) {
  SMILE_DBG(2,"transformFFT set for inverse FFT",inverse);
  inverse = 1;  // sign of exponent
  } else {
  inverse = -1; // sign of exponent
  }
  */
}

/*
int cExampleVectorProcessor::myConfigureInstance()
{
int ret=1;
ret *= cVectorProcessor::myConfigureInstance();
if (ret == 0) return 0;

//...


return ret;
}
*/

/*
int cChromaPerFrame::configureWriter(const sDmLevelConfig *c)
{

// you must return 1, in order to indicate configure success (0 indicated failure)
return 1;
}
*/


// optional

int cChromaPerFrame::setupNamesForField(int i, const char*name, long nEl)
{
  //namesAreSet=1;
  //int l = 0;
  //nChroma = nEl;
  //addNameAppendField(name,"beatindex",nEl); l+=nEl;

  //addNameAppendField(name,"beatindex",nEl); l+=nEl;
  //...
  //return n; 
  return cVectorProcessor::setupNamesForField(i,name,24); 
}


// use this to allocate data like filter coefficient arrays, etc...
void cChromaPerFrame::configureField(int idxi, long __N, long nOut)
{
  //     const sDmLevelConfig *c = reader->getLevelConfig();
  // idxc = getFconf(idxi);
}



/*
int cExampleVectorProcessor::myFinaliseInstance()
{
int ret=1;
ret *= cVectorProcessor::myFinaliseInstance();
//.....
return ret;
}
*/

// a derived class should override this method, in order to implement the actual processing
//int cChromaPerFrame::processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
//{
// do domething to data in *src, save result to *dst
// NOTE: *src and *dst may be the same...

//return 1;
//}

// a derived class should override this method, in order to implement the actual processing
int cChromaPerFrame::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  // do domething to data in *src, save result to *dst
  // NOTE: *src and *dst may be the same...


  if (framedone == 1) framelength = getnextFramelength();
  if (src!=NULL)
  {
    loopnum++;
    processedsamples = (long)floor((loopnum * frameStep + frameSize)*44100); //TODO variable Samplerate
    int i;
    double tone_power=0;
    double tone_per_frame_power=0;

    //sum up all chroma values and get power for normalization
    for (i=0;i<12;i++)
    {
      tone[i]+=src[i];
    }
    for (i=0;i<12;i++)
    {
      tone_power += tone[i]*tone[i];
    }

    double max = 0;
    indexkey = 0;
    indexkeycadence = 0;
    int indexchord = 0;

    //sum up alle chroma value regarding key pattern
    max = 0;
    for (i = 0; i< 12; i++)
    {
      key[i]= key[i] + src[i]+src[(i+2)%12]+ src[(i+4)%12]+src[(i+5)%12]+src[(i+7)%12]+src[(i+9)%12]+src[(i+11)%12];
    }

    double keyval[12];
    double keycadence[12];
    for (i = 0; i<12; i++)
    {
      keycadence[i] = key[i] + key[(i+5)%12] + key[(i+7)%12];		   
    }

    double keymax = 0;
    for (i = 0; i < 12; i++)
    {
      keyval[i] = key[i];
    }
    for (i = 0; i < 12; i++)
    {
      if (keyval[i] > keymax)
      {
        keymax = keyval[i];
        indexkey = i;
      }
    }

    double key_cadence_max= 0;
    for (i = 0; i < 12; i++)
    {
      if (keycadence[i] > key_cadence_max)
      {
        key_cadence_max = keycadence[i];
        indexkeycadence = i;
      }
    }

    //calc chords
    if (processedsamples < framelength)
    {
      for (i=0; i< 12; i++)
      {
        if (i== indexkey || i == ((indexkey+2)%12) ||i == ((indexkey+5)%12) || i == ((indexkey+7)%12) || i == ((indexkey+10)%12))
        {
          chordmaj[i] += src[i]+src[(i+4)%12]+src[(i+7)%12];

        }
        if (i == ((indexkey+2)%12) ||i == ((indexkey+4)%12) || i == ((indexkey+7)%12) || i == ((indexkey+9)%12)|| i == ((indexkey+11)%12))
        {
          chordmin[i] += src[i]+src[(i+3)%12]+src[(i+7)%12];
        }
        tone_per_frame[i]+= src[i];

      }
      return 0;
    }
    else
    {
      for (i=0;i<12;i++)
      {
        tone_per_frame_power += tone_per_frame[i]*tone_per_frame[i];
      }
      for (i = 0; i < 12; i++)
      {
        dst[i] = (FLOAT_DMEM)(tone[i]/sqrt(tone_power));
      }
      int n=0;
      for (i = 12; i < 24; i++)
      {
        dst[i] = (FLOAT_DMEM)(tone_per_frame[n]/sqrt(tone_per_frame_power));
        n++;
      }


      for (i=0;i<12;i++)
      {
        tone_per_frame[i]=0;  
      }
      tone_per_frame_power=0;


      for (i=0; i<12; i++)
      {
        if (chordmaj[i] > max) 
        {
          max = chordmaj[i];
          indexchord = i;  
        }
        if (chordmin[i] > max) 
        {
          max = chordmin[i];
          indexchord = i+12;  
        }

        chordmaj[i] = 0;
        chordmin[i] = 0;
      }
      max = 0;
      int countmax=0;
      indexcount = 0;
      BeatPos[m][2]= indexchord;
      if (BeatPos[m-1][2] != BeatPos[m][2])
      {
        if (BeatPos[m][1] == 1)count[0]++;
        else if (BeatPos[m][1] == 2)count[1]++;
        else if (BeatPos[m][1] == 3)count[2]++;
        else if (BeatPos[m][1] == 4)count[3]++;
      }
      for (int j=0;j<4;j++)
      {
        if (count[j] > countmax)
        {
          countmax = count[j];
          indexcount = j;
        }
      }
      fprintf(filehandle, "\nBeat:%i, Key S:%i, Key C:%i	Chordch:%i	 %i,%i,%i,%i",m,indexkey,indexkeycadence,indexcount,count[0],count[1],count[2],count[3]);
      m++;
      framedone = 1;

    }

    //--------------------------------------------------

  }

  return 1;

}

cChromaPerFrame::~cChromaPerFrame()
{
  fclose(filehandle);
}

double cChromaPerFrame::getnextFramelength()
{
  if (BeatPos[l+1][1]==0) return 0;
  framelength = BeatPos[l+1][0];
  l++;
  framedone = 0;
  return framelength;
}

void cChromaPerFrame::print(int i,int key)
{
  /*	if (key == 0)
  {
  if (i == 0) SMILE_MSG(1,"recognized chord: A");
  if (i == 1) SMILE_MSG(1,"recognized chord: A#");
  if (i == 2) SMILE_MSG(1,"recognized chord: B");
  if (i == 3) SMILE_MSG(1,"recognized chord: C");
  if (i == 4) SMILE_MSG(1,"recognized chord: C#");
  if (i == 5) SMILE_MSG(1,"recognized chord: D");
  if (i == 6) SMILE_MSG(1,"recognized chord: D#");
  if (i == 7) SMILE_MSG(1,"recognized chord: E");
  if (i == 8) SMILE_MSG(1,"recognized chord: F");
  if (i == 9) SMILE_MSG(1,"recognized chord: F#");
  if (i == 10) SMILE_MSG(1,"recognized chord: G");
  if (i == 11) SMILE_MSG(1,"recognized chord: G#");
  if (i == 12) SMILE_MSG(1,"recognized chord: am");
  if (i == 13) SMILE_MSG(1,"recognized chord: a#m");
  if (i == 14) SMILE_MSG(1,"recognized chord: bm");
  if (i == 15) SMILE_MSG(1,"recognized chord: cm");
  if (i == 16) SMILE_MSG(1,"recognized chord: c#m");
  if (i == 17) SMILE_MSG(1,"recognized chord: dm");
  if (i == 18) SMILE_MSG(1,"recognized chord: d#m");
  if (i == 19) SMILE_MSG(1,"recognized chord: em");
  if (i == 20) SMILE_MSG(1,"recognized chord: fm");
  if (i == 21) SMILE_MSG(1,"recognized chord: f#m");
  if (i == 22) SMILE_MSG(1,"recognized chord: gm");
  if (i == 23) SMILE_MSG(1,"recognized chord: g#m");

  }
  if (key == 1)
  {
  if (i == 0) SMILE_MSG(1,"recognized key: A");
  if (i == 1) SMILE_MSG(1,"recognized key: A#");
  if (i == 2) SMILE_MSG(1,"recognized key: B");
  if (i == 3) SMILE_MSG(1,"recognized key: C");
  if (i == 4) SMILE_MSG(1,"recognized key: C#");
  if (i == 5) SMILE_MSG(1,"recognized key: D");
  if (i == 6) SMILE_MSG(1,"recognized key: D#");
  if (i == 7) SMILE_MSG(1,"recognized key: E");
  if (i == 8) SMILE_MSG(1,"recognized key: F");
  if (i == 9) SMILE_MSG(1,"recognized key: F#");
  if (i == 10) SMILE_MSG(1,"recognized key: G");
  if (i == 11) SMILE_MSG(1,"recognized key: G#");
  }
  */
}
void cChromaPerFrame::update(int bar)
{

  if (Tracknumber == 1)
  {
    cComponentMessage KeyClass("KeyClassification");
    if (HTM_SVM == 0)
    {
      KeyClass.intData[0]=indexkey;
      KeyClass.intData[1]=(int)(BeatPos[bar][2]);
      KeyClass.intData[2]=bar;
      KeyClass.intData[3]=(int)(BeatPos[bar][1]);
      KeyClass.intData[4]=(int)(BeatPos[bar+1][2]);
    }
    else if (HTM_SVM == 1) 
    {
      KeyClass.intData[0]=(int)(BeatPos[bar][3]);
      KeyClass.intData[1]=(int)(BeatPos[bar][4]);
      KeyClass.intData[2]=bar;
      KeyClass.intData[3]=(int)(BeatPos[bar][1]);
      KeyClass.intData[4]=(int)(BeatPos[bar+1][4]);
    }
    cComponentMessage *KeyClassMessage = &KeyClass;
    sendComponentMessage("comLink",KeyClassMessage);


  }
  else if (Tracknumber == 2)
  {
    cComponentMessage KeyClass("KeyClassification2");
    if (HTM_SVM == 0)
    {
      KeyClass.intData[0]=indexkey;
      KeyClass.intData[1]=(int)(BeatPos[bar][2]);
      KeyClass.intData[2]=bar;
      KeyClass.intData[3]=(int)(BeatPos[bar][1]);
      KeyClass.intData[4]=(int)(BeatPos[bar+1][2]);

    }
    else if (HTM_SVM == 1) 
    {
      KeyClass.intData[0]=(int)(BeatPos[bar][3]);
      KeyClass.intData[1]=(int)(BeatPos[bar][4]);
      KeyClass.intData[2]=bar;
      KeyClass.intData[3]=(int)(BeatPos[bar][1]);
      KeyClass.intData[4]=(int)(BeatPos[bar+1][4]);
    }
    cComponentMessage *KeyClassMessage = &KeyClass;
    sendComponentMessage("comLink",KeyClassMessage);
  }

}
int cChromaPerFrame::processComponentMessage( cComponentMessage *_msg )
{
  if (isMessageType(_msg,"BeatPositionsforChroma"))  {
    BeatPos[n][0] = (_msg->floatData[0]);
    BeatPos[n][1] = (_msg->floatData[1]);
    n++;
  }
  else if (isMessageType(_msg,"BeatPositionsforChroma2"))  {
    BeatPos[n][0] = (_msg->floatData[0]);
    BeatPos[n][1] = (_msg->floatData[1]);
    n++;
  }

  else if (isMessageType(_msg,"SynchOutput"))  {
    curbar = (_msg->intData[0])-((int)(BeatPos[0][1])-1);
    if (curbar < 0) curbar =0;
    update(curbar);
  }
  else if (isMessageType(_msg,"classificationResult")) {  

    SMILE_IDBG(3,"received 'classificationResult' message");
    if (!strcmp(_msg->msgname,"SVMKeyResultTrack1")) 
    {	
      char strtmp[50];
      sprintf(strtmp,"%.2f",_msg->floatData[0]);
      std::string aroStr(strtmp);
      int result = int(atof(strtmp));
      BeatPos[svm_idx][3] = double(result);
      svm_idx++;

      if (result != BeatPos[svm_idx-2][3])
      {
        cComponentMessage KeyClass("KeyClassforChord");
        KeyClass.intData[0]=result;
        cComponentMessage *KeyClassMessage = &KeyClass;
        sendComponentMessage("SVMChordTrack1",KeyClassMessage);

      }
    }
    if (!strcmp(_msg->msgname,"SVMKeyResultTrack2")) 
    {	
      char strtmp[50];
      sprintf(strtmp,"%.2f",_msg->floatData[0]);
      std::string aroStr(strtmp);
      int result = int(atof(strtmp));
      BeatPos[svm_idx][3] = double(result);
      svm_idx++;

      if (result != BeatPos[svm_idx-2][3])
      {
        cComponentMessage KeyClass("KeyClassforChord2");
        KeyClass.intData[0]=result;
        cComponentMessage *KeyClassMessage = &KeyClass;
        sendComponentMessage("SVMChordTrack2",KeyClassMessage);

      }
    }


    if (!strcmp(_msg->msgname,"SVMChordResult1")) 
    {	
      SMILE_IDBG(3,"received 'classificationResult' message");
      char strtmp[50];
      strcpy(chord_svm_p,"");
      strcpy(chord_svm_p,_msg->msgtext);
      std::string aroStr(strtmp);
      int result = lookupindex(chord_svm_p);
      if (result >= 0 && result < 24)	BeatPos[svm_idx][4] =result;
      else SMILE_MSG(1,"Chord index out of bounds!!");
    }
    if (!strcmp(_msg->msgname,"SVMChordResult2")) 
    {	
      SMILE_IDBG(3,"received 'classificationResult' message");
      char strtmp[50];
      strcpy(chord_svm_p,"");
      strcpy(chord_svm_p,_msg->msgtext);
      std::string aroStr(strtmp);
      SMILE_MSG(1,"Track2 \n%s",chord_svm_p);
      int result = lookupindex(chord_svm_p);
      if (result >= 0 && result < 24)	BeatPos[svm_idx][4] =result;
      else SMILE_MSG(1,"Chord index out of bounds!!");
    }
  }
  return 0;
}
int cChromaPerFrame::lookupindex(char* ref)
{
  int result=-1;
  if (strcmp(ref,"A") == 0) result = 0;
  if (strcmp(ref,"A#") == 0) result = 1;
  if (strcmp(ref,"B") == 0) result = 2;
  if (strcmp(ref,"C") == 0) result = 3;
  if (strcmp(ref,"C#") == 0) result = 4;
  if (strcmp(ref,"D") == 0) result = 5;
  if (strcmp(ref,"D#") == 0) result = 6;
  if (strcmp(ref,"E") == 0) result = 7;
  if (strcmp(ref,"F") == 0) result = 8;
  if (strcmp(ref,"F#") == 0) result = 9;
  if (strcmp(ref,"G") == 0) result = 10;
  if (strcmp(ref,"G#") == 0) result = 11;
  if (strcmp(ref,"am") == 0) result = 12;
  if (strcmp(ref,"a#m") == 0) result = 13;
  if (strcmp(ref,"bm") == 0) result = 14;
  if (strcmp(ref,"cm") == 0) result = 15;
  if (strcmp(ref,"c#m") == 0) result = 16;
  if (strcmp(ref,"dm") == 0) result = 17;
  if (strcmp(ref,"d#m") == 0) result = 18;
  if (strcmp(ref,"em") == 0) result = 19;
  if (strcmp(ref,"fm") == 0) result = 20;
  if (strcmp(ref,"f#m") == 0) result = 21;
  if (strcmp(ref,"gm") == 0) result = 22;
  if (strcmp(ref,"g#m") == 0) result = 23;
  return result;
}
