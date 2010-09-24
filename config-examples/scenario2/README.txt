This folder contains an example for a distributed setup of SEMAINE-2.0

The key idea here is to create a custom start script for each machine,
starting from SEMAINE-2.0\start.bat. All Windows machines can use the common stop.bat.

In this scenario we assume three machines:


Machine 1 is a Linux machine. It should run ActiveMQ, Opensmile, and the Java components.
Its IP address is 192.168.1.10.

Machine 2 is a Windows machine runing the Greta components.

Machine 3 is a Windows machine. It should run the Video analysis components.


Make sure the respective components are available in the respective machines.

-> copy start-machine1.sh into the SEMAINE-2.0 folder on machine 1.
-> copy start-machine2.bat into the SEMAINE-2.0 folder on machine 2.
-> copy start-machine3.bat into the SEMAINE-2.0 folder on machine 3.
-> Now, first run start-machine1 on machine 1, then start-machine2 on machine 2 and start-machine3 on machine 3.