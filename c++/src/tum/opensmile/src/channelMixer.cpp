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

Mixer to mix multiple audio channels (pcm data). 
This component was written by Benedikt Gollan (TUM), 2009.

*/


#include "channelMixer.hpp"

#define MODULE "cChannelMixer"

SMILECOMPONENT_STATICS(cChannelMixer)

SMILECOMPONENT_REGCOMP(cChannelMixer)
{
  SMILECOMPONENT_REGCOMP_INIT

    scname = COMPONENT_NAME_CCHANNELMIXER;
  sdescription = COMPONENT_DESCRIPTION_CCHANNELMIXER;

  // we inherit cDataProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cDataProcessor")

    SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("offset","offset that this dummy component adds to the values",1.0);
  ct->setField("blocksize",NULL,2000);
  ct->setField("isRb", "ringbuffer level (0=no, 1=yes)", 0);
  ct->setField("growDyn", "if not ringbuffer 1=buffer may grow dynamically", 1);

  ct->setField("OriginalTrackValue","Volume Control for Original Track",1);
  ct->setField("BeatTrackValue","Volume Control for Beat Track1",1);
  ct->setField("OriginalTrackValue2","Volume Control for Original Track",1);
  ct->setField("BeatTrackValue2","Volume Control for Beat Track1",1);
  ct->setField("DrumTrackValue","Volume Control for Drum Track1",1);
  ct->setField("DrumTrackValue2","Volume Control for Drum Track2",1);
  ct->setField("HarmonicTrackValue","Volume Control for Harmonic Track1",1);
  ct->setField("HarmonicTrackValue2","Volume Control for Harmonic Track2",1);
  ct->setField("Synch","Flag for PlayMode",1);
  )

    SMILECOMPONENT_MAKEINFO(cChannelMixer);
}


SMILECOMPONENT_CREATE(cChannelMixer)

//-----

cChannelMixer::cChannelMixer(const char *_name) :
cDataProcessor(_name),
ftrack2(100),
ftrack1(100),
karaoke1 (0),
karaoke2 (0)
{
}

void cChannelMixer::fetchConfig()
{
  cDataProcessor::fetchConfig();

  // load all configuration parameters you will later require fast and easy access to here:
  Pause = 0;
  offset = getDouble("offset");
  track1 = getInt("OriginalTrackValue");
  track2 = getInt("BeatTrackValue");
  track3 = getInt("DrumTrackValue");
  track4 = getInt("HarmonicTrackValue");
  track7 = getInt("OriginalTrackValue2");
  track8 = getInt("BeatTrackValue2");
  track9 = getInt("DrumTrackValue2");
  track10 = getInt("HarmonicTrackValue2");

  synch = getInt("Synch");
  // note, that it is "polite" to output the loaded parameters at debug level 2:
  SMILE_DBG(2,"offset = %f",offset);
}

int cChannelMixer::myConfigureInstance()
{
  int ret=1;
  // if you would like to change the write level parameters... do so HERE:

  //
  // .. or override configureWriter() to do so, after the reader config is available!
  //
  ret *= cDataProcessor::myConfigureInstance();
  return ret;
}

/*
int cChannelMixer::configureWriter(const sDmLevelConfig *c)
{

// we must return 1, in order to indicate configure success (0 indicates failure)
return 1;
}
*/
int cChannelMixer::configureWriter(sDmLevelConfig &c) 
{
  // if you would like to change the write level parameters... do so HERE:

  // c.isRb = 0; /* don't forget to set the write level sample/frame period */
  // c.growDyn = 1; /* e.g. 100 frames buffersize for ringbuffer */

  return 1; /* success */
}

int cChannelMixer::setupNewNames(long nEl)
{
  writer->addField( "pcmmixed", 3 );
  namesAreSet = 1;
  return 1;
}

int cChannelMixer::myFinaliseInstance()
{
  // set level names here..... (and set variable namesAreSet to 1)

  // namesAreSet = 1;
  reader->setupSequentialMatrixReading(blocksizeR,blocksizeR); // winsize = pre + post
  return cDataProcessor::myFinaliseInstance();
}


