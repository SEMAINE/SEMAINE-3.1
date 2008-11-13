//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// BAPParser.cpp: implementation of the BAPParser class.
//
//////////////////////////////////////////////////////////////////////
//include "stdafx.h"
#include "BAPParser.h"
#include <iostream>
#include <string>

// uncomment line below to include variable definitions and (commented-out)
// parsing of emote parameters
//define _WITH_EMOTE


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BAPParser::BAPParser()
{

}

BAPParser::~BAPParser()
{

}

int BAPParser::ParseFile(string filename, 
						BAPFrameVector *pFrames,
						EmoteParameters *pEmoteParams)
{
	//BAPFrameVector frames;

	ifstream infile(filename.c_str());
	if (!infile) { 
		cout << "! BAPParser::ParseFile: error opening output file! Returning.\n"; 
		return 1;
	}
/*	string fileContents;
	getline(infile,fileContents,'\0'); //CHECK: can zero appear or not?
	return ParseString(&fileContents,pFrames,pEmoteParams);
}*/

	BAPFrame * thisFrame;
	cout<<". BAPParser::ParseFile() - START"<<endl;
	int maskValue;
	int lines_read = 0;
	int cur; // current index
	string theLine,secondLine,classifier,tempString;
	vector<string> words;
	vector<int> setBAPs;
	
#ifdef _WITH_EMOTE
	int fnum,side; //for emote frames
	int ftype,ktype;
	double ef1,ef2,ef3,ef4; //for emote frames
	//KeypointType {Global, Local};
	//FrameType {Goal, Via, Inbetween, Effort, Shape};
#endif
	
	while (getline(infile,theLine,'\n')) {
		
		// SKIP COMMENT LINES AND EMTPY LINES
		if( theLine.compare(0,1,string("#"))==0) {

#ifdef _WITH_EMOTE
			if(pEmoteParams==NULL) continue;
			// TODO: fix broken emote link
			//*
			if(theLine.compare(0,6,string("#EMOTE"))==0) {
				//handle emote parameters
				//copy $ paste
				if(strncmp(theLine.c_str(),"#EMOTE Ikframe",14)==0) {
					sscanf(theLine.c_str(),"#EMOTE Ikframe %d %d %d %d",
							&fnum, &side, &ftype, &ktype);
					
					//TODO fix arguments
					enum KeypointType {Global, Local};
					enum FrameType {Goal, Via, Inbetween, Effort, Shape};
					pEmoteParams->AddIkFrame(fnum,side,0.0,0.0,0.0,0.0,ftype,ktype,NULL);
				}
				else if (strncmp(theLine.c_str(),"#EMOTE IkEfframe",16)==0) {
					sscanf(theLine.c_str(),"#EMOTE IkEfframe %d %d %lf %lf %lf %lf",
							&fnum, &side, &ef1, &ef2, &ef3, &ef4);
					pEmoteParams->AddEffortFrame(fnum,side,ef1,ef2,ef3,ef4);
					//space weight time flow
				}
				else if (strncmp(theLine.c_str(),"#EMOTE IkShframe",16)==0) {
					sscanf(theLine.c_str(),"#EMOTE IkShframe %d %d %lf %lf %lf %lf",
							&fnum, &side, &ef1, &ef2, &ef3, &ef4);
					pEmoteParams->AddShapeFrame(fnum,side,ef1,ef2,ef3,ef4);
					//horix vert sagg shapeflow
				}
				
			}
			*/
#endif
			continue;
			
		}
		if (theLine.size()==0) {
			continue;
		}
		// TOKENIZE LINE INTO WORDS
		theLine=theLine+ " ";
		words.clear();
		string::size_type pos=0, prev_pos=0; 
		cur=0;
		
		while ((pos=theLine.find_first_of(' ',pos))!=string::npos) {
			tempString = theLine.substr(prev_pos, pos-prev_pos);
			tempString = string(_strupr((char *)tempString.c_str()));
			if (tempString.length() != 0) {
				words.push_back(tempString);
			}
			prev_pos= ++pos;
		}
		int tokens = words.size();
		if(lines_read==0)
		{
			// handle header line
			fps=atoi(words[2].c_str());
		}
		else
		{
			if(getline(infile,secondLine,'\n'))
			{
				thisFrame = new BAPFrame(&this->converter,atoi(words[0].c_str()));
				thisFrame->ReadFromBuffer((char*)(theLine+secondLine).c_str());
				pFrames->push_back(thisFrame);
			}
		}

		/*
		else if (lines_read%2==1){
			//handle mask line
			setBAPs.clear();
			for(unsigned int wI=0; wI<words.size(); wI++) {
				int length=words[wI].length();
				maskValue = atoi(words[wI].c_str());
				if(maskValue) {
					setBAPs.push_back(wI+1); // CHANGED CHECK
				}
			}
		}
		else { 
			
			//handle angle data line
			thisFrame = new BAPFrame(&this->converter,atoi(words[0].c_str()));
			for(unsigned int bI=0; bI<setBAPs.size(); bI++) {
				thisFrame->SetBAP(setBAPs[bI],atoi(words[bI+1].c_str()));
			}
			pFrames->push_back(thisFrame);

		}*/
		lines_read++;
	}

	cout<<". BAPParser::ParseFile() - END - "<<pFrames->size()<<" frames read."<<endl;	
	return 0;
}

