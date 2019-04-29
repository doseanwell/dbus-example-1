#include <string.h>
#include "dbus_common.h"

int main(void) {
    DBusConnection *connection;
    int ret;
    int loop_timeout = 100;
    // int iloop = 0;
    connection = init_connection();
    if (!connection) {
        return NULL;
    }

    if (request_bus_name(connection, DBUS_BUS_SERVER_NAME) != 0) {
        return -1;
    }

    while(1) {
        ret = dbus_connection_read_write_dispatch(connection, loop_timeout);
        // printf(" ...  loop %d res = %d\n", iloop++, ret);
        // DBusDispatchStatus dispatch_status = dbus_connection_get_dispatch_status(g_connection);
        // printf("      dispatch status: %d\n", (int)dispatch_status);
    }
    // closeConnect();
    return 0;
}
