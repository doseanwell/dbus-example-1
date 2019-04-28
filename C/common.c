#include "common.h"

extern DBusConnection *g_connection;

static const char *_introspect_xml_main_empty = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
  </interface>\
</node>";


static const char *_introspect_xml_tmpl = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
  </interface>\
  <interface name=\"org.freedesktop.DBus.Properties\">\
    <method name=\"Get\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"value\" type=\"v\" direction=\"out\"/>\
    </method>\
    <method name=\"Set\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"value\" type=\"v\" direction=\"in\"/>\
    </method>\
    <method name=\"GetAll\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"props\" type=\"{sv}\" direction=\"out\"/>\
    </method>\
  </interface>\
  <node name=\"HelloWorld\"/>\
  <node name=\"StatusNotifierItem\"/>\
</node>";


// DONE: all
static const char *_introspect_xml_tmpl_statusnotifier = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.kde.StatusNotifierItem\">\
    <property name=\"Category\" type=\"s\" access=\"read\"/>\
    <property name=\"Id\" type=\"s\" access=\"read\"/>\
    <property name=\"Title\" type=\"s\" access=\"read\"/>\
    <property name=\"Status\" type=\"s\" access=\"read\"/>\
    <property name=\"WindowId\" type=\"u\" access=\"read\"/>\
    <property name=\"IconName\" type=\"s\" access=\"read\"/>\
    <property name=\"IconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"OverlayIconName\" type=\"s\" access=\"read\"/>\
    <property name=\"OverlayIconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"AttentionIconName\" type=\"s\" access=\"read\"/>\
    <property name=\"AttentionIconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"AttentionMovieName\" type=\"s\" access=\"read\"/>\
    <property name=\"ToolTip\" type=\"(sa(iiay)ss)\" access=\"read\"/>\
    <method name=\"ContextMenu\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"Activate\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"SecondaryActivate\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"Scroll\">\
      <arg name=\"delta\" type=\"i\" direction=\"in\"/>\
      <arg name=\"orientation\" type=\"s\" direction=\"in\"/>\
    </method>\
    <signal name=\"NewTitle\"></signal>\
    <signal name=\"NewIcon\"></signal>\
    <signal name=\"NewAttentionIcon\"></signal>\
    <signal name=\"NewOverlayIcon\"></signal>\
    <signal name=\"NewToolTip\"></signal>\
    <signal name=\"NewStatus\">\
      <arg name=\"status\" type=\"s\"/>\
    </signal>\
  </interface>\
</node>";

// sean: all
static const char *_introspect_xml_hello = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"com.kssim.test.bus\">\
    <method name=\"HelloWorldMethod\">\
      <arg name=\"x\" type=\"s\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"out\"/>\
    </method>\
    <signal name=\"HelloWorldSignal\">\
      <arg name=\"mes\" type=\"s\"/>\
    </signal>\
  </interface>\
</node>";
void print_error_and_free(DBusError *perr) {
    printf("error name: %s\n", perr->name);
    printf("error message: %s\n", perr->message);
    dbus_error_free(perr);
}

int initConnection(void) {
    DBusError error;

    dbus_error_init(&error);
    g_connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "Connection error : %s\n", error.message);
        dbus_error_free(&error);
        return -1;
    }

    if (g_connection == NULL) {
        fprintf(stderr, "Connection is NULL.\n");
        return -1;
    }

    return 0;
}

static int is_for_my_path( DBusMessage *message, const char *path ) {
    const char *mdest = dbus_message_get_destination(message);
    const char *mpath = dbus_message_get_path(message);
    if( (strcmp(mdest, DBUS_BUS_SERVER_NAME) == 0) && (strcmp(mpath, path) == 0) ) {
        return 1;
    }
    return 0;
}


static int is_for_main_path( DBusConnection *conn, DBusMessage *message, const char *path ) {
    const char *unique_name = dbus_bus_get_unique_name(conn);
    const char *mdest = dbus_message_get_destination(message);
    const char *mpath = dbus_message_get_path(message);
    if( (strcmp(mdest, unique_name) == 0) && (strcmp(mpath, path) == 0) ) {
        return 1;
    }
    return 0;
}

static void _send_introspect_reply(DBusConnection *connection, DBusMessage *message, const char *reply_string) {
    // this responds simlple string value
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if( reply ) {
        char *v_STRING = strdup(reply_string);
        retb = dbus_message_append_args(reply,
            DBUS_TYPE_STRING, &v_STRING,
            DBUS_TYPE_INVALID);
        
        if( retb == FALSE ) {
            printf("ERROR: _send_introspect_reply: dbus_message_append_args() failed!\n");
        }
        printf("  sending sig [%s]\n", dbus_message_get_signature(reply) );
        retb = dbus_connection_send(connection, reply, NULL);
        if( retb == FALSE ) {
            printf("ERROR: _send_introspect_reply: could not send reply!\n");
        }
        dbus_connection_flush(connection);
        dbus_message_unref(reply);
        free(v_STRING);
    } else {
        printf("ERROR: _send_introspect_reply: dbus_message_new_method_return() failed!\n");
    }
}

// Function to unregister this handler.  (???)
void ObjectPathUnregisterFunction(DBusConnection *connection, void *user_data) {
    printf("ObjectPathUnregisterFunction() called\n");
}

