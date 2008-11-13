#include <algorithm>
#include "FramesManager.h"
#include "XMLParser.h"
#include "Timer.h"
//#include "SDL_thread.h"
//#include "SDL_net.h"



FramesManager::FramesManager(CentralClock *pc):JThread()
{
	this->pc=pc;
	sortff=false;
	sortbf=false;
	collectgarbage=false;
	lastfapframe=-1;
	lastbapframe=-1;
}

FramesManager::~FramesManager()
{
	//delete psydule;
}

void FramesManager::run()
{
	while(true)
	{
		Sleep(5);
		/*if(sortff==true)
		{
			mutex.enterMutex();
			std::sort(fapframes.begin(),fapframes.end());
			//erase the elements which are too old
			sortff=false;
			mutex.leaveMutex();
		}*/
		if(sortbf==true)
		{
			mutex.enterMutex();
			std::sort(bapframes.begin(),bapframes.end());
			//erase the elements which are too old
			sortbf=false;
			mutex.leaveMutex();
		}
		if(collectgarbage==true)
		{
			mutex.enterMutex();

			std::vector<FAPFrame>::iterator iter;
			iter=fapframes.begin();
			while(iter!=fapframes.end())
			{
				if(((pc->GetTime())-(*iter).use_at)<0)
					break;

				if(((pc->GetTime())-(*iter).use_at)>60)
				{
					iter=fapframes.erase(iter);
//					iter=fapframes.begin();
				}
				else
					iter++;
			}

			std::vector<BAPFrame>::iterator iter2;
			iter2=bapframes.begin();
			while(iter2!=bapframes.end())
			{
				if(((pc->GetTime())-(*iter2).GetFrameNumber())<0)
					break;

				if(((pc->GetTime())-(*iter2).GetFrameNumber())>80)
				{
					iter2=bapframes.erase(iter2);
					//iter2=bapframes.begin();
				}
				else
					iter2++;
			}

			collectgarbage=false;
			mutex.leaveMutex();
		}
	}
}

void FramesManager::AddFAPFrame(FAPFrame ff)
{
	bool inserted;
	std::vector<FAPFrame>::iterator iter;
	
	if(fapframes.empty())
	{
		fapframes.push_back(ff);
		return;
	}

	inserted=false;
	
	if(IsFapFrameAt(ff.use_at)==true)
	{
		//se il Merge non viene bene mettere il Copy
		GetFapFrameAt(ff.use_at)->MergeFrom(&ff);
		return;
	}
	

	int low, high,mid;
	low=0;
	high=fapframes.size()-1;
	mid=-1;

	if(ff.use_at<fapframes[low].use_at)
	{
		fapframes.insert(fapframes.begin(),ff);
		return;
	}

	if(ff.use_at>fapframes[high].use_at)
	{
		fapframes.push_back(ff);
		return;
	}


	while((high-low)>1)
	{
		mid=(int)((high-low)/2.0f)+low;

		if((fapframes[mid].use_at<ff.use_at)&&(ff.use_at<=fapframes[high].use_at))
			low=mid;

		if((fapframes[low].use_at<=ff.use_at)&&(ff.use_at<fapframes[mid].use_at))
			high=mid;
	}

	fapframes.insert(fapframes.begin()+high,ff);

	collectgarbage=true;
}

