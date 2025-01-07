// FastLED definitions
// #define FASTLED_INTERNAL     // Remove annoying pragma messages
#define FASTLED_RMT5_RECYCLE 1  // https://github.com/FastLED/FastLED/issues/1768 Commenting this line should improve perfromance but makes Serial unusable
// Includes
#include <cmath> 
#include <Arduino.h>
#include <FastLED.h>

//////////////////////////////////////////////////////////////////
// Parameters you will likely need to adjust to run the animation
//////////////////////////////////////////////////////////////////
#define NUM_LEDS            32      // Works best if this number is divisible by 32, though other numbers work fine
#define LED_PIN             2       // Set this to the pin the data wire for leds is connected to
#define DEFAULT_BRIGHTNESS  50      // 50 is a good balance between brightness and power usage, set between 0 (no brightness) and 100 (max brightness)
#define LED_TYPE            WS2812B // Set this to the type of FastLED supported strip you are using, see https://github.com/FastLED/FastLED/wiki/Overview

/////////////////////////////////////
// Some animation control parameters
/////////////////////////////////////
#define STAGE_3_SPLASHSIZE     5    // Size of "splash" when changing colours on first beat drop
#define STAGE_3_PIXEL_DISTANCE 0.6  // Fraction of the LED strip that the flying pixel covers
#define STAGE_4_ANIMATION_TIME 2.65 // How long the spread from centre animation takes in seconds
#define STAGE_4_FAST_SPEED     0.55 // How fast the faster part of the animation will run with respect to its time taken if all parts were equal
#define STAGE_4_COLOR_BLEND    3    // How much to blend "wave" colour with background, between 0 (no blend) and 4 (max blend)
#define STAGE_4_FADE_SPEED     5    // Speed at which background fades, between 0 and 255

// These are the colours used in the initial "splashes"
const CRGB stage01Palette[5] = {
  0xecbfff,              // Very Light Purple
  CRGB::Red,
  CRGB::DarkGreen,
  CRGB(26, 171, 176),    // Light Blue
  CRGB::Blue,
};

CRGB stage3Colour1 = CRGB::Yellow;
CRGB stage3Colour2 = 0xc20000;  // Dark Red

const CRGB stage4BgColour = CRGB(255, 84, 84);
CRGBPalette16 stage4Palette(
  stage4BgColour,
  CRGB(166, 10, 44),     // Dark Red 
  stage4BgColour,
  CRGB(250, 177, 193),   // Very Light Red
  stage4BgColour,
  CRGB(59, 0, 13),       // Brown
  stage4BgColour,
  CRGB(227, 61, 169),     // Pinkish Red 
  stage4BgColour,
  CRGB(250, 177, 193),   // Very Light Red
  stage4BgColour,
  CRGB(59, 0, 13),       // Brown
  stage4BgColour,
  0x100000,
  0x100000,
  0x100000
);

/////////////////////////////////////////
// Parameters you shoudn't need to touch
/////////////////////////////////////////
#define SQRT_3 1.4422495
CRGB leds[NUM_LEDS];

uint8_t curStage = 0;
uint8_t curSubStage = 0;

const uint8_t stage0Size = ceil((float)NUM_LEDS / 32);

bool isFlashing = false;
uint16_t nextFlash = 0;

unsigned long startTime;

//////////////////
// Animation Code
//////////////////

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS);
  FastLED.setCorrection(TypicalPixelString);
  Serial.begin(57600);
}

void loop() {
  unsigned long curTime = millis() - startTime; // Add n * 1000 here to start animation at a different point

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
    stage3Animation(CRGB::Yellow, 0xc20000, false); // and Dark red
  } else if (curTime < 64.1 * 1000) {
    stage3Animation(stage3Colour1, stage3Colour2, false);
    stage3AnimationP2(stage3Colour1, stage3Colour2);
  } else if (curTime < 74.8 * 1000) {
    stage3AnimationP3(curTime, CRGB::Magenta, CRGB::LightSeaGreen);
  } else if (curTime < 76.1 * 1000) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  } else if (curTime < (76.1 + STAGE_4_ANIMATION_TIME) * 1000) { // 78.75
    stage4Animation();
  } else if (curTime < 96 * 1000) {
    stage4Waves();
  } else if (curTime < 97.35 * 1000) {
    fadeToBlackBy(leds, NUM_LEDS, STAGE_4_FADE_SPEED);
  } else {
    stage4Rainbow();
  }

  FastLED.show();
}

void stage0Animation() {
  static uint16_t stage0CurrentLed = 0;
  EVERY_N_MILLISECONDS(700) {
    for (int i = stage0CurrentLed; i < stage0CurrentLed + stage0Size; i++) {
      leds[i] = stage01Palette[curSubStage];
    }
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
  static uint16_t stage1Offset = 0;
  if (isFlashing) return;

  EVERY_N_MILLISECONDS(100) {
    // I tried to be more efficient but failed due to the multiple possible configs
    // in theory you can just update the first led in each colour splash
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = stage01Palette[(((NUM_LEDS + stage1Offset + i) % NUM_LEDS) / stage0Size) % 4];
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
  static uint16_t counter = 1;
  static uint16_t counterChange = 1;
  static uint16_t limit = NUM_LEDS * STAGE_3_PIXEL_DISTANCE;
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
        fadeLevel = 170;
      }
    }

    leds[(int)(NUM_LEDS * (1 - STAGE_3_PIXEL_DISTANCE)) + counter] = colour1;
    leds[(int)(NUM_LEDS * STAGE_3_PIXEL_DISTANCE) - counter] = colour2;
    fadeToBlackBy(leds, NUM_LEDS, fadeLevel);
  }
}


