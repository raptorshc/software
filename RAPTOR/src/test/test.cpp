/*
*
*/
#include "test.h"

int readRC(uint8_t pin)
{
    // Increased the samples to 50 to smooth out th values from the RC controller
    //
    // TODO: Test this algorithm. It claims to offering averaging without 
    // having to store every step of the averging process
    float average = 0;
    float delta;
    //for(int i = 0; i < 50; i++){
    //    float result = pulseIn(pin, HIGH, 20000);
    //    average += result;
        //average -= average/i;
        //average += result/i;
    //}
    //for (int j = 0; j < 2; j++)
    //{
        for (int i = 0; i < 10000; i++)
        {
            if (digitalRead(pin) == HIGH)
            {
                average++;
            }
        }
      //  delta += average;
    //}
    //delta = delta/2;

    return average;
    //return average/50;
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