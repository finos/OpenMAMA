API_HOME=/opt/mama

# GNU compiler:
CXX      = g++
CC       = gcc
BSTATIC  = -Xlinker -Bstatic
BDYNAMIC = -Xlinker -Bdynamic

# Standard defines:
CPPFLAGS     = \
    -I$(API_HOME)/include 

LDFLAGS      = -rdynamic \
    -L$(API_HOME)/lib 


MAMA_LIBS    = $(BDYNAMIC) -lmama \
               $(BDYNAMIC) -lrt -lpthread -lm 

# Targets:

all: basicpub basicsub basicinbox 
    
basicpub: basicpub.o
	$(LINK.c) -o $@ $^ $(MAMA_LIBS) $(SYS_LIBS)

basicsub: basicsub.o
	$(LINK.c) -o $@ $^ $(MAMA_LIBS) $(SYS_LIBS)

basicinbox: basicinbox.o
	$(LINK.C) -o $@ $^ $(MAMA_LIBS) $(SYS_LIBS)

