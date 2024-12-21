#include <FastLED.h>

#define NUM_LEDS  18
#define LED_PIN   2
#define LED_TYPE  WS2812B

CRGB leds[NUM_LEDS];

uint8_t currentLed = 0;
uint8_t curStage = 0;
uint8_t curSubStage = 0;

CRGBPalette16 stage1Palette(
  // Stage 1 Colours
  0xecbfff,   // Very Light Purple
  CRGB::Red,
  CRGB::Green,
  CRGB(93, 204, 245)    // Light Blue
);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.setCorrection(TypicalPixelString);
  Serial.begin(115200);
}

void loop() {
  //leds[0] = 0xecbfff;

  EVERY_N_MILLISECONDS(700) {
    leds[currentLed] = ColorFromPalette(stage1Palette, curSubStage * 64); // Settings this multiply to 16 gave some nice gradients
    currentLed += 4;

    if (currentLed >= NUM_LEDS) {
      curSubStage++;
      // Setting the start led to the substage will let us start with an offset
      currentLed = curSubStage;
    }
  }

  // Note stage one has 32 beats in total, 33rd is start of next

  FastLED.show();
}

