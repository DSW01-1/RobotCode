
#ifndef Communication_h
#define Communication_h

#include "Arduino.h"

class Communication
{
  public:
	int commandIndex = 0;
	String commandArray[];
    Communication();
    void readInput();
	void resetCommandArray();
	String getNextCommand();	
};

#endif

