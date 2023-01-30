#include <iostream>
#include <stdlib.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#include <unistd.h>
#include <libcec/cec.h>
#include <libcec/cecloader.h>

#define ON 1
#define OFF 0

#define SERVER_DEFAULT (-1)
#define DONT_CHANGE -2

#define DEFAULT_ON (-50)
#define DEFAULT_TIMEOUT (-600)

#define ALL -1
#define TIMEOUT 1
#define INTERVAL 2
#define PREFER_BLANK 3
#define ALLOW_EXP 4


class Screensaver {
private:
    CEC::ICECAdapter * adapter_;
    Display* dpy_;
    int dpms_supported_;
private:
    static void cec_callback_command_received(void *, CEC::cec_command const *);
    static void cec_callback_log_message(void *, CEC::cec_log_message const *);
    static void cec_callback_configuration_changed(void *, CEC::libcec_configuration const *);
    static void cec_callback_key_press(void *, CEC::cec_keypress const *);
    static void cec_callback_alert(void *, CEC::libcec_alert, CEC::libcec_parameter);
    static int cec_callback_menu_state_changed(void *, CEC::cec_menu_state);
    static void cec_callback_source_activated(void *, CEC::cec_logical_address, uint8_t);

    static void set_saver(Display *dpy, int mask, int value)
    {
        int timeout, interval, prefer_blank, allow_exp;

        XGetScreenSaver(dpy, &timeout, &interval, &prefer_blank, &allow_exp);
        if (mask == TIMEOUT)
            timeout = value;
        if (mask == INTERVAL)
            interval = value;
        if (mask == PREFER_BLANK)
            prefer_blank = value;
        if (mask == ALLOW_EXP)
            allow_exp = value;
        if (mask == ALL) {
            timeout = SERVER_DEFAULT;
            interval = SERVER_DEFAULT;
            prefer_blank = DefaultBlanking;
            allow_exp = DefaultExposures;
        }
        XSetScreenSaver(dpy, timeout, interval, prefer_blank, allow_exp);
        if (mask == ALL && value == DEFAULT_TIMEOUT) {
            XGetScreenSaver(dpy, &timeout, &interval, &prefer_blank, &allow_exp);
            if (!timeout)
                XSetScreenSaver(dpy, -DEFAULT_TIMEOUT, interval, prefer_blank,
                        allow_exp);
        }
    }

    static std::string cec_level_to_string(CEC::cec_log_level lev) {
        switch(lev) {
        case CEC::CEC_LOG_ERROR: return "ERROR";
        case CEC::CEC_LOG_WARNING: return "WARNING";
        case CEC::CEC_LOG_NOTICE: return "NOTICE";
        case CEC::CEC_LOG_DEBUG: return "DEBUG";
        case CEC::CEC_LOG_TRAFFIC: return "TRAFFIC";
        case CEC::CEC_LOG_ALL: return "ALL"; // shouldn't be used
        }
        return "UNKNOWN";
    }
    static std::string cec_alert_to_string(CEC::libcec_alert alert) {
        switch(alert) {
        case CEC::CEC_ALERT_SERVICE_DEVICE: return "SERVICE_DEVICE";
        case CEC::CEC_ALERT_CONNECTION_LOST: return "CONNECTION_LOST";
        case CEC::CEC_ALERT_PERMISSION_ERROR: return "PERMISSION_ERROR";
        case CEC::CEC_ALERT_PORT_BUSY: return "PORT_BUSY";
        case CEC::CEC_ALERT_PHYSICAL_ADDRESS_ERROR: return "PHYSICAL_ADDRESS_ERROR";
        case CEC::CEC_ALERT_TV_POLL_FAILED: return "TV_POLL_FAILED";
        }
        return "UNKONWN";
    }

    void command_received(CEC::cec_command const * cmd);
    void log_message(CEC::cec_log_message const * msg);
    void configuration_changed(CEC::libcec_configuration const * config);
    void key_press(CEC::cec_keypress const * key);
    void alert(CEC::libcec_alert alert, CEC::libcec_parameter param);
    int menu_state_changed(CEC::cec_menu_state state);
    void source_activated(CEC::cec_logical_address address, uint8_t param);
    void open_display()
    {
        dpy_ = XOpenDisplay(nullptr);
    }
    void close_display()
    {
        // Close the connection to the X server
        XCloseDisplay(dpy_);
    }
    void setup_libcec()
    {   
        CEC::ICECCallbacks         g_callbacks;
        CEC::libcec_configuration  g_config;
        g_callbacks.commandReceived = Screensaver::cec_callback_command_received;
        g_callbacks.logMessage = Screensaver::cec_callback_log_message;
        g_callbacks.configurationChanged = Screensaver::cec_callback_configuration_changed;
        g_callbacks.keyPress = Screensaver::cec_callback_key_press;
        g_callbacks.alert = Screensaver::cec_callback_alert;
        g_callbacks.menuStateChanged = Screensaver::cec_callback_menu_state_changed;
        g_callbacks.sourceActivated = Screensaver::cec_callback_source_activated;
        g_config.callbacks = &g_callbacks;
        g_config.callbackParam = this;

        adapter_ = LibCecInitialise(&g_config);
    }
    void teardown_libcec() {

        // cec_command powerOn;
        // powerOn.opcode = CEC_OPCODE_IMAGE_VIEW_ON;

        // // turn on TV
        // parser->Transmit(powerOn);
        // usleep(1000000);
        

        UnloadLibCec(adapter_);
    }
    void initialize() {
        int major, minor;
        dpms_supported_ = DPMSQueryExtension(dpy_, &major, &minor);
        
    }
public:
    Screensaver() : adapter_(nullptr), dpy_(nullptr) {
        setup_libcec();
        open_display();
        initialize();
    } 
    ~Screensaver() {
        close_display();
        teardown_libcec();
    }

