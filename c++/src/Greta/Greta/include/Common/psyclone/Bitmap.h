/***************************** License **********************************

 Copyright (C) 2006 by Communicative Machines
 http://www.cmlabs.com   All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

************************************************************************/


// Bitmap.h: interface for the Bitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BITMAP_H__7CFDF35A_B689_4481_8F4C_EF7B5AD00BE7__INCLUDED_)
#define AFX_BITMAP_H__7CFDF35A_B689_4481_8F4C_EF7B5AD00BE7__INCLUDED_

namespace cmlabs {

// #define GET_RGB_FROM_RGBA(a) (((unsigned char*)&a)[0] << 0) + (((unsigned char*)&a)[1] << 8) + (((unsigned char*)&a)[2] << 16)
#define GET_RGB_FROM_RGBA(a) (a & 0x00ffffff)
//#define GET_R_FROM_RGBA(a) (a & 0xff000000)
//#define GET_G_FROM_RGBA(a) (a & 0x00ff0000)
//#define GET_B_FROM_RGBA(a) (a & 0x0000ff00)
//#define GET_R_FROM_RGBA(a) (char)(a & 0x000000ff)
//#define GET_G_FROM_RGBA(a) (char)(a & 0x0000ff00)
//#define GET_B_FROM_RGBA(a) (char)(a & 0x00ff0000)
//#define GET_R_FROM_RGBA(a) ((unsigned char*)&a)[0]
//#define GET_G_FROM_RGBA(a) ((unsigned char*)&a)[1]
//#define GET_B_FROM_RGBA(a) ((unsigned char*)&a)[2]
#define GET_R_FROM_RGBA(a) (unsigned char)(a & 0x000000ff)
#define GET_G_FROM_RGBA(a) (unsigned char)((a & 0x0000ff00)>>8)
#define GET_B_FROM_RGBA(a) (unsigned char)((a & 0x00ff0000)>>16)
#define GET_RGB_FROM_GRAY(a) (unsigned int)(((unsigned int)a<<16) + ((unsigned int)a<<8) + (unsigned int)a)
#define GET_RGB_FROM_COMP(r,g,b) (unsigned int)((((unsigned int)b)<<16) + (((unsigned int)g)<<8) + (unsigned int)r)
#define GET_GRAY_FROM_RGB(a) (unsigned char)((GET_R_FROM_RGBA(a)+GET_G_FROM_RGBA(a)+GET_B_FROM_RGBA(a))/3)

// Red 0xF800 (5 bits of red) Green 0x07E0 (6 bits of green) Blue 0x001F (5 bits of blue) 
#define GET_DRGB_FROM_2RGBS(a,b) (((unsigned int)GET_R_FROM_RGBA(a)>>3)<<27) + (((unsigned int)GET_G_FROM_RGBA(a)>>2)<<21) + (((unsigned int)GET_B_FROM_RGBA(a)>>3)<<16) + (((unsigned int)GET_R_FROM_RGBA(b)>>3)<<11) + (((unsigned int)GET_G_FROM_RGBA(b)>>2)<<5) + (((unsigned int)GET_B_FROM_RGBA(b)>>3)<<0)
#define GET_FIRSTRGB_FROM_DRGB(a) ((unsigned int)((unsigned short)((a>>16)&0xF800)>>8)) | ((unsigned int)((unsigned short)((a>>16)&0xE000)>>13)) | ((unsigned int)((unsigned short)((a>>16)&0x07E0)<<5)) | ((unsigned int)((unsigned short)((a>>16)&0x0600))>>1) | ((unsigned int)((unsigned short)(a>>16)&0x001F)<<19) | (((unsigned int)(unsigned short)(a>>16)&0x001C)<<14)
#define GET_SECONDRGB_FROM_DRGB(a) ((unsigned int)((unsigned short)(a&0xF800)>>8)) | ((unsigned int)((unsigned short)(a&0xE000)>>13)) | ((unsigned int)((unsigned short)(a&0x07E0)<<5)) | ((unsigned int)((unsigned short)(a&0x0600))>>1) | ((unsigned int)((unsigned short)a&0x001F)<<19) | (((unsigned int)(unsigned short)a&0x001C)<<14)

} // namespace cmlabs

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Object.h"
#include "JString.h"
#include "JTime.h"
#include "Color.h"
#include <stdio.h>
#include <math.h>

