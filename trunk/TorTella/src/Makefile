CC = gcc
CFLAGS = -g -Wall
OBJECTS = tortella.o
INCFLAGS = -I./
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

all: tortella

tortella: $(OBJECTS)
	$(CC) -o tortella $(OBJECTS) $(LDFLAGS) $(LIBS)

tortellaprotocol_testing: tortellaprotocol.o testing/tortellaprotocol_testing.c
	$(CC) -o testing/tortellaprotocol_testing testing/tortellaprotocol_testing.c tortellaprotocol.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

httpmanager_testing: httpmanager.o tortellaprotocol.o testing/httpmanager_testing.c
	$(CC) -o testing/httpmanager_testing testing/httpmanager_testing.c tortellaprotocol.o httpmanager.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

socketmanager_testing: socketmanager.o
	$(CC) -o testing/socketmanager_testing testing/socketmanager_testing.c socketmanager.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

tortella.o: tortella.c
	$(CC) -o tortella.o -c tortella.c $(CFLAGS) $(INCFLAGS)

tortellaprotocol.o: tortellaprotocol.h tortellaprotocol.c
	$(CC) $(CFLAGS) -o tortellaprotocol.o -c tortellaprotocol.c

httpmanager.o: httpmanager.h httpmanager.c tortellaprotocol.h
	$(CC) $(CFLAGS) -o httpmanager.o -c httpmanager.c

socketmanager.o: socketmanager.h socketmanager.c
	$(CC) $(CFLAGS) -o socketmanager.o -c socketmanager.c

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
