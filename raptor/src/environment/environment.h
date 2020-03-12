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
#include "drivers/bmp/bmp.h"

#define BMP_PRESENT

class Environment
{
public:
  static Environment *getInst();

  bool init();
  void update();

  void printable_data(char *data);
  void loggable_data(char *data);

  float correct_alt(uint8_t flight_state);
  bool cutdown_check(void);

  BNO *bno;
  GPS *gps;
#ifdef BMP_PRESENT
  BMP *bmp;
#endif

  elapsedMillis time_elapsed;

private:
  Environment(); // constructor
};

#endif