int cChannelMixer::myTick(long long t)
{
  if (Pause != 1)
  {
    SMILE_DBG(4,"tick # %i, processing value vector",t);
    if (!(writer->checkWrite(blocksizeR))) return 0;
    cMatrix *mat = reader->getNextMatrix();
    if (mat==NULL)
    {
      return 0;
    }
    cMatrix *matout = new cMatrix(3,mat->nT,mat->type);
    //int toSet = processBuffer(mat, matout, 0, 0);
    //matout[][1]= mat[][1]+mat[][3];
    //cDataProcessor::fetchConfig();
    //origtrackvol = getInt("OriginalTrackValue");
    lockMessageMemory();
    ftrack1 = double(track1)/250;
    ftrack2 = double(track2)/250;
    ftrack3 = double(track3)/250;
    ftrack4 = double(track4)/250;
    ftrack7 = double(track7)/250;
    ftrack8 = double(track8)/250;
    ftrack9 = double(track9)/250;
    ftrack10 = double(track10)/250;
    fkaraoke1 = double(karaoke1)/250;
    fkaraoke2 = double(karaoke2)/250;
    unlockMessageMemory();
    //Karaoke Spur
    /*if (karaoke != 0)
    {

    for (int i=0; i<mat->nT; i++)
    {
    //matout->dataF[2 + i * matout->N] = (mat->dataF[0 + i * mat->N] - mat->dataF[1 + i*mat->N]);
    }
    }*/

    if (synch ==0)
    {
      for (int i=0; i<mat->nT; i++)
      {
        matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)( 0.5*(ftrack1*mat->dataF[0 + i * mat->N]+ ftrack2*mat->dataF[2 + i*mat->N] + ftrack3*mat->dataF[4 + i * mat->N] + ftrack4*mat->dataF[5 + i*mat->N])+ fkaraoke1*(mat->dataF[0 + i * mat->N] - mat->dataF[1 + i*mat->N]) );
        matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)( 0.5*(ftrack1*mat->dataF[1 + i * mat->N]+ ftrack2*mat->dataF[2 + i*mat->N] + ftrack3*mat->dataF[4 + i * mat->N] + ftrack4*mat->dataF[5 + i*mat->N])+ fkaraoke1*(mat->dataF[0 + i * mat->N] - mat->dataF[1 + i*mat->N]) );
        matout->dataF[2 + i * matout->N] = mat->dataF[3 + i * mat->N];
      }
      writer->setNextMatrix(matout);
    }
    else if (synch == 1)
    {
      for (int i=0; i<mat->nT; i++)
      {
        matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)( 0.5*(ftrack7*mat->dataF[0 + i * mat->N]+ ftrack8*mat->dataF[2 + i*mat->N]) );// + ftrack9*mat->dataF[4 + i * mat->N] + ftrack10*mat->dataF[5 + i*mat->N])+ fkaraoke2*(mat->dataF[7 + i * mat->N] - mat->dataF[8 + i*mat->N]) );
        matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)( 0.5*(ftrack7*mat->dataF[1 + i * mat->N]+ ftrack8*mat->dataF[2 + i*mat->N]) );// + ftrack9*mat->dataF[4 + i * mat->N] + ftrack10*mat->dataF[5 + i*mat->N])+ fkaraoke2*(mat->dataF[7 + i * mat->N] - mat->dataF[8 + i*mat->N]) );
        matout->dataF[2 + i * matout->N] = mat->dataF[3 + i * mat->N];
      }
      writer->setNextMatrix(matout);
    }
    else if (synch == 2)
    {
      for (int i=0; i<mat->nT; i++)
      {
        matout->dataF[0 + i * matout->N] = (FLOAT_DMEM)( 0.5*(ftrack1*mat->dataF[0 + i * mat->N]+ ftrack2*mat->dataF[2 + i * mat->N] + ftrack3*mat->dataF[4 + i * mat->N] + ftrack4*mat->dataF[5 + i*mat->N] + fkaraoke1*(mat->dataF[0 + i * mat->N] - mat->dataF[1 + i*mat->N]) + ftrack7*mat->dataF[6 + i * mat->N]+ ftrack9*mat->dataF[10 + i * mat->N] + ftrack10*mat->dataF[11 + i*mat->N]+ fkaraoke2*(mat->dataF[6 + i * mat->N] - mat->dataF[7 + i*mat->N])) );//TODO KARAOKE?? Fehler wegen Synchronisation? + ftrack8*mat->dataF[8 + i*mat->N]
        matout->dataF[1 + i * matout->N] = (FLOAT_DMEM)( 0.5*(ftrack1*mat->dataF[1 + i * mat->N]+ ftrack3*mat->dataF[4 + i * mat->N] + ftrack4*mat->dataF[5 + i * mat->N] + fkaraoke1*(mat->dataF[0 + i * mat->N] - mat->dataF[1 + i*mat->N]) + ftrack7*mat->dataF[7 + i * mat->N]+ ftrack8*mat->dataF[8 + i*mat->N]+ ftrack9*mat->dataF[10 + i * mat->N] + ftrack10*mat->dataF[11 + i*mat->N] + fkaraoke2*(mat->dataF[6 + i * mat->N] - mat->dataF[7 + i*mat->N])) ); //+ ftrack2*mat->dataF[2 + i*mat->N]
        matout->dataF[2 + i * matout->N] = mat->dataF[3 + i * mat->N];
      }
      writer->setNextMatrix(matout);

    }
    delete matout;
    //delete mat;
    return 1;

  }
  return 0;
}

cChannelMixer::~cChannelMixer()
{

}

int cChannelMixer::processComponentMessage( cComponentMessage *_msg )
{
  // if (multiOut==1) {

  if (isMessageType(_msg,"Track1"))  {
    // if previous message was not yet processed we update the data
    synch = (long)(_msg->intData[0]);
    track1 = (long)(_msg->intData[1]);
    track2 = (long)(_msg->intData[2]);
    track3 = (long)(_msg->intData[3]);
    track4 = (long)(_msg->intData[4]);
    track5 = (long)(_msg->intData[5]);
    track6 = (long)(_msg->intData[6]);
    karaoke1 = (long) (_msg->intData[7]);
  }
  if (isMessageType(_msg,"Track2"))  {
    // if previous message was not yet processed we update the data
    synch = (long)(_msg->intData[0]);
    track7 = (long)(_msg->intData[1]);
    track8 = (long)(_msg->intData[2]);
    track9 = (long)(_msg->intData[3]);
    track10 = (long)(_msg->intData[4]);
    track11 = (long)(_msg->intData[5]);
    track12 = (long)(_msg->intData[6]);
    karaoke2 = (long) (_msg->intData[7]);
  }

  if (isMessageType(_msg,"Pause"))  {
    Pause = (long)(_msg->intData[0]);
  }
  //}

  return 0;
}
