CC = gcc
CFLAGS = -g -Wall
OBJECTS = tortella.o
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

all: tortella

tortella: $(OBJECTS)
	$(CC) -o tortella $(OBJECTS) $(LDFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

tortella.o: tortella.c
	$(CC) -o tortella.o -c tortella.c $(CFLAGS) $(INCFLAGS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
