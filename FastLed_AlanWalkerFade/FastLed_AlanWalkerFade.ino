#include <FastLED.h>

#define NUM_LEDS            32      // Works best if this number is divisible by 32
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
  CRGB(109, 248, 252),    // Light Blue
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
  // Serial.println(stage0Size); // TODO remove
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
    for (int i = currentLed; i < currentLed + stage0Size; i++) {
      leds[i] = ColorFromPalette(stage0Palette, curSubStage * 16); // use multiply 16 with only stage 1 colours for nice gradients
    }
    Serial.println(100);
    //leds[currentLed] = ColorFromPalette(stage0Palette, curSubStage * 16);
    currentLed += 4 * stage0Size;

    if (currentLed >= NUM_LEDS) {
      curSubStage++;
      // Setting the start led to the substage will let us start with an offset
      currentLed = curSubStage;
      if (curSubStage >= 4) {
        curSubStage = 0;
      }
    }
  }
}

void stage1Animation() {
  EVERY_N_MILLISECONDS(150) {
    // Essentially we want to increment the first occurance of each colour
    uint8_t localOffset = stage1Offset % stage0Size;

    Serial.print(50);
    Serial.print(',');
    for (int i = localOffset; i < NUM_LEDS; i += stage0Size) {
      Serial.println((stage1Offset + i) % 4);
      leds[i] = ColorFromPalette(stage0Palette, ((stage1Offset + i) % 4) * 16);
      localOffset++;
    }
    stage1Offset++;
  }
}


