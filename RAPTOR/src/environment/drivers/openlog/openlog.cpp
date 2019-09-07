/*
  openlog.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "openlog.h"
// use read() , serial.read()

Openlog::Openlog()
{
    uint8_t count = 0;
    int resetOpenLog = 4; //This pin resets OpenLog. Connect pin 4 to pin GRN on OpenLog.

    pinMode(resetOpenLog, OUTPUT);
    Serial.begin(9600);

    //Reset OpenLog
    digitalWrite(resetOpenLog, LOW);
    delay(100);
    digitalWrite(resetOpenLog, HIGH);

    //Wait for OpenLog to respond with '<' to indicate it is alive and recording to a file
    while (count++ < 5 || !(Serial.available() && Serial.read() == '<'))
        ;
}

//This function pushes OpenLog into command mode
void Openlog::command(void)
{
    uint8_t count = 0;
    // Send three control z to enter OpenLog command mode
    // Works with Arduino v1.0
    Serial.write(26);
    Serial.write(26);
    Serial.write(26);

    //Wait for OpenLog to respond with '>' to indicate we are in command mode
    while (count++ < 5 || !(Serial.available() && Serial.read() == '<'))
        ;
}

char *Openlog::read(char *request)
{
    uint8_t count = 0;
    Serial.write(13); //This is \r
    if (Serial.available() > 0)
    {
        Serial.read();
    }

    //New way
    //OpenLog.print("read ");
    //OpenLog.println(filename); //regular println works with OpenLog v2.51 and above

    //The OpenLog echos the commands we send it by default so we have 'disk\r' sitting
    //in the RX buffer. Let's try to not print this.
    while (count++ < 5 || !(Serial.available() && Serial.read() == '<'))
        ;
}

void Openlog::write(String input)
{
    Serial.print(input);
}
