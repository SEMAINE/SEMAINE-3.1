#include "GMEngineParameterSet.h"

GMEngineParameterSet::GMEngineParameterSet()
{

}

GMEngineParameterSet::~GMEngineParameterSet()
{

}

EngineParameter GMEngineParameterSet::getParameter(std::string name)
{
	return this->mp[name];
}

int GMEngineParameterSet::addParameter(std::string name, EngineParameter parameter)
{
	std::cout <<"Ajout du parametre "<<name <<"de valeur " <<  parameter.GetValue() << std::endl;
    this->mp[name]=parameter;
	return 1;
}

float *GMEngineParameterSet::GetAttrAddress(std::string mpname)
{
	if(this->mp.find(mpname)==this->mp.end())
		return 0;
	return this->mp[mpname].GetValueAddr();
}