#include "MathClasses.h"
#include "DataSample.h"

namespace cmlabs {

#define DATACTRL 1
#define RLECTRL 2
#define DIFCTRL 3

#ifndef WIN32
	typedef unsigned char BYTE;
	typedef unsigned int UINT;
	typedef unsigned int DWORD;
	typedef unsigned short WORD;
	typedef signed long LONG;

	#pragma pack(2)

	typedef struct tagBITMAPFILEHEADER {
		WORD    bfType;
		DWORD   bfSize;
		WORD    bfReserved1;
		WORD    bfReserved2;
		DWORD   bfOffBits;
	} BITMAPFILEHEADER;

	#pragma pack(2)
	typedef struct tagBITMAPINFOHEADER {
		DWORD   biSize;
		LONG    biWidth;
		LONG    biHeight;
		WORD    biPlanes;
		WORD    biBitCount;
		DWORD   biCompression;
		DWORD   biSizeImage;
		LONG    biXPelsPerMeter;
		LONG    biYPelsPerMeter;
		DWORD   biClrUsed;
		DWORD   biClrImportant;
	} BITMAPINFOHEADER;
	#pragma pack()
#endif

typedef struct tagRGBA {
	BYTE red;
	BYTE green;
	BYTE blue;
	BYTE alpha;
} valRGBA;

class Pixel {
public:
	Pixel() {x=0; y=0; value = NULL; }
	int x;
	int y;
	unsigned int* value;
};

class HTMLPage;
class BitmapUpdate;
class Bitmap : public DataSample
{
public:
	// Constructors
	Bitmap();
	Bitmap(int w, int h);
	Bitmap(char* imgData, int w, int h, bool takeData = false);
	Bitmap(const JString& filename);
	Bitmap(BitmapUpdate* update);
	virtual ~Bitmap();
	bool init(int w, int h);

	BitmapUpdate* operator -( const Bitmap &bitmap ) const;
	bool updateBitmap(BitmapUpdate* update, bool takeData);
	BitmapUpdate* runLengthEncode() const;
	BitmapUpdate* runLengthDestructiveEncode() const;

	Object* clone() const;
	bool reset();

	// Basic Functions
	bool setPixel(int x, int y, int red, int green, int blue, int alpha = 0);
	bool setPixel(long pos, int red, int green, int blue, int alpha = 0);
	bool setPixelXOR(int x, int y, int red, int green, int blue, int alpha = 0);
	bool setPixelXOR(long pos, int red, int green, int blue, int alpha = 0);

	Pixel getPixel(int x, int y);
	bool getPixel(Pixel* pixel);

	long getPos(int x, int y);
	double getDPos(double x, double y);
	Pixel getXY(long pos);

	Color getPixelColor(int x, int y);

	// Bitmap Functions
	bool eraseBitmap();
	bool eraseBitmap(int r, int g, int b, int a = 0);
	bool eraseBitmap(const Color& color);
	bool replaceColor(const Color& oldColor, const Color& newColor);

	bool fillBox(const Box& box, const Color& color);
	bool fillBox(int x, int y, int w, int h, const Color& color);

	bool putTransPixel(int x, int y, const Color& color, double weight);
	bool drawLine(const Line& line, const Color& color);
	bool drawLine(const PolyLine& polyline, const Color& color);
	bool drawBox(Box& box, const Color& color);
	bool drawCircle(int xCenter, int yCenter, int radius, const Color& color, double weight, double lineWidth, bool filled = false);
	bool putQuadPixel(int xCenter, int yCenter, int x, int y, const Color& color, double weight);

