/*
  raptor.ino -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "src/raptor.h"
#include <Streaming.h>
#include <avr/wdt.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

Raptor *raptor;

void setup()
{
    // wdt_enable(WDTO_2S); // TODO: WATCHDOG STUFF
    raptor = new Raptor();
    raptor->init();
}

void loop()
{
    switch (raptor->flight_state)
    {
    case 0:
        raptor->launch();
        break;
    case 1:
        raptor->ascent();
        break;
    case 2:
        raptor->descent();
        break;
    case 3:
        raptor->landed();
        break;
    default:
        raptor->flight_state = 2;
    }
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
    char c = raptor->environment->gps->read();
}