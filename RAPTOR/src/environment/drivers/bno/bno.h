/*
 * bno.h -
 * Class written for use with Adafruit BNO055 sensor.
 * Utilizes Adafruit BNO055 library.
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef BNO_H_
#define BNO_H_

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

class BNO : public Adafruit_BNO055
{
public:
  static BNO *getInst();
  bool init(void);
  bool update(void);

  sensors_event_t data;

private:
  BNO() : Adafruit_BNO055(55) {}
};

#endif