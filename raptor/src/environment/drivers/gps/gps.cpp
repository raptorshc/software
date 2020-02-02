/*
 * gps.cpp -
 * Contains implementation for functions in gps.h.
 * Utilizes Adafruit Ultimate GPS library, details: https://learn.adafruit.com/adafruit-ultimate-gps
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "gps.h"
#include "math.h"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F\r\n"

/*
 * init begins the GPS readings, sets up the timer counter used for the
 *  millisecond interrupt, which will query and parse the GPS data
 */
void GPS::init(void)
{
  // ****TODO**** This needs to be changed or corrected when we try this again
  Serial1.begin(9600);
  Serial1.println("$PMTK251,115200*1F");
  Serial1.end();

  Serial1.begin(115200);
  Serial1.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  Serial1.println(PMTK_SET_NMEA_UPDATE_10HZ);
}

/*
 * update performs all needed actions for parsing
 *  a new set of data collected by the GPS
 */
void GPS::update(void)
{
  while (Serial1.available() > 0)
  {
    // Serial.print(Serial1.read());
    this->encode(Serial1.read());
  }

  if (this->altitude.isUpdated())
  { // if we have a new altitude, update agl
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