int BAPParser::ParseBuffer(string buffer, 
						BAPFrameVector *pFrames)
{
	//BAPFrameVector frames;

	if (buffer=="") { 
		cout << "! BAPParser::ParseBuffer: Buffer is empty! Returning.\n"; 
		return 1;
	}
/*	string fileContents;
	getline(infile,fileContents,'\0'); //CHECK: can zero appear or not?
	return ParseString(&fileContents,pFrames,pEmoteParams);
}*/

	BAPFrame * thisFrame;
	cout<<". BAPParser::ParseBuffer() - START"<<endl;
	int maskValue;
	int lines_read = 0;
	int cur; // current index
	string theLine,classifier,tempString;
	vector<string> words;
	vector<int> setBAPs;
	
	while (buffer!="") {
		theLine=buffer.substr(0,buffer.find_first_of("\n\0"));
		buffer=buffer.substr(buffer.find_first_of("\n\0")+1);
		// SKIP COMMENT LINES AND EMTPY LINES
		if( theLine.compare(0,1,string("#"))==0) {
			continue;
		}
		if (theLine.size()==0) {
			continue;
		}
		// TOKENIZE LINE INTO WORDS
		theLine=theLine+ " ";
		words.clear();
		string::size_type pos=0, prev_pos=0; 
		cur=0;
		
		while ((pos=theLine.find_first_of(' ',pos))!=string::npos) {
			tempString = theLine.substr(prev_pos, pos-prev_pos);
			tempString = string(_strupr((char *)tempString.c_str()));
			if (tempString.length() != 0) {
				words.push_back(tempString);
			}
			prev_pos= ++pos;
		}
		int tokens = words.size();
		if(lines_read==0) {
			// handle header line
			fps=atoi(words[2].c_str());
		}
		else if (lines_read%2==1){
			//handle mask line
			setBAPs.clear();
			for(unsigned int wI=0; wI<words.size(); wI++) {
				int length=words[wI].length();
				maskValue = atoi(words[wI].c_str());
				if(maskValue) {
					setBAPs.push_back(wI+1); // CHANGED CHECK
				}
			}
		}
		else { 
			
			//handle angle data line
			thisFrame = new BAPFrame(&this->converter,atoi(words[0].c_str()));
			for(unsigned int bI=0; bI<setBAPs.size(); bI++) {
				thisFrame->SetBAP(setBAPs[bI],atoi(words[bI+1].c_str()));
			}
			pFrames->push_back(thisFrame);

		}
		lines_read++;
	}

	cout<<". BAPParser::ParseBuffer() - END - "<<pFrames->size()<<" frames read."<<endl;	
	return 0;
}


// TODO: eliminate redundancy in parsing btw file and string

