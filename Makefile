IDIR =../include
CC=g++
CFLAGS=-I$(IDIR) -Wall

LDIR =../lib

LIBS= -lboost_thread -lboost_system  -lboost_filesystem -lcxxtools -lcxxtools-json -ltntdb -lrt -lm 

DEPS = eventLogger.h locations.h

OBJ = EventLoggerMain.o eventLogger.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

eventLogger: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean all

clean:
	rm -f $(OBJ) 

all: eventLogger
