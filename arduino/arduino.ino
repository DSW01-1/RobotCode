//COMMUNICATION VARIABLES
///////////////////////////////////////////////////////////////////////
boolean hasTaskEnded = false;
boolean isBusyWithTask = false;
String curCom = "";

const byte numChars = 128;
char receivedChars[numChars];
boolean newData = false;

int commandIndex = 0;
const int maxArraySize = 10;
String commandArray[maxArraySize] = {"", "", "", "", "", "", "", "", "", ""};
String extraInfoArray[maxArraySize] = {"", "", "", "", "", "", "", "", "", ""};
boolean robotDidDone = false;


//OTHER VARIABLES
///////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////
//END OF VARIABLES

void setup()
{
  // Setup Motor Pins
  for (int i = 4; i < 12; i++)
  {
    pinMode(i, OUTPUT);
  }

  pinMode(A0, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Serial.begin(9600);
}

void loop()
{
  //Step 1: Read the serial for new commands
  readInput();

  //Step 2: If there is a new command available, add it to the list
  if (newData == true)
  {
    Serial.println("New command available");
    addCommand();
  }

  //Step 3: If the current task has been completed, reset
  if (hasTaskEnded)
  {
    Serial.println("Task done, resetting");
    moveCoor(1,1);
    extractPackage();
    iLoc=0;
    curCom = "";
    isBusyWithTask = false;
    hasTaskEnded = false;
    robotDidDone = false;
  }

  //Step 4: If robot is not fulfilling a command, check if there is a new one available
  if (isBusyWithTask == false)
  {
    CheckForCommand();
  }

  //Step 5: If there is a command available, proceed to fulfil it.
  if (curCom != "")
  {
    isBusyWithTask = true;
    ExecuteCommand();
  }
}

//Gets the products and drops them
void runPickandDrop(int coor[][2], int drop[], int amountOfCoords)
{
  int packageCycleIndex = 0;

  for (int coorN = 0; coorN < amountOfCoords - 1; coorN++)
  {
    Serial.println("Getting next package");
    Serial.print("index == ");
    Serial.print(packageCycleIndex);

    int x = coor[coorN][0];
    int y = coor[coorN][1];
    
    moveCoor(x+1, y);
    delay(300);

    if (packageCycleIndex < amountOfCoords)
    {
      extractPackage();
    }

    packageCycleIndex++;
  }

  for (int dropN = 0; dropN < amountOfCoords - 1; dropN++)
  {
    int x = drop[dropN];
    int box = (8 - amountOfCoords) + dropN;
    empty(x+6, box);
    delay(400);
  }

  robotDidDone = true;
}

void moveXCoor(int coor) {
  if (xDone || (startXLoc == 0 && xLoc == coor) || !coor)
  {
    xDone = true;
  } else {
    int trainSensor = readSensor(0, 400);

    speedX = 95;
    if (startXLoc == 0)
    {
      startXLoc = 1;
      if (coor - xLoc > 0)
      {
        dirX = true;
        moveXMotor(speedX, dirX);
        brakeSpeed = coor - xLoc;
      } else
      {
        dirX = false;
        moveXMotor(speedX, dirX);
        brakeSpeed = xLoc - coor;
      }
    }
    //Zet richting correct

    if (trainSensor && !onXWhite && !onXLoc)
    { //Sensor Wit & Niet op wit & niet al op locatie
      onXWhite = true;
      onXBlackC = 0;
      if (dirX)
      {
        xLoc++; //Tel op of af aan de hand van richting.
      } else
      {
        xLoc--;
      }
      if (xLoc != coor)
      {
        // nog niet op locatie
        moveXMotor(speedX, dirX);
      }
      else if (xLoc == coor)
      { // op locatie
        onXLoc = true;
        moveXMotor(0, !dirX);
      }
    }
    else if (trainSensor && onXWhite && !onXLoc)
    {
      // Sensor wit & op wit & niet op locatie
      moveXMotor(95, dirX);
    }
    else if (trainSensor && onXLoc)
    {
      //Sensor Wit & op locatie
      if (onXWhiteC < 1)
      {
        moveXMotor(255, !dirX);
        delay(brakeSpeed * 2 + 20);
      }
      moveXMotor(0, dirX);
      onXWhite = true;
      onXBlackC = 0;
      onXWhiteC++;
    }
    else if (!trainSensor && onXLoc)
    {
      //Sensor zwart && op locatie
      if (split)
      {
        moveXMotor(speedX + 40, !dirX);
        splitC++;
        if (splitC > 150)
        {
          split = false;
        }
      }
      else
      {
        moveXMotor(0, !dirX);
        splitC++;
        if (splitC > 400)
        {
          splitC = 0;
          split = true;
        }
      }
    }
    else if
    (!trainSensor && onXBlackC > 500)
    {
      // Sensor zwart
      onXWhite = false;
      moveXMotor(speedX, dirX);
    }
    else if (!trainSensor)
    { // Sensor zwart
      onXBlackC++;
    }
    if (onXWhiteC == 1000)
    {
      xDone = true;
      onXWhite = true;
    }
  }
}

void moveYCoor(int coor)
{
  if (yDone || (startYLoc == 0 && yLoc == coor) || !coor)
  {
    yDone = true;
  }
  else
  {
    int rackSensor = readSensor(4, 400);
    if (startYLoc == 0) {
      startYLoc = yLoc;
      if (coor - startYLoc > 0) {
        speedY = 120;
        dirY = true;
      } else {
        dirY = false;
        speedY = 60;
      }
    } //Zet richting correct

    if (rackSensor && !onYWhite && !onYLoc)
    {
      //Sensor Wit & Niet op wit & niet al op locatie
      onYWhite = true;
      onYBlackC = 0;
      if (dirY)
      {
        //Tel op of af aan de hand van richting.
        yLoc++;
      }
      else
      {
        yLoc--;
      }
      if (yLoc != coor)
      {
        // nog niet op locatie
        moveYMotor(speedY, dirY);
      } else if (yLoc == coor)
      {
        // op locatie
        onYLoc = true;
        moveYMotor(0, dirY);
      }
    }
    else if (rackSensor && onYWhite && !onYLoc)
    {
      // Sensor wit & op wit & niet op locatie
      if (dirY)
      {
        moveYMotor(speedY, dirY);
      }
      else {
        if (splitY)
        {
          moveYMotor(speedY, dirY);
          splitYC++;
          if (splitYC > 500)
          {
            splitY = false;
          }
        }
        else
        {
          moveYMotor(0, dirY);
          splitYC++;
          if (splitYC > 1300)
          {
            splitYC = 0;
            splitY = true;
          }
        }
      }
    }
    else if (rackSensor && onYLoc)
    {
      //Sensor Wit & op locatie
      moveYMotor(0, !dirY);
      onYWhite = true;
      onYBlackC = 0;
      onYWhiteC++;
    }
    else if (!rackSensor && onYLoc && !dirY)
    {
      moveYMotor(120, !dirY);
    }
    else if (!rackSensor && onYBlackC > 1000)
    { // Sensor zwart
      onYWhite = false;
      if (dirY)
      {
        moveYMotor(speedY, dirY);
      }
      else
      {
        if (splitY)
        {
          moveYMotor(speedY, dirY);
          splitYC++;
          if (splitYC > 500)
          {
            splitY = false;
          }
        }
        else
        {
          moveYMotor(0, dirY);
          splitYC++;
          if (splitYC > 1300)
          {
            splitYC = 0;
            splitY = true;
          }
        }
      }
    } else if (!rackSensor)
    {
      // Sensor zwart
      onYBlackC++;
    }
    if (onYWhiteC == 500)
    {
      yDone = true;
    }
  }
}

void extractPackage()
{
  moveIMotor(90, 0);
  delay(2000);
  moveIMotor(0, 0);

  int exBCount = 0;
  while (exBCount < 2000) {
    while (readSensor(4, 200)) {
      moveYMotor(120, 1);
    }
    exBCount++;
  }
  moveYMotor(0, 1);

  exBCount = 0;
  while (exBCount < 300) {
    while (!readSensor(5, 500)) {
      moveIMotor(70, 1);
    }
    exBCount++;
  }
  moveIMotor(0, 1);

  exBCount = 0;
  while (exBCount < 300) {
    while (!readSensor(4, 200)) {
      moveYMotor(40, 0);
    }
    exBCount++;
  }
  moveYMotor(90, 1);
  delay(25);
  moveYMotor(0, 0);
  delay(500);

}
void emptyPackage(int coor)
{
  int insertSensor = readSensor(5, 600);
  speedI = 90;
  if (insertSensor && !onIWhite && !onILoc)
  {
    //Sensor Wit & Niet op wit & niet al op locatie
    onIWhite = true;
    onIBlackC = 0;
    iLoc++;

    if (iLoc != coor)
    {
      // nog niet op locatie
    }
    else if (iLoc == coor)
    {
      // op locatie
      onILoc = true;
      moveIMotor(0, 0);
    }
  }
  else if (insertSensor && onIWhite && !onILoc)
  {
    // Sensor wit & op wit & niet op locatie
    if (splitI)
    {
      moveIMotor(speedI, 1);
      splitIC++;
      if (splitIC > 200)
      {
        splitI = false;
      }
    }
    else
    {
      moveIMotor(0, 1);
      splitIC++;
      if (splitIC > 700)
      {
        splitIC = 0;
        splitI = true;
      }
    }
  }
  else if (insertSensor && onILoc)
  {
    //Sensor Wit & op locatie
    moveIMotor(0, 0);
    onIWhiteC++;
  }
  else if (!insertSensor && onIBlackC > 1000)
  {
    // Sensor zwart
    onIWhite = false;
    if (splitI)
    {
      moveIMotor(speedI, 1);
      splitIC++;
      if (splitIC > 200)
      {
        splitI = false;
      }
    }
    else
    {
      moveIMotor(0, 1);
      splitIC++;
      if (splitIC > 700)
      {
        splitIC = 0;
        splitI = true;
      }
    }
  }
  else if (!insertSensor)
  {
    // Sensor zwart
    onIBlackC++;
    if (splitI) {
      moveIMotor(speedI, 1);
      splitIC++;
      if (splitIC > 200)
      {
        splitI = false;
      }
    }
    else
    {
      moveIMotor(0, 1);
      splitIC++;
      if (splitIC > 700)
      {
        splitIC = 0;
        splitI = true;
      }
    }
  }
  if (onIWhiteC == 500)
  {
    iDone = true;
    splitI = true;
  }
}

void moveXMotor(int speed, boolean dir)
{
  analogWrite(trainMotor, speed);
  digitalWrite(trainEnable, dir);
}

void moveYMotor(int speed, boolean dir)
{
  analogWrite(rackMotor, speed);
  digitalWrite(rackEnable, dir);
}

void moveIMotor(int speed, boolean dir)
{
  analogWrite(insertMotor, speed);
  digitalWrite(insertEnable, dir);
}

boolean readSensor(int pin, int value)
{
  char A = A;
  return (analogRead(A + pin) < value);
}

void moveCoor(int x, int y)
{
  while (!yDone || !xDone)
  {
    moveYCoor(y);
    moveXCoor(x);
  }
  resetVars();
}

void empty(int x, int box)
{
  while (!yDone || !xDone)
  {
    moveYCoor(1);
    moveXCoor(x);
  }
  while (!iDone)
  {
    emptyPackage(box);
  }
  resetVars();
}

void resetVars()
{
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

void resetRobot()
{
  moveYMotor(100, 0);
  moveXMotor(110, 0);
  delay(2500);
  moveYMotor(0, 0);
  moveXMotor(0, 0);
  extractPackage();
}

//COMMUNICATION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ExecuteCommand()
{
  String extraInfo1 = "";
  String extraInfo2 = "";
  int amountOfInfo = 0;

  for (int i = 0; i < sizeof(curCom); i++)
  {
    if (curCom.charAt(i))
    {
      amountOfInfo++;
    }
  }

  if (amountOfInfo > 0)
  {
    extraInfo1 = curCom.substring(curCom.indexOf("[") + 1, curCom.indexOf("]") );

    if (amountOfInfo > 1)
    {
      extraInfo2 = curCom.substring(curCom.indexOf("]") + 2, curCom.lastIndexOf("]"));
    }
  }
  curCom = curCom.substring(0, curCom.indexOf("["));

  //Switch between commands
  if (curCom == "cmdSendTest")
  {
    Serial.println("Pong!");
    hasTaskEnded = true;
  }
  else if (curCom == "cmdDoCycle")
  {
    if (hasTaskEnded == false && robotDidDone == false)
    {
      DoCycle(curCom, extraInfo1, extraInfo2);
    }

    if (robotDidDone == true)
    {
      Serial.println("All Done :)");
      // resetRobot();
      hasTaskEnded = true;
    }
  }
  else if (curCom == "cmdMoveXAxis")
  {
    hasTaskEnded = true;
  }
  else if (curCom == "cmdMoveYAxis")
  {
    hasTaskEnded = true;
  }
  else if (curCom == "cmdMoveCoord")
  {
    hasTaskEnded = true;
  }
  else if (curCom == "cmdGetPackage")
  {
    hasTaskEnded = true;
  }
  else if (curCom == "cmdUnloadPackage")
  {
    hasTaskEnded = true;
  }
  else if (curCom == "cmdDumpPackage")
  {
    hasTaskEnded = true;
  }
  else
  {
    //If the given command is not in the list, reset
    Serial.println("Command not recognized");
    hasTaskEnded = true;
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
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

void resetCommandArray()
{
  for (int i = 0; i < maxArraySize; i++)
  {
    commandArray[i] = "";
    extraInfoArray[i] = "";
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
      extraInfoArray[i - 1] = extraInfoArray[i];
    }

    commandArray[commandIndex] = "";
    extraInfoArray[commandIndex] = "";
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
        Serial.println("Received Data");
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

void DoCycle(String curCom, String extraInfo1, String extraInfo2)
{
  //Actually do the cycle
  Serial.println("Doing the cycle");
  Serial.println("Extra info1 = " + extraInfo1);
  Serial.println("Extra info2 = " + extraInfo2);

  int amountOfCoords = 0;
  int amountOfBoxes = 1;

  for (int i = 0; i < extraInfo1.length(); i++)
  {
    if (extraInfo1.charAt(i) == '@')
    {
      amountOfCoords++;
    }
  }

  for (int i = 0; i < extraInfo2.length(); i++)
  {
    if (extraInfo2.charAt(i) == '!')
    {
      amountOfBoxes++;
    }
  }

  amountOfCoords += 1;

  int coor[amountOfCoords][2];
  int drop[amountOfBoxes];
  Serial.println("Amount of boxes: " + String(amountOfBoxes, DEC));

  Serial.println("Amount of coords: " + String(amountOfCoords, DEC));

  for (int i = 0; i < amountOfCoords; i++)
  {
    String coord = getValue(extraInfo1, '@', i);
    if (coord != "")
    {
      Serial.println("Coord at : " + coord.substring(0, 1) + "." + coord.substring(2, 3));

      coor[i][0] = (coord.substring(0, 1)).toInt();
      coor[i][1] = (coord.substring(2, 3)).toInt();
    }
  }

  for (int i = 0; i < amountOfBoxes; i++)
  {
    String box = getValue(extraInfo2, '!', i);
    if (box != "")
    {
      //        drop[i] = box[0];
      Serial.println("box at : " + (box.substring(0, 1)));
      drop[i] =  (box.substring(0, 1)).toInt();
    }
  }

  Serial.println("Starting the robot");
  runPickandDrop(coor, drop, amountOfCoords + 1);
}

