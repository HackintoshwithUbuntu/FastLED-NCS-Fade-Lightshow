//#define FASTLED_INTERNAL      // remove annoying pragma messages
#define FASTLED_RMT5_RECYCLE 1  // https://github.com/FastLED/FastLED/issues/1768
#include <FastLED.h>

#define NUM_LEDS            96      // Works best if this number is divisible by 32 (min 32 leds)
#define LED_PIN             2       // Set this to the pin the data wire for leds is connected to
#define DEFAULT_BRIGHTNESS  50      // 50 is a good balance between brightness and power usage, set between 0 (no brightness) and 100 (max brightness)
#define LED_TYPE            WS2812B // Set this to the type of FastLED supported strip you are using

CRGB leds[NUM_LEDS];

uint8_t currentLed = 0;
uint8_t curStage = 0;
uint8_t curSubStage = 0;

// TODO just change this to a float divide and ceil
const uint8_t stage0Size = (NUM_LEDS < 32) ? 1 : ceil((float)NUM_LEDS / 32);
uint8_t stage1Offset = 0;

unsigned long startTime;

CRGBPalette16 stage0Palette(
  // Stage 0 and 1 Colours
  0xecbfff,   // Very Light Purple
  CRGB::Red,
  CRGB::DarkGreen,
  CRGB(26, 171, 176),    // Light Blue
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000,
  0x100000
);

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  FastLED.setCorrection(TypicalPixelString);
  Serial.begin(57600);
}

void loop() {
  //leds[0] = 0xecbfff;

  // TODO probablly change this to case switch for future use
  unsigned long curTime = millis() - startTime;
  if (curTime < 21.3 * 1000) {
    stage0Animation();
  } else if (curTime < 24.3 * 1000) {
    stage1Animation(1);
  } else if (curTime < 26.8 * 1000) {
    stage1Animation(-1);
  } else if (curTime < 29.7 * 1000) {
    stage1Animation(1);
  } else if (curTime < 32.3 * 1000) {
    stage1Animation(-1);
  } else if (curTime < 42.8 * 1000) {
    stage1Animation(1);
  }
  // 23.3/23.5 (or could use 24.4-24.2), around 26.8, 29.678 (could switch colours), 32.3 + flash, 42.8 is the big change
  // 21.3/21.4
  // Note stage one has 32 beats in total, 33rd is start of next

  FastLED.show();
}

void stage0Animation() {
  EVERY_N_MILLISECONDS(700) {
    for (int i = currentLed; i < currentLed + stage0Size; i++) {
      leds[i] = ColorFromPalette(stage0Palette, curSubStage * 16); // use multiply 16 with only stage 1 colours for nice gradients
    }

    //leds[currentLed] = ColorFromPalette(stage0Palette, curSubStage * 16);
    currentLed += 4 * stage0Size;

    if (currentLed >= NUM_LEDS) {
      curSubStage++;
      // Setting the start led to the substage will let us start with an offset
      currentLed = curSubStage * stage0Size;
      if (curSubStage >= 4) {
        curSubStage = 0;
      }
    }
  }
}
int test = 0;
void stage1Animation(int8_t movement) {
  EVERY_N_MILLISECONDS(100) {
    // I tried to be more efficient but failed due to the multiple possible configs
    // in theory you can just update the first led in each colour splash
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = ColorFromPalette(stage0Palette, (((NUM_LEDS + stage1Offset + i) % NUM_LEDS) / stage0Size) % 4 * 16);
    }
    test++;
    stage1Offset+=movement;
  }
}


