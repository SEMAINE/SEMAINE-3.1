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


// JTime.h: interface for the JTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIME_H__33DD983B_D556_470C_80BD_2BE37411D47A__INCLUDED_)
#define AFX_TIME_H__33DD983B_D556_470C_80BD_2BE37411D47A__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "standard.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#if defined(WIN32_POCKETPC)

	#include "wce_time.h"

#else
	#include <sys/timeb.h>
#endif
#include "Object.h"
#include "JString.h"

#ifndef WIN32
	#include <sys/time.h>
#endif

namespace cmlabs {

class JTime : public Object  
{
public:
	#ifdef WIN32
		JTime(FILETIME filetime);
		static FILETIME timeb2filetime(struct timeb tb);
		static FILETIME time_t2filetime(time_t t);
		static long filetime2time_t(FILETIME * ptr);
		static struct timeb filetime2timeb(FILETIME * ptr);
	#endif
	JTime();
	JTime(const JTime &t);
	JTime(struct timeb tb);
	JTime(const JString& text);
	JTime(XMLNode* node);
	JTime(long ms);
	JTime(long sec, int milli);
	~JTime();
	// virtual ~JTime();
	Object* clone() const;
	unsigned long getHash() const;
	bool reset();
	bool perfCountReset();
	bool equals(const Object* o2) const;

	bool unitTest();
	JString toXML() ;
	JString toXML(const JString& tagname, const JString& params = "") ;
	bool fromXML(XMLNode* node);

	bool setTime(int year, int month, int day, int hour, int min, int sec, int ms = 0);
	bool setLocalTimeZoneInformation();
	JString getLocalTimeZoneInformation() const;
	JString getLocalTimeZoneName() const;
	int getLocalTimeZoneOffset() const;
	int getLocalTimeZoneDST() const;
	JString getOriginalTimeZoneName() const;
	int getOriginalTimeZoneOffset() const;
	int getOriginalTimeZoneDST() const;
	int getTimeDifference() const;
	int getTimeDifference(const JTime &t) const;

	bool operator>(const JTime &t) const;
	bool operator>=(const JTime &t) const;
	bool operator<(const JTime &t) const;
	bool operator<=(const JTime &t) const;
	bool operator==(const JTime &t) const;
	bool operator!=(const JTime &t) const;
	const JTime& operator-=(long millis);
	const JTime& operator+=(long millis);
	int compare(const Object* o2) const;

	long operator-(const JTime &t) const;
	JTime operator+(long millis) const;
	JTime operator-(long millis) const;

	bool addMilliseconds(long millis);
	long microDifference(const JTime& t) const;

	long getAge() const;
	long getMicroAge() const;
	bool isOlderThan(long ms) const;

	//! Nice long date and time
	JString print() ;
	//! Only time part (hour, minute, second, ms)
	JString printTime() const;
	//! Only time part (hour, minute, second)
	JString printTimeSec() const;
	//! Only time part (hour, minute, second, ms)
	JString printTimeMS() const;
	//! Very short date print
	JString printDateShort() const;
	//! Sortable yyyymmdd
	JString printDateSortable() const;
	//! Nice long date
	JString printDateLong() const;
	//! Sortable date and time
	JString printStamp() const;
	//! HH:MM:SS.ms and days, months, years as needed
	JString printDifTime() const;
	
	JString toHTTP() const;
	bool fromHTTP(const JString& text);

	//! Return full time in ms
	double ms() const;
	//! Returns full time in ms as string
	JString asText() const;

	//! Return sec part of time only
	long seconds() const;
	//! Return ms part of time only
	int milliseconds() const;

	void setInvalid();
	bool isValid() const;
	bool valid;

	struct timeb timebuffer;
	JString tzc;
	int atomicOffset;

	#ifdef WIN32
		LARGE_INTEGER perfCount;
	#else
		struct timeval perfCount;
	#endif

private:
};



} // namespace cmlabs

#include "Collections.h"

namespace cmlabs {

class StatSample;

class StopWatch : public Object  
{
public:
	StopWatch();
	virtual ~StopWatch();

	Object* clone() const;

	bool reset();
	bool start();
	bool stop();
	bool setMark();
	bool setMark(const JString& label);
	JString print();
	JString printNoLabels();
	JString printDetails();

	long getTotalTime();
	long getMarkCount();
	long getAverageDistance();
	long getStandardDeviationDistance();
	StatSample getStatSample();

	ObjectDictionary marks;
	JTime timeStarted;
	JTime timeStopped;
};

} // namespace cmlabs

#include "MathClasses.h"

#endif // !defined(AFX_TIME_H__33DD983B_D556_470C_80BD_2BE37411D47A__INCLUDED_)