const uint16_t midpoint = NUM_LEDS / 2;
const float STAGE_4_SLOW_SPEED = (1 - (0.8 * (float)STAGE_4_FAST_SPEED)) / 0.2;
// I tried to get some kind of cubic function working for this but didn't give the result I wanted
void stage4Animation() {
  static uint16_t counter = 0;
  EVERY_N_MILLISECONDS_I(stage4Timer, ((float)STAGE_4_ANIMATION_TIME / (NUM_LEDS / 2) * STAGE_4_FAST_SPEED) * 1000) {
    counter++;
    if (counter == NUM_LEDS / 2) {
      stage4Timer.setPeriod(99 * 1000);
    } else if (counter == uint16_t(NUM_LEDS/2 * 0.4)) {
      stage4Timer.setPeriod((float)STAGE_4_ANIMATION_TIME / (NUM_LEDS / 2) * STAGE_4_SLOW_SPEED * 1000);
    } else if (counter == uint16_t(NUM_LEDS/2 * STAGE_4_FAST_SPEED)) {
      stage4Timer.setPeriod((float)STAGE_4_ANIMATION_TIME / (NUM_LEDS / 2) * STAGE_4_FAST_SPEED * 1000);
    }

    leds[midpoint + counter] = stage4BgColour;
    leds[midpoint - counter] = stage4BgColour;
  }
}

void stage4Waves() {
  static int16_t curLocation = -5;
  static uint8_t waveNumber = 0;
  EVERY_N_MILLISECONDS((2.7 * 1000) / (NUM_LEDS / 2 + 5)) {
    // Clear old wave
    for (int16_t i = curLocation - 1; i < min(NUM_LEDS/2, curLocation + 5); i++) {
      if (i < 0) continue;
      leds[midpoint + i] = stage4BgColour;
      leds[midpoint - i] = stage4BgColour;
    }

    // Add new wave
    for (int16_t i = curLocation; i < min(NUM_LEDS/2, curLocation + 5); i++) {
      if (i < 0) continue;
      Serial.println(((waveNumber * 2 + 6) * 16) + (i - curLocation - 3) * STAGE_4_COLOR_BLEND);
      leds[midpoint + i] = ColorFromPalette(stage4Palette, ((waveNumber * 2 + 1) * 16) + (i - curLocation - 3) * STAGE_4_COLOR_BLEND);
      leds[midpoint - i] = ColorFromPalette(stage4Palette, ((waveNumber * 2 + 1) * 16) + (i - curLocation - 3) * STAGE_4_COLOR_BLEND);
    }

    curLocation++;
    if (curLocation == NUM_LEDS/2) {
      waveNumber++;
      curLocation = -5;
    }
  }
}

void stage4Rainbow() {
  static uint8_t offset = 0;
  EVERY_N_MILLISECONDS(10) {
    fill_rainbow(leds, NUM_LEDS, offset, 8);
    offset++;
  }
}

// Other Stage 4 animation attempts using curves instead which would have been nicer
// uint16_t cubic_entry(unsigned long &curTime) {
//   const long elapsedTime = curTime - 76.1 * 1000;
//   return static_cast<uint16_t>(NUM_LEDS/(2*3))*(pow(((3/(STAGE_4_ANIMATION_TIME * 1000)) * elapsedTime - SQRT_3), 3) + 3);
// }
// void stage4Animation (unsigned long &curTime) {
//   const uint16_t offset = cubic_entry(curTime);
//   Serial.println(offset);
//   leds[midpoint + offset] = CRGB::Red;
//   leds[midpoint - offset] = CRGB::Red;
// }

// void stage4Animation() {
//   // We want a quarter of the wave to complete in the given time, we want a 
//   uint16_t progress = 0; 
//   beatsin16(15/STAGE_4_ANIMATION_TIME, 0, midpoint, 0, 0) - midpoint/2;
//   Serial.println(sinBeat);
//   leds[midpoint + sinBeat] = CRGB::Red;
//   leds[midpoint - sinBeat] = CRGB::Red;
// }
// void stage4Animation () {
//   static uint8_t progress = 0;
//   EVERY_N_MILLISECONDS(STAGE_4_ANIMATION_TIME / NUM_LEDS * 1000) {
//     progress++;
//     leds[midpoint + scale16by8(ease8InOutCubic(progress), NUM_LEDS)/2] = CRGB::Red; // For performance consider ease8InOutApprox() and ease8InOutQuad()
//     leds[midpoint - scale16by8(ease8InOutCubic(progress), NUM_LEDS)/2] = CRGB::Red;
//   }
// }
// void stage4Animation() {
//   static uint8_t progress = 0;
//   EVERY_N_MILLISECONDS(STAGE_4_ANIMATION_TIME / NUM_LEDS * 1000) {
//     progress++;
//     uint8_t transformedValue = ease8InOutCubic(progress);
//     Serial.println(transformedValue);
//     uint16_t scaledValue = scale16by8(transformedValue, NUM_LEDS) / 2;
//     leds[midpoint + scaledValue] = CRGB::Red;
//     leds[midpoint - scaledValue] = CRGB::Red;
//   }
// }

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