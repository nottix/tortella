CC = gcc
CFLAGS = -g -Wall
OBJECTS = 
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

all:
	make -C ./src

.SUFFIXES:	.c .h .o

count:
	wc -l src/*.c src/*.h src/*/*.c src/*/*.h

clean:
	rm -f src/*.o src/*/*.o

.PHONY: all
.PHONY: count
.PHONY: clean
