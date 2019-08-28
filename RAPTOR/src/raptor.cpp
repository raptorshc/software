/*
  raptor.cpp -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "raptor.h"
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/

// #define RC_TEST
#include "guidance/drivers/servo/continuous_servo.h"

elapsedMillis time_elapsed;

/*
 * Arduino setup function, first function to be run.
 */
Raptor::Raptor()
{
    time_elapsed = 0;

    /* Buzzer and LEDs */
    pinMode(BZZ_DTA, OUTPUT);  // Set buzzer to output
    pinMode(LEDS_DTA, OUTPUT); // Set LEDs to output

    Serial.begin(115200);

    /* Solenoids, Servos, BMP, BNO */
    parafoil_sol = new Solenoid(9, A0, A2);
    cutdown_sol = new Solenoid(8, A1, A3);

    startup_sequence();

    /* GPS */
    environment = new Environment();
    // environment->init(true); // for testing pcb

    eeprom = new Prom();
    pinMode(SET_BTN, OUTPUT);
    if (digitalRead(SET_BTN))
    {
        eeprom->write_state(flight_state, environment->bmp->baseline);
    }

    if (!digitalRead(SET_BTN))
    {
        Serial << "Read EEPROM\n";
        eeprom->read_state(&flight_state, &environment->bmp->baseline);

        // print retrieved data
        Serial << "Saved flight state: " << flight_state;
        Serial << "\nSaved baseline: " << environment->bmp->baseline << "\n";
    }

    pilot = new Pilot();

    delay(10);
    Serial.print(F("TIME, TEMPERATURE, PRESSURE, ALTITUDE, LATITUDE, LONGITUDE, ANGLE, GPS_ALT, X, Y, Z, SWC, SWP, TURN, FLIGHT_STATE\n")); // data header
}

void Raptor::launch()
{
    if (environment->bmp->getAltitude() > GROUND_ALT)
    { // at 50ft (15.24 meters), transition to FS1 [ASCENT]
        flight_state = 1;
        eeprom->write_state(flight_state, environment->bmp->baseline);
    }

    // blink the LEDs and print data at a rate of 1Hz
    blink_led(1000);
    print_data();
}

void Raptor::ascent()
{
    if (environment->bmp->getAltitude() > CUTDOWN_ALT)
    { // at the cutdown altitude perform cutdown, deploy, and transition to FS2 [DESCENT]
        // CUTDOWN
        this->cutdown_sol->open();

        if (!this->cutdown_sol->read_switch())
        { // we want to make sure that we have cut down
            Serial << F("\n!!!! CUTDOWN ERROR DETECTED !!!!\n");
            this->cutdown_sol->open(); // try cutdown again, probably won't do much
        }

        // PARAFOIL DEPLOY
        while (environment->bmp->getAltitude() > CUTDOWN_ALT - 3.048)
        { // wait 3 meters to deploy the parafoil
            delay(1);
            print_data();
        }

        this->parafoil_sol->open();

        if (!this->parafoil_sol->read_switch())
        { // make sure the parafoil has deployed
            Serial << F("\n!!!! PARAFOIL DEPLOYMENT ERROR DETECTED !!!!\n");
            this->parafoil_sol->open(); // try deploying parafoil again, probably won't do much
        }

        delay(DEPLOY_DELAY); // wait for the parafoil to deploy/inflate before we begin guidance

        flight_state = 2;
        eeprom->write_state(flight_state, environment->bmp->baseline);
    }

    // blink the LEDs and print data at a rate of 5Hz
    blink_led(200);
    print_data();
}

void Raptor::descent()
{
    // if we have yet to wake the pilot, do so
    if (!didwake)
    {
        // first set up our coordinates
        Coordinate current, target;

        current.latitude = environment->gps->latitude;
        current.longitude = environment->gps->longitude;

        target.latitude = TARGET_LAT;
        target.longitude = TARGET_LONG;

        // then wake the pilot and give it the coordinates
        Serial << "Waking pilot\n";
        pilot->wake(current, target);
        didwake = true;
    }

    fly_time = time_elapsed;
    if (fly_time > FLY_DELAY)
    {                                        // don't want to constantly call fly
        pilot->fly(environment->gps->angle); // the pilot just needs our current angle to do his calculations
        fly_time = 0;
    }

    if (environment->bmp->getAltitude() < GROUND_ALT)
    { // at 50ft (15.24 meters), transition to FS3 [LANDED]
        if (environment->landing_check())
        { // make sure that we have landed by checking the altitude constantly
            pilot->sleep();
            flight_state = 3;
            Serial << "\n!!!! LANDED !!!!\n";
        }
    }

    // blink the LEDs and print data at 10Hz
    blink_led(100);
    print_data();
}

