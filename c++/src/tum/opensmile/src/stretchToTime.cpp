/*******************************************************************************
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

example dataProcessor
writes data to data memory...

** you can use this as a template for custom dataProcessor components **

*/


#include <stretchToTime.hpp>
#undef class

#include <iostream>
#include <fstream>

#include "string.h"
#include <math.h>

#define MODULE "cStretchToTime"

SMILECOMPONENT_STATICS(cStretchToTime)

SMILECOMPONENT_REGCOMP(cStretchToTime)
{
  SMILECOMPONENT_REGCOMP_INIT

    scname = COMPONENT_NAME_CSTRETCHTOTIME;
  sdescription = COMPONENT_DESCRIPTION_CSTRETCHTOTIME;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

    SMILECOMPONENT_IFNOTREGAGAIN(
    //  ct->setField("offsetS","offset that this dummy component adds to the values",1.0);
    // ct->setField("offsetD","offset that this dummy component adds to the values",1.0);
    ct->setField("blocksize",NULL,2000);
  ct->setField("Synchmode","",0);
  ct->setField("Synch","Flag for PlayMode",1);
  ct->setField("startS","read start in seconds from beginning of file",0.0);
  ct->setField("startD","read start in seconds from beginning of file",0.0);
  ct->setField("OriginalSpeed1","read orginal Speed in BPM",0);
  ct->setField("ShiftBeatPos1","Shifting Beatpositions to real position",0);
  ct->setField("ShiftBeatPos2","Shifting Beatpositions to real position",0);
  ct->setField("Deceleration","0 for Repeat, 1 for Reverse",0);
  ct->setField("Halfbeat","",0);
  ct->setField("TrackID","set to 1 if STT Component is for Track1, 2 for Track 2",0);
  ct->setField("Tracknumber","indicates number of track for individual tasks",0);
  )

    SMILECOMPONENT_MAKEINFO(cStretchToTime);
}


SMILECOMPONENT_CREATE(cStretchToTime)

//-----

cStretchToTime::cStretchToTime(const char *_name) :
cDataProcessor(_name)
{

}

void cStretchToTime::fetchConfig()
{
  cDataProcessor::fetchConfig();

  // load all configuration parameters you will later require fast and easy access to here:

  synch = getInt("Synch");
  TrackID = getInt("TrackID");
  Tracknumber = getInt("Tracknumber");
  Synchmode = getInt("Synchmode");
  Deceleration = getInt("Deceleration");

  startS = -1*(getDouble("startS"));
  startD = -1*(getDouble("startD"));

  halfbeat = getInt("Halfbeat");
  org_speedS = getDouble("OriginalSpeed1");
  OffsetTrackS = getInt("ShiftBeatPos1");
  OffsetTrackD = getInt("ShiftBeatPos2");
  // note, that it is "polite" to output the loaded parameters at debug level 2:
  firstframe = 1;
  beatindexmat = 0;
  beatindexS = 0;
  beatindexD = 0;
  first_interpolS = 1;
  first_interpolD = 1;
  patch = 0;
  halfbeatsynch = 1;
  quarterbeatsynch = 1;
  positionssent = 0;
  halfbeatnew=0;
  curbar = 0;
  for (int i=0;i<2000;i++)
  {
    for (int j=0;j<2;j++)
    {
      BeatPosS[i][j]=0;
      BeatPosD[i][j]=0;
    }
  }


  //todo sAFEMATs mit 0 initialisieren wegen wechseln von langsam zu schnell
}

/*  This method is rarely used. It is only there to improve readability of component code.
It is called from cDataProcessor::myFinaliseInstance just before the call to configureWriter.
I.e. you can do everything that you would do here, also in configureWriter()
However, if you implement the method, it must return 1 in order for the configure process to continue!
*/
/*
int cStretchToTime::configureReader() 
{
return 1;
}
*/

//int cStretchToTime::configureWriter(sDmLevelConfig &c) 
//{
// if you would like to change the write level parameters... do so HERE:

// c.isRb=1; /* don't forget to set the write level sample/frame period */
// c.growDyn=0; /* e.g. 100 frames buffersize for ringbuffer */

//return 1; /* success */
//}

/* You shouldn't need to touch this....
int cStretchToTime::myConfigureInstance()
{
int ret = cDataProcessor::myConfigureInstance();
return ret;
}
*/

/*
Do what you like here... this is called after the input names and number of input elements have become available, 
so you may use them here.
*/
/*
int cStretchToTime::dataProcessorCustomFinalise()
{

return 1;
}
*/


