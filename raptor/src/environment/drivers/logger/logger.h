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
    boolean init(void);
    boolean write(String data);

    File file;
    boolean last_write; // indicates success status of last write

private:
    Logger()
    {
        this->last_write = true;
    }
    char filename[32];
};

#endif