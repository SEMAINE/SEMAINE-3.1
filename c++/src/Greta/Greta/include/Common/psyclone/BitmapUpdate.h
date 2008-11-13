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


// BitmapUpdate.h: interface for the BitmapUpdate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_BITMAPUPDATE_H_)
#define _BITMAPUPDATE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Bitmap.h"

namespace cmlabs {

class BitmapUpdate : public DataSample
{
public:
	// Constructors
	BitmapUpdate();
	BitmapUpdate(int w, int h);
	BitmapUpdate(JString filename, bool compress);
	virtual ~BitmapUpdate();
	bool init(int w, int h);
	bool reset();

	bool update(BitmapUpdate* update);

	Bitmap* toBitmap();
	HTMLPage* toHTMLBitmap();

//	bool setWidth(int width);
//	int getWidth();
//	bool setHeight(int height);
//	int getHeight();
	bool setUpdateType(JString updateType);
	JString getUpdateType();

	bool setFullUpdate(Bitmap* bitmap);
	bool readFromFile(JString filename);
	bool readFromFileRLE(JString filename);
	bool readFromFileDiff(JString filename, Bitmap* oldBitmap);
	bool readFromFileRLEDiff(JString filename, Bitmap* oldBitmap);
};

} // namespace cmlabs

#endif // _BITMAPUPDATE_H_
