int trainMotor=5;
int trainEnable=4;

int rackMotor=10;
int rackEnable=11;

int conveyorMotor;
int conveyorEnable;

int insertMotor=9;
int insertEnable=8;

boolean onXWhite;
int onXWhiteC;
boolean onXLoc;
boolean dirXSwitch;
int startXLoc=0;
int xLoc=0;
boolean xDone;
boolean dirX;

boolean onYWhite;
int onYWhiteC;
boolean onYLoc;
boolean dirYSwitch;
int startYLoc=0;
int yLoc=0;
boolean yDone;
boolean dirY;
int speedY;

void setup() {
//  while(true){};
  // Setup Motor Pins
  for(int i=4;i<12;i++){
    pinMode(i,OUTPUT);
  }
  pinMode(A0,INPUT);
  pinMode(A4,INPUT);
  Serial.begin(9600);
  
  while(true){
  while(!yDone||!xDone){
    moveXCoor(2);
    moveYCoor(2);
    delay(5);
    Serial.println(yLoc);
  }
  xDone=false;
  onXLoc=false;
  onXWhiteC=0;
  startXLoc=0;
  yDone=false;
  onYLoc=false;
  onYWhiteC=0;
  startYLoc=0;
  delay(1000);
  
  while(!xDone||!yDone){
    moveYCoor(1);
    moveXCoor(1);
    delay(5);
    Serial.println(yLoc);
  }
  xDone=false;
  onXLoc=false;
  onXWhiteC=false;
  startXLoc=0;
  yDone=false;
  onYLoc=false;
  onYWhiteC=false;
  startYLoc=0;
  delay(1000);
  
  while(!xDone||!yDone){
    moveYCoor(3);
    moveXCoor(3);
    delay(5);
    Serial.println(yLoc);
  }
  xDone=false;
  onXLoc=false;
  onXWhiteC=false;
  startXLoc=0;
  yDone=false;
  onYLoc=false;
  onYWhiteC=false;
  startYLoc=0;
  delay(1000);
  }
}

void loop() {
}

void moveXCoor(int coor){
  int trainSensor = readSensor(0);
  
  if(startXLoc==0){
    startXLoc=1;
    Serial.println(coor-startXLoc);
    if(coor-xLoc>0){
      dirX=true;
    }else{
      dirX=false;
    }
   } 
   //Zet richting correct

  if(trainSensor&&!onXWhite&&!onXLoc){ //Sensor Wit & Niet op wit & niet al op locatie
    onXWhite=true;
    
    if(dirX){xLoc++;}else{xLoc--;} //Tel op of af aan de hand van richting.
    
    if(xLoc!=coor){  // nog niet op locatie
      moveXMotor(95,dirX);
    }else if(xLoc==coor){  // op locatie
      onXLoc=true;
      moveXMotor(0,!dirX);
    }
    
  }else if(trainSensor&&onXWhite&&!onXLoc){ // Sensor wit & op wit & niet op locatie
    moveXMotor(95,dirX);
  }else if(trainSensor&&onXLoc){  //Sensor Wit & op locatie
    if(onXWhiteC<1){
      moveXMotor(230,!dirX);
      delay(40);
    }
    moveXMotor(0,!dirX);
    onXWhite=true;
    onXWhiteC++;
  }else if(!trainSensor&&onXLoc){  //Sensor zwart && op locatie
    moveXMotor(90,!dirX);
  }else if(!trainSensor){  // Sensor zwart
    onXWhite=false;
    moveXMotor(90,dirX);
  }
  if(onXWhiteC==500){
    xDone=true;
  }
}


void moveYCoor(int coor){
  int rackSensor = readSensor(4);
  
  if(startYLoc==0){
    startYLoc=yLoc;
    if(coor-startYLoc>0){
      speedY=120;
      dirY=true;
    }else{
      dirY=false;
      speedY=50;
    }
   } //Zet richting correct

  if(rackSensor&&!onYWhite&&!onYLoc){ //Sensor Wit & Niet op wit & niet al op locatie
    onYWhite=true;
    
    if(dirY){yLoc++;}else{yLoc--;} //Tel op of af aan de hand van richting.
    
    if(yLoc!=coor){  // nog niet op locatie
      moveYMotor(speedY,dirY);
    }else if(yLoc==coor){  // op locatie
      onYLoc=true;
      moveYMotor(0,dirY);
    }
  }else if(rackSensor&&onYWhite&&!onYLoc){ // Sensor wit & op wit & niet op locatie
    moveYMotor(speedY,dirY);
  }else if(rackSensor&&onYLoc){  //Sensor Wit & op locatie
    moveYMotor(0,!dirY);
    onYWhite=true;
    onYWhiteC++;
  }else if(!rackSensor){  // Sensor zwart
    onYWhite=false;
    moveYMotor(speedY,dirY);
  }
  if(onYWhiteC==500){
    yDone=true;
  }  
}

void moveXMotor(int speed, boolean dir){
  analogWrite(trainMotor,speed);
  digitalWrite(trainEnable, dir); 
}

void moveYMotor(int speed, boolean dir){
  analogWrite(rackMotor,speed);
  digitalWrite(rackEnable, dir); 
}

boolean readSensor(int pin){
  char A = A;
  if(analogRead(A+pin)<150){
    return true;
  }else{
    return false;
  }
}

