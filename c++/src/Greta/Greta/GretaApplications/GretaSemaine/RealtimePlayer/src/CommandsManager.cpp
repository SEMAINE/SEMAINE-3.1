#include <algorithm>
#include "CommandsManager.h"
//#include "XMLParser.h"
//#include "Timer.h"
//#include "SDL_thread.h"
//#include "SDL_net.h"


CommandsManager::CommandsManager(CentralClock *pc)
{
	this->pc=pc;
}

CommandsManager::~CommandsManager()
{
	//delete psydule;
}
void CommandsManager::AddCommand(std::string name,std::string arguments)
{
	RTPlayerCommand rtpc;
	rtpc.name=name;
	rtpc.arguments=arguments;
	rtpc.time=atoi(arguments.substr(arguments.find_last_of(" ")+1).c_str());
	//mutex.enterMutex();
	mutex.lock();
	if((rtpc.time-pc->GetTime())>0)
	{
		commands.push_back(rtpc);
		std::sort(commands.begin(),commands.end());
	}
	//mutex.leaveMutex();
	mutex.unlock();
}

bool CommandsManager::GetCommand(std::string &returnc)
{
	//mutex.enterMutex();
	mutex.lock();
	std::vector<RTPlayerCommand>::iterator iter;
	for(iter=commands.begin();iter!=commands.end();iter++)
	{
		if(((*iter).time-pc->GetTime())<0)
		{
			returnc=(*iter).name+" "+(*iter).arguments;
			commands.erase(iter);
			//mutex.leaveMutex();
			mutex.unlock();
			return true;
		}
	}
	//mutex.leaveMutex();
	mutex.unlock();
	return false;
}
