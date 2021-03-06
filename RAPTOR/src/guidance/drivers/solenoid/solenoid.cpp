/*
  solenoid.cpp -
  Class written for use with solenoids, as well as switches and LED indicators for the solenoid.
  Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "solenoid.h"

Solenoid::Solenoid(uint8_t data, uint8_t sw, uint8_t led)
{
    this->dta_pin = data;
    this->sw_pin = sw;
    this->led_pin = led;

    // set pin directions
    pinMode(this->dta_pin, OUTPUT); // solenoid data line
    pinMode(this->sw_pin, INPUT);   // solenoid switch
    pinMode(this->led_pin, OUTPUT); // led indicator
}

void Solenoid::secure(void)
{
    digitalWrite(this->dta_pin, LOW);
}

void Solenoid::release(void)
{
    digitalWrite(this->dta_pin, HIGH);
}

bool Solenoid::read_switch(void)
{
    if (digitalRead(this->sw_pin))
    {
        digitalWrite(this->led_pin, HIGH);
        return true;
    }
    else
    {
        digitalWrite(this->led_pin, LOW);
        return false;
    }
}