int BAPParser::ParseString(string *str, BAPFrameVector *pFrames,EmoteParameters *pEmoteParams)
{
	BAPFrame * thisFrame;
	cout<<". BAPParser::ParseString() - START"<<endl;
	int maskValue;
	int lines_read = 0;
	int cur; // current index
	string theLine,classifier,tempString;
	vector<string> words;
	vector<int> setBAPs;
	
#ifdef _WITH_EMOTE
	int fnum,side; //for emote frames
	int ftype,ktype;
	double ef1,ef2,ef3,ef4; //for emote frames
	//KeypointType {Global, Local};FrameType {Goal, Via, Inbetween, Effort, Shape};
#endif

	
	string::size_type lpos=0,lpos_prev=0; 
	
	while ((lpos=str->find_first_of('\n',lpos))!=string::npos) {
		
		theLine=str->substr(lpos_prev, lpos-lpos_prev);
		lpos_prev=++lpos;

		// SKIP COMMENT LINES AND EMTPY LINES
		if( theLine.compare(0,1,string("#"))==0) {
#ifdef _WITH_EMOTE
			if(pEmoteParams==NULL) continue;
			// TODO: fix broken emote link
			/*
			if(theLine.compare(0,6,string("#EMOTE"))==0) {
				//handle emote parameters
				//copy $ paste
				if(strncmp(theLine.c_str(),"#EMOTE Ikframe",14)==0) {
					sscanf(theLine.c_str(),"#EMOTE Ikframe %d %d %d %d",
							&fnum, &side, &ftype, &ktype);
					
					//TODO fix arguments
					enum KeypointType {Global, Local};
					enum FrameType {Goal, Via, Inbetween, Effort, Shape};
					pEmoteParams->AddIkFrame(fnum,side,0.0,0.0,0.0,0.0,ftype,ktype,NULL);
				}
				else if (strncmp(theLine.c_str(),"#EMOTE IkEfframe",16)==0) {
					sscanf(theLine.c_str(),"#EMOTE IkEfframe %d %d %lf %lf %lf %lf",
							&fnum, &side, &ef1, &ef2, &ef3, &ef4);
					pEmoteParams->AddEffortFrame(fnum,side,ef1,ef2,ef3,ef4);
					//space weight time flow
				}
				else if (strncmp(theLine.c_str(),"#EMOTE IkShframe",16)==0) {
					sscanf(theLine.c_str(),"#EMOTE IkShframe %d %d %lf %lf %lf %lf",
							&fnum, &side, &ef1, &ef2, &ef3, &ef4);
					pEmoteParams->AddShapeFrame(fnum,side,ef1,ef2,ef3,ef4);
					//horix vert sagg shapeflow
				}
				
			}
			*/
#endif
			continue;
			
		}
		if (theLine.size()==0) {
			continue;
		}
		// TOKENIZE LINE INTO WORDS
		theLine=theLine+ " ";
		words.clear();
		string::size_type pos=0, prev_pos=0; 
		cur=0;
		
		while ((pos=theLine.find_first_of(' ',pos))!=string::npos) {
			tempString = theLine.substr(prev_pos, pos-prev_pos);
			tempString = string(_strupr((char *)tempString.c_str()));
			if (tempString.length() != 0) {
				words.push_back(tempString);
			}
			prev_pos= ++pos;
		}
		int tokens = words.size();
		if(lines_read==0) {
			// handle header line
		}
		else if (lines_read%2==1){
			//handle mask line
			setBAPs.clear();
			for(unsigned int wI=0; wI<words.size(); wI++) {
				int length=words[wI].length();
				maskValue = atoi(words[wI].c_str());
				if(maskValue) {
					setBAPs.push_back(wI+1); // CHANGED CHECK
				}
			}
		}
		else { 
			
			//handle angle data line
			thisFrame = new BAPFrame(&this->converter,atoi(words[0].c_str()));
			for(unsigned int bI=0; bI<setBAPs.size(); bI++) {
				thisFrame->SetBAP(setBAPs[bI],atoi(words[bI+1].c_str()));
			}
			pFrames->push_back(thisFrame);

		}
		lines_read++;
	}

	cout<<". BAPParser::ParseString() - END - Read "<<pFrames->size()<<" Frames."<<endl;
	return 0;

}
