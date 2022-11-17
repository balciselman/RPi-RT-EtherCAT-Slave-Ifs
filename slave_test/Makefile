###############################################
#											  #
#											  #
# Makefile to build Etherslave source files   #
# Debug is always active, will be integrated  #
# once the system stabilized				  #
###############################################



OBJS=
OBJSAPP=
LIBS=
cdir=$(shell pwd)

SRCDIR= ./src
LIBDIR= ../lib
###################################TESTDIR= ./??
OBJDIR= ./obj
INCDIR= ../include

TARGET=slave_test

TARGETNAME=$(LIBDIR)/lib$(TARGET).so
APPTEST = ../ethercatslaveif

LIBS += $(TARGETNAME)

# Header Files
H_FILES=$(shell ls ../include/*.h)

CC1 := gcc
CC := g++

LINKLIBS=-lpthread -lrt -lbcm2835 -ldl
LINKLIBSALL=-lpthread -lrt -lbcm2835 -ldl -lrpi_base -lrt_utilities -lshield_9252 -lslave_test -lthread


CPPCOMMON =  -shared -pthread -g3 -Wall -Wextra -fPIC  -MMD -MP -Wno-unused-parameter -fmessage-length=0 -finput-charset=utf-8 -fexec-charset=utf-8 
CPPRELEASE = -O2 -DNDEBUG 
CPPDEBUG = -O0 -D_DEBUG 

CPPEXTRA = $(CPPCOMMON) $(CPPRELEASE) -I $(INCDIR)
CPPEXTRADBG = $(CPPCOMMON) $(CPPDEBUG) -I $(INCDIR)



all:		build

clean:
	rm -rf $(OBJDIR)/
	rm -f $(LIBDIR)/lib$(TARGET).so
	rm -f $(APPTEST)
	@( echo "gpioirq module is removed" ; /usr/sbin/rmmod gpioirq )


build:	$(OBJDIR) $(LIBS) $(APPTEST)


$(OBJDIR):
	mkdir -p $(OBJDIR)


# implicit rule
$(OBJDIR)/%.o:		$(SRCDIR)/%.cpp $(H_FILES)
	$(CC) -c $(CPPEXTRADBG) $< -o $@


OBJS+=$(OBJDIR)/slave_test.o
#OBJS+=$(OBJDIR)/rpi_base.o
#OBJS+=$(OBJDIR)/rt_utilities.o
#OBJS+=$(OBJDIR)/shield_9252.o
#OBJS+=$(OBJDIR)/thread.o


$(LIBS) :		$(OBJS) 
	$(CC) -shared -o $@ $(OBJS) $(LINKLIBS)





$(APPTEST) : 	$(OBJS)
		@( echo "ldconfig..." ; /usr/sbin/ldconfig)
		@( echo "gpioirq module is added" ; /usr/sbin/modprobe gpioirq )
		$(CC) -o $@ $< -L $(LIBDIR) $(LINKLIBSALL) 


#		$(CC) -I $(H_FILES) -L $(LIBDIR) $(SRCDIR)/slave_test.cpp -o $(APPTEST) $(LINKLIBS) -l $(TARGET)  