//#define FASTLED_INTERNAL //remove annoying pragma messages
#define FASTLED_RMT5_RECYCLE 1
#include <FastLED.h>

#define NUM_LEDS            16      // Works best if this number is divisible by 32
#define LED_PIN             2       // Set this to the pin the data wire for leds is connected to
#define DEFAULT_BRIGHTNESS  50      // 50 is a good balance between brightness and power usage, set between 0 (no brightness) and 100 (max brightness)
#define LED_TYPE            WS2812B // Set this to the type of FastLED supported strip you are using

CRGB leds[NUM_LEDS];

uint8_t currentLed = 0;
uint8_t curStage = 0;
uint8_t curSubStage = 0;

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

void stage1Animation(int8_t movement) {
  EVERY_N_MILLISECONDS(100) {
    // Essentially we want to increment the first occurance of each colour
    uint8_t localOffset = stage1Offset % stage0Size;
    // Handle case for updating the starting LED in forward movement
    if (localOffset == 0 && movement == 1) {
      leds[localOffset] = leds[NUM_LEDS - 1];
      localOffset += stage0Size;
    }
    // Handle case for updating final LED in backwards movement
    // do so without if unnecessary if statements in the for loop
    // yes, this is over optimisation
    uint8_t length = (movement == -1) ? (NUM_LEDS - 1) : NUM_LEDS;

    for (int i = localOffset; i < length; i += stage0Size) {
      leds[i] = leds[i - movement];
    }
    if (movement == -1) {
      leds[length] = leds[0];
    }

    stage1Offset+=movement;
  }
}


