CC = gcc
CFLAGS = -g -Wall
OBJECTS = tortella.o
INCFLAGS = -I./
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -lpthread

all: tortella

tortella: $(OBJECTS)
	$(CC) -o tortella $(OBJECTS) $(LDFLAGS) $(LIBS)

tortellaprotocol_testing: tortellaprotocol.o testing/tortellaprotocol_testing.c
	$(CC) -o testing/tortellaprotocol_testing testing/tortellaprotocol_testing.c tortellaprotocol.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

httpmanager_testing: httpmanager.o tortellaprotocol.o logger.o testing/httpmanager_testing.c
	$(CC) -o testing/httpmanager_testing testing/httpmanager_testing.c tortellaprotocol.o httpmanager.o logger.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

socketmanager_testing: socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o testing/socketmanager_testing.c
	$(CC) -o testing/socketmanager_testing testing/socketmanager_testing.c socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

servent_testing: testing/servent_testing.c socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o servent.o list.o
	$(CC) -o testing/servent_testing testing/servent_testing.c socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o servent.o list.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

list_testing: testing/list_testing.c list.o
	$(CC) -o testing/list_testing testing/list_testing.c list.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

tortella.o: tortella.c
	$(CC) -o tortella.o -c tortella.c $(CFLAGS) $(INCFLAGS)

tortellaprotocol.o: tortellaprotocol.h tortellaprotocol.c
	$(CC) $(CFLAGS) -o tortellaprotocol.o -c tortellaprotocol.c

httpmanager.o: httpmanager.h httpmanager.c
	$(CC) $(CFLAGS) -o httpmanager.o -c httpmanager.c

socketmanager.o: socketmanager.h socketmanager.c
	$(CC) $(CFLAGS) -o socketmanager.o -c socketmanager.c

packetmanager.o: packetmanager.h packetmanager.c
	$(CC) $(CFLAGS) -o packetmanager.o -c packetmanager.c

servent.o: servent.h servent.c packetmanager.o
	$(CC) $(CFLAGS) -o servent.o -c servent.c

logger.o: logger.h logger.c
	$(CC) $(CFLAGS) -o logger.o -c logger.c

list.o: list.h list.c
	$(CC) $(CFLAGS) -o list.o -c list.c

utils.o: utils.h utils.c
	$(CC) $(CFLAGS) -o utils.o -c utils.c

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
