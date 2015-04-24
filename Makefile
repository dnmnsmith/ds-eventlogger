IDIR =../include
CC=g++
CFLAGS=-I$(IDIR) -Wall

LDIR =../lib

LOGGEREXE=eventLogger
LATESTEXE=latestEvents
UNKNOWNEXE=unknownEvents
DAEMON=eventLoggerd

LOGGERINSTALLDIR=/usr/local/sbin
LATESTINSTALLDIR=/usr/local/bin
UNKNOWNINSTALLDIR=/usr/local/bin

LIBS= -lboost_thread -lboost_system  -lboost_filesystem -lcxxtools -lcxxtools-json -ltntdb -lrt -lm 

DEPS = eventLogger.h Event.h

LOGGEROBJ = EventLoggerMain.o eventLogger.o Event.o
LATESTOBJ = latestEvents.o Event.o
UNKNOWNOBJ = unknownEvents.o Event.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(LOGGEREXE): $(LOGGEROBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(LATESTEXE): $(LATESTOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(UNKNOWNEXE): $(UNKNOWNOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean all install

clean:
	rm -f $(LOGGEROBJ) $(LATESTOBJ) $(UNKNOWNOBJ) 

clobber:
	rm -f $(LOGGEROBJ) $(LOGGEREXE) $(LATESTEXE) $(UNKNOWNEXE)

all: $(LOGGEREXE) $(LATESTEXE) $(UNKNOWNEXE)

install:
	cp -f $(LOGGEREXE) $(LOGGERINSTALLDIR)
	cp -f $(LATESTEXE) $(LATESTINSTALLDIR)
	cp -f $(UNKNOWNEXE) $(UNKNOWNINSTALLDIR)
	cp -f $(DAEMON) /etc/init.d
	@echo "To start at bootup run:"
	@echo "sudo update-rc.d eventLoggerd defaults"
