/*
  pilot.h -
		DESCRIPTION NEEDED.
		Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef PILOT_h
#define PILOT_h

#include "pathfinder.h"
#include <Servo.h>

class Pilot
{
public:
	static Pilot *getInst();

	void wake(Coordinate current, Coordinate target);
	void fly(float curr_angle);

	/* Mutators */
	void servo_test(void);
	void sleep(void);

	/* Accessors */
	int servo_status(void);
	int get_turn(void);

private:
	Pilot();
	float desired_heading;

	Servo *servo;
	Pathfinder *p;

	int find_turn(float curr_angle);
	void turn(bool direction);

	enum turn_state
	{
		right = 1,
		left = 0,
		straight = 2
	};

	turn_state current_turn; // uses ContinuousServo settings for 0 (left) and 1 (right), 2 is straight
	void turn(turn_state direction);

	static const int SRVO_STRAIGHT = 1500,
					 SRVO_RIGHT = 1000,
					 SRVO_LEFT = 2000;
};

#endif