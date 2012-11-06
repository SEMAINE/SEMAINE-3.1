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


/*

 central data Memory

*/

#define DATAMEMORY_INITIAL_LEVELS     50
#define DATAMEMORY_INITIAL_NAMES      50

/*
A note on LEVELs:
  There are two types of levels:
        A) Periodic levels
        B) Non-periodic levels
  Each level can be organized internally as
       A) ring buffer
       B) full buffer
       
  For each level, there is a data matrix and a meta-data array, which
   contains timing information etc. (basically cDataFrame stuff)
*/

class cDataMemoryLevel {
  public:
    char *name;
    long  id;
    
    // parameters of level:
    double    period; // period == 0 indicates a non-periodic level
    double    length;
    double    start;
    long long n;      // total number of values (rows * columns)
    long      r,c;    // rows (number of elements), columns (=length in frames)
    
    // config of level: (names,...)
    long         nNamesAlloc; // currently # of allocated names (used during dynamic config and reallocating)
    char      ** name;        // r names (one for each row)
    int          configDone; // config done flag (set to 1 after config is finished, i.e. allocateData was called
    
    // data (ring)buffer:
    int          bufferType; // 0 = ringbuffer, 1=full
    cDataFrame * meta;       // meta information for each time frame
    FL_DATA    * data;       // HUGE(!) data matrix array (organised columnwise)
    // ring buffer pointers:
    
    // read/write pointers for all the elements:

   /***********************************************/

   // create the data memory level without specifiying its size... the size must be set via allocateData!
   cDataMemoryLevel(char * _name, int _id, int _fullBuffer=0);
   // create a new level with "name" and "id" (note... ids should be unique-> they will be managed by the cDataMemory class)
   cDataMemoryLevel(char * _name, int _id, double _length, double _period, int _fullBuffer=0);
   // .. same as above, except that length has to be specified in frames ..
   cDataMemoryLevel(char * _name, int _id, long _lengthFrames, double _period, int _fullBuffer=0);

   int registerWriter(int _n, char **_name);   // [addNames] "register" a data writer for this level (i.e. set up the names, etc.)
                                              // add "n" new data elements with names given in **name
   int registerWriter(int _n, cDataDesc *_desc);   // [addNames] "register" a data writer for this level (i.e. set up the names, etc.)
                                                  // add "n" new data elements with names given in *desc

   // allocate the space for actual data, after everything has been configured, then set configDone=1
   // (this method may also be used to reallocate the data. -> TODO)
   // length = 0.0 ==> use length set via constructor , else override initial length
   // period = -1.0  ==> use period set via constructor , else override initial period
   // AN exception will be raised, if no length and period is specified and it has not been set via the constructor!
   void allocateData(double length=0.0, double period=-1.0);
   // same as above, only that the length is to be specified in number of frames
   void allocateData(long lengthFrames=0, double period=-1.0);


   void clearAndReset();    // completely deconfigure the level and free ALL data
   virtual ~cDataMemoryLevel();
};


/* A note on frame indicies:
     all indicies are absolute (from 0)
     all times are relative to beginning of (virtual) buffer
//    a ringbuffer appears as infinitely long virtual buffer...
 */
class cDataMemory: public cComponent {
  public:
    cDataMemory(int nLevelsInit=DATAMEMORY_INITIAL_LEVELS);
    
    // time or absIdx < 0  : append frame/sequence/matrix at end...

    //.... raw writers
    writeFrameRaw(cDataMask *_mask, cDataFrame *_frame, long _absIdx=-1);
    writeFrameRaw(cDataMask *_mask, cDataFrame *_frame, double _time=-1.0);

    writeSequenceRaw(sDataElementID *_id, cDataSequence *_sequence, long _absIdx=-1);
    writeSequenceRaw(sDataElementID *_id, cDataSequence *_sequence, double _time=-1.0);

    writeMatrixRaw(cDataMask *_mask, cDataMatrix *_matrix, long _absIdx=-1);
    writeMatrixRaw(cDataMask *_mask, cDataMatrix *_matrix, double _time=-1.0);

    //.... raw readers
    cDataFrame * readFrameRaw(cDataMask *_mask, long _absIdx=-1);
    cDataFrame * readFrameRaw(cDataMask *_mask, double _time=-1.0);

    cDataSequence * readSequenceRaw(sDataElementID *_id, long _absIdx, long _nFrames);
    cDataSequence * readSequenceRaw(sDataElementID *_id, double _time, long _length);


    // event based write/read & registration of writers/readers:
    // NOTE: even if an application uses Raw R/W functions, it should register during configuration phase to obtain a valid data mask! This is especially true fro writers, because the data elements will not be reserved in the memory, if the writer does not register!
    registerWriter(cDataWriter *_writer);  // register a writer for a certain level with certain data elements and names
                       // writing to a registered writer, will trigger events that will push data to registered readers
    registerReader(cDataReader *_reader);  // register a reader for certain data elements, if values are added to these elements, the reader will receive a data event

	virtual ~cDataMemory();
		   
  private:
  		  
			
  protected:
    int nLevels;             // actual number of levels
    int nLevelsAlloc;        // size of level pointer array
	cDataMemoryLevel *level; // actual data for each level

	readers                  // information and status of registered readers
	writers                  // information and status of registered writers
};
