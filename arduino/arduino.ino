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

int iLoc;
boolean onIWhite;
boolean onILoc;
boolean iDone;
int speedI;
int onIWhiteC;
int onIBlackC;



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
  {1,1},
  {2,2},
  {3,3},
  {4,4},
  {5,5},
  };
  int drop[5][2] = {
  {5,2},
  {4,3},// X Item number
  {3,4},
  {2,5},
  {1,6},//first item picked up
  };
  runPickandDrop(coor,drop);
  
}

void loop() {
}

void runPickandDrop(int coor[5][2],int drop[5][2]){
  for(int coorN=0;coorN<5;coorN++){
    int x=coor[coorN][0];
    int y=coor[coorN][1];
    moveCoor(x,y);
    delay(300);
    extractPackage();
  }
  for(int dropN=0;dropN<5;dropN++){
    int x=drop[dropN][0];
    int box=drop[dropN][1];
    empty(x,box);
    delay(300);
  }
}

void moveXCoor(int coor) {
  if(xDone||(startXLoc==0&&xLoc==coor)){xDone = true;
  }else{
  int trainSensor = readSensor(0, 150);
  
  speedX = 110;
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
    onXBlackC=0;
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
      moveXMotor(200, !dirX);
      delay(brakeSpeed+10);
    }
    moveXMotor(0, dirX);
    onXWhite = true;
    onXBlackC=0;
    onXWhiteC++;
  } else if (!trainSensor && onXLoc) { //Sensor zwart && op locatie
    if(split){
      moveXMotor(speedX+20, !dirX);
      splitC++;
      if(splitC>100){
        split=false;
      }
    }else{
      moveXMotor(0, !dirX);
      splitC++;
      if(splitC>250){
        splitC=0;
        split=true;
      }
    }
  } else if (!trainSensor&&onXBlackC>300) { // Sensor zwart
    onXWhite = false;
    moveXMotor(speedX, dirX);
  }else if (!trainSensor) { // Sensor zwart
    onXBlackC++;
  }
  if (onXWhiteC == 1000) {
    xDone = true;
    onXWhite = true;
  }
  }
}


void moveYCoor(int coor) {
  if(yDone||(startYLoc==0&&yLoc==coor)){yDone = true;
  }else{
  int rackSensor = readSensor(4, 200);
  if (startYLoc == 0) {
    startYLoc = yLoc;
    if (coor - startYLoc > 0) {
      speedY = 120;
      dirY = true;
    } else {
      dirY = false;
      speedY = 50;
    }
  } //Zet richting correct

  if (rackSensor && !onYWhite && !onYLoc) { //Sensor Wit & Niet op wit & niet al op locatie
    onYWhite = true;
    onYBlackC=0;
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
    moveYMotor(speedY, dirY);
  } else if (rackSensor && onYLoc) { //Sensor Wit & op locatie
    moveYMotor(0, !dirY);
    onYWhite = true;
    onYBlackC=0;
    onYWhiteC++;
  } else if (!rackSensor && onYLoc && !dirY) {
    moveYMotor(120, !dirY);
  } else if (!rackSensor&&onYBlackC>300) { // Sensor zwart
    onYWhite = false;
    moveYMotor(120, dirY);
  }else if (!rackSensor) { // Sensor zwart
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
  int insertSensor = readSensor(5, 400);
  speedI = 80;
  if (insertSensor && !onIWhite && !onILoc) { //Sensor Wit & Niet op wit & niet al op locatie
    onIWhite = true;
    onIBlackC=0;
    iLoc++;
    if (iLoc != coor) { // nog niet op locatie
      moveIMotor(speedI, 1);
    } else if (iLoc == coor) { // op locatie
      onILoc = true;
      moveIMotor(0, 0);
    }
  } else if (insertSensor && onIWhite && !onILoc) { // Sensor wit & op wit & niet op locatie
    moveIMotor(speedI, 1);
  } else if (insertSensor && onILoc) { //Sensor Wit & op locatie
    moveIMotor(0, 0);
    onIWhiteC++;
  }else if (!insertSensor&&onIBlackC>150) { // Sensor zwart
    onIWhite = false;
    moveIMotor(speedI, 1);
  }else if (!insertSensor) { // Sensor zwart
    onIBlackC++;
  }
  if (onIWhiteC == 500) {
    iDone = true;
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

void moveCoor(int x,int y){
  while(!yDone||!xDone){
    moveYCoor(y);
    moveXCoor(x);
  }
  resetVars();
}

void empty(int x, int box){
  while(!yDone||!xDone){
     moveYCoor(1);
     moveXCoor(x);
  }
  while(!iDone){
    emptyPackage(box);
  }
  resetVars();
}

void resetVars() {
  xDone = false;
  onXLoc = false;
  onXWhiteC = 0;
  startXLoc = 0;
  onXBlackC=0;

  yDone = false;
  onYLoc = false;
  onYWhiteC = 0;
  startYLoc = 0;
  onYBlackC=0;

  iDone = false;
  onILoc = false;
  onIWhiteC = 0;
  onIBlackC=0;
}
void resetRobot(){
  extractPackage();
  moveYMotor(100,0);
  moveXMotor(110,0);
  delay(5000);
  moveYMotor(0,0);
  moveXMotor(0,0);
}

