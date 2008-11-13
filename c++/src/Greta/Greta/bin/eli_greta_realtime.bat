cd .\psyclone\
start psyclone spec="GretaEVOSpec.xml" verbose=0
cd ..
.\utils\WAIT.exe 3

start .\ClockPsydule.exe
.\utils\WAIT.exe 3

start .\ReactiveBehaviourPsydule.exe
.\utils\WAIT.exe 3

start .\RealtimeBMLPsydule.exe greta.ini
.\utils\WAIT.exe 3

start .\RealtimeFMLPsydule.exe
.\utils\WAIT.exe 3

start  RealtimePlayer.exe
.\utils\WAIT.exe 3

start  WoZWindow.exe
.\utils\WAIT.exe 3

