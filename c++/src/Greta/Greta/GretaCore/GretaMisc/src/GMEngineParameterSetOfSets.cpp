#include "GMEngineParameterSetOfSets.h"
#include "XMLGenericParser.h"

GMEngineParameterSet * GMEngineParameterSetOfSets::GetEngineParameterSet(std::string name)
{
	//this->Print();
	/*if((listOfParameters.find(name)==listOfParameters.end()))
	{
		return 0;
	}*/
	GMEngineParameterSet * toto;
	// TO DO reactive this methods
	// listOfParameters.find(name)->second()
	return(toto);
}

float GMEngineParameterSetOfSets::GetAttrValue(std::string name)
{
	std::string opname=name.substr(0,name.find_first_of("."));
	std::string opattr=name.substr(name.find_first_of(".")+1);

	if((GetEngineParameterSet(opname)==0)
		||((GetEngineParameterSet(opname)!=0)
		&&(GetEngineParameterSet(opname)->GetAttrAddress(opattr)==0)))
	{
		if(name.find_first_not_of("0123456789-.")!=std::string::npos)
		{
			return 0;
		}
		return (float)atof(name.c_str());
	}
	else
		return *GetEngineParameterSet(opname)->GetAttrAddress(opattr);
}

GMEngineParameterSetOfSets::GMEngineParameterSetOfSets()
{
}

GMEngineParameterSetOfSets::~GMEngineParameterSetOfSets()
{
}



int GMEngineParameterSetOfSets::addParameter(std::string name,GMEngineParameterSet param)
{
	std::cout << "Ajout du set de parametre " << name << std::endl;

	//if (listOfParameters.find(name)!=0)
	//CHRIS fix: to test when item not found in map or vector, compare with ::end
	if (listOfParameters.find(name) != listOfParameters.end())
	{		
		// if the parameters already exist : we delete it
		listOfParameters.erase(name);
		std::cout << "cible "<<name<<" deja existante "<<std::endl;
	}
	// recording of the parameters
	listOfParameters.insert(std::pair<std::string,GMEngineParameterSet>(name,param));
	return 1;
}


int GMEngineParameterSetOfSets::LoadFromBuffer(std::string buffer)
{
	XMLGenericParser p;
	XMLGenericTree *t;
	t=p.ParseBufferWithXSD((char*)buffer.c_str(),"mmsystem/xsd/profiles.xsd");
	if(t==0)
	{
		printf("GMEngineParameterSetOfSets: error parsing buffer\n");
		return 0;
	}

	std::list<XMLGenericTree*>::iterator mod,modpar;
	
	for(mod=(t->child).begin();mod!=(t->child).end();mod++)
	{
		GMEngineParameterSet m;
		m.name=(*mod)->GetAttribute("name");

		for(modpar=((*mod)->child).begin();modpar!=((*mod)->child).end();modpar++)
		{
			EngineParameter mp;
			mp.name=(*modpar)->GetAttribute("name");
			mp.SetValue((*modpar)->GetAttributef("value"));
			(m.mp)[mp.name]=mp;
		}
		(*this).listOfParameters[m.name]=m;
	}

	delete t;
	return 1;
}

int GMEngineParameterSetOfSets::LoadFromFile(std::string filepath)
{
	std::string buffer,line;
	if(filepath=="")
	{
		printf("GMEngineParameterSetOfSets: no filename given\n");
		return 0;
	}
	std::ifstream inputfile(filepath.c_str());
	if(inputfile.is_open())
	{
		while((inputfile.rdstate()&std::ios::eofbit)==0)
		{
			std::getline(inputfile,line,'\n');
			buffer=buffer+line+"\n";
		}
		inputfile.close();
		if(LoadFromBuffer(buffer)==0)
			return 0;
		else
			return 1;
	}
	return 0;
}
