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


#include "RealtimeBMLPsydule.h"
#include "IniManager.h"
#include "DataContainer.h"
#include "../../../include/common/definitions.h"

DataContainer *datacontainer;
IniManager inimanager;
std::string ini_filename;

int main(int argc, char ** argv)
{
	
	JString GretaName;
	JString host;
	int port;
	
	if(argc==2)
	{
		ini_filename=argv[1];
	} else {
		ini_filename="greta.ini";	
	}

	inimanager.ReadIniFile(ini_filename);
	datacontainer = new DataContainer();
	
	int code=datacontainer->initBMLEngine();
	if (code==0) {
		printf("Problem : out \n");
		exit(1);
	}

	host=(JString)inimanager.GetValueString("PSYCLONE_HOST").c_str();
	port=inimanager.GetValueInt("PSYCLONE_PORT");
	GretaName=(JString)inimanager.GetValueString("GRETA_NAME").c_str();
	

	RealtimeBMLPsydule bmlengine(GretaName+REALTIMEBMLPSYDULE_MODULE_NAME, host, port, GretaName);

	bmlengine.MainLoop();

	return 1;
}
