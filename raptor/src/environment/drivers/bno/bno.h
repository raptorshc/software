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
  bool goingDown();
  float accelX();
  float accelY();
  float accelZ();

  sensors_event_t data;

private:
  BNO() : Adafruit_BNO055(55) {}
  const float MIN_ACCEL = 1; // The min acceleration before we assume we are now going down
  // Also need to know what direction that is going to be huh?
  float accel_x[49];
  float accel_y[49];
  float accel_z[49];
};

#endif