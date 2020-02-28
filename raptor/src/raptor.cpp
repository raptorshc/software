/*
  raptor.cpp -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "raptor.h"
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/

#include "test/test.h"

#define PRINT_DELAY 500 // [TODO: switch to 120 for flight
#define FLY_DELAY 500

#define MIN_DESCENT_ALT 10 // minimum descent altitude to prevent early transition
#define GROUND_ALT 10      // altitude to transition to landed state

#define TARGET_LONG -86.659917
#define TARGET_LAT 36.241586 // HARD CODED TARGET COORDINATES

#define BZZ_DTA 3 // buzzer

static long print_time = 0; // last time we printed data
static long fly_time = 0;   // amount of time passed between flight controlling

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

    /* Buzzer and LEDs */
    pinMode(BZZ_DTA, OUTPUT); // Set buzzer to output

    startup_sequence();

    logger->write("ELAPSED TIME,REAL TIME,GPS LAT,GPS LONG,GPS HEADING,AGL,"
                  "ABSOLUTE ALT,ALT AGE,SPEED,SATS,ORIENTATION X,ORIENTATION Y,"
                  "ORIENTATION Z,LINEAR ACCEL X,LINEAR ACCEL Y,LINEAR ACCEL Z,PILOT TURN,"
                  "PILOT SERVO,INIT ALT,INIT LAT,INIT LONG\n"); // data header
}

void Raptor::ascent()
{
    if ((environment->gps->agl > MIN_DESCENT_ALT) && this->environment->bno->goingDown())
    { // at the minimum altitude and bno detects we're going down transition to FS1 [DESCENT]
        this->flight_state = 2;
        Serial << "\n!!!! DESCENT !!!!\n";
    }

    environment->update();
    if (this->environment->time_elapsed - print_time > PRINT_DELAY)
    {
        if (this->environment->gps->agl < MIN_DESCENT_ALT)
        {
            if (this->environment->gps->first_gps)
            {
                beep(400);
            }
            else
            {
                beep(100);
            }
        }

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
                       environment->gps->init_lat, environment->gps->init_long)); // the pilot just needs our current angle to do his calculations
        fly_time = 0;
    }

    if (environment->gps->agl < GROUND_ALT)
    { // at 50ft (15.24 meters), transition to FS3 [LANDED]
        pilot->sleep();
        this->flight_state = 3;
        Serial << "\n!!!! LANDED !!!!\n";
    }

    environment->update();
    if (this->environment->time_elapsed - print_time > PRINT_DELAY)
    {
        print_time = this->environment->time_elapsed;
        print_data();
    }
}

void Raptor::landed()
{
    int bzz_status = 0;
    this->environment->update();
    if (this->environment->time_elapsed - print_time > PRINT_DELAY + 900)
    {
        print_time = this->environment->time_elapsed;
        print_data();

        // buzz
        if (bzz_status > 0)
        {
            analogWrite(BZZ_DTA, 0);
            bzz_status = 0;
        }
        else
        {
            bzz_status = 0;
            analogWrite(BZZ_DTA, 200);
        }
    }
}

/*
 * print_data updates sensor readings then prints all relevant data to the serial pins.
 */
void Raptor::print_data()
{
    /* Let's spray the serial port with a hose of data */
    char data[1024];
    sprintf(data, "----------\nTIME (elapsed, real): %lu,%lu\n"
                  "GPS (lat, long, heading): %.6f,%.6f,%.2lf\n"
                  "ALT (agl, absolute, age): %.2f,%.2f,%lu\n"
                  "SPEED (mph): %.2f\tSATELLITES: %lu\n"
                  "ORIENTATION (x, y, z): %.4f,%.4f,%.4f\n"
                  "LINEAR ACCEL (x, y, z): %.4f,%.4f,%.4f\n"
                  "PILOT (turn, servo): %d, %d\n"
                  "INIT (alt, lat, long): %f, %lf, %lf\n",
            (unsigned long)(this->environment->time_elapsed), environment->gps->time.value(),
            environment->gps->location.lat(), environment->gps->location.lng(), environment->gps->course.deg(),
            environment->gps->agl, environment->gps->altitude.meters(), environment->gps->altitude.age(),
            environment->gps->speed.mph(), environment->gps->satellites.value(),
            environment->bno->data.orientation.x,
            environment->bno->data.orientation.y, environment->bno->data.orientation.z,
            environment->bno->accelX(), environment->bno->accelY(), environment->bno->accelZ(),
            this->pilot->get_turn(), this->pilot->servo_status(),
            this->environment->gps->init_alt, this->environment->gps->init_lat, this->environment->gps->init_long);

    Serial << data << "\n";

    sprintf(data, "%lu,%lu,"
                  "%.6f,%.6f,%.2f,"
                  "%.2f,%.2f,%.2f,"
                  "%.2f,%lu,"
                  "%.4f,%.4f,%.4f,"
                  "%.4f,%.4f,%.4f,"
                  "%d,%d,"
                  "%f, %lf, %lf\n",
            (unsigned long)(this->environment->time_elapsed), environment->gps->time.value(),
            environment->gps->location.lat(), environment->gps->location.lng(), environment->gps->course.deg(),
            environment->gps->agl, environment->gps->altitude.meters(), environment->gps->altitude.age(),
            environment->gps->speed.mph(), environment->gps->satellites.value(),
            environment->bno->data.orientation.x,
            environment->bno->data.orientation.y, environment->bno->data.orientation.z,
            environment->bno->accelX(), environment->bno->accelY(), environment->bno->accelZ(),
            this->pilot->get_turn(), this->pilot->servo_status(),
            this->environment->gps->init_alt, this->environment->gps->init_lat, this->environment->gps->init_long);

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