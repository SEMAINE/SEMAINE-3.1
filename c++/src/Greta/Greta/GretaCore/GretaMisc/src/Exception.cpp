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

//////////////////////////////////////////////////////////////////////
// created : 21-05-08  by : Radek  (niewiadomski@iut.univ-paris8.fr)

//modifications:
//

#include "Exception.h"

Exception::Exception()
{
	this->message="Unknown exception";
	this->code=-1;
}

Exception::Exception(std::string message)
{
	this->message=message;
	this->code=-1;
}

Exception::Exception(std::string message, int code){
	this->message=message;
	this->code=-code;
}
Exception::~Exception()
{
}
std::string Exception::getMessage()
{ 
	return message;
}


int Exception::getCode()
{
	return code;
}


