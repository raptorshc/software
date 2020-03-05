/*
  raptor.cpp -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "raptor.h"
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/

#include "test/test.h"

#define PRINT_DELAY 100 // delay between print statements
#define FLY_DELAY 100   // delay between flight calls

#define MIN_DESCENT_ALT 20 // minimum descent altitude to prevent early transition
#define GROUND_ALT 10      // altitude to transition to landed state

#define BZZ_DTA 3 // buzzer

static long print_time = 0; // last time we printed data
static long fly_time = 0;   // amount of time passed between flight controlling

Raptor::Raptor()
{
    this->environment = Environment::getInst();
    this->pilot = Pilot::getInst();
    this->logger = Logger::getInst();
}

void Raptor::init(void)
{
    Serial.begin(115200);
    this->environment->time_elapsed = 0;

    /* Buzzer and LEDs */
    pinMode(BZZ_DTA, OUTPUT); // Set buzzer to output

    startup_sequence();

    this->logger->write("ELAPSED TIME,REAL TIME,GPS LAT,GPS LONG,GPS HEADING,AGL,"
                        "ABSOLUTE ALT,SPEED,SATS,ORIENTATION X,ORIENTATION Y,"
                        "ORIENTATION Z,LINEAR ACCEL X,LINEAR ACCEL Y,LINEAR ACCEL Z,PILOT TURN,"
                        "PILOT SERVO,FLIGHT STATE,INIT ALT,INIT LAT,INIT LONG\n"); // data header
}

void Raptor::ascent()
{
    environment->update();
    if ((environment->gps->agl > MIN_DESCENT_ALT) && this->environment->bno->goingDown())
    { // at the minimum altitude and bno detects we're going down transition to FS1 [DESCENT]
        this->flight_state = 2;
        this->logger->write("\n!!!! DESCENT !!!!\n");
    }

    if (this->environment->time_elapsed - print_time > PRINT_DELAY)
    {
        print_time = this->environment->time_elapsed;
        if (this->environment->gps->gps_num == 0)
        { // beep at a slower rate when we have yet to acquire gps fix
            beep(400);
        }
        else
        { // only log data once we've gotten a gps packet
            print_data();
            beep(100);
        }
    }
}

void Raptor::descent()
{
    environment->update();

    if (environment->gps->agl < GROUND_ALT)
    { // at ground alt, transition to FS2 [LANDED]
        pilot->sleep();
        this->flight_state = 3;
        this->logger->write("\n!!!! LANDED !!!!\n");
    }

    if (this->environment->time_elapsed - fly_time > FLY_DELAY)
    { // don't want to constantly call fly
        pilot->fly(environment->gps->course.deg(),
                   environment->gps->courseTo(
                       environment->gps->location.lat(), environment->gps->location.lng(),
                       environment->gps->init_lat, environment->gps->init_long)); // the pilot just needs our current angle to do his calculations
        fly_time = this->environment->time_elapsed;
    }

    if (this->environment->time_elapsed - print_time > PRINT_DELAY + 50)
    {
        print_time = this->environment->time_elapsed;
        print_data();
    }
}

void Raptor::landed()
{
    static int bzz_status = 0;

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
            bzz_status = 200;
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
                  "ALT (agl, absolute): %.2f,%.2f\n"
                  "SPEED (mph): %.2f\tSATELLITES: %lu\n"
                  "ORIENTATION (x, y, z): %.4f,%.4f,%.4f\n"
                  "LINEAR ACCEL (x, y, z): %.4f,%.4f,%.4f\n"
                  "PILOT (turn, servo, fs): %d, %d, %d\n"
                  "INIT (alt, lat, long): %f, %lf, %lf\n",
            (unsigned long)(this->environment->time_elapsed), environment->gps->time.value(),
            environment->gps->location.lat(), environment->gps->location.lng(), environment->gps->course.deg(),
            environment->gps->agl, environment->gps->altitude.meters(),
            environment->gps->speed.mph(), environment->gps->satellites.value(),
            environment->bno->data.orientation.x,
            environment->bno->data.orientation.y, environment->bno->data.orientation.z,
            environment->bno->accelX(), environment->bno->accelY(), environment->bno->accelZ(),
            this->pilot->get_turn(), this->pilot->servo_status(), this->flight_state,
            this->environment->gps->init_alt, this->environment->gps->init_lat, this->environment->gps->init_long);

    Serial << data << "\n";

    sprintf(data, "%lu,%lu,"
                  "%.6f,%.6f,%.2lf,"
                  "%.2f,%.2f,"
                  "%.2f,%lu,"
                  "%.4f,%.4f,%.4f,"
                  "%.4f,%.4f,%.4f,"
                  "%d,%d,%d"
                  "%f, %lf, %lf\n",
            (unsigned long)(this->environment->time_elapsed), environment->gps->time.value(),
            environment->gps->location.lat(), environment->gps->location.lng(), environment->gps->course.deg(),
            environment->gps->agl, environment->gps->altitude.meters(),
            environment->gps->speed.mph(), environment->gps->satellites.value(),
            environment->bno->data.orientation.x,
            environment->bno->data.orientation.y, environment->bno->data.orientation.z,
            environment->bno->accelX(), environment->bno->accelY(), environment->bno->accelZ(),
            this->pilot->get_turn(), this->pilot->servo_status(), this->flight_state,
            this->environment->gps->init_alt, this->environment->gps->init_lat, this->environment->gps->init_long);

    if (this->logger->write(data) == false)
    { // if we failed to write data, beep slowly
        beep(500);
    }
}

/*
 *  startup_sequence initializes our solenoids, servos, and sensors.
 *   If in flight state 0 (launch), performs a sequence that indicates board power,
 *   solenoid power, servo power, and successful sensor initialization.
 */
void Raptor::startup_sequence(void)
{
    // initialize sensors, then indicate if we were successful or not
    if (this->environment->init() && this->logger->init())
    { // if the initialization was successful test the servos
        pilot->servo_test();
        delay(200);
    }
    else
    { // if the initialization was unsuccessful beep 15 times slowly
        for (int i = 0; i < 15; i++)
        {
            beep(1000);
        }
    }
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