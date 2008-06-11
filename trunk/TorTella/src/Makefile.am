
INCFLAGS = -I./ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/pixman-1
OBJECTS = tortellaprotocol.o httpmanager.o socketmanager.o packetmanager.o servent.o datamanager.o logger.o utils.o init.o routemanager.o confmanager.o controller.o gui.o
LIBS = -lpthread -lglib-2.0 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0 -lgthread-2.0 -lrt -lglib-2.0
BIN = ../bin

bin_PROGRAMS = tortella
tortella_SOURCES = tortella.c
tortella_LDADD = $(OBJECTS) $(LIBS)
tortella_CFLAGS = $(INCFLAGS)

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

datamanager.o: datamanager.h datamanager.c
	@echo -e "\nBuilding datamanager"
	$(CC) $(CFLAGS) $(INCFLAGS) -o datamanager.o -c datamanager.c

logger.o: logger.h logger.c
	@echo -e "\nBuilding logger"
	$(CC) $(CFLAGS) $(INCFLAGS) -o logger.o -c logger.c

utils.o: utils.h utils.c
	@echo -e "\nBuilding utils"
	$(CC) $(CFLAGS) $(INCFLAGS) -o utils.o -c utils.c

gui.o: gui.h gui.c
	@echo -e "\nBuilding gui"
	$(CC) $(CFLAGS) $(INCFLAGS) -o gui.o -c gui.c 

init.o: init.h init.c
	@echo -e "\nBuilding init"
	$(CC) $(CFLAGS) $(INCFLAGS) -o init.o -c init.c 

routemanager.o: routemanager.h routemanager.c
	@echo -e "\nBuilding routemanager"
	$(CC) $(CFLAGS) $(INCFLAGS) -o routemanager.o -c routemanager.c
	
confmanager.o: confmanager.h confmanager.c
	@echo -e "\nBuilding confmanager"
	$(CC) $(CFLAGS) $(INCFLAGS) -o confmanager.o -c confmanager.c
	
controller.o: controller.h controller.c
	@echo -e "\nBuilding controller"
	$(CC) $(CFLAGS) $(INCFLAGS) -o controller.o -c controller.c
