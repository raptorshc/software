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

};
#endif