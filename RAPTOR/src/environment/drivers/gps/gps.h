/*
 * gps.h -
 * Class written for use with Adafruit Ultimate GPS.
 * Utilizes Adafruit Ultimate GPS library.
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef GPS_H_
#define GPS_H_

#include <Adafruit_GPS.h>

class GPS
    : public Adafruit_GPS
{
public:
  static GPS &getInst(SoftwareSerial &s);
  void init(void);

  void update(void);

  float init_alt, agl; // initial altitude and above ground level altitude
  bool first_gps;

private:
  GPS(SoftwareSerial &s) : Adafruit_GPS(&s)
  {
    this->first_gps = true;
  }
  void dms_to_dec(void);
};

#endif