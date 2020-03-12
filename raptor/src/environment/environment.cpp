/*
  environment.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "environment.h"

#define BNO_DELAY 10
#define GPS_DELAY 10

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
#ifdef BMP_PRESENT
    return this->bno->init() && this->bmp->init();
#else
    return this->bno->init();
#endif
}

/*
 *  updates/queries all sensors, returns false if any fail
 */
void Environment::update()
{
    if (time_elapsed - gps_time > GPS_DELAY)
    {
        this->gps->update();
        gps_time = time_elapsed;
    }

    if (time_elapsed - bno_time > BNO_DELAY)
    {
        bno_time = time_elapsed;
        this->bno->update();
    }
}

void Environment::printable_data(char *data)
{
    char str[1024];
    sprintf(data, "----------\nTIME (elapsed, real): %lu,%lu\n"
                  "GPS (lat, long, heading): %.6f,%.6f,%.2lf\n"
                  "ALT (agl, absolute): %.2f,%.2f\n"
                  "SPEED (mph): %.2f\tSATELLITES: %lu\n"
                  "ORIENTATION (x, y, z): %.4f,%.4f,%.4f\n"
                  "LINEAR ACCEL (x, y, z): %.4f,%.4f,%.4f\n"
#ifdef BMP_PRESENT
                  "BMP (pres, alt): %.4f, %.4f\n"
#endif
                  "INIT (alt, lat, long): %f, %lf, %lf\n",
            (unsigned long)(this->time_elapsed),
            this->gps->time.value(),
            this->gps->location.lat(), this->gps->location.lng(), this->gps->course.deg(),
            this->gps->agl, this->gps->altitude.meters(),
            this->gps->speed.mph(), this->gps->satellites.value(),
            this->bno->data.orientation.x,
            this->bno->data.orientation.y, this->bno->data.orientation.z,
            this->bno->accelX(), this->bno->accelY(), this->bno->accelZ(),
#ifdef BMP_PRESENT
            this->bmp->getPressure(), this->bmp->getAltitude(),
#endif
            this->gps->init_alt, this->gps->init_lat, this->gps->init_long);

    strncpy(str, data, sizeof(str) / sizeof(str[0]));
}

void Environment::loggable_data(char *data)
{
    char str[1024];
    sprintf(data, "%lu,%lu,"
                  "%.6f,%.6f,%.2lf,"
                  "%.2f,%.2f,"
                  "%.2f,%lu,"
                  "%.4f,%.4f,%.4f,"
                  "%.4f,%.4f,%.4f,"
#ifdef BMP_PRESENT
                  "%.4f,%.4f,"
#endif
                  "%f,%lf,%lf",
            (unsigned long)(time_elapsed),
            this->gps->time.value(),
            this->gps->location.lat(), this->gps->location.lng(), this->gps->course.deg(),
            this->gps->agl, this->gps->altitude.meters(),
            this->gps->speed.mph(), this->gps->satellites.value(),
            this->bno->data.orientation.x,
            this->bno->data.orientation.y, this->bno->data.orientation.z,
            this->bno->accelX(), this->bno->accelY(), this->bno->accelZ(),
#ifdef BMP_PRESENT
            bmp->getPressure(), bmp->getAltitude(),
#endif
            gps->init_alt, gps->init_lat, gps->init_long);

    strncpy(str, data, sizeof(str) / sizeof(str[0]));
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

#ifdef BMP_PRESENT
    /* BMP */
    this->bmp = BMP::getInst();
#endif
}