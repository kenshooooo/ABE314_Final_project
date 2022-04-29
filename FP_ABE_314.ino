
/* Name:                Haoyue Li, Yuecheng Wang, Jianxiang Tao
   Class:               ABE 314
   Instructor:          Dr. Jin
   Lab Section:         001
   Date Created:        04/12/2022
   Date Submitted:      04/26/2022
   Lab Number:          001
   Lab Title:           Final project
   Sketch Description:  A robot that does self-calibration, line following, marker counting, and object detection
*/

//We used robot #10

//Initilization 
#include <QTRSensors.h>
#include <stdint.h>

// wheels
const int PWMA = 6; //left wheel
const int PWMB = 12; //right wheel

// pins
const int AIN2 = 7;
const int AIN1 = 8;
const int BIN1 = 10;
const int BIN2 = 11;
const int STBY = 9;

//line following
QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

//unltrasonic sensor 
const int leftPin[] = {3,2}; 
const int rightPin[] = {5,4}; 

//globle variable 
int numSumArray = 0;
int second = 0;
int blackOne, blackTwo, blackThree, blackFour, oneTwoDiff;
int blackLineCount = 0;
char state = 'S';

//main setup function which contains two sub setup functions
void setup() {
  Serial.begin(9600);
  setupWheel();          
  delay(500);
  setupLineFollowing();
  delay(5000);             // pause for 5 seconds before entering loop function
}

//loop funcitons will run until the power is turned off
void loop() {
  stateMachine(sensorDecision()); //state machine function
  timer();                        //timer
 
}

//function for running the robot forward
void motorsForward(){
  
  //error detection for line following and claculation for rotational speed
  int position = linePos();
  int errorVal = 3500 - position;
  double rotSpeed = (0.015 * errorVal);

  //printing for data collection
  Serial.print(position);
  Serial.print(',');
  Serial.print(17);
  Serial.print(',');
  Serial.println(rotSpeed);

  driver(17, rotSpeed); //call driver function 
  }


//function for making the robot stop 
void motorsStop() {
  driver(0,0);         //call driver function 
  }

//function for motors acceleration
void motorsAccelerate() {

  //intentionally do not put it into the for loop to save some computational time
  int position = linePos();
  int errorVal = 3500 - position;
  double rotSpeed = (0.01 * errorVal);

  //printing for data collection
  Serial.print(position);
  Serial.print(',');
  Serial.print(17);
  Serial.print(',');
  Serial.println(rotSpeed);

  //increase the linear speed gradually 
  for (int i = 5; i < 17; i++){
      driver(i, rotSpeed);      //call driver function
      }
  }

//function for motors deacceleration
void motorsDeaccelerate(){
  
  //intentionally do not put it into the for loop to save some computational time
  int position = linePos();
  int errorVal = 3500 - position;
  double rotSpeed = (0.01 * errorVal);

  //printing for data collection
  Serial.print(position);
  Serial.print(',');
  Serial.print(17);
  Serial.print(',');
  Serial.println(rotSpeed);

  //decrease the linear speed gradually 
  for (int i = 17; i > 4; i--){
      driver(i, rotSpeed);      //call driver function
      }
  }


//function for line position calculation, return an integer value
int linePos(){

  // read calibrated sensor values and obtain a measure of the line position
  // from 0 to 5000 (for a white line, use readLineWhite() instead)
  uint16_t position = qtr.readLineBlack(sensorValues);

  Serial.print(second);   //print current time index
  Serial.print(',');
  
  //print each sensor value
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print(',');
  }
  numSumArray = sumArray(sensorValues);

  return position;
  }


//function that takes in linear speed and rotational speed, and run the motors
void driver(int linSpd, int rotSpd) {

  int rws = linSpd + rotSpd;
  int lws = linSpd - rotSpd;

  digitalWrite(STBY, HIGH);
  if (rws > 0) //if right wheel speed is greater than 0, set right wheel to move forward
  {
    digitalWrite(BIN1, 1);
    digitalWrite(BIN2, 0);
  }
  else //if right wheel speed is not greater than 0, set right wheel to move reverse
  {
    digitalWrite(BIN1, 0);
    digitalWrite(BIN2, 1);
  }
  if (lws > 0) //if left wheel speed is greater than 0, set left wheel to move forward
  {
    digitalWrite(AIN1, 0);
    digitalWrite(AIN2, 1);
  }
  else //if left wheel speed is not greater than 0, set left wheel to move reverse
  {
    digitalWrite(AIN1, 1);
    digitalWrite(AIN2, 0);
  }

  analogWrite(PWMA, constrain(map(abs(lws), 0, 100, 0, 255), 0, 255)); //0-255, left wheel
  analogWrite(PWMB, constrain(map(abs(rws), 0, 100, 0, 255), 0, 255)); //0-255, right wheel

}

