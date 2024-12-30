// FastLED definitions
// #define FASTLED_INTERNAL     // remove annoying pragma messages
#define FASTLED_RMT5_RECYCLE 1  // https://github.com/FastLED/FastLED/issues/1768
// Includes
#include <Arduino.h>
#include <FastLED.h>

// Parameters you will likely need to adjust to run the animation
#define NUM_LEDS            96      // Works best if this number is divisible by 32
#define LED_PIN             2       // Set this to the pin the data wire for leds is connected to
#define DEFAULT_BRIGHTNESS  50      // 50 is a good balance between brightness and power usage, set between 0 (no brightness) and 100 (max brightness)
#define LED_TYPE            WS2812B // Set this to the type of FastLED supported strip you are using

// Some animation control parameters
#define STAGE_3_SPLASHSIZE     5    // Size of "splash" when changing colours on first beat drop
#define STAGE_3_PIXEL_DISTANCE 0.6  // Fraction of the LED strip that the flying pixel covers
CRGB leds[NUM_LEDS];

uint8_t curStage = 0;
uint8_t curSubStage = 0;

// TODO just change this to a float divide and ceil
const uint8_t stage0Size = (NUM_LEDS < 32) ? 1 : ceil((float)NUM_LEDS / 32);
uint8_t stage0CurrentLed = 0;
uint8_t stage1Offset = 0;

CRGB stage3Colour1 = CRGB::Yellow;
CRGB stage3Colour2 = 0xc20000;

bool isFlashing = false;
uint16_t nextFlash = 0;

unsigned long startTime;

// TODO probably just make this into an array
CRGBPalette16 stage0Palette(
  // Stage 0 and 1 Colours
  0xecbfff,   // Very Light Purple
  CRGB::Red,
  CRGB::DarkGreen,
  CRGB(26, 171, 176),    // Light Blue
  CRGB::Blue,
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
  unsigned long curTime = millis() - startTime + 40*1000;

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
  } else if (curTime < 36.7 * 1000) {
    stage1Animation(1);
  } else if (curTime < 42.8 * 1000) {
    stage1Animation(1);
    flashAnimation(curTime, 11);
  } else if (curTime < 53.1 * 1000) {
    stage3Animation(CRGB::Blue, CRGB::Orange, false);
  } else if (curTime < 53.8 * 1000) {
    stage3Animation(0x00ff4c, 0x00ff4c, true); // Bright green
  } else if (curTime < 58.7 * 1000) {
    // TODO could set variable inital value to these and then use same code line with larger first interval in part 2
    stage3Animation(CRGB::Yellow, 0xc20000, false); // and Dark red
  } else if (curTime < 64.1 * 1000) {
    stage3Animation(stage3Colour1, stage3Colour2, false);
    stage3AnimationP2(stage3Colour1, stage3Colour2);
  } else if (curTime < 74.8 * 1000) {
    stage3AnimationP3(curTime, CRGB::Magenta, CRGB::LightSeaGreen);
  } else if (curTime < 76.1 * 1000) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  } else if (curTime < 100 * 1000) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  }
  
  // 58.7/8 first beat, 60.1 second, 61.4 third (very accurate last 2), 62.8 (though .7 may be ok as well)
  // Other sound at 63.2, 63.75, 64.1
  // 102.7 first

  FastLED.show();
}

void stage0Animation() {
  EVERY_N_MILLISECONDS(700) {
    for (int i = stage0CurrentLed; i < stage0CurrentLed + stage0Size; i++) {
      leds[i] = ColorFromPalette(stage0Palette, curSubStage * 16, NOBLEND); // use multiply 16 with only stage 1 colours for nice gradients
    }

    //leds[stage0CurrentLed] = ColorFromPalette(stage0Palette, curSubStage * 16);
    stage0CurrentLed += 4 * stage0Size;

    if (stage0CurrentLed >= NUM_LEDS) {
      curSubStage++;
      // Setting the start led to the substage will let us start with an offset
      stage0CurrentLed = curSubStage * stage0Size;
      if (curSubStage >= 4) {
        curSubStage = 0;
      }
    }
  }
}

void stage1Animation(int8_t movement) {
  if (isFlashing) return;

  EVERY_N_MILLISECONDS(100) {
    // I tried to be more efficient but failed due to the multiple possible configs
    // in theory you can just update the first led in each colour splash
    for (int i = 0; i < NUM_LEDS; i++) {
      // TODO possibly use a scale fast math function here and elsewhere
      leds[i] = ColorFromPalette(stage0Palette, (((NUM_LEDS + stage1Offset + i) % NUM_LEDS) / stage0Size) % 4 * 16);
    }
    stage1Offset+=movement;
  }
}

