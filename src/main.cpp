#include <iostream>
#include <dbus/dbus.h>

int main(int ac, char * av[]) {
    DBusError err;
    dbus_error_init(&err);

    DBusConnection *connection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) {
        std::cerr << "Failed to connect to system bus: " << err.message << std::endl;
        return -1;
    }

    DBusMessage* msg = dbus_message_new_method_call("org.freedesktop.UPower", 
                                                   "/org/freedesktop/UPower", 
                                                   "org.freedesktop.UPower", 
                                                   "Resume");

    if(!msg) {
        std::cerr << "Failed to create message" << std::endl;
        return -1;
    }

    dbus_connection_send(connection, msg, NULL);
    dbus_connection_flush(connection);
    dbus_message_unref(msg);
    dbus_connection_unref(connection);

    return 0;
}