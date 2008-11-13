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


// HTML.h: interface for the HTMLPage and HTTPRequest classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__HTML__)
#define __HTML__

#include "standard.h"
#include "JTime.h"
#include "Collection.h"
#include "Dictionary.h"
#include "NetworkConnection.h"
#include "JFile.h"

namespace cmlabs {

class Bitmap;
class HTMLPage : public Object  
{
public:
	HTMLPage();
	HTMLPage(char* rawdata, int len, const JString& type);
	HTMLPage(Bitmap* bitmap);
	HTMLPage(const JString& filename);
	HTMLPage(XMLNode* node);
	virtual ~HTMLPage();

	bool fromXML(XMLNode* node);
	JString toXML();

	Object* clone() const;
	bool generateHTML();
	bool generateHTMLfromFile(const JString& filename);
	bool generateHTMLfromXML(const JString& xml);
	bool generateHTMLfromRawData();
	bool generateUseLocalCopyPage();
	bool generateMovedPermanentlyPage();
	static JString printErrorHTML(const JString& title = "", const JString& msg = "");
	bool readFile(const JString& filename, bool isBinary);
	bool setXMLContent(const JString& xml);
	bool setMovedPermanently(const JString& location);

	JString rootdir;
	JString htmlFilename;
	JString title;
	JString body;
	JString location;
	JString everything;
	JString frameset;
	JString xmlData;
	char* data;
	int datalength;
	char* rawData;
	int rawDataLength;
	JString rawDataType;

	JString autoRefreshPage;
	int autoRefresh;
	JString connectionType;
	bool keep_alive;

	bool noCaching;
	bool useLocalCopy;
	bool movedPermanently;

	JString bodyBGColor;
	JString bodyFGColor;

private:
	void init();
	JString generateHeader(int length);

};

class HTTPRequest : public Object  
{
public:
	HTTPRequest();
	HTTPRequest(const JString& xml);
	HTTPRequest(XMLNode* node);
	virtual ~HTTPRequest();

	Object* clone() const;
	bool fromXML(const JString& xml);
	bool fromXML(XMLNode* node);
	JString toXML();
	
	bool addEntry(const JString& str);
	bool addEntry(const JString& key, const JString& val);
	JString getEntry(const JString& str);

	JString getCommand();
	JString getRequestPage();
	JString getProtocol();
	JString getProtocolVersion();
	JString getParameter(const JString& key);

	bool shouldKeepAlive();
	bool isComplete(JSocket* socket);
	bool isComplete(NetworkConnection* con);
	bool createParameterDictionary(const JString& post);

	Dictionary entries;
	Dictionary paramdict;

private:
	void init();

	int postBytesStillRemaining;
	bool complete;
	bool keep_alive;
	JTime ifModifiedSince;

};

} // namespace cmlabs

#include "Bitmap.h"

#endif // __HTML__


