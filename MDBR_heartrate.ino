
#include "heart_sensor.h"
#include "Sound.h"

int count = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);


  if (!heart_init()) {
    Serial.println(HEART_SENSOR_NOT_FOUND);
    while (1);
  }
  Serial.println(HEART_SENSOR_OK);

  init_mp3();
}

void loop() {
  Beat_State beat_state = check_beat();

  switch (beat_state) {
    case Beat_State::BEAT:
      digitalWrite(LED_BUILTIN, HIGH);
      play_beat();
      break;
    case Beat_State::PRECENSE:
      if (count++ % 8 == 0) {
        Serial.println("+");
      }
      break;
  }
  if (millis() >= lastBeat + KEEP_LED ) {
    digitalWrite(LED_BUILTIN, LOW);
  }
}


//void old_loop() {
//
//  //  long irValue = particleSensor.getIR();
//  //
//  //  if (checkForBeat(irValue) == true)
//  //  {
//  //    //We sensed a beat!
//  //    long delta = millis() - lastBeat;
//  //    lastBeat = millis();
//  //
//  //    if (irValue > 50000) {
//  //      Serial.print(", BPM=");
//  //      Serial.print(beatsPerMinute);
//  //      Serial.print(", Avg BPM=");
//  //      Serial.print(beatAvg);
//  //      Serial.println(" ------ <3");
//  //      digitalWrite(LED_BUILTIN, HIGH);
//  //      play_beat();
//  //    }  else {
//  //      Serial.println(">");
//  //    }
//  //
//  //    beatsPerMinute = 60 / (delta / 1000.0);
//  //
//  //    if (beatsPerMinute < 255 && beatsPerMinute > 20)
//  //    {
//  //      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
//  //      rateSpot %= RATE_SIZE; //Wrap variable
//  //
//  //      //Take average of readings
//  //      beatAvg = 0;
//  //      for (byte x = 0 ; x < RATE_SIZE ; x++)
//  //        beatAvg += rates[x];
//  //      beatAvg /= RATE_SIZE;
//  //    }
//  //  } else if (count++ % 8 == 0) {
//  //    if (irValue > 50000)
//  //      //      Serial.println(String("+ ") + lastBeat + " | " + (lastBeat + KEEP_LED) + " | " +  millis());
//  //      Serial.println("+");
//  //    else
//  //      Serial.println(".");
//  //  }
//  //
//  //  if (millis() >= lastBeat + KEEP_LED ) {
//  //    digitalWrite(LED_BUILTIN, LOW);
//  //  }
//
//  //  Serial.print("IR=");
//  //  Serial.print(irValue);
//  //  Serial.print(", BPM=");
//  //  Serial.print(beatsPerMinute);
//  //  Serial.print(", Avg BPM=");
//  //  Serial.print(beatAvg);
//
//  //  if (irValue < 50000)
//  //    Serial.println(" ------ <3");
//  //  else
//
//  //  Serial.println();
//
//}
