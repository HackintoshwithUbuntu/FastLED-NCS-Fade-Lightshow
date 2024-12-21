#include <FastLED.h>

#define NUM_LEDS  18
#define LED_PIN   2

CRGB leds[NUM_LEDS];

uint8_t currentled = 0;

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  Serial.begin(57600);
}

void loop() {

  EVERY_N_SECONDS(4) {
    leds[currentled % NUM_LEDS] = CRGB::Blue;
    currentled += 4;
  }

  FastLED.show();
}

