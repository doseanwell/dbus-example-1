CC = gcc
OPTIONS = -Wall -g -O3 -std=c99
LIBPATH = -L/usr/lib/x86_64-linux-gnu
LIB = `pkg-config --libs dbus-1 dbus-glib-1 glib-2.0` -lpthread -lrt

INCLUDE = `pkg-config --cflags dbus-glib-1 glib-2.0`

all : clean client server

client:
	$(CC) $(OPTIONS) $(INCLUDE) -o client client.c dbus_common.c dbus_handlers.c dbus_error_replies.c $(LIB)

server:
	$(CC) $(OPTIONS) $(INCLUDE) -o server server.c dbus_common.c dbus_handlers.c dbus_error_replies.c $(LIB)

clean:
	rm -f client server
