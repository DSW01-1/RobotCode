#include "Arduino.h"
#include "Communication.h"

Communication::Communication()
{
	
}

String Communication::readInput()
{
	const byte numChars = 32;	
	char receivedChars[numChars];
	boolean newData = false;	
	
	static boolean recvInProgress = false;
	static byte index = 0;
	char startMarker = '<';
    char endMarker = '>';
	String inputString = "";
    
    while (Serial.available() > 0 && newData == false)
	{
       char rc = Serial.read();

         if (recvInProgress == true) 
		 {
            if (rc != endMarker)
			{
                receivedChars[index] = rc;
                index++;
                if (index >= numChars) 
				{
                    index = numChars - 1;
				}
            }
            else
			{
                receivedChars[index] = '\0'; // terminate the string
                recvInProgress = false;
                index = 0;
                newData = true;
				inputString = receivedChars;
            }
        }
        else if (rc == startMarker) 
		{
            recvInProgress = true;
        }
    }
	
	return inputString;
}

