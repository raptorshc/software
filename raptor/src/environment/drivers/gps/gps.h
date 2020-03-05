/*
 * gps.h -
 * Class written for use with Adafruit Ultimate GPS.
 * Utilizes Adafruit Ultimate GPS library.
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef GPS_H_
#define GPS_H_

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

class GPS
    : public TinyGPSPlus
{
public:
  GPS() : TinyGPSPlus()
  {
    this->gps_num = 0;
    this->init_alt = 0;
    this->init_lat = 0;
    this->init_long = 0;
    this->agl = 0;
  }
  void init(void);
  void update(void);

  float init_alt, agl; // initial altitude and above ground level altitude
  double init_lat, init_long;
  long unsigned int gps_num;

private:
  void dms_to_dec(void);
};

#endif