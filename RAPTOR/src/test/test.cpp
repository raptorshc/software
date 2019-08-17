/*
  test.cpp - 
    Collection of various testing functions.
  Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur, Sean Mitchell
*/
#include "test.h"

int readRC(uint8_t pin)
{
    // ==========================================================
    // THIS DOESN'T WORK, DON'T USE IT WIHTOUT FIXING (6/23/2019)
    // ===========================================================
    //
    // Increased the samples to 50 to smooth out th values from the RC controller
    //
    // TODO: Test this algorithm. It claims to offering averaging without 
    // having to store every step of the averging process
    float average = 0;
    float delta = 0;
    //for(int i = 0; i < 50; i++){
    //    float result = pulseIn(pin, HIGH, 20000);
    //    average += result;
        //average -= average/i;
        //average += result/i;
    //}
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 10000; i++)
        {
            if (digitalRead(pin) == HIGH)
            {
                average++;
            }
            
        }
        delta += average;
    }
    delta = delta/2;

    return delta;
    //return average/50;
}

int Read_RC_Digital(uint8_t pin)
{
    int val;
    int avg = 0;
    int avg_cofficient = 3;

    // Samples the digitalRead() for the number of avg_cofficient
    for (int i = 0; i < avg_cofficient; i++)
    {
        val = digitalRead(pin);
        if (val == 1)
        {
            avg++;
        }
        delay(10);
    }

    // Checks the avg value and return a '0' or '1' depending on if the avg is greater than 33% of its max value
    // Greater than 33% of max = 1
    // Less than 33% of max = 0
    if (avg > (avg_cofficient/3))
    {
        val = 1;
    }
    else 
    {
        val = 0;
    }

    // Returns the '0' or '1'
    return val;
}

int Read_RC_Analog(uint8_t pin)
{
    // Samples the digitalRead() for the number of avg_cofficient
    // and the returns the avg of those samples

    int val;
    int avg = 0;
    int avg_cofficient = 3;

    for (int i = 0; i < avg_cofficient; i++)
    {
        val = analogRead(pin);
        avg += val;
    }
    avg = avg / avg_cofficient;
    return avg;
    
}

/*
 * custom_angle returns an angle parsed from user input
 */
float custom_angle(void)
{
    Serial.print("\nPlease input an angle: ");
    while (Serial.available() == 0)
        ; // wait for user input
    float angle = Serial.parseFloat();
    return angle;
}