CC = gcc
CFLAGS = -g -Wall
OBJECTS = tortellaprotocol.o httpmanager.o socketmanager.o packetmanager.o servent.o supernodedata.o logger.o utils.o init.o
INCFLAGS = -I./ `pkg-config --cflags glib-2.0`
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -lpthread `pkg-config --libs glib-2.0`

TORTELLA_GUI_CFLAGS = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/libglade-2.0 -I/usr/include/libxml2  
TORTELLA_GUI_LIBS = -lglade-2.0 -lgtk-x11-2.0 -lxml2 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  

all: tortella

tortella: $(OBJECTS) tortella.c
	$(CC) -o tortella tortella.c $(OBJECTS) $(LDFLAGS) $(LIBS)

gui: $(OBJECTS) gui.c
	$(CC) -o gui gui.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS) `pkg-config --cflags --libs gtk+-2.0`

tortellaprotocol_testing: $(OBJECTS) testing/tortellaprotocol_testing.c
	$(CC) -o testing/tortellaprotocol_testing testing/tortellaprotocol_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

httpmanager_testing: $(OBJECTS) testing/httpmanager_testing.c
	$(CC) -o testing/httpmanager_testing testing/httpmanager_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

socketmanager_testing: $(OBJECTS) testing/socketmanager_testing.c
	$(CC) -o testing/socketmanager_testing testing/socketmanager_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

servent_testing: $(OBJECTS) testing/servent_testing.c
	$(CC) -o testing/servent_testing testing/servent_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

supernodedata_testing: $(OBJECTS) testing/supernodedata_testing.c
	$(CC) -o testing/supernodedata_testing testing/supernodedata_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

list_testing: $(OBJECTS) testing/list_testing.c
	$(CC) -o testing/list_testing testing/list_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

init_testing: $(OBJECTS) testing/init_testing.c
	$(CC) -o testing/init_testing testing/init_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)
	
.SUFFIXES:	.c .h .o

tortellaprotocol.o: tortellaprotocol.h tortellaprotocol.c
	@echo -e "Building tortellaprotocol"
	$(CC) $(CFLAGS) $(INCFLAGS) -o tortellaprotocol.o -c tortellaprotocol.c

httpmanager.o: httpmanager.h httpmanager.c
	@echo -e "\nBuilding httpmanager"
	$(CC) $(CFLAGS) $(INCFLAGS) -o httpmanager.o -c httpmanager.c

socketmanager.o: socketmanager.h socketmanager.c
	@echo -e "\nBuilding socketmanager"
	$(CC) $(CFLAGS) $(INCFLAGS) -o socketmanager.o -c socketmanager.c

packetmanager.o: packetmanager.h packetmanager.c
	@echo -e "\nBuilding packetmanager"
	$(CC) $(CFLAGS) $(INCFLAGS) -o packetmanager.o -c packetmanager.c

servent.o: servent.h servent.c
	@echo -e "\nBuilding servent"
	$(CC) $(CFLAGS) $(INCFLAGS) -o servent.o -c servent.c

supernodedata.o: supernodedata.h supernodedata.c
	@echo -e "\nBuilding supernodedata"
	$(CC) $(CFLAGS) $(INCFLAGS) -o supernodedata.o -c supernodedata.c

logger.o: logger.h logger.c
	@echo -e "\nBuilding logger"
	$(CC) $(CFLAGS) $(INCFLAGS) -o logger.o -c logger.c

list.o: list.h list.c
	@echo -e "\nBuilding list"
	$(CC) $(CFLAGS) $(INCFLAGS) -o list.o -c list.c

utils.o: utils.h utils.c
	@echo -e "\nBuilding utils"
	$(CC) $(CFLAGS) $(INCFLAGS) -o utils.o -c utils.c

gui.o: gui.h gui.c
	@echo -e "\nBuilding gui"
	$(CC) $(CFLAGS) $(INCFLAGS) -o gui.o -c gui.c 

init.o: init.h init.c
	@echo -e "\nBuilding init"
	$(CC) $(CFLAGS) $(INCFLAGS) -o init.o -c init.c 


count:
	wc -l *.c *.h */*.c */*.h

clean:
	rm -f *.o testing/*.o

.PHONY: all
.PHONY: count
.PHONY: clean
