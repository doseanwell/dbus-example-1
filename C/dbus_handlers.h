#ifndef __DBUS_HANDLERS_H__
#define __DBUS_HANDLERS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

DBusHandlerResult handle_introspect(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_signal(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_helloworld(DBusConnection *connection, DBusMessage *message);
DBusHandlerResult handle_property_get_all(DBusConnection *connection, DBusMessage *message);
#endif //__DBUS_HANDLERS_H__