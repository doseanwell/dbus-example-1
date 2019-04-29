#include "dbus_handlers.h"
#include "dbus_error_replies.h"
#include "dbus_xml_define.h"
#include "dbus_bus_define.h"

static int is_for_my_path(DBusMessage *message, const char *path)
{
    const char *mdest = dbus_message_get_destination(message);
    const char *mpath = dbus_message_get_path(message);
    if ((strcmp(mdest, DBUS_BUS_SERVER_NAME) == 0) && (strcmp(mpath, path) == 0))
    {
        return 1;
    }
    return 0;
}
static int is_for_path(DBusMessage *message, const char *path)
{
    const char *mpath = dbus_message_get_path(message);
    if (strcmp(mpath, path) == 0)
    {
        return 1;
    }
    return 0;
}
static int is_for_main_path(DBusConnection *conn, DBusMessage *message, const char *path)
{
    const char *unique_name = dbus_bus_get_unique_name(conn);
    const char *mdest = dbus_message_get_destination(message);
    const char *mpath = dbus_message_get_path(message);
    if ((strcmp(mdest, unique_name) == 0) && (strcmp(mpath, path) == 0))
    {
        return 1;
    }
    return 0;
}

static void _send_introspect_reply(DBusConnection *connection, DBusMessage *message, const char *reply_string)
{
    // this responds simlple string value
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if (reply)
    {
        char *v_STRING = strdup(reply_string);
        retb = dbus_message_append_args(reply,
                                        DBUS_TYPE_STRING, &v_STRING,
                                        DBUS_TYPE_INVALID);
        if (retb == FALSE)
        {
            printf("ERROR: _send_introspect_reply: dbus_message_append_args() failed!\n");
        }
        printf("  sending sig [%s]\n", dbus_message_get_signature(reply));
        retb = dbus_connection_send(connection, reply, NULL);
        if (retb == FALSE)
        {
            printf("ERROR: _send_introspect_reply: could not send reply!\n");
        }
        dbus_connection_flush(connection);
        dbus_message_unref(reply);
        free(v_STRING);
    }
    else
    {
        printf("ERROR: _send_introspect_reply: dbus_message_new_method_return() failed!\n");
    }
}

static void _send_reply_properties_getall_append_attr_s(DBusMessageIter *piter, const char *name, const char *value)
{
    DBusMessageIter dict_iter, var_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter); // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name);             // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "s", &var_iter); // open variant
    dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_STRING, &value);             // write attr value as Variant(string)
    dbus_message_iter_close_container(&dict_iter, &var_iter);                        // close variant
    dbus_message_iter_close_container(piter, &dict_iter);                            // close dict
}
static void _send_reply_properties_getall_append_attr_u(DBusMessageIter *piter, const char *name, dbus_uint32_t value)
{
    DBusMessageIter dict_iter, var_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter); // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name);             // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "u", &var_iter); // open variant
    dbus_message_iter_append_basic(&var_iter, DBUS_TYPE_UINT32, &value);             // write attr value as Variant(uint32)
    dbus_message_iter_close_container(&dict_iter, &var_iter);                        // close variant
    dbus_message_iter_close_container(piter, &dict_iter);                            // close dict
}
static void _send_reply_properties_getall_append_attr_pixmap(DBusMessageIter *piter, const char *name)
{
    DBusMessageIter dict_iter, var_iter, arr_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter);       // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name);                   // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "a(iiay)", &var_iter); // open variant
    // key value - empty pixmap array inside Variant
    // "AttentionIconPixmap" = [Variant: [Argument: a(iiay) {}]]
    dbus_message_iter_open_container(&var_iter, DBUS_TYPE_ARRAY, "(iiay)", &arr_iter);
    dbus_message_iter_close_container(&var_iter, &arr_iter);
    //
    dbus_message_iter_close_container(&dict_iter, &var_iter); // close variant
    dbus_message_iter_close_container(piter, &dict_iter);     // close dict
}
static void _send_reply_properties_getall_append_attr_tooltip(DBusMessageIter *piter,
                                                              const char *name,
                                                              const char *icon_name,
                                                              const char *tip_title,
                                                              const char *tip_text)
{
    DBusMessageIter dict_iter, var_iter, st_iter, arr_iter;
    dbus_message_iter_open_container(piter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_iter);            // open dict
    dbus_message_iter_append_basic(&dict_iter, DBUS_TYPE_STRING, &name);                        // write attr name as dict key
    dbus_message_iter_open_container(&dict_iter, DBUS_TYPE_VARIANT, "(sa(iiay)ss)", &var_iter); // open variant
    // key value - struct (sa(iiay)ss)
    dbus_message_iter_open_container(&var_iter, DBUS_TYPE_STRUCT, NULL, &st_iter); // open struct
    dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &icon_name);
    dbus_message_iter_open_container(&st_iter, DBUS_TYPE_ARRAY, "(iiay)", &arr_iter); // open array
    // no contents in icon array
    dbus_message_iter_close_container(&st_iter, &arr_iter); // close array
    dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &tip_title);
    dbus_message_iter_append_basic(&st_iter, DBUS_TYPE_STRING, &tip_text);
    dbus_message_iter_close_container(&var_iter, &st_iter); // close struct
    //
    dbus_message_iter_close_container(&dict_iter, &var_iter); // close variant
    dbus_message_iter_close_container(piter, &dict_iter);     // close dict
}
static void _send_reply_properties_getall(DBusConnection *connection, DBusMessage *message)
{
    dbus_bool_t retb = FALSE;
    DBusMessage *reply = dbus_message_new_method_return(message);
    if (!reply)
        return;
    //
    DBusMessageIter iter, arr_iter;
    dbus_message_iter_init_append(reply, &iter);
    retb = dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &arr_iter); // open array
    // if( retb == FALSE ) { printf("ERROR: cannot open container for array!\n"); }
    // else { printf("  OK: opened container for array\n"); } // all ok till now
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Category", "ApplicationStatus");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Id", "1");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Title", "test_dbus");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "Status", "Active");
    _send_reply_properties_getall_append_attr_u(&arr_iter, "WindowId", 0);
    // ^^ KDE sets this as int32, not uint32... what?
    _send_reply_properties_getall_append_attr_s(&arr_iter, "IconName", "applications-development");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "OverlayIconName", "");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "AttentionIconName", "");
    _send_reply_properties_getall_append_attr_s(&arr_iter, "AttentionMovieName", "");
    _send_reply_properties_getall_append_attr_pixmap(&arr_iter, "IconPixmap");
    _send_reply_properties_getall_append_attr_pixmap(&arr_iter, "OverlayIconPixmap");
    _send_reply_properties_getall_append_attr_pixmap(&arr_iter, "AttentionIconPixmap");
    _send_reply_properties_getall_append_attr_tooltip(&arr_iter,
                                                      "ToolTip",                      // attribute name
                                                      "applications-development",     // icon name
                                                      "Test DBus",                    // tooltip title
                                                      "sample text to test tray icon" // tooltip text
    );
    dbus_message_iter_close_container(&iter, &arr_iter); // close array
    //
    printf("  sending sig %s\n", dbus_message_get_signature(reply));
    retb = dbus_connection_send(connection, reply, NULL);
    if (retb == FALSE)
    {
        printf("ERROR: _send_reply_properties_getall: could not send reply!\n");
    }
    dbus_message_unref(reply);
}

