#include <FastLED.h>
#include <stdint.h>
FASTLED_USING_NAMESPACE

#define HEART_PIN    2
#define VEIN_PIN    4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define HUE_RED_HIGHER HUE_RED + 255

#define BRIGHTNESS         100
#define FRAMES_PER_SECOND  120

#define VEIN_COLOR_TRANSITION_SHRINK 8 // Normally 2 < val < 8

// --- Blood Speed controls
//#define HOLE_TAIL 15
#define PUMP_SPEED_PEAK 2
#define PUMP_SPEED_DECAY 0.2
#define PUMP_SPEED_BASELINE 0.1

// --- Blood valley dim controls
#define SMOOTH_FACTOR 0.5
#define MIN_BRIGHT 40
#define BRIGHT_RAISE_AFTER_BEAT 4

// --- Blood peaks scale
#define BLOOD_PEAKS_SCALE 2.5f //needs to be float!

#define PRESENCE_HUE_SHIFT -15

///------------- HEART LEDS

/// ------------ END HEART LEDS


CRGB get_vein_color(const uint8_t _i, const uint8_t _start, const uint8_t _end,
                    const uint8_t _bright = 255, const uint8_t _shift = 0) {
  if (_i < _start) {
    return CHSV(HUE_BLUE + _shift, 255, _bright);
  } else if ( _i > _end ) {
    return CHSV(HUE_RED + _shift, 255, _bright);
  }
  int _hue = map( _i, _start, _end, HUE_BLUE, HUE_RED_HIGHER);
  return CHSV(_hue + _shift, 255, _bright);
}

void show_leds() {
  FastLED.show();
}

char myASCII[] = ".,:+iwW#";

char bright_to_ascii(float b) {
  return myASCII[static_cast<int>(static_cast<float>(sizeof(myASCII) - 1) * b)];
}

void led_setup(CRGB *veinleds, int veinnum, CRGB *heartleds, int heartnum) {
  FastLED.addLeds<LED_TYPE, VEIN_PIN, COLOR_ORDER>(veinleds, veinnum).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, HEART_PIN, COLOR_ORDER>(heartleds, heartnum).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

#define HEART_LED_BASE_BRIGHT 100
void pump_heart(CRGB *heartleds, int heartnum, float _pump_speed, float precense_effect) {
  static float led_val = HEART_LED_BASE_BRIGHT;
  if (_pump_speed <= PUMP_SPEED_PEAK / 2) {
    led_val = max(led_val - 50, HEART_LED_BASE_BRIGHT);
  } else {
    led_val = min(255, led_val + _pump_speed * 50);

  }
  //  Serial.println(led_val);


  for (int i = 0 ; i < heartnum ; i++) {
    int bright_val = led_val;
    if (precense_effect) {
      int randNumber = random(100);
      if (randNumber > 50) {
        bright_val -= randNumber * precense_effect;
      }
      heartleds[i] = CHSV(HUE_RED+PRESENCE_HUE_SHIFT, 255, bright_val);
    } else {
      heartleds[i] = CHSV(HUE_RED, 255, bright_val);
    }
    
  }
}


template <int NUM, int OFFSET = 0>
struct Vein {
  CRGB *leds;
  static bool controller_setup;
  float smooth_pump_intensity;
  float hole;
  const int TRANSITION_START, TRANSITION_END;
  Vein(CRGB *_leds):
    hole(0),
    smooth_pump_intensity(0),
    TRANSITION_START((NUM / 2) - max(2, (NUM / VEIN_COLOR_TRANSITION_SHRINK))),
    TRANSITION_END((NUM / 2) + max(2, (NUM / VEIN_COLOR_TRANSITION_SHRINK)))
  {
    leds = _leds;
  }

  void start() {
    int rand = random(255);
    for (int i = 0 ; i < NUM ; i++) {
      leds[i + OFFSET] = CHSV(rand, 255, 255);
    }
    FastLED.show();
  }

  //  int dist_behind(const uint16_t _i, const uint16_t _p) {
  //    int dist = min(_p % NUM - ((_i + NUM) % NUM), HOLE_TAIL) * 255 / HOLE_TAIL;
  //    return (dist < 0) ? 255 : dist;
  //  }

  void update(float _pump_vel, float precense_effect) {
    float pump_intensity = (_pump_vel - PUMP_SPEED_BASELINE) / (PUMP_SPEED_PEAK - PUMP_SPEED_BASELINE);
    smooth_pump_intensity = smooth_pump_intensity * SMOOTH_FACTOR + pump_intensity * (1 - SMOOTH_FACTOR);

    for (int i = 0 ; i < NUM ; i++) {
      // get bright for hole and tail
      uint8_t bright = max( MIN_BRIGHT , sin((hole + i) / BLOOD_PEAKS_SCALE) * 256);
      uint8_t shift = 0;

      // dim hole and tail when speed low
      bright = (bright == 255) ? 255 : min(255, bright * BRIGHT_RAISE_AFTER_BEAT * (1 - smooth_pump_intensity));
      if (precense_effect) {
        int randNumber = random(255);
        if (randNumber > 200) {
          bright -= randNumber * precense_effect;

        }
        shift = PRESENCE_HUE_SHIFT * precense_effect;
      }


      //      Serial.print(bright_to_ascii(bright / 256.0));

      leds[i + OFFSET] = get_vein_color(i, TRANSITION_START, TRANSITION_END, bright, shift);
    }
    //    Serial.print("  | ORIGINAL:  ");
    //    Serial.print(pump_intensity);
    //    Serial.print("  | SMOOTH:  ");
    //    Serial.print(smooth_pump_intensity);
    //    Serial.println();
    hole += _pump_vel;
  }
};
