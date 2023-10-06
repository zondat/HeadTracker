
#include <Wire.h>
////////////////////// MPU 6050 //////////////////////////////////
//Measure Angle with a MPU-6050(GY-521)
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265;
int maxVal=402;
int roll, pitch, yaw;
int maxRoll = 0, maxPitch = 0, maxYaw = 0;
int minRoll = 360, minPitch = 360, minYaw = 360;
int offsetRoll = 0, offsetPitch = 0, offsetYaw = 0;

void getMotion(int* roll, int* pitch, int* yaw, int offsetRoll = 0, int offsetPitch = 0, int offsetYaw = 0) {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);

  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();

  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);

  *roll = RAD_TO_DEG * (atan2(-yAng, -zAng)+PI) - offsetRoll;
  *pitch = RAD_TO_DEG * (atan2(-xAng, -zAng)+PI) - offsetPitch;
  *yaw = RAD_TO_DEG * (atan2(-yAng, -xAng)+PI) - offsetYaw;
}

void calibrate() {
getMotion(&offsetRoll, &offsetPitch, &offsetYaw);

  Serial.println("Rotate around X");
  for (int i=0; i<100; i++) {
    delay(25);
    getMotion(&roll, &pitch, &yaw);
    if (roll > maxRoll) maxRoll = roll;
    if (roll < minRoll) minRoll = roll;
  }
  maxRoll = maxRoll - offsetRoll;
  minRoll = minRoll - offsetRoll;
  Serial.println("Min, max roll: " + String(minRoll) + ", " + String(maxRoll));

  delay(1000);
  Serial.println("Rotate around Y");
  for (int i=0; i<100; i++) {
    delay(25);
    getMotion(&roll, &pitch, &yaw);
    if (pitch > maxPitch) maxPitch = pitch;
    if (pitch < minPitch) minPitch = pitch;
  }
  maxPitch = maxPitch - offsetPitch;
  minPitch = minPitch - offsetPitch;
  Serial.println("Min, max pitch: " + String(minPitch) + ", " + String(maxPitch));

  delay(1000);
  Serial.println("Rotate around Z");
  for (int i=0; i<100; i++) {
    delay(25);
    getMotion(&roll, &pitch, &yaw);
    if (yaw > maxYaw) maxYaw = yaw;
    if (yaw < minYaw) minYaw = yaw;
  }
  maxYaw = maxYaw - offsetYaw;
  minYaw = minYaw - offsetYaw;
  Serial.println("Min, max yaw: " + String(minYaw) + ", " + String(maxYaw));
  delay(1000);
}

//////////////////////CONFIGURATION///////////////////////////////
#include "PPMEncoder.h"
#define CHANNEL_NUMBER 8  //set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1500  //set the default servo value
#define CHANNEL_MAX_VALUE 2000
#define CHANNEL_MIN_VALUE 1000
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 500  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 10  //set PPM signal output pin on the arduino

/*this array holds the servo values for the ppm signal
 change theese values in your code (usually servo values move between 1000 and 2000)*/
int ppm[CHANNEL_NUMBER];

void setup(){    
  // Mpu6050 settings
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  calibrate();

  //initiallize default ppm values
  for(int i=0; i<CHANNEL_NUMBER; i++){
      ppm[i]= CHANNEL_DEFAULT_VALUE;
  }

  // PPM encoder
  ppmEncoder.begin(sigPin);

  // pinMode(sigPin, OUTPUT);
  // writePPM();
}

void loop(){

  getMotion(&roll, &pitch, &yaw, offsetRoll, offsetPitch, offsetYaw);

  long ppmRoll = map(roll, minRoll, maxRoll, CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE);
  long ppmPitch = map(pitch, minPitch, maxPitch, CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE);
  long ppmYaw = map(yaw, minYaw, maxYaw, CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE);

  ppmEncoder.setChannel(0, ppmRoll);
  ppmEncoder.setChannel(1, ppmPitch);
  ppmEncoder.setChannel(2, ppmYaw);  
  for (int i=3; i<CHANNEL_NUMBER; i++) {
    ppmEncoder.setChannel(i, CHANNEL_DEFAULT_VALUE);
  }
}