/* 
Use setupNewNames() to freely set the data elements and their names in the output level
The input names are available at this point, you can get them via reader->getFrameMeta()
Please set "namesAreSet" to 1, when you do set names
*/
/*
int cStretchToTime::setupNewNames(long nEl) 
{
// namesAreSet = 1;
return 1;
}
*/

/*
If you don't use setupNewNames() you may set the names for each input field by overwriting the following method:
*/
/*
int cStretchToTime::setupNamesForField( TODO )
{
// DOC TODO...
}
*/

int cStretchToTime::myFinaliseInstance()
{
  //reader->setupSequentialMatrixReading(beatsamplesD,beatsamplesD); // winsize = pre + post
  int ret = cDataProcessor::myFinaliseInstance();
  if (ret) {

    // in Config Aufruf richtig anzugeben!!
    offsetS = (int)floor(startS / reader->getLevelT());
    offsetD = (int)floor(startD / reader->getLevelT());
    parsebeatpos();
    for (int i=0;i<2000;i++)
    {
      BeatPosS[i][0]=BeatPosS[i][0]/ reader->getLevelT() + offsetS;
      BeatPosD[i][0]=BeatPosD[i][0]/ reader->getLevelT() + offsetD;
    }
    //send positions once
    if (positionssent == 0 && Tracknumber == 1)
    {
      sendbeatpos(1);
      positionssent = 1;
    }
    if (positionssent == 0 && Tracknumber == 2)
    {
      sendbeatpos(2);
      positionssent = 1;
    }
    if (OffsetTrackS !=0)
    {
      adaptbeatpos(1); //1 == Source 2== Destination
    }
    if (OffsetTrackD !=0)
    {
      adaptbeatpos(2);
    }
    if (BeatPosS[0][1] == 1) first_interpol_torunS =4;
    if (BeatPosS[0][1] == 2) first_interpol_torunS =1;
    if (BeatPosS[0][1] == 3) first_interpol_torunS =2;
    if (BeatPosS[0][1] == 4) first_interpol_torunS =3;

    if (BeatPosD[0][1] == 1) first_interpol_torunD =4;
    if (BeatPosD[0][1] == 2) first_interpol_torunD =1;
    if (BeatPosD[0][1] == 3) first_interpol_torunD =2;
    if (BeatPosD[0][1] == 4) first_interpol_torunD =3;

    beatindexmat = 0;
    speed1 = int (org_speedS);
    speed2 = 0;

    beatindexS = 0;
    beatindexD = 0;
    fadelength = floor(0.1*(1/reader->getLevelT())*(60/double(speed1)));
    if (fadelength < 500) fadelength = 500;
    f = 1/fadelength;
    fade = f;

  }
  return ret;
}


