#include <iostream>
#include <stdlib.h>
#include <stdarg.h>
#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <unistd.h>

int main(int ac, char * av[]) {
    // Open a connection to the X server
    Display* dpy = XOpenDisplay(nullptr);

    // Get the current DPMS state
    BOOL on;
    CARD16 state;
    DPMSInfo(dpy, &state, &on);

    std::cout << "state: " << state << std::endl;
    std::cout << "on: " << on << std::endl;

    if(!on) {
        DPMSEnable(dpy);
        usleep(100000);
    }


    switch(state) {
    case DPMSModeOn:
        std::cout << "DPMSModeOn" << std::endl;
        DPMSSetTimeouts(dpy, 1, 1, 1);
        DPMSForceLevel(dpy, DPMSModeStandby);
        break;
    case DPMSModeStandby:
        std::cout << "DPMSModeStandby" << std::endl;
        DPMSForceLevel(dpy, DPMSModeOn);
        break;
    case DPMSModeSuspend:
        std::cout << "DPMSModeSuspend" << std::endl;
        DPMSForceLevel(dpy, DPMSModeOn);
        break;
    case DPMSModeOff:
        std::cout << "DPMSModeOff" << std::endl;
        DPMSForceLevel(dpy, DPMSModeOn);
        break;
    }
    usleep(100000);


    // Close the connection to the X server
    XCloseDisplay(dpy);

    return 0;
}