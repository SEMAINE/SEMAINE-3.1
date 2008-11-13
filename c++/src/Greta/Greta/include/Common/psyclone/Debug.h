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


#if defined(_WIN32_WCE)
	#undef DEBUG
#endif

namespace cmlabs {

#define DEBUGLEVEL(n)	(debugLevel >= n)
#define STATUS			 1
#define DEBUG			 2
#define DETAILEDSTATUS	 3
#define MOREDETAILED	 4
#define KITCHENSINK	     5
#define HEARTBEAT	     6
#define VERBOSENESS(m)  (verboseness >= m)
#define LEVEL1			1
#define LEVEL2			2
#define LEVEL3			3
#define LEVEL4          4
#define LEVEL5          5
#define LEVEL6          6
#define LEVEL7          7
#define LEVEL8          8
#define LEVEL9          9
#define LEVEL10        10
extern int debugLevel;
extern int verboseness;

} // namespace cmlabs
