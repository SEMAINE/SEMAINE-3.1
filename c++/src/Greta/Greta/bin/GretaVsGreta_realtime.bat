cd .\psyclone\
start psyclone spec="GretaVsGretaEVOSpec.xml" verbose=4
cd ..
.\utils\WAIT.exe 5

start .\ClockPsydule.exe 
.\utils\WAIT.exe 3

start .\ReactiveBehaviourPsydule.exe 
.\utils\WAIT.exe 5

start .\RealtimeBMLPsydule.exe 
.\utils\WAIT.exe 5

start .\RealtimeFMLPsydule.exe 
.\utils\WAIT.exe 5

start  RealtimePlayer.exe 
.\utils\WAIT.exe 3

start .\ClockPsydule.exe greta2.ini
.\utils\WAIT.exe 3

start .\ReactiveBehaviourPsydule.exe greta2.ini RulesGreta2
.\utils\WAIT.exe 5

start .\RealtimeBMLPsydule.exe greta2.ini
.\utils\WAIT.exe 5

start .\RealtimeFMLPsydule.exe greta2.ini
.\utils\WAIT.exe 5

start  RealtimePlayer.exe greta2.ini
.\utils\WAIT.exe 3