	bool drawBitmap(Bitmap* bitmap, int x, int y);
	bool drawBitmap(Bitmap* bitmap, int x, int y, int w, int h);
	bool drawBitmap(Bitmap* bitmap, const Point& p);
	bool drawBitmap(Bitmap* bitmap, const Box& box);
	bool drawBitmap(Bitmap* bitmap, int srcx, int srcy, int dstx, int dsty, int w, int h);

	Bitmap* getCopy(const Box& box);

	bool canBeResizedTo(int width, int height);
	Bitmap* getResizedCopy(int w, int h, bool proportional);
	Bitmap* getResizedCopy(double scale);
	bool resizeTo(int w, int h, bool proportional);
	bool copyDataFromBitmap(Bitmap* bitmap, bool shouldResize = false);
	bool takeDataFromBitmap(Bitmap* bitmap);
	double getBestResizeFactor(double factor);
	double getBestResizeFactors(double factor, int &scaleUp, int &scaleDown, int &scale66);
	char* createResizedData(int newWidth, int newHeight, int scaleUp, int scaleDown, int scale66);

	bool shape(int x1, int y1, int x2, int y2, int x3, int y3);
	bool shapeSlow(int x1, int y1, int x2, int y2, int x3, int y3);

	// Advanced Functions
	bool rotate(double angle);
	bool rotateSlow(double angle);

	// File Routines
	bool readFromFile(const JString& filename);
	bool saveToFile(const JString& filename);

	static bool convertBitmapFileData(char* src, int width, int height, int depth, char* dst, long dstlen);
	static char* convertBitmapFileDataRunLength(char* src, int width, int height, int depth, long &dstlen);
	static char* differenceBitmapFileData(char* orig, char* src, int width, int height, int depth, long &dstlen);
	static char* differenceBitmapFileDataRunLength(char* orig, char* src, int width, int height, int depth, long &dstlen);

	BitmapUpdate* getCompressedUpdate(bool destructive) const;

	char* getGrayScaleDataCopy(int &len);
	bool copyGrayScaleData(char* src, int len);

	int getMaxUpdates();
	bool setMaxUpdates(int max);
	bool resetBitmapUpdates();
	bool setBitmapUpdated();
	bool addBitmapUpdateRegion(const Box& updateBox);
	bool addBitmapUpdateRegion(Box* box);
	bool hasBitmapBeenUpdated();
	bool hasBitmapBeenTotallyChanged();

	// Public variables
//	JString name;
//	JTime timestamp;
//	int width;
//	int height;
//	int depth;
//	int byteDepth;
//	int byteWidth;
//	int planes;
//	long size;
//	char* data;

//	long* N;
//	long* P;
//	int* L;
//	Color bgColor;
//	Color fgColor;
//	Bitmap* altBitmap;
//	char* eraseLine;

	#ifdef WIN32
		bool drawOn(HDC hdc, int x, int y, int w, int h);
		bool copyFromHDC(HDC hdc, int x, int y, int w, int h);
//		char* createHBitmap();
//		HBITMAP hBitmap;
//		void* hBitmapData;
	#else
	#endif // WIN32

	// Math Helper Routines
	int round(double d);

	HTMLPage* toHTMLBitmap();
	char* toBitmapFileFormat(int& len);
	JString toHTML();
	JString toXML();
	bool fromXML(XMLNode* node);


protected:

//	bool hBitmapChanged;
//	bool bitmapChanged;
};

char* ImageResizeDown66(char* bytesource, int &cols, int &rows);
char* ImageResizeDown(char* bytesource, int &cols, int &rows, int factor);
char* ImageResizeUp66(char* bytesource, int &cols, int &rows);
char* ImageResizeUp(char* bytesource, int &cols, int &rows, int factor);

} // namespace cmlabs

#include "BitmapUpdate.h"
#include "HTML.h"

#endif // !defined(AFX_BITMAP_H__7CFDF35A_B689_4481_8F4C_EF7B5AD00BE7__INCLUDED_)