uint32_t getFlashInterval() {
  if (isFlashing) {
    return 20;
  }
  return nextFlash;
}

void flashAnimation(unsigned long &curTime, uint8_t ratio) {
  EVERY_N_MILLISECONDS_DYNAMIC(getFlashInterval()) {
      if (isFlashing) {
        isFlashing = false;
        nextFlash = (42.8 * 1000 - curTime) / ratio;
      } else {
        fill_solid(leds, NUM_LEDS, CRGB::Blue); 
        isFlashing = true;
      }
    }
}

// Adapted from https://github.com/s-marley/FastLED-basics/blob/main/5.%20Multiple%20patterns/functionsTimer/functionsTimer.ino
const uint16_t sinMiddle1 = (NUM_LEDS) * STAGE_3_PIXEL_DISTANCE - 1;
const uint16_t sinMiddle2 = (NUM_LEDS) * (1 - STAGE_3_PIXEL_DISTANCE) - 1;
void stage3Animation(CRGB colour1, CRGB colour2, bool splash) {
  uint16_t sinBeat   = beatsin16(15, 0, sinMiddle1, 0, 0);
  uint16_t sinBeat2  = beatsin16(15, sinMiddle2, NUM_LEDS - 1, 0, 32767); // Phase of half 65535 (max uint16_t)

  if (splash) {
    for (uint8_t i = 0; i < STAGE_3_SPLASHSIZE; i++) {
      leds[sinBeat - (STAGE_3_SPLASHSIZE/2) + i] = colour1;
      leds[sinBeat2 - (STAGE_3_SPLASHSIZE/2) + i] = colour2;
    }
  } else {
    leds[sinBeat]   = colour1;
    leds[sinBeat2]  = colour2;
  }
  
  fadeToBlackBy(leds, NUM_LEDS, 10);
}

const uint16_t stage3Intervals[] = {10, 1.3 * 1000, 1.3 * 1000, 1.3 * 1000, 0.35 * 1000, 0.35 * 1000, 999 * 1000};
void stage3AnimationP2(CRGB &colour1, CRGB &colour2) {
  EVERY_N_MILLISECONDS_DYNAMIC(stage3Intervals[curSubStage]) {
    colour1 = CHSV(random8(), random8(), random8());
    colour2 = CHSV(random8(), random8(), random8());
    curSubStage++;
  }
}

void stage3AnimationP3(unsigned long &curTime, CRGB colour1, CRGB colour2) {
  // Tried to use a curve here for a better effect but failed due to needing to also have 
  // dynamic jumps in how much the counter value increased by as well as the end value of 
  // the curve which I had computed using a scale function
  static uint8_t counter = 1;
  static uint8_t counterChange = 1;
  static uint8_t limit = NUM_LEDS * 0.6;
  static uint8_t fadeLevel = 90;
  EVERY_N_MILLISECONDS_I(climaxTimer, 30) {
    counter+=counterChange;
    if (counter == limit) {
      counterChange = -1;
    } else if (counter == 0) {
      counterChange = 1;
      limit = limit/1.5;
      if (limit < 0.17 * NUM_LEDS) {
        limit = 0.17 * NUM_LEDS;
        // climaxTimer.setPeriod(40);
        // fadeLevel = 200;
        fadeLevel = 240;
      }
    }

    leds[(int)(NUM_LEDS * (1 - STAGE_3_PIXEL_DISTANCE)) + counter] = colour1;
    leds[(int)(NUM_LEDS * STAGE_3_PIXEL_DISTANCE) - counter] = colour2;
    fadeToBlackBy(leds, NUM_LEDS, fadeLevel);
  }
}

/*
// Keeping this because I liked the look of the animation but it didn't work with the music
void stage3AnimationP3(CRGB colour1, CRGB colour2) {
  // scaling is one thing and we can do it with the time remaining
  // The other factor is using the ease function to calculate position using current progress (0-255) and then scaling that to a position in NUM_LEDS
  // When progress is done we need to start going negative progress down back to 0
  static uint8_t counter = 0;
  EVERY_N_MILLISECONDS(20) {
    counter++;
    uint8_t location = scale8(quadwave8(counter), (NUM_LEDS * STAGE_3_PIXEL_DISTANCE));
    leds[(int)(NUM_LEDS * (1 - STAGE_3_PIXEL_DISTANCE)) + location] = colour1;
    leds[(int)(NUM_LEDS * STAGE_3_PIXEL_DISTANCE) - location] = colour2;
    fadeToBlackBy(leds, NUM_LEDS, 10);
  }
}
*/