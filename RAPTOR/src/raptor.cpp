/*
  raptor.cpp -
    DESCRIPTION NEEDED.
    Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "raptor.h"
#include <Streaming.h> // http://arduiniana.org/libraries/streaming/

#include "test/test.h"

/*
 * Arduino setup function, first function to be run.
 */

static elapsedMillis time_elapsed;

Raptor::Raptor()
{
    this->parafoil_sol = new Solenoid(9, A0, A2);
    this->cutdown_sol = new Solenoid(8, A1, A3);

    this->environment = Environment::getInst();
    this->eeprom = new Prom();
    pilot = Pilot::getInst();
}

void Raptor::init(void)
{
    Serial.begin(115200);
    time_elapsed = 0;

    Serial << "begin init\n";

    /* Buzzer and LEDs */
    pinMode(BZZ_DTA, OUTPUT);  // Set buzzer to output
    pinMode(LEDS_DTA, OUTPUT); // Set LEDs to output

    /* GPS */
    environment->init(true); // for testing pcb

    pinMode(SET_BTN, OUTPUT);
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
                   "TEMPERATURE,PRESSURE,ALTITUDE,"
                   "LATITUDE,LONGITUDE,ANGLE,GPS_ALT,"
                   "X,Y,Z, "
                   "SWC,SWP,"
                   "TURN,FLIGHT_STATE\n")); // data header
}

void Raptor::launch()
{
    if (environment->bmp->getAltitude() > GROUND_ALT)
    { // at 50ft (15.24 meters), transition to FS1 [ASCENT]
        this->flight_state = 1;
        eeprom->write_state(this->flight_state, environment->bmp->baseline);
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
        this->cutdown_sol->release();

        if (!this->cutdown_sol->read_switch())
        { // we want to make sure that we have cut down
            Serial << F("\n!!!! CUTDOWN ERROR DETECTED !!!!\n");
            this->cutdown_sol->release(); // try cutdown again, probably won't do much
        }

        // PARAFOIL DEPLOY
        while (environment->bmp->getAltitude() > CUTDOWN_ALT - 3.048)
        { // wait 3 meters to deploy the parafoil
            delay(1);
            print_data();
        }

        this->parafoil_sol->release();

        if (!this->parafoil_sol->read_switch())
        { // make sure the parafoil has deployed
            Serial << F("\n!!!! PARAFOIL DEPLOYMENT ERROR DETECTED !!!!\n");
            this->parafoil_sol->release(); // try deploying parafoil again, probably won't do much
        }

        delay(DEPLOY_DELAY); // wait for the parafoil to deploy/inflate before we begin guidance

        this->flight_state = 2;
        eeprom->write_state(this->flight_state, environment->bmp->baseline);
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
            this->flight_state = 3;
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
 * Keeps Raptor in state to allow testing of parafoil control using RC receiver.
 */
void Raptor::rc_test()
{
    // Solenoid secure() = can be engaged
    // Solenoid release() = releases an engaged solenoid
    static const uint8_t parafoil_pin = 7;
    static const uint8_t cutdown_pin = 4;

    pinMode(parafoil_pin, INPUT);
    pinMode(cutdown_pin, INPUT);

    delay(1000); // wait 5 seconds before starting

    Serial << "Starting RC Test!\n";

    pilot->servo_init();
    while (true)
    {
        //int para_value = Read_RC_Digital(parafoil_pin);
        //int cutdown_value = Read_RC_Digital(cutdown_pin);
        //int test = analogRead(A0);
        int para_value = Read_RC_Analog(A1);
        int cutdown_value = Read_RC_Analog(A0);

        Serial << "Parafoil: " << para_value << ", Cutdown: " << cutdown_value << "\n";
        // Prevents random noise saying that both switches are triggered

        //if(para_value == 1 && cutdown_value == 0)
        //{
        //    para_value = 1022;
        //    cutdown_value = 1022;
        //    Serial << "Both HIGH\n";
        //}

        // Outputs current value to serial
        //Serial << "Digital read (parafoil): " << para_value << "\n";
        //Serial << "Digital read (cutdown): " << cutdown_value << "\n\n";

        // Balloon/Drone cutdown logic
        if (cutdown_value > 1000)
        {
            // right analog stick all the way up
            cutdown_sol->secure();
            Serial << "Cutdown Solenoid: Closed.\n";
            analogWrite(A2, 255);
        }
        else
        {
            cutdown_sol->release();
            Serial << "Cutdown Solenoid: Open.\n";
            analogWrite(A2, 0);
        }

        // Parafoil cutdown logic
        if (para_value > 1000)
        {
            // right analog stick all the way down
            parafoil_sol->secure();
            Serial << "Parafoil Solenoid Closed. \n";
            analogWrite(A3, 255);
        }
        else
        {
            parafoil_sol->release();
            Serial << "Parafoil Solenoid Open. \n";
            analogWrite(A3, 0);
        }
    }
}

/*
 * print_data updates sensor readings then prints all relevant data to the serial pins.
 */
void Raptor::print_data()
{
    environment->update();

    /* Let's spray the serial port with a hose of data */
    // time, temperature, pressure, altitude,
    Serial << time_elapsed << F(",") << environment->bmp->readTemperature() << F(",") << environment->bmp->getPressure()
           << F(",") << environment->bmp->getAltitude() << F(",");

    // latitude, longitude, angle, (gps) altitude,
    Serial << _FLOAT(environment->gps->latitude, 7) << F(",") << _FLOAT(environment->gps->longitude, 7)
           << F(",") << _FLOAT(environment->gps->angle, 7) << F(",") << environment->gps->altitude << F(",");

    // x orientation, y orientation, z orientation,
    Serial << _FLOAT(environment->bno->data.orientation.x, 4) << F(",") << _FLOAT(environment->bno->data.orientation.y, 4)
           << F(",") << _FLOAT(environment->bno->data.orientation.z, 4) << F(",");

    // cutdown switch, parafoil switch, turn status, flight state
    Serial << this->cutdown_sol->read_switch() << F(",") << this->parafoil_sol->read_switch() << F(",")
           << pilot->get_turn() << F(",") << this->flight_state << "\n"; // write everything to SD card
}

/*
 *  startup_sequence initializes our solenoids, servos, and sensors.
 *   If in flight state 0 (launch), performs a sequence that indicates board power,
 *   solenoid power, servo power, and successful sensor initialization.
 */
void Raptor::startup_sequence(void)
{
    // indicate board power with a buzzer beep if in flight state 0
    if (this->flight_state == 0)
    {
        beep(500);
    }

    // initialize solenoids, should hear them click and see the indicator LEDs turn on
    parafoil_sol->secure();
    parafoil_sol->read_switch();

    cutdown_sol->secure();
    parafoil_sol->read_switch();

    // initialize servos, if we're in flight state 0 we'll test them as well
    pilot->servo_init();
    if (this->flight_state == 0)
    {
        pilot->servo_test(); // rotates and resets each servo
        delay(200);
    }

    // initialize sensors, then indicate if we were successful or not
    if (environment->init(this->flight_state))
    { // if the initialization was successful and we're in flight state 0 blink 5 times
        if (this->flight_state == 0)
        {
            for (int i = 0; i < 5; i++)
                blink_led(500);
        }
    }
    else
    { // if the initialization was unsuccessful and we're in flight state 1 beep & blink 15 times
        if (this->flight_state == 0)
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