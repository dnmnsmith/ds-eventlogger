IDIR =../include
CC=g++
CFLAGS=-I$(IDIR) -Wall

LDIR =../lib

LOGGEREXE=eventLogger
EVENTSEXE=events
DAEMON=eventLoggerd

LOGGERINSTALLDIR=/usr/local/sbin
EVENTSINSTALLDIR=/usr/local/bin

LIBS= -lboost_thread -lboost_system  -lboost_filesystem -lcxxtools -lcxxtools-json -ltntdb -lrt -lm 

DEPS = eventLogger.h Event.h

LOGGEROBJ = EventLoggerMain.o eventLogger.o Event.o
EVENTSOBJ = events.o Event.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(LOGGEREXE): $(LOGGEROBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(EVENTSEXE): $(EVENTSOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean all install

clean:
	rm -f $(LOGGEROBJ) $(EVENTSOBJ) $(UNKNOWNOBJ) 

clobber:
	rm -f $(LOGGEROBJ) $(LOGGEREXE) $(LATESTEXE) $(UNKNOWNEXE)

all: $(LOGGEREXE) $(LATESTEXE) $(UNKNOWNEXE)

install:
	cp -f $(LOGGEREXE) $(LOGGERINSTALLDIR)
	cp -f $(EVENTSEXE) $(EVENTSINSTALLDIR)
	ln -s -f $(EVENTSINSTALLDIR)/$(EVENTSEXE) $(EVENTSINSTALLDIR)/latestEvents
	ln -s -f $(EVENTSINSTALLDIR)/$(EVENTSEXE) $(EVENTSINSTALLDIR)/minEvents
	ln -s -f $(EVENTSINSTALLDIR)/$(EVENTSEXE) $(EVENTSINSTALLDIR)/maxEvents
	ln -s -f $(EVENTSINSTALLDIR)/$(EVENTSEXE) $(EVENTSINSTALLDIR)/unknownEvents
	cp -f $(DAEMON) /etc/init.d
	@echo "To start at bootup run:"
	@echo "sudo update-rc.d eventLoggerd defaults"
	@echo "sudo service eventLoggerd start"
