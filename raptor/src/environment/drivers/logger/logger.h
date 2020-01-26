/*
 * logger.h -
 * Class written for use with the Adafruit Micro SD Card Breakout.
 * Utilizes Arduino SD Library.
 * Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/
#ifndef LOGGER_H_
#define LOGGER_H_

#include <SD.h>

class Logger
{
public:
    static Logger *getInst();
    void init(void);
    void write(String data);

    File file;

private:
    Logger() {}
    char filename[32];
};

#endif