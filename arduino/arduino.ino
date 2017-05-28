#include "Communication.h"

Communication communication;

int trainMotor = 5;
int trainEnable = 4;

int rackMotor = 10;
int rackEnable = 11;

int conveyorMotor;
int conveyorEnable;

int insertMotor = 9;
int insertEnable = 8;

boolean onXWhite;
int onXWhiteC;
boolean onXLoc;
boolean dirXSwitch;
int startXLoc = 0;
int xLoc = 0;
boolean xDone;
boolean dirX;
int speedX;
int onXBlackC;
int brakeSpeed;
boolean split;
int splitC;

boolean onYWhite;
int onYWhiteC;
boolean onYLoc;
boolean dirYSwitch;
int startYLoc = 0;
int yLoc = 0;
boolean yDone;
boolean dirY;
int speedY;
int onYBlackC;
boolean splitY;
int splitYC;

int iLoc;
boolean onIWhite;
boolean onILoc;
boolean iDone;
int speedI;
int onIWhiteC;
int onIBlackC;
int splitIC;
boolean splitI = true;



void setup() {
  // Setup Motor Pins
  for (int i = 4; i < 12; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(A0, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Serial.begin(9600);
  resetRobot();
  int coor[5][2] = {
    {1, 5},
    {2, 2},
    {3, 5},
    {4, 2},
    {5, 5},
  };
  int drop[5] = {1, 2, 3, 4, 5 };
  runPickandDrop(coor, drop);

}

void loop() {
}

void runPickandDrop(int coor[5][2], int drop[5]) {
  for (int coorN = 0; coorN < 5; coorN++) {
    int x = coor[coorN][0];
    int y = coor[coorN][1];
    moveCoor(x, y);
    delay(300);
    extractPackage();
  }
  for (int dropN = 0; dropN < 5; dropN++) {
    int x = drop[dropN];
    int box = dropN + 2;
    empty(x, box);
    delay(400);
  }
}

void moveXCoor(int coor) {
  if (xDone || (startXLoc == 0 && xLoc == coor) || !coor) {
    xDone = true;
  } else {
    int trainSensor = readSensor(0, 150);

    speedX = 95;
    if (startXLoc == 0) {
      startXLoc = 1;
      if (coor - xLoc > 0) {
        dirX = true;
        moveXMotor(speedX, dirX);
        brakeSpeed = coor - xLoc;
      } else {
        dirX = false;
        moveXMotor(speedX, dirX);
        brakeSpeed = xLoc - coor;
      }
    }
    //Zet richting correct

    if (trainSensor && !onXWhite && !onXLoc) { //Sensor Wit & Niet op wit & niet al op locatie
      onXWhite = true;
      onXBlackC = 0;
      if (dirX) {
        xLoc++; //Tel op of af aan de hand van richting.
      } else {
        xLoc--;
      }
      if (xLoc != coor) { // nog niet op locatie
        moveXMotor(speedX, dirX);
      } else if (xLoc == coor) { // op locatie
        onXLoc = true;
        moveXMotor(0, !dirX);
      }
    } else if (trainSensor && onXWhite && !onXLoc) { // Sensor wit & op wit & niet op locatie
      moveXMotor(95, dirX);
    } else if (trainSensor && onXLoc) { //Sensor Wit & op locatie
      if (onXWhiteC < 1) {
        moveXMotor(255, !dirX);
        delay(brakeSpeed * 2 + 10);
      }
      moveXMotor(0, dirX);
      onXWhite = true;
      onXBlackC = 0;
      onXWhiteC++;
    } else if (!trainSensor && onXLoc) { //Sensor zwart && op locatie
      if (split) {
        moveXMotor(speedX + 20, !dirX);
        splitC++;
        if (splitC > 150) {
          split = false;
        }
      } else {
        moveXMotor(0, !dirX);
        splitC++;
        if (splitC > 400) {
          splitC = 0;
          split = true;
        }
      }
    } else if (!trainSensor && onXBlackC > 300) { // Sensor zwart
      onXWhite = false;
      moveXMotor(speedX, dirX);
    } else if (!trainSensor) { // Sensor zwart
      onXBlackC++;
    }
    if (onXWhiteC == 1000) {
      xDone = true;
      onXWhite = true;
    }
  }
}


void moveYCoor(int coor) {
  if (yDone || (startYLoc == 0 && yLoc == coor) || !coor) {
    yDone = true;
  } else {
    int rackSensor = readSensor(4, 200);
    if (startYLoc == 0) {
      startYLoc = yLoc;
      if (coor - startYLoc > 0) {
        speedY = 120;
        dirY = true;
      } else {
        dirY = false;
        speedY = 70;
      }
    } //Zet richting correct

    if (rackSensor && !onYWhite && !onYLoc) { //Sensor Wit & Niet op wit & niet al op locatie
      onYWhite = true;
      onYBlackC = 0;
      if (dirY) {
        yLoc++; //Tel op of af aan de hand van richting.
      } else {
        yLoc--;
      }
      if (yLoc != coor) { // nog niet op locatie
        moveYMotor(speedY, dirY);
      } else if (yLoc == coor) { // op locatie
        onYLoc = true;
        moveYMotor(0, dirY);
      }
    } else if (rackSensor && onYWhite && !onYLoc) { // Sensor wit & op wit & niet op locatie
      if (dirY) {
        moveYMotor(speedY, dirY);
      }
      else {
        if (splitY) {
          moveYMotor(speedY, dirY);
          splitYC++;
          if (splitYC > 500) {
            splitY = false;
          }
        } else {
          moveYMotor(0, dirY);
          splitYC++;
          if (splitYC > 1200) {
            splitYC = 0;
            splitY = true;
          }
        }
      }
    } else if (rackSensor && onYLoc) { //Sensor Wit & op locatie
      moveYMotor(0, !dirY);
      onYWhite = true;
      onYBlackC = 0;
      onYWhiteC++;
    } else if (!rackSensor && onYLoc && !dirY) {
      moveYMotor(120, !dirY);
    } else if (!rackSensor && onYBlackC > 300) { // Sensor zwart
      onYWhite = false;
      if (dirY) {
        moveYMotor(speedY, dirY);
      }
      else {
        if (splitY) {
          moveYMotor(speedY, dirY);
          splitYC++;
          if (splitYC > 500) {
            splitY = false;
          }
        } else {
          moveYMotor(0, dirY);
          splitYC++;
          if (splitYC > 1200) {
            splitYC = 0;
            splitY = true;
          }
        }
      }
    } else if (!rackSensor) { // Sensor zwart
      onYBlackC++;
    }
    if (onYWhiteC == 500) {
      yDone = true;
    }
  }
}

void extractPackage() {
  moveIMotor(90, 0);
  delay(2000);
  moveIMotor(0, 0);

  while (readSensor(4, 300)) {
    moveYMotor(120, 1);
  }
  moveYMotor(0, 1);

  while (!readSensor(5, 500)) {
    moveIMotor(80, 1);
  }
  moveIMotor(0, 1);

  while (!readSensor(4, 300)) {
    moveYMotor(50, 0);
  }
  moveYMotor(0, 0);
}
void emptyPackage(int coor) {
  int insertSensor = readSensor(5, 650);
  speedI = 90;
  if (insertSensor && !onIWhite && !onILoc) { //Sensor Wit & Niet op wit & niet al op locatie
    onIWhite = true;
    onIBlackC = 0;
    iLoc++;
    if (iLoc != coor) { // nog niet op locatie
    } else if (iLoc == coor) { // op locatie
      onILoc = true;
      moveIMotor(0, 0);
    }
  } else if (insertSensor && onIWhite && !onILoc) { // Sensor wit & op wit & niet op locatie
    if (splitI) {
      moveIMotor(speedI, 1);
      splitIC++;
      if (splitIC > 200) {
        splitI = false;
      }
    } else {
      moveIMotor(0, 1);
      splitIC++;
      if (splitIC > 700) {
        splitIC = 0;
        splitI = true;
      }
    }
  } else if (insertSensor && onILoc) { //Sensor Wit & op locatie
    moveIMotor(0, 0);
    onIWhiteC++;
  } else if (!insertSensor && onIBlackC > 1000) { // Sensor zwart
    onIWhite = false;
    if (splitI) {
      moveIMotor(speedI, 1);
      splitIC++;
      if (splitIC > 200) {
        splitI = false;
      }
    } else {
      moveIMotor(0, 1);
      splitIC++;
      if (splitIC > 700) {
        splitIC = 0;
        splitI = true;
      }
    }
  } else if (!insertSensor) { // Sensor zwart
    onIBlackC++;
    if (splitI) {
      moveIMotor(speedI, 1);
      splitIC++;
      if (splitIC > 200) {
        splitI = false;
      }
    } else {
      moveIMotor(0, 1);
      splitIC++;
      if (splitIC > 700) {
        splitIC = 0;
        splitI = true;
      }
    }
  }
  if (onIWhiteC == 500) {
    iDone = true;
    splitI = true;
  }
}

void moveXMotor(int speed, boolean dir) {
  analogWrite(trainMotor, speed);
  digitalWrite(trainEnable, dir);
}

void moveYMotor(int speed, boolean dir) {
  analogWrite(rackMotor, speed);
  digitalWrite(rackEnable, dir);
}

void moveIMotor(int speed, boolean dir) {
  analogWrite(insertMotor, speed);
  digitalWrite(insertEnable, dir);
}

boolean readSensor(int pin, int value) {
  char A = A;
  if (analogRead(A + pin) < value) {
    return true;
  } else {
    return false;
  }
}

void moveCoor(int x, int y) {
  while (!yDone || !xDone) {
    moveYCoor(y);
    moveXCoor(x);
  }
  resetVars();
}

void empty(int x, int box) {
  while (!yDone || !xDone) {
    moveYCoor(1);
    moveXCoor(x);
  }
  while (!iDone) {
    emptyPackage(box);
  }
  resetVars();
}

void resetVars() {
  xDone = false;
  onXLoc = false;
  onXWhiteC = 0;
  startXLoc = 0;
  onXBlackC = 0;

  yDone = false;
  onYLoc = false;
  onYWhiteC = 0;
  startYLoc = 0;
  onYBlackC = 0;

  iDone = false;
  onILoc = false;
  onIWhiteC = 0;
  onIBlackC = 0;
}
void resetRobot() {
  extractPackage();
  moveYMotor(100, 0);
  moveXMotor(110, 0);
  delay(5000);
  moveYMotor(0, 0);
  moveXMotor(0, 0);
}

//COMMUNICATION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ExecuteCommand()
{
  //Switch between commands
  if (curCom == "cmdSendTest")
  {
    DoTaskTest();
  }
  else if (curCom == "cmdLol")
  {
    DoTaskLol();
  }
  else
  {
    //If the given command is not in the list, reset
    Serial.println("Command not recognized");
    hasTaskEnded = true;
  }
}

void CheckForCommand()
{
  //If not already working on something, switch to the next command in line
  if (curCom == "" && commandArray[0] != "")
  {
    Serial.println("looking for command");
    curCom = getNextCommand();
  }
}

void DoTaskTest()
{
  buttonState1 = digitalRead(pushbutton);

  if (buttonState1 == HIGH)
  {
    if (previousbuttonState1 == LOW)
    {
      // THIS IS THE OPERATION I ONLY WANT TO RUN ONCE
      Serial.println("All done with task Test!");
      hasTaskEnded = true;
    }
  }
  previousbuttonState1 = buttonState1;
}

void DoTaskLol()
{
  buttonState1 = digitalRead(pushbutton);

  if (buttonState1 == HIGH)
  {
    if (previousbuttonState1 == LOW)
    {
      // THIS IS THE OPERATION I ONLY WANT TO RUN ONCE
      Serial.println("All done with task LOL!");
      hasTaskEnded = true;
    }
  }
  previousbuttonState1 = buttonState1;
}

void resetCommandArray()
{
  for (int i = 0; i < maxArraySize; i++)
  {
    commandArray[i] = "";
  }
}

String getNextCommand()
{
  String commandToReturn = "";

  if (commandIndex > 0)
  {
    commandToReturn = commandArray[0];

    for (int i = 1; i < maxArraySize; i++)
    {
      commandArray[i - 1] = commandArray[i];
    }

    commandArray[commandIndex] = "";
  }
  commandIndex--;

  return commandToReturn;
}

void readInput()
{
  static boolean recvInProgress = false;
  static byte index = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false)
  {
    rc = Serial.read();

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
      }
    }
    else if (rc == startMarker)
    {
      recvInProgress = true;
    }
  }
}

void addCommand()
{
  String inputString = String(receivedChars);

  if (commandIndex < maxArraySize - 1)
  {
    commandArray[commandIndex] = inputString;
    commandIndex++;

    Serial.print("new command added: ");
    Serial.println(inputString);
  }
  newData = false;
}

void sendOutput(String command, String extraInfo)
{
  String output = "<" + command;

  if (extraInfo != "")
  {
    output += "[" + extraInfo + "]";
  }
  output += ">";
  Serial.println(output);
}


