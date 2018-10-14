#include "Arduino.h"
#include "SmokeMonitor.h"

SmokeMonitor::SmokeMonitor(int smoke_pin, int co_pin, int alarm_pin)
{
    _smoke_pin = smoke_pin;
    _co_pin = co_pin;
    _alarm_pin = alarm_pin;

    pinMode(_smoke_pin, INPUT);
    pinMode(_co_pin, INPUT);
    pinMode(_alarm_pin, OUTPUT);

    digitalWrite(_alarm_pin, HIGH);
}

void SmokeMonitor::alarm_on()
{
    digitalWrite(_alarm_pin, LOW);
}

void SmokeMonitor::alarm_off()
{
    digitalWrite(_alarm_pin, HIGH);
}

bool SmokeMonitor::is_alarm_on()
{
    return _alarm_pin == LOW;
}

bool SmokeMonitor::is_smoke_detected()
{
    return _smoke_status == HIGH;
}

bool SmokeMonitor::is_co_detected()
{
    return _co_status == HIGH;
}

void SmokeMonitor::update()
{
    int previous_smoke_status = _smoke_status;
    int previous_co_status = _co_status;
    _smoke_status = digitalRead(_smoke_pin);
    _co_status = digitalRead(_co_pin);

    if(previous_smoke_status != _smoke_status || previous_co_status != _co_status)
    {
        handle_change();
    }
}

void SmokeMonitor::handle_change()
{
    callback();
    // hack: delay a bit to avoid input bounces
    delay(500);
}

void SmokeMonitor::on_change(void (*callback)())
{
    this->callback = callback;
}