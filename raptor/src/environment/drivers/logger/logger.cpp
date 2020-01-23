/*
 * logger.cpp -
 * Contains implementation for functions in logger.h.
 * Part of the Thunderbird project, authors: Sean Widmier, Colin Oberthur
*/
#include "logger.h"

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
 * init checks for a valid SD card name and creates a new file
 */
void Logger::init(void)
{
    String filename = "data";
    int number = 0;
    while (SD.exists(filename + String(number++)))
        ;
    pinMode(10, OUTPUT);
    this->file = SD.open(filename + String(number), FILE_WRITE);
    // maybe add error handling
}