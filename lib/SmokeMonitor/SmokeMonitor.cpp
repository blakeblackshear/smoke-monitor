#include "Arduino.h"
#include "SmokeMonitor.h"

SmokeMonitor::SmokeMonitor(int smoke_pin, int co_pin, int alarm_pin)
{
    _smoke_pin = smoke_pin;
    _co_pin = co_pin;
    _alarm_pin = alarm_pin;
    _manual_alarm_on = false;
    _smoke_start = 0;
    _co_start = 0;

    pinMode(_smoke_pin, INPUT);
    pinMode(_co_pin, INPUT);
    pinMode(_alarm_pin, OUTPUT);

    digitalWrite(_alarm_pin, HIGH);
}

void SmokeMonitor::alarm_on()
{
    _manual_alarm_on = true;
    digitalWrite(_alarm_pin, LOW);
    handle_change();
}

void SmokeMonitor::alarm_off()
{
    _manual_alarm_on = false;
    digitalWrite(_alarm_pin, HIGH);
    handle_change();
}

bool SmokeMonitor::is_alarm_on()
{
    return _manual_alarm_on;
}

bool SmokeMonitor::is_smoke_detected()
{
    return _smoke_alarm_on;
}

bool SmokeMonitor::is_co_detected()
{
    return _co_alarm_on;
}

void SmokeMonitor::update()
{
    bool previous_smoke_pin_status = _smoke_pin_status;
    bool previous_co_pin_status = _co_pin_status;
    _smoke_pin_status = digitalRead(_smoke_pin);
    _co_pin_status = digitalRead(_co_pin);

    // rising edge
    if(_smoke_pin_status == HIGH && previous_smoke_pin_status == LOW)
    {
        _smoke_start = millis();
    }

    // rising edge
    if(_co_pin_status == HIGH && previous_co_pin_status == LOW)
    {
        _co_start = millis();
    }

    bool previous_smoke_alarm = _smoke_alarm_on;
    bool previous_co_alarm = _co_alarm_on;

    // if detected for more than 250ms
    _smoke_alarm_on = _smoke_pin_status && ((millis() - _smoke_start) >= 250);
    _co_alarm_on = _co_pin_status && ((millis() - _co_start) >= 250);

    if(previous_smoke_alarm != _smoke_alarm_on || previous_co_alarm != _co_alarm_on)
    {
        handle_change();
    }
}

void SmokeMonitor::handle_change()
{
    callback();
}

void SmokeMonitor::on_change(void (*callback)())
{
    this->callback = callback;
}