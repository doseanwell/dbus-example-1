#!/usr/bin/env python

import dbus

# DBUS_BUS_NAME = 'com.kssim.test'
# DBUS_OBJECT_PATH = '/com/kssim/test'
DBUS_BUS_SERVER_NAME = "com.kssim.test.server"
DBUS_BUS_OBJECT_PATH = "/HelloWorld"
DBUS_BUS_INTERFACE = "com.kssim.test.bus"
if __name__ == '__main__':
    try:
        bus = dbus.SessionBus()
        bus_object = bus.get_object(DBUS_BUS_SERVER_NAME, DBUS_BUS_OBJECT_PATH)
        bus_interface = dbus.Interface(bus_object, DBUS_BUS_INTERFACE)

        # bus_interface.receive_signal('send_signal')

        # reply_message = bus_object.get_dbus_method('reply_msg', DBUS_BUS_SERVER_NAME)
        # send_arg_and_msg = bus_object.get_dbus_method('send_arg_and_msg', DBUS_BUS_SERVER_NAME)
        bus_interface.HelloWorldMethod(1)
        print "Run success"
        # print(reply_message())
        # print(send_arg_and_msg('Hello', 'world'))
    except:
        print ('Server is offline.')
