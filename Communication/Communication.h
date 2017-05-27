
#ifndef Communication_h
#define Communication_h

#include "Arduino.h"

class Communication
{
  public:
	int commandIndex = 0;
    Communication();
    void readInput();
	String getNextCommand();	
};

#endif

