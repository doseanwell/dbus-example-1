#include "dbus_common.h"

void print_error_and_free(DBusError *perr)
{
    printf("error name: %s\n", perr->name);
    printf("error message: %s\n", perr->message);
    dbus_error_free(perr);
}

DBusConnection *init_connection(void)
{
    DBusError error;
    DBusConnection *connection;

    dbus_error_init(&error);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

    if (dbus_error_is_set(&error))
    {
        fprintf(stderr, "Connection error : %s\n", error.message);
        dbus_error_free(&error);
        return NULL;
    }

    if (connection == NULL)
    {
        fprintf(stderr, "Connection is NULL.\n");
        return NULL;
    }

    return connection;
}

// Function to unregister this handler.  (???)
void ObjectPathUnregisterFunction(DBusConnection *connection, void *user_data)
{
    printf("ObjectPathUnregisterFunction() called\n");
}

// Function to handle messages.
DBusHandlerResult ObjectPathMessageFunction(
    DBusConnection *connection,
    DBusMessage *message,
    void *user_data)
{
    const char *mpath = dbus_message_get_path(message);
    const char *mintf = dbus_message_get_interface(message);
    const char *mmemb = dbus_message_get_member(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *sender = dbus_message_get_sender(message);
    int mtype = dbus_message_get_type(message);
    int no_reply = dbus_message_get_no_reply(message);
    printf("Message from %s type = %d, no_reply = %d: \n", sender, mtype, no_reply);
    printf("  dest   = %s\n", mdest);
    printf("  path   = %s\n", mpath);
    printf("  iface  = %s\n", mintf);
    printf("  member = %s\n", mmemb);
    if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Introspectable", "Introspect"))
    {
        return handle_introspect(connection, message);
    }
    else if (strcmp(mpath, "/HelloWorld") == 0) //判断path
    {
        if (dbus_message_is_method_call(message, "com.kssim.test.bus", "HelloWorldMethod"))
            return handle_helloworld(connection, message);
        else if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Introspectable", "Introspect"))
            return handle_introspect(connection, message);
        else if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "GetAll"))
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    else if (dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "GetAll"))
    {
        return handle_property_get_all(connection, message);
    }
    // else if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "Get") ) {
    //     return handle_property_get(connection, message);
    // } else if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "Set") ) {
    //     return handle_property_set(connection, message);
    // } else if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Properties", "GetAll") ) {
    //     return handle_property_get_all(connection, message);
    // } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "Activate") ) {
    //     return handle_icon_activate(connection, message);
    // } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "SecondaryActivate") ) {
    //     return handle_icon_activate_secondry(connection, message);
    // } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "ContextMenu") ) {
    //     return handle_icon_context_menu(connection, message);
    // } else if( dbus_message_is_method_call(message, "org.kde.StatusNotifierItem", "Scroll") ) {
    //     return handle_icon_scroll(connection, message);
    // }

    // handle signals
    if (mtype == DBUS_MESSAGE_TYPE_SIGNAL)
    {
        return handle_signal(connection, message);
    }

    if (no_reply == 0)
    {
        printf("  WARNING: unhandled message waiting for reply!\n");
    }

    // enum DBusHandlerResult
    // DBUS_HANDLER_RESULT_HANDLED -Message has had its effect - no need to run more handlers.
    // DBUS_HANDLER_RESULT_NOT_YET_HANDLED - Message has not had any effect
    //                                    - see if other handlers want it.
    // DBUS_HANDLER_RESULT_NEED_MEMORY - Please try again later with more memory.
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
int request_bus_name(DBusConnection *connection, const char *busName)
{
    DBusError error;
    int ret;
    DBusObjectPathVTable root_vtable;
    dbus_bool_t resb = FALSE;

    memset(&root_vtable, 0, sizeof(root_vtable));
    root_vtable.unregister_function = ObjectPathUnregisterFunction;
    root_vtable.message_function = ObjectPathMessageFunction;

    dbus_error_init(&error);

    //resb = dbus_connection_try_register_object_path(pconn, "/", &root_vtable, NULL, &error);
    resb = dbus_connection_try_register_fallback(connection, "/", &root_vtable, NULL, &error);
    if (resb == FALSE)
    {
        printf("Failed to register fallback handler!\n");
        print_error_and_free(&error);
    }
    printf("Root path fallback handler registered OK!\n");
    dbus_connection_read_write_dispatch(connection, 1000);

    printf("Connected to session bus as unique name: [%s]\n", dbus_bus_get_unique_name(connection));
    dbus_connection_set_exit_on_disconnect(connection, FALSE);
    dbus_connection_read_write_dispatch(connection, 1000);

    ret = dbus_bus_request_name(connection, busName, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (dbus_error_is_set(&error))
    {
        fprintf(stderr, "Request bus name error : %s\n", error.message);
        dbus_error_free(&error);
        return -1;
    }

    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
    {
        fprintf(stderr, "Not primary owner : %d \n", ret);
        return -1;
    }

    return 0;
}
void close_connect(DBusConnection *connection, const char *busName)
{
    DBusError error;
    dbus_error_init(&error);
    dbus_bus_release_name(connection, busName, &error);
    dbus_connection_unref(connection);
    printf("Unreferenced connection\n");
}