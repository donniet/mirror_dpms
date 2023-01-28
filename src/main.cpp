#include <iostream>
#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>

int main(int ac, char * av[]) {
    // Open a connection to the X server
    Display* dpy = XOpenDisplay(nullptr);

    // Get the current DPMS state
    BOOL on;
    CARD16 state;
    DPMSInfo(dpy, &state, &on);

    if (on) {
        // Turn DPMS off
        DPMSDisable(dpy);
    } else {
        // Turn DPMS on
        DPMSEnable(dpy);
    }

    // Close the connection to the X server
    XCloseDisplay(dpy);

    return 0;
}