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


// JFile.h: interface for the JFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__JFILE__)
#define __JFILE__


#include "standard.h"
#if defined(CYGWIN)
	#include <unistd.h>
#endif
#ifdef WIN32
	#if !defined(_WIN32_WCE)
		#include <io.h>
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <fcntl.h>
		#if !defined(CYGWIN)
			#include <direct.h>
		#endif
	#endif
	#include "direntwin.h"
#else
	#include <limits.h>
	#include <stdlib.h>
	#ifdef Darwin
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <sys/dir.h>
		#include <fcntl.h>
	#else
		#include <dirent.h>
		//	#include <sys/vfs.h>
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <fcntl.h>
		#include <errno.h>
	#endif // Darwin
#endif // WIN32

#include "Object.h"
#include "JTime.h"
#include "Collection.h"

namespace cmlabs {

class JFile : public Object {
public:

	static JString readAFileASCII(const JString& name);
	static char* readAFileASCII(const JString& name, int &length);
	static char* readAFileBinary(const JString& name, int &length);
	static bool readAFileASCII(const JString& name, char* data, int &length);
	static bool readAFileBinary(const JString& name, char* data, int &length);

	static bool writeAFileASCII(const JString& name, const JString& text);
	static bool writeAFileASCII(const JString& name, char* str, int length);
	static bool writeAFileBinary(const JString& name, char* str, int length);

	static bool appendToAFileASCII(const JString& name, const JString& text);
	static bool appendToAFileASCII(const JString& name, char* str, int length);

	static Collection getFilesInADir(const JString& name);
	static bool deleteAFile(const JString& name);
	static bool deleteADir(const JString& name, bool recursive);

	static unsigned long getAFileSize(const JString& name);
	static bool doesAFileExist(const JString& name);

	static bool createADir(const JString& fname);
	static bool createADir(const JString& fname, bool recursive);

// ***********************************************************

	JFile(const JString& filedirname);
	~JFile(void);

	Object* clone() const;
	bool createFile(bool asBinary);
	bool createDir(bool recursive = false);
	bool openFile(bool asBinary);
	bool closeFile();

	bool deleteFile(bool force = false);
	bool deleteDir(bool recursive = false);

	bool lock();
	bool unlock();
	bool flush();

	bool truncate();

	bool gotoStart();
	bool gotoEnd();
	bool gotoPos(int pos);
	bool isAtEOF();
	bool isInError();

	bool isValid();
	bool exists();
	bool isADir();
	bool isAFile();

	Collection getFilesInDir();

	char* readBinary(int &length);
	JString readAscii(int &length);
	JString readLine();
	bool writeBinary(char* str, int &length);
	bool writeAscii(const JString& str);
	bool writeLine(const JString& str);

	int getSize();
	bool isFileReadable();
	bool isFileWritable();
	bool isFileExecutable();
	bool isFileBinary();

	JTime getLastAccessTime();
	JTime getLastModifyTime();
	JTime getCreationTime();

	JString getFileNameOnly();
	JString getDirNameOnly();
	JString getFullPathName();

	// Basic Interface
	bool deleteFile(const JString& name);
	bool deleteDir(const JString& name);
	bool changeAttr(const JString& name, bool read, bool write);
//	bool createDir(const JString& name);

protected:
	bool doesExist;
	bool isDirectory;
	bool isReadable;
	bool isWritable;
	bool isExecutable;
	int isBinary;
	int size;
	bool isEOF;

	JString name;
	JTime lastAccessTime;
	JTime lastModifyTime;
	JTime creationTime;

	FILE* fileptr;

	bool collectFileInfo();

};

} // namespace cmlabs

#endif // __JFILE__
