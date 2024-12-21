#include <FastLED.h>

#define NUM_LEDS  18
#define LED_PIN   2
#define LED_TYPE  WS2812B

CRGB leds[NUM_LEDS];

uint8_t currentled = 0;
uint8_t curStage = 0;

DEFINE_GRADIENT_PALETTE( stage1_gp ) { 
  // Stage 1
  0,    253,  247,  255,    // light purple
  100,    255,    0,  0,      // red
  200,   55,  222,   70,      // bright green
  50,    0,  194,  255       // light blue
};

CRGBPalette16 stage1Palette(stage1_gp);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.setCorrection(TypicalPixelString);
  Serial.begin(115200);
}

void loop() {
  //leds[0] = 0xecbfff;

  EVERY_N_MILLISECONDS(700) {
    leds[currentled % NUM_LEDS] = ColorFromPalette(stage1Palette, curStage);
    leds[0] = 0xecbfff;
    currentled += 4;
  }

  FastLED.show();
}

