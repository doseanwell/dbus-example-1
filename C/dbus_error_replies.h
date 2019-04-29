#ifndef __DBUS_ERROR_REPLIES_H__
#define __DBUS_ERROR_REPLIES_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

void error_reply_unknown_property(DBusConnection *connection, DBusMessage *message, const char *prop_name);
void error_reply_unknown_interface(DBusConnection *connection, DBusMessage *message, const char *iface);
void error_reply_invalid_args(DBusConnection *connection, DBusMessage *message);
void error_reply_read_only_prop(DBusConnection *connection, DBusMessage *message, const char *prop_name);

#endif //__DBUS_ERROR_REPLIES_H__