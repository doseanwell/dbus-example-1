#ifndef __DBUS_COMMON_H__
#define __DBUS_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <pthread.h>
#include "dbus_bus_define.h"

DBusConnection *init_connection(void);
int request_bus_name(DBusConnection *connection, const char* busName);
void close_connect(DBusConnection *connection, const char *busName);
#endif //__DBUS_COMMON_H__
