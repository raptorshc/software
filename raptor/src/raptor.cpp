/*
  raptor.cpp -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "raptor.h"
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/

#include "test/test.h"

#define PRINT_DELAY 100
#define FLY_DELAY 500

static long print_time = 0; // last time we printed data
static long fly_time = 0;   // amount of time passed between flight controlling

/*
 * Arduino setup function, first function to be run.
 */
Raptor::Raptor()
{
    this->environment = Environment::getInst();
    pilot = Pilot::getInst();
    logger = Logger::getInst();
}

void Raptor::init(void)
{
    Serial.begin(115200);
    this->environment->time_elapsed = 0;

    Serial << "begin init!\n";

    /* Buzzer and LEDs */
    pinMode(BZZ_DTA, OUTPUT); // Set buzzer to output

    startup_sequence();

    delay(10);
    Serial.print(F("TIME,"
                   "LATITUDE,LONGITUDE,ANGLE,GPS_ALT,"
                   "X,Y,Z,Time\n")); // data header
}

void Raptor::launch()
{
    if (environment->gps->altitude.meters() > GROUND_ALT)
    { // at 50ft (15.24 meters), transition to FS1 [ASCENT]
        this->flight_state = 1;
    }

    environment->update();
    if (this->environment->time_elapsed - print_time > PRINT_DELAY + 900)
    {
        print_time = this->environment->time_elapsed;
        print_data();
    }
}

void Raptor::ascent()
{
    if (environment->gps->altitude.meters() > CUTDOWN_ALT)
    { // at the cutdown altitude perform cutdown, deploy, and transition to FS2 [DESCENT]
        this->flight_state = 2;
    }

    environment->update();
    if (this->environment->time_elapsed - print_time > PRINT_DELAY)
    {
        print_time = this->environment->time_elapsed;
        print_data();
    }
}

void Raptor::descent()
{
    fly_time = this->environment->time_elapsed;
    if (fly_time > FLY_DELAY)
    { // don't want to constantly call fly
        pilot->fly(environment->gps->course.deg(),
                   environment->gps->courseTo(
                       environment->gps->location.lat(), environment->gps->location.lng(),
                       TARGET_LAT, TARGET_LONG)); // the pilot just needs our current angle to do his calculations
        fly_time = 0;
    }

    // if (environment->gps->altitude.meters() < GROUND_ALT)
    // { // at 50ft (15.24 meters), transition to FS3 [LANDED]
    //     pilot->sleep();
    //     this->flight_state = 3;
    //     Serial << "\n!!!! LANDED !!!!\n";
    // }

    environment->update();
    if (this->environment->time_elapsed - print_time > PRINT_DELAY)
    {
        print_time = this->environment->time_elapsed;
        print_data();
    }
}

void Raptor::landed()
{
    // in the landed state, and buzzer every 2 seconds, then print data
    analogWrite(BZZ_DTA, 0);
    delay(200);
    analogWrite(BZZ_DTA, 200);

    if (this->environment->time_elapsed - print_time > PRINT_DELAY + 900)
    {
        print_time = this->environment->time_elapsed;
        print_data();
    }
}

/*
 * print_data updates sensor readings then prints all relevant data to the serial pins.
 */
void Raptor::print_data()
{
    /* Let's spray the serial port with a hose of data */
    char data[256];
    sprintf(data, "----------\nTIME (elapsed, real): %lu,%lu\n"
                  "GPS (lat, long, heading, alt): %f,%f,%f,%f\n"
                  "ORIENTATION (x, y, z): %f,%f,%f\n"
                  "LINEAR ACCEL (x, y, z): %f,%f,%f\n",
            (unsigned long)(this->environment->time_elapsed), environment->gps->time.value(),
            environment->gps->location.lat(), environment->gps->location.lng(), environment->gps->course.deg(), environment->gps->altitude.meters(),
            environment->bno->data.orientation.x, environment->bno->data.orientation.y, environment->bno->data.orientation.z,
            environment->bno->accelX(), environment->bno->accelY(), environment->bno->accelZ());

    Serial << data << "\n";
    logger->write(data);
}

/*
 *  startup_sequence initializes our solenoids, servos, and sensors.
 *   If in flight state 0 (launch), performs a sequence that indicates board power,
 *   solenoid power, servo power, and successful sensor initialization.
 */
void Raptor::startup_sequence(void)
{
    // initialize sensors, then indicate if we were successful or not
    if (environment->init())
    { // if the initialization was successful and we're in flight state 0 blink 5 times
        pilot->servo_test();
        delay(200);
    }
    else
    { // if the initialization was unsuccessful and we're in flight state 1 beep & blink 15 times
        for (int i = 0; i < 15; i++)
        {
            beep(500);
        }
    }
    this->logger->init();
}

/*
 * beep turns on the buzzer, delays for length, then turns off the buzzer.
 */
void Raptor::beep(int length)
{
    analogWrite(BZZ_DTA, 200);
    delay(length);
    analogWrite(BZZ_DTA, 0);
}