int cStretchToTime::myTick(long long t)
{

  //nächsten Beat laden, Anzahl der Samples für nächsten Beat holen:

  if (parsebeatlength() == 1)
  {
    fadelength = floor(0.1 * beatsamplesS);
    if (halfbeat == 1 && firstframe == 0)
    {
      beatsamplesS = beatsamplesS/2;
      beatsamplesD = beatsamplesD/2;
      preset = beatsamplesS - beatsamplesD;
      fadelength = floor(0.1 * beatsamplesD);
    }
    if (halfbeat == 2 && firstframe == 0)
    {
      beatsamplesS = beatsamplesS/4;
      beatsamplesD = beatsamplesD/4;
      preset = beatsamplesS - beatsamplesD;
      fadelength = floor(0.1 * beatsamplesD);
    }
    f = 1/fadelength;
    fade = f;

    //Bypass
    if (Synchmode != TrackID && Synchmode != 0)
    {
      if (!(writer->checkWrite(long(beatsamplesS)))) return 0;
      cMatrix *mat = reader->getMatrix((long)(beatindexS),(long)(beatsamplesS));
      if (mat!=NULL)
      {
        cMatrix *matout = new cMatrix(mat->N,beatsamplesS,mat->type);
        for (int i=0;i<beatsamplesS;i++)
        {
          matout->dataF[0 + i * matout->N] = mat->dataF[0 + i * mat->N];
          if (mat->N == 2)
          {
            matout->dataF[1 + i * matout->N] = mat->dataF[1 + i * mat->N];
            if (Tracknumber == 2)
            {
              matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
            }
          }

        }
        writer->setNextMatrix(matout);
        delete matout;
        beatindexS = beatindexS + beatsamplesS;
        beatindexD = beatindexD + beatsamplesD;

        if (halfbeat == 1)
        {
          if (halfbeatsynch == 0)	
          {
            beatindexmat ++;
            curbar++;
          }
          if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
          if (halfbeatsynch == 0) halfbeatsynch++;
          else if (halfbeatsynch == 1) halfbeatsynch = 0;
        }
        else if (halfbeat == 2)
        {
          if (halfbeatsynch == 0)	
          {
            beatindexmat ++;
            curbar++;
          }
          if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
          if (halfbeatsynch < 3) halfbeatsynch++;

          else if (halfbeatsynch == 3) halfbeatsynch = 0;
        }
        else 
        {
          beatindexmat ++;
          curbar++;
          halfbeat = halfbeatnew;
        }

        firstframe = 0;
        //delete mat;
        return 1;
      }
    }
    if (beatsamplesS >= beatsamplesD)
    {
      f = 1;
      if (!(writer->checkWrite(long(beatsamplesS)))) return 0;
      cMatrix *mat = reader->getMatrix((long)(beatindexS),(long)(beatsamplesS));// + abs(preset)));
      if (mat!=NULL)
      {
        int i=0;

        if (firstframe == 0)
        {

          f = 0;
          cMatrix *matout = new cMatrix(mat->N,beatsamplesD,mat->type);
          //von Beginn bis Anfang des Ausfadens kopieren

          for (i=0; i<(beatsamplesD - fadelength); i++)
          {
            matout->dataF[0 + i * matout->N] = mat->dataF[0 + i * mat->N];
            if (mat->N == 2)
            {
              matout->dataF[1 + i * matout->N] = mat->dataF[1 + i * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }
            }
            if (matout->dataF[0 + i * matout->N] != mat->dataF[0 + i * mat->N])
            {
              SMILE_MSG(1,"mat: %f, matout: %f", mat->dataF[0 + i * mat->N],matout->dataF[0 + i * matout->N]);
            }
          }

          //Ausfaden und Einfaden
          double a = -1/(fadelength*fadelength);
          int x = 0;
          double q=0;


          int j = (int)(beatsamplesS - fadelength);
          for (i = (int)(beatsamplesD - fadelength); i < (int)beatsamplesD; i++)
          {
            p=(a*x*x+1);
            q=(a*(x-fadelength)*(x-fadelength)+1);
            matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + i * mat->N] + (FLOAT_DMEM)q * mat->dataF[0 + j * mat->N];
            if (mat->N == 2)
            {
              matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[1 + i * mat->N] + (FLOAT_DMEM)q * mat->dataF[0 + j * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }

            }
            //SMILE_IMSG(1,"safe mat + safeMatPre %f  , %f",matout->dataF[0 + i * matout->N],matout->dataF[1 + i * matout->N]);
            //SMILE_MSG(1,"f: %f ",f);
            j++;
            x++;
            if (j == beatsamplesS) break;
            //f+=fade;
          }
          //	}		
          writer->setNextMatrix(matout);
          firstframe = 0;
          beatindexS = beatindexS + beatsamplesS;
          beatindexD = beatindexD + beatsamplesD;
          if (halfbeat == 1)
          {
            if (halfbeatsynch == 0)	
            {
              beatindexmat ++;
              curbar++;
            }
            if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
            if (halfbeatsynch == 0) halfbeatsynch++;
            else if (halfbeatsynch == 1) halfbeatsynch = 0;
          }
          else if (halfbeat == 2)
          {
            if (halfbeatsynch == 0)	
            {
              beatindexmat ++;
              curbar++;
            }
            if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
            if (halfbeatsynch < 3) halfbeatsynch++;
            else if (halfbeatsynch == 3) halfbeatsynch = 0;
          }
          else 
          {
            beatindexmat ++;
            curbar++;
            halfbeat = halfbeatnew;
          }

          firstframe = 0;
          delete matout;
          //delete mat;
          return 1;
        }

        //hole neue Matrix matout mit Länge von anderem Beat
        if (firstframe == 1) 
        {

          i=0;
          cMatrix *matout = new cMatrix(mat->N,beatsamplesD,mat->type);
          for (i; i<beatsamplesD; i++)
          {
            matout->dataF[0 + i * matout->N] = mat->dataF[0 + i * mat->N];
            if (mat->N == 2)
            {

              matout->dataF[1 + i * matout->N] = mat->dataF[1 + i * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }

              //if (matout->dataF[1 + i * matout->N] == 0) matout->dataF[1 + i * matout->N] = matout->dataF[1 + (i-1) * matout->N];
            }

            //	matout->dataF[1 + i * matout->N] = mat->dataF[1 + i * mat->N];
          }
          int j = 0;
          /*for (i; i < beatsamplesD; i++)
          {
          safeMat[j][0] = f * mat->dataF[0 + i * mat->N];
          safeMat[j][1] = f * mat->dataF[1 + i * mat->N];
          f -= fade;
          //		SMILE_IMSG(1,"%f  %f",safeMat[j][0],safeMat[j][1]);
          j++;
          }
          j=0;
          f = 0;
          for (i = beatsamplesS - fadelength; i < beatsamplesS; i++)
          {
          if (i == beatsamplesD) break;
          safeMatpreBeat[j][0] = f * mat->dataF[0 + i * mat->N];
          safeMatpreBeat[j][1] = f * mat->dataF[1 + i * mat->N];
          f += fade;
          //SMILE_IMSG(1,"%f  %f",safeMatpreBeat[j][0],safeMatpreBeat[j][1]);
          j++;
          }*/
          writer->setNextMatrix(matout);
          firstframe = 0;
          beatindexS = beatindexS + beatsamplesS;
          beatindexD = beatindexD + beatsamplesD;
          if (halfbeat == 1)
          {
            if (halfbeatsynch == 0)	
            {
              beatindexmat ++;
              curbar++;
            }
            if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
            if (halfbeatsynch == 0) halfbeatsynch++;
            else if (halfbeatsynch == 1) halfbeatsynch = 0;
          }
          else if (halfbeat == 2)
          {
            if (halfbeatsynch == 0)	
            {
              beatindexmat ++;
              curbar++;
            }
            if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
            if (halfbeatsynch < 3) halfbeatsynch++;
            else if (halfbeatsynch == 3) halfbeatsynch = 0;
          }
          else 
          {
            beatindexmat ++;
            curbar++;
            halfbeat = halfbeatnew;
          }


          firstframe = 0;

          delete matout;
          //delete mat;
          return 1;
        }

      }	
      //else SMILE_IMSG(1, "I HAVE NO DATA!");
    }
    //langsamer machen
    else if (beatsamplesS < beatsamplesD)
    {
      if (!(writer->checkWrite(long(beatsamplesD)))) return 0;
      int i=0;
      cMatrix *mat = reader->getMatrix((long)(beatindexS),(long)(beatsamplesS));// + abs(preset)));
      //if (firstframe == 0) mat = reader->getMatrix((long)(beatindexS),(long)(beatsamplesS));// + abs(preset)));
      //mat = reader->getMatrix((long)(beatindexS),(long)(beatsamplesS));// + abs(preset)));
      patch = abs(preset);
      double q = 1/(patch+fadelength);
      double qfade = q;
      p = 1 /fadelength;
      pfade = p;
      f=0;
      float factor = 0;

      if (mat!=NULL)
      {

        cMatrix *matout = new cMatrix(mat->N,beatsamplesD,mat->type);
        if (matout->nT!= beatsamplesD) 
        {
          delete matout;
          return 0;
        }

        if (Deceleration == 0)
        {

          //Original Daten bis Ende - 2*fadelength
          for (i=0; i<beatsamplesS-2*fadelength; i++)
          {
            matout->dataF[0 + i * matout->N] = mat->dataF[0 + i * mat->N];
            if (mat->N == 2)
            {

              matout->dataF[1 + i * matout->N] = mat->dataF[1 + i * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }
            }
          }

          int k = (int)beatsamplesS - 3*(int)fadelength - (int)patch;
          if (k > beatsamplesS) k = (int)beatsamplesS- 3*(int)fadelength - (int)patch;
          if (k<0) k=0;
          //Fade zwischen Original und WDH
          double a = -1/(fadelength*fadelength);
          int x = 0;
          for (i = (int)beatsamplesS-2*(int)fadelength; i < (int)beatsamplesS - (int)fadelength; i++)
          {
            if (k > beatsamplesS) k = (int)beatsamplesS- 3*(int)fadelength - (int)patch;
            if (k<0) k= 0;

            p=(a*x*x+1);
            q=(a*(x-fadelength)*(x-fadelength)+1);
            matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + i * mat->N] + (FLOAT_DMEM)q * mat->dataF[0 + k * mat->N];

            if (mat->N == 2)
            {

              matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[1 + i * mat->N] + (FLOAT_DMEM)q * mat->dataF[1 + k * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }

            }
            k++;
            x++;
          }

          //Langsamer Outfade von PatchBereich
          p=0;
          q=0;
          k = (int)beatsamplesS - 2*(int)fadelength - (int)patch;
          int l = (int)beatsamplesS - (int)fadelength;  
          a = -1/((fadelength)*(fadelength));
          double a2 = -1/(fadelength*fadelength);
          x=0;
          int x2=0;
          for (i = (int)beatsamplesS - (int)fadelength; i < (int)beatsamplesD; i++)
          {
            if(mat->N == 1)
            {
              if (k > beatsamplesS) k = (int)beatsamplesS - 3*(int)fadelength - (int)patch;
              if (k<0) k= 0;
              matout->dataF[0 + i * matout->N] = mat->dataF[0 + k * mat->N];
              //Einfade von Vor-Beat
              if (i>beatsamplesD-fadelength)
              {
                p=(a*x*x+1);
                q=(a2*(x2-fadelength)*(x2-fadelength)+1);
                matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + k * mat->N]+ (FLOAT_DMEM)q * mat->dataF[0 + l * mat->N];
                l++;
                x++;
                x2++;
              }
            }			
            if (mat->N == 2)
            {
              if (k > beatsamplesS) k = (int)beatsamplesS - 3*(int)fadelength - (int)patch;
              if (k<0) k= 0;
              matout->dataF[0 + i * matout->N] = mat->dataF[0 + k * mat->N];
              matout->dataF[1 + i * matout->N] = mat->dataF[1 + k * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }

              if (i>(int)beatsamplesD-(int)fadelength)
              {
                if (k > beatsamplesS) k = (int)beatsamplesS- 3*(int)fadelength - (int)patch;
                if (k<0) k= 0;
                p=(a*x*x+1);
                q=(a2*(x2-fadelength)*(x2-fadelength)+1);
                matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + k * mat->N]+ (FLOAT_DMEM)q * mat->dataF[0 + l * mat->N];
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[1 + k * mat->N]+ (FLOAT_DMEM)q * mat->dataF[1 + l * mat->N];
                if (Tracknumber == 2)
                {	
                  matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
                }

                l++;
                x2++;
                x++;
              }
            }
            k++;
          }
        }
        else if(Deceleration == 1)
        {
          //Original Daten bis Ende - 2*fadelength
          for (i=0; i<beatsamplesS-2*fadelength; i++)
          {
            matout->dataF[0 + i * matout->N] = mat->dataF[0 + i * mat->N];
            if (mat->N == 2)
            {
              matout->dataF[1 + i * matout->N] = mat->dataF[1 + i * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }
            }
          }
          //
          int k = (int)beatsamplesS - (int)fadelength;
          //if (k<0) k=0;
          //Fade zwischen Original und WDH
          double a = -1/(fadelength*fadelength);
          int x = 0;
          for (i = (int)beatsamplesS-2*(int)fadelength; i < (int)beatsamplesS - (int)fadelength; i++)
          {
            if (k > (int)beatsamplesS) k = (int)beatsamplesS - 3*(int)fadelength - (int)patch;
            if (k<0) k = 0;
            p=(a*x*x+1);
            q=(a*(x-fadelength)*(x-fadelength)+1);
            matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + i * mat->N] + (FLOAT_DMEM)q * mat->dataF[0 + k * mat->N];
 
            if (mat->N == 2)
            {
              matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[1 + i * mat->N] + (FLOAT_DMEM)q * mat->dataF[1 + k * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }
            }

            k--;
            x++;
          }

          //Langsamer Outfade von PatchBereich
          p=0;
          q=0;
          //k = beatsamplesS - 2*fadelength;
          int l = (int)beatsamplesS - (int)fadelength;  
          a = -1/((fadelength)*(fadelength));
          double a2 = -1/(fadelength*fadelength);
          x=0;
          int x2=0;
          for (i = (int)beatsamplesS - (int)fadelength; i < (int)beatsamplesD; i++)
          {
            if(mat->N == 1)
            {
              matout->dataF[0 + i * matout->N] = mat->dataF[0 + k * mat->N];
              if (k > (int)beatsamplesS) k = (int)beatsamplesS- 3*(int)fadelength - (int)patch;
              if (k<0) k = 0;
              //Einfade von Vor-Beat
              if (i > (int)beatsamplesD - (int)fadelength)
              {
                p=(a*x*x+1);
                q=(a2*(x2-fadelength)*(x2-fadelength)+1);
                matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + k * mat->N]+ (FLOAT_DMEM)q * mat->dataF[0 + l * mat->N];
                l++;
                x++;
                x2++;
              }
            }			
            if (mat->N == 2)
            {
              if (k > (int)beatsamplesS) k = (int)beatsamplesS - 3*(int)fadelength - (int)patch;
              if (k<0) k= 0;
              matout->dataF[0 + i * matout->N] = mat->dataF[0 + k * mat->N];
              matout->dataF[1 + i * matout->N] = mat->dataF[1 + k * mat->N];
              if (Tracknumber == 2)
              {	
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
              }

              if (i>beatsamplesD-fadelength)
              {
                p=(a*x*x+1);
                q=(a2*(x2-fadelength)*(x2-fadelength)+1);
                matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[0 + k * mat->N]+ (FLOAT_DMEM)q * mat->dataF[0 + l * mat->N];
                matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)p * mat->dataF[1 + k * mat->N]+ (FLOAT_DMEM)q * mat->dataF[1 + l * mat->N];
                if (Tracknumber == 2)
                {	
                  matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)curbar;
                }

                l++;
                x2++;
                x++;
              }
            }

            k--;
          }


        }



        writer->setNextMatrix(matout);
        firstframe = 0;
        beatindexS = beatindexS + beatsamplesS;
        beatindexD = beatindexD + beatsamplesD;
        if (halfbeat == 1)
        {
          if (halfbeatsynch == 0)	
          {
            beatindexmat ++;
            curbar++;
          }

          if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
          if (halfbeatsynch == 0) halfbeatsynch++;
          else if (halfbeatsynch == 1) halfbeatsynch = 0;
        }
        else if (halfbeat == 2)
        {
          if (halfbeatsynch == 0)
          {
            beatindexmat ++;
            curbar++;
          }
          if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
          if (halfbeatsynch < 3) halfbeatsynch++;
          else if (halfbeatsynch == 3) halfbeatsynch = 0;
        }
        else 
        {
          beatindexmat ++;
          curbar++;
          halfbeat = halfbeatnew;
        }

        firstframe = 0;

        delete matout;
        //delete mat;
        return 1;


        /*
        writer->setNextMatrix(matout);
        firstframe = 0;
        beatindexS = beatindexS + beatsamplesS;
        beatindexD = beatindexD + beatsamplesD;
        if (halfbeat == 1)
        {
        if (halfbeatsynch == 0)
        {
        beatindexmat ++;
        curbar++;
        }
        if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
        if (halfbeatsynch == 0) halfbeatsynch++;
        else if (halfbeatsynch == 1) halfbeatsynch = 0;
        }
        else if (halfbeat == 2)
        {
        if (halfbeatsynch == 0)
        {
        beatindexmat ++;
        curbar++;
        }
        if (halfbeatsynch == 0)	halfbeat = halfbeatnew;
        if (halfbeatsynch < 3) halfbeatsynch++;
        else if (halfbeatsynch == 3) halfbeatsynch = 0;
        }
        else 
        {
        beatindexmat ++;
        curbar++;
        halfbeat = halfbeatnew;
        }

        firstframe = 0;
        delete matout;
        return 1;*/

      }
      //else SMILE_IMSG(1, "I HAVE NO DATA!");

    }


    //SMILE_IMSG(1,"Beatindex: %i",beatindexmat);

    //	writer->setMatrix(beatindexS,mat);
    //writer->setNextMatrix(dummy);
    //			firstsamples = 0;
    //SMILE_MSG(1,"STT can't write, level full... checkWrite(bs=%i)",blocksizeW);
    //} else {
    //return 1;


    //}
  }


  //if ((writer->checkWrite(blocksizeR))) dummy = reader->getNextMatrix();
  //writer->setNextMatrix(dummy);
  return 0;
}


