/*
  environment.h -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef ENVIRONMENT_h
#define ENVIRONMENT_h

#include <elapsedMillis.h>
#include "drivers/bmp/bmp.h"
#include "drivers/bno/bno.h"
#include "drivers/gps/gps.h"
#include "drivers/openlog/openlog.h"

class Environment
{
public:
  static Environment *getInst();

  bool init(bool set_baseline);
  bool update();

  float correct_alt(uint8_t flight_state);

  bool landing_check(void);
  bool cutdown_check(void);

  BMP *bmp;
  BNO *bno;
  GPS *gps;

private:
  Environment(); // constructor
};

#endif