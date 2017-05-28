#include "Arduino.h"
#include "Communication.h"

const int maxArraySize = 10;
String commandArray[maxArraySize] = {"", "", "", "", "", "", "", "", "", ""};

Communication::Communication()
{
}

void Communication::resetCommandArray()
{
	for(int i = 0; i < maxArraySize; i++)
	{
		commandArray[i] = "";
	}	
}

String Communication::getNextCommand()
{
	String commandToReturn = "";
		
	if(commandIndex > 0)
	{
		commandToReturn = commandArray[0];
		
		for(int i = 1; i < maxArraySize; i++)
		{
			commandArray[i - 1] = commandArray[i];
		}
		
		commandArray[commandIndex] = "";
	}
	
	commandIndex--;
	
	
	return commandToReturn;
}

void Communication::readInput()
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
								
				if(commandIndex < maxArraySize - 1)
				{
					commandArray[commandIndex] = inputString;
					commandIndex++;					
				}				
            }
        }
        else if (rc == startMarker) 
		{
            recvInProgress = true;
        }
    }
}

