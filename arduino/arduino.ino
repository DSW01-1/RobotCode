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

boolean onYWhite;
int onYWhiteC;
boolean onYLoc;
boolean dirYSwitch;
int startYLoc = 0;
int yLoc = 0;
boolean yDone;
boolean dirY;
int speedY;

int iLoc;
boolean onIWhite;
boolean onILoc;
boolean iDone;
int speedI;
int onIWhiteC;

void setup() {
  //  while(true){};
  // Setup Motor Pins
  for (int i = 4; i < 12; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(A0, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Serial.begin(9600);

  //  while(true){
  //  while(!yDone||!xDone){
  //    moveYCoor(1);
  //    moveXCoor(2);
  //    delay(5);
  //  }
  //  extractPackage();
  //  resetVars();
  //  while(!yDone||!xDone){
  //    moveYCoor(2);
  //    moveXCoor(1);
  //    delay(5);
  //  }
  //  extractPackage();
  //  resetVars();
  //  while(!yDone||!xDone){
  //    moveYCoor(3);
  //    moveXCoor(2);
  //    delay(5);
  //  }
  //  extractPackage();
  //  resetVars();
  //  while(!yDone||!xDone){
  //    moveYCoor(4);
  //    moveXCoor(1);
  //    delay(5);
  //  }
  //  extractPackage();
  //  resetVars();
  //  while(!yDone||!xDone){
  //    moveYCoor(5);
  //    moveXCoor(2);
  //    delay(5);
  //  }
  //  extractPackage();
  //  resetVars();
  //  while(!yDone||!xDone){
  //    moveYCoor(1);
  //    moveXCoor(1);
  //    delay(5);
  //  }
  //}

  moveIMotor(80, 0);
  delay(3000);
  while (!iDone) {
    emptyPackage(2);
    delay(5);
    Serial.println(iLoc);
  }
  resetVars();
  while (!iDone) {
    emptyPackage(3);
    delay(5);
    Serial.println(iLoc);
  }
  resetVars();
  while (!iDone) {
    emptyPackage(4);
    delay(5);
    Serial.println(iLoc);
  }
  resetVars();
  while (!iDone) {
    emptyPackage(5);
    delay(5);
    Serial.println(iLoc);
  }
  resetVars();
  while (!iDone) {
    emptyPackage(6);
    delay(5);
    Serial.println(iLoc);
  }
  resetVars();
}

void loop() {
  Serial.println(analogRead(A5));
}

void moveXCoor(int coor) {
  int trainSensor = readSensor(0, 200);
  int brakeSpeed;
  speedX = 100;
  if (startXLoc == 0) {
    startXLoc = 1;
    if (coor - xLoc > 0) {
      dirX = true;
      moveXMotor(speedX, dirX);
      delay(100);
      brakeSpeed = coor - xLoc;
    } else {
      dirX = false;
      moveXMotor(speedX, dirX);
      delay(100);
      brakeSpeed = xLoc - coor;
    }
  }
  //Zet richting correct

  if (trainSensor && !onXWhite && !onXLoc) { //Sensor Wit & Niet op wit & niet al op locatie
    onXWhite = true;
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
      delay(brakeSpeed);
    }
    moveXMotor(0, !dirX);
    onXWhite = true;
    onXWhiteC++;
  } else if (!trainSensor && onXLoc) { //Sensor zwart && op locatie
    moveXMotor(speedX - 15, !dirX);
  } else if (!trainSensor) { // Sensor zwart
    onXWhite = false;
    moveXMotor(speedX, dirX);
  }
  if (onXWhiteC == 500) {
    xDone = true;
    onXWhite = true;
  }
}


void moveYCoor(int coor) {
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
    onYWhiteC++;
  } else if (!rackSensor && onYLoc && !dirY) {
    moveYMotor(100, !dirY);
  } else if (!rackSensor) { // Sensor zwart
    onYWhite = false;
    moveYMotor(speedY, dirY);
  }
  if (onYWhiteC == 500) {
    yDone = true;
  }
}

void extractPackage() {
  moveIMotor(70, 0);
  delay(2500);
  moveIMotor(0, 0);

  while (readSensor(4, 200)) {
    moveYMotor(100, 1);
    delay(100);
  }
  moveYMotor(0, 1);

  while (!readSensor(5, 300)) {
    moveIMotor(70, 1);
  }
  moveIMotor(0, 1);

  while (!readSensor(4, 200)) {
    moveYMotor(50, 0);
    delay(100);
  }
  moveYMotor(0, 0);
}

void emptyPackage(int coor) {
  int insertSensor = readSensor(5, 600);
  speedI = 60;
  if (insertSensor && !onIWhite && !onILoc) { //Sensor Wit & Niet op wit & niet al op locatie
    onIWhite = true;
    //    Serial.println(1);
    iLoc++;
    if (iLoc != coor) { // nog niet op locatie
      moveIMotor(speedI, 1);
    } else if (iLoc == coor) { // op locatie
      onILoc = true;
      moveIMotor(0, 0);
    }
  } else if (insertSensor && onIWhite && !onILoc) { // Sensor wit & op wit & niet op locatie
    moveIMotor(speedI, 1);
    //    Serial.println(2);
  } else if (insertSensor && onILoc) { //Sensor Wit & op locatie
    moveIMotor(0, 0);
    //    Serial.println(3);
    onIWhiteC++;
  } else if (!insertSensor) { // Sensor zwart
    onIWhite = false;
    //    Serial.println(4);
    moveIMotor(speedI, 1);
  }
  if (onIWhiteC == 300) {
    //    Serial.println(5);
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
void resetVars() {
  xDone = false;
  onXLoc = false;
  onXWhiteC = 0;
  startXLoc = 0;

  yDone = false;
  onYLoc = false;
  onYWhiteC = 0;
  startYLoc = 0;

  iDone = false;
  onILoc = false;
  onIWhiteC = 0;
}

