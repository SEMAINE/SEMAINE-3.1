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

/////////////////////////////////////////////////////////////////////////////////////////
// Recording class
/////////////////////////////////////////////////////////////////////////////////////////
#include "DGLAVI.h"
#include <FL/gl.h>

/**@#-*/
using namespace GL2AVI;
/**@#+*/

CAVIRecord::CAVIRecord()
{
  dataBuffer = NULL;
  m_iFPS      = 25;
  m_iCurFrame = 0;
}

CAVIRecord::~CAVIRecord()
{
}


// Initialize and prepare an AVI file for writing
bool CAVIRecord::open(char *filename, int top, int bottom, int left, int right, int frameRateInFPS)
{
  ShowCursor(true);

  m_iFPS = frameRateInFPS;

  avifilename=filename;

  AVIFileInit();
  
  // FIX: When calling AVIFileOpen below with the OF_CREATE flag, the file is supposed to be reset to zero length
  //  if the file already exists. For some dumb reason, this doesn't happen, so let's do that ourselves...
  DeleteFile(filename);

  

  // Fill rect and check for valid values
  rect.top = top;
  rect.bottom = bottom;
  rect.left = left;
  rect.right = right;
  if (top == -1) return false;
  if (bottom == -1) return false;
  if (left == -1) return false;
  if (right == -1) return false;
  if (rect.top < 0)                     rect.top = 0;
  if (rect.bottom < 0)                  rect.bottom = 0;
  if (rect.left < 0)                    rect.left = 0;
  if (rect.right < 0)                   rect.right = 0;


  // Get width and height from rect
  m_iWidth = rect.right - rect.left;
  m_iHeight = rect.bottom - rect.top;

  /*
  if((m_iWidth%4)!=0)
  {
	  printf("width must be a multiple of 4\n");
		return 0;
  }

   if((m_iHeight%4)!=0)
  {
	  printf("height must be a multiple of 4\n");
		return 0;
  }*/

int r;
  r=0;
  // Create a new file for writing
  r=AVIFileOpen(&pAVIFile, filename, OF_CREATE|OF_WRITE, NULL);

  // Used to set AVI data format
	memset(&bmphead, 0, sizeof(bmphead));
	bmphead.biSize        = sizeof(BITMAPINFOHEADER);
	bmphead.biWidth       = m_iWidth;
	bmphead.biHeight      = m_iHeight;
	bmphead.biPlanes      = 1;
	bmphead.biBitCount    = 24;//era 24
  bmphead.biCompression = BI_RGB;

  // Set temp buffer size for storing pixel colours from backbuffer
  dataSize = m_iWidth * m_iHeight * (bmphead.biBitCount/8) * 2;
  dataBuffer = new unsigned char[dataSize];

  // Create the new video stream
  if(!createVidStream())
	  return false;

  ShowCursor(false);
  return true;
}

bool CAVIRecord::createVidStream(void)
{
  // Set stream information
  pAVIStreamVidInfo.fccType = streamtypeVIDEO;
  pAVIStreamVidInfo.dwScale = 1;
  pAVIStreamVidInfo.dwRate  = m_iFPS;
  pAVIStreamVidInfo.rcFrame = rect;

  // Create RAW stream
  AVIFileCreateStream(pAVIFile, &pAVIStreamVidRAW, &pAVIStreamVidInfo);

  // Set compression options (Open Dialog)
  AVICOMPRESSOPTIONS compressOpt;   // Holds options passed to AVISaveOptions
  AVICOMPRESSOPTIONS *pCompOpt;
  pCompOpt = &compressOpt;
  memset(pCompOpt, 0, sizeof(AVICOMPRESSOPTIONS));
  AVISaveOptions(NULL, 0, 1, &pAVIStreamVidRAW, &pCompOpt);

  // Create compressed stream using RAW stream
  if(AVIMakeCompressedStream(&pAVIStreamVidCOMP, pAVIStreamVidRAW, &compressOpt, NULL)!=AVIERR_OK)
  {
	  printf("error making compress stream.\n");
	  return false;
  }

  // Set format of compressed stream
  if(AVIStreamSetFormat(pAVIStreamVidCOMP, 0, &bmphead, bmphead.biSize)!=0)
  {
	  printf("error setting format of compress stream.\n");
	  return false;
  }


  return true;
}

