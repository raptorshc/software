/*
 * bmp.h -
 * Class written for use with Adafruit BMP180 sensor.
 * Utilizes Adafruit BMP library
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef BMP_H_
#define BMP_H_

#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

class BMP
    : public Adafruit_BMP280
{
public:
  BMP() : Adafruit_BMP280() {}
  bool init(bool set_baseline);
  float getAltitude(void);

  float baseline;
};

#endif /* BMP_H_ */