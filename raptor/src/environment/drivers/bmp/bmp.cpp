/*
 * bmp.cpp -
 * Contains implementation for functions in bmp.h
 * Utilizes the adafruit BMP280 external library, details here: https://learn.adafruit.com/bmp085
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "bmp.h"
#include <Streaming.h>

/*
 *
 */
BMP *BMP::getInst()
{
  static BMP *inst = NULL;
  if (!inst)
  {
    inst = new BMP;
  }
  return inst;
}

/*
 *	init begins the BMP measurements and
 *   grabs a baseline pressure for alt calculations.
 */
bool BMP::init(bool set_baseline)
{
  // check to make sure it initializes properly
  if (!this->begin())
  { // begin bmp measurements
    Serial.print("No BMP detected!\n");
    return false;
  }

  this->baseline = 1013.25; // put in a fake baseline for the initial calculation, which won't be used
  float temp_sum = 0;
  for (int i = 0; i < 10; i++)
  {
    temp_sum += this->getPressure(); // grab a baseline pressure
    delay(10);
  }
  this->baseline = temp_sum / 10; // average first 10 readings for baseline
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
 * return altitude calculated with baseline pressure
 */
float BMP::getAltitude(void)
{
  return this->readAltitude(this->baseline);
}