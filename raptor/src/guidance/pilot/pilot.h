/*
  pilot.h -
		DESCRIPTION NEEDED.
		Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef PILOT_h
#define PILOT_h

#include <Servo.h>

class Pilot
{
public:
	static Pilot *getInst();

	void fly(float curr_angle, float desired_heading);

	/* Mutators */
	void servo_test(void);
	void sleep(void);

	/* Accessors */
	int servo_status(void);
	int get_turn(void);

private:
	Pilot();

	Servo *servo;

	enum turn_state
	{
		right = 1,
		left = 0,
		straight = 2
	};

	turn_state find_turn(float curr_angle, float desired_heading);
	void turn(bool direction);

	turn_state current_turn; // uses ContinuousServo settings for 0 (left) and 1 (right), 2 is straight
	void turn(turn_state direction);

	static const int SRVO_STRAIGHT = 90,
					 SRVO_RIGHT = 20,
					 SRVO_LEFT = 170;
};

#endif