/*
  openlog.h -
	DESCRIPTION NEEDED.
	Part of the RAPTOR project, authors: Sean Widmier, Colin Oberthur
*/

#include "Arduino.h"

class Openlog
{
public:
    Openlog();
    char *read(char *request);
    void write(String input);

private:
    void command(void);
};