DBusHandlerResult handle_introspect(DBusConnection *connection, DBusMessage *message) {
    const char *mpath = dbus_message_get_path(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *msig = dbus_message_get_signature(message);
    printf("  Introspect() for dest \"%s\", path \"%s\", signature [%s]\n", mdest, mpath, msig);
    if( is_for_my_path(message, "/") ) {
        _send_introspect_reply( connection, message, _introspect_xml_tmpl);
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if( is_for_my_path(message, "/StatusNotifierItem") ) {
        _send_introspect_reply( connection, message, _introspect_xml_tmpl_statusnotifier );
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if( is_for_my_path(message, "/HelloWorld") ) {
        _send_introspect_reply( connection, message, _introspect_xml_hello );
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    } else if( is_for_main_path(connection, message, "/" ) ) {
        _send_introspect_reply( connection, message, _introspect_xml_main_empty );
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    // enum DBusHandlerResult
    // DBUS_HANDLER_RESULT_HANDLED -Message has had its effect - no need to run more handlers. 
    // DBUS_HANDLER_RESULT_NOT_YET_HANDLED - Message has not had any effect 
    //                                    - see if other handlers want it. 
    // DBUS_HANDLER_RESULT_NEED_MEMORY - Please try again later with more memory.
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
/* Message from org.freedesktop.DBus type = 4, no_reply = 1: 
  dest   = :1.482
  path   = /org/freedesktop/DBus
  iface  = org.freedesktop.DBus
  member = NameAcquired
  SIGNAL: marking as handled! */
DBusHandlerResult handle_signal(DBusConnection *connection, DBusMessage *message) {
    const char *mpath = dbus_message_get_path(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *mmemb = dbus_message_get_member(message);
    const char *mintf = dbus_message_get_interface(message);
    DBusError derr;
    dbus_error_init(&derr);
    printf("  dest   = %s\n", mdest);
    printf("  path   = %s\n", mpath);
    printf("  iface  = %s\n", mintf);
    printf("  member = %s\n", mmemb);
    if( (strcmp(mpath, "/org/freedesktop/DBus") == 0) &&
        (strcmp(mintf, "org.freedesktop.DBus") == 0) &&
        (strcmp(mmemb, "NameAcquired") == 0) ) {
        char *v_STRING = NULL;
        dbus_message_get_args( message, &derr,
                              DBUS_TYPE_STRING, &v_STRING,
                              DBUS_TYPE_INVALID );
        printf("  SIGNAL NameAcquired(): [%s]\n", v_STRING);
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    // enum DBusHandlerResult
    // DBUS_HANDLER_RESULT_HANDLED -Message has had its effect - no need to run more handlers. 
    // DBUS_HANDLER_RESULT_NOT_YET_HANDLED - Message has not had any effect 
    //                                    - see if other handlers want it. 
    // DBUS_HANDLER_RESULT_NEED_MEMORY - Please try again later with more memory.
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
// Function to handle messages.
DBusHandlerResult ObjectPathMessageFunction(
            DBusConnection *connection,
            DBusMessage *message,
            void *user_data ) {
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
    if( dbus_message_is_method_call(message, "org.freedesktop.DBus.Introspectable", "Introspect") ) {
        return handle_introspect(connection, message);
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
    if( mtype == DBUS_MESSAGE_TYPE_SIGNAL ) {
        return handle_signal(connection, message);
    }
    
    if( no_reply == 0 ) {
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
int requestBusName(const char* busName) {
    DBusError error;
    int ret;
    DBusObjectPathVTable root_vtable;
    dbus_bool_t resb = FALSE;
    // int loop_timeout = 5000;
    // int loops_max = 100;
    // int iloop = 0;

    memset(&root_vtable, 0, sizeof(root_vtable));
    root_vtable.unregister_function = ObjectPathUnregisterFunction;
    root_vtable.message_function = ObjectPathMessageFunction;

    dbus_error_init(&error);

    //resb = dbus_connection_try_register_object_path(pconn, "/", &root_vtable, NULL, &error);
    resb = dbus_connection_try_register_fallback(g_connection, "/", &root_vtable, NULL, &error);
    if (resb == FALSE) {
        printf("Failed to register fallback handler!\n");
        print_error_and_free(&error);
    }
    printf("Root path fallback handler registered OK!\n");
    dbus_connection_read_write_dispatch(g_connection, 1000);

    printf("Connected to session bus as unique name: [%s]\n", dbus_bus_get_unique_name(g_connection));
    dbus_connection_set_exit_on_disconnect(g_connection, FALSE);
    dbus_connection_read_write_dispatch(g_connection, 1000);

    ret = dbus_bus_request_name(g_connection, busName, DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (dbus_error_is_set(&error)) {
        fprintf(stderr, "Request bus name error : %s\n", error.message);
        dbus_error_free(&error);
        return -1;
    }

    if (ret!= DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "Not primary owner : %d \n", ret);
        return -1;
    }
    // printf("Falling into loop\n");
    // for(iloop = 0; iloop < loops_max; iloop++ ) {
    //     ret = dbus_connection_read_write_dispatch(g_connection, loop_timeout);
    //     printf(" ...  loop %d res = %d\n", iloop, ret);
    //     //DBusDispatchStatus dispatch_status = dbus_connection_get_dispatch_status(pconn);
    //     //printf("      dispatch status: %d\n", (int)dispatch_status);
    // }
    return 0;
}
void closeConnect()
{
    DBusError error;
    dbus_error_init(&error);
    dbus_bus_release_name(g_connection, DBUS_BUS_SERVER_NAME, &error);
    dbus_connection_unref(g_connection);
    printf("Unreferenced g_connection\n");
}