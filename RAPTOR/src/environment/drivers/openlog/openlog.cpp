/*
  openlog.cpp -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#include "openlog.h"
// use read() , serial.read()

#define RESET_OPENLOG 4

Openlog::Openlog()
{
}

void Openlog::init(void)
{
    uint8_t count = 0;

    pinMode(RESET_OPENLOG, OUTPUT);
    Serial.begin(115200);

    while (!Serial)
        ;

    //Reset OpenLog
    digitalWrite(RESET_OPENLOG, LOW);
    delay(100);
    digitalWrite(RESET_OPENLOG, HIGH);

    //Wait for OpenLog to respond with '<' to indicate it is alive and recording to a file
    while ((count++ < 5) && !(Serial.available() && Serial.read() == '<'))
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

void Openlog::write(char *input)
{
    Serial.println(input);
}