DBusHandlerResult handle_introspect(DBusConnection *connection, DBusMessage *message)
{
    const char *mpath = dbus_message_get_path(message);
    const char *mdest = dbus_message_get_destination(message);
    const char *msig = dbus_message_get_signature(message);
    printf("  Introspect() for dest \"%s\", path \"%s\", signature [%s]\n", mdest, mpath, msig);
    if (is_for_my_path(message, "/"))
    {
        _send_introspect_reply(connection, message, _introspect_xml_tmpl);
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    else if (is_for_my_path(message, "/StatusNotifierItem"))
    {
        _send_introspect_reply(connection, message, _introspect_xml_tmpl_statusnotifier);
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    else if (is_for_path(message, "/HelloWorld"))
    {
        _send_introspect_reply(connection, message, _introspect_xml_hello);
        dbus_message_unref(message);
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    else if (is_for_main_path(connection, message, "/"))
    {
        _send_introspect_reply(connection, message, _introspect_xml_main_empty);
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
DBusHandlerResult handle_signal(DBusConnection *connection, DBusMessage *message)
{
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
    if ((strcmp(mpath, "/org/freedesktop/DBus") == 0) &&
        (strcmp(mintf, "org.freedesktop.DBus") == 0) &&
        (strcmp(mmemb, "NameAcquired") == 0))
    {
        char *v_STRING = NULL;
        dbus_message_get_args(message, &derr,
                              DBUS_TYPE_STRING, &v_STRING,
                              DBUS_TYPE_INVALID);
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
/*读取消息的参数，并且返回两个参数，一个是bool值stat，一个是整数level*/
static void reply_to_method_call(DBusMessage * msg, DBusConnection * conn)
{
    DBusMessage * reply;
    DBusMessageIter arg;
    char * param = NULL;
    dbus_bool_t stat = TRUE;
    dbus_uint32_t level = 2010;
    dbus_uint32_t serial = 0;
    void *reply_str;
    char *value_str = "Hello World!";
    //从msg中读取参数，这个在上一次学习中学过
    // if(!dbus_message_iter_init(msg,&arg)) {
    //     printf("Message has no args/n");
    // } else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING) {
    //     printf("Arg is not string!/n");
    // } else {
    //     dbus_message_iter_get_basic(&arg,& param);
    // }
    // if(param == NULL) return;

    //创建返回消息reply
    reply = dbus_message_new_method_return(msg);
    if (!reply)
    {
        printf("Out of Memory!\n");
        return;
    }
    reply_str = &value_str;
    //在返回消息中填入两个参数，和信号加入参数的方式是一样的。这次我们将加入两个参数。
    dbus_message_iter_init_append(reply,&arg);
    if(!dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, reply_str)) {
        printf("Out of Memory!/n");
        exit(1);
    }
    // if(!dbus_message_iter_append_basic(&arg,DBUS_TYPE_UINT32,&level)) {
    //     printf("Out of Memory!/n");
    //     exit(1);
    // }
    //发送返回消息
    if( !dbus_connection_send(conn, reply, &serial)){
        printf("Out of Memory/n");
        exit(1);
    }
    dbus_connection_flush (conn);
    dbus_message_unref (reply);
}
DBusHandlerResult handle_helloworld(DBusConnection *connection, DBusMessage *message)
{
    const char *msig = NULL;
    DBusError derr;
    dbus_error_init(&derr);
    msig = dbus_message_get_signature(message);
    printf("msig is:%s\n", msig);
    if( strcmp(msig, "s") == 0 ) {
        dbus_int32_t x = 0;
        char *str = NULL;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
            DBUS_TYPE_STRING, &str,
            DBUS_TYPE_INVALID );
        if( resb == FALSE ) {
            printf("ERROR: StatusNotifierItem.Activate(): Failed to get message args!\n");
            error_reply_invalid_args(connection, message);
        } else {
            printf("  HelloWorld.HelloWorldMethod( %s ): \n", str);
            reply_to_method_call(message, connection);
            // does not need reply
            // DBusMessage *reply;
            // printf("Received method call\n");
            // reply = dbus_message_new_method_return(message);

            // dbus_connection_send(g_connection, reply, NULL);
            // dbus_connection_flush(g_connection);
            // printf("Send reply.\n");
            // printf("=================================================\n");
        }
    } else {
        printf("ERROR: StatusNotifierItem.Activate(): incorrect message signature!\n");
    }
    // if (strcmp(msig, "i") == 0)
    // {
    //     dbus_int32_t x = 0;
    //     char *str = NULL;
    //     dbus_bool_t resb = dbus_message_get_args(message, &derr,
    //                                              DBUS_TYPE_INT32, &x,
    //                                              DBUS_TYPE_INVALID);
    //     if (resb == FALSE)
    //     {
    //         printf("ERROR: StatusNotifierItem.Activate(): Failed to get message args!\n");
    //         error_reply_invalid_args(connection, message);
    //     }
    //     else
    //     {
    //         printf("  HelloWorld.HelloWorldMethod( %d ): \n", x);
    //         // does not need reply
    //         DBusMessage *reply;
    //         printf("Received method call\n");
    //         reply = dbus_message_new_method_return(message);

    //         dbus_connection_send(g_connection, reply, NULL);
    //         dbus_connection_flush(g_connection);
    //         printf("Send reply.\n");
    //         printf("=================================================\n");
    //     }
    // }
    // else
    // {
    //     printf("ERROR: StatusNotifierItem.Activate(): incorrect message signature!\n");
    // }
    
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}
#define ORG_KDE_STATUSNOTIFIER_INTERFACE "org.kde.StatusNotifierItem"
DBusHandlerResult handle_property_get_all(DBusConnection *connection, DBusMessage *message)
{
    const char *msig = dbus_message_get_signature(message);
    DBusError derr;
    dbus_error_init(&derr);
    if (strcmp(msig, "s") == 0)
    {
        char *v_IFACE_NAME = NULL;
        dbus_bool_t resb = dbus_message_get_args(message, &derr,
                                                 DBUS_TYPE_STRING, &v_IFACE_NAME,
                                                 DBUS_TYPE_INVALID);
        if (resb == FALSE)
        {
            printf("ERROR: Properties.GetAll(): Failed to get message args!\n");
            error_reply_unknown_property(connection, message, "whatever");
        }
        else
        {
            printf("  Properties.GetAll( \"%s\" ): \n", v_IFACE_NAME);
            if (strcmp(v_IFACE_NAME, ORG_KDE_STATUSNOTIFIER_INTERFACE) == 0)
            {
                // okay, respond with all our "properties"
                printf("  OK, replying with all our properties\n");
                _send_reply_properties_getall(connection, message);
            }
            else
            {
                printf("  ERROR: unknown interface requested: [%s]\n", v_IFACE_NAME);
                error_reply_unknown_interface(connection, message, v_IFACE_NAME);
            }
        }
    }
    else
    {
        printf("ERROR: Properties.GetAll(): incorrect message signature!\n");
    }
    dbus_message_unref(message);
    return DBUS_HANDLER_RESULT_HANDLED;
}


