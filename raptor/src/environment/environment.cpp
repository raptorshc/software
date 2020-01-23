/*
  environment.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "environment.h"

/* PUBLIC METHODS */

/*
 *
 */
Environment *Environment::getInst()
{
    static Environment *inst = NULL;
    if (!inst)
    {
        inst = new Environment;
    }
    return inst;
}

/*
 * initializes all sensors, returns false if any initializations fail return false
 */
bool Environment::init(bool set_baseline)
{
    this->gps->init();
    return this->bno->init();
}

/*
 *  updates/queries all sensors, returns false if any fail
 */
bool Environment::update()
{
    this->gps->update();
    if (this->bno->update())
        return true;
    else
        return false;
}

/*
 *  Constructor for Environment
 */
Environment::Environment()
{
    /* BNO */
    this->bno = BNO::getInst();

    /* GPS */
    SoftwareSerial *gps_serial = new SoftwareSerial(3, 2); // GPS serial comm pins
    //this->gps = new GPS(*gps_serial);
    this->gps = &GPS::getInst(*gps_serial);
}