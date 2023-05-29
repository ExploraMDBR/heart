
#include "heart_sensor.h"
#include "vein.h"
#include "Sound.h"

int count = 0;
bool _detected = false;

// Vein leds stuff
#define PUMP_BPM 60
#define NUM_LEDS    60

float _pump_vel = 0;
unsigned long _last_pump = 0;
constexpr int _pump_delay = static_cast<int>(60 * 1000.0 / PUMP_BPM);

CRGB leds[NUM_LEDS];

Vein < NUM_LEDS / 2, 0 > vAB = Vein < NUM_LEDS / 2, 0 > (leds);
Vein < NUM_LEDS / 2, NUM_LEDS / 2 > vCD = Vein < NUM_LEDS / 2, NUM_LEDS / 2 > (leds);
// end Vein leds stuff

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //  if (!heart_init()) {
  //    Serial.println(HEART_SENSOR_NOT_FOUND);
  //    while (1);
  //  }
  //  Serial.println(HEART_SENSOR_OK);

  init_mp3();

  delay(1000); // 1 second delay for recovery
  led_setup(leds, NUM_LEDS);
  vAB.start();
  delay(300);
  vCD.start();
  delay(300);

}

const char symbols[4] = {'~', '-', '.', '='};

void loop() {
  Beat_State beat_state = check_beat();

  switch (beat_state) {
    case Beat_State::BEAT:
      digitalWrite(LED_BUILTIN, HIGH);
      _pump_vel = PUMP_SPEED_PEAK;
      break;
    case Beat_State::PRECENSE:
      Serial.print(symbols[count++ % 4]);

      if (!_detected) {
        _detected = true;
        play_wait();
        Serial.println("-----wait-----");
      }
      break;
    case Beat_State::NO:
      Serial.print('%');
      _detected = false;
      stop_sounds();
      break;
  }
  if (count > 30) {
    count = 0;
    Serial.println('#');
  }

  if (millis() >= lastBeat + KEEP_LED ) {
    digitalWrite(LED_BUILTIN, LOW);
  }


  //  unsigned long now = millis();
  //  if (now > _last_pump + _pump_delay) {
  //    _pump_vel = PUMP_SPEED_PEAK;
  //    _last_pump = now;
  //  }

  vAB.update(_pump_vel);
  vCD.update(_pump_vel);

  _pump_vel = max(PUMP_SPEED_BASELINE, _pump_vel - PUMP_SPEED_DECAY);

  show_leds();
  delay(50);
  if (beat_state == Beat_State::BEAT) {
    play_beat();
    Serial.println("-----BEAT-----");
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
