INCFLAGS = -I./

AM_CFLAGS = $(gtk_CFLAGS) $(gthread_CFLAGS) $(INCFLAGS)

OBJECTS = tortellaprotocol.o httpmanager.o socketmanager.o packetmanager.o servent.o datamanager.o logger.o utils.o init.o routemanager.o confmanager.o controller.o gui.o

LIBS = $(gthread_LIBS) $(gtk_LIBS)

bin_PROGRAMS = tortella
tortella_SOURCES = tortella.c
tortella_DEPENDENCIES = $(OBJECTS) tortella.c
tortella_LDADD = $(OBJECTS) $(LIBS)
tortella_CFLAGS = $(AM_CFLAGS)

tortellaprotocol.o: tortellaprotocol.h tortellaprotocol.c
	@echo -e "Building tortellaprotocol"
	$(CC) $(AM_CFLAGS) -o tortellaprotocol.o -c tortellaprotocol.c

httpmanager.o: httpmanager.h httpmanager.c
	@echo -e "\nBuilding httpmanager"
	$(CC) $(AM_CFLAGS) -o httpmanager.o -c httpmanager.c

socketmanager.o: socketmanager.h socketmanager.c
	@echo -e "\nBuilding socketmanager"
	$(CC) $(AM_CFLAGS) -o socketmanager.o -c socketmanager.c

packetmanager.o: packetmanager.h packetmanager.c
	@echo -e "\nBuilding packetmanager"
	$(CC) $(AM_CFLAGS) -o packetmanager.o -c packetmanager.c

servent.o: servent.h servent.c
	@echo -e "\nBuilding servent"
	$(CC) $(AM_CFLAGS) -o servent.o -c servent.c

datamanager.o: datamanager.h datamanager.c
	@echo -e "\nBuilding datamanager"
	$(CC) $(AM_CFLAGS) -o datamanager.o -c datamanager.c

logger.o: logger.h logger.c
	@echo -e "\nBuilding logger"
	$(CC) $(AM_CFLAGS) -o logger.o -c logger.c

utils.o: utils.h utils.c
	@echo -e "\nBuilding utils"
	$(CC) $(AM_CFLAGS) -o utils.o -c utils.c

gui.o: gui.h gui.c
	@echo -e "\nBuilding gui"
	$(CC) $(AM_CFLAGS) -o gui.o -c gui.c 

init.o: init.h init.c
	@echo -e "\nBuilding init"
	$(CC) $(AM_CFLAGS) -o init.o -c init.c 

routemanager.o: routemanager.h routemanager.c
	@echo -e "\nBuilding routemanager"
	$(CC) $(AM_CFLAGS) -o routemanager.o -c routemanager.c
	
confmanager.o: confmanager.h confmanager.c
	@echo -e "\nBuilding confmanager"
	$(CC) $(AM_CFLAGS) -o confmanager.o -c confmanager.c
	
controller.o: controller.h controller.c
	@echo -e "\nBuilding controller"
	$(CC) $(AM_CFLAGS) -o controller.o -c controller.c
