#pragma once

#include <vector>
//#include "JString.h"
//#include "JThread.h"
//#include "JSemaphore.h"
#include "FAPFrame.h"
#include "BAPFrame.h"
#include "CentralClock.h"

#include <decaf/util/concurrent/Mutex.h>
#include <decaf/lang/Thread.h>

using namespace decaf::util::concurrent;
using namespace decaf::lang;
//using namespace cmlabs;

class FramesManager : public Thread //: public JThread
{
public:

	FramesManager(CentralClock *pc);
	virtual ~FramesManager();
	

	void AddFAPFrame(FAPFrame ff);
	//void AddFAPFrame(cmlabs::JString object);
	void AddBAPFrame(BAPFrame bf);
	//void AddBAPFrame(cmlabs::JString object);

	void AddFAPFrames(std::vector<FAPFrame> ffs);
	void AddBAPFrames(std::vector<BAPFrame> bfs);

	bool GetFAPFrame(FAPFrame &returnff);
	bool GetBAPFrame(BAPFrame &returnbf);

	int GetFAPSize();
	int GetBAPSize();

	void EmptyBuffer();

	void DeleteDiff(std::string label);

	bool IsFapFrameAt(int time);

	FAPFrame *GetFapFrameAt(int time);

	void run();

	bool sortff;
	bool sortbf;
	bool collectgarbage;

	CentralClock *pc;

	BAPConverter converter;

private:
	std::vector<FAPFrame> fapframes;
	std::vector<BAPFrame> bapframes;

	//JMutex mutex;
	Mutex mutex;

	int lastfapframe;
	int lastbapframe;
};