//sub setup function for wheels
void setupWheel() {
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  }

//sub setup function for line following 
void setupLineFollowing() {
     // configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){39,37,35,33,31,29,27,25}, SensorCount);
  qtr.setEmitterPin(23);

  delay(500);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // turn on Arduino's LED to indicate we are in calibration mode
  delay(500);

  //move forward
  for (uint16_t i = 0; i < 80; i++)
  {
    qtr.calibrate();
    driver(13,0);    
  }


  //move backward 
  for (uint16_t i = 0; i < 80; i++)
  {
    qtr.calibrate();
    driver(-13,0);    
  }

  //move forward
  for (uint16_t i = 0; i < 100; i++)
  {
    qtr.calibrate();
    driver(13,0);    
  }
  
  driver(0,0); //pause
  delay(25);
  
  digitalWrite(LED_BUILTIN, LOW); // turn off Arduino's LED to indicate we are through with calibration
  delay(1000);
  }


//function that returns long integer for obstcle distance detection from ultrasonic sensors
long customDistanceFunction(int sensor) {

  long duration;
  
  if (sensor == 0) {
    pinMode(leftPin[1], OUTPUT);
    digitalWrite(leftPin[1], LOW);
    delayMicroseconds(2);
    digitalWrite(leftPin[1], HIGH);
    delayMicroseconds(5);
    digitalWrite(leftPin[1], LOW);
  
    // The same pin is used to read the signal from the PING))): a HIGH pulse
    // whose duration is the time (in microseconds) from the sending of the ping
    // to the reception of its echo off of an object.
    pinMode(leftPin[0], INPUT);
    duration = pulseIn(leftPin[0], HIGH);

    return microsecondsToCentimeters(duration);
    }
    
  if (sensor == 1) {
    pinMode(rightPin[1], OUTPUT);
    digitalWrite(rightPin[1], LOW);
    delayMicroseconds(2);
    digitalWrite(rightPin[1], HIGH);
    delayMicroseconds(5);
    digitalWrite(rightPin[1], LOW);
  
    // The same pin is used to read the signal from the PING))): a HIGH pulse
    // whose duration is the time (in microseconds) from the sending of the ping
    // to the reception of its echo off of an object.
    pinMode(rightPin[0], INPUT);
    duration = pulseIn(rightPin[0], HIGH);

    return microsecondsToCentimeters(duration);
    
    } 
  }

//funcution that converts seconds to distance 
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}


//state machine function 
void stateMachine(int sensors) {

  switch(state) {
    case 'S':
       if (sensors == 0){
        motorsAccelerate();
        state = 'F';
        
       }else {
        motorsStop();
        state = 'S';

      }
      break;

     case 'F':
      if(sensors == 1){
        motorsDeaccelerate();
        state = 'S';
        delay(5000);
      } else if (sensors == 2){
        motorsDeaccelerate();
        state = 'S';
        delay(2000);
      } else if(sensors == 3) {
        motorsDeaccelerate();
        state = 'S';
        digitalWrite(STBY,LOW);
      } else {
        motorsForward();
        state = 'F';
        }
      break;
    }
  }


//sensor decision function that process the marker information and return different integer depending on the sensor input
int sensorDecision() {
    long rightDis, leftDis;

    leftDis = customDistanceFunction(0);
    rightDis = customDistanceFunction(1);

    //marker counting
    if (numSumArray > 3600) { //check the sum of the array
      blackLineCount++;       //add one

      if(blackLineCount == 1) {
        blackOne = second;    //store time
        }

      if(blackLineCount == 2) {
        blackTwo = second;    
        oneTwoDiff = blackTwo - blackOne;
        }

      if(blackLineCount == 3) {
        blackThree = second;
        blackFour = blackThree + (2.4 * oneTwoDiff);  //calculate the time at 24 inch
        }
    }

    //for robot not on the line
    if (sensorValues[3] < 20 && sensorValues[4] < 20 && blackLineCount >= 3) {
      return 3;
      }

    //when at 24 inch mark
    if (second == blackFour){
      return 2;
      }

    //when there is no obstcles detected
    if (leftDis > 15 && rightDis > 15) {
      return 0;
      }

    //when there is no obstcle
    if (leftDis <= 15 && rightDis <= 15) {
      return 1;
      }
    }


//function for summing up the integer in the array
int sumArray(int colorArray[]){
  int num = 0;
  for (int i=0; i<8; i++) {
    num = num + colorArray[i];
    }
  return num;
  }

//timer
void timer() {
  second++;
  delay(200);
}  
