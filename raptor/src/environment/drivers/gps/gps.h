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
  GPS(SoftwareSerial *s) : TinyGPSPlus()
  {
    this->s = s;
    this->first_gps = true;
  }
  void init(void);

  void update(void);

  float init_alt, agl; // initial altitude and above ground level altitude
  bool first_gps;

private:
  SoftwareSerial *s;
  void dms_to_dec(void);
};

#endif