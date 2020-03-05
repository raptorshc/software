/*
 * logger.cpp -
 * Contains implementation for functions in logger.h.
 * Part of the Thunderbird project, authors: Sean Widmier, Colin Oberthur
*/
#include "logger.h"
#include <SPI.h>

/*
 *
 */
Logger *Logger::getInst()
{
    static Logger *inst = NULL;
    if (!inst)
    {
        inst = new Logger;
    }
    return inst;
}

/*
 * init checks for a valid SD card file name and creates a new file
 */
boolean Logger::init(void)
{
    String filename = "data";
    int number = 0;
    SPI.setMOSI(11); // Audio shield has MOSI on pin 7
    SPI.setMISO(12); // Audio shield has MOSI on pin 7
    SPI.setSCK(13);  // Audio shield has SCK on pin 14

    pinMode(10, OUTPUT);

    if (!SD.begin(10))
    {
        Serial.println("Card failed, or not present");
        return false;
    }
    Serial.println("card initialized.");

    itoa(number, this->filename, 10);
    while (SD.exists(this->filename))
    {
        itoa(number++, this->filename, 10);
    }
    this->file = SD.open(this->filename, FILE_WRITE);
    if (!this->file)
    {
        Serial.println("Error opening file.");
        return false;
    }

    this->file.close();
    return true;
}

/*
 * write opens the file, writes the data, then closes it to ensure the data is written
 */
boolean Logger::write(String data)
{
    this->file = SD.open(this->filename, FILE_WRITE);
    if (!this->file)
    {
        Serial.println("Error opening file.");
        return false;
    }
    else
    {
        this->file.print(data);
    }
    this->file.close();
    this->last_write = true;
    return true;
}