#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
sensors_event_t accel, gyro, temp;
#define ANGLE_RATE_MAX 5 // rad/s
#define ANGLE_RATE_MIN -5 // rad/s

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
  // Serial.begin(9600);

  // Mpu6050 settings
  while (!mpu.begin()) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_184_HZ);

  //initiallize default ppm values
  for(int i=0; i<CHANNEL_NUMBER; i++){
      ppm[i]= CHANNEL_DEFAULT_VALUE;
  }

  // PPM encoder
  ppmEncoder.begin(sigPin);
}

void loop(){
  mpu.getEvent(&accel, &gyro, &temp);

  ppm[0] = map(gyro.orientation.roll, ANGLE_RATE_MIN, ANGLE_RATE_MAX, CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE);
  ppm[1] = map(gyro.orientation.pitch, ANGLE_RATE_MIN, ANGLE_RATE_MAX, CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE);
  ppm[2] = map(gyro.orientation.heading, ANGLE_RATE_MIN, ANGLE_RATE_MAX, CHANNEL_MIN_VALUE, CHANNEL_MAX_VALUE);

  for (int i=3; i<CHANNEL_NUMBER; i++) {
    ppmEncoder.setChannel(i, ppm[i]);
  }
}

