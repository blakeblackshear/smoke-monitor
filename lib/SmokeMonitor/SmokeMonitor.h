#ifndef SmokeMonitor_h
#define SmokeMonitor_h

#include "Arduino.h"

class SmokeMonitor
{
    public:
        SmokeMonitor(int smoke_pin, int co_pin, int alarm_pin);
        void alarm_on();
        void alarm_off();
        bool is_alarm_on();
        bool is_smoke_detected();
        bool is_co_detected();
        void update();
        void on_change(void (*callback)());
    private:
        int _smoke_pin;
        int _co_pin;
        int _alarm_pin;
        bool _smoke_status;
        bool _co_status;
        void (*callback)();
        void handle_change();
};

#endif