void Raptor::landed()
{
    // in the landed state, only toggle the LEDs and buzzer every 2 seconds, then print data
    analogWrite(BZZ_DTA, 200);
    blink_led(2000);
    analogWrite(BZZ_DTA, 0);

    delay(200);
    print_data();
}

/*
 * print_data updates sensor readings then prints all relevant data to the serial pins.
 */
void Raptor::print_data()
{
    environment->update();

    /* Let's spray the serial port with a hose of data */
    // time, temperature, pressure, altitude,
    Serial << time_elapsed << F(",") << environment->bmp->readTemperature() << F(",") << environment->bmp->readPressure()
           << F(",") << environment->bmp->getAltitude() << F(",");

    // // latitude, longitude, angle, (gps) altitude,
    Serial << _FLOAT(environment->gps->latitude, 7) << F(",") << _FLOAT(environment->gps->longitude, 7)
           << F(",") << _FLOAT(environment->gps->angle, 7) << F(",") << environment->gps->altitude << F(",");

    // // x orientation, y orientation, z orientation,
    Serial << _FLOAT(environment->bno->data.orientation.x, 4) << F(",") << _FLOAT(environment->bno->data.orientation.y, 4)
           << F(",") << _FLOAT(environment->bno->data.orientation.z, 4) << F(",");

    // // cutdown switch, parafoil switch, turn status, flight state
    Serial << this->cutdown_sol->read_switch() << F(",") << this->parafoil_sol->read_switch() << F(",")
           << pilot->get_turn() << F(",") << flight_state << "\n"; // write everything to SD card
}

/*
 *  startup_sequence initializes our solenoids, servos, and sensors.
 *   If in flight state 0 (launch), performs a sequence that indicates board power,
 *   solenoid power, servo power, and successful sensor initialization.
 */
void Raptor::startup_sequence(void)
{
    // indicate board power with a buzzer beep if in flight state 0
    if (flight_state == 0)
    {
        beep(500);
    }

    // initialize solenoids, should hear them click and see the indicator LEDs turn on
    parafoil_sol->close();
    parafoil_sol->read_switch();

    cutdown_sol->close();
    parafoil_sol->read_switch();

    // initialize servos, if we're in flight state 0 we'll test them as well
    pilot->servo_init();
    if (flight_state == 0)
    {
        pilot->servo_test(); // rotates and resets each servo
        delay(200);
    }

    // initialize sensors, then indicate if we were successful or not
    if (environment->init(flight_state))
    { // if the initialization was successful and we're in flight state 0 blink 5 times
        if (flight_state == 0)
        {
            for (int i = 0; i < 5; i++)
                blink_led(500);
        }
    }
    else
    { // if the initialization was unsuccessful and we're in flight state 1 beep & blink 15 times
        if (flight_state == 0)
        {
            for (int i = 0; i < 15; i++)
            {
                beep(500, true);
            }
        }
    }
}

/*
 * blink_led toggles the LED, then delays for a certain length of time.
 *  Can be used to achieve a blink rate, but will delay the entire execution.
 */
void Raptor::blink_led(int length)
{
    digitalWrite(LEDS_DTA, !digitalRead(LEDS_DTA));
    delay(length);
}

/*
 * beep turns on the buzzer, delays for length, then turns off the buzzer.
 *  Can optionally blink the led while the buzzer is on
 */
void Raptor::beep(int length, bool blink = false)
{
    analogWrite(BZZ_DTA, 200);
    if (blink)
        blink_led(length);
    else
        delay(length);
    analogWrite(BZZ_DTA, 0);
}