    bool dpms_supported() const { return dpms_supported_; }
    
};

void Screensaver::cec_callback_command_received(void * self, const CEC::cec_command * cmd) {
    reinterpret_cast<Screensaver*>(self)->command_received(cmd);
};
void Screensaver::cec_callback_log_message(void * self, CEC::cec_log_message const * msg) {
    reinterpret_cast<Screensaver*>(self)->log_message(msg);
}
void Screensaver::cec_callback_configuration_changed(void * self, CEC::libcec_configuration const * config) {
    reinterpret_cast<Screensaver*>(self)->configuration_changed(config);
}
void Screensaver::cec_callback_key_press(void * self, CEC::cec_keypress const * keypress) {
    reinterpret_cast<Screensaver*>(self)->key_press(keypress);
}
void Screensaver::cec_callback_alert(void * self, CEC::libcec_alert cec_alert, CEC::libcec_parameter param) {
    reinterpret_cast<Screensaver*>(self)->alert(cec_alert, param);
}
int Screensaver::cec_callback_menu_state_changed(void * self, CEC::cec_menu_state menu) {
    return reinterpret_cast<Screensaver*>(self)->menu_state_changed(menu);
}
void Screensaver::cec_callback_source_activated(void * self, CEC::cec_logical_address addr, uint8_t active) {
    reinterpret_cast<Screensaver*>(self)->source_activated(addr, active);
}

void Screensaver::command_received(CEC::cec_command const * cmd) {}
void Screensaver::log_message(CEC::cec_log_message const * msg) {
    std::cout << "CEC " << cec_level_to_string(msg->level) << ": " << msg->message << std::endl;
}
void Screensaver::configuration_changed(CEC::libcec_configuration const * config) {}
void Screensaver::key_press(CEC::cec_keypress const * key) {}
void Screensaver::alert(CEC::libcec_alert alert, CEC::libcec_parameter param) {
    std::cout << "CEC ALERT: " << cec_alert_to_string(alert) << std::endl;
}
int Screensaver::menu_state_changed(CEC::cec_menu_state state) {
    return 0;
}
void Screensaver::source_activated(CEC::cec_logical_address address, uint8_t param) {}

int main(int ac, char * av[]) {
    Screensaver saver();

    // Open a connection to the X server
    // Display* dpy = XOpenDisplay(nullptr);

    // int major, minor;
    // Bool dpms_supported_ = DPMSQueryExtension(dpy, &major, &minor);
    // if (!dpms_supported_) {
    //     // DPMS not supported
    //     std::cerr << "DPMS not supported" << std::endl;
    //     XCloseDisplay(dpy);
    //     return -1;
    // }


    // // Get the current DPMS state
    // BOOL on;
    // CARD16 state;
    // DPMSInfo(dpy, &state, &on);

    // std::cout << "state: " << state << std::endl;
    // std::cout << "on: " << on << std::endl;

    // if(!on) {
    //     DPMSEnable(dpy);
    //     usleep(100000);
    // }


    // switch(state) {
    // case DPMSModeOn:
    //     std::cout << "DPMSModeOn" << std::endl;
    //     DPMSSetTimeouts(dpy, 0, 0, 0);
    //     DPMSForceLevel(dpy, DPMSModeStandby);
    //     break;
    // case DPMSModeStandby:
    //     std::cout << "DPMSModeStandby" << std::endl;
    //     DPMSSetTimeouts(dpy, 600, 600, 600);
    //     DPMSForceLevel(dpy, DPMSModeOn);
    //     break;
    // case DPMSModeSuspend:
    //     std::cout << "DPMSModeSuspend" << std::endl;
    //     DPMSSetTimeouts(dpy, 600, 600, 600);
    //     DPMSForceLevel(dpy, DPMSModeOn);
    //     break;
    // case DPMSModeOff:
    //     std::cout << "DPMSModeOff" << std::endl;
    //     DPMSForceLevel(dpy, DPMSModeOn);
    //     DPMSSetTimeouts(dpy, 600, 600, 600);
    //     break;
    // }
    // usleep(100000);

    // // Close the connection to the X server
    // XCloseDisplay(dpy);

    return 0;
}