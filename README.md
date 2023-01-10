# Real-Time EtherCAT Slave Interface Implementation on a Raspberry PI 4

This is the code of my master thesis project that is conducted in Moehwald GmbH to test and integrate real-time Rasperry Pi to automation test benches and a model for future projects.

The title can be considered as a compact definition of the project. A Raspberry Pi 4b patched with Preempt_RT on Debian 11 Bullseye with EtherC V1.6 (Microchip LAN9252 based) extension board is mainly used in the project. 

A EK1100 EtherCAT coupler is used to connect master and slaves. A real-time master computer is configured to manage slaves and test Raspberry Pi. 

(Maybe a photo that shows the hardware that we used???)


User space and Kernel mode driver is programmed to integrate and syncronize for Ethernet based communication protocol EtherCAT with interrupts. Pi is adjusted for sending a counter and monotonic timer. The master test system is configured to send back these messages to measure inconsistencies and runtime.

At the end the system showed complete syncronization and fail-safe operation through interrupts at 10ms cycle rate with with 200µs avarage runtime. The runtime range is about 200µs which is believed to be reducible using core isolation which could not tested for the project due to time constraints.

(Console output photo)

With the hardware ready the program can be runned easily.
```
git clone https://github.com/balciselman/RPi-RT-EtherCAT-Slave-Ifs.git
make
./ethercateslaveif
```



