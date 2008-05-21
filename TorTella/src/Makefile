CC = gcc
CFLAGS = -g -Wall
OBJECTS = tortellaprotocol.o httpmanager.o socketmanager.o packetmanager.o servent.o supernodedata.o logger.o utils.o init.o routemanager.o confmanager.o controller.o gui.o
INCFLAGS = -I./ `pkg-config --cflags glib-2.0`
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = -lpthread `pkg-config --libs glib-2.0`
BIN = ../bin

all: tortella

tortella: $(OBJECTS) tortella.c
	$(CC) -o $(BIN)/tortella tortella.c $(OBJECTS) $(LDFLAGS) $(LIBS)

gui_testing: $(OBJECTS) testing/gui_testing.c
	$(CC) -o $(BIN)/gui_testing testing/gui_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS) `pkg-config --cflags --libs gtk+-2.0`

tortellaprotocol_testing: $(OBJECTS) testing/tortellaprotocol_testing.c
	$(CC) -o $(BIN)/tortellaprotocol_testing testing/tortellaprotocol_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

httpmanager_testing: $(OBJECTS) testing/httpmanager_testing.c
	$(CC) -o $(BIN)/httpmanager_testing testing/httpmanager_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

socketmanager_testing: $(OBJECTS) testing/socketmanager_testing.c
	$(CC) -o $(BIN)/socketmanager_testing testing/socketmanager_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

servent_testing: $(OBJECTS) testing/servent_testing.c
	$(CC) -o $(BIN)/servent_testing testing/servent_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

supernodedata_testing: $(OBJECTS) testing/supernodedata_testing.c
	$(CC) -o $(BIN)/supernodedata_testing testing/supernodedata_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

list_testing: $(OBJECTS) testing/list_testing.c
	$(CC) -o $(BIN)/list_testing testing/list_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)

init_testing: $(OBJECTS) testing/init_testing.c
	$(CC) -o $(BIN)/init_testing testing/init_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)
	
confmanager_testing: $(OBJECTS) testing/confmanager_testing.c
	$(CC) -o $(BIN)/confmanager_testing testing/confmanager_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)
	
controller_testing: $(OBJECTS) testing/controller_testing.c
	$(CC) -o $(BIN)/controller_testing testing/controller_testing.c $(OBJECTS) $(LDFLAGS) $(LIBS) $(INCFLAGS)
	
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
	$(CC) $(CFLAGS) $(INCFLAGS) -o gui.o -c gui.c `pkg-config --cflags gtk+-2.0`

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

count:
	wc -l *.c *.h */*.c

clean:
	rm -f *.o testing/*.o

.PHONY: all
.PHONY: count
.PHONY: clean
