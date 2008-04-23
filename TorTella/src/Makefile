CC = gcc
CFLAGS = -g -Wall
OBJECTS = tortella.o
INCFLAGS = -I./ `pkg-config --cflags glib-2.0`
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -lpthread `pkg-config --libs glib-2.0`

TORTELLA_GUI_CFLAGS = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/libglade-2.0 -I/usr/include/libxml2  
TORTELLA_GUI_LIBS = -lglade-2.0 -lgtk-x11-2.0 -lxml2 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  

all: tortella

tortella: $(OBJECTS)
	$(CC) -o tortella $(OBJECTS) $(LDFLAGS) $(LIBS)

gui: gui.o
	$(CC) -o gui gui.o $(LDFLAGS) $(LIBS) $(INCFLAGS) `pkg-config --cflags --libs gtk+-2.0`

tortellaprotocol_testing: tortellaprotocol.o testing/tortellaprotocol_testing.c
	$(CC) -o testing/tortellaprotocol_testing testing/tortellaprotocol_testing.c tortellaprotocol.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

httpmanager_testing: httpmanager.o tortellaprotocol.o logger.o testing/httpmanager_testing.c
	$(CC) -o testing/httpmanager_testing testing/httpmanager_testing.c tortellaprotocol.o httpmanager.o logger.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

socketmanager_testing: socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o testing/socketmanager_testing.c
	$(CC) -o testing/socketmanager_testing testing/socketmanager_testing.c socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

servent_testing: testing/servent_testing.c socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o servent.o list.o supernodedata.o
	$(CC) -o testing/servent_testing testing/servent_testing.c socketmanager.o tortellaprotocol.o httpmanager.o packetmanager.o utils.o servent.o list.o supernodedata.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

supernodedata_testing: testing/supernodedata_testing.c supernodedata.o utils.o
	$(CC) -o testing/supernodedata_testing testing/supernodedata_testing.c supernodedata.o utils.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

list_testing: testing/list_testing.c list.o
	$(CC) -o testing/list_testing testing/list_testing.c list.o $(LDFLAGS) $(LIBS) $(INCFLAGS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

tortella.o: tortella.c
	$(CC) -o tortella.o -c tortella.c $(CFLAGS) $(INCFLAGS)

tortellaprotocol.o: tortellaprotocol.h tortellaprotocol.c
	@echo "Building tortellaprotocol"
	$(CC) $(CFLAGS) -o tortellaprotocol.o -c tortellaprotocol.c

httpmanager.o: httpmanager.h httpmanager.c
	$(CC) $(CFLAGS) -o httpmanager.o -c httpmanager.c

socketmanager.o: socketmanager.h socketmanager.c
	$(CC) $(CFLAGS) -o socketmanager.o -c socketmanager.c

packetmanager.o: packetmanager.h packetmanager.c
	$(CC) $(CFLAGS) -o packetmanager.o -c packetmanager.c

servent.o: servent.h servent.c packetmanager.o
	$(CC) $(CFLAGS) $(INCFLAGS) -o servent.o -c servent.c

supernodedata.o: supernodedata.h supernodedata.c
	$(CC) $(CFLAGS) $(INCFLAGS) -o supernodedata.o -c supernodedata.c

logger.o: logger.h logger.c
	$(CC) $(CFLAGS) -o logger.o -c logger.c

list.o: list.h list.c
	$(CC) $(CFLAGS) -o list.o -c list.c

utils.o: utils.h utils.c
	$(CC) $(CFLAGS) -o utils.o -c utils.c

gui.o: gui.h gui.c
	$(CC) $(CFLAGS) `pkg-config --cflags --libs gtk+-2.0` -o gui.o -c gui.c 

gui/callbacks.o: gui/callbacks.h gui/callbacks.c
	$(CC) $(CFLAGS) `pkg-config --cflags --libs gtk+-2.0 libglade-2.0` -o gui/callbacks.o -c gui/callbacks.c

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
