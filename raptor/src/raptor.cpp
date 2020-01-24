/*
  raptor.cpp -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "raptor.h"
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/
// #include "environment/drivers/logger/logger.h"

#include "test/test.h"

/*
 * Arduino setup function, first function to be run.
 */

static elapsedMillis time_elapsed;

Raptor::Raptor()
{
    this->environment = Environment::getInst();
    // this->eeprom = new Prom();
    pilot = Pilot::getInst();
    logger = Logger::getInst();
}

void Raptor::init(void)
{
    Serial.begin(115200);
    time_elapsed = 0;

    Serial << "begin init!\n";

    /* Buzzer and LEDs */
    // pinMode(BZZ_DTA, OUTPUT);  // Set buzzer to output
    // pinMode(LEDS_DTA, OUTPUT); // Set LEDs to output

    /* GPS */
    // environment->init(true);

    /* EEPROM */
    // pinMode(SET_BTN, OUTPUT);
    // if (digitalRead(SET_BTN))
    // {
    //     Serial << "Write EEPROM\n";
    //     eeprom->write_state(this->flight_state, environment->bmp->baseline);
    // }
    // else
    // {
    //     Serial << "Read EEPROM\n";
    //     eeprom->read_state(&this->flight_state, &environment->bmp->baseline);

    //     // print retrieved data
    //     Serial << "Saved flight state: " << this->flight_state;
    //     Serial << "\nSaved baseline: " << environment->bmp->baseline << "\n";
    // }

    startup_sequence();

    delay(10);
    Serial.print(F("TIME,"
                   "LATITUDE,LONGITUDE,ANGLE,GPS_ALT,"
                   "X,Y,Z\n")); // data header
}

void Raptor::launch()
{
    // if (environment->bmp->getAltitude() > GROUND_ALT)
    // { // at 50ft (15.24 meters), transition to FS1 [ASCENT]
    //     this->flight_state = 1;
    //     eeprom->write_state(this->flight_state, environment->bmp->baseline);
    // }

    // blink the LEDs and print data at a rate of 1Hz
    blink_led(1000);
    print_data();
}

// void Raptor::ascent()
// {
//     if (environment->bmp->getAltitude() > CUTDOWN_ALT)
//     { // at the cutdown altitude perform cutdown, deploy, and transition to FS2 [DESCENT]
//         // CUTDOWN
//         this->cutdown_sol->release();

//         if (!this->cutdown_sol->read_switch())
//         { // we want to make sure that we have cut down
//             Serial << F("\n!!!! CUTDOWN ERROR DETECTED !!!!\n");
//             this->cutdown_sol->release(); // try cutdown again, probably won't do much
//         }

//         // PARAFOIL DEPLOY
//         while (environment->bmp->getAltitude() > CUTDOWN_ALT - 3.048)
//         { // wait 3 meters to deploy the parafoil
//             delay(1);
//             print_data();
//         }

//         this->parafoil_sol->release();

//         if (!this->parafoil_sol->read_switch())
//         { // make sure the parafoil has deployed
//             Serial << F("\n!!!! PARAFOIL DEPLOYMENT ERROR DETECTED !!!!\n");
//             this->parafoil_sol->release(); // try deploying parafoil again, probably won't do much
//         }

//         delay(DEPLOY_DELAY); // wait for the parafoil to deploy/inflate before we begin guidance

//         this->flight_state = 2;
//         eeprom->write_state(this->flight_state, environment->bmp->baseline);
//     }

//     // blink the LEDs and print data at a rate of 5Hz
//     blink_led(200);
//     print_data();
// }

// void Raptor::descent()
// {
//     // if we have yet to wake the pilot, do so
//     if (!didwake)
//     {
//         // first set up our coordinates
//         Coordinate current, target;

//         current.latitude = environment->gps->latitude;
//         current.longitude = environment->gps->longitude;

//         target.latitude = TARGET_LAT;
//         target.longitude = TARGET_LONG;

//         // then wake the pilot and give it the coordinates
//         Serial << "Waking pilot\n";
//         pilot->wake(current, target);
//         didwake = true;
//     }

