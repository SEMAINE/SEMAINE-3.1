//Copyright 1999-2005 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
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

***********************************************************
*IMPORTANT!! BEFORE RUNNING THE GRETA INTERFACE:          *
*                                                         *
*1. INSTALL THE FESTIVAL SPEECH SYSTEM (suggested path:   *
*C:\Festival)                                             *
*                                                         *
*2.SET UP FESTIVAL INSTALLATION PATH INTO GRETA.INI FILE  *
*(that is, open greta.ini file and set up "FESTIVAL_PATH" *
*to the path in which you have installed Festival; if you *
*have installed it into C:\Festival you do not need to    *
*anything);                                               *
*3. IN THE DIRECTORY 'C:\tmp', PLEASE MAKE SURE TO PUT THE*
*THE FILE apml.dtd THAT CAN BE FOUND IN THE 'runtime'     *
*DIRECTORY                                                *
***********************************************************

------------------------
Installation/Compilation
------------------------


(please note that this file explains how to install
and compile Greta and not how to use itself; to
know that please refer to the "documentation" folder)

first be sure to have installed Festival system;

extract the archive into your hd in a folder called "Greta";

Greta has been written using Visual C++. You can use either
Visual C++ 6 and Visual C++ 7.
Open the C++ solution (for VC 7) or workspace (for VC 6) file.
The files can be found in the directory 'GretaModular'.
They are GretaModular.sln when using VC 7 and
GretaModular_vc6.dsw when using VC 6.
Make sure that in the project property, the work directory
is set to '..\runtime'. 

Compile the project with "Build/Rebuild All".
The executable 'greta.exe' will be stored into the
directory 'runtime'.

To be able to create movies using the 'capture' command
of the Greta player, make sure to compile in the debug version.

Several tools are provided with the Greta project:
- FaceLibraryViewer
- FacePartsTool
- GestureEditor
- PoseEditor


These tools need to be compiled one by one by opening the
corresponding projects in the folders with the same name.