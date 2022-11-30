###############################################
#											  #
#											  #
# Makefile to build Etherslave source files   #
#											  #
###############################################



OBJS=
OBJSDBG=
OBJSAPP=
LIBS=
LIBSDBG=
cdir=$(shell pwd)

SRCDIR= ./src
LIBDIR= ../lib
OBJDIR= ./obj
INCDIR= ../include


DEPS := $(OBJDIR)/*.d

-include $(DEPS)


TARGET=slave_test
TARGET_DBG=slave_test_dbg

TARGETNAME=$(LIBDIR)/lib$(TARGET).so
TARGETNAME_DBG=$(LIBDIR)/lib$(TARGET_DBG).so

APPTEST = ../ethercatslaveif
APPTEST_DBG = ../ethercatslaveif_dbg


LIBS += $(TARGETNAME)

LIBSDBG += $(TARGETNAME_DBG)

# Header Files
H_FILES=$(shell ls ../include/*.h)

CC1 := gcc
CC := g++

LINKLIBS=-lpthread -lrt -lbcm2835 -ldl -L $(LIBDIR)

LINKLIBSALL=$(LINKLIBS) -lrpi_base -lrt_utilities -lshield_9252 -lslave_test -lthread
LINKLIBSALL_DBG=$(LINKLIBS) -lrpi_base_dbg -lrt_utilities_dbg -lshield_9252_dbg -lslave_test_dbg -lthread_dbg

CPPCOMMON =  -shared -pthread -g3 -Wall -Wextra -fPIC  -MMD -MP -Wno-unused-parameter -fmessage-length=0 -finput-charset=utf-8 -fexec-charset=utf-8 
CPPRELEASE = -O2 -DNDEBUG 
CPPDEBUG = -O0 -D_DEBUG 

CPPEXTRA = $(CPPCOMMON) $(CPPRELEASE) -I $(INCDIR)
CPPEXTRADBG = $(CPPCOMMON) $(CPPDEBUG) -I $(INCDIR)



all:	build


debug:	builddbg


clean:
	rm -rf $(OBJDIR)/
	rm -f $(LIBDIR)/lib$(TARGET).so
	rm -f $(APPTEST)
	@( echo "gpioirq module is removed" ; /usr/sbin/rmmod gpioirq )

cleandbg:
	rm -rf $(OBJDIR)/
	rm -f $(TARGETNAME_DBG)
	rm -f $(APPTEST_DBG)
	@( echo "gpioirq module is removed" ; /usr/sbin/rmmod gpioirq )


build:	$(OBJDIR) $(LIBS) $(APPTEST)


builddbg:	$(OBJDIR) $(LIBSDBG) $(APPTEST_DBG)



$(OBJDIR):
	mkdir -p $(OBJDIR)


OBJS+=$(OBJDIR)/slave_test.o

OBJSDBG+=$(OBJDIR)/slave_test_dbg.o


# implicit rule
$(OBJDIR)/%.o:		$(SRCDIR)/%.cpp $(H_FILES)
	$(CC) -c $(CPPEXTRA) $< -o $@


# implicit rule
$(OBJDIR)/%_dbg.o:		$(SRCDIR)/%.cpp $(H_FILES)
	$(CC) -c $(CPPEXTRADBG) $< -o $@



$(LIBS) :		$(OBJS) 
	$(CC) -shared -o $@ $(OBJS) $(LINKLIBS)


$(LIBSDBG) :		$(OBJSDBG) 
	$(CC) -shared -o $@ $(OBJSDBG) $(LINKLIBS)




$(APPTEST) : 	$(OBJS)
		@( echo "gpioirq module is added" ; /usr/sbin/modprobe gpioirq )
		$(CC) $< -o $@ $(LINKLIBSALL) 


$(APPTEST_DBG) : 	$(OBJSDBG)
		@( echo "gpioirq module is added" ; /usr/sbin/modprobe gpioirq )
		$(CC) $< -o $@ $(LINKLIBSALL_DBG) 

