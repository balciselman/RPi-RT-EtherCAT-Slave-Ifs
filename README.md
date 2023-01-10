# Real-Time EtherCAT Slave Interface Implementation on a Raspberry PI 4

This is the code of my master thesis project that is conducted in Moehwald GmbH to test and integrate real-time Rasperry Pi to automation test benches and a model for future projects.

The title can be considered as a compact definition of the project. A Raspberry Pi 4b patched with Preempt_RT on Debian 11 Bullseye with EtherC V1.6 (Microchip LAN9252 based) extension board is mainly used in the project. 

A EK1100 EtherCAT coupler is used to connect master and slaves. A real-time master computer is configured to manage slaves and test Raspberry Pi. 

#### Hardware
![Hardware](https://user-images.githubusercontent.com/16516613/211666114-8907fa91-ed8c-4970-8e8d-834a58f9484b.jpg)


User space and Kernel mode driver is programmed to integrate and syncronize for Ethernet based communication protocol EtherCAT with interrupts. Pi is adjusted for sending a counter and monotonic timer. The master test system is configured to send back these messages to measure inconsistencies and runtime.

At the end the system showed complete syncronization and fail-safe operation through interrupts at 10ms cycle rate with with 200µs avarage runtime. The runtime range is about 200µs which is believed to be reducible using core isolation which could not tested for the project due to time constraints.

#### statistical measurements during test

![Console Screen Shot](https://user-images.githubusercontent.com/16516613/211666531-6bcd6fda-d3a8-41ea-948f-3b410ed4f2fc.png)

#### Oscilloscope output of the transfer cycle

![Transfer Cycle Screenshot](https://user-images.githubusercontent.com/16516613/211666679-4362ea6c-1023-42be-8659-698f93a4daf0.png)


With the hardware ready the program can be runned easily.
```
git clone https://github.com/balciselman/RPi-RT-EtherCAT-Slave-Ifs.git
make
./ethercatslaveif
```



