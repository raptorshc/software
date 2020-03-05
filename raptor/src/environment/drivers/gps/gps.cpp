/*
 * gps.cpp -
 * Contains implementation for functions in gps.h.
 * Utilizes Adafruit Ultimate GPS library, details: https://learn.adafruit.com/adafruit-ultimate-gps
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "gps.h"
#include "math.h"
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F\r\n"

/*
 * init begins the GPS readings, sets up the timer counter used for the
 *  millisecond interrupt, which will query and parse the GPS data
 */
void GPS::init(void)
{
  Serial1.begin(9600);

  Serial1.println("$PMTK251,115200*1F");
  Serial1.end();
  delay(10);
  Serial1.begin(115200);

  delay(10);
  Serial1.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(10);
  Serial1.println(PMTK_SET_NMEA_UPDATE_10HZ);
}

/*
 * update performs all needed actions for parsing
 *  a new set of data collected by the GPS
 */
void GPS::update(void)
{
  char c;

  while (Serial1.available() > 0)
  {
    c = Serial1.read();
    // Serial.print(c);
    this->encode(c);
  }

  if (this->altitude.isUpdated())
  { // passes only when we've gotten an altitude update from the last call
    static float alts[10];
    if (this->gps_num < 10)
    { // collect and average the first 10 altitude readings for a ground level alt
      alts[this->gps_num] = this->altitude.meters();
      if (this->gps_num == 9)
      { // on tenth reading collect launch location
        this->init_lat = this->location.lng();
        this->init_long = this->location.lat();
      }
    }
    else
    {
      if (this->init_alt != 0)
      { // average the first 10 readings to get a (hopefully) more accurate ground level altitude
        int temp_sum = 0;
        for (int i = 0; i < 10; i++)
        {
          temp_sum += alts[i];
        }
        this->init_alt = temp_sum / 10;
      }

      // calculate AGL
      this->agl = this->altitude.meters() - this->init_alt;
    }
    this->gps_num++;
  }
}