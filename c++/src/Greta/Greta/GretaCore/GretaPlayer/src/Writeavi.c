/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (C) 1992 - 1996 Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
/****************************************************************************
 *
 *  WRITEAVI.C
 *
 *  Creates the file OUTPUT.AVI, an AVI file consisting of a rotating clock
 *  face.  This program demonstrates using the functions in AVIFILE.DLL
 *  to make writing AVI files simple.
 *
 *  This is a stripped-down example; a real application would have a user
 *  interface and check for errors.
 *
 ***************************************************************************/
#include "writeavi.h"
#include <stdlib.h>
#include <stdio.h>

int xDim;
int yDim;

char FName[256];

AVISTREAMINFO strhdr;
PAVIFILE pfile=0;
PAVISTREAM ps;
PAVISTREAM psCompressed;
PAVISTREAM psText;
AVICOMPRESSOPTIONS opts;
AVICOMPRESSOPTIONS FAR * aopts[1];

AVICOMPRESSOPTIONS compressOpt;   // Holds options passed to AVISaveOptions
AVICOMPRESSOPTIONS *pCompOpt;

DWORD dwTextFormat;
int nFrames=0;
int bOK;

void AviInit(char *avifilename, int xdim, int ydim)
{
	WORD wVer;

	memset(FName,0,256);
	strcpy(FName,avifilename);
	xDim=xdim;
	yDim=ydim;
	bOK=1;
	nFrames=0;

	pfile = NULL;
	ps = NULL;
	psCompressed = NULL;
	psText = NULL;
	aopts[0] = &opts;
	wVer = HIWORD(VideoForWindowsVersion());
	if (wVer < 0x010A)
	{
		// oops, we are too old, blow out of here
		bOK = 0;
	}
	else
	{
		AVIFileInit();
	}
	return;
}

int AddFrame(HANDLE hDib)
{
	HRESULT hr;
	LPBITMAPINFOHEADER alpbi;
	if (!bOK)
		return 0;
	alpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	if (alpbi == NULL)
		return 0;
	if (xDim>=0 && xDim != alpbi->biWidth)
	{
		GlobalFreePtr(alpbi);
		return 0;
	}
	if (yDim>=0 && yDim != alpbi->biHeight)
	{
		GlobalFreePtr(alpbi);
		return 0;
	}
	xDim = alpbi->biWidth;
	yDim = alpbi->biHeight;

	hr = AVIERR_OK;

	if (pfile == 0)
	{

		hr = AVIFileOpen(&pfile,		    // returned file pointer
			    FName,							// file name
				OF_WRITE | OF_CREATE,		    // mode to open file with
				NULL);							// use handler determined
												// from file extension....
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			bOK = 0;
			return 0;
		}
		_fmemset(&strhdr, 0, sizeof(strhdr));
		strhdr.fccType                = streamtypeVIDEO;// stream type
		strhdr.fccHandler             = 0;
		strhdr.dwScale                = 1;
		strhdr.dwRate                 = 25;		    // 15 fps
		strhdr.dwSuggestedBufferSize  = alpbi->biSizeImage;
		SetRect(&strhdr.rcFrame, 0, 0,		    // rectangle for stream
			(int) alpbi->biWidth,
			(int) alpbi->biHeight);

		// And create the stream;
		hr = AVIFileCreateStream(pfile,		    // file pointer
						         &ps,		    // returned stream pointer
								 &strhdr);	    // stream header
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			bOK = 0;
			printf("error creating avi stream\n");
			return 0;
		}

		_fmemset(&opts, 0, sizeof(opts));

		
		pCompOpt = &compressOpt;
		memset(pCompOpt, 0, sizeof(AVICOMPRESSOPTIONS));

		if (!AVISaveOptions(NULL, 0, 1, &ps, &pCompOpt))
		{
			GlobalFreePtr(alpbi);
			bOK = 0;
			printf("error creating avi save options\n");
			return 0;
		}

		hr = AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL);
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			bOK = 0;
			printf("error creating avi compressed stream\n");
			return 0;
		}

		hr = AVIStreamSetFormat(psCompressed, 0,
					   alpbi,	    // stream format
				       alpbi->biSize +   // format size
				       alpbi->biClrUsed * sizeof(RGBQUAD));
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			bOK = 0;
			printf("error setting avi stream format\n");
			return 0;
		}

		// Fill in the stream header for the text stream....

		// The text stream is in 60ths of a second....
