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


#include "EngineParameter.h"

EngineParameter::EngineParameter()
{
	InitValue(-1.0, 0.0, 1.0);
}

EngineParameter::EngineParameter(float new_lower, float new_upper)
{
	InitValue(new_lower, (new_lower + new_upper) * 0.5f, new_upper);
}

EngineParameter::EngineParameter(float new_lower, float new_center, float new_upper)
{
	InitValue(new_lower, new_center, new_upper);
}

void EngineParameter::InitValue(float new_lower, float new_center, float new_upper)
{
	lower  = new_lower;
	center = new_center;
	upper  = new_upper;

	value = 0.0;
	scaled_value = center;
}

EngineParameter::~EngineParameter()
{
}

void EngineParameter::SetValue(float new_value)
{
	value = new_value;

	RescaleValue();
}

void EngineParameter::SetScaling(float new_lower, float new_upper)
{
	SetScaling(new_lower, (new_lower + new_upper) * 0.5f, new_upper);
}

void EngineParameter::SetScaling(float new_lower, float new_center, float new_upper)
{
	lower  = new_lower;
	center = new_center;
	upper  = new_upper;

	RescaleValue();
}

void EngineParameter::RescaleValue()
{
	if (value < 0)
		scaled_value = lower + (value+1) * (center - lower);
	else
		scaled_value = center + value * (upper - center);
}