cStretchToTime::~cStretchToTime()
{
  // cleanup...

}

void cStretchToTime::parsebeatpos() //TODO Abbruchbedingung
{
  int length;
  bool nextValue = true;
  bool Valuedone = false;
  double mem=0;
  char * buffer, *transbuffer;
  int z=0;
  int posonedone =0;
  int *beatindex;
  int s=0;
  std::ifstream is;

  if (TrackID == 1)
  {
    is.open ("beatpos.txt", std::ios::binary );//TODO allgemein gültig machen
  }
  else if (TrackID == 2)
  {
    is.open ("beatpos2.txt", std::ios::binary );//TODO allgemein gültig machen
  }
  // get length of file:
  is.seekg (0, std::ios::end);
  length = is.tellg();
  is.seekg (0, std::ios::beg);

  // allocate memory:
  buffer = new char [length];
  //  bufferdouble = new double [length];
  beatindex = new int [length];
  printf("length: %i",length);

  // read data as a block:
  is.read (buffer,length);
  is.close();

  //read = getline(&transbuffer,&n,buffer);
  for (int i=0;i<length;i++)
  {
    if (buffer[i]>45 && Valuedone==true)
    {
      nextValue = true;
      Valuedone = false;
    }
    if (nextValue == true)
    {
      transbuffer = &buffer[i];
      BeatPosS [z][s] = atof(transbuffer);
      nextValue = false;
      z=z+s;
      s++;
      s=s%2;

    }
    if (buffer[i]<45 && Valuedone==false)
    {
      Valuedone = true;
    }
  }
  //cout.write (buffer,length);

  delete[] buffer;
  delete[] beatindex;


  nextValue = true;
  Valuedone = false;
  mem=0;
  z=0;
  posonedone =0;
  s=0;

  if (TrackID == 1)
  {
    is.open ("beatpos2.txt", std::ios::binary );//TODO allgemein gültig machen
  }
  else if (TrackID == 2)
  {
    is.open ("beatpos.txt", std::ios::binary );//TODO allgemein gültig machen
  }
  // get length of file:
  is.seekg (0, std::ios::end);
  length = is.tellg();
  is.seekg (0, std::ios::beg);

  // allocate memory:
  buffer = new char [length];
  //  bufferdouble = new double [length];
  beatindex = new int [length];
  printf("length: %i",length);

  // read data as a block:
  is.read (buffer,length);
  is.close();

  //read = getline(&transbuffer,&n,buffer);
  for (int i=0;i<length;i++)
  {
    if (buffer[i]>45 && Valuedone==true)
    {
      nextValue = true;
      Valuedone = false;
    }
    if (nextValue == true)
    {
      transbuffer = &buffer[i];
      BeatPosD [z][s] = atof(transbuffer);
      nextValue = false;
      z=z+s;
      s++;
      s=s%2;

    }
    if (buffer[i]<45 && Valuedone==false)
    {
      Valuedone = true;
    }
  }
  //cout.write (buffer,length);

  delete[] buffer;
  delete[] beatindex;
  if (BeatPosS[0][0] > 4)
  {
    BeatPosS[0][0] = 2 * BeatPosS[1][0] - BeatPosS[2][0];
  }
  if (BeatPosD[0][0] > 4)
  {
    BeatPosD[0][0] = 2 * BeatPosD[1][0] - BeatPosD[2][0];
  } 
}

