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
OBJDIR= ./obj
INCDIR= ../include

TARGET=rpi_base

TARGETNAME=$(LIBDIR)/lib$(TARGET).so

LIBS += $(TARGETNAME)

# Header Files
H_FILES=$(shell ls ../include/*.h)

CC1 := gcc
CC := g++

LINKLIBS=-lpthread -lrt -lbcm2835 -ldl


CPPCOMMON =  -shared -pthread -g3 -Wall -Wextra -fPIC  -MMD -MP -Wno-unused-parameter -fmessage-length=0 -finput-charset=utf-8 -fexec-charset=utf-8 
CPPRELEASE = -O2 -DNDEBUG 
CPPDEBUG = -O0 -D_DEBUG 

CPPEXTRA = $(CPPCOMMON) $(CPPRELEASE) -I $(INCDIR)
CPPEXTRADBG = $(CPPCOMMON) $(CPPDEBUG) -I $(INCDIR)



all:		build

clean:
	rm -rf $(OBJDIR)/
	rm -rf $(TARGETNAME)

build:	$(OBJDIR) $(LIBS) 


$(OBJDIR):
	mkdir -p $(OBJDIR)


# implicit rule
$(OBJDIR)/%.o:		$(SRCDIR)/%.cpp $(H_FILES)
	$(CC) -c $(CPPEXTRADBG) $< -o $@




OBJS+=$(OBJDIR)/rpi_base.o





$(LIBS) :		$(OBJS) 
	$(CC) -shared -o $@ $(OBJS) $(LINKLIBS)

