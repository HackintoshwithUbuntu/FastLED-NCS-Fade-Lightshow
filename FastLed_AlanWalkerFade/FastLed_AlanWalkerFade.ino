#include <FastLED.h>

#define NUM_LEDS            18      // Works best if this number is divisible by 32
#define LED_PIN             2       // Set this to the pin the data wire for leds is connected to
#define DEFAULT_BRIGHTNESS  50      // 50 is a good balance between brightness and power usage, set between 0 (no brightness) and 100 (max brightness)
#define LED_TYPE            WS2812B // Set this to the type of FastLED supported strip you are using

CRGB leds[NUM_LEDS];

uint8_t currentLed = 0;
uint8_t curStage = 0;
uint8_t curSubStage = 0;
uint8_t paletteIndex = 0;

unsigned long startTime;

CRGBPalette16 stage0Palette(
  // Stage 1 Colours
  0xecbfff,   // Very Light Purple
  CRGB::Red,
  CRGB::Green,
  CRGB(93, 204, 245)    // Light Blue
);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  FastLED.setCorrection(TypicalPixelString);
  Serial.begin(115200);
}

void loop() {
  //leds[0] = 0xecbfff;

  // TODO probablly change this to case switch for future use
  if (millis() - startTime < 21.3 * 1000) {
    stage0Animation();
  } else {
    if (curStage == 0) {
      curStage = 1;
      curSubStage = 0;
    }
    stage1Animation();

  }
  // Note stage one has 32 beats in total, 33rd is start of next

  FastLED.show();
}

void stage0Animation() {
  EVERY_N_MILLISECONDS(700) {
    leds[currentLed] = ColorFromPalette(stage0Palette, curSubStage * 64); // use multiply 16 with only stage 1 colours for nice gradients
    currentLed += 4;

    if (currentLed >= NUM_LEDS) {
      curSubStage++;
      // Setting the start led to the substage will let us start with an offset
      currentLed = curSubStage;
    }
  }
}

void stage1Animation() {
  EVERY_N_MILLISECONDS(150) {
    fill_palette(leds, NUM_LEDS, paletteIndex, 255/ NUM_LEDS, stage0Palette, 255, NOBLEND);
    paletteIndex++;
  }
}


