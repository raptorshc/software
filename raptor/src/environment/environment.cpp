/*
  environment.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "environment.h"

#define BNO_DELAY 10
#define GPS_DELAY 100

static long bno_time = 0;
static long gps_time = 0;

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
bool Environment::init()
{
    this->gps->init();
    return this->bno->init();
}

/*
 *  updates/queries all sensors, returns false if any fail
 */
bool Environment::update()
{
    if (time_elapsed - gps_time > GPS_DELAY)
    {
        this->gps->update();
    }

    if (time_elapsed - bno_time > BNO_DELAY)
    {
        return this->bno->update();
    }
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
    SoftwareSerial *gps_serial = new SoftwareSerial(0, 1); // GPS serial comm pins
    //this->gps = new GPS(*gps_serial);
    this->gps = new GPS(gps_serial);
}