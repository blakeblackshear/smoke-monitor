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
        unsigned long _smoke_start;
        unsigned long _co_start;
        bool _smoke_pin_status;
        bool _co_pin_status;
        bool _smoke_alarm_on;
        bool _co_alarm_on;
        bool _manual_alarm_on;
        void (*callback)();
        void handle_change();
};

#endif