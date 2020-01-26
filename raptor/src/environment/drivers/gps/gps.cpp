/*
 * gps.cpp -
 * Contains implementation for functions in gps.h.
 * Utilizes Adafruit Ultimate GPS library, details: https://learn.adafruit.com/adafruit-ultimate-gps
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "gps.h"
#include "math.h"

/*
 * init begins the GPS readings, sets up the timer counter used for the
 *  millisecond interrupt, which will query and parse the GPS data
 */
void GPS::init(void)
{
  // ****TODO**** This needs to be changed or corrected when we try this again
  this->s->begin(9600);
  // delay(100)
  // this->s->write("$PMTK220,100*2F<CR><LF>");
  // this->s->write("$PMTK251,115200*27<CR><LF>");
  // delay(100);
  // this->s->end();
  // delay(100);
  // this->s->begin(115200);
  

}

/*
 * update performs all needed actions for parsing
 *  a new set of data collected by the GPS
 */
void GPS::update(void)
{
  while (s->available() > 0)
    this->encode(s->read());

  if (this->location.isValid())
  { // if we have a new location, parse it
    if (this->first_gps)
    { // set the ground level altitude on our first reading
      this->first_gps = false;
      this->init_alt = this->altitude.meters();
    }

    // correct the coordinates to decimal-degrees and altitude to AGL
    this->agl = this->altitude.meters() - this->init_alt;

    // to-do, figure out decimal stuff
  }
}

// /*
//  * converts lat/long from Adafruit degree-minute format to decimal-degrees
//  */
// void GPS::dms_to_dec(void)
// // from http://arduinodev.woofex.net/2013/02/06/adafruit_gps_forma/
// {
//   float min_long = this->location.long();
//   float min_lat = this->location.lat();
//   double minlo = 0.0;
//   double minla = 0.0;

//   //get the minutes, fmod() requires double
//   minlo = fmod((double)min_long, 100.0);
//   minla = fmod((double)min_lat, 100.0);

//   //rebuild coordinates in decimal degrees
//   min_long = (int)(min_long / 100);
//   this->longitude = min_long + (minlo / 60);

//   min_lat = (int)(min_lat / 100);
//   this->latitude = min_lat + (minla / 60);
// }