// Writes current contents of backbuffer to a frame within the AVI file.
void CAVIRecord::writeFrame(void)
{
  if (dataBuffer == NULL)
    return;
  
  // Grab backbuffer contents
  glReadPixels(rect.left, rect.top, m_iWidth, m_iHeight, GL_RGB, GL_UNSIGNED_BYTE, dataBuffer);

  // Swap R and B
  for (unsigned int i=0; i<dataSize-2; i+=3)
  {
    unsigned char temp = dataBuffer[i];
    dataBuffer[i] = dataBuffer[i+2];
    dataBuffer[i+2] = temp;
  }
/*
    int r;
r=AVIERR_UNSUPPORTED;
r=AVIERR_BADFORMAT;
r=AVIERR_MEMORY;
r=AVIERR_INTERNAL;
r=AVIERR_BADFLAGS;
r=AVIERR_BADPARAM;
r=AVIERR_BADSIZE;
r=AVIERR_BADHANDLE;
r=AVIERR_FILEREAD;
r=AVIERR_FILEWRITE;
r=AVIERR_FILEOPEN;
r=AVIERR_COMPRESSOR;
r=AVIERR_NOCOMPRESSOR;
r=AVIERR_READONLY;
r=AVIERR_NODATA;
r=AVIERR_BUFFERTOOSMALL;
r=AVIERR_CANTCOMPRESS;
r=AVIERR_USERABORT;
r=AVIERR_ERROR;        
*/
  //Add data to AVI stream
LONG res=AVIStreamTimeToSample(pAVIStreamVidCOMP,m_iCurFrame*40);

  AVIStreamWrite(pAVIStreamVidCOMP, res, 1, (LPVOID)dataBuffer, dataSize, AVIIF_KEYFRAME, NULL, NULL);
  m_iCurFrame++;
}

// Closes and finalizes the AVI file
void CAVIRecord::close(void)
{
  if (dataBuffer != NULL)
  {
    delete[] dataBuffer;
    dataBuffer = NULL;
  }
    AVIStreamClose(pAVIStreamVidCOMP);
	  AVIStreamClose(pAVIStreamVidRAW);
  	AVIFileClose(pAVIFile);
	  AVIFileExit();
}

int CAVIRecord::add_wav(const char* strFileWAV)
{
HRESULT hr;
PAVIFILE pfileWav = NULL;
AVISTREAMINFO siWav;
PAVISTREAM psWav = NULL, psAvi = NULL;
PCMWAVEFORMAT fmtWav;
long lFmtSize, lStreamLength;
byte *lpbData = NULL;

hr = AVIFileOpen(&pAVIFile, avifilename.c_str(), OF_WRITE, NULL);
if(hr != 0)
	return 0;

hr = AVIFileOpen(&pfileWav, strFileWAV, OF_READ, NULL);
if(hr != 0)
	return 0;

hr = AVIFileGetStream(pfileWav, &psWav, streamtypeAUDIO, 0);
if(hr != 0)
	return 0;

hr = AVIStreamInfo(psWav, &siWav, sizeof(AVISTREAMINFO));

lFmtSize=0;

hr = AVIStreamFormatSize(psWav, 0, &lFmtSize);


int sizesize=sizeof(fmtWav);
if(lFmtSize > sizeof(fmtWav))
return 0;

hr = AVIStreamReadFormat(psWav, 0, &fmtWav, &lFmtSize);

// Read WAV data
lStreamLength = AVIStreamLength(psWav)*2;
lpbData = new byte[lStreamLength];
hr = AVIStreamRead(psWav, 0, lStreamLength, lpbData, lStreamLength, NULL, NULL);
if(hr != 0)
return 0;

// Create an AVI stream with WAV stream info in AVI file
hr = AVIFileCreateStream(pAVIFile, &psAvi, &siWav);
if(hr != 0)
return 0;
hr = AVIStreamSetFormat(psAvi, 0, &fmtWav, sizeof(fmtWav));

// Write WAV data to AVI stream
LONG res=AVIStreamTimeToSample(psWav,0);
hr = AVIStreamWrite(psAvi, res, lStreamLength, lpbData, lStreamLength, AVIIF_KEYFRAME, NULL, NULL);
if(hr != 0)
return 0;

AVIStreamRelease(psAvi);
delete lpbData;
AVIStreamRelease(psWav);
AVIFileRelease(pfileWav);

AVIFileClose(pAVIFile);

AVIFileExit();

return 1;
}