int cStretchToTime::processComponentMessage( cComponentMessage *_msg )
{
  // if (multiOut==1) {

  if (isMessageType(_msg,"SynchSpeed1"))  {
    // if previous message was not yet processed we update the data
    speed1 = (_msg->intData[0]);
  }
  if (isMessageType(_msg,"SynchSpeed2"))  {
    // if previous message was not yet processed we update the data
    speed2 = (_msg->intData[0]);
  }
  if (isMessageType(_msg,"HalfBeatChange"))  {
    // if previous message was not yet processed we update the data
    halfbeatnew = (_msg->intData[0]);
  }
  if (isMessageType(_msg,"Synchmode"))  {
    // if previous message was not yet processed we update the data
    Synchmode = (_msg->intData[0]);
  }
  if (isMessageType(_msg,"STTMode"))  {
    // if previous message was not yet processed we update the data
    Deceleration = (_msg->intData[0]);
  }
  // TODO: message queque??
  //}

  return 0;
}

int cStretchToTime::parsebeatlength()
{
  if (first_interpolS == 0) beatsamplesS = (long)( BeatPosS[beatindexmat + 1][0]-BeatPosS[beatindexmat][0] );
  if (first_interpolD == 0) beatsamplesD = (long)( BeatPosD[beatindexmat + 1][0]-BeatPosD[beatindexmat][0] );

  if (first_interpolS == 1)
  {
    if (BeatPosS[0][1] == 1 && first_interpol_torunS != 0)
    {
      beatsamplesS = (long)( BeatPosS[0][0]/4 );
      first_interpol_torunS --;
    }
    if (BeatPosS[0][1] == 2 && first_interpol_torunS != 0)
    {
      beatsamplesS = (long)( BeatPosS[0][0] );		
      first_interpol_torunS --;
    }
    if (BeatPosS[0][1] == 3 && first_interpol_torunS != 0)
    {
      beatsamplesS = (long)( BeatPosS[0][0]/2 );
      first_interpol_torunS --;
    }
    if (BeatPosS[0][1] == 4 && first_interpol_torunS != 0)
    {
      beatsamplesS = (long)( BeatPosS[0][0]/3 );
      first_interpol_torunS --;
    }
    if (first_interpol_torunS == 0) first_interpolS = 0;
  }
  if (first_interpolD == 1)
  {
    // für BeatPosD
    if (BeatPosD[0][1] == 1 && first_interpol_torunD != 0)
    {
      beatsamplesD = (long)( BeatPosD[0][0]/4 );
      first_interpol_torunD --;
    }
    if (BeatPosD[0][1] == 2 && first_interpol_torunD != 0)
    {
      beatsamplesD = (long)( BeatPosD[0][0] );
      first_interpol_torunD --;
    }
    if (BeatPosD[0][1] == 3 && first_interpol_torunD != 0)
    {
      beatsamplesD = (long)( BeatPosD[0][0]/2 );
      first_interpol_torunD --;
    }
    if (BeatPosD[0][1] == 4 && first_interpol_torunD != 0)
    {
      beatsamplesD = (long)( BeatPosD[0][0]/3 );
      first_interpol_torunD --;
    }
    if (first_interpol_torunD == 0) first_interpolD = 0;

  }
  preset = beatsamplesS - beatsamplesD;
  if (beatsamplesS <= 0) return 0;
  if (beatsamplesD <= 0) return 0;

  if (Synchmode == 0 && first_interpol_torunS == 0 && first_interpol_torunD == 0) 
  {
    beatsamplesD = (long)( (1.0/reader->getLevelT())*(60.0/double(speed1)) );
    preset = beatsamplesS - beatsamplesD;
    if (beatsamplesS <= 0) return 0;
    if (beatsamplesD <= 0) return 0;
  }
  return 1;
}

