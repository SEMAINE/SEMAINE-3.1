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


#ifndef __CHANNELMIXER_HPP
#define __CHANNELMIXER_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CCHANNELMIXER "Mixer to mix multiple audio channels (pcm data).\n  This component was written by Benedikt Gollan (TUM), 2009."
#define COMPONENT_NAME_CCHANNELMIXER "cChannelMixer"

#undef class
class DLLEXPORT cChannelMixer : public cDataProcessor {
private:
  double offset;
  int synch,Pause;
  int track1,track2,track3,track4,track5,track6,track7,track8,track9,track10,track11,track12;
  int karaoke1,karaoke2;
  double ftrack1,ftrack2,ftrack3,ftrack4,ftrack5,ftrack6,ftrack7,ftrack8,ftrack9,ftrack10,ftrack11,ftrack12;
  double fkaraoke1,fkaraoke2;

protected:
  SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
  virtual int myConfigureInstance();
  virtual int myFinaliseInstance();
  virtual int myTick(long long t);
  virtual int setupNewNames(long nEl);
  virtual int processComponentMessage( cComponentMessage *_msg );
  int configureWriter(sDmLevelConfig &c); 

  //virtual int configureWriter(const sDmLevelConfig *c);

public:
  SMILECOMPONENT_STATIC_DECL

    cChannelMixer(const char *_name);

  virtual ~cChannelMixer();
};




#endif // __EXAMPLE_PROCESSOR_HPP
