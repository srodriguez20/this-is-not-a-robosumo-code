#include <SPI.h>
#include "RF24.h"

RF24 radio(9, 10);                // define the object to control NRF24L01
byte addresses[5] = "00007";      // define communication address which should correspond to remote control
int data[9]={512, 512, 0, 0, 1, 0, 512, 512, 512};  // define array used to save the communication data
//0 x axis right joystick
//1 y axis left joystick
//2 button A
//3 button B
//4 button C
//5 button D
//6 como que nada
//7 como que tampoco
//8 y axis right joystick
int mode[1];

int trigPin = 0;                  // define Trig pin for ultrasonic ranging module
int echoPin = 1;                  // define Echo pin for ultrasonic ranging module
float maxDistance = 200;          // define the range(cm) for ultrasonic ranging module, Maximum sensor distance is rated at 400-500cm.
float soundVelocity = 340;        // Sound velocity = 340 m/s
float rangingTimeOut = 2 * maxDistance / 100 / soundVelocity * 1000000; // define the timeout(ms) for ultrasonic ranging module

#define FORWARD HIGH
#define BACKWARD LOW

const int dirAPin = 7;    // define pin used to control rotational direction of motor A
const int pwmAPin = 6;    // define pin for PWM used to control rotational speed of motor A
const int dirBPin = 4;    // define pin used to control rotational direction of motor B
const int pwmBPin = 5;    // define pin for PWM used to control rotational speed of motor B
const int snsAPin = 0;    // define pin for detecting current of motor A
const int snsBPin = 1;    // define pin for detecting current of motor B

const int infra1 = A0;
const int RPin = A3; 
const int GPin = A4; 
const int BPin = A5;

int RGBVal = 0;
int automatic = 0;

int infraValue1 = 0;
int infraValue2 = 0;
int infraMaxValue = 90;
#define FORWARD LOW
#define BACKWARD HIGH

void setup() {
  Serial.begin(9600);
  radio.begin();                      // initialize RF24
  radio.setRetries(0, 15);            // set retries times
  radio.setPALevel(RF24_PA_LOW);      // set power
  radio.openReadingPipe(1, addresses);// open delivery channel
  radio.startListening();             // start monitoring

  pinMode(dirAPin, OUTPUT);   // set dirAPin to output mode
  pinMode(pwmAPin, OUTPUT);   // set pwmAPin to output mode
  pinMode(dirBPin, OUTPUT);   // set dirBPin to output mode
  pinMode(pwmBPin, OUTPUT);   // set pwmBPin to output mode
  
  pinMode(infra1, INPUT);
  pinMode(RPin, OUTPUT);   // set RPin to output mode
  pinMode(GPin, OUTPUT);   // set GPin to output mode
  pinMode(BPin, OUTPUT);   // set BPin to output mode

  pinMode(trigPin, OUTPUT); // set trigPin to output mode
  pinMode(echoPin, INPUT);  // set echoPin to input mode
}

void loop()
{
    receiveData();
    mode[0]=0;
    // radio.write( mode, sizeof(mode) );
    // calculate the steering angle of servo according to the direction joystick of remote control and the deviation
    infraValue1= analogRead(infra1);
    Serial.println(infraValue1);
    
    int motorSpdA = data[8] - 512;
    bool motorDirA = motorSpdA < 0 ? BACKWARD : FORWARD;
    motorSpdA = abs(constrain(motorSpdA, -512, 512));
    motorSpdA = map(motorSpdA, 0, 512, 0, 255);

    int motorSpdB = data[1] - 512;
    bool motorDirB = motorSpdB < 0 ? BACKWARD : FORWARD;
    motorSpdB = abs(constrain(motorSpdB, -512, 512));
    motorSpdB = map(motorSpdB, 0, 512, 0, 255);

    
    // control the steering and travelling of the smart car
    ctrlCar0( motorDirA, motorSpdA, motorDirB,motorSpdB);
    switch(RGBVal){
      case 0: digitalWrite(RPin, HIGH);digitalWrite(GPin, HIGH);digitalWrite(BPin, HIGH);break;
      case 1: digitalWrite(RPin, LOW);digitalWrite(GPin, LOW);digitalWrite(BPin, LOW);   break;
      case 2: digitalWrite(RPin, LOW);digitalWrite(GPin, HIGH);digitalWrite(BPin, HIGH); break;
      case 3: digitalWrite(RPin, HIGH);digitalWrite(GPin, LOW);digitalWrite(BPin, HIGH); break;
      case 4: digitalWrite(RPin, HIGH);digitalWrite(GPin, HIGH);digitalWrite(BPin, LOW); break;
      default: break;
    }
  
}
void receiveData(){
   if ( radio.available()) {             // if receive the data
    while (radio.available()) {         // read all the data
      radio.read( data, sizeof(data) ); // read data
    }
    if(!data[3]){
      automatic = 0;
    }
    if(!data[4]){
      automatic = 1;
    }
   }
   else{
      digitalWrite(dirAPin, FORWARD);
      analogWrite(pwmAPin, 0);
      digitalWrite(dirBPin, FORWARD);
      analogWrite(pwmBPin, 0);
    }
}
void ctrlCar0( bool motorDirA, byte motorSpdA, bool motorDirB, byte motorSpdB) {
  digitalWrite(dirAPin, motorDirA);
  if(automatic == 1 && infraValue1<infraMaxValue && motorDirA == BACKWARD )
  {
   analogWrite(pwmAPin, 0);
  }
  else{
   analogWrite(pwmAPin, motorSpdA);
  }
  
  
  digitalWrite(dirBPin, motorDirB);
  if(automatic == 1 && infraValue1<infraMaxValue && motorDirB == BACKWARD )
  {
   analogWrite(pwmAPin, 0);
  }
  else{
   analogWrite(pwmBPin, motorSpdB);
  }
  
}
