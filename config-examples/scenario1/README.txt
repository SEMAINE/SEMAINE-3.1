This folder contains an example for a distributed setup of SEMAINE-2.0

The key idea here is to create a custom start script for each machine,
starting from SEMAINE-2.0\start.bat. All machines can use the common stop.bat.


In this scenario we assume two machines:

Machine 1 is a Windows machine. It should run ActiveMQ, the java components, and the Greta components.
Its IP address is 192.168.1.1

Machine 2 is also a Windows machine. It should run Opensmile and the Video analysis components.


Make sure the respective components are available in the respective machines.

-> copy start-machine1.bat into the SEMAINE-2.0 folder on machine 1.
-> copy start-machine2.bat into the SEMAINE-2.0 folder on machine 2.
-> Now, first run start-machine1 on machine 1, then start-machine2 on machine 2.