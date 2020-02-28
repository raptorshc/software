/*
  pilot.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "pilot.h"
#include <stdio.h>
#include <Arduino.h>

#define SRVO_DTA 5 // servo pin

/* PUBLIC METHODS */
/*
 *
 */
Pilot *Pilot::getInst()
{
    static Pilot *inst = NULL;
    if (!inst)
    {
        inst = new Pilot;
    }
    return inst;
}

/*
 *  fly will ..
 */
void Pilot::fly(float curr_angle, float desired_heading)
{
    turn_state target_turn = find_turn(curr_angle, desired_heading);
    if (target_turn != current_turn)
    {
        turn(target_turn);
    }
}

/*
 * servo_status acts as a public accessor for the read method of servo
 */
int Pilot::servo_status(void)
{
    return servo->readMicroseconds();
}

/*
 * get_turn returns the current turn of the pilot
 */
int Pilot::get_turn(void)
{
    return this->current_turn;
}

/*
 * servo_test turns and resets both servos to indicate servo power and attachment
 */
void Pilot::servo_test(void)
{
    Serial.println("Beginning servo test");
    turn(turn_state::left);
    delay(500);
    turn(turn_state::straight);

    delay(1000);

    turn(turn_state::right);
    delay(500);
    turn(turn_state::straight);
}

/*
 * sleep shuts down the pilot and sets the payload straight
 */
void Pilot::sleep(void)
{
    turn(turn_state::straight);
}

/* PRIVATE METHODS */
/*
 *  Constructor for Pilot
 */
Pilot::Pilot()
{
    this->current_turn = turn_state::straight;
    servo = new Servo();
    servo->attach(SRVO_DTA);
}

/*
 * find_turn takes the current angle and finds the correct
 * turn based on basic heading math
 */
Pilot::turn_state Pilot::find_turn(float curr_angle, float desired_heading)
{
    float alpha_angle, beta_angle;

    if (abs(abs(curr_angle - desired_heading) - 360) < 15 || abs(desired_heading - curr_angle) < 15)
        return turn_state::straight; // if the difference between our current and desired heading is less than 15,  continue straight

    alpha_angle = desired_heading + 90; // alpha angle is in the quadrant to the left of our target angle
    beta_angle = desired_heading + 270; // beta angle is in the quadrant to the right

    if (alpha_angle > 360)
        alpha_angle -= 360;
    if (beta_angle > 360)
        beta_angle -= 360;

    /* Determine if alpha or beta angle is closer to our current angle, adjust based on that. */
    float alpha_dif = abs(alpha_angle - curr_angle);
    float beta_dif = abs(beta_angle - curr_angle);

    /* As we have created a straight line across the graph, neither difference should be greater than 180 degrees */
    if (alpha_dif > 180)
        alpha_dif = 360 - alpha_dif;
    if (beta_dif > 180)
        beta_dif = 360 - beta_dif;

    /* If we are closer to the left, turn right, and vice versa */
    if (alpha_dif > beta_dif)
        return turn_state::right;
    else
        return turn_state::left;
}

void Pilot::turn(turn_state direction)
{
    if (direction == this->current_turn)
    {
        return;
    }
    if (direction == turn_state::straight)
    {
        servo->write(this->SRVO_STRAIGHT); // straighten out the servos
        this->current_turn = turn_state::straight;
        Serial.print("Turn: straight\n");
    }
    else if (direction == turn_state::right)
    {
        servo->write(this->SRVO_RIGHT);
        this->current_turn = turn_state::right;
        Serial.print("Turn: right\n");
    }
    else
    { // left turn
        servo->write(this->SRVO_LEFT);
        this->current_turn = turn_state::left;
        Serial.print("Turn: left\n");
    }
}