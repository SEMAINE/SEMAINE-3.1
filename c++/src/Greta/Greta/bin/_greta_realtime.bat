cd C:\code\greta\bin\psyclone\
start psyclone spec="GretaEVOSpec.xml" verbose=4
C:\code\greta\bin\utils\WAIT.exe 5

cd C:\code\greta\bin\
start C:\code\greta\bin\ClockPsydule.exe
C:\code\greta\bin\utils\WAIT.exe 3

start C:\code\greta\bin\RealtimeBMLPsydule.exe radek_greta.ini
C:\code\greta\bin\utils\WAIT.exe 5


cd C:\code\greta\bin\
start  RealtimePlayer.exe
C:\code\greta\bin\utils\WAIT.exe 3

cd c:\code\Greta\GretaAuxiliary\interface\dist
start start.bat
