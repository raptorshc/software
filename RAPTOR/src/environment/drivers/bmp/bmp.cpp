/*
 * bmp.cpp -
 * Contains implementation for functions in bmp.h
 * Utilizes the adafruit BMP280 external library, details here: https://learn.adafruit.com/bmp085
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "bmp.h"
#include <Streaming.h>

/*
 *	init begins the BMP measurements and
 *   grabs a baseline pressure for alt calculations.
 */
bool BMP::init(bool set_baseline)
{
  // check to make sure it initializes properly
  if (!this->begin())
  { // begin bmp measurements
    Serial.print(F("No BMP detected!\n"));
    return false;
  }

  // if it does and we're in FS0 [LAUNCH], grab a new baseline
  uint8_t counter = 0;
  if (set_baseline)
  {
    this->baseline = 1013.25; // put in a fake baseline for the initial calculation, which won't be used

    this->baseline = this->getPressure(); // grab a baseline pressure
  }
  return true;
}

/*
 * wrapper for readPressure that divides by 100 to actually get hPa
 */
float BMP::getPressure(void)
{
  return this->readPressure() / 100;
}

/*
 * might or might not work
 */
float BMP::getAltitude(void)
{
  return this->readAltitude(this->baseline);
}