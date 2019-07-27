#include "src/raptor.h"
#include <Streaming.h>
#include <avr/wdt.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

Raptor *raptor;

void setup()
{
    wdt_enable(WDTO_2S); // TODO: WATCHDOG STUFF
    raptor = new Raptor();
}

void loop()
{
    raptor->launch();
    raptor->ascent();
    raptor->descent();
    raptor->landed();
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
    char c = raptor->environment->gps->read();
}