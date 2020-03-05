/*
 * bno.cpp -
 * Class written for use with Adafruit BNO055 sensor.
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "bno.h"
#include <Adafruit_BNO055.h>
#include <Streaming.h>

/*
 *
 */
BNO *BNO::getInst()
{
    static BNO *inst = NULL;
    if (!inst)
    {
        inst = new BNO;
    }
    return inst;
}

/*
 *	init begins the BNO measurements
 */
bool BNO::init(void)
{
    if (!this->begin())
    {
        Serial.print(F("No BNO055 detected!\n"));
        return false;
    }
    //calibrate here
    return true;
}

/*
 *	update reads data from the sensor
 */
bool BNO::update()
{
    return this->getEvent(&this->data);
}

/*
* Uses linear acceleration to see if the rocket is descending
*/
bool BNO::goingDown()
{
    imu::Vector<3> accel = this->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

    // For debugging
    // Serial << "X: " << accel.x() << "Y: " << accel.y() << "Z: " << accel.z() << endl;

    // Need to figure out how this is    going to go in the rocket as well as the value
    // It may just be zero and depending on the orientation it is positive or negative

    if (accel.x() < 0.5 && accel.x() > -0.5)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
* Returns the raw linear acceleration values 
*/
float BNO::accelX()
{
    return this->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL).x();
}

float BNO::accelY()
{
    return this->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL).y();
}

float BNO::accelZ()
{
    return this->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL).z();
}