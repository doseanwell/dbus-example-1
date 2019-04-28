CC = gcc
OPTIONS = -Wall -g -O3 -std=c99
LIBPATH = -L/usr/lib/x86_64-linux-gnu
LIB = `pkg-config --libs dbus-1 dbus-glib-1 glib-2.0` -lpthread -lrt
#INCLUDE = -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include
INCLUDE = `pkg-config --cflags dbus-glib-1 glib-2.0`
#-ldbus-glib-1 -lgobject-2.0 -lglib-2.0
#`pkg-config --cflags dbus-glib-1 glib-2.0`
all : clean client server

client:
	$(CC) $(OPTIONS) $(INCLUDE) -o client client.c common.c $(LIB)

server:
	$(CC) $(OPTIONS) $(INCLUDE) -o server server.c common.c $(LIB)

clean:
	rm -f client server