void FramesManager::AddFAPFrame(cmlabs::JString object)
{
	std::string received;
	std::string twolines;
	std::string label;
	received=object.c_str();
	mutex.enterMutex();
	if(received[received.size()-1]!='\n')
		received+="\n";
	label=received.substr(0,received.find_first_of("\n"));
	received=received.substr(received.find_first_of("\n")+1);
	while(received!="")
	{
		twolines="";
		twolines=received.substr(0,received.find_first_of("\n"))+"\n";
		received=received.substr(received.find_first_of("\n")+1);
		twolines+=received.substr(0,received.find_first_of("\n"))+"\n";
		received=received.substr(received.find_first_of("\n")+1);
		FAPFrame ff;
		ff.ReadFromBuffer((char*)twolines.c_str());
		ff.use_at=ff.framenumber;
		ff.id=label;
		AddFAPFrame(ff);
	}
	/*
	cmlabs::XMLParser* parser = new cmlabs::XMLParser();
	parser->parseXML(object.c_str());
	cmlabs::XMLNode* tree = parser->getRootNode();
	if(tree==0)
	{
	}
	else
	{
		cmlabs::JString frame;
		if(tree->getChildNode("body")!=0)
		{
			frame=(tree->getChildNode("body")->getText());
			if(frame!="")
			{
				ff.ReadFromBuffer((char*)frame.c_str());
				if(tree->getChildNode("timestamp")->getChildNode("use")->hasAttribute("at"))
					ff.use_at=atoi(tree->getChildNode("timestamp")->getChildNode("use")->getAttribute("at"));
				if(tree->getChildNode("timestamp")->getChildNode("expire")->hasAttribute("at"))
					ff.use_before=atoi(tree->getChildNode("timestamp")->getChildNode("expire")->getAttribute("at"));
				ff.received_at=pc->GetTime();
				AddFAPFrame(ff);
			}
		}
	}
	delete parser;*/
	mutex.leaveMutex();
}

void FramesManager::AddBAPFrame(BAPFrame bf)
{
	mutex.enterMutex();
	bapframes.push_back(bf);
	sortbf=true;
	collectgarbage=true;
	mutex.leaveMutex();
}

void FramesManager::AddBAPFrame(cmlabs::JString object)
{
	std::string received;
	std::string twolines;
	std::string label;
	received=object.c_str();
	mutex.enterMutex();
	if(received[received.size()-1]!='\n')
		received+="\n";
	label=received.substr(0,received.find_first_of("\n"));
	received=received.substr(received.find_first_of("\n")+1);
	while(received!="")
	{
		twolines="";
		twolines=received.substr(0,received.find_first_of("\n"))+"\n";
		received=received.substr(received.find_first_of("\n")+1);
		twolines+=received.substr(0,received.find_first_of("\n"))+"\n";
		received=received.substr(received.find_first_of("\n")+1);
		BAPFrame bf(&converter);
		bf.ReadFromBuffer((char*)twolines.c_str());
		//bf.use_at=bf.GetFrameNumber();
		bf.id=label;
		AddBAPFrame(bf);
	}
	/*
	cmlabs::XMLParser* parser = new cmlabs::XMLParser();
	parser->parseXML(object.c_str());
	cmlabs::XMLNode* tree = parser->getRootNode();
	if(tree==0)
	{
	}
	else
	{
		cmlabs::JString frame;
		if(tree->getChildNode("body")!=0)
		{
			frame=(tree->getChildNode("body")->getText());
			if(frame!="")
			{
				bf.ReadFromBuffer((char*)frame.c_str());
				if(tree->getChildNode("timestamp")->getChildNode("use")->hasAttribute("at"))
					bf.use_at=atoi(tree->getChildNode("timestamp")->getChildNode("use")->getAttribute("at"));
				if(tree->getChildNode("timestamp")->getChildNode("expire")->hasAttribute("at"))
					bf.use_before=atoi(tree->getChildNode("timestamp")->getChildNode("expire")->getAttribute("at"));
				bf.received_at=pc->GetTime();
				AddBAPFrame(bf);
			}
		}
	}
	delete parser;*/
	mutex.leaveMutex();
}

void FramesManager::AddFAPFrames(std::vector<FAPFrame> ffs)
{
	mutex.enterMutex();
	std::vector<FAPFrame>::iterator iter;
	for(iter=ffs.begin();iter!=ffs.end();iter++)
	{
		AddFAPFrame((*iter));
	}
	collectgarbage=true;
	mutex.leaveMutex();
}

void FramesManager::AddBAPFrames(std::vector<BAPFrame> bfs)
{
}

bool FramesManager::GetFAPFrame(FAPFrame &returnff)
{
	int time;
	time=pc->GetTime();
	mutex.enterMutex();
	std::vector<FAPFrame>::iterator iter;

	iter=fapframes.begin();

	while(iter!=fapframes.end())
	{
		if((abs(time-(*iter).use_at))<40)
		{
			if((lastfapframe<(*iter).use_at)&&(((*iter).use_at%40)==0))
			{
				returnff.CopyFrom(&(*iter));
				lastfapframe=(*iter).use_at;
				//printf("%d (%d) ",(*iter).use_at,time);
				fapframes.erase(iter);
				mutex.leaveMutex();
				collectgarbage=true;
				return true;
			}
			else
			{
				if(((*iter).use_at%40)!=0)
					printf("frame not aligned with clock!\n");
				iter=fapframes.erase(iter);
			}
		}
		else
			iter++;
	}
	mutex.leaveMutex();
	collectgarbage=true;
	return false;
}

