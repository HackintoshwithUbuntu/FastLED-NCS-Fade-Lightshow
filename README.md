# FastLed Lightshow for NCS Alan Walker Fade
A (mostly) efficient rendition of a light show that compliments the first 2 minutes of music from the NCS version of the Alan Walker Fade soundtrack (the one you here in every second YouTube compilation).

## See it in action
TODO add images / videos here

## Running the show
The code is based on the Arduino framework using the FastLED library. It can probably be easily ported to PlatformIO, the following instructions are relevant to those using either the Arduino IDE or CLI.
1. Ensure the Arduino framework is setup on your computer (and libraries for any boards you are using are setup through the board manager)
2. Install the FastLED library using the sidebar
3. Open the file `FastLed_AlanWalkerFade.ino` inside the `FastLed_AlanWalkerFade` folder
4. Adjust the following parameters that are at the top of the file so they match up with your setup
```cpp
#define NUM_LEDS            96      // Works best if this number is divisible by 32, though other numbers work fine
#define LED_PIN             2       // Set this to the pin the data wire for leds is connected to
#define DEFAULT_BRIGHTNESS  50      // 50 is a good balance between brightness and power usage, set between 0 (no brightness) and 100 (max brightness)
#define LED_TYPE            WS2812B // Set this to the type of FastLED supported strip you are using, see https://github.com/FastLED/FastLED/wiki/Overview
```
5. (Optional) Customise any other parts of the code you wish. Some of the #defines at the top provide an easy entry into changing the animation behaviour.
6. Run the code
    * Ensure your setup can provide enough power for the LEDs
    * Once the code is uploaded the easiest way to align the start of the program with the music is to use two hands: one to press reset button and the other to press play button for the music

### Hardware setup and power consumption
I used an Esp32-C3 Supermini board for this project. To supply power while debugging I just used my computer's USB port and only applied the animation to between 32 and 96 LEDs. 

(TODO Insert picture)

For 96 LEDs I found the max power consumption to be around 0.45 Amps at 5 volts with my WS2812B ECO leds strips. Your mileage may vary but note that real world power consumption is often much lower than the maximum figures quoted online. 
Ensure your power supply is capable of powering whatever load you give it. I found [QuinLED's guide](https://quinled.info/2020/03/12/digital-led-power-usage/) a good place to start for real world power consumption figures.

(TODO picture of power consumption)

For powering more LEDs (and at greater brightness levels), I used an old 5V 3A USB-C power supply by Labists that came with my Raspberry Pi but was later replaced by the official power supply since it seemed not to handle the load of my SSD well and caused random shutdowns
(after replacing the power supply and fixing [SSD enclosure UASP issues,](https://forums.raspberrypi.com/viewtopic.php?t=245931&start=225) the shutdowns have since want away).

(TODO picture of power supply)

## How I did it
Most of my work consisted of two things. Using this [online editor](https://audiomass.co/) to help visualise waveforms to find patterns to optimise timings and identify beat changes. The second part was just trial and error. 

## Using more than 256 LEDs
The code has been designed to work with any number of LEDs. However, I don't have the hardware to test with more than 256 LEDs. There may be some minor adjustments to the code required (such as changing a `uint8_t` to `uint16_t`) for it to run perfectly with that many LEDs (though it's possible no changes are required).

## Other notes
There are some other #defines at the top of the code that you may want to adjust. The two lines at the top were required to reduce some FastLED verbose output on my ESP32-C3 Supermini. 

If you are interested in making edits or debugging part of the animation. One way to make your life easier is to edit the following line to add some amount of time to skip to a certain point in the animation eg. add `+ 60 * 1000` to start from the 60 second mark. Some animation that rely on time elapased since the stage started won't work perfectly like this and you will need to instead forward to just before that start of that "stage".
```cpp
unsigned long curTime = millis() - startTime; // Add n * 1000 here to start animation at a different point
```

There are 2 other folders in this repo besides the main show and track, they were some examples I used to start experimenting with my ESP32 and to ensure everything was working. They are not required to run the show but have been kept in case you want to conduct some tests, especially for the infamous [C3 Supermini Wifi bug](https://roryhay.es/blog/esp32-c3-super-mini-flaw).

I have tried to keep the code mostly performant. Though some places were just too hard and took too much time and could use improvements (eg. when the colours are spinning near the start, you could just edit the last/first pixel in each colour "splash" though with the number of different sizing combinations possible I found it too difficult and modern microcontrollers are easily able to cope with the load of the current implementation).
