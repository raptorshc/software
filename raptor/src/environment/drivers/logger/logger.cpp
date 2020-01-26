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
void Logger::init(void)
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
        // don't do anything more:
        return;
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
    }

    this->file.close();
    // maybe add error handling
}

/*
 * write opens the file, writes the data, then closes it to ensure the data is written
 */
void Logger::write(String data)
{
    this->file = SD.open(this->filename, FILE_WRITE);
    if (!this->file)
    {
        Serial.println("Error opening file.");
    }
    else
    {
        this->file.print(data);
    }
    this->file.close();
}