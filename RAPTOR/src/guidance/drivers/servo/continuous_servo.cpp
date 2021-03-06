/*
  continuous_servo.cpp -
  Class written for use with continuous rotation servos.
  Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include <Arduino.h>
#include "continuous_servo.h"
#include <math.h>

/* Public Methods */
/*
 *	turn will rotate the servos to a certain configuration, based on the set time to rotate (TTR),
 * 		if reset is given as true, the servo will rotate in the opposite direction for the same amount of time.
 */
void ContinuousServo::turn(bool reset /*= false*/)
{
	this->attach(this->pin);
	this->writeMicroseconds(STOP); // Stop the servos just in-case they're running already.
	delay(10);
	if (servo == RIGHT)
	{
		if (reset)
			this->writeMicroseconds(CW);
		else
			this->writeMicroseconds(CCW); // based on the turn, we will rotate CW or CCW
	}
	else
	{ // left servo
		if (reset){
			this->writeMicroseconds(CCW); // reset will turn the servo the other way for the same amount of time
		}
		else{
			this->writeMicroseconds(CW);
		}
	}
	delay(TTR);

	this->writeMicroseconds(STOP); // Stop the servos once we've reached the deflection setting.
	delay(100);
	this->detach();
}

/*
 *	reset will reset the servos to the default position after a turn.
 */
void ContinuousServo::reset()
{
	turn(true);
}