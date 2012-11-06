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

example for dataProcessor descendant

*/


#ifndef __STRETCHTOTIME_HPP
#define __STRETCHTOTIME_HPP

#include <smileCommon.hpp>
#include <dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CSTRETCHTOTIME "Stretch to time component used in the ADMIRE demonstrator, INCOMPLETE and UNSUPPORTED"
#define COMPONENT_NAME_CSTRETCHTOTIME "cStretchToTime"

class cStretchToTime : public cDataProcessor {
  private:
    //double offset;
	int firstframe;
	int halfbeatsynch,quarterbeatsynch;
	void parsebeatpos();
	void sendbeatpos(int);
	int parsebeatlength();
//	long int index1,index2;
	long int beatsamplesS,beatsamplesD;
	long int beatindexS,beatindexD;
	int synch,beatindexmat;
	int offsetS,offsetD,preset;
	double startS,startD;
	double BeatPosS[2000][2];
	double BeatPosD[2000][2];	
	//double safeMat[4001][2];
	//double safeMatpreBeat[4001][2];
	//double safeMatpatch[10000][2];
	double f,p;
	double fade,pfade;
	double fadelength;
	int runID,first_interpolS,first_interpol_torunS,first_interpolD,first_interpol_torunD;
	int speed1,speed2,Synchmode;
	double org_speedS,org_speedD;
	double patch;
	int adaptbeatpos(int);
	int OffsetTrackS,OffsetTrackD;
	int halfbeat,halfbeatnew;
	int TrackID,Tracknumber;
	int patchsize;
	int positionssent;
	int Deceleration;
	double curbar;
	//cMatrix * matout;
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
	virtual int processComponentMessage( cComponentMessage *_msg );

    // virtual int dataProcessorCustomFinalise();
    // virtual int setupNewNames(long nEl);
    // virtual int setupNamesForField();
    //virtual int configureWriter(sDmLevelConfig &c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cStretchToTime(const char *_name);
	


    virtual ~cStretchToTime();
};




#endif // __EXAMPLE_PROCESSOR_HPP
