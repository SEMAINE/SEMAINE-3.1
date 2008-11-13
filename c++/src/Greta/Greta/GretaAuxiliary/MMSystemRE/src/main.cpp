#include <stdio.h>
#include "BehaviorQualityComputation.h"
#include "CommunicativeAct.h"
#include "MultimodalSignalSelection.h"

void main()
{
	
	Baseline bl;
	Dynamicline dl;
	bl.Load("mmsystem/baselinetest.xml");
	bl.Print();
	CommunicativeAct ca;
	ca.name="performative";
	ca.CommunicativeContext=0;
	BehaviorQualityComputation bqc("mmsystem/test.xml");
	ca.name="affect=sadness";
	dl=bqc.ComputeDynamicline(ca,bl);
	dl.Print();
	
	MultimodalSignalSelection mss("mmsystem/lexicon.xml");
}