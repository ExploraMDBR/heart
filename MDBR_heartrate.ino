
#include "heart_sensor.h"
#include "vein.h"
#include "Sound.h"

int count = 0;
bool _detected = false;

// Vein leds stuff
#define PUMP_BPM 60
#define LEDS_AB (25+26)
#define LEDS_CD (47+49)
#define LEDS_EF (21+20)
#define LEDS_GH (48+50)

#define VEIN_NUM_LEDS    LEDS_AB+LEDS_CD+LEDS_EF+LEDS_GH
#define HEART_NUM_LEDS  70

float _pump_vel = 0;
unsigned long _last_pump = 0;
constexpr int _pump_delay = static_cast<int>(60 * 1000.0 / PUMP_BPM);

CRGB leds[VEIN_NUM_LEDS];
CRGB heartleds[HEART_NUM_LEDS];

Vein < LEDS_AB, 0 > vAB (leds);
Vein < LEDS_CD, LEDS_AB > vCD(leds);
Vein < LEDS_EF, LEDS_AB + LEDS_CD > vEF(leds);
Vein < LEDS_GH, LEDS_AB + LEDS_CD + LEDS_EF > vGH(leds);
// end Vein leds stuff

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

  delay(1000); // 1 second delay for recovery
  led_setup(leds, VEIN_NUM_LEDS, heartleds, HEART_NUM_LEDS);
  vAB.start();
  play_beat();
  delay(500);
  vCD.start();
  play_beat();
  delay(500);
  vEF.start();
  play_beat();
  delay(500);
  vGH.start();
  play_beat();
  delay(500);

}

const char symbols[4] = {'~', '-', '.', '='};
float precense_effect = 0;

void loop() {
  Beat_State beat_state = check_beat();

  switch (beat_state) {
    case Beat_State::BEAT:
      digitalWrite(LED_BUILTIN, HIGH);
      _pump_vel = PUMP_SPEED_PEAK;
      precense_effect = 0;
      break;
    case Beat_State::PRECENSE:
      Serial.print(symbols[count++ % 4]);

      if (!_detected) {
        _detected = true;
        precense_effect = 1;
        play_wait();
        Serial.println("-----Precense-----");
      }
      break;
    case Beat_State::NO:
      Serial.print('%');
      count++;
      _detected = false;
      precense_effect = max( 0, precense_effect - 0.05);
      stop_sounds();
      break;
  }
  if (count > 30) {
    count = 0;
  }

  if (millis() >= lastBeat + KEEP_LED ) {
    digitalWrite(LED_BUILTIN, LOW);
  }

  vAB.update(_pump_vel, precense_effect);
  vCD.update(_pump_vel, precense_effect);
  vEF.update(_pump_vel, precense_effect);
  vGH.update(_pump_vel, precense_effect);
  pump_heart(heartleds, HEART_NUM_LEDS, _pump_vel, precense_effect);

  _pump_vel = max(PUMP_SPEED_BASELINE, _pump_vel - PUMP_SPEED_DECAY);

  show_leds();

  if (beat_state == Beat_State::BEAT) {
    play_beat();
    Serial.println("-----BEAT-----");
  }

  delay(50);
}