/*
		_fmemset(&strhdr, 0, sizeof(strhdr));
		strhdr.fccType                = streamtypeTEXT;
		strhdr.fccHandler             = mmioFOURCC('D', 'R', 'A', 'W');
		strhdr.dwScale                = 1;
		strhdr.dwRate                 = 60;
		strhdr.dwSuggestedBufferSize  = sizeof(szText);
		SetRect(&strhdr.rcFrame, 0, (int) alpbi->biHeight,
			(int) alpbi->biWidth, (int) alpbi->biHeight + TEXT_HEIGHT);

		// ....and create the stream.
		hr = AVIFileCreateStream(pfile, &psText, &strhdr);
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			bOK = false;
			return false;
		}

		dwTextFormat = sizeof(dwTextFormat);
		hr = AVIStreamSetFormat(psText, 0, &dwTextFormat, sizeof(dwTextFormat));
		if (hr != AVIERR_OK)
		{
			GlobalFreePtr(alpbi);
			bOK = false;
			return false;
		}
*/
	}

	// Jetzt eigentliches Schreiben
	hr = AVIStreamWrite(psCompressed,	// stream pointer
		nFrames,				// time of this frame
		1,				// number to write
		(LPBYTE) alpbi +		// pointer to data
			alpbi->biSize +
			alpbi->biClrUsed * sizeof(RGBQUAD),
			alpbi->biSizeImage,	// size of this frame
		AVIIF_KEYFRAME,			 // flags....
		NULL,
		NULL);
	if (hr != AVIERR_OK)
	{
		GlobalFreePtr(alpbi);
		bOK = 0;
		printf("error writing on avi stream\n");
		return 0;
	}

	// Make some text to put in the file ...
	//LoadString(hInstance, IDS_TEXTFORMAT, szMessage, BUFSIZE );
	/*
	strcpy(szMessage, "This is frame #%d");
		
	int iLen = wsprintf(szText, szMessage, (int)(nFrames + 1));

	// ... and write it as well.
	hr = AVIStreamWrite(psText,
			nFrames * 40,
			1,
			szText,
			iLen + 1,
			AVIIF_KEYFRAME,
			NULL,
			NULL);
	if (hr != AVIERR_OK)
	{
		GlobalFreePtr(alpbi);
		bOK = 0;
		return 0;
	}
	*/
	GlobalFreePtr(alpbi);

	nFrames++;
	return 1;
}

//
// Method
//
int AddWAV(char *strFileWAV)
{
    HRESULT hr;
    PAVIFILE pfileWav = NULL;
    AVISTREAMINFO siWav;
    PAVISTREAM psWav = NULL, psAvi = NULL;
    PCMWAVEFORMAT fmtWav;
    long lFmtSize, lStreamLength;
    unsigned char *lpbData = NULL;
	FILE *wavfile;
	long wavfilesize;

	wavfile=fopen(strFileWAV,"rb");
	if (!wavfile)
		return 0;
	fseek(wavfile,0L,SEEK_END);
	wavfilesize=ftell(wavfile);
	fclose(wavfile);


    hr = AVIFileOpen(&pfileWav, strFileWAV, OF_READ, NULL);
    if(hr != 0)
        return 0;

    hr = AVIFileGetStream(pfileWav, &psWav, streamtypeAUDIO, 0);
    if(hr != 0)
        goto addwav_error;

    hr = AVIStreamInfo(psWav, &siWav, sizeof(AVISTREAMINFO));

    hr = AVIStreamFormatSize(psWav, 0, &lFmtSize);
    if(lFmtSize > sizeof(fmtWav))
        goto addwav_error;
    hr = AVIStreamReadFormat(psWav, 0, &fmtWav, &lFmtSize);

    // Read WAV data
    lStreamLength = AVIStreamLength(psWav);
    lpbData = (unsigned char*)malloc(wavfilesize);
	memset(lpbData,0,wavfilesize);

    hr = AVIStreamRead(psWav, 0, lStreamLength, lpbData, wavfilesize, NULL, NULL);
    if(hr != 0)
        goto addwav_error;

    // Create an AVI stream with WAV stream info in AVI file
    hr = AVIFileCreateStream(pfile, &psAvi, &siWav);
    if(hr != 0)
        goto addwav_error;
    hr = AVIStreamSetFormat(psAvi, 0, &fmtWav, sizeof(fmtWav));

    // Write WAV data to AVI stream
    hr = AVIStreamWrite(psAvi, 0, AVISTREAMREAD_CONVENIENT, lpbData, wavfilesize, AVIIF_KEYFRAME, NULL, NULL);
    if(hr != 0)
        goto addwav_error;

    AVIStreamRelease(psAvi);
    free(lpbData);
    AVIStreamRelease(psWav);
    AVIFileRelease(pfileWav);

    return 1;

addwav_error:
    if(psAvi)
        AVIStreamRelease(psAvi);
    if(lpbData)
        free(lpbData);
    if(psWav)
        AVIStreamRelease(psWav);
    if(pfileWav)
        AVIFileRelease(pfileWav);

    return 0;
}

void AviClose()
{
	WORD wVer;

	if (ps)
		AVIStreamClose(ps);

	if (psCompressed)
		AVIStreamClose(psCompressed);

	if (psText)
		AVIStreamClose(psText);

	if (pfile)
		AVIFileClose(pfile);

	wVer = HIWORD(VideoForWindowsVersion());
	if (wVer >= 0x010A)
	{
		AVIFileExit();
	}
}
