/*
  environment.h -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef ENVIRONMENT_h
#define ENVIRONMENT_h

#include <elapsedMillis.h>
#include "drivers/bno/bno.h"
#include "drivers/gps/gps.h"
#include "drivers/logger/logger.h"

class Environment
{
public:
  static Environment *getInst();

  bool init();
  bool update();

  float correct_alt(uint8_t flight_state);

  bool cutdown_check(void);

  BNO *bno;
  GPS *gps;

  elapsedMillis time_elapsed;

private:
  Environment(); // constructor
};

#endif