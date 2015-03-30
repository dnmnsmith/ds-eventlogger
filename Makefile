IDIR =../include
CC=g++
CFLAGS=-I$(IDIR) -Wall

LDIR =../lib

EXE=eventLogger

INSTALLDIR=/usr/local/sbin


LIBS= -lboost_thread -lboost_system  -lboost_filesystem -lcxxtools -lcxxtools-json -ltntdb -lrt -lm 

DEPS = eventLogger.h Event.h

OBJ = EventLoggerMain.o eventLogger.o Event.o


%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean all install

clean:
	rm -f $(OBJ) 

clobber:
	rm -f $(OBJ) $(EXE)

all: $(EXE)

install:
	cp -f $(EXE) $(INSTALLDIR)

