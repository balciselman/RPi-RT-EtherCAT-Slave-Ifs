# Adding modprobe might be neccassary
# or checking if the kernel driver is loaded 



KVER = $(shell uname -r)

cdir=$(shell pwd)

obj-m += gpioirq.o 

 

PWD := $(cdir) 
MODDIR=/lib/modules/$(KVER)/gpioirq
INCDIR=../include/gpioirq/
TARGET=gpioirq.ko
APPTARGET = /projects/rt-ethercatslave/gpioirq_kdriver
APPDIR = /projects/rt-ethercatslave/gpioirq_kdriver/user_space_test
DEPMOD = depmod
CHKRULE = $(cdir)/chk_rulefile.sh



all: build install rules

build:		$(TARGET) $(APPTARGET)


$(APPTARGET): $(APPDIR)/test.cpp $(INCDIR)/gpioirq.h
		g++ -IC -g -o $(APPTARGET) $(APPDIR)/test.cpp


$(TARGET):	gpioirq.c $(INCDIR)/gpioirq.h
	make -C  /lib/modules/$(KVER)/build M=$(PWD) modules 


install:		$(MODDIR)/$(TARGET)

rules:		
		$(CHKRULE)


$(MODDIR)/$(TARGET):	$(TARGET)	
	mkdir -p $(MODDIR)
	cp -f $(TARGET) $(MODDIR)
	$(DEPMOD)
	
clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	- rm $(APPTARGET)