cd .\psyclone\
start psyclone spec="GretaEVOSpec.xml" verbose=0
cd ..
.\utils\WAIT.exe 5

start .\ClockPsydule.exe
.\utils\WAIT.exe 3

start .\RealtimeBMLPsydule.exe greta.ini
.\utils\WAIT.exe 5

start .\RealtimeFMLPsydule.exe
.\utils\WAIT.exe 5

start  RealtimePlayer.exe
.\utils\WAIT.exe 3

