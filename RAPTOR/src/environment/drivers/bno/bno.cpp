/*
 * bno.cpp -
 * Class written for use with Adafruit BNO055 sensor.
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "bno.h"
#include <Adafruit_BNO055.h>

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