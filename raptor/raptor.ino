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
        raptor->ascent();
        break;
    case 1:
        raptor->descent();
        break;
    case 2:
        raptor->landed();
        break;
    default:
        raptor->flight_state = 1;
    }
}
