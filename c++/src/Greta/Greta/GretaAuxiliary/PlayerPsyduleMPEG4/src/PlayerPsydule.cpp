#include ".\PlayerPsydule.h"
#include "FileNames.h"

extern FileNames filenames;

PlayerPsydule::PlayerPsydule(JString name, JString host, int port) : Psydule(name, host, port)
{
	this->name=name;
	this->host=host;
	this->port=port;
}

PlayerPsydule::~PlayerPsydule()
{
}

int PlayerPsydule::PlayerRegister()
{
	std::list<JString> types;
	types.push_back("Greta.Data.FAPdata");
	types.push_back("Greta.Data.BAPdata");
	types.push_back("Greta.Data.Audio");
	types.push_back("Greta.PlayerPsydule.Command.AssignFile");
	types.push_back("Greta.PlayerPsydule.Command.Start");

	return(this->Register("Greta.Whiteboard", types));
}