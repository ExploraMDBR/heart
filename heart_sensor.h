/*
  Optical Heart Rate Detection (PBA Algorithm) using the MAX30105 Breakout
  By: Nathan Seidle @ SparkFun Electronics
  Date: October 2nd, 2016
  https://github.com/sparkfun/MAX30105_Breakout

  This is a demo to show the reading of heart rate or beats per minute (BPM) using
  a Penpheral Beat Amplitude (PBA) algorithm.

  It is best to attach the sensor to your finger using a rubber band or other tightening
  device. Humans are generally bad at applying constant pressure to a thing. When you
  press your finger against the sensor it varies enough to cause the blood in your
  finger to flow differently which causes the sensor readings to go wonky.

  Hardware Connections (Breakoutboard to Arduino):
  -5V = 5V (3.3V is allowed)
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  The MAX30105 Breakout can handle 5V or 3.3V I2C logic. We recommend powering the board with 5V
  but it will also run at 3.3V.
*/

//https://lastminuteengineers.com/max30102-pulse-oximeter-heart-rate-sensor-arduino-tutorial/

#include "types.h"

#define REMOTE_HEART_MIN_DELAY 200

long lastBeat = 0; //Time at which the last beat occurred

#ifdef REMOTE_HEART_MIN_DELAY // Sse digital pins to get the sensor state from other Arduino

#define PRECENSE_OUT_PIN 8
#define BEAT_OUT_PIN 9

bool heart_init() {
  pinMode(PRECENSE_OUT_PIN, INPUT);
  pinMode(BEAT_OUT_PIN, INPUT);
  return true;
}

Beat_State check_beat() {
  int precense = digitalRead(PRECENSE_OUT_PIN);
  int beat = digitalRead(BEAT_OUT_PIN);

  if (beat) {
    long delta = millis() - lastBeat;
    if (delta >= REMOTE_HEART_MIN_DELAY) {

      lastBeat = millis();
      return Beat_State::BEAT;
    }
  } 
  if (precense){
    return Beat_State::PRECENSE;
  }

  return Beat_State::NO;
}

#else // Use this arduino to read the sensor

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"


MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;


float beatsPerMinute;
int beatAvg;

#define PRECENSE_THRESHOLD 50000

bool heart_init() {
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    return false;
  }

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  return true;

}


Beat_State check_beat() {
  Beat_State result = Beat_State::NO;
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    if (irValue > PRECENSE_THRESHOLD) {
      result = Beat_State::BEAT;
    }

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  } else if (irValue > PRECENSE_THRESHOLD) {
    result = Beat_State::PRECENSE;
  }

  return result;
}

#endif //end REMOTE_HEART_MIN_DELAY