int cStretchToTime::adaptbeatpos(int Track)
{
  //if (OffsetTrackS < 0) OffsetTrackS = OffsetTrackS + 4;
  //if (OffsetTrackD < 0) OffsetTrackD = OffsetTrackD + 4;
  if (Track == 1)
  {
    for (int i = 0; i < 1000; i++)
    {
      if (BeatPosS[i][1] == 0) return 1;
      BeatPosS[i][1] = double(int(BeatPosS[i][1] + OffsetTrackS));
      if (BeatPosS[i][1] > 4) BeatPosS[i][1] = BeatPosS[i][1] - 4;
      if (BeatPosS[i][1] < 0) BeatPosS[i][1] = BeatPosS[i][1] + 4;
    }
  }
  if (Track == 2)
  {
    for (int i = 0; i < 800; i++)
    {
      if (BeatPosS[i][1] == 0) return 1;
      BeatPosS[i][1] =	double(int(BeatPosS[i][1] + OffsetTrackS));
      if (BeatPosS[i][1]  > 4) BeatPosS[i][1] = BeatPosS[i][1] - 4;
      if (BeatPosS[i][1]  < 0) BeatPosS[i][1] = BeatPosS[i][1] + 4;
    }
  }
  return 0;


}

void cStretchToTime::sendbeatpos(int i)
{
  if (i==1)
  {
    for (int n = 0; n < 2000; n++)
    {
      cComponentMessage BeatPos("BeatPositionsforChroma");
      if (BeatPosS[n][0] == 0) break;
      BeatPos.floatData[0]=BeatPosS[n][0];
      BeatPos.floatData[1]=BeatPosS[n][1];
      cComponentMessage *BeatPostoChroma = &BeatPos;
      sendComponentMessage("chromaperFrame",BeatPostoChroma);
    }
  }
  if (i==2)
  {
    for (int n = 0; n < 2000; n++)
    {
      cComponentMessage BeatPos("BeatPositionsforChroma2");
      if (BeatPosS[n][0] == 0) break;
      BeatPos.floatData[0]=BeatPosD[n][0];
      BeatPos.floatData[1]=BeatPosD[n][1];
      cComponentMessage *BeatPostoChroma = &BeatPos;
      sendComponentMessage("chromaperFrame2",BeatPostoChroma);
    }
  }
}

