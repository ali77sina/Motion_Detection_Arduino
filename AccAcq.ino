/*
  Arduino LSM9DS1 - Simple accelerometer

  This example reads the accelerometer values from the LSM9DS1
  sensor and prints them to the Serial Monitor
  or Serial Plotter in bursts of 90 data points

  The circuit:
  - Arduino Nano 33 BLE Sense

  created 10 Jul 2019
  by Riccardo Rizzo

  Modifed 25 Sep 2021
  by Ali Kavoosi

  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");
  pinMode(22,OUTPUT);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

}

void loop() {
  float x, y, z;

  digitalWrite(22, HIGH);
  Serial.println("wait");
  delay(1000);
  digitalWrite(22, LOW);

  int i = 0;
  while(i<90){
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    Serial.print(x);
    Serial.print(',');
    Serial.print(y);
    Serial.print(',');
    Serial.println(z);
    i++;
  }
  }
}
