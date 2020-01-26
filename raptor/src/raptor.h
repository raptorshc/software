/*
  raptor.h -
		DESCRIPTION NEEDED.
		Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef RAPTOR_h
#define RAPTOR_h

#include "guidance/pilot/pilot.h"
#include "environment/environment.h"

class Raptor
{
public:
  Raptor();

  void init();
  void launch();
  void ascent();
  void descent();
  void landed();

  Environment *environment;
  uint8_t flight_state = 0; // current flight state

private:
  void startup_sequence();

  void print_data();
  void beep(int length);

  Pilot *pilot;
  Logger *logger;

  bool didwake = false; // whether or not we have woken the pilot yet

  const float GROUND_ALT = 15.24;  // altitude to transition to FS1 [ASCENT] or FS3 [LANDED], =50ft
  const float CUTDOWN_ALT = 609.6; // altitude to transition to FS2 [DESCENT], =1000ft

  const float TARGET_LONG = -86.633730;
  const float TARGET_LAT = 34.722988; // HARD CODED TARGET COORDINATES

  const uint8_t BZZ_DTA = 3; // buzzer
};
#endif