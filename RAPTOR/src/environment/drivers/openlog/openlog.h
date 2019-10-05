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
    void init(void);
    char *read(char *request);
    void write(char *input);

private:
    void command(void);
};