//     fly_time = time_elapsed;
//     if (fly_time > FLY_DELAY)
//     {                                        // don't want to constantly call fly
//         pilot->fly(environment->gps->angle); // the pilot just needs our current angle to do his calculations
//         fly_time = 0;
//     }

//     if (environment->bmp->getAltitude() < GROUND_ALT)
//     { // at 50ft (15.24 meters), transition to FS3 [LANDED]
//         if (environment->landing_check())
//         { // make sure that we have landed by checking the altitude constantly
//             pilot->sleep();
//             this->flight_state = 3;
//             Serial << "\n!!!! LANDED !!!!\n";
//         }
//     }

//     // blink the LEDs and print data at 10Hz
//     blink_led(100);
//     print_data();
// }

// void Raptor::landed()
// {
//     // in the landed state, only toggle the LEDs and buzzer every 2 seconds, then print data
//     analogWrite(BZZ_DTA, 200);
//     blink_led(2000);
//     analogWrite(BZZ_DTA, 0);

//     delay(200);
//     print_data();
// }

/*
 * print_data updates sensor readings then prints all relevant data to the serial pins.
 */
void Raptor::print_data()
{
    // environment->update();

    /* Let's spray the serial port with a hose of data */
    // time, temperature, pressure, altitude,
    Serial << time_elapsed << F(","); // << environment->bmp->readTemperature() << F(",") << environment->bmp->getPressure()
        //    << F(",") << environment->bmp->getAltitude() << F(",");

    // latitude, longitude, angle, (gps) altitude,
    Serial << (float)environment->gps->location.lat() << F(",") << (float)environment->gps->location.lng()
           << F(",") << (float)environment->gps->course.deg() << F(",") << (float)environment->gps->altitude.meters() << F(",");

    // x orientation, y orientation, z orientation,
    Serial << _FLOAT(environment->bno->data.orientation.x, 4) << F(",") << _FLOAT(environment->bno->data.orientation.y, 4)
           << F(",") << _FLOAT(environment->bno->data.orientation.z, 4) << "\n";

    // cutdown switch, parafoil switch, turn status, flight state
    // Serial << this->cutdown_sol->read_switch() << F(",") << this->parafoil_sol->read_switch() << F(",")
    //        << pilot->get_turn() << F(",") << this->flight_state << "\n"; // write everything to SD card

    // char data[256];
    // // time, temperature, pressure, altitude,
    // sprintf(data, "%d,%d,%d,%d,%d,%d,%d,%d\n", time_elapsed,
    //         environment->gps->latitude, environment->gps->longitude, environment->gps->angle, environment->gps->altitude,
    //         environment->bno->data.orientation.y, environment->bno->data.orientation.y, environment->bno->data.orientation.y);

    // // Serial << "hello\n";
    // // sprintf(data, "test\n");
    // Serial << data;
    // logger->file.write(data);
}

/*
 *  startup_sequence initializes our solenoids, servos, and sensors.
 *   If in flight state 0 (launch), performs a sequence that indicates board power,
 *   solenoid power, servo power, and successful sensor initialization.
 */
void Raptor::startup_sequence(void)
{
    // indicate board power with a buzzer beep if in flight state 0
    // if (this->flight_state == 0)
    // {
    //     beep(500);
    // }

    // if we're in flight state 0 test the servos
    if (this->flight_state == 0)
    {
        pilot->servo_test(); // rotates and resets each servo
        delay(200);
    }

    // initialize sensors, then indicate if we were successful or not
    if (environment->init(this->flight_state))
    { // if the initialization was successful and we're in flight state 0 blink 5 times
        // if (this->flight_state == 0)
        // {
        //     for (int i = 0; i < 5; i++)
        //         blink_led(500);
        // }
    }
    else
    { // if the initialization was unsuccessful and we're in flight state 1 beep & blink 15 times
        // if (this->flight_state == 0)
        // {
        //     for (int i = 0; i < 15; i++)
        //     {
        //     beep(500, true);
        //     }
        // }
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

void Raptor::sensor_demo()
{
    delay(1000);
    print_data();
}