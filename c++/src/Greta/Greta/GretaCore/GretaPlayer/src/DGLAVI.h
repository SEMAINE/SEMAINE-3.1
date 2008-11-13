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

#pragma once

#include "windows.h"
#include "vfw.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////////////
// De-compression (playback)
///////////////////////////////////////////////////////////////////////////////////////

//! AVI Animation playback (Fullscreen)
/*! I know this is obvious, but this decompression stuff is pretty CPU intensive, if you notice
    a slow down, really choppy performance, make sure you've linked to the release build of the lib.  */
//! AVI Animation recording
/*!  */
/**@#-*/
namespace GL2AVI{
/**@#+*/

class CAVIRecord
{
public:
  CAVIRecord();
  ~CAVIRecord();

  //! Initialize and prepare an AVI file for writing
  bool open(char *filename,       /*!< The AVI filename                                                           */
            int top,              /*!< Top most pixel position of the area to capture. A value of -1 = top.       */
            int bottom,           /*!< Bottom most pixel position of the area to capture. A value of -1 = bottom. */
            int left,             /*!< Left most pixel position of area to capture. A value of -1 = far left      */
            int right,            /*!< Right most pixel position of area to capture. A value of -1 = far right    */
            int frameRateInFPS    /*!< Playback framerate of the AVI file                                         */
            );

  //! Writes current contents of backbuffer to a frame within the AVI file.
  void writeFrame(void);

  int add_wav(const char* strFileWAV);

  int getFrameNum(void){  return m_iCurFrame; }

  int getFPS(void){ return m_iFPS;  }

  //! Closes and finalizes the AVI file
  void close(void);
private:

	std::string avifilename;
  //! creates a new video stream to an already opened AVI file
  bool createVidStream(void);
								  
  unsigned char   *dataBuffer;      // Stores contents of backbuffer from glReadPixels()
  unsigned int    dataSize;         // Size of the above buffer
  RECT            rect;             // Holds rectangle of buffer to copy
  int             m_iWidth;         // Width of AVI
  int             m_iHeight;        // Height of AVI
  int             m_iFPS;           // Frame rate of AVI file
  unsigned long   m_iCurFrame;      // Holds current frame number needed when writing
  PAVIFILE        pAVIFile;         // File
  PAVISTREAM      pAVIStreamVidRAW; // Pointer to an open AVI stream (RAW Video)
  PAVISTREAM      pAVIStreamVidCOMP;// Pointer to an open AVI stream (Compressed Video)
  AVISTREAMINFO   pAVIStreamVidInfo;// Holds information about the opened AVI stream (Video)
  BITMAPINFOHEADER bmphead;         // Bitmap header used to define format of AVI during it's creation
};

/**@#-*/
} //end namespace
/**@#+*/