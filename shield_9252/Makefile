###############################################
#											  #
#											  #
# Makefile to build Microchip LAN9252         #
# integrated shield differences				  #
# 				  							  #
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


TARGET=shield_9252
TARGET_DBG=shield_9252_dbg

TARGETNAME=$(LIBDIR)/lib$(TARGET).so
TARGETNAME_DBG=$(LIBDIR)/lib$(TARGET_DBG).so

LIBS += $(TARGETNAME)
LIBSDBG += $(TARGETNAME_DBG)

# Header Files
H_FILES=$(shell ls ../include/*.h)

CC1 := gcc
CC := g++

LINKLIBS=-lpthread -lrt -lbcm2835 -ldl -L $(LIBDIR)


CPPCOMMON =  -shared -pthread -g3 -Wall -Wextra -fPIC  -MMD -MP -Wno-unused-parameter -fmessage-length=0 -finput-charset=utf-8 -fexec-charset=utf-8 
CPPRELEASE = -O2 -DNDEBUG 
CPPDEBUG = -O0 -D_DEBUG 

CPPEXTRA = $(CPPCOMMON) $(CPPRELEASE) -I $(INCDIR)
CPPEXTRADBG = $(CPPCOMMON) $(CPPDEBUG) -I $(INCDIR)



all:	build


debug:	builddbg


clean:
	rm -rf $(OBJDIR)/
	rm -f $(TARGETNAME)


cleandbg:
	rm -rf $(OBJDIR)/
	rm -f $(TARGETNAME_DBG)


build:	$(OBJDIR) $(LIBS)


builddbg:	$(OBJDIR) $(LIBSDBG)


$(OBJDIR):
	mkdir -p $(OBJDIR)


OBJS+=$(OBJDIR)/shield_9252.o

OBJSDBG+=$(OBJDIR)/shield_9252_dbg.o

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