bool FramesManager::GetBAPFrame(BAPFrame &returnbf)
{
	mutex.enterMutex();
	std::vector<BAPFrame>::iterator iter;
	for(iter=bapframes.begin();iter!=bapframes.end();iter++)
	{
		if((abs(pc->GetTime()-(*iter).GetFrameNumber()))<25)
		{
			if(lastbapframe!=(*iter).GetFrameNumber())
			{
				returnbf=(*iter);
				lastbapframe=(*iter).GetFrameNumber();
				bapframes.erase(iter);
				mutex.leaveMutex();
				collectgarbage=true;
				return true;
			}
		}
	}
	mutex.leaveMutex();
	collectgarbage=true;
	return false;
}

int FramesManager::GetFAPSize()
{
	return fapframes.size();
}

int FramesManager::GetBAPSize()
{
	return bapframes.size();
}

void FramesManager::EmptyBuffer()
{
	fapframes.clear();
	bapframes.clear();
}

void FramesManager::DeleteDiff(std::string label)
{
	//printf("delete diff %s\n",label.c_str());
	mutex.enterMutex();
	std::vector<BAPFrame>::iterator iter;
	iter=bapframes.begin();
	while(iter!=bapframes.end())
	{
		if((*iter).id!=label)
		{
			iter=bapframes.erase(iter);
			//iter=bapframes.begin();
		}
		else
			iter++;
	}
	std::vector<FAPFrame>::iterator iter2;
	iter2=fapframes.begin();
	while(iter2!=fapframes.end())
	{
		if((*iter2).id!=label)
		{
			iter2=fapframes.erase(iter2);
			//iter2=fapframes.begin();
		}
		else
			iter2++;
	}
	mutex.leaveMutex();
}

bool FramesManager::IsFapFrameAt(int time)
{
	mutex.enterMutex();
	int low,high,mid;
	low=0;
	high=fapframes.size()-1;
	mid=-1;

	if(fapframes.empty())
	{
		mutex.leaveMutex();
		return false;
	}

	if((time<fapframes[low].use_at)||(time>fapframes[high].use_at))
	{
		mutex.leaveMutex();
		return false;
	}

	while(low!=mid)
	{
		mid=(int)((high-low)/2.0f)+low;
		if(fapframes[mid].use_at==time)
		{
			mutex.leaveMutex();
			return true;
		}
		else
		{
			if((fapframes[mid].use_at<time)&&(time<=fapframes[high].use_at))
				low=mid+1;
			if((fapframes[low].use_at<=time)&&(time<fapframes[mid].use_at))
				high=mid-1;
		}
		if((time<fapframes[low].use_at)||(time>fapframes[high].use_at))
		{
			mutex.leaveMutex();
			return false;
		}
	}
	mutex.leaveMutex();
	return false;
}

FAPFrame *FramesManager::GetFapFrameAt(int time)
{
	mutex.enterMutex();
	int low,high,mid;
	low=0;
	high=fapframes.size()-1;
	mid=-1;

	if(fapframes.empty())
	{
		mutex.leaveMutex();
		return 0;
	}

	if((time<fapframes[low].use_at)||(time>fapframes[high].use_at))
	{
		mutex.leaveMutex();
		return 0;
	}

	while(low!=mid)
	{
		mid=(int)((high-low)/2.0f)+low;
		if(fapframes[mid].use_at==time)
		{
			mutex.leaveMutex();
			return &(fapframes[mid]);
		}
		else
		{
			if((fapframes[mid].use_at<time)&&(time<=fapframes[high].use_at))
				low=mid+1;
			if((fapframes[low].use_at<=time)&&(time<fapframes[mid].use_at))
				high=mid-1;
		}
		if((time<fapframes[low].use_at)||(time>fapframes[high].use_at))
		{
			mutex.leaveMutex();
			return 0;
		}
	}
	mutex.leaveMutex();
	return 0;
}
