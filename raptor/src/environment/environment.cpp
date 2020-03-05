/*
  environment.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "environment.h"

#define BNO_DELAY 10
#define GPS_DELAY 10
#ifdef BMP_PRESENT
#define BMP_DELAY 10
#endif

static long bno_time = 0;
static long gps_time = 0;
#ifdef BMP_PRESENT
static long bmp_time = 0;
#endif

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

#ifndef BMP_PRESENT
    return this->bno->init();
#endif
#else
    return this->bno->init() && this->bmp->init();
#endif
}

/*
 *  updates/queries all sensors, returns false if any fail
 */
bool Environment::update()
{
    if (time_elapsed - gps_time > GPS_DELAY)
    {
        this->gps->update();
        gps_time = time_elapsed;
    }

    if (time_elapsed - bno_time > BNO_DELAY)
    {
        bno_time = time_elapsed;
        if (!this->bno->update())
        {
            return false;
        }
    }

#ifdef BMP_PRESENT
    if (time_elapse - bmp_time > BMP_DELAY)
    {
        bmp_time = time_elapsed;
        if (!this->bmp->update())
        {
            return false;
        }
    }
#endif
    return true;
}

/*
 *  Constructor for Environment
 */
Environment::Environment()
{
    /* BNO */
    this->bno = BNO::getInst();

    /* GPS */
    this->gps = new GPS();

/* BMP */
#ifdef BMP_PRESENT
    this->bmp = BMP::getInst();
#endif
}