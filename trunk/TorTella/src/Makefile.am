INCFLAGS = -I./

AM_CFLAGS = $(gtk_CFLAGS) $(gthread_CFLAGS) $(INCFLAGS) -Wall -g
#AM_CFLAGS = $(INCFLAGS) -Wall -g `pkg-config --cflags glib-2.0` `pkg-config --cflags gtk+-2.0`

OBJECTS = tortellaprotocol.o httpmanager.o socketmanager.o packetmanager.o servent.o datamanager.o logger.o utils.o init.o routemanager.o confmanager.o controller.o gui.o

#LIBS = -lpthread `pkg-config --libs glib-2.0` `pkg-config --libs gtk+-2.0` `pkg-config --libs gthread-2.0`

LIBS = $(gthread_LIBS) $(gtk_LIBS)

MAN_FILES = ../docs/documentation/man/man3/chat.3 ../docs/documentation/man/man3/bye_desc.3 ../docs/documentation/man/man3/datamanager.c.3 ../docs/documentation/man/man3/http_packet.3 ../docs/documentation/man/man3/logger.c.3 ../docs/documentation/man/man3/search_desc.3 ../docs/documentation/man/man3/supernodedata_testing.c.3 ../docs/documentation/man/man3/chat.3 ../docs/documentation/man/man3/datamanager.h.3 ../docs/documentation/man/man3/init.c.3 ../docs/documentation/man/man3/logger.h.3 ../docs/documentation/man/man3/searchhits_desc.3 ../docs/documentation/man/man3/tortella.c.3 ../docs/documentation/man/man3/chatclient.3  ../docs/documentation/man/man3/gui.c.3 ../docs/documentation/man/man3/init_data.3 ../docs/documentation/man/man3/message_desc.3 ../docs/documentation/man/man3/servent.c.3 ../docs/documentation/man/man3/tortella_header.3 ../docs/documentation/man/man3/common.h.3 ../docs/documentation/man/man3/gui.h.3 ../docs/documentation/man/man3/init.h.3 ../docs/documentation/man/man3/packetmanager.c.3 ../docs/documentation/man/man3/servent_data.3 ../docs/documentation/man/man3/tortella_packet.3 ../docs/documentation/man/man3/confmanager.c.3 ../docs/documentation/man/man3/gui_testing.c.3 ../docs/documentation/man/man3/init_testing.c.3 ../docs/documentation/man/man3/packetmanager.h.3 ../docs/documentation/man/man3/servent.h.3 ../docs/documentation/man/man3/tortellaprotocol.c.3 ../docs/documentation/man/man3/confmanager.h.3 ../docs/documentation/man/man3/http_header_request.3 ../docs/documentation/man/man3/join_desc.3 ../docs/documentation/man/man3/ping_desc.3 ../docs/documentation/man/man3/servent_testing.c.3 ../docs/documentation/man/man3/tortellaprotocol.h.3 ../docs/documentation/man/man3/confmanager_testing.c.3 ../docs/documentation/man/man3/http_header_response.3 ../docs/documentation/man/man3/leave_desc.3 ../docs/documentation/man/man3/pm_data.3 ../docs/documentation/man/man3/socketmanager.c.3 ../docs/documentation/man/man3/tortellaprotocol_testing.c.3 ../docs/documentation/man/man3/controller.c.3 ../docs/documentation/man/man3/httpmanager.c.3 ../docs/documentation/man/man3/list_desc.3 ../docs/documentation/man/man3/route_entry.3 ../docs/documentation/man/man3/socketmanager.h.3 ../docs/documentation/man/man3/tree_model.3 ../docs/documentation/man/man3/controller.h.3 ../docs/documentation/man/man3/httpmanager.h.3 ../docs/documentation/man/man3/listhits_desc.3 ../docs/documentation/man/man3/routemanager.c.3 ../docs/documentation/man/man3/socketmanager_testing.c.3 ../docs/documentation/man/man3/utils.c.3 ../docs/documentation/man/man3/controller_testing.c.3 ../docs/documentation/man/man3/httpmanager_testing.c.3 ../docs/documentation/man/man3/list_testing.c.3 ../docs/documentation/man/man3/routemanager.h.3 ../docs/documentation/man/man3/SocketTest.c.3 ../docs/documentation/man/man3/utils.h.3

bin_PROGRAMS = tortella
tortella_SOURCES = tortella.c
tortella_DEPENDENCIES = $(OBJECTS) tortella.c
tortella_LDADD = $(OBJECTS)
tortella_LDFLAGS = $(LIBS)
tortella_CFLAGS = $(AM_CFLAGS)
sysconf_DATA = conf/tortella.conf conf/tortella1.conf
man_MANS = $(MAN_FILES)
data_DATA = data/init_data

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
